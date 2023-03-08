#
# copies files using a manifest
#
# the syntax is as follows:
#
# python copyManifest.py manifest_file source_directory target_directory
#
# entries in the manifest file consist of a file pattern and a directory
# we never expect to copy a file to a file of a different name
#
# we only copy a file if it is considered to be different by the filecmp
# module (which checks the stat entry) unless we have used the -o option
# (overwrite)
#
# manifest file entries consist of comments (start with #) or blank lines
# or two entries on the same line - if you want space delimited files
# then you should put the pattern inside quotes
#
# in most cases you will want to copy the file to the directory of the
# same name - in which case you can omit the second entry on the same line
#
# if you specify a directory instead of a file, then the entire contents
# of that directory will be copied (one file at a time)
#

import glob
import filecmp
import os
import sys
import posixpath
import shutil

if sys.version_info.major < 3:
    input = raw_input

def readManifest(fn):
    """
    Reads a manifest file and returns a list of 2-tuples

    The first entry is the file pattern and the second entry is the target
    directory - normally the second entry is blank in which case the
    directory of the file is used instead.
    """
    fp = open(fn)
    manifest = []
    for line in fp.readlines():
        work = line.strip()
        if work.startswith("#"): continue
        if not work: continue
        if work.startswith('"'):
            source = work.split('"')[1]
            work = work[len(source)+2:].strip()
        else:
            source = work.split()[0]
            work = work[len(source):].strip()
        if not work or work.startswith("#"):
            target = None
        elif work.startswith('"'):
            target = work.split('"')[1]
            work = work[len(target)+2:].strip()
        else:
            target = work.split()[0]
            work = work[len(target):].strip()
        if not work.startswith("#") and work:
            raise Exception("Bad format line %s" % line.strip())
        manifest.append((source,target))
    return manifest

def copyDirectory(dn, odn, overwrite):
    filesCopied = 0
    odn = posixpath.join(odn, posixpath.basename(dn))
    for sdn,dns,fns in os.walk(dn):
        assert(sdn.startswith(dn))
        tdn = odn + sdn[len(dn):]
        for fn in fns:
            ffn = posixpath.join(sdn, fn)
            filesCopied += copyFile(ffn, tdn, overwrite)
    return filesCopied

def copyFile(fn, odn, overwrite):
    if not posixpath.isdir(odn):
        print("creating %s" % odn)
        os.makedirs(odn)
    ofn = posixpath.join(odn, posixpath.basename(fn))
    if not posixpath.isfile(ofn) or overwrite or not filecmp.cmp(fn,ofn):
        print("copying %s" % ofn)
        shutil.copy2(fn, ofn)
        return 1
    return 0

def main(manifestFile, source, target, replacements,
         overwrite=False, clean=False):
    source = source.replace("\\", "/")
    target = target.replace("\\", "/")
    if source == target:
        raise Exception("source (%s) = target" % source)
    manifest = readManifest(manifestFile)

    rdict = {}
    for r in replacements:
        parts = r.split("=")
        if len(parts) != 2: continue
        rdict[parts[0]] = parts[1]

    if clean and posixpath.isdir(target):
        print("deleting %s" % target)
        shutil.rmtree(target)

    filesCopied = 0
    for pattern, dn in manifest:
        if rdict:
            bfn = posixpath.basename(pattern)
            rfn = bfn % rdict
            if bfn != rfn:
                pattern = posixpath.join(posixpath.dirname(pattern), rfn)
        sourcePattern = posixpath.join(source, pattern)
        fns = glob.glob(sourcePattern)
        if not fns:
            raise Exception("Could not find %s in %s" % (pattern, source))
        for fn in fns:
            fn = fn.replace("\\", "/")
            if dn is None:
                odn = posixpath.dirname(fn).replace(source,target,1)
            else: odn = posixpath.join(target, dn)
            if posixpath.isdir(fn):
                filesCopied += copyDirectory(fn, odn, overwrite)
            else: filesCopied += copyFile(fn, odn, overwrite)
    print("%d files copied" % filesCopied)

if __name__ == "__main__":
    import getopt

    kwargs = {}

    opts,args = getopt.getopt(sys.argv[1:], "oc")

    if len(args) == 0:
        input("testing *.man files in current directory:")
        import glob
        for fn in glob.glob("*.man"):
            print(fn)
            input("enter to continue:")
            manifest = readManifest(fn)
            for item in manifest: print(item)
        raise SystemExit(42)

    if len(args) < 3:
        raise Exception("Expecting 'manifest source target' on command line")

    replacements = args[3:]

    for opt in opts:
        if opt[0] == "-o": kwargs["overwrite"] = True
        elif opt[0] == "-c": kwargs["clean"] = True
        else: raise Exception("Unknown option %s" % opt[0])

    main(args[0], args[1], args[2], replacements, **kwargs)



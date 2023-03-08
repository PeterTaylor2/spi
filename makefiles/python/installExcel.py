#
# script to install a list of files into a directory (with backup)
# and optionally update a file which contains the list of add-ins to open
#

import os
import sys
import filecmp
import shutil

# this code is windows specific
import getpass
import subprocess

if sys.version_info.major < 3:
    input = raw_input

def win32_set_permission(ofn, username = None):
    if username is None:
        username = getpass.getuser()
    args = ["icacls"]
    args.append(os.path.normpath(ofn))
    args.append("/grant")
    args.append("%s:F" % username)
    if os.path.isdir(ofn): args.append("/T")

    FNULL = open(os.devnull, "w")
    result = subprocess.call(args, stdout=FNULL)
    FNULL.close()

    if result != 0:
        raise Exception("Failed to set permissions for %s" % ofn)

def installFile(src, dst):
    if not os.path.isfile(src):
        raise Exception("Source file %s is not a file" % os.path.normpath(src))
    if not os.path.isfile(dst):
        install = True
    else: install = not filecmp.cmp(src, dst)

    if install:
        print("installing %s" % dst)
        shutil.copy2(src, dst)
        if sys.platform == "win32":
            win32_set_permission(dst)

    return install

def updateXlas(xlas, xla):
    # we add the XLA file to the XLAS file
    # if there is a line for the same filename but different directory we replace it
    if not(os.path.isfile(xlas)):
        contents = ["%s\n" % xla]
        print("creating %s" % xlas)
        ofp = open(xlas, "w")
        ofp.writelines(contents)
        ofp.close()
        return True

    print("reading %s" % xlas)
    fp = open(xlas, "r")
    contents = fp.readlines()
    fp.close()

    xlaBasename = os.path.basename(xla)
    updated = False
    newContents = []
    done = set()
    for line in contents:
        item = line.strip()
        if not item or item.startswith("#"):
            newContents.append(line)
            continue

        bfn = os.path.basename(item)
        if bfn in done:
            raise Exception("Duplicate XLA (%s) in %s" % (bfn, xlas))
        done.add(bfn)

        if xlaBasename == bfn:
            newLine = "%s\n" % xla
            if newLine != line:
                newContents.append(newLine)
                updated = True
            else:
                newContents.append(line)
        else:
            newContents.append(line)

    if xlaBasename not in done:
        newContents.append("%s\n" % xla)
        updated = True

    if updated:
        print("updating %s" % xlas)
        ofp = open(xlas, "w")
        ofp.writelines(newContents)
        ofp.close()
        if sys.platform == "win32":
            win32_set_permission(xlas)
        return True

    return False

def checkInputs(ifns):
    errors = []
    for ifn in ifns:
        if not os.path.isfile(ifn):
            errors.append(ifn)

    if len(errors):
        raise Exception("Could not find: %s" % (", ".join(errors)))

def main(odn, xla, fns, backup=False, clean=False, xlas=None):

    if backup:
        raise Exception("--backup option not supported")

    ifns = [xla]
    ifns.extend(fns)
    checkInputs(ifns)

    if not os.path.isdir(odn):
        print("creating %s" % odn)
        os.makedirs(odn)
        backup = False
        clean = False
    if not os.path.isdir(odn):
        raise Exception("Could not create %s directory" % odn)

    bfns = set([os.path.basename(ifn) for ifn in ifns])

    if clean:
        for fn in os.listdir(odn):
            ffn = os.path.join(odn, fn)
            if fn not in bfns:
                if os.path.isdir(ffn):
                    print("removing directory %s" % ffn)
                    shutil.rmtree(ffn, ignore_errors=True)
                else:
                    print("removing %s" % ffn)
                    os.remove(ffn)

    count = 0
    for ifn in ifns:
        ofn = os.path.join(odn, os.path.basename(ifn))
        if installFile(ifn, ofn):
            count += 1

    if xlas:
        xlaInstalled = os.path.abspath(os.path.join(odn, os.path.basename(xla)))
        if updateXlas(xlas, xlaInstalled):
            count += 1

    if count == 0:
        print("No files updated")
    else:
        print("%d file%s updated" % (count, "s" if count > 1 else ""))

if __name__ == "__main__":
    import sys
    import getopt

    try:
        opts,args = getopt.getopt(sys.argv[1:], "bcw", 
                                  ["backup", "clean", "wait", "xlas="])
        if len(args) < 3:
            raise Exception("Expecting 2+ arguments: outputDirectory xla files")

        kwargs = {}
        for opt in opts:
            if opt[0] == "-b" or opt[0] == "--backup":
                kwargs["backup"] = True
            elif opt[0] == "-c" or opt[0] == "--clean":
                kwargs["clean"] = True
            elif opt[0] == "--xlas":
                kwargs["xlas"] = os.path.normpath(opt[1])
            elif opt[0] == "-w" or opt[0] == "--wait":
                input("continue:")
            else:
                raise Exception("Unknown option %s=%s" % (opt[0], opt[1]))
            
        odn = args[0]
        xla = args[1]
        fns = args[2:]

        main(odn, xla, fns, **kwargs)
    except Exception as e:
        sys.stderr.write("ERROR: %s\n" % str(e))
        raise SystemExit(-1)


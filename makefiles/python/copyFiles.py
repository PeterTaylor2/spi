#
# script to copy a list of files into a directory 
#
# files only get copied if they are either new or have changed
#

import os
import sys
import filecmp
import shutil
import glob

# this code is windows specific
import getpass
import subprocess

if sys.version_info.major < 3:
    input = raw_input

# do we really need to do this?
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

def copyFile(src, dst):
    if not os.path.isfile(src):
        raise Exception("Source file %s is not a file" % os.path.normpath(src))
    if not os.path.isfile(dst):
        copySource = True
        ddn = os.path.dirname(dst)
        if ddn and not os.path.isdir(ddn):
            print("creating directory %s" % ddn)
            os.makedirs(ddn)
    else: copySource = not filecmp.cmp(src, dst)

    if copySource:
        print("copying %s" % dst)
        shutil.copy2(src, dst)
        if sys.platform == "win32":
            win32_set_permission(dst)

    return copySource

def main(odn, ffns, clean=False):

    count = 0

    for pattern in ffns:
        for ffn in glob.glob(pattern):
            fn = os.path.basename(ffn)
            src = ffn
            dst = os.path.join(odn, fn)
            count += copyFile(src, dst)

    if clean:
        bfns = set([os.path.basename(ffn) for ffn in ffns])
        for fn in os.listdir(odn):
            ffn = os.path.join(odn, fn)
            if fn not in bfns:
                if os.path.isdir(ffn):
                    print("removing directory %s" % ffn)
                    shutil.rmtree(ffn, ignore_errors=True)
                else:
                    print("removing %s" % ffn)
                    os.remove(ffn)

    if count > 0:
        print("%d file%s copied to %s" % (count, "s" if count > 1 else "", odn))

if __name__ == "__main__":
    import sys
    import getopt

    try:
        opts,args = getopt.getopt(sys.argv[1:], "cw", 
                                  ["clean", "wait"])
        if len(args) < 1:
            raise Exception("Expecting 1+ arguments: outputDirectory files")

        kwargs = {}
        for opt in opts:
            if opt[0] == "-c" or opt[0] == "--clean":
                kwargs["clean"] = True
            elif opt[0] == "-w" or opt[0] == "--wait":
                input("continue:")
            else:
                raise Exception("Unknown option %s=%s" % (opt[0], opt[1]))
            
        odn = args[0]
        ffns = args[1:]

        main(odn, ffns, **kwargs)
    except Exception as e:
        sys.stderr.write("ERROR: %s\n" % str(e))
        raise SystemExit(-1)


###########################################################################
# For a list of directories on the command line this script will copy
# an existing vcxproj file to a new vcxproj file with a different name
# corresponding to a different version of the compiler.
#
# The idea is that the solution refers to the GUIDs so by copying the
# vcxproj we preserve the GUIDs so when we edit the top level solution
# we will only have to change the vcxproj file names rather than the GUIDs
# and thus we will preserve the dependency structure.
#
# We will have the option of providing source control so that we copy via
# source control, e.g. svn cp or hg cp. We can provide both - svn cp will
# occur first followed by hg cp --after which allows you to record the fact
# of the copy after the event.
#
# The command line will be like this:
#
# copyVcxproj.py [--svn] [--hg] v1 v2 [directory names]
#
# We allow v1 to equal None.
# v2 must be a number.
# v1 is either None or a number.
#
# The files copied will be v[v1].vcxproj and v[v1].vcxproj.filters.
# The target file will be v[v2].vcxproj and v[v2].vcxproj.filters.
#
# We need to be careful when selecting to ensure that we don't accidentally
# select vcxproj files with versions in the name in the case we specify
# vcxproj without a version number.
###########################################################################

import os
import sys
import shutil
import glob

if sys.version_info.major < 3:
    input = raw_input

def _copy_file(src, dst, svn, hg):
    if not os.path.isfile(src):
        raise Exception("Source file %s is not a file" % os.path.normpath(src))

    if os.path.isfile(dst):
        raise Exception("Destination file %s is a file" % os.path.normpath(dst))

    if svn:
        cmd = "svn cp %s %s" % (src, dst)
        print(cmd)
        os.system(cmd)

    if hg:
        if svn:
            cmd = "hg cp --after %s %s" % (src, dst)
        else:
            cmd = "hg cp %s %s" % (src, dst)
        print(cmd)
        os.system(cmd)

    if not svn and not hg:
        print("copying %s to %s" % (src, dst))
        shutil.copy2(src, dst)

def _copy_dir(dn, v1, v2, svn, hg):
    extensions = [".vcxproj", ".vcxproj.filters"]
    if v1 is None:
        for ext in extensions:
            for src in glob.glob(os.path.join(dn, "*%s" % ext)):
                prefix = src[:-len(ext)]
                if "." in prefix:
                    last = prefix.split(".")[-1]
                    if last.startswith("v"):
                        print("skipping %s" % src)
                        continue
                dst = "%s.v%d%s" % (prefix, v2, ext)
                try:
                    _copy_file(src, dst, svn, hg)
                except Exception as e:
                    print("WARNING: %s" % str(e))
    else:
        for ext in extensions:
            extv1 = ".v%d%s" % (v1, ext)
            for src in glob.glob(os.path.join(dn, "*%s" % extv1)):
                prefix = src[:-len(extv1)]
                dst = "%s.v%d%s" % (prefix, v2, ext)
                try:
                    _copy_file(src, dst, svn, hg)
                except Exception as e:
                    print("WARNING: %s" % str(e))

def main():
    import getopt

    opts, args = getopt.getopt(sys.argv[1:], "w", ["svn", "hg"])
    svn = False
    hg = False
    for opt in opts:
        if opt[0] == "--svn": svn = True
        elif opt[0] == "--hg": hg = True
        elif opt[0] == "-w":
            input("Enter to continue:")

    if len(args) < 3:
        raise Exception("No directories provided")

    v1 = args[0]
    v2 = args[1]
    dns = args[2:]

    if v1 == "None": v1 = None
    else:
        try: v1 = int(v1)
        except:
            raise Exception("v1 '%s' must be an integer" % v1)
    try:
        v2 = int(v2)
    except:
        raise Exception("v2 '%s' must be an integer" % v2)

    for dn in dns: _copy_dir(dn, v1, v2, svn, hg)

if __name__ == "__main__":
    main()


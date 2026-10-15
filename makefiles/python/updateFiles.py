
# Script design to update the files in the target directory by copying
# from the source directory
#
# You provide the names of the source and target directory + a list of files to copy
# You can also provide the list of files copied last time and if there are any
# files in that list then you will remove those files from the target directory
#
# The lists of files are stored in one (or two) filenames.
#
# The concept is that you use your source control system to provide the list
# of files (e.g. hg files or git ls-files) - that is done external to this script
#
# Then your goal is to copy the files that are in source control (thus excluding
# all the temporary build files etc) to another directory. The assumption is that
# the target directory is not using source control directly.
#
# One use case is you are developing code on Windows and you have WSL installed
# and you want to check your files for linux compatibility by also compiling
# on WSL. It transpires that compiling on WSL using the windows file system
# is slow - so the idea is that you this script to copy the files to WSL
# native file system (which can be seen from Windows), in order to compile
# the files in the WSL native file system.
#
# You can use the same script to restore the files that have been changed
# on WSL back to the host file system.

import os
import sys
import filecmp
import shutil

def copyFile(src, dst, quiet=False, dry_run=False):

    if not os.path.isfile(src):
        raise Exception("Source file %s is not a file" % os.path.normpath(src))

    if not os.path.isfile(dst):
        copySource = True
        ddn = os.path.dirname(dst)
        if ddn and not os.path.isdir(ddn) and not dry_run:
            if not quiet:
                print("creating directory %s" % ddn)
            os.makedirs(ddn)
    else: copySource = not filecmp.cmp(src, dst)

    if copySource:
        if not quiet:
            print("copying %s" % dst)
        if not dry_run:
            shutil.copy2(src, dst)

    return copySource

def main(rootSource, rootTarget, newfn, oldfn, quiet=False, dry_run=False):

    # newfn gives you a list of files that you want to copy
    # oldfn gives you the list of files that you copied earlier
    # if there are files in oldfn which are not in newfn then these files will get deleted
    # if clean then we are fairly brutal and any file not in olfn is deleted (later)

    count = 0
    dcount = 0

    if not quiet:
        print("reading files to copy from %s" % newfn)
    newfp = open(newfn)
    newfns = set([fn.strip() for fn in newfp.readlines()])
    newfp.close()

    oldfns = []

    if oldfn is not None:
        if not quiet:
            print("reading files that were previously copied from %s" % oldfn)
        try:
            oldfp = open(oldfn)
            for fn in [fn.strip() for fn in oldfp.readlines()]:
                if fn not in newfns:
                    oldfns.append(fn)
            oldfp.close()
        except FileNotFoundError:
            if not quiet:
                print("old file %s provided but it doesn't exist - treat as empty")

    for oldfn in oldfns:
        dst = os.path.normpath(os.path.join(rootTarget, oldfn))
        if os.path.isfile(dst):
            dcount += 1
            if not quiet:
                print("deleting %s from %s" % (oldfn, rootTarget))
            if not dry_run:
                os.remove(dst)

    for newfn in sorted(newfns):
        src = os.path.normpath(os.path.join(rootSource, newfn))
        dst = os.path.normpath(os.path.join(rootTarget, newfn))
        try:
            count += copyFile(src, dst, quiet=quiet, dry_run=dry_run)
        except Exception as e:
            sys.stderr.write("WARNING: %s\n" % str(e))

    print("%d file%s copied to %s" % (count, "" if count == 1 else "s", rootTarget))
    print("%d file%s removed from %s" % (dcount, "" if dcount == 1 else "s", rootTarget))

if __name__ == "__main__":
    import sys
    import getopt

    try:
        opts,args = getopt.getopt(sys.argv[1:], "wq", 
                                  ["wait", "quiet", "dry_run"])
        if len(args) < 3 or len(args) > 4:
            raise Exception("Expecting 3 or 4 arguments: rootSource rootTarget newfn (oldfn)")

        kwargs = {}

        for opt in opts:
            if opt[0] == "-w" or opt[0] == "--wait":
                input("continue:")
            elif opt[0] == "-q" or opt[0] == "--quiet":
                kwargs["quiet"] = True
            elif opt[0] == "--dry_run":
                kwargs["dry_run"] = True
            else:
                raise Exception("Unknown option %s=%s" % (opt[0], opt[1]))

        rootSource = args[0]
        rootTarget = args[1]
        newfn = args[2]
        oldfn = args[3] if len(args) > 3 else None
        main(rootSource, rootTarget, newfn, oldfn, **kwargs)

    except Exception as e:
        sys.stderr.write("ERROR: %s\n" % str(e))
        raise SystemExit(-1)


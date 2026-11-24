# Script designed to update the files in the target directory by copying
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

class Conflict(Exception):
    pass

def copyFile(src, dst, quiet=False, dry_run=False, protect_newer=False):

    if not os.path.isfile(src):
        raise Exception("Source file %s is not a file" % os.path.normpath(src))

    if not os.path.isfile(dst):
        copySource = True
        ddn = os.path.dirname(dst)
        if ddn and not os.path.isdir(ddn) and not dry_run:
            if not quiet:
                print("creating directory %s" % ddn)
            os.makedirs(ddn)
    else:
        copySource = not filecmp.cmp(src, dst)
        # copy2 gives the copy the source's mtime, so a destination that is
        # newer than its source (2s tolerance) has been edited since the last copy
        if copySource and protect_newer and os.stat(dst).st_mtime > os.stat(src).st_mtime + 2:
            raise Conflict("%s is newer than %s - not overwriting" % (dst, src))

    if copySource:
        if not quiet:
            print("copying %s" % dst)
        if not dry_run:
            shutil.copy2(src, dst)

    return copySource

def main(rootSource, rootTarget, new_list_fn, old_list_fn, quiet=False, dry_run=False, protect_newer=False):

    # new_list_fn gives you a list of files that you want to copy
    # old_list_fn gives you the list of files that you copied earlier
    # if there are files in old_list_fn which are not in new_list_fn then these files will get deleted

    copied_count = 0
    deleted_count = 0

    if not quiet:
        print("reading files to copy from %s" % new_list_fn)
    newfp = open(new_list_fn)
    newfns = set([fn.strip() for fn in newfp.readlines()])
    newfp.close()

    oldfns = []

    if old_list_fn is not None:
        if not quiet:
            print("reading files that were previously copied from %s" % old_list_fn)
        try:
            oldfp = open(old_list_fn)
            for fn in [fn.strip() for fn in oldfp.readlines()]:
                if fn not in newfns:
                    oldfns.append(fn)
            oldfp.close()
        except FileNotFoundError:
            if not quiet:
                print("old file %s provided but it doesn't exist - treat as empty" % old_list_fn)

    conflicts = 0
    for oldfn in oldfns:
        dst = os.path.normpath(os.path.join(rootTarget, oldfn))
        if os.path.isfile(dst):
            src = os.path.normpath(os.path.join(rootSource, oldfn))
            # the file has left source control; if the target copy differs from
            # the source (when that still exists), it was edited here, so keep it
            if protect_newer and os.path.isfile(src) and not filecmp.cmp(src, dst):
                conflicts += 1
                sys.stderr.write("CONFLICT: %s was removed from the file list but has local changes - not deleting\n" % dst)
                continue
            deleted_count += 1
            if not quiet:
                print("deleting %s from %s" % (oldfn, rootTarget))
            if not dry_run:
                os.remove(dst)

    for newfn in sorted(newfns):
        src = os.path.normpath(os.path.join(rootSource, newfn))
        dst = os.path.normpath(os.path.join(rootTarget, newfn))
        try:
            copied_count += copyFile(src, dst, quiet=quiet, dry_run=dry_run,
                              protect_newer=protect_newer)
        except Conflict as e:
            conflicts += 1
            sys.stderr.write("CONFLICT: %s\n" % str(e))
        except Exception as e:
            sys.stderr.write("WARNING: %s\n" % str(e))

    copied = "would be copied" if dry_run else "copied"
    removed = "would be removed" if dry_run else "removed"
    print("%d file%s %s to %s" % (copied_count, "" if copied_count == 1 else "s", copied, rootTarget))
    print("%d file%s %s from %s" % (deleted_count, "" if deleted_count == 1 else "s", removed, rootTarget))
    if conflicts:
        print("%d file%s skipped because the target has local changes "
              "(omit --protect_newer to overwrite)"
              % (conflicts, "" if conflicts == 1 else "s"))

    return conflicts

if __name__ == "__main__":
    import getopt

    try:
        opts,args = getopt.getopt(sys.argv[1:], "wq", 
                                  ["wait", "quiet", "dry_run", "protect_newer"])
        if len(args) < 3 or len(args) > 4:
            raise Exception("Expecting 3 or 4 arguments: rootSource rootTarget new_list_fn (old_list_fn)")

        kwargs = {}

        for opt in opts:
            if opt[0] == "-w" or opt[0] == "--wait":
                input("continue:")
            elif opt[0] == "-q" or opt[0] == "--quiet":
                kwargs["quiet"] = True
            elif opt[0] == "--dry_run":
                kwargs["dry_run"] = True
            elif opt[0] == "--protect_newer":
                kwargs["protect_newer"] = True
            else:
                raise Exception("Unknown option %s=%s" % (opt[0], opt[1]))

        rootSource = args[0]
        rootTarget = args[1]
        new_list_fn = args[2]
        old_list_fn = args[3] if len(args) > 3 else None
        raise SystemExit(1 if main(rootSource, rootTarget, new_list_fn, old_list_fn, **kwargs) else 0)

    except Exception as e:
        sys.stderr.write("ERROR: %s\n" % str(e))
        raise SystemExit(-1)


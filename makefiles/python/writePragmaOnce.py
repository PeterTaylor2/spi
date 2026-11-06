"""
Finds all the .h and .hpp files in the directory tree and converts any #ifndef stuff to #pragma once

Assumes a certain structure to the header files so not a completely resilient script
"""

import os
import sys
import getopt
import glob

def find_files():
    fns = glob.glob("**/*.h", recursive=True)
    fns.extend(glob.glob("**/*.hpp", recursive=True))

    return sorted(fns)

def update_file(fn, dry_run):
    ifp = open(fn, "r")
    lines = ifp.readlines()
    ifp.close()

    has_pragma_once = False
    first_ifndef = 0
    first_define = 0
    last_endif = 0
    guard = None
    ln = 0
    for line in lines:
        ln += 1
        sline = line.strip()
        if sline.startswith("#pragma once"):
            has_pragma_once = True
            break

        if first_ifndef == 0 and sline.startswith("#ifndef"):
            guard = sline.split()[1]
            first_ifndef = ln
            continue

        if first_define == 0 and first_ifndef > 0 and sline.startswith("#define"):
            try_guard = sline.split()[1]
            if try_guard == guard:
                first_define = ln
                continue
            sys.stderr.write("WARNING: %s: %03d:#ifndef %s does not match %03d:#define %s\n" %
                             (fn, first_ifndef, guard, ln, try_guard))
            return False
        
        if first_ifndef > 0 and first_define > 0 and sline.startswith("#endif"):
            last_endif = ln
            continue

    if has_pragma_once:
        return False

    if first_ifndef == 0 or first_define == 0 or last_endif == 0:
        sys.stderr.write("WARNING: %s - could not find guard and matching endif\n" % fn)
        return False

    newLines = []
    ln = 0
    print("Editing %s" % fn)
    for line in lines:
        ln += 1
        if ln == first_ifndef:
            newLine = "#pragma once\n"
            sys.stdout.write("+++ %s" % newLine)
            sys.stdout.write("--- %s" % line)
            newLines.append("#pragma once\n")
        elif ln == first_define or ln == last_endif:
            sys.stdout.write("--- %s" % line)
        else:
            newLines.append(line)

    if not dry_run:
        print("Writing %s" % fn)
        ofp = open(fn, "w", newline="\n")
        ofp.writelines(newLines)

    return True

def pragma_once(dn, dry_run=False):
    cwd = os.getcwd()
    fdn = os.path.abspath(os.path.join(cwd, dn))
    if not os.path.isdir(fdn):
        sys.stderr.write("WARNING: %s is not a directory\n" % fdn)
        return

    try:
        print(fdn)
        os.chdir(fdn)
        fns = find_files()
        edits = 0
        count = 0
        for fn in fns:
            count += 1
            ffn = os.path.join(fdn, fn)
            edits += update_file(ffn, dry_run)
        print("Number of files:", count)
        print("Number of edits:", edits)
    finally:
        os.chdir(cwd)

def main():
    opts, args = getopt.getopt(sys.argv[1:], "n", ["dry_run"])

    if len(args) == 0:
        raise Exception("Need to provide file names on the command line")

    kwargs = {}

    for opt in opts:
        if opt[0] == "-n" or opt[0] == "--dry_run":
            kwargs["dry_run"] = True

    for dn in args:
        pragma_once(dn, **kwargs)


if __name__ == "__main__":
    main()


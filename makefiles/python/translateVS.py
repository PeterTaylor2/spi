#
# this script transforms a top-level VS solution from one version of VS to another
# the assumption is that all the GUIDs are the same
#
import os
import shutil
import sys

MAPPING = { "16" : "16.0.29001.49",
            "17" : "17.3.32929.385",
            "18" : "18.0.0.0" }

def transform_line(line, old, new):
    global MAPPING

    if line.startswith("# Visual Studio Version"):
        line = "# Visual Studio Version %s" % new
    elif line.startswith("VisualStudioVersion = "):
        line = "VisualStudioVersion = %s" % MAPPING[new]
    else:
        old_suffix = ".v%s.vcxproj" % old
        new_suffix = ".v%s.vcxproj" % new
        line = line.replace(old_suffix, new_suffix)
    return line

def transform(ifn, ofn, old, new):

    ifp = open(ifn)
    if os.path.isfile(ofn):
        bfn = ofn + ".bak"
        print("backing up output file %s to %s" % (ofn, bfn))
        shutil.copy(ofn, bfn)

    print("creating %s" % ofn)
    ofp = open(ofn, "w")

    changes = 0
    for line in ifp.readlines():
        line = line.rstrip()
        new_line = transform_line(line, old, new)
        if new_line != line:
            print("- %s" % line)
            print("+ %s" % new_line)
            print()
            changes += 1
        ofp.write("%s\n" % new_line)

    print("number of changes: %d" % changes)
    ofp.close()

def main():
    import getopt

    old = "16"
    new = "17"
    opts, args = getopt.getopt(sys.argv[1:], "", ["old=", "new="])
    if len(args) != 2:
        raise Exception("Expect input file and output file on command line")

    for opt in opts:
        if opt[0] == "--old": old = opt[1]
        elif opt[0] == "--new": new = opt[1]

    global MAPPING

    if old == new:
        raise Exception("old version and new version (%s) must be different" % old)

    if old not in MAPPING:
        raise Exception("old version (%s) not supported" % old)

    if new not in MAPPING:
        raise Exception("new version (%s) not supported" % new)

    transform(args[0], args[1], old, new)

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        sys.stderr.write("ERROR: %s\n" % str(e))



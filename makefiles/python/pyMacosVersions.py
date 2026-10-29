import glob
import os
import re
import sys
import getopt

from vsVersions import updateFile

def eligible_pythons(min_minor=9):
    """Return [(minor,path)] for python3 versions >= min_minor, sorted lower first."""

    # it seems that /usr/local/bin includes symbolic links to the Python.framework directories
    search_dirs = ("/usr/bin", "/usr/local/bin")
    name_re = re.compile(r"/python3\.(\d+)$")

    found = {}
    for dn in search_dirs:
        for path in glob.glob(os.path.join(dn, "python3.*")):
            m = name_re.search(path)
            if m and os.access(path, os.X_OK):
                minor = int(m.group(1))
                if minor >= min_minor:
                    found.setdefault(minor, path) # first directory wins: /usr/bin over /usr/local/bin
    return sorted(found.items())

def includes(vn):
    includes="-I/Library/Frameworks/Python.framework/Versions/3.%d/include/python3.%d" % (vn,vn)
    return includes
    
def main():
    if sys.platform != 'darwin':
        raise Exception("%s must be running on darwin and not %s" % (sys.argv[0], sys.platform))

    opts, args = getopt.getopt(sys.argv[1:], "M:", ["min="])

    if len(args) == 1:
        ofn = args[0]
    elif len(args) > 1:
        raise Exception("Cannot update more than one file")
    else:
        ofn = None

    kwargs = {}
    for opt in opts:
        if opt[0] == "-M" or opt[0] == "--min":
            kwargs["min_minor"] = int(opt[1])

    pythons = eligible_pythons(**kwargs)
    if not pythons:
        raise Exception("No eligible pythons found")

    contents = []
    contents.append("SARTORIAL_CONFIG=macos64")
    contents.append("")
    for versions in pythons:
        vn = versions[0]
        py = versions[1]
        contents.append("G_MACOS64_PYTHON3.%d=%s" % (vn, py))
        contents.append("G_MACOS64_PYTHON_INCLUDES3.%d=%s" % (vn, includes(vn)))
    contents.append("")
    contents.append("G_MACOS64_PYTHON3-abi=%s" % (pythons[-1][1]))
    contents.append("G_MACOS64_PYTHON_INCLUDES3-abi=%s" % includes(pythons[-1][0]))

    for line in contents:
        print(line)

    if ofn is not None:
        updateFile(ofn, "\n".join(contents))

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        sys.stderr.write("ERROR: %s\n" % str(e))
        exit(1)





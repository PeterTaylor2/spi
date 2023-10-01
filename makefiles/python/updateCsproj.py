import os
import glob

def update(fnproj, fns):
    fp = open(fnproj)
    contents = fp.read()
    fp.close()
    startGroup = "<Compile Include="
    endGroup = "</ItemGroup>"
    lines = []
    skip = False
    for line in contents.split("\n"):
        sline = line.strip()
        if skip:
            if sline.startswith(endGroup):
                skip = False
            else: continue

        if sline.startswith(startGroup):
            skip = True
            for fn in fns:
                lines.append("    <Compile Include=\"%s\" />" % os.path.basename(fn))
        else:
            lines.append(line)
    newcontents = "\n".join(lines)
    if contents != newcontents:
        print("updating", fnproj)
        fp = open(fnproj, "w")
        fp.write(newcontents)
        fp.close()

def main(fn):
    dn = os.path.dirname(fn)
    fns = glob.glob(os.path.join(dn,"*.cs"))
    update(fn, fns)

if __name__ == "__main__":
    import getopt
    import sys

    opts, args = getopt.getopt(sys.argv[1:], "")

    if len(args) != 1:
        raise Exception("Expecting CSPROJ filename parameter")

    main(args[0])




import sys
import os

def main(fns, verbose=False, test=False):
    for fn in fns:
        print(fn)
        fp = open(fn)
        lines = fp.readlines()
        fp.close()
        newlines = []
        changed = False
        for line in lines:
            newline = "%s\n" % line.rstrip()
            newlines.append(newline)
            if newline != line:
                if verbose:
                    print("- %s" % line.replace("\n", "<<EOL>>"))
                    print("+ %s" % newline.replace("\n", "<<EOL>>"))
                changed = True

        if changed:
            print(fn, "changed")
            if not test:
                fp = open(fn, "w")
                fp.writelines(newlines)
                fp.close()

if __name__ == "__main__":
    import getopt
    kwargs = {}
    try:
        opts,args = getopt.getopt(sys.argv[1:], "vt")
        for opt in opts:
            if opt[0] == "-v": kwargs["verbose"] = True
            elif opt[0] == "-t": kwargs["test"] = True
        fns = args[:]
        main(args[:], **kwargs)
    except Exception as e:
        print(e)
        raise SystemExit(1)



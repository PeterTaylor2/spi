def convert(fn, ofn):
    print("reading %s" % fn)
    fp = open(fn)
    lines = fp.readlines()
    fp.close()

    print("writing %s" % ofn)
    ofp = open(ofn, "w")
    ofp.write("EXPORTS\n")
    started = False
    for line in lines:
        parts = line.strip().split()
        if len(parts) == 4 and parts == ["ordinal", "hint", "RVA", "name"]:
            started = True
            continue
        if started and len(parts) > 4:
            ordinal = parts[0]
            hint = parts[1]
            rva = parts[2]
            name = parts[3]
            ofp.write("%s\n" % name)


if __name__ == "__main__":
    import sys
    import getopt

    opts, args = getopt.getopt(sys.argv[1:], "")
    if len(args) == 2:
        convert(*args)


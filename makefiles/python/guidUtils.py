import uuid
import os

def read_and_write_guids(fileName, names, gfn=None):
    gfn = gfn or guid_file_name(fileName)
    guids = _read_guids(gfn, fileName)
    guids = _update_guids(guids, names)
    _write_guids(gfn, guids)
    return guids

def guid_file_name(fileName):
    dn = os.path.dirname(fileName)
    fn = os.path.basename(fileName)
    gfn = os.path.normpath(os.path.join(dn, fn.split(".")[0] + ".guid"))
    return gfn

def _read_guids(fn, pfn):
    guids = {}
    try:
        fp = open(fn)
        for line in fp.readlines():
            line = line.strip()
            if not line: continue
            if line.startswith("#"): continue
            parts = line.split(":")
            if len(parts) == 2:
                name = parts[0]
                guid = parts[1]
                guids[name] = guid
        fp.close()
    except: pass # ignore fails to read

    if "Project" not in guids and os.path.isfile(pfn):
        pfp = open(pfn)
        for line in pfp.readlines():
            line = line.strip()
            if "<ProjectGuid>" in line:
                guid = line.split("<ProjectGuid>")[1].split('<')[0]
                if guid.startswith("{") and guid.endswith("}"):
                    guid = guid[1:-1]
                    print("Using guid {%s} from %s for 'Project'" % (guid, pfn))
                    guids["Project"] = guid
                    break

    if os.path.isfile(pfn) and pfn.endswith(".filters"):
        pfp = open(pfn)
        filterName = None
        for line in pfp.readlines():
            line = line.strip()
            if line.startswith("<Filter Include=\""):
                filterName = line.split('"')[1]
                continue
            if line.startswith("<UniqueIdentifier>"):
                guid = line.split("<UniqueIdentifier>")[1].split("<")[0]
                if guid.startswith("{") and guid.endswith("}"):
                    guid = guid[1:-1]
                    if filterName not in guids:
                        print("Using guid (%s) from %s for '%s'" % (guid, pfn, filterName))
                        guids[filterName] = guid

    return guids

def _update_guids(guids, names):
    for name in names:
        if name not in guids:
            guid = str(uuid.uuid4())
            guids[name] = guid
    return guids

def _write_guids(fn, guids):
    try:
        fp = open(fn)
        oldcontents = fp.read()
        fp.close()
    except: oldcontents = ""

    lines = []
    for name in sorted(guids):
        lines.append("%s:%s" % (name, guids[name]))

    lines.append("")
    contents = "\n".join(lines)

    if contents != oldcontents:
        print(fn)
        fp = open(fn, "w")
        fp.write(contents)
        fp.close()
        return fn

    return None

if __name__ == "__main__":
    import sys
    import getopt

    try:
        opts,args = getopt.getopt(sys.argv[1:], "")
        if len(args) < 2:
            raise Exception("Expecting at least the filename and one name on the command line")
        names = args[1:]
        fn = args[0]
        guids = _read_guids(fn)
        guids = _update_guids(guids, names)
        ofn = _write_guids(fn, guids)
        if ofn is None:
            print("%s unchanged" % fn)

    except Exception as e:
        sys.stderr.write("ERROR: %s\n" % str(e))
        raise SystemExit(-1)



"""
Writes a shell script to copy all the release builds into the equivalent
debug build directories.
"""

import os
import shutil
import sys

def run(rdn, sn):
    unpack = []
    for dn, dns, fns in os.walk(rdn):
        assert(dn.startswith(rdn))
        xdn = dn[len(rdn)+1:]
        bn = os.path.basename(dn)
        if bn.startswith("win32") and bn.endswith("_release"):
            bn2 = bn[:-8] + "_debug"
            dn2 = os.path.join(os.path.dirname(dn), bn2)
            if not os.path.isdir(dn2):
                assert(dn.startswith(rdn))
                assert(dn2.startswith(rdn))
                xdn2 = dn2[len(rdn)+1:]
                unpack.append("")
                unpack.append("echo %s" % xdn2)
                unpack.append("cp -r %s %s" % (xdn, xdn2))
                unpack.append("chmod +rwx %s/*.*" % os.path.join(xdn2))

    unpack.append("rm -f %s" % sn)
    ffn = os.path.join(rdn, sn)
    print("writing %s" % ffn)
    fp = open(ffn, "wb")
    fp.write("#!/bin/sh\n")
    fp.write("\n".join(unpack).replace("\\", "/"))
    fp.write("\n")
    fp.close()

if __name__ == "__main__":
    import getopt
    kwargs = {}
    try:
        opts,args = getopt.getopt(sys.argv[1:], "")
        run(*args)
    except Exception as e:
        sys.stderr.write("\n")
        sys.stderr.write("%s\n" % ("=" * 75))
        sys.stderr.write("%s\n" % str(e))
        sys.stderr.write("%s\n" % ("=" * 75))
        sys.stderr.write("\n")
        raise SystemExit(1)



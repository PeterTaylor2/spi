#
# looks for calls to a given Python extension within a list of Python files
#

import os
import sys
import glob

def findCallsInLine(fn, lineNumber, line, libName, ofp):
    work = line.strip()
    calls = 0
    while len(work) > 0:
        parts = work.split(libName + ".", 1)
        if len(parts) < 2: break
        work = parts[1]
        parts = work.split("(", 1)
        if len(parts) < 2: break
        func = parts[0]
        work = parts[1]
        ofp.write("%s,%s,%s\n" % (fn, func, lineNumber))
        calls += 1

    return calls


def findCalls(fn, libName, ofp, basename):

    totalCalls = 0
    fn = os.path.normpath(fn)
    lineNumber = 0
    for line in open(fn).readlines():
        lineNumber += 1
        if basename: fn = os.path.basename(fn)
        totalCalls += findCallsInLine(fn, lineNumber, line, libName, ofp)

    return totalCalls
    
def main(libName, dn, ofn=None, basename=True):

    if ofn:
        print ("writing %s" % ofn)
        ofp = open(ofn, "w")
    else: ofp = sys.stdout
    totalCalls = 0

    ofp.write("filename,func,lineNumber\n")
    for fn in glob.glob(os.path.join(dn, "*.py")):
        totalCalls += findCalls(fn, libName, ofp, basename)

    print ("number of calls detected: %d" % totalCalls)

def commandLine():
    import getopt

    kwargs = {}
    opts, args = getopt.getopt(sys.argv[1:], "o:b")

    if len(args) != 2:
        raise Exception("Must provide output file and source directory")
    
    for opt in opts:
        if opt[0] == "-o": kwargs["ofn"] = opt[1]
        elif opt[0] == "-b": kwargs["basename"] = True
        pass #end-for

    try:
        main(*args, **kwargs)
    except Exception as e:
        print ("ERROR: %s" % str(e))
        return -1
    return 0

if __name__ == "__main__": exit(commandLine())

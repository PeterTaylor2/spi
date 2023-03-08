import os
import sys
import difflib
import glob
import shutil

def compareOne(fn, baseline, other, altBaseline=None, tmpBaseline=None):
    fn1 = os.path.join(baseline, fn)
    if not os.path.isfile(fn1) and altBaseline is not None:
        afn = os.path.join(altBaseline, fn)
        if os.path.isfile(afn): fn1 = afn
    if tmpBaseline:
        shutil.copy(fn1, tmpBaseline)
    print ("comparing with %s" % fn1)
    fn2 = os.path.join(other, fn)
    fnd = os.path.join(other, fn + ".diff")
    if not os.path.isfile(fn1):
        fp = open(fnd, "w")
        fp.write("%s missing from %s\n" % (fn, os.path.dirname(fn1)))
        fp.close()
        return 1
    if not os.path.isfile(fn2):
        fp = open(fnd, "w")
        fp.write("%s missing from %s\n" % (fn, other))
        fp.close()
        return 1

    fp1 = open(fn1)
    fp2 = open(fn2)
    lines1 = [line.lstrip() for line in fp1.readlines()]
    lines2 = [line.lstrip() for line in fp2.readlines()]
    fp1.close()
    fp2.close()

    diffs = [line for line in difflib.context_diff(lines1, lines2,
                                                   fromfile=fn1,
                                                   tofile=fn2)]

    if len(diffs):
        fp = open(fnd, "w")
        fp.writelines(diffs)
        fp.close()
        return 1

    if os.path.isfile(fnd): os.remove(fnd)
    return 0

def main(baseline, other, ext="log", altBaseline=None, tmpBaseline=None):
    
    baseline = os.path.normpath(baseline)
    other    = os.path.normpath(other)
    if altBaseline is not None:
        altBaseline = os.path.normpath(altBaseline)
    if tmpBaseline is not None:
        tmpBaseline = os.path.normpath(tmpBaseline)
    oneFile  = False

    if tmpBaseline is not None:
        if os.path.isdir(tmpBaseline):
            print("Clearing %s" % tmpBaseline)
            for fn in glob.glob(os.path.join(tmpBaseline, "*.out")):
                os.remove(fn)
        else:
            print("Creating %s" % tmpBaseline)
            os.makedirs(tmpBaseline)

    if os.path.isfile(baseline) or os.path.isfile(other):
        if os.path.isdir(baseline):
            # hence other is a file but baseline is a directory
            allFiles = [os.path.join(baseline, os.path.basename(other))]
            other    = os.path.dirname(other)
        elif os.path.isdir(other):
            # hence baseline is a file and other is a directory
            allFiles = [baseline]
            baseline = os.path.dirname(baseline)
        else:
            # at least one of baseline and other is a file
            # neither are directories
            # it is possible that one of files is missing
            if os.path.basename(baseline) != os.path.basename(other):
                raise Exception("File basenames are not the same: %s %s" %
                                (baseline, other))
            allFiles = [baseline]
            baseline = os.path.dirname(baseline)
            other    = os.path.dirname(other)
        oneFile = True
    elif not os.path.isdir(baseline) and not os.path.isdir(other):
        # baseline and other are neither files nor directories
        # still the comparison was requested so we will show both as missing
        oneFile = True
        allFiles = [baseline]
        baseline = os.path.dirname(baseline)
        other    = os.path.dirname(other)

    if not oneFile:
        print ("removing old difference files")
        for fn in glob.glob(os.path.join(other, "*.diff")):
            print (fn)
            os.remove(fn)
        allFiles = glob.glob(os.path.join(other, "*.%s" % ext))
        allFiles.extend(glob.glob(os.path.join(baseline, "*.%s" % ext)))
        if altBaseline is not None:
            allFiles.extend(glob.glob(os.path.join(altBaseline, "*.%s" % ext)))

    done = set()
    numErrors = 0

    for fn in allFiles:
        fn = os.path.basename(fn)
        if fn in done: continue
        done.add(fn)
        numErrors += compareOne(fn, baseline, other, altBaseline=altBaseline, tmpBaseline=tmpBaseline)

    print ("number of errors detected: %d" % numErrors)

def commandLine():
    import getopt

    kwargs = {}
    opts, args = getopt.getopt(sys.argv[1:], "w", ["ext=", "altBaseline=", "tmpBaseline="])

    if len(args) != 2:
        raise Exception("Must provide baseline and other directory")
    
    for opt in opts:
        if opt[0] == "-w": raw_input("Enter to continue:")
        elif opt[0] == "--ext": kwargs["ext"] = opt[1]
        elif opt[0] == "--altBaseline": kwargs["altBaseline"] = opt[1]
        elif opt[0] == "--tmpBaseline": kwargs["tmpBaseline"] = opt[1]

    try:
        main(*args, **kwargs)
    except Exception as e:
        print (str(e))
        return -1
    return 0

if __name__ == "__main__": exit(commandLine())

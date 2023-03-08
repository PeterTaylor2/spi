import csv
import getopt
import glob
import json
import os
import sys

def run(abi, appendFile=None, outputFile=None, details=False):
    headings = ["type", "directory", "testsRun", "failedToRun",
                "failedTest", "elapsedTime"]

    writers = []
    if appendFile is not None:
        newFile = not(os.path.isfile(appendFile))
        afp = open(appendFile, "a")
        awriter = csv.writer(afp)
        if newFile: awriter.writerow(headings)
        writers.append(awriter)

    if outputFile is not None:
        ofp = open(outputfile, "w")
        owriter = csv.writer(ofp)
        owriter.writerow(headings)
        writers.append(owriter)

    dn = os.path.basename(os.getcwd())
    tt = "regression"
    elapsedTime = 0
    testsRun = 0
    failedToRun = 0
    failedTest = 0

    for fn in glob.glob(os.path.join(abi, "*.json")):
        data = json.load(open(fn))
        elapsedTime += data["elapsedTime"]
        testsRun    += 1
        errorText   = ""
        if data["failToRun"] == 0:
            diffFile = fn[:-5] + ".diff"
            if os.path.isfile(diffFile): failedTest += 1
        else: failedToRun += 1; errorText="(failed)"
        if details:
            nm = os.path.basename(fn).split(".")[0]
            print ("%-20s: %f %s" % (nm, data["elapsedTime"], errorText))

    if not writers:
        if details: print("")
        print ("testsRun:    %d" % testsRun)
        print ("elapsedTime: %.6f" % elapsedTime)
        print ("failedToRun: %d" % failedToRun)
        print ("failedTest:  %d" % failedTest)

    for writer in writers:
        writer.writerow([tt,dn,testsRun,failedToRun,failedTest,elapsedTime])

def main():
    kwargs = {}
    opts,args = getopt.getopt(sys.argv[1:], "a:o:d")
    for opt in opts:
        if opt[0] == "-a": kwargs["appendFile"] = opt[1]
        elif opt[0] == "-o": kwargs["outputFile"] = opt[1]
        elif opt[0] == "-d": kwargs["details"] = True
    if len(args) != 1:
        raise Exception("Must provide ABI on command line")
    run(*args, **kwargs)

if __name__ == "__main__":
    main()
    

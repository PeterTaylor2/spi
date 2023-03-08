import datetime
import glob
import json
import time
import os

import TestLib

def runOne(name, inputs, output):
    module = __import__(name)
    ifn = os.path.join(inputs, name + ".inp")
    ofn = os.path.join(output, name + ".out")

    driverClass = module.Driver

    return TestLib.runDriverClass(driverClass, ifn, ofn)

def main(drivers, inputs, output, startup=None):

    sys.path.insert(0, drivers)

    if not(os.path.isdir(output)):
        print ("creating directory %s" % output)
        os.makedirs(output)

    pythonPath = ";".join([item for item in os.getenv("PYTHONPATH").split(";")
                           if not "regression-test" in item])

    count = 0
    good = 0

    if startup:
        print ("executing: %s" % startup)
        exec(startup)
    
    for fn in glob.glob(os.path.join(drivers, "*.py")):
        name = os.path.basename(fn)[:-3]
        print ("==============================================================")
        print ("Running tests for %s" % name)
        print ("Writing to %s/%s.out" % (output, name))
        print ("PYTHONPATH=%s" % pythonPath)
        print ("==============================================================")
        success = runOne(name, inputs, output)
        count += 1
        if success:
            print ("ok")
            good += 1
        else: print ("*** FAILED ***")

    print
    print ("%s succeeded out of %s test drivers" % (good, count))
    print

if __name__ == "__main__":

    import getopt
    import sys

    # runs all the python scripts in the drivers directory
    # using inputs from the inputs directory and writing the
    # results to the output directory

    kwargs = {}
    opts, args = getopt.getopt(sys.argv[1:], "w", ["startup="])
    if len(args) != 3:
        raise RuntimeError(
            "%s: Expect [drivers] [inputs] [output] on command line" %
            (sys.argv[0]))

    for opt in opts:
        if opt[0] == "-w": raw_input("Enter to continue")
        elif opt[0] == "--startup": kwargs["startup"] = opt[1]
        pass #end-for

    main(*args, **kwargs)
    



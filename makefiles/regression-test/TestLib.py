import json
import time
import os
import getopt
import datetime
import sys
import re

if sys.version_info[0] > 2:
    raw_input = input
    time.clock = time.perf_counter

def runDriverClass(driverClass, ifn, ofn, profile=False):
    #
    # runs a driver class returning True on success and False on failure
    #
    odn = os.path.dirname(ofn)
    if not os.path.isdir(odn):
        print ("Making output directory %s" % odn)
        os.makedirs(odn)

    startTime = str(datetime.datetime.now())[:19]
    start = time.clock()

    try:
        driver = driverClass()
        if profile:
            import cProfile
            cProfile.runctx("driver.run(ifn,ofn)", globals(), locals())
        else:
            driver.run(ifn, ofn)
    except Exception as e:
        # driver.run should deal with exceptions for individual tests
        # hence any failures would be something more significant
        print ("ERROR: %s\n" % str(e))
        end = time.clock()
        elapsedTime = end - start
        jfp = open(ofn + ".json", "w")
        json.dump({"startTime" : startTime,
                   "ifn" : ifn,
                   "ofn" : ofn,
                   "numPerms" : 0,
                   "failToRun" : 1,
                   "elapsedTime" : elapsedTime}, jfp, indent=4)
        jfp.write("\n")
        jfp.close()
        return False

    return True

def TestMain(driverClass, driverFile):

    import getopt
    import sys

    opts,args = getopt.getopt(sys.argv[1:], "w", ["startup=", "profile"])
    compare = False
    profile = False
    for opt in opts:
        if opt[0] == "-w": raw_input("Enter to continue:")
        if opt[0] == "--startup":
            startupCode = opt[1]
            print ("executing %s" % startupCode)
            exec(startupCode)
        if opt[0] == "--profile": profile = True

    if len(args) == 0:
        # when run without parameters we want to guess the inut file
        # and run the comparison at the end
        #
        # from the Makefile we give it the parameters and then do the
        # comparisons afterwards
        #
        # without parameters we assume the add-ins have been installed into
        # the site-packages directory of the main python distribution and we
        # want to run the comparisons as well

        fn = os.path.abspath(driverFile)
        driverName = os.path.basename(fn)[:-3]
        dn = os.path.dirname(os.path.dirname(fn))
        ifn = os.path.join(dn, "inputs", driverName + ".inp")
        ofn = os.path.join(dn, "output", driverName + ".out")
        args.append(ifn)
        args.append(ofn)
        compare = True
        if not os.path.isfile(ifn):
            print ("creating dummy input file: %s" % ifn)
            driverClass().writeInputFile(ifn)

    if len(args) != 2:
        raise Exception("Expect inputFile and outputFile on command line")

    ifn = args[0]
    ofn = args[1]
    success = runDriverClass(driverClass, ifn, ofn, profile=profile)

    if compare and success:
        import compareResults
        bdn = os.path.join(os.path.dirname(odn), "baseline")
        result = compareResults.compareOne(os.path.basename(ofn), bdn, odn)
        if result == 0:
            print ("no differences with baseline")
        else:
            print ("differences detected with baseline")
            fnd = os.path.join(ofn + ".diff")
            print (open(fnd).read())

class TestDriver:

    BOOL   = 0
    INT    = 1
    FLOAT  = 2
    DATE   = 3
    STRING = 4
    OBJECT = 5
    MIN    = BOOL
    MAX    = OBJECT

    INDEX_TYPE_STRING = {
        BOOL : "BOOL",
        INT : "INT",
        FLOAT : "FLOAT",
        DATE : "DATE",
        STRING : "STRING",
        OBJECT : "OBJECT_FILE_NAME"}

    def __init__(self, dataFormat):
        self.dataFormat = {}
        self.fieldNames = []
        for item in dataFormat:
            if len(item) < 2 or len(item) > 3:
                raise Exception("Should be name,dataType with optional isArray")
            name     = item[0]
            dataType = item[1]
            isArray  = item[2] if len(item) == 3 else False
            if not(isinstance(name, str)):
                raise Exception("Name should be a string")
            if len(name) == 0 or len(name) > 30:
                raise Exception("Name should be a string of length 1-30")
            if not(isinstance(dataType, int)):
                raise Exception("DataType should be an integer")
            if dataType < TestDriver.MIN or dataType > TestDriver.MAX:
                raise Exception("DataType is out of range")
            if not(isinstance(isArray, bool)):
                raise Exception("isArray should be a bool")
            if name in self.dataFormat:
                raise Exception("Duplicate name '%s' defined" % name)
            self.dataFormat[name] = (dataType, isArray)
            self.fieldNames.append(name)

    def run(self, ifn, ofn):
        perms = self.readInputFile(ifn)

        ofp = open(ofn, "w")
        count = 0
        jfp = open(ofn + ".json", "w")
        startTime = time.ctime()
        start = time.clock()
        failToRun = 1
        try:
            for perm in perms:
                count += 1
                print ("Permutation %d of %d" % (count, len(perms)))
                ofp.write("Permutation %d of %d\n" % (count, len(perms)))
                self.writePermutation(ofp, perm)
                self.runOne(ofp, **perm)
                ofp.write("\n")
            failToRun = 0
        finally:
            end = time.clock()
            elapsedTime = end - start
            json.dump({"startTime" : startTime,
                       "ifn" : ifn,
                       "ofn" : ofn,
                       "numPerms" : len(perms),
                       "failToRun" : failToRun,
                       "elapsedTime" : elapsedTime},
                      jfp,
                      indent=4)
            jfp.write("\n")
            jfp.close()
            ofp.close()

    def readInputFile(self, ifn):
        perms = []
        data  = []
        incompleteList = False
        for line in open(ifn).readlines():
            line = line.strip()
            if not line:
                perms.extend(self.getPerms(data, incompleteList))
                data = []
                continue
            if line.startswith("#"): continue
            incompleteList = self.parseLine(line, data, incompleteList)
        perms.extend(self.getPerms(data, incompleteList))
        return perms

    def writeInputFile(self, fn):
        maxlen = max([len(name) for name in self.fieldNames])
        mask = "# %%-%ds %%s\n" % (maxlen+1)
        fp = open(fn, "a")
        fp.write("# example inputs\n\n")
        for name in self.fieldNames:
            dataType,isArray = self.dataFormat[name]
            typeString = self.INDEX_TYPE_STRING[dataType]
            if isArray: typeString = "[%s, ...]" % typeString
            fp.write(mask % (name + ":", typeString))

    def getPerms(self, data, incompleteList):
        if len(data) == 0: return []
        if incompleteList:
            raise Exception("Value for list '%s' is incomplete" % data[-1][0])

        allData = {}
        for name in self.fieldNames: allData[name] = []
        for item in data:
            name  = item[0]
            value = item[1]
            if name not in allData:
                raise Exception("Unexpected name '%s'" % name)
            allData[name].append(value)

        for name in self.fieldNames:
            if len(allData[name]) == 0: allData[name] = [None]

        sizes = [len(allData[name]) for name in self.fieldNames]
        numPerms = 1
        for size in sizes: numPerms *= size

        perms = []
        for i in range(numPerms):
            perm = {}
            divisor = numPerms
            for name,size in zip(self.fieldNames, sizes):
                divisor //= size # this will be exact integer division
                index = (i//divisor) % size # this involves discarding remainders
                perm[name] = allData[name][index]
            perms.append(perm)
        return perms

    def parseLine(self, line, data, listContinuation):
        # format of a good line is fieldName: value
        #
        # value can be list, bool, int, float, date, string, filename
        #
        # repeated values are handled by having the fieldName appear multiple
        # times in the list of data
        #
        # lists are enclosed in [..] and comma separated
        #
        # lists can go on multiple lines in which we mark the list as incomplete

        if listContinuation:
            name  = data[-1][0]
            value = "[" + line.strip()
        else:
            nameValue = line.split(":", 1)
            if len(nameValue) != 2:
                raise Exception("Line format is name: value")
            name  = nameValue[0]
            value = nameValue[1]

        if name not in self.dataFormat:
            raise Exception("Undefined field name '%s'" % name)

        try:
            value, incompleteList = self.parseValue(
                value, listContinuation, *self.dataFormat[name])
        except Exception as e:
            raise Exception("Could not parse value for '%s': %s" % (
                name, str(e)))

        if listContinuation:
            data[-1][1].extend(value)
        else: data.append([name, value])

        return incompleteList

    def parseValueNoCheck(self, value):
        if value[0] == '"':
            parts = value.split('"', 2)
            if len(parts) != 3: raise Exception("Unterminated string")
            item    = parts[1]
            theRest = parts[2].strip()
            if theRest:
                raise Exception("Unexpected data after string '%s'" % item)
            return item
        return self.parseNonStringItem(value)

    def parseNonStringItem(self, item):
        if item == "True": return True
        if item == "False": return False
        if item == "None": return None
        try: return int(item)
        except: pass # not an integer

        try: return float(item)
        except: pass # not a string

        # we accept yyyy-mm-dd as a date - otherwise treat as string after all
        try:
            parts = item.split("-")
            year  = int(parts[0])
            month = int(parts[1])
            day   = int(parts[2])
            return datetime.date(year, month, day)
        except: pass
        return item

    def validateValue(self, value, dataType):
        if isinstance(value, str):
            if dataType == TestDriver.STRING: return value
            if dataType == TestDriver.OBJECT: return value
            value = self.parseNonStringItem(value)

        if value is None: return value

        if dataType == TestDriver.BOOL:
            if isinstance(value, bool): return value
            if isinstance(value, int): return value != 0
            raise Exception("Value '%s' cannot be converted to bool" % value)

        if dataType == TestDriver.INT:
            if isinstance(value, int): return value
            if isinstance(value, bool): return 1 if value else 0
            raise Exception("Value '%s' cannot be converted to int" % value)

        if dataType == TestDriver.FLOAT:
            if isinstance(value, float): return value
            if isinstance(value, int): return float(value)
            raise Exception("Value '%s' cannot be converted to float" % value)

        if dataType == TestDriver.DATE:
            if isinstance(value, datetime.date): return value
            raise Exception("Value '%s' cannot be converted to date" % value)

        if dataType == TestDriver.STRING:
            return str(value)

        if dataType == TextDriver.OBJECT:
            raise Exception("Object filename '%s' must be a string" % value)

        raise Exception("Bad dataType %d" % dataType)

    def parseArray(self, value, dataType):
        if value[0] != "[":
            raise Exception("Array [..] expected")
        value = value[1:].strip()
        items = []
        while len(value) and value[0] != "]":
            if len(items) > 0:
                if value[0] != ",":
                    raise Exception("Values should be comma separated")
                value = value[1:].strip()
            if len(value) == 0: return items, True # incomplete list
            if value[0] == '"':
                parts = value.split('"', 2)
                if len(parts) != 3:
                    raise Exception("Unterminated string in array")
                item  = parts[1]
                value = parts[2].strip()
            else:
                commaPos   = value.find(",")
                bracketPos = value.find("]")
                if commaPos < 0 and bracketPos < 0:
                    raise Exception("Need , or ] to delimit array items")
                if commaPos == 0:
                    raise Exception("Undefined item in list")
                if commaPos > 0:
                    item  = self.parseNonStringItem(value[:commaPos])
                    value = value[commaPos:]
                else:
                    assert(bracketPos > 0)
                    item = self.parseNonStringItem(value[:bracketPos])
                    value = value[bracketPos:]
            items.append(self.validateValue(item,dataType))
        if len(value) == 0:
            raise Exception("Unterminated list - must end with , or ]")
        if value[0] == "]" and len(value) > 1:
            raise Exception("Unexpected text after end of list")
        return items, False # complete list

    def parseValue(self, value, listContinuation, dataType, isArray):
        value = value.strip()
        if not value:
            raise Exception("Undefined value")
        if listContinuation and not isArray:
            raise Exception("Cannot have listContinuation unless isArray")
        if isArray:
            return self.parseArray(value, dataType)

        goodValue = self.validateValue(self.parseValueNoCheck(value), dataType)
        return goodValue, False

    def writePermutation(self, ofp, perm):
        for name in self.fieldNames:
            value = perm[name]
            ofp.write("%-22s%s\n" % (name+":", self.valueToString(value)))
        ofp.write("\n")

    def valueToString(self, value):
        if isinstance(value, list):
            itemStrings = [self.valueToString(item) for item in value]
            return "[%s]" % (", ".join(itemStrings))
        if isinstance(value, float):
            s = "%.9g" % value
            if "." not in s and "e" not in s: s = "%s.0" % s
            return s
        if isinstance(value, str):
            if (len(value.split()) == 1) and value == value.strip():
                return value
            return repr(value)
        return str(value)


def stripErrors(errors):
    # in an attempt to match errors from different systems
    # we look through each line of the errors which are assumed to be
    # separated by new lines
    #
    # if we find a blank line we remove it
    # if we find a line which starts with functionName: then we remove function
    # if the remaining line just says Failed then we remove it
    #
    lines = errors.split("\n")
    output = []
    pattern = re.compile(r"e[-+]0\d\d")

    def toShortExponent(match):
        text = match.group()
        assert(len(text) == 5)
        assert(text[2] == "0")
        return text[:2] + text[3:]

    for line in lines:
        line = line.strip()
        if not line: continue
        parts = line.split(": ", 1)
        if len(parts) == 2: line = parts[1].strip()
        if line.startswith("Failed"): continue
        line = pattern.sub(toShortExponent, line)
        output.append(line)

    return "\n".join(output)

if __name__ == "__main__":

    import sys
    ifn = r"C:\TEMP\MyTest.inp"
    ofn = r"C:\TEMP\MyTest.out"

    ifp = open(ifn, "w")
    ifp.write("# this is a comment\n")
    ifp.write("aBool: True\n")
    ifp.write("aBool: False\n")
    ifp.write("\n")
    ifp.write("aBool: False\n")
    ifp.close()

    class MyTestDriver(TestDriver):
        def __init__(self):
            TestDriver.__init__(
                self,
                [("aBool", TestDriver.BOOL)])
        def runOne(self, ofp, **kwargs):
            print (kwargs)
            ofp.write("%s\n" % kwargs["aBool"])

    TestMain(MyTestDriver, __file__)





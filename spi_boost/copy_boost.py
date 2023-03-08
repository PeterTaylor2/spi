#
# python script to copy from boost to spi_boost
#
# command line:
#   python copy_boost.py <source_directory> files_and_directories_to_copy
#
# we will always replace all instances of boost and BOOST with spi_boost
# and SPI_BOOST
#

import os
import sys

def copyLine(oldLine, ofp):
    newLine = oldLine.replace("boost", "spi_boost")
    newLine = newLine.replace("www.spi_boost", "www.boost")
    newLine = newLine.replace("BOOST", "SPI_BOOST")
    newLine = newLine.replace(" UUID_", " SPI_BOOST_UUID_")
    if newLine != oldLine:
        sys.stdout.write("- %s" % oldLine)
        sys.stdout.write("+ %s" % newLine)
    ofp.write(newLine)

def copyFile(sourceDir, fileName):
    dn = os.path.dirname(fileName)
    if dn and not os.path.isdir(dn): os.makedirs(dn)
    ifn = os.path.join(sourceDir, fileName)
    ofn = fileName
    ifp = open(ifn)
    print ofn
    ofp = None
    ofp = open(ofn, "w")
    for line in ifp.xreadlines(): copyLine(line, ofp)

def copyDirectory(sourceDir, dirName):
    rootDir = os.path.join(sourceDir, dirName)
    for dn,dns,fns in os.walk(rootDir):
        # dn includes rootDir
        # copyFile wants fileName relative to sourceDir
        dn = dn[len(sourceDir)+1:]
        for fn in fns: copyFile(sourceDir, os.path.join(dn, fn))

def copyList(sourceDir, toCopyList):
    for item in toCopyList:
        pathName = os.path.join(sourceDir, item)
        if os.path.isfile(pathName): copyFile(sourceDir, item)
        elif os.path.isdir(pathName): copyDirectory(sourceDir, item)
        else: print ("WARNING: %s is not a file or directory in %s" % (
            item, sourceDir))

if __name__ == "__main__":
    import sys
    import getopt

    opts,args = getopt.getopt(sys.argv[1:], "")

    if len(args) == 1:
        sourceDir = args[0]
    else:
        raise Exception, "Expecting source directory on command line"

    sourceDir = os.path.normpath(sourceDir)
    toCopyList = ["assert.hpp",
                  "checked_delete.hpp",
                  "config.hpp",
                  "cstdint.hpp",
                  "core",
                  "current_function.hpp",
                  "enable_shared_from_this.hpp",
                  "intrusive_ptr.hpp",
                  "limits.hpp",
                  "memory_order.hpp",
                  "predef",
                  "predef.h",
                  "scoped_array.hpp",
                  "scoped_ptr.hpp",
                  "shared_array.hpp",
                  "shared_ptr.hpp",
                  "smart_ptr.hpp",
                  "throw_exception.hpp",
                  "weak_ptr.hpp",
                  "config",
                  "detail",
                  "exception",
                  "smart_ptr"]

    copyList(sourceDir, toCopyList)

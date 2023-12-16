#
# copies windows redist files
#
# the syntax is as follows:
#
# python copyRedist.py outputDir sourceDir
#
# any files in outputDir of the format api-ms-win-*.dll will be deleted
# if not defined in sourceDir
#
# all files in sourceDir of the format *.dll will be copied
# this also typically includes ucrtbase.dll
#

import glob
import filecmp
import os
import sys
import shutil

if sys.version_info.major < 3:
    input = raw_input

def copyFile(fn, odn):
    if not os.path.isdir(odn):
        print("creating %s" % odn)
        os.makedirs(odn)
    ofn = os.path.join(odn, os.path.basename(fn))
    copy = not os.path.isfile(ofn)
    if not copy:
        copy = not filecmp.cmp(fn,ofn)

    if copy:
        print("copying %s" % ofn)
        shutil.copy2(fn, ofn)
        return 1

    return 0

def main(target, source):

    source = os.path.normpath(source)
    target = os.path.normpath(target)
    if source == target:
        raise Exception("source (%s) = target" % source)

    originals = set()
    for ffn in glob.glob(os.path.join(target, "api-ms-win*.dll")):
        originals.add(os.path.basename(ffn))

    filesCopied = 0
    filesRemoved = 0
    for ffn in glob.glob(os.path.join(source, "*.dll")):
        fn = os.path.basename(ffn)
        if fn in originals: originals.remove(fn)
        filesCopied += copyFile(ffn, target)

    for fn in originals:
        ffn = os.path.join(target, fn)
        if os.path.isfile(ffn):
            print("Removing %s" % ffn)
            try:
                os.remove(ffn)
                filesRemoved += 1
            except: pass # ignore failures

    if filesCopied > 0:
        print("%d files copied" % filesCopied)

    if filesRemoved > 0:
        print("%d files removed" % filesRemoved)

if __name__ == "__main__":
    import getopt

    kwargs = {}

    opts,args = getopt.getopt(sys.argv[1:], "")

    if len(args) != 2:
        raise Exception("Expecting 'target source' on command line")

    main(*args)



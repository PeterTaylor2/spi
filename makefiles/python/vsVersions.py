import os

"""
Checks whether a directory exists and throws an exception if it doesn't
"""
def checkDirectory(dn):
    if not os.path.isdir(dn):
        raise Exception("%s is not a directory")
    return True

"""
Reverse sorts the directories in dn and returns that as the latest version.
We are assuming that the multi-part separations are in alphabetical order.
"""
def getLatestVersion(dn):
    versions = os.listdir(dn)
    for version in sorted(versions, reverse=True):
        dn = os.path.join(dn, version)
        if os.path.isdir(dn):
            return version
    raise Exception("no version directories found in directory %s" % dn)

def getToolsVersion(compiler, packageType):
    rootDir = compilerRoot(compiler, packageType=packageType)
    msvcDir = os.path.join(rootDir, "VC", "Tools", "MSVC")
    checkDirectory(msvcDir)
    return getLatestVersion(msvcDir)
        
def getKitsVersion():
    kitsRoot = os.path.normpath("C:/Program Files (x86)/Windows Kits")

    found = False
    for dn in ["10", "8.1"]:
        fdn = os.path.join(kitsRoot, dn)
        if os.path.isdir(fdn):
            kitsRoot = fdn
            found = True
            break

    if not found:
        raise Exception("Could not find windows kits version in %s" % kitsRoot)

    includeDir = os.path.join(kitsRoot, "Include")
    checkDirectory(includeDir)
    return getLatestVersion(includeDir)

def getPackageType(compiler):
    rootDir = compilerRoot(compiler)
    if not os.path.isdir(rootDir):
        raise Exception("%s is not a directory - cannot find packageType")

    for packageType in ["Professional", "Community"]:
        dn = os.path.join(rootDir, packageType)
        if os.path.isdir(dn):
            return packageType

    raise Exception("Could not find package type via directory %s" % rootDir)

def getCompilerDirectory(compiler):
    if compiler == "VS15":
        return "2017"
    if compiler == "VS16":
        return "2019"
    if compiler == "VS17":
        return "2022"

    raise Exception("Unknown compiler %s" % compiler)

def compilerRoot(compiler, packageType=None):
    rootDir = os.path.normpath(os.path.join(
            "C:/Program Files/Microsoft Visual Studio",
            getCompilerDirectory(compiler)))
    if packageType is not None:
        rootDir = os.path.join(rootDir, packageType)
    return rootDir

def updateFile(name, contents):
    if os.path.isfile(name):
        print("updating %s" % name)
        fp = open(name, "a")
    else:
        print("writing %s" % name)
        fp = open(name, "w")

    fp.write(contents)

def main():
    computerName = os.getenv("COMPUTERNAME")
    if not computerName:
        raise Exception("Cannot find COMPUTERNAME")
                               
    me = os.path.normpath(__file__)
    config = os.path.normpath(os.path.join(me, "..", "..", "config"))
    computers = os.path.join(config, "computers")
    hasComputers = os.path.isdir(computers)

    ofn = os.path.join(computers, "%s.mk" % computerName) if hasComputers else None

    contents = []
    kitsVersion = getKitsVersion() 
    for compiler in ["VS15", "VS16", "VS17"]:
        try:
            packageType = getPackageType(compiler)
            toolsVersion = getToolsVersion(compiler, packageType)
            contents.append("")
            contents.append("G_%s_PACKAGE_TYPE=%s" % (compiler, packageType))
            contents.append("G_%s_KITS_VERSION=%s" % (compiler, kitsVersion))
            contents.append("G_%s_TOOLS_VERSION=%s" % (compiler, toolsVersion))
        except Exception:
            print("compiler %s not installed" % compiler)

    if len(contents):
        contents.append("")
        if ofn is not None:
            updateFile(ofn, "\n".join(contents))
        for line in contents: print(line)

if __name__ == "__main__":
    try:
        main()
        exit(0)
    except Exception as e:
        print("ERROR: %s" % str(e))
        exit(-1)

                
        

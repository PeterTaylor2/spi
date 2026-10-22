import os
from pathlib import Path
import json
import subprocess

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
        v0 = version[0]
        if v0 < '1' or v0 > '9':
            continue
        dn = os.path.join(dn, version)
        if os.path.isdir(dn):
            return version
    raise Exception("no version directories found in directory %s" % dn)

def getToolsVersion(compiler, rootDir):
    msvcDir = os.path.join(rootDir, "VC", "Tools", "MSVC")
    checkDirectory(msvcDir)
    return getLatestVersion(msvcDir)

def getRedistVersion(compiler, rootDir):
    msvcDir = os.path.join(rootDir, "VC", "Redist", "MSVC")
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
    if compiler == "VS16":
        return "2019"
    if compiler == "VS17":
        return "2022"
    if compiler == "VS18":
        return "18"

    raise Exception("Unknown compiler %s" % compiler)

def compilerRoot(compiler, packageType=None):
    rootDir = os.path.normpath(os.path.join(
            "C:/Program Files/Microsoft Visual Studio",
            getCompilerDirectory(compiler)))
    if packageType is not None:
        rootDir = os.path.join(rootDir, packageType)
    return rootDir

def getValues(lines):
    # assume we have a Makefile and lines are the lines coming from readlines
    values = {}

    for line in lines:
        sline = line.strip()
        items = sline.split("=")
        if len(items) == 2:
            name = items[0].strip()
            value = items[1].strip()
            values[name] = value
    return values

def getNewLines(contents, values):
    newLines = []
    lines = contents.split("\n")
    for line in lines:
        sline = line.strip()
        items = sline.split("=")
        if len(items) == 2:
            name = items[0].strip()
            value = items[1].strip()
            if name in values and value == values[name]:
                continue
        newLines.append(line)

    return newLines

def combineLines(lines, newLines):
    # we only add blank lines when we have content
    combined = lines[:]
    hasBlank = False
    changed = False
    for line in newLines:
        sline = line.strip()
        if not sline:
            hasBlank = True
            continue
        changed = True
        if hasBlank:
            combined.append("\n")
        combined.append("%s\n" % sline)
        hasBlank = False

    return combined, changed

def updateFile(name, contents):
    if os.path.isfile(name):
        ifp = open(name, "r")
        lines = ifp.readlines()
        ifp.close()
        values = getValues(lines)
        newLines = getNewLines(contents, values)
        combined, changed = combineLines(lines, newLines)
        if changed:
            print("updating %s" % name)
            ofp = open(name, "w", newline="\n")
            ofp.writelines(combined)
        else:
            print("%s unchanged" % name)
    else:
        print("writing %s" % name)
        ofp = open(name, "w", newline="\n")
        ofp.write(contents)
        ofp.close()

def get_contents():
    contents = []

    vswhere = str(Path(os.environ['ProgramFiles(x86)']) / 'Microsoft Visual Studio/Installer/vswhere.exe')
    print("using %s to locate visual studio installations" % vswhere)

    installs = json.loads(subprocess.check_output([
            vswhere, '-products', '*', '-version', '[16.0,19.0)',
            '-requires', 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64', '-format', 'json']))

    indexCompilerRootDir = {}
    for item in installs:
        version = item['installationVersion']
        location = item['installationPath']

        for cv in ["16", "17", "18"]:
            if version.startswith(cv+"."):
                indexCompilerRootDir["VS"+cv] = location

    kitsVersion = getKitsVersion() 
    for compiler in ["VS16", "VS17", "VS18"]:
        try:
            rootDir = indexCompilerRootDir[compiler]
            print(compiler, "installed in", rootDir)
            packageType = os.path.basename(rootDir)
            # the makefiles have a hard-coded path for the compiler
            # we need to check that these match with the full path given
            rootDir2 = compilerRoot(compiler, packageType=packageType)
            if rootDir != rootDir2:
                raise Exception("Makefiles will not be able to find %s for %s" % (rootDir, compiler))
            toolsVersion = getToolsVersion(compiler, rootDir)
            redistVersion = getRedistVersion(compiler, rootDir)
            contents.append("")
            contents.append("G_%s_PACKAGE_TYPE=%s" % (compiler, packageType))
            contents.append("G_%s_KITS_VERSION=%s" % (compiler, kitsVersion))
            contents.append("G_%s_TOOLS_VERSION=%s" % (compiler, toolsVersion))
            contents.append("G_%s_REDIST_VERSION=%s" % (compiler, redistVersion))
        except Exception:
            print("compiler %s not installed" % compiler)
    return contents

def main():
    computerName = os.getenv("COMPUTERNAME")
    if not computerName:
        raise Exception("Cannot find COMPUTERNAME")
                               
    me = os.path.normpath(__file__)
    config = os.path.normpath(os.path.join(me, "..", "..", "config"))
    computers = os.path.join(config, "computers")
    hasComputers = os.path.isdir(computers)

    ofn = os.path.join(computers, "%s.mk" % computerName) if hasComputers else None

    contents = get_contents()

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

                
        

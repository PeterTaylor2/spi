# makes a visual studio project file for VC9

import uuid
import glob
import os

def xmlfiles(filenames, indent=12):
    """
    Creates the XML block for representing a bunch of filenames
    """
    indentString = " " * indent
    lines = ['%s<File RelativePath="%s"/>' % (indentString, filename)
             for filename in filenames]
    return "\n".join(lines)

def makeProj(fileName, name, compiler, target, srcDir, incDir,
             headerPatterns, sourcePatterns,
             extraHeaderDirs, extraSourceDirs,
             buildSuffix="",
             makefileTarget="target",
             cleanTarget="clean"):

    compiler = "msvc9" # we don't allow a later compiler

    headerFiles = []
    sourceFiles = []
    for pattern in headerPatterns:
        headerFiles.extend(glob.glob(os.path.join(incDir,pattern)))
        if srcDir != incDir:
            headerFiles.extend(glob.glob(os.path.join(srcDir,pattern)))
        for dn in extraHeaderDirs:
            headerFiles.extend(glob.glob(os.path.join(dn, pattern)))
    for pattern in sourcePatterns:
        sourceFiles.extend(glob.glob(os.path.join(srcDir, pattern)))
        for dn in extraSourceDirs:
            sourceFiles.extend(glob.glob(os.path.join(dn, pattern)))

    if os.path.isfile("Makefile"): sourceFiles.append("Makefile")

    headerFiles = [os.path.normpath(fn) for fn in headerFiles]
    sourceFiles = [os.path.normpath(fn) for fn in sourceFiles]

    debugBuildDir   = "win32_" + compiler + "_debug" + buildSuffix
    releaseBuildDir = "win32_" + compiler + "_release" + buildSuffix

    data = {"name" : name,
            "debug_output_dir" : debugBuildDir,
            "debug_intermediate_dir": debugBuildDir,
            "release_output_dir" : releaseBuildDir,
            "release_intermediate_dir" : releaseBuildDir,
            "compiler" : compiler,
            "makefile_target" : makefileTarget,
            "target" : target,
            "clean_target" : cleanTarget}

    data["headerFiles"] = xmlfiles(headerFiles)
    data["sourceFiles"] = xmlfiles(sourceFiles)

    try:
        fp = open(fileName)
        oldcontents = fp.read()
        fp.close()
    except: oldcontents = ""

    if "ProjectGUID" in oldcontents:
        guid = oldcontents.split("ProjectGUID")[1].split('"')[1]
        if guid.startswith("{") and guid.endswith("}"):
            guid = guid[1:-1]
        else: guid = None
    else: guid = None
    if guid is None: guid = str(uuid.uuid1()).upper()
    data["guid"] = guid

    global template
    contents = template % data

    if contents != oldcontents:
        print(fileName)
        fp = open(fileName, "w")
        fp.write(contents)
        fp.close()

template = """\
<?xml version="1.0" encoding="Windows-1252"?>
<VisualStudioProject
    ProjectType="Visual C++"
    Version="9.00"
    Name="%(name)s"
    ProjectGUID="{%(guid)s}"
    RootNamespace="%(name)s"
    Keyword="MakeFileProj"
    TargetFrameworkVersion="196613"
    >
    <Platforms>
        <Platform Name="Win32"/>
    </Platforms>
    <Configurations>
        <Configuration
            Name="Debug|Win32"
            OutputDirectory="%(debug_output_dir)s"
            IntermediateDirectory="%(debug_intermediate_dir)s"
            ConfigurationType="0"
            >
            <Tool
                Name="VCNMakeTool"
                BuildCommandLine="make -j2 %(makefile_target)s COMPILER=%(compiler)s DEBUG=1"
                ReBuildCommandLine="make %(clean_target)s COMPILER=%(compiler)s DEBUG=1 &amp;&amp; make -j2 %(makefile_target)s COMPILER=%(compiler)s DEBUG=1"
                CleanCommandLine="make %(clean_target)s COMPILER=%(compiler)s DEBUG=1"
                Output="$(ProjectDir)%(debug_output_dir)s\%(target)s"
            />
        </Configuration>
        <Configuration
            Name="Release|Win32"
            OutputDirectory="%(release_output_dir)s"
            IntermediateDirectory="%(release_intermediate_dir)s"
            ConfigurationType="0"
            >
            <Tool
                Name="VCNMakeTool"
                BuildCommandLine="make -j2 %(makefile_target)s COMPILER=%(compiler)s"
                ReBuildCommandLine="make %(clean_target)s COMPILER=%(compiler)s &amp;&amp; make -j2 %(makefile_target)s COMPILER=%(compiler)s"
                CleanCommandLine="make %(clean_target)s COMPILER=%(compiler)s"
                Output="$(ProjectDir)%(release_output_dir)s\%(target)s"
            />
        </Configuration>
    </Configurations>
    <Files>
        <Filter Name="Source Files">
%(sourceFiles)s
        </Filter>
        <Filter Name="Header Files">
%(headerFiles)s
        </Filter>
    </Files>
</VisualStudioProject>
"""

if __name__ == "__main__":
    import sys
    import getopt

    opts,args = getopt.getopt(sys.argv[1:], "h:s:c:t:S:H:",
                              ["buildSuffix=",
                               "makefileTarget=",
                               "cleanTarget="])
    headerPatterns  = ["*.hpp", "*.h"]
    sourcePatterns  = ["*.cpp", "*.c"]
    extraSourceDirs = []
    extraHeaderDirs = []
    compiler        = "msvc9"
    kwargs          = {}
    for opt in opts:
        if opt[0] == "-h": headerPatterns.append(opt[1])
        elif opt[0] == "-s": sourcePatterns.append(opt[1])
        elif opt[0] == "-H": extraHeaderDirs.append(opt[1])
        elif opt[0] == "-S": extraSourceDirs.append(opt[1])
        elif opt[0] == "-c": compiler = opt[1]
        elif opt[0] == "-t": kwargs["makefileTarget"] = opt[1]
        elif opt[0] == "--buildSuffix": kwargs["buildSuffix"] = opt[1]
        elif opt[0] == "--makefileTarget": kwargs["makefileTarget"] = opt[1]
        elif opt[0] == "--cleanTarget": kwargs["cleanTarget"] = opt[1]

    if len(args) != 5:
        print(" ".join(sys.argv))
        raise Exception("Expecting 5 arguments: fileName name srcDir incDir target")

    fileName = args[0]
    name     = args[1]
    srcDir   = args[2]
    incDir   = args[3]
    target   = args[4]

    makeProj(fileName, name, compiler, target, srcDir, incDir,
             headerPatterns, sourcePatterns,
             extraHeaderDirs, extraSourceDirs, **kwargs)


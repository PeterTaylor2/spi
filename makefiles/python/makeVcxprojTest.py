# makes a visual studio project file for running regression tests

import glob
import os

import makeVcxproj
import vstools
import guidUtils

def _xmlfiles(filenames, label, indent=4):
    """
    Creates the XML block for representing a bunch of filenames
    """
    indentString = " " * indent
    lines = ['%s<%s Include="%s"/>' % (indentString, label, filename)
             for filename in filenames]
    return "\n".join(lines)

def _get_property_groups(platforms, makefileTarget, cleanTarget, bin,
        compiler, defaultCompiler, defaultBits, includePath, exePath):

    vsIncludePath = []
    for include in includePath:
        if include == ".": vsIncludePath.append("$(MSBuildProjectDirectory)")
        else: vsIncludePath.append(
            "$(MSBuildProjectDirectory)\\%s" % os.path.normpath(include))
    if len(vsIncludePath): vsIncludePath.append("")
    vsIncludePath = ";".join(vsIncludePath)

    lines = []
    for platform in platforms:
        name = platform[0]
        bits = platform[1]
        systemIncludes = vstools.systemIncludes(compiler, bits)
        for debug in ["Debug", "Release"]:
            debugFlag = " DEBUG=1" if debug == "Debug" else ""
            condition = "'$(Configuration)|$(Platform)'=='%s|%s'" % (debug,name)
            lines.append("    <NMakeBuildCommandLine Condition=\"%s\">make %s VS_BUILD=1 COMPILER=%s BITS=%s%s</NMakeBuildCommandLine>" % (
                condition, makefileTarget, compiler, bits, debugFlag))
            lines.append("    <NMakeReBuildCommandLine Condition=\"%s\">make %s VS_BUILD=1 COMPILER=%s BITS=%s%s &amp;&amp; make %s VS_BUILD=1 COMPILER=%s BITS=%s%s</NMakeReBuildCommandLine>" % (
                    condition, cleanTarget, compiler, bits, debugFlag,
                    makefileTarget, compiler, bits, debugFlag))
            lines.append("    <NMakeCleanCommandLine Condition=\"%s\">make %s VS_BUILD=1 COMPILER=%s BITS=%s%s</NMakeCleanCommandLine>" % (
                condition, cleanTarget, compiler, bits, debugFlag))
            lines.append("    <ExecutablePath Condition=\"%s\">%s;$(ExecutablePath)</ExecutablePath>" % (condition, os.path.normpath(bin)))
            if len(vsIncludePath):
                lines.append("    <NMakeIncludeSearchPath>%s%s</NMakeIncludeSearchPath>" % (
                    vsIncludePath, systemIncludes))

    if exePath:
        for platform in platforms:
            name = platform[0]
            bits = platform[1]
            for debug in ["Debug", "Release"]:
                abi = makeVcxproj.get_abi(compiler, bits, debug, defaultCompiler, defaultBits)
                condition = "'$(Configuration)|$(Platform)'=='%s|%s'" % (debug,name)
                lines.append("  </PropertyGroup>")
                lines.append("  <PropertyGroup Condition=\"%s\">" % condition)
                lines.append("    <NMakeOutput>%s</NMakeOutput>" % (exePath.replace("G_ABI", abi)))

    return "\n".join(lines)

def make_proj(fileName, 
              name,
              driverPatterns,
              inputPatterns,
              baselinePatterns,
              compiler,
              defaultCompiler,
              defaultBits,
              toolsVersion,
              platformToolset,
              makefileTarget="all", 
              bin=r"C:\cygwin\bin",
              includes=None,
              exePath=None):

    guids = guidUtils.read_and_write_guids(fileName, ["Project"])
    guids.update(guidUtils.read_and_write_guids(fileName + ".filters", ["baseline", "inputs", "drivers"],
        gfn = guidUtils.guid_file_name(fileName)))

    driverFiles = []
    inputFiles = []
    baselineFiles = []
    includePath = [] if includes is None else includes[:]
    for pattern in driverPatterns:
        driverFiles.extend(glob.glob(pattern))

    for pattern in inputPatterns:
        inputFiles.extend(glob.glob(pattern))

    for pattern in baselinePatterns:
        baselineFiles.extend(glob.glob(pattern))

    driverFiles   = [os.path.normpath(fn) for fn in driverFiles]
    inputFiles    = [os.path.normpath(fn) for fn in inputFiles]
    baselineFiles = [os.path.normpath(fn) for fn in baselineFiles]
    buildFiles    = makeVcxproj.get_build_files()

    platforms = vstools.platforms(compiler)

    projectConfigurations  = makeVcxproj.get_project_configurations(platforms)
    propertyConfigurations = makeVcxproj.get_property_configurations(
        platforms, platformToolset)
    importPropertySheets   = makeVcxproj.get_import_property_sheets(platforms)
    propertyGroups         = _get_property_groups(platforms, makefileTarget, "clean", bin,
            compiler, defaultCompiler, defaultBits, includePath, exePath)

    data = {"name" : name,
            "tools_version" : toolsVersion}

    allFiles = sorted(driverFiles + inputFiles + baselineFiles + buildFiles)

    data["allFiles"]               = _xmlfiles(allFiles, "None")
    data["projectConfigurations"]  = projectConfigurations
    data["propertyConfigurations"] = propertyConfigurations
    data["importPropertySheets"]   = importPropertySheets
    data["propertyGroups"]         = propertyGroups

    try:
        fp = open(fileName)
        oldcontents = fp.read()
        fp.close()
    except: oldcontents = ""

    guid = guids["Project"]
    data["guid"] = guid

    global _template
    contents = _template % data

    if contents != oldcontents:
        print(fileName)
        fp = open(fileName, "w")
        fp.write(contents)
        fp.close()
        if len(oldcontents):
            print(fileName + ".bak")
            fp = open(fileName + ".bak", "w")
            fp.write(oldcontents)
            fp.close()
    elif os.path.isfile(fileName + ".bak"):
        print("removing", (fileName + ".bak"))
        os.remove(fileName + ".bak")

    filters  = [("None", "baseline", sorted(baselineFiles)),
                ("None", "inputs", sorted(inputFiles)),
                ("None", "drivers", sorted(driverFiles)),
                ("None", None, sorted(buildFiles))]
    makeVcxproj.write_filters(fileName, filters, toolsVersion, guids)

def command_line(compiler, toolsVersion, platformToolset):
    import sys
    import getopt

    opts,args = getopt.getopt(sys.argv[1:], "I:i:o:d:t:b:",
                              ["compiler=", "exe="])
    driverPatterns   = ["drivers/*.py"]
    inputPatterns    = ["inputs/*.inp"]
    baselinePatterns = ["baseline/*.out"]
    kwargs           = {}
    includes         = []
    for opt in opts:
        if opt[0] == "-d": driverPatterns.append(opt[1])
        elif opt[0] == "-i": inputPatterns.append(opt[1])
        elif opt[0] == "-I": includes.append(opt[1])
        elif opt[0] == "-o": baselinePatterns.append(opt[1])
        elif opt[0] == "-t": kwargs["makefileTarget"] = opt[1]
        elif opt[0] == "-b": kwargs["bin"] = opt[1]
        elif opt[0] == "--compiler": compiler = opt[1]
        elif opt[0] == "--exe": kwargs["exePath"] = opt[1]

    if len(includes): kwargs["includes"] = includes
    if len(args) != 4:
        print((" ".join(sys.argv)))
        raise Exception("Expecting 4 arguments: fileName name defaultCompiler defaultBits")

    fileName = args[0]
    name     = args[1]
    defaultCompiler = args[2]
    defaultBits     = int(args[3])

    make_proj(fileName, name, driverPatterns, inputPatterns, baselinePatterns,
              compiler, defaultCompiler, defaultBits, toolsVersion, platformToolset, **kwargs)


_template = """\
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" ToolsVersion="%(tools_version)s" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
%(projectConfigurations)s
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <ProjectGuid>{%(guid)s}</ProjectGuid>
    <RootNamespace>%(name)s</RootNamespace>
    <ProjectName>%(name)s</ProjectName>
    <Keyword>MakeFileProj</Keyword>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\\Microsoft.Cpp.Default.props" />
%(propertyConfigurations)s
  <Import Project="$(VCTargetsPath)\\Microsoft.Cpp.props" />
  <ImportGroup Label="ExtensionSettings">
  </ImportGroup>
%(importPropertySheets)s
  <PropertyGroup Label="UserMacros" />
  <PropertyGroup>
%(propertyGroups)s
  </PropertyGroup>
  <ItemDefinitionGroup>
  </ItemDefinitionGroup>
  <ItemGroup>
%(allFiles)s
  </ItemGroup>
  <Import Project="$(VCTargetsPath)\\Microsoft.Cpp.targets" />
</Project>
"""


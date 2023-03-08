# makes a visual studio project file for VC9

import uuid
import glob
import os

import makeVcxproj
import vstools
import guidUtils

def command_line(compiler, toolsVersion, platformToolset):
    import sys
    import getopt

    opts,args = getopt.getopt(sys.argv[1:], "x:s:c:t:b:I:",
                              ["compiler="])
    configPatterns  = ["*.cfg"]
    servicePatterns = ["*.svc"]
    extraSourceDirs = []
    kwargs = {}
    includePath = []
    for opt in opts:
        if opt[0] == "-c": configPatterns.append(opt[1])
        elif opt[0] == "-s": servicePatterns.append(opt[1])
        elif opt[0] == "-x": extraSourceDirs.append(opt[1])
        elif opt[0] == "-t": kwargs["makefileTarget"] = opt[1]
        elif opt[0] == "-b": kwargs["bin"] = opt[1]
        elif opt[0] == "-I": includePath.append(opt[1])
        elif opt[0] == "--compiler": compiler = opt[1]

    if len(args) != 4:
        print((" ".join(sys.argv)))
        raise Exception("Expecting 4 arguments: fileName name srcDir target")

    fileName = args[0]
    name     = args[1]
    srcDir   = args[2]
    target   = args[3]

    projectFileVersion = "UNKNOWN"

    make_proj(fileName, name, target, srcDir, configPatterns, servicePatterns,
               includePath, compiler, toolsVersion, platformToolset, projectFileVersion, extraSourceDirs,
               **kwargs)


def _xmlfiles(filenames, label, indent=4):
    """
    Creates the XML block for representing a bunch of filenames
    """
    indentString = " " * indent
    lines = ['%s<%s Include="%s"/>' % (indentString, label, filename)
             for filename in filenames]
    return "\n".join(lines)

def _get_property_groups(platforms, target, makefileTarget, cleanTarget, bin,
                      compiler, includePath):

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
    return "\n".join(lines)

def make_proj(fileName, name, target, srcDir,
             configPatterns, servicePatterns,
             includePath,
             compiler,
             toolsVersion, platformToolset, projectFileVersion,
             extraSourceDirs,
             makefileTarget="target", bin=r"C:\cygwin\bin"):

    guids = guidUtils.read_and_write_guids(fileName, ["Project"])

    configFiles  = []
    serviceFiles = []
    for pattern in configPatterns:
        configFiles.extend(glob.glob(os.path.join(srcDir,pattern)))
        for xd in extraSourceDirs:
            configFiles.extend(glob.glob(os.path.join(xd,pattern)))

    for pattern in servicePatterns:
        serviceFiles.extend(glob.glob(os.path.join(srcDir, pattern)))

    configFiles  = [os.path.normpath(fn) for fn in configFiles]
    serviceFiles = [os.path.normpath(fn) for fn in serviceFiles]
    serviceFiles.extend(makeVcxproj.get_build_files())

    platforms = vstools.platforms(compiler)

    projectConfigurations  = makeVcxproj.get_project_configurations(platforms)
    propertyConfigurations = makeVcxproj.get_property_configurations(
        platforms, platformToolset)
    importPropertySheets   = makeVcxproj.get_import_property_sheets(platforms)
    propertyGroups         = _get_property_groups(
        platforms, target, makefileTarget, "clean", bin, compiler, includePath)

    data = {"name" : name,
            "tools_version" : toolsVersion,
            "project_file_version" : projectFileVersion}

    data["configFiles"]  = _xmlfiles(configFiles, "ClInclude")
    data["serviceFiles"] = _xmlfiles(serviceFiles, "ClCompile")
    data["projectConfigurations"] = projectConfigurations
    data["propertyConfigurations"] = propertyConfigurations
    data["importPropertySheets"] = importPropertySheets
    data["propertyGroups"] = propertyGroups

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
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
%(propertyConfigurations)s
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <ImportGroup Label="ExtensionSettings">
  </ImportGroup>
%(importPropertySheets)s
  <PropertyGroup Label="UserMacros" />
  <PropertyGroup>
    <_ProjectFileVersion>%(project_file_version)s</_ProjectFileVersion>
%(propertyGroups)s
  </PropertyGroup>
  <ItemDefinitionGroup>
  </ItemDefinitionGroup>
  <ItemGroup>
%(serviceFiles)s
  </ItemGroup>
  <ItemGroup>
%(configFiles)s
  </ItemGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
</Project>
"""


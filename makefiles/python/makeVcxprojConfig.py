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

    opts,args = getopt.getopt(sys.argv[1:], "x:s:c:t:b:I:f:",
                              ["compiler="])
    configPatterns  = ["*.cfg"]
    servicePatterns = ["*.svc"]
    extraSourceDirs = []
    configFilters = []
    kwargs = {}
    includePath = []
    for opt in opts:
        if opt[0] == "-c": configPatterns.append(opt[1])
        elif opt[0] == "-s": servicePatterns.append(opt[1])
        elif opt[0] == "-x": extraSourceDirs.append(opt[1])
        elif opt[0] == "-f": configFilters.append(opt[1])
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
               includePath, compiler, toolsVersion, platformToolset, projectFileVersion,
               extraSourceDirs, configFilters,
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

def write_filters(filename, filters, toolsVersion, guids):
    ffn = filename + ".filters"
    nbFiles = 0
    for groupName,filterName,files in filters: nbFiles += len(files)
    if nbFiles == 0 and os.path.isfile(ffn):
        print("removing %s" % ffn)
        os.remove(ffn)
        return
    
    try:
        fp = open(ffn)
        oldContents = fp.read()
        fp.close()
    except: oldContents = ""

    oldLines = oldContents.split("\n")
    state = None
    uids = {}
    for line in oldLines:
        line = line.strip()
        if line.startswith("<Filter Include="):
            state = line.split('"')[1]
        elif line.startswith("</Filter>"):
            state = None
        elif line.startswith("<UniqueIdentifier>") and state is not None:
            uid = line.split("{")[1].split("}")[0]
            uids[state] = uid

    newLines = []
    newLines.append('<?xml version="1.0" encoding="utf-8"?>')
    newLines.append('<Project ToolsVersion="%s" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">' % toolsVersion)

    started = False
    for groupName,filterName,files in filters:
        if len(files) == 0: continue
        if filterName is None: continue
        uid = guids[filterName]
        if not started:
            newLines.append('  <ItemGroup>')
            started = True
        newLines.append('    <Filter Include="%s">' % filterName)
        newLines.append('      <UniqueIdentifier>{%s}</UniqueIdentifier>' % uid)
        newLines.append('    </Filter>')
    if started: newLines.append('  </ItemGroup>')

    for groupName,filterName,files in filters:
        if len(files) == 0: continue
        newLines.append('  <ItemGroup>')
        if filterName is None:
            for fn in files:
                newLines.append('    <%s Include="%s" />' % (groupName, fn))
        else:
            for fn in files:
                newLines.append('    <%s Include="%s">' % (groupName,fn))
                newLines.append('      <Filter>%s</Filter>' % filterName)
                newLines.append('    </%s>' % groupName)
        newLines.append('  </ItemGroup>')
    newLines.append('</Project>')
    newContents = "\n".join(newLines)

    if oldContents != newContents:
        print(ffn)
        fp = open(ffn, "w")
        fp.write(newContents)
        fp.close()
        if len(oldContents):
            print(ffn + ".bak")
            fp = open(ffn + ".bak", "w")
            fp.write(oldContents)
            fp.close()
    elif os.path.isfile(ffn + ".bak"):
        print("removing", (ffn + ".bak"))
        os.remove(ffn + ".bak")

def make_proj(fileName, name, target, srcDir,
             configPatterns, servicePatterns,
             includePath,
             compiler,
             toolsVersion, platformToolset, projectFileVersion,
             extraSourceDirs,
             configFilters,
             makefileTarget="target", bin=r"C:\cygwin\bin"):

    guids = guidUtils.read_and_write_guids(fileName, ["Project"])
    allConfigFilters = ["Config Files"]
    for cf in configFilters:
        allConfigFilters.append("Config Files\%s" % cf)
    guids.update(guidUtils.read_and_write_guids(
        fileName + ".filters", allConfigFilters,
        gfn = guidUtils.guid_file_name(fileName)))

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
        if len(oldcontents):
            print(fileName + ".bak")
            fp = open(fileName + ".bak", "w")
            fp.write(oldcontents)
            fp.close()
    elif os.path.isfile(fileName + ".bak"):
        print("removing", (fileName + ".bak"))
        os.remove(fileName + ".bak")         
    
    splitConfigFiles = {"": []}
    for cf in configFilters:
        splitConfigFiles[cf] = []
    
    for ffn in configFiles:
        for cf in configFilters:
            scf = "\\%s\\" % cf
            if scf in ffn:
                splitConfigFiles[cf].append(ffn)
                break
        else:
            splitConfigFiles[""].append(ffn)

    filters = [("ClCompile", None, serviceFiles)]
    for cf in splitConfigFiles:
        fns = splitConfigFiles[cf]
        if len(fns):
            nm = "Config Files"
            if len(cf):
                nm += "\\%s" % cf
            filters.append(("ClInclude", nm, fns))
            
    write_filters(fileName, filters, toolsVersion, guids)

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


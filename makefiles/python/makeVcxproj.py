# makes a visual studio project file for VC15 (and above)
#
# naming convention is that functions starting with an underscore should not be called
# outside this module, whereas other functions are being called outside this module

import glob
import os

import vstools
import guidUtils

# interprets the command line (with the standard compiler for the given project type)
# then calls the main routine for this project type
def command_line(compiler, toolsVersion, platformToolset):
    import sys
    import getopt

    opts,args = getopt.getopt(sys.argv[1:], "h:s:c:t:b:H:S:I:j:F:",
                              ["buildSuffix=",
                               "exe=",
                               "makefileTarget=",
                               "cleanTarget=",
                               "silent"])
    headerPatterns  = ["*.hpp", "*.h"]
    sourcePatterns  = ["*.cpp", "*.c"]
    extraSourceDirs = []
    extraHeaderDirs = []
    kwargs          = {}
    includePath     = []
    extraFiles      = []
    for opt in opts:
        if opt[0] == "-h": headerPatterns.append(opt[1])
        elif opt[0] == "-s": sourcePatterns.append(opt[1])
        elif opt[0] == "-H": extraHeaderDirs.append(opt[1])
        elif opt[0] == "-S": extraSourceDirs.append(opt[1])
        elif opt[0] == "-F": extraFiles.append(opt[1])
        elif opt[0] == "-c": compiler = opt[1]
        elif opt[0] == "-t": kwargs["makefileTarget"] = opt[1]
        elif opt[0] == "--buildSuffix": kwargs["buildSuffix"] = opt[1]
        elif opt[0] == "--makefileTarget": kwargs["makefileTarget"] = opt[1]
        elif opt[0] == "--cleanTarget": kwargs["cleanTarget"] = opt[1]
        elif opt[0] == "--exe": kwargs["exeName"] = opt[1]
        elif opt[0] == "--silent": kwargs["silent"] = True
        elif opt[0] == "-I": includePath.append(opt[1])
        elif opt[0] == "-b": kwargs["bin"] = opt[1]
        elif opt[0] == "-j": kwargs["parallel"] = int(opt[1])

    if len(args) != 6:
        print (" ".join(sys.argv))
        raise Exception("Expecting 6 arguments: fileName name srcDir incDir defaultCompiler defaultBits")

    fileName = args[0]
    name     = args[1]
    srcDir   = args[2]
    incDir   = args[3]
    defaultCompiler = args[4]
    defaultBits     = int(args[5])
    
    make_proj(fileName,
              name,
              compiler,
              srcDir,
              incDir,
              includePath,
              headerPatterns,
              sourcePatterns,
              extraHeaderDirs,
              extraSourceDirs, 
              toolsVersion,
              platformToolset,
              defaultCompiler,
              defaultBits,
              extraFiles,
              **kwargs)

def _xmlfiles(filenames, label, filterName=None, indent=4):
    """
    Creates the XML block for representing a bunch of filenames
    """
    if len(filenames) == 0: return []
    indentString = " " * indent
    if filterName is None:
        lines = ['%s<%s Include="%s" />' % (indentString, label, filename)
                 for filename in filenames]
    else:
        lines = ['%s<%s Include="%s">\n%s  <Filter>%s</Filter>\n"%s</%s>' % (
            indentString, label, fileName,
            indentString, filterName,
            indentString, label)
                 for filename in filenames]
    lines.insert(0, "  <ItemGroup>")
    lines.append("  </ItemGroup>")
    return lines

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

def _get_extra_files(extraFilePatterns):
    # should return dict of filter : filenames
    # filter will be the base directory
    # filename will be the full relative path to the file
    # filenames will be a set to ensure uniqueness

    out = {}
    
    for xfp in extraFilePatterns:
        for xf in glob.glob(xfp):
            if os.path.isfile(xf):
                dn = os.path.dirname(xf)
                if dn not in out:
                    out[dn] = set()
                out[dn].add(xf)

    return out

def _get_header_files(headerPatterns, incDir, srcDir, extraHeaderDirs):
    headerFiles = []
    for pattern in headerPatterns:
        headerFiles.extend(glob.glob(os.path.join(incDir,pattern)))
        if srcDir != incDir:
            headerFiles.extend(glob.glob(os.path.join(srcDir,pattern)))
        for dn in extraHeaderDirs:
            headerFiles.extend(glob.glob(os.path.join(dn, pattern)))
    return [os.path.normpath(fn) for fn in headerFiles]

def _get_source_files(sourcePatterns, srcDir, extraSourceDirs):
    sourceFiles = []
    for pattern in sourcePatterns:
        sourceFiles.extend(glob.glob(os.path.join(srcDir, pattern)))
        for dn in extraSourceDirs:
            sourceFiles.extend(glob.glob(os.path.join(dn, pattern)))
    return [os.path.normpath(fn) for fn in sourceFiles]

def get_build_files():
    buildFiles = []
    if os.path.isfile("Makefile"): buildFiles.append("Makefile")
    buildFiles.extend(glob.glob("*.mk"))
    return buildFiles

def _get_item_group_files(sourceFiles, headerFiles, buildFiles, extraFiles):
    itemGroupFiles = []
    itemGroupFiles.extend(_xmlfiles(sourceFiles, "ClCompile"))
    itemGroupFiles.extend(_xmlfiles(headerFiles, "ClInclude"))
    
    noneGroup = buildFiles[:]
    for xfd in extraFiles:
        noneGroup.extend(sorted(extraFiles[xfd]))
    itemGroupFiles.extend(_xmlfiles(noneGroup, "None"))
    return itemGroupFiles

def get_project_configurations(platforms):
    lines = []
    for platform in platforms:
        name = platform[0]
        bits = platform[1]
        for debug in ["Debug", "Release"]:
            lines.append("    <ProjectConfiguration Include=\"%s|%s\">" % (debug, name))
            lines.append("      <Configuration>%s</Configuration>" % debug)
            lines.append("      <Platform>%s</Platform>" % name)
            lines.append("    </ProjectConfiguration>")
    return "\n".join(lines)

def get_property_configurations(platforms, platformToolset):
    lines = []
    for platform in platforms:
        name = platform[0]
        bits = platform[1]
        ## systemIncludes = vstools.systemIncludes(compiler, bits)
        for debug in ["Release", "Debug"]:
            lines.append("  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\" Label=\"Configuration\">" % (debug, name))
            lines.append("    <ConfigurationType>Makefile</ConfigurationType>")
            if platformToolset is not None:
                lines.append("    <PlatformToolset>%s</PlatformToolset>" %
                             platformToolset)
            lines.append("  </PropertyGroup>")
    return "\n".join(lines)

def get_import_property_sheets(platforms):
    lines = []
    for platform in platforms:
        name = platform[0]
        for debug in ["Release", "Debug"]:
            lines.append("  <ImportGroup Condition=\"'$(Configuration)|$(Platform)'=='%s|%s'\" Label=\"PropertySheets\">" % (debug, name))
            lines.append("    <Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />")
            lines.append("  </ImportGroup>")
    return "\n".join(lines)

def get_abi(compiler, bits, debug, defaultCompiler, defaultBits):
    if bits != 32 and bits != 64:
        raise Exception("bits (%s) must be 32 or 64" % bits)
    if debug != "Debug" and debug != "Release":
        raise Exception("debug (%s) must be Debug or Release" % debug)
    if compiler == defaultCompiler and bits == defaultBits:
        abi = debug
    elif compiler == defaultCompiler:
        abi = "%s-%s" % (debug, "x64" if bits == 64 else "x86")
    else:
        abi = "%s-%s-%s" % (debug, "x64" if bits == 64 else "x86", compiler.replace("msvc", "vc"))
    return abi

def _get_property_groups(platforms, makefileTarget, cleanTarget, compiler, defaultCompiler, defaultBits,
        bin, buildSuffix, includePath, parallel, exeName, silent):

    vsIncludePath = []
    for include in includePath:
        if include == ".":
            vsIncludePath.append("$(MSBuildProjectDirectory)")
        elif ":" in include: # absolute path
            vsIncludePath.append(os.path.normpath(include))
        else:
            vsIncludePath.append("$(MSBuildProjectDirectory)\\%s" % os.path.normpath(include))
    if len(vsIncludePath): vsIncludePath.append("")
    vsIncludePath = ";".join(vsIncludePath)

    lines = []
    make = "make -s" if silent else "make"
    parallelMake = make if parallel <= 1 else ("%s -j%d" % (make, parallel))
    for platform in platforms:
        name = platform[0]
        bits = platform[1]
        systemIncludes = vstools.systemIncludes(compiler, bits)
        for debug in ["Debug", "Release"]:
            debugFlag = " DEBUG=1" if debug == "Debug" else ""
            dn = "win%s\\%s" % (bits, debug)
            condition = "'$(Configuration)|$(Platform)'=='%s|%s'" % (debug,name)
            lines.append("    <OutDir Condition=\"%s\">%s\\</OutDir>" % (
                    condition, dn))
            lines.append("    <IntDir Condition=\"%s\">%s\\</IntDir>" % (
                    condition, dn))
            lines.append("    <NMakeBuildCommandLine Condition=\"%s\">%s %s VS_BUILD=1 COMPILER=%s BITS=%s%s</NMakeBuildCommandLine>" % (
                    condition, parallelMake, makefileTarget, compiler, bits, debugFlag))
            lines.append("    <NMakeReBuildCommandLine Condition=\"%s\">%s %s VS_BUILD=1 COMPILER=%s BITS=%s%s &amp;&amp; %s %s VS_BUILD=1 COMPILER=%s BITS=%s%s</NMakeReBuildCommandLine>" % (
                    condition, make, cleanTarget, compiler, bits, debugFlag,
                    parallelMake, makefileTarget, compiler, bits, debugFlag))
            lines.append("    <NMakeCleanCommandLine Condition=\"%s\">%s %s VS_BUILD=1 COMPILER=%s BITS=%s%s</NMakeCleanCommandLine>" % (
                    condition, make, cleanTarget, compiler, bits, debugFlag))
            lines.append("    <ExecutablePath Condition=\"%s\">%s;$(ExecutablePath)</ExecutablePath>" % (condition, os.path.normpath(bin)))
            if len(vsIncludePath):
                lines.append("    <NMakeIncludeSearchPath>%s%s</NMakeIncludeSearchPath>" % (
                        vsIncludePath, systemIncludes))

    if exeName:
        for platform in platforms:
            name = platform[0]
            bits = platform[1]
            for debug in ["Debug", "Release"]:
                abi = get_abi(compiler, bits, debug, defaultCompiler, defaultBits)
                condition = "'$(Configuration)|$(Platform)'=='%s|%s'" % (debug,name)
                lines.append("  </PropertyGroup>")
                lines.append("  <PropertyGroup Condition=\"%s\">" % condition)
                lines.append("    <NMakeOutput>%s</NMakeOutput>" % (exeName.replace("G_ABI", abi)))

    return "\n".join(lines)

def make_proj(fileName, name, compiler, srcDir, incDir,
             includePath,
             headerPatterns, sourcePatterns,
             extraHeaderDirs, extraSourceDirs,
             toolsVersion,
             platformToolset,
             defaultCompiler,
             defaultBits,
             extraFilePatterns,
             buildSuffix="",
             makefileTarget="target",
             cleanTarget="clean",
             bin=r"C:\cygwin\bin",
             exeName=None,
             parallel=4,
             silent=False):

    extraFiles = _get_extra_files(extraFilePatterns)
    headerFiles = _get_header_files(headerPatterns, incDir, srcDir, extraHeaderDirs)
    sourceFiles = _get_source_files(sourcePatterns, srcDir, extraSourceDirs)

    filterNames = set()
    if len(headerFiles):
        filterNames.add("Header Files")
    if len(sourceFiles):
        filterNames.add("Source Files")
    for xfdn in extraFiles:
        filterNames.add(xfdn)

    guids = guidUtils.read_and_write_guids(fileName, ["Project"])
    
    if len(filterNames):
        # import pprint
        # pprint.pprint(filterNames)
        # pprint.pprint(extraFiles)
        guids.update(guidUtils.read_and_write_guids(
            fileName + ".filters",
            sorted(filterNames),
            gfn = guidUtils.guid_file_name(fileName)))

    buildFiles  = get_build_files()

    itemGroupFiles = _get_item_group_files(
        sourceFiles, headerFiles, buildFiles, extraFiles)

    platforms = vstools.platforms(compiler)

    projectConfigurations  = get_project_configurations(platforms)
    propertyConfigurations = get_property_configurations(
        platforms, platformToolset)
    importPropertySheets   = get_import_property_sheets(platforms)
    propertyGroups         = _get_property_groups(
        platforms, makefileTarget, cleanTarget, compiler, defaultCompiler, defaultBits,
        bin, buildSuffix, includePath, parallel, exeName, silent)

    data = {"name" : name,
            "tools_version" : toolsVersion}

    data["itemGroupFiles"] = "\n".join(itemGroupFiles)
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

    filters = []
    if len(sourceFiles):
        filters.append(("ClCompile", "Source Files", sourceFiles))
    if len(headerFiles):
        filters.append(("ClInclude", "Header Files", headerFiles))
    if len(buildFiles):
        filters.append(("None", None, buildFiles))
    for xfd in extraFiles:
        filters.append(("None", xfd, sorted(extraFiles[xfd])))
        
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
%(itemGroupFiles)s
  <Import Project="$(VCTargetsPath)\\Microsoft.Cpp.targets" />
</Project>
"""


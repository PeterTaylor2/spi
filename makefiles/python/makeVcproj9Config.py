# makes a visual studio project file for VS9 for config files

import uuid
import glob
import os

def xmlfiles(filenames, tabs=3):
    """
    Creates the XML block for representing a bunch of filenames
    """
    indentString = "\t" * tabs
    lines = ['%s<File\n%s\tRelativePath="%s"\n%s\t>\n%s</File>' % (
        indentString, indentString, filename, indentString, indentString)
             for filename in filenames]
    return "\n".join(lines)

def makeProj(fileName, name, target, srcDir, configPatterns, servicePatterns,
             makefileTarget="vcproj all"):

    configFiles  = []
    serviceFiles = []
    for pattern in configPatterns:
        configFiles.extend(glob.glob(os.path.join(srcDir,pattern)))
    for pattern in servicePatterns:
        serviceFiles.extend(glob.glob(os.path.join(srcDir, pattern)))

    configFiles  = [os.path.normpath(fn) for fn in configFiles]
    serviceFiles = [os.path.normpath(fn) for fn in serviceFiles]
    if os.path.isfile("Makefile"): serviceFiles.append("Makefile")

    data = {"name" : name,
            "target" : target,
            "makefile_target" : makefileTarget}

    data["configFiles"]  = xmlfiles(configFiles)
    data["serviceFiles"] = xmlfiles(serviceFiles)

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

# we will try to exactly match the tabs and unnecessary values of the
# Microsoft project because Visual Studio 2008 is forever modifying this
# file during a session back to its default layout

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
		<Platform
			Name="Win32"
		/>
	</Platforms>
	<ToolFiles>
	</ToolFiles>
	<Configurations>
		<Configuration
			Name="Debug|Win32"
			OutputDirectory="$(SolutionDir)$(ConfigurationName)"
			IntermediateDirectory="$(ConfigurationName)"
			ConfigurationType="0"
			>
			<Tool
				Name="VCNMakeTool"
				BuildCommandLine="make %(makefile_target)s DEBUG=1"
				ReBuildCommandLine="make clean &amp;&amp; make %(makefile_target)s DEBUG=1"
				CleanCommandLine="make clean"
				Output="$(ProjectDir)%(target)s"
				PreprocessorDefinitions=""
				IncludeSearchPath=""
				ForcedIncludes=""
				AssemblySearchPath=""
				ForcedUsingAssemblies=""
				CompileAsManaged=""
			/>
		</Configuration>
		<Configuration
			Name="Release|Win32"
			OutputDirectory="$(SolutionDir)$(ConfigurationName)"
			IntermediateDirectory="$(ConfigurationName)"
			ConfigurationType="0"
			>
			<Tool
				Name="VCNMakeTool"
				BuildCommandLine="make %(makefile_target)s"
				ReBuildCommandLine="make clean &amp;&amp; make %(makefile_target)s"
				CleanCommandLine="make clean"
				Output="$(ProjectDir)%(target)s"
				PreprocessorDefinitions=""
				IncludeSearchPath=""
				ForcedIncludes=""
				AssemblySearchPath=""
				ForcedUsingAssemblies=""
				CompileAsManaged=""
			/>
		</Configuration>
	</Configurations>
	<References>
	</References>
	<Files>
		<Filter
			Name="Service Files"
			>
%(serviceFiles)s
		</Filter>
		<Filter
			Name="Configuration Files"
			>
%(configFiles)s
		</Filter>
	</Files>
	<Globals>
	</Globals>
</VisualStudioProject>
"""

if __name__ == "__main__":
    import sys
    import getopt

    opts,args = getopt.getopt(sys.argv[1:], "s:c:t:")
    configPatterns  = ["*.cfg"]
    servicePatterns = ["*.svc"]
    kwargs = {}
    for opt in opts:
        if opt[0] == "-c": configPatterns.append(opt[1])
        elif opt[0] == "-s": servicePatterns.append(opt[1])
        elif opt[0] == "-t": kwargs["makefileTarget"] = opt[1]

    if len(args) != 4:
        print(" ".join(sys.argv))
        raise Exception("Expecting 4 arguments: fileName name srcDir target")

    fileName = args[0]
    name     = args[1]
    srcDir   = args[2]
    target   = args[3]

    makeProj(fileName, name, target, srcDir, configPatterns, servicePatterns,
             **kwargs)


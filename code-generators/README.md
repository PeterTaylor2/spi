
# code-generators

SPI works using code-generators to generate user code for providing all the specific
functionality for a service.

This document shows the purpose of each code generator followed by the displaying the
help message for that executable. The help message is seen by running the code-generator
with the --help option.            

## SPCL


SPCL is the code generator that parses the Service and API files and generates the
code for the C++ layer.
It also creates the .svo file that is consumed by subsequent code generators.
Note that the .svo files are written in the SPI-text format (which has a first line
of <spi>).

Note that of the code generators, only SPCL actually reads the Service and API files.
All the others read the corresponding .svo file.
```
USAGE: spcl.exe [-w] [-t tdirname] [-v] [-d] [-S sfilename] [longOptions] infile outfile dirname
where longOptions can be as follows:
	--help
	--allHeader
	--noVerbatimLine
	--noHeaderSplit
	--noTidyup
	--noGeneratedCodeNotice
	--sortSummary
	--version=<version>
	--optionalArgs
	--license
	--outputDir=<outputDir>
	--sessionLogging
	--licenseFile=<licenseFile>
	--backup
	--checkNonNull
	--textFormat

Help:
	-h or --help
		Print this help message and exit
	--allHeader
		Setting this flag means that we create a header file which includes all the other header files.
		This might be easier to use but will increase the dependencies.
	--noVerbatimLine
		Do not print the #line directives in the generated code
	--noHeaderSplit
		By default you get a header file for the classes defined by a module and a separate
		header file for the functions. This can help reduce the dependencies when compiling.
		Setting noHeaderSplit reverses this behaviour - the two header files are combined into one.
	--noTidyup
		By default after generating the code then any files in the generated code directory
		which were not created by the generator will be removed. Setting noTidyup reverses this behaviour.
	--noGeneratedCodeNotice
		Do not print the generated code notice at the top of each file in the generated code
		In addition we also trigger noVerbatimLine option
	--sortSummary
		If the summary file is requested (-S option) then the contents will be sorted.
	--version = <version>
		Usually the version number is defined in the .svc file. If you don't define it in that file, then the
		default value is 1.0.0.0. By using the --version option you are changing the default value.
		The value defined in the .svc object (if set) still takes precedence.
	--optionalArgs
		In the C++ code generated support optional arguments.
	--license
		Prints the license for SPCL.
	--outputDir = <outputDir>
		Defines the output directory where we will write the binaries.
		In the makefiles this is defined as U_OUTPUT_DIR and there is a default location
		defined in the makefile fragments defined by SPI.
	--sessionLogging
		Insert session logging commands into the generated code
	--licenseFile = <licenseFile>
		This is the name of a file which contains the license for your code.
		The file will be read and the contents included near the top of the generated files.
	--backup
		Creates a backup file (original name with .bak appended) whenever a file is changed.
	--checkNonNull
		Will add checks that an output object is not null.
		Having set this flag you can avoid the check by marking the output as optional.
		Ideally this should be the default behaviour (i.e. checkNonNull) but for backward
		compatibility we kept the previous behaviour since it might break too much code to
		add this check to existing generated code.
	--textFormat
		Do we write the generated files in text format (specific to the platform).
		If undefined, then the files will be written with Unix line endings.
	-w
		Waits at the start - the purpose is to allow a debugger to be attached
	-t <tdirname>
		Generate types only code. What this does is that it generates code only for the
		class definitions. Functions and class methods are not included.
		The code is generated in the directory defined by the option. The
		regular code is generated at the same time.
	-v
		Verbose - shows some details of the parser
	-d
		Generate the autodoc - only one argument - the output file which should end with .tex
	-S <sfilename>
		Creates a summary file of the contents of the service
```

## SPPY


SPPY is the code generator that creates code written in C++ using the C-Python API.
At present the code generated needs to be linked against the header files and libraries
for the version of Python that you are targetting for use.

Optionally we also generate a python script which can be used to import the library
using the namespace of the service.

Pre-requisite: SPCL
```
USAGE: sppy.exe [-w] [-v] [longOptions] infile outfile dirname
where longOptions can be as follows:
	--help
	--noImporter
	--keywords
	--lowerCase
	--lowerCaseMethod
	--objectCoerce
	--helpFuncList
	--fastcall
	--textFormat
	--backup
	--noGeneratedCodeNotice
	--license
	--licenseFile=<licenseFile>

Help:
	-h or --help
		Print this help message and exit
	--noImporter
		Do not generate (and hence don't overwrite) the importer script with the name <ns>.py
		where <ns> is the namespace of the library
	--keywords
		Support keyword arguments to functions
	--lowerCase
		Convert all function names to lower-case
	--lowerCaseMethod
		Convert all member function names to lower-case
	--objectCoerce
		Provides a service level object_coerce function. Note that each class already supports
		a Coerce method so the service level object_coerce function is not really needed.
	--helpFuncList
		Provides a service level help_func_list function. Not really necessary in the Python
		context since there are so many standard Python methods of gathering this information
	--fastcall
		An attempt to use a faster method of calling functions - didn't seem to make any difference.
		hence we don't particularly recommend using this option since we haven't tested it fully.
	--textFormat
		Not sure what this means
	--backup
		Creates a backup file (original name with .bak appended) whenever a file is changed.
	--noGeneratedCodeNotice
		Do not print the generated code notice at the top of each file in the generated code
		In addition we also trigger noVerbatimLine option
	--license
		Prints the license for SPCS.
	--licenseFile = <licenseFile>
		This is the name of a file which contains the license for your code.
		The file will be read and the contents included near the top of the generated files.
	-w
		Waits at the start - the purpose is to allow a debugger to be attached
	-v
		Verbose - shows some details of the parser
```

## SPXL


SPXL is the code generator that creates code written in C++ using the Excel C-API.

Pre-requisite: SPCL
```
USAGE: spxl.exe [-w] [-v] [longOptions] infile outfile dirname indirvba
where longOptions can be as follows:
	--help
	--optionsFile=<optionsFile>
	--nameAtEnd
	--noObjectFuncs
	--noPrefixObjectFuncs
	--parent=<parent>
	--errIsNA
	--xlTargetVersion=<xlTargetVersion>
	--nsUpperCase
	--upperCase
	--funcNameSep=<funcNameSep>
	--backup
	--noGeneratedCodeNotice
	--license
	--licenseFile

Help:
	-h or --help
		Print this help message and exit
	--optionsFile = <optionsFile>
		Defines an options file written in JSON which defines various options.
		If you want to change the names of the functions that are written on your behalf for manipulating
		objects in a generic manner, then the optionsFile approach is the one that you should be using.

		These options are available with the optionsFile: funcNameSep, noGeneratedCodeNotice, nameAtEnd,
		upperCase, nsUpperCase, noObjectFuncs, errIsNA, xlVersion.
		These options can also be specified independently on the command line.

		These options can only be defined via the optionsFile - they define the names for particular functions
		that are generated for all services:
		helpFunc, hepFuncList, helpEnum, objectCoerce, startLogging, stopLogging, startTiming, stopTiming,
		clearTimings, getTimings, setErrorPopups, objectToString, objectFromString, objectGet, objectPut,
		objectToFile, objectFromFile, objectCount, objectFree, objectFreeAll, objectList, objectSHA
	--nameAtEnd
		Functions which return objects in the Excel API are returned as strings to the
		spreadsheet. You need to provide a name which will form part of this string - the so-called
		baseName. By default, the name is required as the first parameter, but it is actually more
		natural to have the baseName as the final parameter. Initially (i.e. when the SPI project was started)
		defining the name at the end had the problem that if you added extra (hopefully optional) parameters
		over time, then existing Excel spreadsheets would now be providing the base name of the object in the
		position where we expected the new function input. Essentially this would break the Excel spreadsheet.
		However we solved this problem - the key is that if the name is defined at the end, then we assume
		that the last parameter entered is the baseName, and that any missing parameters are assumed to take
		their default value. Hence we recommend turning on the nameAtEnd feature.
	--noObjectFuncs
		If defined, then we do not define any of the object functions, e.g. objectToString
		etc. See the optionsFile parameter for a list of the possible object functions.
		You might set this flag if another of the Excel add-ins contains all the object functions.
	--noPrefixObjectFuncs
		If defined, then we do not use the namespace prefix for the object functions.
	--parent = <parent>
		This option is deprecated and ignored
	--errIsNA
		Use this option to set error results as #N/A as opposed to the default which is #NUM!
	--xlTargetVersion = <xlTargetVersion>
		Defines which version of Excel we are targetting. By default 12.
		Other possible values are 4 and 15, but 4 is now really old-fashioned and somewhat restrictive.
	--nsUpperCase
		The namespace component of the name is converted to upper case.
	--upperCase
		Convert all function names entirely to upper case.
	--funcNameSep = <funcNameSep>
		Defines the separator that is used join together parts of the function name.
		The default value is '.' - one which we have seen quite often used is '_'
	--backup
		Creates a backup file (original name with .bak appended) whenever a file is changed.
	--noGeneratedCodeNotice
		Do not print the generated code notice at the top of each file in the generated code
		In addition we also trigger noVerbatimLine option
	--license
		Prints the license for SPCS.
	--licenseFile
		This is the name of a file which contains the license for your code.
	-w
		Waits at the start - the purpose is to allow a debugger to be attached
	-v
		Verbose - this appears to have no effect
```

## SPC


SPC is the code generator that write C++ code with a C interface for a library designed
to be used from P/INVOKE in a .NET context.
In particular it designed for using within the C# code that is generated by SPCS.

Since the code generated uses specialised memory management functions it is not
really suitable for any other purpose than to be called from the C# interface.

Pre-requisite: SPCL
```
USAGE: spc.exe [-w] [-v] [longOptions] infile outfile dirname
where longOptions can be as follows:
	--help
	--import=<import>
	--satellite=<satellite>
	--noGeneratedCodeNotice
	--license
	--licenseFile=<licenseFile>
	--backup

Help:
	-h or --help
		Print this help message and exit
	-i <import> or --import = <import>
		You can define one or more imports. We will then include the c_dll_<import>.hpp file for
		any higher level product (with distinct namespace) that this project relies upon.
	-s <satellite> or --satellite = <satellite>
		You can define one or more satellites. Satellites are products with the same
		namespace, but you need to be able to define a common start-up routine and shutdown routine.
	--noGeneratedCodeNotice
		Do not print the generated code notice at the top of each file in the generated code
		In addition we also trigger noVerbatimLine option
	--license
		Prints the license for SPCL.
	--licenseFile = <licenseFile>
		This is the name of a file which contains the license for your code.
		The file will be read and the contents included near the top of the generated files.
	--backup
		Creates a backup file (original name with .bak appended) whenever a file is changed.
	-w
		Waits at the start - the purpose is to allow a debugger to be attached
	-v
		Verbose - shows some details of the parser
```

## SPCS


SPCS is the code generator that writes C# code which we do not actually compile.
The C# code calls into the C-library created by SPC using P/INVOKE.
This method should work for all platforms that support C#.

The code is provided and it is up to the client software to compile the C# code.
The minimum requirement is that you should target at least version 8 of .NET due to
the use of nullable values.

Previously we created code using C++/CLI (common language interface) but it appears
that C++/CLI has not been ported to Linux (and there are no plans to do so).
Hence we decided to use P/INVOKE approach for wider compatibility.

Pre-requisite: SPC
```
USAGE: spcs.exe [-x exclusion] [-w] [-v] [longOptions] infile outfile dirname
where longOptions can be as follows:
	--help
	--import=<import>
	--satellite=<satellite>
	--backup
	--noTidyup
	--csNamingStyle
	--nullable
	--noGeneratedCodeNotice
	--license
	--licenseFile=<licenseFile>

Help:
	-h or --help
		Print this help message and exit
	-i <import> or --import = <import>
		You can define one or more imports. We will then ensure that this higher level
		product is initialised by calling its version function!
	-s <satellite> or --satellite = <satellite>
		You can define one or more satellites. Satellites are products with the same
		namespace, but you need to be able to define a common start-up routine and shutdown routine.
	-x <exclusion>
		Defines extra keywords that you need to exclude. C# variables cannot be keywords in C#.
		Since there are more keywords than we know about, the idea is that if some C# code
		fails to compile since we are using a keyword badly, then on your next run you would
		define that keyword with the -x flag. The field name is then changed by adding an
		underscore to the end of the field name.
	--backup
		Creates a backup file (original name with .bak appended) whenever a file is changed.
	--noTidyup
		By default after generating the code then any files in the generated code directory
		which were not created by the generator will be removed. Setting noTidyup reverses this behaviour.
	--csNamingStyle
		If defined, then C# field names use the standard C# naming style.
		This involves capitalizing the first character in the string.
	--nullable
		If defined, then we will generate code such that optional fields defined in the API
		will be marked as nullable in the C# code.
	--noGeneratedCodeNotice
		Do not print the generated code notice at the top of each file in the generated code
		In addition we also trigger noVerbatimLine option
	--license
		Prints the license for SPCS.
	--licenseFile = <licenseFile>
		This is the name of a file which contains the license for your code.
		The file will be read and the contents included near the top of the generated files.
	-w
		Waits at the start - the purpose is to allow a debugger to be attached
	-v
		Verbose - shows some details of the parser
```

## SPTEX


SPTEX is the LaTeX file generator that creates .tex files which can be combined together
to produce a user guide.

Pre-requisite: SPCL
```
USAGE: sptex.exe [-S sfilename] [-w] [-v] [longOptions] infile outfile dirname
where longOptions can be as follows:
	--help
	--import
	--writeIncludes
	--backup
	--license

Help:
	-h or --help
		Print this help message and exit
	-i or --import
		You can define one or more imports. The import files will be used to define types
		used by this service but created by one of the imported services (higher-level services).
	--writeIncludes
		Writes the location of the corresponding header file for any function or class.
	--backup
		Creates a backup file (original name with .bak appended) whenever a file is changed.
	--license
		Prints the license for SPTEX.
	-S <sfilename>
		Writes a summary file.
	-w
		Waits at the start - the purpose is to allow a debugger to be attached
	-v
		Verbose - this appears to have no effect
```

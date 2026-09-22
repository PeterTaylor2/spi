# spi/makefiles directory

This directory contains a set of rules to build a software product built using
SPI (Sartorial Programming Interface).

We are a bit old-fashioned - we use gnu make (almost throughout) rather than
Visual Studio project files or cmake or anything else really!

For Visual Studio users we create visual studio project files which use make
to build each directory. Given that we are in the Microsoft world the option
is stated to use NMAKE, but fortunately you can define the commands yourself
and we use make instead.

## spi/makefiles/cygwin64 directory

Since make is not a standard command in Windows we provide a directory of
linux-like commands using a product called cygwin. Rather than expecting you
to download the whole of cygwin onto your computer (which might be difficult
in a corporate environment), we instead provide a cut down version of cygwin
commands - sufficient to run all the commands we use in our makefiles. These
commands are in the directory spi/makefiles/cygwin64/bin. These are a fairly
up-to-date versions of the 64-bit version of cygwin - but they do the job we need.
For each Visual Studio project we create we will put this directory into the
path.

For other platforms (linux and mac-os) you will be expected to have downloaded
the packages which provide compilers and build tools including make.

## spi/makefiles/gendep directory

The gnu c++ compiler provides the ability with the command line options -MM
and -MP to create dependency files with the prefix ".d".
The dependency file enables you to detect dependencies on #include files,
and thus you can trigger a rebuild of a source file if one of its dependent
header files has changed.

As far as we know there is no simple equivalent using Visual Studio compilers.
Hence as a workaround we have created a fairly simple executable called
gendep.exe and provided the source code for it in the spi/makefiles/gendep
directory. Every time we compile a file using Visual Studio, we also run
gendep.exe on the file with the same include path. This will then create the
same style of ".d" files for the source file.

## directories created during the build process

When compiling code we will compile the code to the G_BUILD_DIR directory
where G_BUILD_DIR is a macro defined for the combination of the platform,
compiler and build architecture. For example, G_BUILD_DIR might be
win64/msvc17_release or linux64/gcc_debug etc. The directory defined by
G_BUILD_DIR is created automatically (if it doesn't already exist) by the
makefiles.

When gathering the final output into a convenient location we will often use
the directory defined by G_ABI. For example, G_ABI mighr be Release or
Debug-linux64 (corresponding to the two G_BUILD_DIR examples defined above).
It is important that we can build to different directories for Windows and
Linux since it is possible that we are using the same file system (e.g. via
WSL - Windows Subsystem for Linux).

Note that sometimes there is a problem on Windows when compiling in parallel
in debug mode for the first time (i.e. before the directory has been created).
We are trying to create a .pdb file during the parallel build and this can cause
a problem. There is an option (which we use) to allow each individual file
compilation to update the .pdb file safely. However, what I think is happening
is that the new directory is not created 'quickly enough' and hence the .pdb file is
not in place when you try to update it from many different threads. In practice,
the second build n such cases is usually successful.

## local configuration

Since we are using make and not Visual Studio project files we need to know
which directories we should search for binaries (the compiler) and libraries.
We have a guess for these in the file msvc_version.mk but this probably
doesn't match your environment. This is because msvc_version.mk is a snapshot
in time and we cannot be sure which version of the compiler you have installed.
There are two different ways to configure locally.

The first method involves the spi/makefiles/config/site.mk file. We don't
provide this file but we do try to include it. This file is designed to have
site-specific configuration unique for all users in a build system which is
using SPI. Typically you would put a redirection to some other directory
which is outside SPI directory system and that would contain the site specific
rules. You can also then define separate configuration rules by computer name.

This is one example of how you might do this:
```
-include $(U_SPI_HOME)/../makefiles/site.mk
-include $(U_SPI_HOME)/../makefiles/config-$(COMPUTERNAME).mk
```

Here is an example of some of the contents of the ../makefiles/site.mk file referred
to above:
```
G_WIN64_PY_VERSION:=312
G_WIN64_PY_VERSIONS:=39 312

G_SPCL_OPTIONS+=--backup
G_SPXL_OPTIONS+=--backup
G_SPPY_OPTIONS+=--backup
G_SPC_OPTIONS+=--backup
G_SPCS_OPTIONS+=--backup
G_SPTEX_OPTIONS+=--backup

G_LINUX64_SANITIZER_FLAGS=-fsanitize=address
G_MACOS64_SANITIZER_FLAGS=-fsanitize=address
```
Notice that we definedparameters for windows, linux and MacOS all in the same
configuration file. We also defined ome globally defined options for the different
code generators.
In this case we are defining the --backup option for all the code generators
which creates a backup of any source file which is about to be changed by
that invocation of the code generator.

The second method is more *ad hoc* and involves the spi/makefiles/config/computers
directory. We do not provide this directory (except perhaps for a README.txt
file). Entries in this directory should have the name $(COMPUTERNAME).mk
where COMPUTERNAME is the environment variable describing the computer.

We provide a script in spi/makefiles/python/vsVersions.py. If you run this
script, then it scans the file system and attempts to discover the variables
that you need to define for the makefile system to find the compilers and
libraries.

Note that COMPUTERNAME is only defined by default in Windows. If you are using
Linux or Mac-OS then you will need to define this variable at startup
yourself. For example, you could define it in your .bashrc file.
At one point we tried to use the HOSTNAME variable, but it turned out that
this was a shell variable and not an environment variable - and hence the
makefiles did not see $(HOSTNAME). You can confirm this for yourself
```
echo $HOSTNAME
```
in the shell usually gives a result, but
```
env | grep HOSTNAME
```
will not show anything.

## Other directories of interest.

### spi/makefiles/config

All the general configuration rules for each compiler.
Usually we have one file for general purposes (e.g. linux64.mk) and then a file
for the compiler (e.g. linux64_gcc.mk). Then another file which gives the differences
between debug and release builds (e.g. linux64_gcc_release.mk and linux64_gcc_debug.mk).

We don't distinguish between different versions of gcc - but we do distinguish between
different versions of the MSVC compilers.

### spi/makefiles/build

Directory for defining build commands for different types of targets.
For example, exe.mk is used for building executables. These are quite general
and not specific to SPI.

### spi/makefiles/test

This directory contains some really simple makefiles designed for testing the
makefile system itself. There are three different versions of "Hello World"
applications.

### spi/makefiles/spi

This directory contains rules for using SPI as a code generator and for
building the shared libraries and executables provided as part of SPI.

### spi/makefiles/python

This directory contains python scripts used as part of the build process.

### spi/makefiles/cversion

This directory contains a simple executable for displaying the version of
the compiler that you are using.

### spi/makefiles/regression-test

This directory contains files for helping you write regression tests using
python.

## Other topics

We try to keep the makefile contents as platform neutral as possible.
However, there is a major difference in how we link the libraries.
On Windows, we will tend to use the variable U_DEP_LIBS to indicate
the dependent librarie, and with make's precedence rules this will mean
we compile the executable (or shared library) whenever one of the dependent
libraries has changed.

On Windows we can usually set U_LIBS equal to U_DEP_LIBS.

However, for Linux and MacOS we need to define a different syntax for U_LIBS.
Typically this has
```
-Ldirectory_name
```
for the directory which contains the libraries, and
```
-llibrary_name
```
for each library.
To determine which to use you can use the variable G_PLATFORM which will either be
win32 or linux64 or macos64 as currently configured.

## Anything else

If there is something unclear then please ask and perhaps we can add more information
to this README.




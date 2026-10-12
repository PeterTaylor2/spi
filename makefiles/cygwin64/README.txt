In the cygwin64/bin directory we include various cygwin 64-bit executables and shared libraies
that are the minimum necessary for running build commands from inside Visual Studio.

Since we use linux style makefiles (rather than Microsoft build tools) we need to be able to run
a sub-set of linux commands to get the build to work out of the box.

This copy of the cygwin64 binaries is for convenience - you can replace them with your own
versions of cygwin64 (or indeed any other toolset which supports the commands in the bin
directory).

According to licensing rules we need to provide you with a method of getting the source code
for these binaries.

The web site www.cygwin.com is the website which gives you access to the source code. They provide
links to the git repositories in use (https://www.cygwin.com/git.html).

The file file-packages.csv indicates which package was used for the each file in the bin directory.

The file package-sources.csv indicates where you can find the source code for each package.

In general cygwin is covered by GPL and LGPL licenses.
See the files GPL-LICENSE and LGPL-LICENSE for details.


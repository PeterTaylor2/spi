# 3rd Party directory

The 3rdParty directory contains files from other open source libraries needed by SPI.
The rule is that we copy the code into 3rdParty (and provide instructions on how to do it),
but that we compile the code with the SPI build system based on makefiles.

If we refer to a filename or directory in these notes, then it refers to a file or directory
relative to the root directory of the SPI installation.

## libcurl

On Windows we compile libcurl ourselves. As far as I can tell there is no libcurl bundled with the
Windows operating system - otherwise we would use that copy. The curl application on Windows appears
to be statically linked to libcurl, but nevertheless you can run "curl -V" to discover the version
number of the underlying version of libcurl.

Our plan is to shadow that version of libcurl - although of course we might not always notice that
the Windows version of curl has changed.

For Linux and MacOS operating systems, libcurl is typically installed via the package updating system.
We will use those versions of libcurl, and make no attempt to build the software ourselves.

The process for getting a new version of libcurl is to fetch the zip file (or tar.gz file) from
www.github.com/curl/curl/tags directory. Extract all the code - typically this will create a folder
called curl-curl-V1-V2-V3 where V1, V2 and V3 are the version numbers. Then you can either run the
script copyManifest.py (in the directory makefiles/python) or else type "make -f copy-curl.mk"
which will provide instructions on how to use the copy-curl.mk makefile. You will of course need
to have make in your path in order to undertake the latter step. If it is not in your path, then
you can add the makefiles/cygwin64/bin directory to your path.

If you want to call copyManifest directly, then the command to use is as follows (from this directory):

```
    python ../makefiles/python/copyManifest.py curl.man curl-curl-V1-V2-V3 curl-V1.V2.V3
``` 

Of course python must be in your path - perhaps it is easier to use py instead on a Windows machine.

## zlib

The intention is to use the same mechanism for zlib - but this has not been implemented yet.

## boost

We do not intend to use this mechanism for our use of the boost libraries. The full boost library distribution
is massive - instead we cut it down to the minimal files necessary to support the main shared pointer classes.
These are shared\_ptr, shared\_array and intrusive\_ptr. In order to distinguish these packages from another
version of boost installed for the client libraries we have renamed the root directory as spi\_boost and changed
the namespace to be spi\_boost throughout instead of boost.

Hence this code lives in the spi\_boost directory.


# definitions for building 64-bit libraries and applications on Windows
# using the VC15 compiler

G_ABI=Release-x64-vc15
G_BUILD_DIR=win64/msvc15_release

ifeq ($(G_WIN32_COMPILER),msvc15)
G_ABI=Release
endif

include $(U_MAKEFILES)/config/win64_msvc15.mk

###########################################################################
# for C/C++ compilation we need to define the following:
#
# G_CFLAGS G_CPPFLAGS G_SYS_INCLUDES
# 
# since we set the system includes by defining INCLUDE we can skip setting
# G_SYS_INCLUDES
###########################################################################

I_DEFINES = -D_CRT_SECURE_NO_WARNINGS $(U_DEFINES) $(U_WIN64_RELEASE_CFLAGS)
I_CFLAGS = -c -MD -W3 -Gy -nologo -O2 $(I_DEFINES)

# if we want multi-character byte set then add -D_MCBS

ifdef U_CLR

# we need to use -clr flag plus the 2.0 version of the .NET framework files
# also we must force use of C++ for C files

G_CPPFLAGS = $(I_CFLAGS) -EHa -clr $(G_CLR_USING)
G_CFLAGS = /TP $(G_CXX_CFLAGS)

else

# /arch:SSE2 unknown option

G_CFLAGS = $(I_CFLAGS) $(U_WIN64_CFLAGS)
G_CPPFLAGS = $(I_CFLAGS) -EHsc $(U_WIN64_CPPFLAGS)

endif

G_MSBUILD_PROPERTIES=/p:Configuration=Release /p:Platform=x64

############################################################################
# Linker flags - taken from a new visual studio project.
#
# /INCREMENTAL:NO Do not use incremental linking.
# /OPT:REF Removes unused data and functions.
# /OPT:ICF Number of iterations to use when searching for unused objects.
#          Default is equal to 2.
#
# Extra stuff from past experience:
#
# /NODEFAULTLIB To avoid problems with system library files.
# /FIXED:NO     This is standard behaviour for DLLs. Apparently for EXEs
#               FIXED:NO helps purify.
# /VERBOSE:LIB  If you want to see the files processed during the link phase
############################################################################
G_OPTIMIZE_LFLAGS = /OPT:REF /OPT:ICF
I_LFLAGS = /profile /incremental:no /nodefaultlib /fixed:no $(G_OPTIMIZE_LFLAGS) $(U_WIN64_RELEASE_LFLAGS)

G_DLL_LFLAGS = $(I_LFLAGS) -dll
G_EXE_LFLAGS = $(I_LFLAGS) 

############################################################################
# Standard system libraries
############################################################################
G_SYS_LIBS = msvcprt.lib msvcrt.lib vcruntime.lib ucrt.lib $(I_SYS_LIBS)

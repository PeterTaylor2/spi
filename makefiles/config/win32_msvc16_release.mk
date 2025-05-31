# definitions for building 32-bit libraries and applications on Windows
# using the VC16 compiler

G_ABI=Release-x86-vc16
G_BUILD_DIR=win32/msvc16_release

ifeq ($(G_WIN32_COMPILER),msvc16)
G_ABI=Release-x86
endif

include $(U_MAKEFILES)/config/win32_msvc16.mk

###########################################################################
# for C/C++ compilation we need to define the following:
#
# G_CFLAGS G_CPPFLAGS G_SYS_INCLUDES
# 
# since we set the system includes by defining INCLUDE we can skip setting
# G_SYS_INCLUDES
###########################################################################

I_DEFINES = -D_CRT_SECURE_NO_WARNINGS $(U_DEFINES) $(U_WIN32_RELEASE_CFLAGS)
I_CFLAGS = -c -MD -W3 -Gy -nologo -O2 $(I_DEFINES)

# if we want multi-character byte set then add -D_MCBS

ifdef U_CLR

# we need to use -clr flag plus the 2.0 version of the .NET framework files
# also we must force use of C++ for C files

G_CPPFLAGS = $(I_CFLAGS) -EHa -clr $(G_CLR_USING)
G_CFLAGS = /TP $(G_CXX_CFLAGS)

else

# /arch:SSE2 incompatible with -clr

G_CFLAGS = $(I_CFLAGS) /arch:SSE2 $(U_WIN32_CFLAGS)
G_CPPFLAGS = $(I_CFLAGS) -EHsc /arch:SSE2 $(U_WIN32_CPPFLAGS)

endif

G_MSBUILD_PROPERTIES=/p:Configuration=Release /p:Platform=x86

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
I_LFLAGS = /profile /incremental:no /nodefaultlib /fixed:no $(G_OPTIMIZE_LFLAGS) $(U_WIN32_RELEASE_LFLAGS)

G_DLL_LFLAGS = $(I_LFLAGS) -dll
G_EXE_LFLAGS = $(I_LFLAGS) 

############################################################################
# Standard system libraries
############################################################################
G_SYS_LIBS = msvcprt.lib msvcrt.lib vcruntime.lib ucrt.lib $(I_SYS_LIBS)

# definitions for building 64-bit libraries and applications on linux64
# using the gcc compiler

G_ABI=Debug-linux64
G_BUILD_DIR=linux64/gcc_debug
G_DEBUG=_d

include $(U_MAKEFILES)/config/linux64_gcc.mk

###########################################################################
# for C/C++ compilation we need to define the following:
#
# G_CFLAGS G_CPPFLAGS G_SYS_INCLUDES
# 
# since we set the system includes by defining INCLUDE we can skip setting
# G_SYS_INCLUDES
###########################################################################

I_DEFINES = $(U_DEFINES) $(U_DEBUG_CFLAGS) $(U_LINUX64_DEBUG_CFLAGS)
I_CFLAGS = -pthread -m64 $(G_LINUX64_GCC_WARNINGS) -ggdb3 -c -fPIC -pipe $(G_LINUX_VISIBILITY) $(I_DEFINES)

G_CFLAGS = $(I_CFLAGS) $(U_LINUX64_CFLAGS)
G_CPPFLAGS = $(I_CFLAGS) $(U_LINUX64_CPPFLAGS)

############################################################################
# Linker flags 
############################################################################
G_DEBUG_LFLAGS = 
I_LFLAGS = -m64 -o $@ $(G_DEBUG_LFLAGS) $(U_LINUX64_DEBUG_LFLAGS) -Wl,-rpath,. -z defs
I_DLL_LFLAGS = -shared $(G_LINKMAP_SPEC)
G_DLL_LFLAGS = $(I_LFLAGS) $(I_DLL_LFLAGS)
G_EXE_LFLAGS = $(I_LFLAGS) 

############################################################################
# Standard system libraries
############################################################################
G_SYS_LIBS = $(I_SYS_LIBS)

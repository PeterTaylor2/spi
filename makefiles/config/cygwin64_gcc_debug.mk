# definitions for building 64-bit libraries and applications on cygwin64
# using the gcc compiler

G_ABI=Debug-cygwin64
G_BUILD_DIR=cygwin64/gcc_debug
G_DEBUG=_d

include $(U_MAKEFILES)/config/cygwin64_gcc.mk

###########################################################################
# for C/C++ compilation we need to define the following:
#
# G_CFLAGS G_CPPFLAGS G_SYS_INCLUDES
# 
# since we set the system includes by defining INCLUDE we can skip setting
# G_SYS_INCLUDES
###########################################################################

I_DEFINES = $(U_DEFINES) $(U_DEBUG_CFLAGS) $(U_LINUX64_DEBUG_CFLAGS)
I_CFLAGS = -pthread -m64 -Wall -Werror -Wno-unused-value -Wno-deprecated -Wno-unused-variable -Wno-unused-but-set-variable -Wno-write-strings -Wno-sequence-point -ggdb3 -c -fPIC $(I_DEFINES)

G_CFLAGS = $(I_CFLAGS) 
G_CPPFLAGS = $(I_CFLAGS) -std=gnu++11

############################################################################
# Linker flags 
############################################################################
G_DEBUG_LFLAGS = 
I_LFLAGS = -m64 -o $@ $(G_DEBUG_LFLAGS) $(U_LINUX64_DEBUG_LFLAGS) -Wl,-rpath,.
I_DLL_LFLAGS = -shared $(G_LINKMAP_SPEC)
G_DLL_LFLAGS = $(I_LFLAGS) $(I_DLL_LFLAGS)
G_EXE_LFLAGS = $(I_LFLAGS) 

############################################################################
# Standard system libraries - since we use -MD instead of -MDd these are the
# same as the libraries for the optimised build
############################################################################
G_SYS_LIBS = $(I_SYS_LIBS)

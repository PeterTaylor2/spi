# definitions for building 32-bit libraries and applications on cygwin32
# using the gcc compiler

G_ABI=Debug-cygwin32
G_BUILD_DIR=cygwin32/gcc_debug
G_DEBUG=_d

include $(U_MAKEFILES)/config/cygwin32_gcc.mk

###########################################################################
# for C/C++ compilation we need to define the following:
#
# G_CFLAGS G_CPPFLAGS G_SYS_INCLUDES
# 
# since we set the system includes by defining INCLUDE we can skip setting
# G_SYS_INCLUDES
###########################################################################

I_DEFINES = $(U_DEFINES) $(U_DEBUG_CFLAGS) $(U_LINUX32_DEBUG_CFLAGS)
I_CFLAGS = -pthread -m32 -Wall -Werror -Wno-unused-value -Wno-deprecated -Wno-unused-variable -Wno-unused-but-set-variable -Wno-write-strings -Wno-sequence-point -ggdb3 -c -fPIC -pipe $(I_DEFINES)

G_CFLAGS = $(I_CFLAGS) 
G_CPPFLAGS = $(I_CFLAGS) -std=gnu++11

############################################################################
# Linker flags 
############################################################################
G_DEBUG_LFLAGS = 
I_LFLAGS = -m32 -o $@ $(G_DEBUG_LFLAGS) $(U_LINUX32_DEBUG_LFLAGS) -Wl,-rpath,.
I_DLL_LFLAGS = -shared $(G_LINKMAP_SPEC)
G_DLL_LFLAGS = $(I_LFLAGS) $(I_DLL_LFLAGS)
G_EXE_LFLAGS = $(I_LFLAGS) 

############################################################################
# Standard system libraries - since we use -MD instead of -MDd these are the
# same as the libraries for the optimised build
############################################################################
G_SYS_LIBS = $(I_SYS_LIBS)

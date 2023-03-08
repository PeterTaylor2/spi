# common build parameters for building 64-bit libraries and applications
# on cygwin using gcc
#
# this file will set all necessary path variables for using the compiler,
# so there is no need to rely on the path to be set externally
#

###########################################################################
# Set the environment variables needed to get the compiler working. 
###########################################################################
include $(U_MAKEFILES)/config/cygwin64.mk

###########################################################################
# define standard variables to be used in all makefiles to avoid platform
# dependence
###########################################################################

G_OBJ=o
G_EXE=
G_LIB_PFX=lib
G_LIB_EXT=.a
G_DLL_PFX=lib
G_DLL_EXT=.so
G_DLL_LIB_EXT=.so

G_CC=gcc -o $(I_BUILD_DIR)/$*.o -MMD -MP
G_CPPC=g++ -o $(I_BUILD_DIR)/$*.o -MMD -MP
G_LINK=g++
G_LIB=ar rv $@ 

#G_LINKMAP_SPEC=-Xlinker --version-script -Xlinker $(U_LINKMAP)

# these are the system libraries independent of whether we are using
# debug or optimised code
I_SYS_LIBS=-lc -lm -ldl -lpthread


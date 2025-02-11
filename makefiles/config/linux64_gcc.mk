# common build parameters for building 64-bit libraries and applications
# on linux using gcc
#
# this file will set all necessary path variables for using the compiler,
# so there is no need to rely on the path to be set externally
#

###########################################################################
# Set the environment variables needed to get the compiler working. 
###########################################################################
include $(U_MAKEFILES)/config/linux64.mk

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

# these are the system libraries independent of whether we are using
# debug or optimised code
I_SYS_LIBS=-lc -lm -ldl -lpthread $(G_UUID_LIBS)

# common warning flags for both release and debug mode
# you can override this entire definition before including the configuration file
#
# to add flags use U_DEFINES instead (defined only for linux64_gcc context)
# you can also use U_LINUX64_RELEASE_CFLAGS or U_LINUX64_DEBUG_CFLAGS

G_LINUX64_GCC_WARNINGS?=-Wall -Wno-unused-value -Wno-deprecated -Wno-unused-variable -Wno-unused-but-set-variable -Wno-write-strings -Wno-sequence-point -Werror

G_LINUX_VISIBILITY?=-fvisibility=hidden


#
# common build parameters for building 32-bit libraries and applications
# on Windows using the VC9 compiler
#
# our preference is to use /MD builds (regardless of whether we are in
# debug or release mode)
#
# these will link with the relevant run-time DLLs and hence all memory is
# actually allocated and managed within that particular run-time DLL
#
# this file will set all necessary path variables for using the compiler,
# so there is no need to rely on the path to be set externally
#
# note that the build system is assumed to be driven by cygwin - hence all
# file names are in cygwin style - when setting certain types of environment
# variables for use by Windows executables, then we need to ensure they will
# be in the correct Windows format
#
# cygwin itself translates PATH, but it doesn't know about variables such
# as INCLUDE or LIB - we want to use subst inside the makefile rather than
# calling cygpath since that is an unnecessary executable called for each
# invocation of make
#

###########################################################################
# Set the environment variables needed to get the compiler working. 
# 
# Deal with the tedious complication of different file-naming styles.
###########################################################################
include $(U_MAKEFILES)/config/win32.mk

# it seems like this is the correct home regardless of whether we are building
# on 64-bit or 32-bit windows
I_MSSDK_HOME:=/cygdrive/c/Program Files/Microsoft SDKs/Windows/v6.0A
I_MSSDK_HOME_DOS:=$(subst /,\,$(subst $(G_CYGWIN_ROOT),C:,$(I_MSSDK_HOME)))
I_VS9_HOME:=$(G_WIN32_PROGRAM_FILES)/Microsoft Visual Studio 9.0
I_VS9_HOME_DOS:=$(I_WIN32_PROGRAM_FILES_DOS)\Microsoft Visual Studio 9.0
I_VC9_PATH:=$(I_VS9_HOME)/VC/bin:$(I_VS9_HOME)/Common7/IDE:$(I_MSSDK_HOME)/Bin

PATH:=$(I_VC9_PATH):$(PATH)
INCLUDE:=$(I_VS9_HOME_DOS)\VC\include;$(I_MSSDK_HOME_DOS)\Include
LIB:=$(I_MSSDK_HOME_DOS)\Lib;$(I_VS9_HOME_DOS)\VC\lib

export PATH
export INCLUDE
export LIB

vc9:
	@echo "PATH=$(PATH)"
	@echo
	@echo "INCLUDE=$(INCLUDE)"
	@echo
	@echo "LIB=$(LIB)"
	@echo
	@echo "where is the compiler:"
	@which cl
	@echo
	@echo "where is the library archiver:"
	@which lib
	@echo
	@echo "where is the linker:"
	@which link
	@echo
	@echo "where is the manifest builder:"
	@which mt


###########################################################################
# define standard variables to be used in all makefiles to avoid platform
# dependence
###########################################################################

G_OBJ=obj
G_EXE=.exe
G_LIB_PFX=
G_LIB_EXT=.lib
G_DLL_PFX=
G_DLL_EXT=.dll
G_DLL_LIB_EXT=.lib
G_EMBED_MANIFEST=1

G_CC=cl /Fo"$(I_BUILD_DIR)/" /nologo
G_CPPC=cl /Fo"$(I_BUILD_DIR)/" /nologo
G_LINK=link /out:"$@" /nologo
G_LIB=lib /out:"$@" /nologo

ifdef U_PRECOMPILED_HEADER
	G_PCH=$(I_BUILD_DIR)/$(U_PRECOMPILED_HEADER).pch
	G_PCH_OBJ=$(I_BUILD_DIR)/$(U_PRECOMPILED_HEADER).pch
	G_PCH_CC=cl.exe /Yc"$(U_PRECOMPILED_HEADER).hpp" /Fp"$(G_PCH) /Fo"$(G_PCH_OBJ)"
	G_PCH_CFLAGS=/Yu"$(U_PRECOMPILED_HEADER).hpp" /Fp"$(G_PCH)"
endif

# these are the system libraries independent of whether we are using
# debug or optimised code
I_SYS_LIBS=\
 kernel32.lib\
 wsock32.lib\
 netapi32.lib\
 advapi32.lib\
 oldnames.lib\
 comdlg32.lib\
 comctl32.lib\
 user32.lib\
 ws2_32.lib\
 $(G_UUID_LIBS)\
 wldap32.lib

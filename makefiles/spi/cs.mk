target:

.PHONY: code target 

############################################################################
# commands for creating the Visual Studio project files for installation
############################################################################
U_VCPROJ_OPTIONS+=-t target

U_SPI_BUILD_DIR?=$(G_BUILD_DIR)
I_SRC=src
U_DLL_DIR?=../dll
U_CONFIG_DIR?=../config

-include $(U_DLL_DIR)/properties.mk

###########################################################################
# optional pre-build command for generating the code - usually done within
# the config directory
###########################################################################
code:
	$(MAKE) -C $(U_CONFIG_DIR) code-cs

U_FORCE_REBUILD+=Makefile $(U_DLL_DIR)/properties.mk
U_FORCE_REBUILD+=$(U_SPI_HOME)/makefiles/version.mk
############################################################################
# the following values depend on properties.mk but can be overridden
############################################################################
U_TARGET?=$(U_SERVICE_NAMESPACE)-csc
U_VCPROJ?=$(U_SERVICE)-csc
U_CFLAGS+=-D$(U_DECLSPEC)_CSC_EXPORT
U_DEBUG_CFLAGS+=-DSPI_DEBUG

include $(U_SPI_HOME)/makefiles/version.mk

U_SRC_DIR:=src
U_INC_DIR:=.
U_INCLUDES+=-I$(U_SPI_HOME) -I$(U_DLL_DIR)

include $(U_SPI_HOME)/makefiles/spi/base_service.mk
include $(U_SPI_HOME)/makefiles/spi/runtime.mk

############################################################################
# define the libraries including the SPI runtime libraries
#
# we need both the main DLL and spi_util as well as sppy.lib from SPI
# we need the service DLL
#
# probably no need for extra DEP_LIBS to be user defined
############################################################################
ifeq ($(G_PLATFORM),win32)

U_DEP_LIBS:=\
$(U_DLL_DIR)/$(G_BUILD_DIR)/$(U_SERVICE_DLL).lib\
$(U_DEP_LIBS)\
$(I_SPI_RUNTIME_BIN_DIR)/spi-c.lib\
$(I_SPI_RUNTIME_BIN_DIR)/$(SPI_DLL).lib\
$(I_SPI_RUNTIME_BIN_DIR)/$(SPI_UTIL_DLL).lib\
$(I_SPI_RUNTIME_BIN_DIR)/$(SPI_CURL_DLL).lib\
$(I_SPI_RUNTIME_BIN_DIR)/spi_boost.lib

U_LIBS=$(U_DEP_LIBS) 

G_DLL_EXT:=$(G_DEBUG_EXT).dll

G_SPI_DLLS=$(SPI_DLL).dll

else

U_DEP_LIBS:=\
$(U_DLL_DIR)/$(G_BUILD_DIR)/lib$(U_SERVICE_DLL)$(G_DLL_EXT)\
$(U_DEP_LIBS)\
$(I_SPI_RUNTIME_BIN_DIR)/libspi-c$(G_DLL_EXT)\
$(I_SPI_RUNTIME_BIN_DIR)/lib$(SPI_DLL)$(G_DLL_EXT)\
$(I_SPI_RUNTIME_BIN_DIR)/libspi_util.a

U_LIBS:=\
-L$(U_DLL_DIR)/$(G_BUILD_DIR) -l$(U_SERVICE_DLL)\
$(U_LIBS)\
-L$(I_SPI_RUNTIME_BIN_DIR) -lspi-c -l$(SPI_DLL)\
$(I_SPI_RUNTIME_BIN_DIR)/libspi_util.a

G_DLL_PFX=

G_SPI_DLLS:=lib$(SPI_DLL)$(G_DLL_EXT) 

endif

###########################################################################
# regular DLL build except that we change the extension for windows to be
# .pyd instead of the standard .dll
###########################################################################
include $(U_SPI_HOME)/makefiles/build/dll.mk

###########################################################################
# what we do after the build
###########################################################################
post_build: dll

dll: ../$(G_ABI)/$(U_TARGET)$(G_DLL_EXT)
dll: ../$(G_ABI)/$(G_DLL_PFX)spi-c$(G_DLL_EXT)

../$(G_ABI)/$(U_TARGET)$(G_DLL_EXT): $(G_BUILD_DIR)/$(U_TARGET)$(G_DLL_EXT)
	@mkdir -p ../$(G_ABI)
	cp -f $< ../$(G_ABI)

../$(G_ABI)/%$(G_DLL_EXT): $(U_SPI_HOME)/bin/$(G_ABI)/%$(G_DLL_EXT)
	@mkdir -p ../$(G_ABI)
	cp -f $< ../$(G_ABI)

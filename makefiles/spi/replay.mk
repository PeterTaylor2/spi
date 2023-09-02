include $(U_SPI_HOME)/makefiles/version.mk

U_INCLUDES+=-I$(U_SPI_HOME) -I../dll
U_SPI_BUILD_DIR?=$(G_BUILD_DIR)
U_DLL_DIR?=../dll
U_SRC_DIR?=.
U_INC_DIR?=.
U_OUTPUT_DIR?=..

include $(U_SPI_HOME)/makefiles/spi/base_service.mk
include $(U_SPI_HOME)/makefiles/spi/runtime.mk

-include $(U_DLL_DIR)/properties.mk

U_FORCE_REBUILD+=Makefile $(U_DLL_DIR)/properties.mk
U_FORCE_REBUILD+=$(U_SPI_HOME)/makefiles/version.mk
###########################################################################
# the following values depend on properties.mk but can be overridden
###########################################################################
U_TARGET?=$(U_SERVICE_NAMESPACE)-replay
U_VCPROJ?=$(U_SERVICE)-replay

ifeq ($(G_PLATFORM),win32)

U_DEP_LIBS:=\
$(U_DLL_DIR)/$(G_BUILD_DIR)/$(U_SERVICE_DLL).lib\
$(U_DEP_LIBS)\
$(I_SPI_RUNTIME_BIN_DIR)/$(G_LIB_PFX)$(SPI_DLL)$(G_LIB_EXT)\
$(I_SPI_RUNTIME_BIN_DIR)/$(G_LIB_PFX)$(SPI_UTIL_DLL)$(G_LIB_EXT)\
$(I_SPI_RUNTIME_BIN_DIR)/$(G_LIB_PFX)$(SPI_CURL_DLL)$(G_LIB_EXT)

U_LIBS=$(U_DEP_LIBS)

else

U_DEP_LIBS:=\
$(U_DLL_DIR)/$(G_BUILD_DIR)/lib$(U_SERVICE_DLL)$(G_DLL_EXT)\
$(U_DEP_LIBS)\
$(I_SPI_RUNTIME_BIN_DIR)/$(G_DLL_PFX)$(SPI_DLL)$(G_DLL_EXT)\
$(I_SPI_RUNTIME_BIN_DIR)/$(G_DLL_PFX)$(SPI_UTIL_DLL)$(G_DLL_EXT)

ifdef G_CYGWIN
U_LIBS:=\
$(U_DLL_DIR)/$(G_BUILD_DIR)/lib$(U_SERVICE_DLL)$(G_DLL_EXT)\
$(U_LIBS)\
$(I_SPI_RUNTIME_BIN_DIR)/$(G_DLL_PFX)$(SPI_DLL)$(G_DLL_EXT)\
$(I_SPI_RUNTIME_BIN_DIR)/$(G_DLL_PFX)$(SPI_UTIL_DLL)$(G_DLL_EXT)
else
U_LIBS:=\
-L$(U_DLL_DIR)/$(G_BUILD_DIR) -l$(U_SERVICE_DLL)\
$(U_LIBS)\
-L$(I_SPI_RUNTIME_BIN_DIR) -l$(SPI_DLL) -l$(SPI_UTIL_DLL)
endif

endif

include $(U_SPI_HOME)/makefiles/build/exe.mk

post_build: dll copy_exe

dll: $(U_OUTPUT_DIR)/$(G_ABI)/$(G_DLL_PFX)$(SPI_DLL)$(G_DLL_EXT)
dll: $(U_OUTPUT_DIR)/$(G_ABI)/$(G_DLL_PFX)$(SPI_UTIL_DLL)$(G_DLL_EXT)
dll: $(U_OUTPUT_DIR)/$(G_ABI)/$(G_DLL_PFX)$(U_SERVICE_DLL)$(G_DLL_EXT)

ifeq ($(G_PLATFORM),win32)

dll: $(U_OUTPUT_DIR)/$(G_ABI)/$(G_DLL_PFX)$(SPI_CURL_DLL)$(G_DLL_EXT)

endif

copy_exe: $(U_OUTPUT_DIR)/$(G_ABI)/$(U_TARGET)$(G_EXE)

$(U_OUTPUT_DIR)/$(G_ABI)/%$(G_DLL_EXT): $(I_SPI_RUNTIME_BIN_DIR)/%$(G_DLL_EXT)
	@mkdir -p $(U_OUTPUT_DIR)/$(G_ABI)
	cp -f $< $(U_OUTPUT_DIR)/$(G_ABI)
ifeq ($(G_PLATFORM),win32)
	@if [ -f $(basename $<).pdb ]; then cp -f $(basename $<).pdb $(U_OUTPUT_DIR)/$(G_ABI); fi
endif

$(U_OUTPUT_DIR)/$(G_ABI)/$(G_DLL_PFX)$(U_SERVICE_DLL)$(G_DLL_EXT): $(U_DLL_DIR)/$(G_BUILD_DIR)/$(G_DLL_PFX)$(U_SERVICE_DLL)$(G_DLL_EXT)
	@mkdir -p $(U_OUTPUT_DIR)/$(G_ABI)
	cp -f $< $(U_OUTPUT_DIR)/$(G_ABI)
ifeq ($(G_PLATFORM),win32)
	@if [ -f $(basename $<).pdb ]; then cp -f $(basename $<).pdb $(U_OUTPUT_DIR)/$(G_ABI); fi
endif

$(U_OUTPUT_DIR)/$(G_ABI)/$(U_TARGET)$(G_EXE): $(I_TARGET)
	@mkdir -p $(U_OUTPUT_DIR)/$(G_ABI)
	cp -f $< $(U_OUTPUT_DIR)/$(G_ABI)
ifeq ($(G_PLATFORM),win32)
	@if [ -f $(basename $<).pdb ]; then cp -f $(basename $<).pdb $(U_OUTPUT_DIR)/$(G_ABI); fi
endif


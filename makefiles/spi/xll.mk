############################################################################
# we have a target 'all' which builds all available versions of Excel
# and the usual 'target' which just makes the preferred version of Excel
#
# any other rule which acts on all available versions of Excel will have
# the name '-all' appended to the equivalent rule which just acts on
# a single version of Python
#
# the preferred version of Excel will be $(G_XL_VERSION) and all available
# versions of Excel will be $(G_XL_VERSIONS) 
#
# the first block of commands are the ones which loop through the versions
#
# the second block of commands are the ones which run for a specific version
############################################################################

.PHONY: all xll code target targets

# for backward compatibility only
xll: target

U_HAS_ALL_TARGET=1

############################################################################
# BLOCK ONE - commands which loop through the versions
############################################################################
all::
	$(MAKE) build-all

build-all:
	@for xlVersion in $(G_XL_VERSIONS); do \
		echo "Building for XL_VERSION=$$xlVersion"; \
		$(MAKE) target XL_VERSION=$$xlVersion; \
	done

clean-all::
	@rm -fr $(G_BUILD_DIR)
	@for xlVersion in $(G_XL_VERSIONS); do \
		echo "Cleaning XL_VERSION=$$xlVersion"; \
		$(MAKE) -s clean XL_VERSION=$$xlVersion; \
	done

############################################################################
# we need U_VCPROJ_OPTIONS for vcproj and vcxproj targets
############################################################################
U_CONFIG_DIR?=../config
U_DLL_DIR?=../dll
U_OUTPUT_DIR?=..

-include $(U_DLL_DIR)/properties.mk

###########################################################################
# optional pre-build command for generating the code - usually done within
# the config directory
###########################################################################
code:
	$(MAKE) -C $(U_CONFIG_DIR) code-excel

U_FORCE_REBUILD+=Makefile $(U_DLL_DIR)/properties.mk
U_FORCE_REBUILD+=$(U_SPI_HOME)/makefiles/version.mk
###########################################################################
# the following values depend on properties.mk but can be overridden
###########################################################################
U_TARGET?=$(U_SERVICE_NAMESPACE)
U_VCPROJ?=$(U_SERVICE)-excel
U_CFLAGS+=-DXL_$(U_DECLSPEC)_EXPORT
U_VCPROJ_OPTIONS+=-s"*.bas" -s"*.frm"

############################################################################
# BLOCK TWO - commands to compile the code or install the extension for
# a specific excel version
#
# the first thing we do is try to weed out (based on version) which
# builds we don't actually run
############################################################################
ifneq ($(G_PLATFORM),win32)
U_NO_BUILD=1
endif

############################################################################
# defining the build for given G_XL_VERSION
############################################################################
U_BUILD_SUFFIX=-xl$(G_XL_VERSION)
U_CFLAGS+=-DSPI_XL_VERSION=$(G_XL_VERSION)
U_SPI_BUILD_DIR?=$(G_BUILD_DIR)

include $(U_SPI_HOME)/makefiles/version.mk

U_SRC_DIR:=src
U_INC_DIR:=.
U_INCLUDES+=-Isrc -I$(U_SPI_HOME) -I$(U_DLL_DIR)
ifndef U_XLADDIN_TARGET
U_XLADDIN_TARGET:=$(U_TARGET).xlam
endif

include $(U_SPI_HOME)/makefiles/spi/base_service.mk
include $(U_SPI_HOME)/makefiles/spi/runtime.mk

###########################################################################
# define the libraries including the SPI runtime libraries
#
# since this only works for windows we can use specific extensions to make
# things a bit easier to read
#
# we need the main DLL, spxl.lib and spi_util from the SPI runtime
# we need the service DLL
#
# probably no need for any extra DEP_LIBS to be user defined
###########################################################################
G_XL_ABI=$(G_ABI)/xl$(G_XL_VERSION)

U_DEP_LIBS:=\
$(U_DLL_DIR)/$(G_BUILD_DIR)/$(U_SERVICE_DLL).lib\
$(U_DEP_LIBS)\
$(I_SPI_RUNTIME_BIN_DIR)/xl$(G_XL_VERSION)/$(SPI_DLL)-xl$(G_XL_VERSION).lib\
$(I_SPI_RUNTIME_BIN_DIR)/$(SPI_DLL).lib\
$(I_SPI_RUNTIME_BIN_DIR)/$(SPI_UTIL_DLL).lib\
$(I_SPI_RUNTIME_BIN_DIR)/$(SPI_CURL_DLL).lib

U_LIBS=$(U_DEP_LIBS) $(U_SPI_HOME)/excel/win32/xl$(G_XL_VERSION)/xlcall32.lib

###########################################################################
# regular DLL build except that we change the extension to be .xll instead
# of the standard .dll
###########################################################################
G_DLL_EXT:=.xll

ifneq ($(U_XLL_TARGET),)
U_TARGET:=$(U_XLL_TARGET)
else 
U_TARGET:=xl_$(U_TARGET)
endif

include $(U_SPI_HOME)/makefiles/build/dll.mk

###########################################################################
# what we do after the build
###########################################################################
post_build: dll

U_NO_XLADDIN?=0

ifeq ($(U_NO_XLADDIN),0)

post_build: xladdin

xladdin: $(U_OUTPUT_DIR)/$(G_XL_ABI)/$(U_XLADDIN_TARGET)

$(U_OUTPUT_DIR)/$(G_XL_ABI)/$(U_XLADDIN_TARGET): $(U_XLADDIN_TARGET)
	@mkdir -p $(U_OUTPUT_DIR)/$(G_XL_ABI)
	cp -f $(U_XLADDIN_TARGET) $(U_OUTPUT_DIR)/$(G_XL_ABI)

endif

###########################################################################
# G_EXCEL_MAKE_XLADDIN can be used to construct the XLAM file
#
# in your makefile execute the command
#
# $(G_EXCEL_MAKE_XLADDIN) $(U_XLADDIN_TARGET) *.bas *.frm
# (not exactly like that - specify the files you want)
#
# $(G_EXCEL_INSTALLER) --clean $(G_EXCEL_ADDINS_DIR)/$(U_SERVICE_NAME) *.xlam *.xll
# (not exactly like that - specify the files you need - all *.dll from G_XL_ABI and G_ABI
###########################################################################
G_EXCEL_MAKE_XLADDIN=$(U_SPI_HOME)/makefiles/win32/makeXLAddin.exe
G_EXCEL_INSTALLER=$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/installExcel.py
G_EXCEL_ADDINS_DIR?=$(abspath $(APPDATA))/Microsoft/AddIns

dll: $(U_OUTPUT_DIR)/$(G_XL_ABI)/$(U_TARGET).xll
dll: $(U_OUTPUT_DIR)/$(G_XL_ABI)/$(SPI_DLL)-xl$(G_XL_VERSION).dll
dll: $(U_OUTPUT_DIR)/$(G_XL_ABI)/$(U_SERVICE_SVO)

$(U_OUTPUT_DIR)/$(G_XL_ABI)/$(SPI_DLL)-xl$(G_XL_VERSION).dll: $(I_SPI_RUNTIME_BIN_DIR)/xl$(G_XL_VERSION)/$(SPI_DLL)-xl$(G_XL_VERSION).dll
	@mkdir -p $(U_OUTPUT_DIR)/$(G_XL_ABI)
	cp -f $< $(U_OUTPUT_DIR)/$(G_XL_ABI)
	@if [ -f $(basename $<).pdb ]; then cp -f $(basename $<).pdb $(U_OUTPUT_DIR)/$(G_XL_ABI); fi

$(U_OUTPUT_DIR)/$(G_XL_ABI)/$(U_TARGET).xll: $(G_BUILD_DIR)-xl$(G_XL_VERSION)/$(U_TARGET).xll
	@mkdir -p $(U_OUTPUT_DIR)/$(G_XL_ABI)
	cp -f $< $(U_OUTPUT_DIR)/$(G_XL_ABI)
	@if [ -f $(basename $<).pdb ]; then cp -f $(basename $<).pdb $(U_OUTPUT_DIR)/$(G_XL_ABI); fi

# this rule => only have to define the extra files to be copied in Makefile
# note that we don't actually have much need to copy the DLLs since we are
# unlikely to load the XLADDIN from G_XL_ABI
#
# nevertheless in case we do need this feature we use ln to save on space
$(U_OUTPUT_DIR)/$(G_XL_ABI)/%: $(U_OUTPUT_DIR)/$(G_ABI)/%
	@mkdir -p $(U_OUTPUT_DIR)/$(G_XL_ABI)
	ln -f $< $(U_OUTPUT_DIR)/$(G_XL_ABI)


ifdef U_USE_EXCEL_INSTALL

I_INSTALL_TARGET_DIR=$(abspath $(APPDATA))/Microsoft/Addins/$(U_SERVICE_NAMESPACE)

I_INSTALL_FILES=\
$(U_OUTPUT_DIR)/$(G_XL_ABI)/$(U_TARGET).xll\
$(U_OUTPUT_DIR)/$(G_XL_ABI)/$(SPI_DLL)-xl$(G_XL_VERSION).dll\
$(U_OUTPUT_DIR)/$(G_ABI)/$(U_SERVICE_SVO)\
$(U_OUTPUT_DIR)/$(G_ABI)/$(U_SERVICE_DLL).dll\
$(U_OUTPUT_DIR)/$(G_ABI)/$(SPI_DLL).dll\
$(U_OUTPUT_DIR)/$(G_ABI)/$(SPI_UTIL_DLL).dll\
$(U_OUTPUT_DIR)/$(G_ABI)/$(SPI_CURL_DLL).dll\
$(addprefix $(U_OUTPUT_DIR)/$(G_ABI)/,$(notdir $(U_DLLS)))

ifeq ($(U_NO_XLADDIN),0)

I_INSTALL_FILES+=$(U_OUTPUT_DIR)/$(G_XL_ABI)/$(U_XLADDIN_TARGET)

endif

install:
	$(G_EXCEL_INSTALLER) $(U_EXCEL_INSTALL_OPTS) $(I_INSTALL_TARGET_DIR) $(I_INSTALL_FILES)

install-info:
	@echo TARGET=$(I_INSTALL_TARGET_DIR)
	@echo FILES=$(I_INSTALL_FILES)

v16.vcxproj::
	@$(I_VCPROJ16_CMD) $(I_COMPILER_OPTIONS) -j1 -t install -b $(U_SPI_HOME)/makefiles/cygwin32/bin $(U_VCPROJ)-install.v16.vcxproj $(U_VCPROJ)-install . . $(G_WIN32_COMPILER) $(G_WIN32_BITS)

v17.vcxproj::
	@$(I_VCPROJ17_CMD) $(I_COMPILER_OPTIONS) -j1 -t install -b $(U_SPI_HOME)/makefiles/cygwin32/bin $(U_VCPROJ)-install.v17.vcxproj $(U_VCPROJ)-install . . $(G_WIN32_COMPILER) $(G_WIN32_BITS)

v18.vcxproj::
	@$(I_VCPROJ18_CMD) $(I_COMPILER_OPTIONS) -j1 -t install -b $(U_SPI_HOME)/makefiles/cygwin32/bin $(U_VCPROJ)-install.v18.vcxproj $(U_VCPROJ)-install . . $(G_WIN32_COMPILER) $(G_WIN32_BITS)

endif

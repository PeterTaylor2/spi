############################################################################
# the main target is 'xllTest' instead of the more usual 'target'
#
# this is because we invoke the makefile more than once for each version
# of Excel we support, but ultimately it uses the exe.mk mechanism which
# expects something called 'target'
#
# the first block of commands are the ones which loop through the versions
#
# the second block of commands are the ones which run for a specific version
############################################################################

.PHONY: xllTest target targets

xllTest: target

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
U_VCPROJ_OPTIONS=
U_PRODUCT_DIR?=../..
U_OUTPUT_DIR?=../..

include $(U_PRODUCT_DIR)/dll/properties.mk

U_FORCE_REBUILD+=Makefile $(U_PRODUCT_DIR)/dll/properties.mk
U_FORCE_REBUILD+=$(U_SPI_HOME)/makefiles/version.mk
U_FORCE_REBUILD+=$(U_SPI_HOME)/makefiles/spi/xllTest.mk
###########################################################################
# the following values depend on properties.mk but can be overridden
###########################################################################
U_TARGET?=xltest-$(U_SERVICE_NAMESPACE)
U_VCPROJ?=$(U_SERVICE)-excel-test
U_XLL_TARGET?=xl_$(U_SERVICE_NAMESPACE)

############################################################################
# BLOCK TWO - commands to compile the code or install the extension for
# a specific excel version
#
# the first thing we do is try to weed out (based on version) which
# builds we don't actually run
############################################################################
ifneq ($(G_PLATFORM),win32)
U_NO_BUILD=1
else ifeq ($(G_BITS),64)
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
U_INC_DIR:=src
U_INCLUDES+=-I$(U_SPI_HOME) -I$(U_SPI_HOME)/spi/excel/src/xl$(G_XL_VERSION)

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
U_DEP_LIBS:=\
$(U_DEP_LIBS)\
$(U_SPI_HOME)/xltest/$(G_BUILD_DIR)$(U_BUILD_SUFFIX)/xltest.lib\
$(U_SPI_HOME)/xlcall32/$(G_BUILD_DIR)$(U_BUILD_SUFFIX)/xlcall32.lib\
$(I_SPI_RUNTIME_BIN_DIR)/xl$(G_XL_VERSION)/$(SPI_DLL)-xl$(G_XL_VERSION).lib\
$(I_SPI_RUNTIME_BIN_DIR)/$(SPI_DLL).lib\
$(I_SPI_RUNTIME_BIN_DIR)/$(SPI_UTIL_DLL).lib\
$(I_SPI_RUNTIME_BIN_DIR)/$(SPI_CURL_DLL).lib

U_LIBS=$(U_DEP_LIBS)
ifeq ($(G_XL_VERSION),12)
U_LFLAGS+=/export:MdCallBack12
endif

include $(U_SPI_HOME)/makefiles/build/exe.mk

###########################################################################
# what we do after the build
###########################################################################
post_build: exe

G_XL_ABI=$(G_ABI)/xl$(G_XL_VERSION)

exe: $(G_XL_ABI)/$(U_TARGET).exe
exe: $(G_XL_ABI)/$(SPI_DLL).dll
exe: $(G_XL_ABI)/$(SPI_UTIL_DLL).dll
exe: $(G_XL_ABI)/$(SPI_CURL_DLL).dll
exe: $(G_XL_ABI)/$(U_SERVICE_DLL).dll
exe: $(G_XL_ABI)/$(U_XLL_TARGET).xll
exe: $(G_XL_ABI)/xlcall32.dll

exe: $(G_XL_ABI)/$(SPI_DLL)-xl$(G_XL_VERSION).dll

$(G_XL_ABI)/$(U_TARGET).exe: $(G_BUILD_DIR)-xl$(G_XL_VERSION)/$(U_TARGET).exe
	@mkdir -p $(G_XL_ABI)
	cp -f $< $(G_XL_ABI)

$(G_XL_ABI)/%.dll: $(U_OUTPUT_DIR)/$(G_XL_ABI)/%.dll
	@mkdir -p $(G_XL_ABI)
	cp -f $< $(G_XL_ABI)

$(G_XL_ABI)/%.dll: $(U_OUTPUT_DIR)/$(G_ABI)/%.dll
	@mkdir -p $(G_XL_ABI)
	cp -f $< $(G_XL_ABI)

$(G_XL_ABI)/%.xll: $(U_OUTPUT_DIR)/$(G_XL_ABI)/%.xll
	@mkdir -p $(G_XL_ABI)
	cp -f $< $(G_XL_ABI)

$(G_XL_ABI)/xlcall32.dll: $(U_SPI_HOME)/xlcall32/$(G_BUILD_DIR)-xl$(G_XL_VERSION)/xlcall32.dll
	@mkdir -p $(G_XL_ABI)
	cp -f $< $(G_XL_ABI)


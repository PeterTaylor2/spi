target:

U_SPI_HOME=..
U_HAS_ALL_TARGET=1

include $(U_SPI_HOME)/makefiles/config/config.mk

#################################################################################
# BLOCK ONE: targets for all available versions of Excel
#################################################################################
all::
	@for xlVersion in $(G_XL_VERSIONS); do \
		echo "Building for XL_VERSION=$$xlVersion"; \
		$(MAKE) target XL_VERSION=$$xlVersion; \
	done

clean-all::
	@for xlVersion in $(G_XL_VERSIONS); do \
		$(MAKE) -s clean XL_VERSION=$$xlVersion; \
	done

dumpbin-all:
	@for xlVersion in $(G_XL_VERSIONS); do \
		$(MAKE) -s dumpbin XL_VERSION=$$xlVersion; \
	done

#################################################################################
# BLOCK TWO: targets for a specific version of Excel
#################################################################################
target:

ifneq ($(G_PLATFORM),win32)
U_NO_BUILD=1
endif

U_SRC_DIR=src
U_INC_DIR=.
U_INCLUDES=-I$(U_SPI_HOME)/spi/excel/src/xl$(G_XL_VERSION)
U_TARGET=xlcall32
U_VCPROJ=xlcall32
U_VCPROJ_OPTIONS=-H $(U_SPI_HOME)/spi/excel/src/xl4 -H $(U_SPI_HOME)/spi/excel/src/xl12
U_FORCE_REBUILD+=xlcall32.def
U_BUILD_SUFFIX=-xl$(G_XL_VERSION)

U_CFLAGS+=-DSPI_XL_VERSION=$(G_XL_VERSION)
U_LFLAGS+=/def:xlcall32.def
G_OVERRIDE_NEW_DELETE=0

include $(U_SPI_HOME)/makefiles/build/dll.mk


dumpbin:
	dumpbin /exports $(I_TARGET)


# not convinced by the post_build commands
# post_build: bin

bin: ../bin/$(G_ABI)/xl$(G_XL_VERSION)/$(notdir $(I_TARGET))

../bin/$(G_ABI)/xl$(G_XL_VERSION)/$(notdir $(I_TARGET)): $(I_TARGET)
	@mkdir -p ../bin/$(G_ABI)/xl$(G_XL_VERSION)
	cp -f $(I_TARGET) ../bin/$(G_ABI)/xl$(G_XL_VERSION)
ifeq ($(G_PLATFORM),win32) 
ifeq ($(DEBUG),1)
	cp -f $(G_BUILD_DIR)$(U_BUILD_SUFFIX)/xlcall32$(G_XL_VERSION).pdb ../bin/$(G_ABI)/xl$(G_XL_VERSION)
endif
endif


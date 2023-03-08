target:

U_SPI_HOME=..
U_HAS_ALL_TARGET=1

include $(U_SPI_HOME)/makefiles/config/config.mk

###########################################################################
# BLOCK ONE: targets for building for all version of excel
###########################################################################
all::
	@for xlVersion in $(G_XL_VERSIONS); do \
		echo "Building for XL_VERSION=$$xlVersion"; \
		$(MAKE) target XL_VERSION=$$xlVersion; \
	done

clean-all::
	@for xlVersion in $(G_XL_VERSIONS); do \
		$(MAKE) -s clean XL_VERSION=$$xlVersion; \
	done

###########################################################################
# BLOCK TWO: targets for building for a specific version of excel
###########################################################################
ifneq ($(G_PLATFORM),win32)
U_NO_BUILD=1
endif

U_SRC_DIR=src
U_INC_DIR=.
U_INCLUDES=-I$(U_SPI_HOME) -I$(U_SPI_HOME)/xlcall32 -I$(U_SPI_HOME)/spi/excel/src/xl$(G_XL_VERSION)
U_TARGET=xltest
U_VCPROJ=xltest
U_VCPROJ_OPTIONS=
U_BUILD_SUFFIX=-xl$(G_XL_VERSION)

U_CFLAGS+=-DSPI_XL_VERSION=$(G_XL_VERSION)

include $(U_SPI_HOME)/makefiles/build/lib.mk

# not convinced by the post_build commands
# post_build: bin

bin: ../bin/$(G_ABI)/xl$(G_XL_VERSION)/$(notdir $(I_TARGET))

../bin/$(G_ABI)/xl$(G_XL_VERSION)/$(notdir $(I_TARGET)): $(I_TARGET)
	@mkdir -p ../bin/$(G_ABI)/xl$(G_XL_VERSION)
	cp -f $(I_TARGET) ../bin/$(G_ABI)/xl$(G_XL_VERSION)
ifeq ($(G_PLATFORM),win32) 
ifeq ($(DEBUG),1)
	cp -f $(G_BUILD_DIR)$(U_BUILD_SUFFIX)/xltest.pdb ../bin/$(G_ABI)/xl$(G_XL_VERSION)
endif
endif


target:

U_SPI_HOME=..
U_HAS_ALL_TARGET=1

include $(U_SPI_HOME)/makefiles/config/config.mk

all:
	@for xlVersion in $(G_XL_VERSIONS); do \
		echo "Building for XL_VERSION=$$xlVersion"; \
		$(MAKE) target XL_VERSION=$$xlVersion; \
	done

clean-all:
	@for xlVersion in $(G_XL_VERSIONS); do \
		$(MAKE) -s clean XL_VERSION=$$xlVersion; \
	done

include $(U_SPI_HOME)/makefiles/version.mk

ifneq ($(G_PLATFORM),win32)
U_NO_BUILD=1
endif

U_SRC_DIR=$(U_SPI_HOME)/spi/excel/src
U_INC_DIR=$(U_SPI_HOME)/spi/excel
U_INCLUDES=-I$(U_SPI_HOME) -I$(U_SPI_HOME)/spi/excel/src/xl$(G_XL_VERSION)
U_TARGET=$(SPI_DLL)-xl$(G_XL_VERSION)
U_VCPROJ=spxl-dll
U_VCPROJ_OPTIONS=-H $(U_SRC_DIR)/xl4 -H $(U_SRC_DIR)/xl12 -S $(U_SRC_DIR)/xl4 -S $(U_SRC_DIR)/xl12
U_BUILD_SUFFIX=-xl$(G_XL_VERSION)
U_CFLAGS+=-DSPI_XL_EXPORT

U_DEP_LIBS=\
../bin/$(G_ABI)/$(G_LIB_PFX)$(SPI_DLL)$(G_LIB_EXT)\
../bin/$(G_ABI)/$(G_LIB_PFX)$(SPI_UTIL_DLL)$(G_LIB_EXT)\
../bin/$(G_ABI)/$(G_LIB_PFX)spi_boost$(G_LIB_EXT)

U_LIBS=$(U_DEP_LIBS) $(U_SPI_HOME)/excel/win$(G_BITS)/xl$(G_XL_VERSION)/xlcall32.lib

ifeq ($(G_XL_VERSION),4)

vpath %.c $(U_SRC_DIR)/xl4

U_EXTRA_OBJS=framewrk.$(G_OBJ)

endif

ifeq ($(G_XL_VERSION),12)

vpath %.c $(U_SRC_DIR)/xl12
vpath %.cpp $(U_SRC_DIR)/xl12

U_EXTRA_OBJS=xlcall.$(G_OBJ) framewrk.$(G_OBJ) MemoryManager.$(G_OBJ) MemoryPool.$(G_OBJ)

endif

ifeq ($(G_XL_VERSION),15)

vpath %.c $(U_SRC_DIR)/xl15
vpath %.cpp $(U_SRC_DIR)/xl15

U_EXTRA_OBJS=xlcall.$(G_OBJ) framewrk.$(G_OBJ) MemoryManager.$(G_OBJ) MemoryPool.$(G_OBJ)

endif

U_CFLAGS+=-DSPI_XL_VERSION=$(G_XL_VERSION)

ifeq ($(DEBUG),1)
U_CFLAGS+=-DXL_AUTO_OPEN_ALERT
endif

include $(U_SPI_HOME)/makefiles/build/dll.mk

post_build: bin

bin: ../bin/$(G_ABI)/xl$(G_XL_VERSION)/$(U_TARGET).lib

../bin/$(G_ABI)/xl$(G_XL_VERSION)/$(U_TARGET).lib: $(I_TARGET)
	@mkdir -p ../bin/$(G_ABI)/xl$(G_XL_VERSION)
	cp -f $(G_BUILD_DIR)$(U_BUILD_SUFFIX)/$(U_TARGET).lib ../bin/$(G_ABI)/xl$(G_XL_VERSION)

bin: ../bin/$(G_ABI)/xl$(G_XL_VERSION)/$(notdir $(I_TARGET))

../bin/$(G_ABI)/xl$(G_XL_VERSION)/$(notdir $(I_TARGET)): $(I_TARGET)
	@mkdir -p ../bin/$(G_ABI)/xl$(G_XL_VERSION)
	cp -f $(I_TARGET) ../bin/$(G_ABI)/xl$(G_XL_VERSION)
ifeq ($(G_PLATFORM),win32)
	@if [ -f $(basename $<).pdb ]; then cp -f $(basename $<).pdb ../bin/$(G_ABI)/xl$(G_XL_VERSION); fi
endif


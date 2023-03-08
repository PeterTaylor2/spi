target:

U_SPI_HOME=..

include $(U_SPI_HOME)/makefiles/config/config.mk
include $(U_SPI_HOME)/makefiles/version.mk

U_SRC_DIR=$(U_SPI_HOME)/spi/c/src
U_INC_DIR=$(U_SPI_HOME)/spi/c
U_INCLUDES=-I$(U_SPI_HOME)
U_TARGET=spi-c
U_VCPROJ=spi-c-dll
U_CFLAGS=-DSPI_C_EXPORT
U_FORCE_REBUILD=Makefile $(U_SPI_HOME)/makefiles/version.mk

ifeq ($(G_PLATFORM),win32)
U_DEP_LIBS=\
../bin/$(G_ABI)/$(G_LIB_PFX)$(SPI_DLL)$(G_LIB_EXT)\
../bin/$(G_ABI)/$(G_LIB_PFX)$(SPI_UTIL_DLL)$(G_LIB_EXT)\
../bin/$(G_ABI)/$(G_LIB_PFX)spi_boost$(G_LIB_EXT)

I_SYS_LIBS+=ole32.lib

U_LIBS=$(U_DEP_LIBS)

else

U_DEP_STATIC_LIBS=../bin/$(G_ABI)/$(G_LIB_PFX)spi_boost$(G_LIB_EXT)

U_DEP_LIBS=\
../bin/$(G_ABI)/$(G_LIB_PFX)$(SPI_DLL)$(G_DLL_EXT)\
../bin/$(G_ABI)/$(G_LIB_PFX)$(SPI_UTIL_DLL)$(G_DLL_EXT)\
$(U_DEP_STATIC_LIBS)

U_LIBS=-L../bin/$(G_ABI) -l$(SPI_DLL) -l$(SPI_UTIL_DLL) $(U_DEP_STATIC_LIBS)

endif

include $(U_SPI_HOME)/makefiles/build/dll.mk

rstrip:
	@$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/rstrip.py $(U_SRC_DIR)/*.[ch]* $(U_INC_DIR)/*.[ch]*

post_build: bin

bin: ../bin/$(G_ABI)/$(notdir $(I_TARGET))

../bin/$(G_ABI)/$(notdir $(I_TARGET)): $(I_TARGET)
	@mkdir -p ../bin/$(G_ABI)
	cp -f $(I_TARGET) ../bin/$(G_ABI)
ifeq ($(G_PLATFORM),win32)
	cp -f $(G_BUILD_DIR)/$(U_TARGET).lib ../bin/$(G_ABI)
	@if [ -f $(basename $<).pdb ]; then cp -f $(basename $<).pdb ../bin/$(G_ABI); fi
endif



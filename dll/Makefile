target:

U_SPI_HOME=..

include $(U_SPI_HOME)/makefiles/config/config.mk
include $(U_SPI_HOME)/makefiles/version.mk

U_SRC_DIR=$(U_SPI_HOME)/spi/src
U_INC_DIR=$(U_SPI_HOME)/spi
U_INCLUDES=-I$(U_SPI_HOME)
U_FORCE_REBUILD=Makefile $(U_SPI_HOME)/makefiles/version.mk

U_TARGET=$(SPI_DLL)
U_VCPROJ=spi-dll

ifeq ($(G_PLATFORM),win32)

U_DEP_LIBS=\
../bin/$(G_ABI)/$(G_LIB_PFX)$(SPI_UTIL_DLL)$(G_LIB_EXT)\
../bin/$(G_ABI)/$(G_LIB_PFX)spi_boost$(G_LIB_EXT)

U_LIBS=$(U_DEP_LIBS)

else

U_DEP_LIBS=\
../bin/$(G_ABI)/$(G_LIB_PFX)$(SPI_UTIL_DLL)$(G_DLL_EXT)\
../bin/$(G_ABI)/$(G_LIB_PFX)spi_boost$(G_LIB_EXT)

ifdef G_CYGWIN
U_LIBS=$(U_DEP_LIBS) $(G_CURL_LIBS)
else
U_LIBS=-L../bin/$(G_ABI) -l$(SPI_UTIL_DLL)\
../bin/$(G_ABI)/$(G_LIB_PFX)spi_boost$(G_LIB_EXT)\
$(G_CURL_LIBS)
endif

endif

U_CFLAGS+=-DSPI_EXPORT

# U_CFLAGS+=-DTRACE_WRAPPER_CACHE

include $(U_SPI_HOME)/makefiles/build/dll.mk

post_build: bin

bin: ../bin/$(G_ABI)/$(notdir $(I_TARGET))

../bin/$(G_ABI)/$(notdir $(I_TARGET)): $(I_TARGET)
	@mkdir -p ../bin/$(G_ABI)
	cp -f $(I_TARGET) ../bin/$(G_ABI)
ifeq ($(G_PLATFORM),win32)
	cp -f $(G_BUILD_DIR)/$(SPI_DLL).lib ../bin/$(G_ABI)
	@if [ -f $(basename $<).pdb ]; then cp -f $(basename $<).pdb ../bin/$(G_ABI); fi
endif

rstrip:
	@$(G_PYTHON) $(U_SPI_HOME)/makefiles/python3/rstrip.py $(U_SRC_DIR)/*.[ch]pp $(U_INC_DIR)/*.h*


target:

U_SPI_HOME=..
U_HAS_ALL_TARGET=1

include $(U_SPI_HOME)/makefiles/config/config.mk
include $(U_SPI_HOME)/makefiles/version.mk

#############################################################################
# BLOCK ONE: loop through all available versions of python
#############################################################################

all:
	@for pyVersion in $(G_PY_VERSIONS); do \
		echo "Building for PY_VERSION=$$pyVersion"; \
		$(MAKE) target PY_VERSION=$$pyVersion; \
	done

clean-all:
	@for pyVersion in $(G_PY_VERSIONS); do \
		$(MAKE) clean PY_VERSION=$$pyVersion; \
	done

###########################################################################
# BLOCK TWO: code which applies for a specific version of python
#
# previously this code was in python.mk but that seems unnecessary
###########################################################################

U_SPI_HOME=..

include $(U_SPI_HOME)/makefiles/config/config.mk
include $(U_SPI_HOME)/makefiles/version.mk

U_SRC_DIR=$(U_SPI_HOME)/spi/python/src
U_INC_DIR=$(U_SPI_HOME)/spi/python
U_INCLUDES=-I$(U_SPI_HOME) $(G_PYTHON_INCLUDES)
U_TARGET=$(SPI_DLL)-py$(G_PY_VERSION)
U_VCPROJ=sppy-dll
U_VCPROJ_OPTIONS=

ifeq ($(G_PLATFORM),win32)

U_DEP_LIBS=\
../bin/$(G_ABI)/$(G_LIB_PFX)$(SPI_DLL)$(G_LIB_EXT)\
../bin/$(G_ABI)/$(G_LIB_PFX)$(SPI_UTIL_DLL)$(G_LIB_EXT)\
../bin/$(G_ABI)/$(G_LIB_PFX)spi_boost$(G_LIB_EXT)

U_LIBS=$(U_DEP_LIBS) $(G_PYTHON_LIBS)

else

U_DEP_STATIC_LIBS=\
../bin/$(G_ABI)/$(G_LIB_PFX)spi_boost$(G_LIB_EXT)

U_DEP_LIBS=\
../bin/$(G_ABI)/$(G_LIB_PFX)$(SPI_DLL)$(G_DLL_EXT)\
../bin/$(G_ABI)/$(G_LIB_PFX)$(SPI_UTIL_DLL)$(G_DLL_EXT)\
$(U_DEP_STATIC_LIBS)

ifdef G_CYGWIN
U_LIBS=$(U_DEP_LIBS) $(G_PYTHON_LIBS)
else
U_LIBS=-L../bin/$(G_ABI) -l$(SPI_DLL) -l$(SPI_UTIL_DLL) $(U_DEP_STATIC_LIBS) $(G_PYTHON_LIBS)
endif

endif

U_FORCE_REBUILD+=Makefile
U_CFLAGS+=-DSPI_PY_EXPORT

U_BUILD_SUFFIX=-py$(G_PY_VERSION)

ifeq ($(DEBUG),1)
U_CFLAGS+=-DPY_STARTUP_ALERT
endif

ifeq ($(G_BITS),64)
ifeq ($(G_PLATFORM),win32)
ifeq ($(G_PLATFORM),26)
U_NO_BUILD=1
endif
endif
endif

include $(U_SPI_HOME)/makefiles/build/dll.mk

rstrip:
	@$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/rstrip.py $(U_SRC_DIR)/*.[ch]* $(U_INC_DIR)/*.[ch]*

post_build: bin

ifeq ($(G_PLATFORM),win32)

# for windows we need to copy the .lib file as well as the shared library

bin: ../bin/$(G_ABI)/py$(G_PY_VERSION)/$(U_TARGET).lib

../bin/$(G_ABI)/py$(G_PY_VERSION)/$(U_TARGET).lib: $(I_TARGET)
	@mkdir -p ../bin/$(G_ABI)/py$(G_PY_VERSION)
	cp -f $(G_BUILD_DIR)$(U_BUILD_SUFFIX)/$(U_TARGET).lib ../bin/$(G_ABI)/py$(G_PY_VERSION)

endif

bin: ../bin/$(G_ABI)/py$(G_PY_VERSION)/$(notdir $(I_TARGET))

../bin/$(G_ABI)/py$(G_PY_VERSION)/$(notdir $(I_TARGET)): $(I_TARGET)
	@mkdir -p ../bin/$(G_ABI)/py$(G_PY_VERSION)
	cp -f $(I_TARGET) ../bin/$(G_ABI)/py$(G_PY_VERSION)


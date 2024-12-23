############################################################################
# we have a target 'all' which builds all available versions of Python
# and the usual 'target' which just makes the preferred version of Python
#
# any other rule which acts on all available versions of Python will have
# the name '-all' appended to the equivalent rule which just acts on
# a single version of Python
#
# the preferred version of Python will be $(G_PY_VERSION) and all available
# versions of Python will be $(G_PY_VERSIONS) 
#
# the first block of commands are the ones which loop through the versions
#
# the second block of commands are the ones which run for a specific version
############################################################################

.PHONY: all code target build-all clean-all pyd

include $(U_SPI_HOME)/makefiles/config/python.mk

# for backward compatibility only
pyd: target

U_HAS_ALL_TARGET=1

############################################################################
# BLOCK ONE - commands which loop through the versions
############################################################################
all::
	$(MAKE) build-all

build-all::
	@for pyVersion in $(G_PY_VERSIONS); do \
		echo "Building for PY_VERSION=$$pyVersion"; \
		$(MAKE) target PY_VERSION=$$pyVersion; \
	done

install-all::
	@for pyVersion in $(G_PY_VERSIONS); do \
		echo "Installing for PY_VERSION=$$pyVersion"; \
		$(MAKE) install PY_VERSION=$$pyVersion; \
	done

clean-all::
	@for pyVersion in $(G_PY_VERSIONS); do \
		echo "Cleaning PY_VERSION=$$pyVersion"; \
		$(MAKE) clean PY_VERSION=$$pyVersion; \
	done

############################################################################
# commands for creating the Visual Studio project files for installation
############################################################################
ifeq ($(G_PLATFORM),win32)

U_VCPROJ_OPTIONS+=-s '*.py'

v15.vcxproj::
	@$(I_VCPROJ15_CMD) -j1 -t "install INSTALL_FROM_VS=1" -b $(U_MAKEFILES)/cygwin32/bin $(U_VCPROJ)-install.v15.vcxproj $(U_VCPROJ)-install . . $(G_WIN32_COMPILER) $(G_WIN32_BITS)

v16.vcxproj::
	@$(I_VCPROJ16_CMD) -j1 -t "install INSTALL_FROM_VS=1" -b $(U_MAKEFILES)/cygwin32/bin $(U_VCPROJ)-install.v16.vcxproj $(U_VCPROJ)-install . . $(G_WIN32_COMPILER) $(G_WIN32_BITS)

v17.vcxproj::
	@$(I_VCPROJ17_CMD) -j1 -t "install INSTALL_FROM_VS=1" -b $(U_MAKEFILES)/cygwin32/bin $(U_VCPROJ)-install.v17.vcxproj $(U_VCPROJ)-install . . $(G_WIN32_COMPILER) $(G_WIN32_BITS)

endif

U_BUILD_SUFFIX=-py$(G_PY_VERSION)
U_SPI_BUILD_DIR?=$(G_BUILD_DIR)
I_SRC=src
U_DLL_DIR?=../dll
U_CONFIG_DIR?=../config
U_OUTPUT_DIR?=..

-include $(U_DLL_DIR)/properties.mk

###########################################################################
# optional pre-build command for generating the code - usually done within
# the config directory
###########################################################################
code:
	$(MAKE) -C $(U_CONFIG_DIR) code-python

U_FORCE_REBUILD+=Makefile $(U_DLL_DIR)/properties.mk
U_FORCE_REBUILD+=$(U_SPI_HOME)/makefiles/version.mk
############################################################################
# the following values depend on properties.mk but can be overridden
############################################################################
U_TARGET?=py_$(U_SERVICE)
U_VCPROJ?=$(U_SERVICE)-python
U_CFLAGS+=-DPY_$(U_DECLSPEC)_EXPORT
U_DEBUG_CFLAGS+=-DSPI_DEBUG

############################################################################
# BLOCK TWO - commands to compile the code or install the extension for
# a specific python version
############################################################################

include $(U_SPI_HOME)/makefiles/version.mk

U_SRC_DIR:=src
U_INC_DIR:=src
U_INCLUDES+=-I$(U_SPI_HOME) $(G_PYTHON_INCLUDES) -I$(U_DLL_DIR)

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
$(I_SPI_RUNTIME_BIN_DIR)/py$(G_PY_VERSION)/$(SPI_DLL)-py$(G_PY_VERSION).lib\
$(I_SPI_RUNTIME_BIN_DIR)/$(SPI_DLL).lib\
$(I_SPI_RUNTIME_BIN_DIR)/$(SPI_UTIL_DLL).lib\
$(I_SPI_RUNTIME_BIN_DIR)/$(SPI_CURL_DLL).lib\
$(I_SPI_RUNTIME_BIN_DIR)/spi_boost.lib

U_LIBS=$(U_DEP_LIBS) $(G_PYTHON_LIBS)

G_DLL_EXT:=$(G_DEBUG_EXT).pyd

G_SPI_DLLS=$(SPI_DLL).dll $(SPI_UTIL_DLL).dll $(SPI_CURL_DLL).dll $(SPI_DLL)-py$(G_PY_VERSION).dll
G_INSTALL_DLL_EXT:=.dll
G_INSTALL_PYD_EXT:=.pyd
G_INSTALL_DLL_PFX:=
G_INSTALL_PYD_PFX:=

else

ifdef G_CYGWIN
G_DLL_EXT:=$(G_DEBUG_EXT).dll
endif

U_DEP_LIBS:=\
$(U_DLL_DIR)/$(G_BUILD_DIR)/lib$(U_SERVICE_DLL).so\
$(U_DEP_LIBS)\
$(I_SPI_RUNTIME_BIN_DIR)/py$(G_PY_VERSION)/lib$(SPI_DLL)-py$(G_PY_VERSION).so\
$(I_SPI_RUNTIME_BIN_DIR)/lib$(SPI_DLL).so\
$(I_SPI_RUNTIME_BIN_DIR)/lib$(SPI_UTIL_DLL).so\
$(I_SPI_RUNTIME_BIN_DIR)/libspi_boost$(G_LIB_EXT)

ifdef G_CYGWIN
U_LIBS:=\
$(U_DLL_DIR)/$(G_BUILD_DIR)/lib$(U_SERVICE_DLL).so\
$(U_LIBS)\
$(I_SPI_RUNTIME_BIN_DIR)/py$(G_PY_VERSION)/lib$(SPI_DLL)-py$(G_PY_VERSION).so\
$(I_SPI_RUNTIME_BIN_DIR)/lib$(SPI_DLL).so\
$(I_SPI_RUNTIME_BIN_DIR)/lib$(SPI_UTIL_DLL).so\
$(I_SPI_RUNTIME_BIN_DIR)/libspi_boost$(G_LIB_EXT)\
$(G_PYTHON_LIBS)
else
U_LIBS:=\
-L$(U_DLL_DIR)/$(G_BUILD_DIR) -l$(U_SERVICE_DLL)\
$(U_LIBS)\
-L$(I_SPI_RUNTIME_BIN_DIR)/py$(G_PY_VERSION) -l$(SPI_DLL)-py$(G_PY_VERSION)\
-L$(I_SPI_RUNTIME_BIN_DIR) -l$(SPI_DLL) -l$(SPI_UTIL_DLL)\
$(I_SPI_RUNTIME_BIN_DIR)/libspi_boost$(G_LIB_EXT)\
$(G_PYTHON_LIBS)
endif

G_DLL_PFX=

G_SPI_DLLS:=lib$(SPI_DLL).so lib$(SPI_UTIL_DLL).so lib$(SPI_DLL)-py$(G_PY_VERSION).so
G_INSTALL_DLL_EXT:=.so
G_INSTALL_PYD_EXT:=.so
G_INSTALL_DLL_PFX:=lib
G_INSTALL_PYD_PFX:=

ifeq ($(G_PLATFORM),macos64)

G_DLL_EXT:=.so
G_INSTALL_PYD_EXT:=.so

endif

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

G_PY_ABI=$(G_ABI)/py$(G_PY_VERSION)
U_NO_IMPORTER?=0

ifeq ($(U_NO_IMPORTER),0)

post_build: importer

importer: $(U_OUTPUT_DIR)/$(G_PY_ABI)/$(U_SERVICE_NAMESPACE).py

$(U_OUTPUT_DIR)/$(G_PY_ABI)/$(U_SERVICE_NAMESPACE).py: src/$(U_SERVICE_NAMESPACE).py
	@mkdir -p $(U_OUTPUT_DIR)/$(G_PY_ABI)
	@rm -f $(U_OUTPUT_DIR)/$(G_PY_ABI)/*.pyc
	cp -f $< $(U_OUTPUT_DIR)/$(G_PY_ABI)

endif

ifeq ($(G_PLATFORM),win32)

dll: $(U_OUTPUT_DIR)/$(G_PY_ABI)/$(SPI_DLL).dll
dll: $(U_OUTPUT_DIR)/$(G_PY_ABI)/$(SPI_UTIL_DLL).dll
dll: $(U_OUTPUT_DIR)/$(G_PY_ABI)/$(SPI_CURL_DLL).dll
dll: $(U_OUTPUT_DIR)/$(G_PY_ABI)/$(U_SERVICE_DLL).dll
dll: $(U_OUTPUT_DIR)/$(G_PY_ABI)/$(U_TARGET)$(G_DLL_EXT)
dll: $(U_OUTPUT_DIR)/$(G_PY_ABI)/$(SPI_DLL)-py$(G_PY_VERSION).dll
dll: $(addprefix $(U_OUTPUT_DIR)/$(G_PY_ABI)/,$(notdir $(U_DLLS)))

$(U_OUTPUT_DIR)/$(G_PY_ABI)/%.dll: $(I_SPI_RUNTIME_BIN_DIR)/py$(G_PY_VERSION)/%.dll
	@mkdir -p $(U_OUTPUT_DIR)/$(G_PY_ABI)
	cp -f $< $(U_OUTPUT_DIR)/$(G_PY_ABI)
	cp -f $(basename $<).pdb $(U_OUTPUT_DIR)/$(G_PY_ABI)

$(U_OUTPUT_DIR)/$(G_PY_ABI)/$(U_TARGET)$(G_DLL_EXT): $(G_BUILD_DIR)-py$(G_PY_VERSION)/$(U_TARGET)$(G_DLL_EXT)
	@mkdir -p $(U_OUTPUT_DIR)/$(G_PY_ABI)
	cp -f $< $(U_OUTPUT_DIR)/$(G_PY_ABI)
	cp -f $(basename $<).pdb $(U_OUTPUT_DIR)/$(G_PY_ABI)

else

dll: $(U_OUTPUT_DIR)/$(G_PY_ABI)/lib$(SPI_DLL)$(G_INSTALL_DLL_EXT)
dll: $(U_OUTPUT_DIR)/$(G_PY_ABI)/lib$(SPI_UTIL_DLL)$(G_INSTALL_DLL_EXT)
dll: $(U_OUTPUT_DIR)/$(G_PY_ABI)/lib$(U_SERVICE_DLL)$(G_INSTALL_DLL_EXT)
dll: $(U_OUTPUT_DIR)/$(G_PY_ABI)/$(U_TARGET)$(G_INSTALL_PYD_EXT)
dll: $(U_OUTPUT_DIR)/$(G_PY_ABI)/lib$(SPI_DLL)-py$(G_PY_VERSION)$(G_INSTALL_DLL_EXT)
dll: $(addprefix $(U_OUTPUT_DIR)/$(G_PY_ABI)/,$(notdir $(U_DLLS)))

$(U_OUTPUT_DIR)/$(G_PY_ABI)/%$(G_INSTALL_DLL_EXT): $(I_SPI_RUNTIME_BIN_DIR)/py$(G_PY_VERSION)/%$(G_INSTALL_DLL_EXT)
	@mkdir -p $(U_OUTPUT_DIR)/$(G_PY_ABI)
	cp -f $< $(U_OUTPUT_DIR)/$(G_PY_ABI)


$(U_OUTPUT_DIR)/$(G_PY_ABI)/$(U_TARGET)$(G_INSTALL_PYD_EXT): $(G_BUILD_DIR)-py$(G_PY_VERSION)/$(U_TARGET)$(G_INSTALL_PYD_EXT)
	@mkdir -p $(U_OUTPUT_DIR)/$(G_PY_ABI)
	cp -f $< $(U_OUTPUT_DIR)/$(G_PY_ABI)

endif

# this rule => only have to define the files to be copied from the common directory
# to the python directory in the Makefile - use ln to save on disk space
#
# note that we need to copy shared libraries to the PY_ABI directory to run tests since
# python (certainly since 3.8) requires that the dependent DLLs live in the same directory
# as the imported shared library - however it was good practice anyway since otherwise
# we would have to rely on environment variables
$(U_OUTPUT_DIR)/$(G_PY_ABI)/%: $(U_OUTPUT_DIR)/$(G_ABI)/%
	@mkdir -p $(U_OUTPUT_DIR)/$(G_PY_ABI)
	ln -f $< $(U_OUTPUT_DIR)/$(G_PY_ABI)

###########################################################################
# commands for installing the build to site-packages
###########################################################################
ifdef U_NO_BUILD

install:
	@echo "No build for this platform - hence nothing to install"

else

ifdef U_INSTALL_PACKAGE
I_INSTALL_OPTS:=-p $(U_INSTALL_PACKAGE)
else
I_INSTALL_OPTS:=
endif

I_INSTALL_FILES=\
$(G_INSTALL_DLL_PFX)$(SPI_DLL)$(G_INSTALL_DLL_EXT)\
$(G_INSTALL_DLL_PFX)$(SPI_UTIL_DLL)$(G_INSTALL_DLL_EXT)\
$(G_INSTALL_DLL_PFX)$(U_SERVICE_DLL)$(G_INSTALL_DLL_EXT)\
$(U_TARGET)$(G_INSTALL_PYD_EXT)\
$(G_INSTALL_DLL_PFX)$(SPI_DLL)-py$(G_PY_VERSION)$(G_INSTALL_DLL_EXT)\
$(notdir $(U_DLLS))

ifeq ($(U_NO_IMPORTER),0)
I_INSTALL_FILES+=$(U_SERVICE_NAMESPACE).py
endif

install:
	$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/installPython.py $(U_INSTALL_OPTS) $(I_INSTALL_OPTS) $(U_SERVICE_NAMESPACE) $(U_OUTPUT_DIR)/$(G_PY_ABI) $(I_INSTALL_FILES) $(U_INSTALL_FILES)

install-info:
	@echo DLLS=$(U_DLLS)
	@echo FILES=$(I_INSTALL_FILES)

endif 

###########################################################################
# commands for running python using the newly built library
###########################################################################
PYTHONPATH=$(U_OUTPUT_DIR)/$(G_PY_ABI)
export PYTHONPATH

run:
	$(WRAPPER) $(G_PYTHON) $(ARGS)


target:

U_SPI_HOME=../..
U_HAS_ALL_TARGET=1

include $(U_SPI_HOME)/makefiles/config/config.mk

###############################################################################
# BLOCK ONE: commands for all versions of python
###############################################################################
all::
	@for pyVersion in $(G_PY_VERSIONS); do \
		echo "Building for PY_VERSION=$$pyVersion"; \
		$(MAKE) target PY_VERSION=$$pyVersion; \
	done

clean-all::
	@for pyVersion in $(G_PY_VERSIONS); do \
		$(MAKE) clean PY_VERSION=$$pyVersion; \
	done

install-all::
	@for pyVersion in $(G_PY_VERSIONS); do \
		$(MAKE) install PY_VERSION=$$pyVersion; \
	done

###############################################################################
# BLOCK TWO: commands for a single version of python
###############################################################################
default: target

.PHONY: bin

U_SPI_HOME=../..

include $(U_SPI_HOME)/makefiles/version.mk

U_FORCE_REBUILD=Makefile
U_CFLAGS=-DPY_SPI_EXPORT
U_SRC_DIR=src
U_INC_DIR=src
U_INCLUDES=-I$(U_SPI_HOME) $(G_PYTHON_INCLUDES) -I$(U_SPI_HOME)/spi
U_TARGET=py_spdoc
U_BUILD_SUFFIX=-py$(G_PY_VERSION)
U_VCPROJ=spdoc

include $(U_SPI_HOME)/makefiles/spi/runtime.mk

ifeq ($(G_PLATFORM),win32)

U_DEP_LIBS=\
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
$(I_SPI_RUNTIME_BIN_DIR)/py$(G_PY_VERSION)/lib$(SPI_DLL)-py$(G_PY_VERSION).so\
$(I_SPI_RUNTIME_BIN_DIR)/lib$(SPI_DLL).so\
$(I_SPI_RUNTIME_BIN_DIR)/lib$(SPI_UTIL_DLL).so\
$(I_SPI_RUNTIME_BIN_DIR)/libspi_boost$(G_LIB_EXT)

ifdef G_CYGWIN
U_LIBS=$(U_DEP_LIBS) $(G_PYTHON_LIBS)
else
U_LIBS=\
-L$(I_SPI_RUNTIME_BIN_DIR)/py$(G_PY_VERSION) -l$(SPI_DLL)-py$(G_PY_VERSION)\
-L$(I_SPI_RUNTIME_BIN_DIR) -l$(SPI_DLL) -l$(SPI_UTIL_DLL)\
$(I_SPI_RUNTIME_BIN_DIR)/libspi_boost$(G_LIB_EXT)\
$(G_PYTHON_LIBS)
endif

G_DLL_PFX=

G_SPI_DLLS=lib$(SPI_DLL).so lib$(SPI_UTIL_DLL).so lib$(SPI_DLL)-py$(G_PY_VERSION).so
G_INSTALL_DLL_EXT:=.so
G_INSTALL_PYD_EXT:=.so
G_INSTALL_DLL_PFX:=lib
G_INSTALL_PYD_PFX:=

ifeq ($(G_PLATFORM),macos64)

G_DLL_EXT:=$(G_DEBUG_EXT).so
G_INSTALL_PYD_EXT:=.so

endif

endif

U_INSTALL_FILES=$(G_SPI_DLLS)\
$(G_INSTALL_PYD_PFX)py_spdoc$(G_INSTALL_PYD_EXT)


###########################################################################
# regular DLL build except that we change the extension for windows to be
# .pyd instead of the standard .dll
###########################################################################
include $(U_SPI_HOME)/makefiles/build/dll.mk

###########################################################################
# what we do after the build
###########################################################################
post_build: importer bin

U_BIN_DIR=bin/$(G_ABI)-py$(G_PY_VERSION)

importer: $(U_BIN_DIR)/spdoc.py

$(U_BIN_DIR)/spdoc.py: src/spdoc.py
	@mkdir -p $(U_BIN_DIR)
	@rm -f $(U_BIN_DIR)/*.pyc
	cp -f $< $(U_BIN_DIR)

ifeq ($(G_PLATFORM),win32)

bin: $(U_BIN_DIR)/$(SPI_DLL).dll
bin: $(U_BIN_DIR)/$(SPI_UTIL_DLL).dll
bin: $(U_BIN_DIR)/$(SPI_CURL_DLL).dll
bin: $(U_BIN_DIR)/$(U_TARGET)$(G_DLL_EXT)
bin: $(U_BIN_DIR)/$(SPI_DLL)-py$(G_PY_VERSION).dll

$(U_BIN_DIR)/%.dll: $(I_SPI_RUNTIME_BIN_DIR)/%.dll
	@mkdir -p $(U_BIN_DIR)
	cp -f $< $(U_BIN_DIR)

$(U_BIN_DIR)/%.dll: $(I_SPI_RUNTIME_BIN_DIR)/py$(G_PY_VERSION)/%.dll
	@mkdir -p $(U_BIN_DIR)
	cp -f $< $(U_BIN_DIR)

$(U_BIN_DIR)/$(U_TARGET)$(G_DLL_EXT): $(G_BUILD_DIR)-py$(G_PY_VERSION)/$(U_TARGET)$(G_DLL_EXT)
	@mkdir -p $(U_BIN_DIR)
	cp -f $< $(U_BIN_DIR)

else

bin: $(U_BIN_DIR)/lib$(SPI_DLL)$(G_INSTALL_DLL_EXT)
bin: $(U_BIN_DIR)/lib$(SPI_UTIL_DLL)$(G_INSTALL_DLL_EXT)
bin: $(U_BIN_DIR)/$(U_TARGET)$(G_INSTALL_PYD_EXT)
bin: $(U_BIN_DIR)/lib$(SPI_DLL)-py$(G_PY_VERSION)$(G_INSTALL_DLL_EXT)

$(U_BIN_DIR)/%$(G_INSTALL_DLL_EXT): $(I_SPI_RUNTIME_BIN_DIR)/%$(G_INSTALL_DLL_EXT)
	@mkdir -p $(U_BIN_DIR)
	cp -f $< $(U_BIN_DIR)

$(U_BIN_DIR)/%$(G_INSTALL_DLL_EXT): $(I_SPI_RUNTIME_BIN_DIR)/py$(G_PY_VERSION)/%$(G_INSTALL_DLL_EXT)
	@mkdir -p $(U_BIN_DIR)
	cp -f $< $(U_BIN_DIR)

$(U_BIN_DIR)/$(U_TARGET)$(G_INSTALL_PYD_EXT): $(G_BUILD_DIR)-py$(G_PY_VERSION)/$(U_TARGET)$(G_INSTALL_PYD_EXT)
	@mkdir -p $(U_BIN_DIR)
	cp -f $< $(U_BIN_DIR)

endif

###########################################################################
# commands for installing the build to site-packages
###########################################################################
ifdef U_NO_BUILD

install:
	@echo "No build for this platform - hence nothing to install"

else

ifeq ($(G_PLATFORM),win32)

install:
	$(G_PYTHON) $(U_SPI_HOME)/makefiles/python$(G_PY_MAJOR_VERSION)/installPython.py spdoc $(U_BIN_DIR) $(U_INSTALL_FILES)

else

install-py_version::
	sudo $(G_PYTHON) $(U_SPI_HOME)/makefiles/python$(G_PY_MAJOR_VERSION)/installPython.py spdoc $(U_BIN_DIR) $(U_INSTALL_FILES)

endif

endif 

###########################################################################
# commands for running python using the newly built library
###########################################################################
PYTHONPATH=$(U_BIN_DIR)
export PYTHONPATH

ifeq ($(G_PLATFORM),linux64)
LD_LIBRARY_PATH:=$(U_BIN_DIR):$(LD_LIBRARY_PATH)
export LD_LIBRARY_PATH
endif

run:
	$(WRAPPER) $(G_PYTHON) $(ARGS)





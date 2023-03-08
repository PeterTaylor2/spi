# common build parameters for building 64-bit libraries and applications
# on macos64

G_PLATFORM=macos64
G_BITS=64
G_EXTLIBS_HOME=$(U_SPI_HOME)/3rdParty
G_CURL_LIBS=-lcurl
G_PY_VERSIONS=2.7

ifdef PY_VERSION
G_PY_VERSION=$(PY_VERSION)
else
G_PY_VERSION=2.7
endif
G_PYTHON=/usr/bin/python
G_PYTHON_FRAMEWORK_DIR=/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks/Python.framework/Versions/$(G_PY_VERSION)
G_PYTHON_INCLUDES=-I$(G_PYTHON_FRAMEWORK_DIR)/include/python$(G_PY_VERSION)
G_PYTHON_LIBS=/usr/lib/libpython$(G_PY_VERSION).dylib


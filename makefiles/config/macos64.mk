# common build parameters for building 64-bit libraries and applications
# on macos64

G_PLATFORM=macos64
G_BITS=64
G_EXTLIBS_HOME=$(U_SPI_HOME)/3rdParty
G_CURL_LIBS=-lcurl

ifdef G_MACOS64_PY_VERSIONS
G_PY_VERSIONS=$(G_MACOS64_PY_VERSIONS)
else
G_PY_VERSIONS=3.14
endif

ifdef PY_VERSION
G_PY_VERSION=$(PY_VERSION)
G_PY_VERSIONS=$(PY_VERSION)
else
ifdef G_MACOS64_PY_VERSION
G_PY_VERSION=$(G_MACOS64_PY_VERSION)
else
G_PY_VERSION=3.14
endif
endif

G_PYTHON=/usr/bin/python

G_PYTHON_FRAMEWORK_VERSION=/Library/Frameworks/Python.framework/Versions/$(G_PY_VERSION)

G_PYTHON=$(G_PYTHON_FRAMEWORK_VERSION)/bin/python$(G_PY_VERSION)
G_PYTHON_INCLUDES=-I$(G_PYTHON_FRAMEWORK_VERSION)/include/python$(G_PY_VERSION)

# we should not define G_PYTHON_LIBS since we will be using -undefined dynamic_lookup for python builds

# common build parameters for building 64-bit libraries and applications
# on macos64

G_CURL_LIBS=-L/usr/lib -lcurl

G_NO_PDFLATEX=1

ifdef G_MACOS64_NO_UUID
G_NO_UUID=$(G_MACOS64_NO_UUID)
else
G_NO_UUID=0
endif

ifeq ($(G_NO_UUID),0)
G_UUID_LIBS=-luuid
else
G_UUID_LIBS=
endif


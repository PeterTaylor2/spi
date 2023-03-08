# common build parameters for building 64-bit libraries and applications
# on cygwin64

G_PLATFORM=cygwin64
G_BITS=64
G_EXTLIBS_HOME=$(U_SPI_HOME)/3rdParty

G_PY_VERSIONS=3.8

ifdef PY_VERSION
G_PY_VERSION=$(PY_VERSION)
else
G_PY_VERSION=3.8
endif
G_PYTHON=/usr/bin/python$(G_PY_VERSION)
G_PYTHON_INCLUDES=-I/usr/include/python$(G_PY_VERSION)
G_PYTHON_LIBS=-lpython$(G_PY_VERSION)
G_CURL_LIBS=-lcurl
G_CYGWIN=1


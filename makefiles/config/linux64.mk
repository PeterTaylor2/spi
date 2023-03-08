# common build parameters for building 64-bit libraries and applications
# on linux64

G_PLATFORM=linux64
G_BITS=64
G_EXTLIBS_HOME=$(U_SPI_HOME)/3rdParty

ifdef G_LINUX64_PY_VERSIONS
G_PY_VERSIONS=$(G_LINUX64_PY_VERSIONS)
else
G_PY_VERSIONS=3.8
endif

ifdef PY_VERSION
G_PY_VERSION=$(PY_VERSION)
G_PY_VERSIONS=$(PY_VERSION)
else
ifdef G_LINUX64_PY_VERSION
G_PY_VERSION=$(G_LINUX64_PY_VERSION)
else
G_PY_VERSION=3.8
endif
endif

G_PYTHON=/usr/bin/python$(G_PY_VERSION)
G_PYTHON_INCLUDES=-I/usr/include/python$(G_PY_VERSION)
G_PYTHON_LIBS=-L/usr/lib/x86_64-linux-gnu -lpython$(G_PY_VERSION)
G_CURL_LIBS=-L/usr/lib/x86_64-linux-gnu -lcurl

G_NO_PDFLATEX=1

ifdef G_LINUX64_NO_UUID
G_NO_UUID=$(G_LINUX64_NO_UUID)
else
G_NO_UUID=0
endif

ifeq ($(G_NO_UUID),0)
G_UUID_LIBS=-luuid
else
G_UUID_LIBS=
endif


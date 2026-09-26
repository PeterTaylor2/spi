# common build parameters for building 64-bit libraries and applications
# on linux64

G_PLATFORM=linux64
G_BITS=64
G_EXTLIBS_HOME=$(U_SPI_HOME)/3rdParty

ifdef G_LINUX64_PY_VERSIONS
G_PY_VERSIONS=$(G_LINUX64_PY_VERSIONS)
else
G_PY_VERSIONS=3.9
endif

ifdef PY_VERSION
G_PY_VERSION=$(PY_VERSION)
G_PY_VERSIONS=$(PY_VERSION)
else
ifdef G_LINUX64_PY_VERSION
G_PY_VERSION=$(G_LINUX64_PY_VERSION)
else
G_PY_VERSION=3.9
endif
endif

# whenever we compile for python shared library we must define U_PYTHON_BUILD
# hence we do not need to define G_PYTHON_LIBS

ifeq ($(G_PY_VERSION),3-abi)

# we require python3.12 to be installed
# but we run with whatever is locally defined as python3
# with an override allowed by defining (via site.mk etc) G_LINUX64_PYTHON3-abi
# note that minus signs are allowed in makefile variables

ifdef G_LINUX64_PYTHON3-abi

G_PYTHON=$(G_LINUX64_PYTHON3-abi)

else

G_PYTHON=python3

endif

G_PYTHON_INCLUDES=-I/usr/include/python3.12
G_PY_LIMITED_API_CFLAGS:=-DPy_LIMITED_API=0x030C0000

else

G_PYTHON=/usr/bin/python$(G_PY_VERSION)
G_PYTHON_INCLUDES=-I/usr/include/python$(G_PY_VERSION)

endif


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


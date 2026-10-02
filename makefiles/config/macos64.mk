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

# G_PYTHON_FRAMEWORK_VERSION is normally where we find python installed on a Mac
#
# if you have installed it somewhere else then you can localise your set-up
# you will either need to use config/site.mk which we deliberately do not provide
# or something in config/computers directory matching your $COMPUTERNAME environment variable
#
# the latter approach is really for ad hoc builds
# the former approach is more controlled 
#
# for a python3.12 build for example, the variables to provide are G_MACOS64_PYTHON3.12
# and G_MACOS64_PYTHON_INCLUDES3.12 and they should be set to the value you want for
# G_PYTHON and G_PYTHON_INCLUDES when G_PY_VERSION=3.12
#
# you can then use the command "make site" to see what is going

G_PYTHON_FRAMEWORK_VERSION=/Library/Frameworks/Python.framework/Versions/$(G_PY_VERSION)

SITE_PYTHON=G_MACOS64_PYTHON$(G_PY_VERSION)
ifdef $(SITE_PYTHON)
G_PYTHON=$($(SITE_PYTHON))
else
G_PYTHON=$(G_PYTHON_FRAMEWORK_VERSION)/bin/python$(G_PY_VERSION)
endif

SITE_PYTHON_INCLUDES=G_MACOS64_PYTHON_INCLUDES$(G_PY_VERSION)
ifdef $(SITE_PYTHON_INCLUDES)
G_PYTHON_INCLUDES=$($(SITE_PYTHON_INCLUDES))
else
G_PYTHON_INCLUDES=-I$(G_PYTHON_FRAMEWORK_VERSION)/include/python$(G_PY_VERSION)
endif

site::
	@echo SITE_PYTHON=$(SITE_PYTHON)
	@echo SITE_PYTHON_INCLUDES=$(SITE_PYTHON_INCLUDES)
	@echo G_PYTHON=$(G_PYTHON)
	@echo G_PYTHON_INCLUDES=$(G_PYTHON_INCLUDES)

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


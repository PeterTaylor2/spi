# common build parameters for building 64-bit libraries and applications
# on Windows

###########################################################################
# Deal with the tedious complication of different file-naming styles.
###########################################################################
G_CYGWIN_ROOT?=/cygdrive/c

# G_WIN32_PROGRAM_FILES is a cygwin-style path (or should be)
# I_WIN32_PROGRAM_FILES defined below is the equivalent Windows style
# G_WIN64_PROGRAM_FILES is a cygwin-style path (or should be)
# I_WIN64_PROGRAM_FILES defined below is the equivalent Windows style

I_WIN32_PROGRAM_FILES_DOS:=$(subst /,\,$(subst $(G_CYGWIN_ROOT),C:,$(G_WIN32_PROGRAM_FILES)))
I_WIN64_PROGRAM_FILES_DOS:=$(subst /,\,$(subst $(G_CYGWIN_ROOT),C:,$(G_WIN64_PROGRAM_FILES)))

include $(U_MAKEFILES)/config/msvc_version.mk

G_PLATFORM=win32
G_BITS=64
G_EXTLIBS_HOME=$(U_SPI_HOME)/3rdParty

G_XL_VERSIONS=15
G_XL_VERSION=15

ifdef G_WIN64_XL_VERSIONS
G_XL_VERSIONS=$(G_WIN64_XL_VERSIONS)
else
G_XL_VERSIONS=15
endif

ifdef XL_VERSION
G_XL_VERSION=$(XL_VERSION)
else
G_XL_VERSION=15
endif

ifdef G_WIN64_PY_VERSIONS
G_PY_VERSIONS=$(G_WIN64_PY_VERSIONS)
else
G_PY_VERSIONS=39 312 313
endif

ifdef PY_VERSION
G_PY_VERSION=$(PY_VERSION)
G_PY_VERSIONS=$(PY_VERSION)
else
ifdef G_WIN64_PY_VERSION
G_PY_VERSION=$(G_WIN64_PY_VERSION)
else
G_PY_VERSION=39
endif
endif

G_PYTHON_INCLUDES=-I$(U_SPI_HOME)/python/include/py$(G_PY_VERSION)
G_PYTHON_LIBS=$(U_SPI_HOME)/python/x64/lib/python$(G_PY_VERSION)$(G_DEBUG_EXT).lib

SITE_PYTHON=G_WIN64_PYTHON$(G_PY_VERSION)
ifdef $(SITE_PYTHON)
G_PYTHON=$(realpath $($(SITE_PYTHON)))
# realpath has problems with spaces
ifeq ("$(G_PYTHON)","")
G_PYTHON=$($(SITE_PYTHON))
endif
else
G_PYTHON=C:/Python$(G_PY_VERSION)-64/python.exe
endif

site::
	@echo SITE_PYTHON=$(SITE_PYTHON)
	@echo G_PYTHON=$(G_PYTHON)

ifdef DOTNET_VERSION
G_DOTNET_VERSION=$(DOTNET_VERSION)
else
G_DOTNET_VERSION=4.0.30319
endif

G_CLR_USING=\
/FU "c:\WINDOWS\Microsoft.NET\Framework\v$(G_DOTNET_VERSION)\System.dll" \
/FU "c:\WINDOWS\Microsoft.NET\Framework\v$(G_DOTNET_VERSION)\System.Data.dll" \
/FU "c:\WINDOWS\Microsoft.NET\Framework\v$(G_DOTNET_VERSION)\System.XML.dll"

ifdef G_WIN64_NO_UUID
G_NO_UUID=$(G_WIN64_NO_UUID)
else
G_NO_UUID=0
endif

ifeq ($(G_NO_UUID),0)
G_UUID_LIBS=rpcrt4.lib
else
G_UUID_LIBS=
endif

ifdef VS_BUILD

# note that G_CYGWIN_BIN might be set by site.mk
# if it hasn't been set then we have an alternative location

ifdef U_SPI_HOME
G_CYGWIN_BIN?=$(U_SPI_HOME)/makefiles/cygwin32/bin
else
G_CYGWIN_BIN?=$(U_MAKEFILES)/cygwin32/bin
endif
PATH:=$(abspath $(G_CYGWIN_BIN)):$(PATH)
export PATH

endif


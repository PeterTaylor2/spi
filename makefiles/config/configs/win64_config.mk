# win64_config.mk
#
# parameters for building on a 64-bit machine using by default the 64-bit
# VC16 compiler (2019)

# note this is included after the optional site.mk which can override compiler
G_WIN32_COMPILER?=msvc16
G_WIN32_BITS=64

BITS?=64

ifdef WIN32_COMPILER
COMPILER?=$(WIN32_COMPILER)
else
COMPILER?=msvc16
endif

G_WIN32_PROGRAM_FILES:=/cygdrive/c/Program Files (x86)
G_WIN64_PROGRAM_FILES:=/cygdrive/c/Program Files
G_BASE_CONFIG:=win$(BITS)_$(COMPILER)

# this is the compiler that we use to build the binaries that are only
# used during the build system rather than for release
# site.mk can be used to define the Windows specific compiler used
# the ancient default is to use MSVC9 - an attempt to avoid runtime library problems

ifdef G_WIN32_CONFIG_COMPILER
G_CONFIG_COMPILER?=$(G_WIN32_CONFIG_COMPILER)
else
G_CONFIG_COMPILER?=msvc9
endif

ifdef G_WIN32_CONFIG_COMPILER_BITS
G_CONFIG_COMPILER_BITS?=$(G_WIN32_CONFIG_COMPILER_BITS)
else
G_CONFIG_COMPILER_BITS?=32
endif

ifdef G_WIN32_CONFIG_COMPILER_ABI
G_CONFIG_COMPILER_ABI?=$(G_WIN32_CONFIG_COMPILER_ABI)
else
G_CONFIG_COMPILER_ABI?=Release-vc9
endif


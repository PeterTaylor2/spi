# win64_32_config.mk
#
# parameters for building on a 64-bit machine using by default the 32-bit
# VC17 compiler (2022)

# note this is included after the optional site.mk which can override compiler
G_WIN32_COMPILER?=msvc17
G_WIN32_BITS=32

BITS?=32

ifdef WIN32_COMPILER
COMPILER?=$(WIN32_COMPILER)
else
COMPILER?=msvc17
endif

G_WIN32_PROGRAM_FILES:=/cygdrive/c/Program Files (x86)
G_WIN64_PROGRAM_FILES:=/cygdrive/c/Program Files
G_BASE_CONFIG:=win$(BITS)_$(COMPILER)


# win32_config.mk
#
# parameters for building 32-bit libraries and applications on a 32-bit machine
# using VC16 as the default compiler
#
# the main issue is how to define the directory which contains the compiler
# which on a 32-bit machine will be installed in Program Files
#

# note this is included after the optional site.mk which can override compiler
G_WIN32_COMPILER?=msvc16
G_WIN32_BITS=32

ifdef WIN32_COMPILER
COMPILER?=$(WIN32_COMPILER)
else
COMPILER?=msvc16
endif

G_WIN32_PROGRAM_FILES:=/cygdrive/c/Program Files
G_BASE_CONFIG:=win32_$(COMPILER)

# this is the compiler that we use to build the binaries that are only
# used during the build system rather than for release
# we used to use MSVC9 to avoid runtime library problems
# however now we compile these binaries from source as part of the build process

G_CONFIG_COMPILER?=$(G_WIN32_COMPILER)
G_CONFIG_COMPILER_BITS?=$(G_WIN32_BITS)
G_CONFIG_COMPILER_ABI?=Release-x86

all: code code-python

U_SPI_HOME=../..
SARTORIAL_CONFIG?=win64
include $(U_SPI_HOME)/makefiles/config/config.mk

U_SERVICE=spdoc
U_CONFIG_DIR=.
U_TARGET_DIR=../../spi
U_PYTHON_DIR=../../svo/spdoc
U_SPCL_OPTIONS=--noGeneratedCodeNotice --noHeaderSplit --licenseFile=../../LGPL.h
U_SPPY_OPTIONS=--noGeneratedCodeNotice --licenseFile=../../LGPL.h
U_VCXPROJ_OPTIONS=-tall

ifeq ($(DEBUG),1)
U_SPCL_OPTIONS+=-w
endif

include $(U_SPI_HOME)/makefiles/spi/spcl.mk
include $(U_SPI_HOME)/makefiles/spi/sppy.mk


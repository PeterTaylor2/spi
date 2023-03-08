all: code code-python

U_SPI_HOME=../..
include $(U_SPI_HOME)/makefiles/config/config.mk

U_SERVICE=replay
U_CONFIG_DIR=.
U_PYTHON_DIR=../python
U_EXCEL_DIR=../excel
U_DOTNET_DIR=../dotnet
U_COMPANY_NAMESPACE=sartorial
U_COMPANY_NAME=Sartorial Programming Ltd.
U_SPPY_OPTIONS+=--noImporter
#U_SPCL_OPTIONS=--noHeaderSplit
#U_SPCL_OPTIONS+=--noVerbatimLine
U_SPPY_OPTIONS+=--noGeneratedCodeNotice --licenseFile=../../LGPL.h
U_SPCL_OPTIONS+=--noGeneratedCodeNotice --licenseFile=../../LGPL.h

U_VCXPROJ_OPTIONS=-tall

include $(U_SPI_HOME)/makefiles/spi/spcl.mk
include $(U_SPI_HOME)/makefiles/spi/sppy.mk


target: code 

U_SPI_HOME=../..
include $(U_SPI_HOME)/makefiles/config/config.mk

U_SERVICE=test
U_CONFIG_DIR=.
U_SPCL_OPTIONS+=--optionalArgs --allHeader --backup

# same as include path for DLL
U_ISDA_CDS=$(U_SAMPLE_HOME)/libs/isda-cds
U_VCXPROJ_OPTIONS+=$(G_BOOST_INCLUDES)\
-I../lib\
-I$(U_SPI_HOME)

WAIT?=0
ifneq ($(WAIT),0)
U_SPCL_OPTIONS+=-w
endif

include $(U_SPI_HOME)/makefiles/spi/spcl.mk


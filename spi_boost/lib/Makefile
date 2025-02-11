target:

G_LINUX_VISIBILITY=
U_SPI_HOME=../..

include $(U_SPI_HOME)/makefiles/config/config.mk

U_VCPROJ=spi-boost-lib

U_SRC_DIR=.
U_INC_DIR=.
U_INCLUDES=-I../.. 
U_TARGET=spi_boost
U_FORCE_REBUILD=Makefile 

include $(U_SPI_HOME)/makefiles/build/lib.mk

post_build: bin

bin: ../../bin/$(G_ABI)/$(notdir $(I_TARGET))

../../bin/$(G_ABI)/$(notdir $(I_TARGET)): $(I_TARGET)
	@mkdir -p ../../bin/$(G_ABI)
	cp -f $(I_TARGET) ../../bin/$(G_ABI)


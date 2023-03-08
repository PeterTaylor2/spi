target:

U_SPI_HOME=..

include $(U_SPI_HOME)/makefiles/config/config.mk

U_VCPROJ=spi-new-delete

U_SRC_DIR=src
U_INC_DIR=.
U_INCLUDES=-I$(U_SPI_HOME)
U_TARGET=spi_new_delete
U_FORCE_REBUILD=Makefile 

include $(U_SPI_HOME)/makefiles/build/lib.mk

post_build: bin

bin: $(U_SPI_HOME)/bin/$(G_ABI)/$(notdir $(I_TARGET))

$(U_SPI_HOME)/bin/$(G_ABI)/$(notdir $(I_TARGET)): $(I_TARGET)
	@mkdir -p $(U_SPI_HOME)/bin/$(G_ABI)
	cp -pf $(I_TARGET) $(U_SPI_HOME)/bin/$(G_ABI)


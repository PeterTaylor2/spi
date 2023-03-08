target:

U_SPI_HOME=..

include $(U_SPI_HOME)/makefiles/config/config.mk
include $(U_SPI_HOME)/makefiles/version.mk

U_VCPROJ=spi-lib

U_SRC_DIR=$(U_SPI_HOME)/spi/src
U_INC_DIR=$(U_SPI_HOME)/spi
U_INCLUDES=-I$(U_SPI_HOME)
U_TARGET=spi
U_FORCE_REBUILD=Makefile $(U_SPI_HOME)/makefiles/version.mk
U_CFLAGS+=-DSPI_STATIC -DSPI_UTIL_STATIC

include $(U_SPI_HOME)/makefiles/build/lib.mk

rstrip:
	@$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/rstrip.py $(U_SRC_DIR)/*.[ch]pp $(U_INC_DIR)/*.h*


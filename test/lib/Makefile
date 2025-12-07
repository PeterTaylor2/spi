target:

U_SPI_HOME=../..
U_TARGET=test-inner

include $(U_SPI_HOME)/makefiles/config/config.mk

U_VCPROJ=test-inner

U_SRC_DIR=src
U_INC_DIR=.
U_INCLUDES=-I$(U_SPI_HOME)/spi_util
U_FORCE_REBUILD=Makefile

include $(U_SPI_HOME)/makefiles/build/lib.mk

rstrip:
	@$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/rstrip.py $(U_SRC_DIR)/*.[ch]pp $(U_INC_DIR)/*.h*


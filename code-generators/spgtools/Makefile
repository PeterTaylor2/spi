target:

ARGS=
U_SPI_HOME=../..
U_TARGET=spgtools

include $(U_SPI_HOME)/makefiles/config/config.mk

U_CFLAGS += -DSPI_STATIC -DSPI_UTIL_STATIC
U_SRC_DIR = src
U_INC_DIR = .
U_INCLUDES = -I$(U_SPI_HOME) -Isrc
U_FORCE_REBUILD = Makefile

include $(U_SPI_HOME)/makefiles/build/lib.mk

lex:
	$(MAKE) -C src/lex

rstrip:
	@$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/rstrip.py *.cpp *.hpp Makefile


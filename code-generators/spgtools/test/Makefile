# tests for the SPI library
#
# use NAME= to define the test that you want to build and run

target:

NAME?=test
OPTS=
U_SPI_HOME=../../..
U_TARGET=$(NAME)

include $(U_SPI_HOME)/makefiles/config/config.mk
include $(U_SPI_HOME)/makefiles/version.mk

U_OBJS = $(NAME).$(G_OBJ)

U_DEP_LIBS = \
$(U_SPI_HOME)/config/spgtools/$(G_BUILD_DIR)/$(G_LIB_PFX)spgtools$(G_LIB_EXT)\
$(U_SPI_HOME)/lib/$(G_BUILD_DIR)/$(G_LIB_PFX)spi$(G_LIB_EXT)\
$(U_SPI_HOME)/spi_util/$(G_BUILD_DIR)/$(G_LIB_PFX)spi_util$(G_LIB_EXT)

U_CFLAGS = -DSPI_STATIC

U_LIBS = $(U_DEP_LIBS)
U_SRC_DIR = .
U_INC_DIR = .
U_INCLUDES = -I$(U_SPI_HOME) -I$(U_SPI_HOME)/config/spgtools

include $(U_SPI_HOME)/makefiles/build/exe.mk

pre_build:
	$(MAKE) -C $(U_SPI_HOME)/spi_util
	$(MAKE) -C $(U_SPI_HOME)/lib
	$(MAKE) -C $(U_SPI_HOME)/config/spgtools/src/lex
	$(MAKE) -C $(U_SPI_HOME)/config/spgtools

debug:
	$(MAKE) run DEBUG=1 ARGS=-w

rstrip:
	@$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/rstrip.py *.cpp

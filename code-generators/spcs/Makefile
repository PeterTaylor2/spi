###########################################################################
# Makefile for the spcs executable.
###########################################################################
target:

ARGS=
U_SPI_HOME=../..
U_TARGET=spcs

include $(U_SPI_HOME)/makefiles/config/config.mk

U_DEP_LIBS = \
 ../spgtools/$(G_BUILD_DIR)/$(G_LIB_PFX)spgtools$(G_LIB_EXT)\
 $(U_SPI_HOME)/lib/$(G_BUILD_DIR)/$(G_LIB_PFX)spi$(G_LIB_EXT)\
 $(U_SPI_HOME)/spi_util/lib/$(G_BUILD_DIR)/$(G_LIB_PFX)spi_util$(G_LIB_EXT)

U_CFLAGS += -DSPI_STATIC -DSPI_UTIL_STATIC
U_LIBS = $(U_DEP_LIBS)
U_SRC_DIR = src
U_INC_DIR = src
U_INCLUDES = -I$(U_SPI_HOME) -I..
U_FORCE_REBUILD = Makefile
U_VCPROJ=spcs-exe
G_OVERRIDE_NEW_DELETE=0

include $(U_SPI_HOME)/makefiles/build/exe.mk

libs:
	$(MAKE) -C ../spgtools
	$(MAKE) -C ../../lib

debug:
	$(MAKE) run DEBUG=1 ARGS="-w $(ARGS)"

rstrip:
	@$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/rstrip.py src/*.[ch]pp Makefile

post_build: bin

bin: ../bin/$(G_ABI)/$(notdir $(I_TARGET))

../bin/$(G_ABI)/$(notdir $(I_TARGET)): $(I_TARGET)
	@mkdir -p ../bin/$(G_ABI)
	cp -f $(I_TARGET) ../bin/$(G_ABI)




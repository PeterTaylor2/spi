#
# Makefile to build the official released binaries using the config compiler
#
# Thus somebody using the code generators can use them from the canonical
# locations
#

default: build

U_SPI_HOME=..
include $(U_SPI_HOME)/makefiles/config/config.mk

BUILD_DIRS=\
../spi_util/lib\
../lib\
spgtools\
spcl\
spc\
spcs\
sppy\
sptex\
spxl

U_VCPROJ=config-build
U_VCPROJ_OPTIONS=-tconfig-build
U_SRC_DIR=.
U_INC_DIR=.

include $(U_SPI_HOME)/makefiles/build/vcproj.mk

info::
	@echo G_PLATFORM=$(G_PLATFORM)
	@echo BUILD_DIRS=$(BUILD_DIRS)
	@echo G_CONFIG_COMPILER=$(G_CONFIG_COMPILER)
	@echo G_CONFIG_COMPILER_BITS=$(G_CONFIG_COMPILER_BITS)
	@echo G_CONFIG_COMPILER_ABI=$(G_CONFIG_COMPILER_ABI)

build::
	@for lib in $(BUILD_DIRS); do \
		echo Building $$lib; \
		$(MAKE) -C $$lib; \
	done

all:: build
	echo Building spi-user-guide
	@mkdir -p $(U_SPI_HOME)/config/bin-$(G_PLATFORM)
	$(MAKE) -C config/spcl doc COMPILER=$(G_CONFIG_COMPILER) BITS=$(G_CONFIG_COMPILER_BITS)

clean::
	rm -fr $(U_SPI_HOME)/config/bin-$(G_PLATFORM)
	@for lib in $(BUILD_DIRS); do \
		$(MAKE) -C $$lib clean; \
	done

config-build::
	@$(MAKE) build DEBUG=0 COMPILER=$(G_CONFIG_COMPILER) BITS=$(G_CONFIG_COMPILER_BITS)
	mkdir -p $(U_SPI_HOME)/config/bin-$(G_PLATFORM)
	cp $(U_SPI_HOME)/config/bin/$(G_CONFIG_COMPILER_ABI)/*$(G_EXE) $(U_SPI_HOME)/config/bin-$(G_PLATFORM)
	rm -fr $(U_SPI_HOME)/config/bin

rebuild: clean build

code:
	make -C spcl types-code
	make -C spdoc

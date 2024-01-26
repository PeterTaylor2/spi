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

EXTRA_VCPROJ_DIRS=spcl/types spdoc

U_VCPROJ=config-build
U_VCPROJ_OPTIONS=-j4 -tconfig-build --cleanTarget=config-clean --silent
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

# note that after copying to the bin-$(G_PLATFORM) directory we need to delete the bin directory
# since the latter directory may take precedence in makefiles
config-build::
	@$(MAKE) build DEBUG=0 COMPILER=$(G_CONFIG_COMPILER) BITS=$(G_CONFIG_COMPILER_BITS)
	mkdir -p bin-$(G_PLATFORM)
	$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/copyFiles.py bin-$(G_PLATFORM) bin/$(G_CONFIG_COMPILER_ABI)/*$(G_EXE)
	rm -fr $(U_SPI_HOME)/config/bin

config-clean::
	@$(MAKE) clean DEBUG=0 COMPILER=$(G_CONFIG_COMPILER) BITS=$(G_CONFIG_COMPILER_BITS)
	rm -fr $(U_SPI_HOME)/config/bin/$(G_CONFIG_COMPILER_ABI)

rebuild: clean build

code:
	make -C spcl types-code
	make -C spdoc

vs-translate:
	@echo "Translating spi-config-vs16.sln to spi-config-vs17.sln"
	@$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/translateVS.py spi-config-vs16.sln spi-config-vs17.sln > spi-config-vs17.log
	@echo "See spi-config-vs17.log for details"

vc-all:
	@$(MAKE) -s v16.vcxproj U_VCPROJ=refresh-projects U_VCPROJ_OPTIONS="-j1 -tvc-all"
	@$(MAKE) -s v17.vcxproj U_VCPROJ=refresh-projects U_VCPROJ_OPTIONS="-j1 -tvc-all"
	@$(MAKE) -s v16.vcxproj U_VCPROJ=config-build U_VCPROJ_OPTIONS="-j4 -tconfig-build --cleanTarget=config-clean --silent"
	@$(MAKE) -s v17.vcxproj U_VCPROJ=config-build U_VCPROJ_OPTIONS="-j4 -tconfig-build --cleanTarget=config-clean --silent"
	@$(MAKE) -s v16.vcxproj U_VCPROJ=all U_VCPROJ_OPTIONS="-j4 -tbuild"
	@$(MAKE) -s v17.vcxproj U_VCPROJ=all U_VCPROJ_OPTIONS="-j4 -tbuild"
	@for lib in $(BUILD_DIRS) $(EXTRA_VCPROJ_DIRS); do \
		echo Creating vcproject for $$lib; \
		$(MAKE) -s -C $$lib v16.vcxproj; \
		$(MAKE) -s -C $$lib v17.vcxproj; \
	done

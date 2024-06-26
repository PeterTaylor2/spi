default: build

U_SPI_HOME=.
include $(U_SPI_HOME)/makefiles/config/config.mk

RUNTIME_BUILD_DIRS=\
spi_boost/lib\
spi_curl\
spi_util\
dll\
c\
excel\
python\
config/spdoc\
svo/spdoc\
replay/config\
replay/dll\
replay/python\
xlcall32\
xltest

CONFIG_BUILD_DIRS=\
spi_util/lib\
lib\
makeXLAddin\
config/spgtools\
config/spcl\
config/spc\
config/spcs\
config/sppy\
config/sptex\
config/spxl

EXTRA_VCPROJ_DIRS=\
makefiles/gendep\
config\
config/spdoc\
config/spcl/types

BUILD_DIRS=$(CONFIG_BUILD_DIRS) $(RUNTIME_BUILD_DIRS)

.PHONY: package package.zip

rstrip::
	@for lib in $(BUILD_DIRS); do \
		echo Cleaning $$lib; \
		$(MAKE) -C $$lib rstrip; \
	done

runtime::
	@for lib in $(RUNTIME_BUILD_DIRS); do \
		echo Building $$lib; \
		$(MAKE) -C $$lib all; \
	done

build::
	@for lib in $(BUILD_DIRS); do \
		echo Building $$lib; \
		$(MAKE) -C $$lib; \
	done

all::
	@for lib in $(BUILD_DIRS); do \
		echo Building $$lib; \
		$(MAKE) -C $$lib all; \
	done
	echo Building spi-user-guide
	$(MAKE) -C config/spcl doc

clean::
	@for lib in $(BUILD_DIRS); do \
		$(MAKE) -C $$lib clean; \
	done

clean-all::
	@for lib in $(BUILD_DIRS); do \
		$(MAKE) -C $$lib clean-all; \
	done

U_VCPROJ=refresh-projects
U_VCPROJ_OPTIONS=-j1 -tvc-all
U_SRC_DIR=.
U_INC_DIR=.

include $(U_SPI_HOME)/makefiles/build/vcproj.mk

rebuild: clean build

test:
	@$(MAKE) -C test all

vc-all:
	$(MAKE) -s v16.vcxproj U_VCPROJ=refresh-projects U_VCPROJ_OPTIONS="-j1 -tvc-all"
	$(MAKE) -s v17.vcxproj U_VCPROJ=refresh-projects U_VCPROJ_OPTIONS="-j1 -tvc-all"
	$(MAKE) -s v16.vcxproj U_VCPROJ=all U_VCPROJ_OPTIONS="-j4 -tbuild"
	$(MAKE) -s v17.vcxproj U_VCPROJ=all U_VCPROJ_OPTIONS="-j4 -tbuild"
	@for lib in $(BUILD_DIRS) $(EXTRA_VCPROJ_DIRS); do \
		echo Creating vcproject for $$lib; \
		$(MAKE) -s -C $$lib v16.vcxproj; \
		$(MAKE) -s -C $$lib v17.vcxproj; \
	done

vc-all-runtime:
	@for lib in $(RUNTIME_BUILD_DIRS); do \
		echo Creating vcproject for $$lib; \
		$(MAKE) -s -C $$lib v16.vcxproj; \
		$(MAKE) -s -C $$lib v17.vcxproj; \
	done


WIN32_COMPILERS=msvc16 msvc17

win32:
	@for compiler in $(WIN32_COMPILERS); do \
		echo "=========================================================="; \
		echo "Building for COMPILER=$$compiler" DEBUG=$(DEBUG) BITS=32; \
		echo "=========================================================="; \
		$(MAKE) build COMPILER=$$compiler BITS=32; \
		$(MAKE) build COMPILER=$$compiler BITS=32; \
	done

win64:
	@for compiler in $(WIN32_COMPILERS); do \
		echo "=========================================================="; \
		echo "Building for COMPILER=$$compiler" DEBUG=$(DEBUG) BITS=64; \
		echo "=========================================================="; \
		$(MAKE) build COMPILER=$$compiler BITS=64; \
		$(MAKE) build COMPILER=$$compiler BITS=64; \
	done


vs16.sln:
	$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/translateVS.py --old=17 --new=16 spi-vs17.sln spi-vs16.sln

vs17.sln:
	$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/translateVS.py --old=16 --new=17 spi-vs16.sln spi-vs17.sln


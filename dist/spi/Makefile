default: build

U_SPI_HOME=.
SARTORIAL_CONFIG?=win64
include $(U_SPI_HOME)/makefiles/config/config.mk

BUILD_DIRS=\
spi_boost/lib\
zlib\
spi_curl\
spi_util\
dll\
c\
makeXLAddin\
excel\
python\
svo/spdoc\
replay/dll\
replay/python\
xlcall32\
xltest

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

clean::
	@for lib in $(BUILD_DIRS); do \
		$(MAKE) -C $$lib clean; \
	done

clean-all::
	@for lib in $(BUILD_DIRS); do \
		$(MAKE) -C $$lib clean-all; \
	done

include $(U_SPI_HOME)/makefiles/build/vcproj.mk

rebuild: clean build

test:
	@$(MAKE) -C test all

vc-all:
	@$(MAKE) -s v16.vcxproj U_VCPROJ=refresh-projects U_VCPROJ_OPTIONS="-j1 -tvc-all" U_SRC_DIR=. U_INC_DIR=None
	@$(MAKE) -s v17.vcxproj U_VCPROJ=refresh-projects U_VCPROJ_OPTIONS="-j1 -tvc-all" U_SRC_DIR=. U_INC_DIR=None
	@$(MAKE) -s v16.vcxproj U_VCPROJ=all U_VCPROJ_OPTIONS="-j4 -tbuild" U_SRC_DIR=. U_INC_DIR=None
	@$(MAKE) -s v17.vcxproj U_VCPROJ=all U_VCPROJ_OPTIONS="-j4 -tbuild" U_SRC_DIR=. U_INC_DIR=None
	@for lib in $(BUILD_DIRS); do \
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



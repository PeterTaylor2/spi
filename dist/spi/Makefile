default: build

U_SPI_HOME=.
SARTORIAL_CONFIG?=win64
include $(U_SPI_HOME)/makefiles/config/config.mk

BUILD_DIRS=\
spi_boost/lib\
spi_curl\
spi_curl/dll\
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

v16.vcxproj:
	@$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/makeVcproj16.py -b $(U_SPI_HOME)/build-win32/bin --makefileTarget=build --cleanTarget=clean spi.v16.vcxproj spi spi/src spi
	@for lib in $(BUILD_DIRS); do \
		echo Creating vcproject for $$lib; \
		$(MAKE) -C $$lib v16.vcxproj; \
	done

v17.vcxproj:
	@$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/makeVcproj17.py -b $(U_SPI_HOME)/build-win32/bin --makefileTarget=build --cleanTarget=clean spi.v17.vcxproj spi spi/src spi
	@for lib in $(BUILD_DIRS); do \
		echo Creating vcproject for $$lib; \
		$(MAKE) -C $$lib v17.vcxproj; \
	done

rebuild: clean build

test:
	@$(MAKE) -C test all

vc-all:
	$(MAKE) -s v16.vcxproj
	$(MAKE) -s v17.vcxproj

win32:
	@for compiler in msvc16 msvc17; do \
		echo "=========================================================="; \
		echo "Building for COMPILER=$$compiler" DEBUG=$(DEBUG) BITS=32; \
		echo "=========================================================="; \
		$(MAKE) build COMPILER=$$compiler BITS=32; \
		$(MAKE) build COMPILER=$$compiler BITS=32; \
	done

win64:
	@for compiler in msvc16 msvc17; do \
		echo "=========================================================="; \
		echo "Building for COMPILER=$$compiler" DEBUG=$(DEBUG) BITS=64; \
		echo "=========================================================="; \
		$(MAKE) build COMPILER=$$compiler BITS=64; \
		$(MAKE) build COMPILER=$$compiler BITS=64; \
	done

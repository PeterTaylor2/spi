default: build

U_SPI_HOME=.
include $(U_SPI_HOME)/makefiles/config/config.mk

RUNTIME_BUILD_DIRS=\
spi_boost/lib\
zlib\
spi_curl\
spi_util\
dll\
c\
excel\
python\
code-generators/spdoc\
svo/spdoc\
replay/config\
replay/dll\
replay/python\
xlcall32\
xltest

EXTRA_VCPROJ_DIRS=\
makefiles\
makefiles/gendep\
makefiles/cversion

BUILD_DIRS=$(RUNTIME_BUILD_DIRS) code-generators

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

code-generators::
	$(MAKE) -C code-generators config-build

clean-runtime::
	@for lib in $(RUNTIME_BUILD_DIRS); do \
		$(MAKE) -C $$lib clean; \
	done

clean-all-runtime::
	@for lib in $(RUNTIME_BUILD_DIRS); do \
		$(MAKE) -C $$lib clean-all; \
	done

clean-code-generators::
	$(MAKE) -C code-generators config-clean

build::
ifeq ($(G_PLATFORM),win32)
	@$(MAKE) -C makefiles/gendep config-install
endif
	@$(MAKE) code-generators
	@$(MAKE) runtime

all::
	@$(MAKE) build
	echo Building spi-user-guide
	@$(MAKE) -C config/spcl doc

clean::
	@$(MAKE) clean-code-generators
	@$(MAKE) clean-runtime

clean-all::
	@$(MAKE) clean-code-generators
	@$(MAKE) clean-all-runtime

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
	$(MAKE) -s v18.vcxproj U_VCPROJ=refresh-projects U_VCPROJ_OPTIONS="-j1 -tvc-all"
	$(MAKE) -s v16.vcxproj U_VCPROJ=all U_VCPROJ_OPTIONS="-j4 -tbuild"
	$(MAKE) -s v17.vcxproj U_VCPROJ=all U_VCPROJ_OPTIONS="-j4 -tbuild"
	$(MAKE) -s v18.vcxproj U_VCPROJ=all U_VCPROJ_OPTIONS="-j4 -tbuild"
	$(MAKE) -C code-generators vc-all
	@for lib in $(BUILD_DIRS) $(EXTRA_VCPROJ_DIRS); do \
		echo Creating vcproject for $$lib; \
		$(MAKE) -s -C $$lib v16.vcxproj; \
		$(MAKE) -s -C $$lib v17.vcxproj; \
		$(MAKE) -s -C $$lib v18.vcxproj; \
	done

vc-all-runtime:
	@for lib in $(RUNTIME_BUILD_DIRS); do \
		echo Creating vcproject for $$lib; \
		$(MAKE) -s -C $$lib v16.vcxproj; \
		$(MAKE) -s -C $$lib v17.vcxproj; \
		$(MAKE) -s -C $$lib v18.vcxproj; \
	done


WIN32_COMPILERS=msvc16 msvc17 msvc18

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

vs18.sln:
	$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/translateVS.py --old=17 --new=18 spi-vs17.sln spi-vs18.sln


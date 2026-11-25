default: build

U_SPI_HOME=.
include $(U_SPI_HOME)/makefiles/config/config.mk

CORE_RUNTIME_BUILD_DIRS=\
zlib\
spi_curl\
spi_util\
dll

RUNTIME_BUILD_DIRS=\
$(CORE_RUNTIME_BUILD_DIRS)\
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

TEST_BUILD_DIRS=\
test/lib\
test/config\
test/dll\
test

EXTRA_VCPROJ_DIRS=\
makefiles\
makefiles/gendep\
makefiles/cversion

BUILD_DIRS=$(RUNTIME_BUILD_DIRS) code-generators $(TEST_BUILD_DIRS)

.PHONY: package package.zip code-generators

rstrip::
	@for lib in $(BUILD_DIRS); do \
		echo Cleaning $$lib; \
		$(MAKE) -C $$lib rstrip; \
	done

core-runtime::
	+@$(call G_MAKE_BUILD_DIRS,$(CORE_RUNTIME_BUILD_DIRS),all)

runtime::
	+@$(call G_MAKE_BUILD_DIRS,$(RUNTIME_BUILD_DIRS),all)

# if somebody wants to generate code before building the runtime
# then they will likely use the code-generators target
# hence we must build gendep in this step
#
# note that the build target builds the code-generators first

code-generators::
	@$(MAKE) --no-print-directory -C code-generators all

testing::
	+@$(call G_MAKE_BUILD_DIRS,$(TEST_BUILD_DIRS))

clean-runtime::
	+@$(call G_MAKE_BUILD_DIRS,$(RUNTIME_BUILD_DIRS),clean,Cleaning)

clean-all-runtime::
	+@$(call G_MAKE_BUILD_DIRS,$(RUNTIME_BUILD_DIRS),clean-all,Cleaning)

clean-code-generators::
	$(MAKE) -C code-generators clean-all

ifdef G_USE_GENDEP

gendep::
	@echo Building makefiles/gendep
	@$(MAKE) --no-print-directory -C makefiles/gendep config-install

else

gendep::

endif

pre-build::
ifeq ($(G_PLATFORM),win32)
	$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/vsVersions.py
	@$(MAKE) gendep 
endif
ifeq ($(G_PLATFORM),linux64)
	$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/pyLinuxVersions.py $(U_SPI_HOME)/makefiles/config/computers/$(COMPUTERNAME).mk
endif
ifeq ($(G_PLATFORM),macos64)
	$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/pyMacosVersions.py $(U_SPI_HOME)/makefiles/config/computers/$(COMPUTERNAME).mk
endif


build::
	@$(MAKE) code-generators
	@$(MAKE) --no-print-directory runtime

all::
	@$(MAKE) build
	echo Building spi-user-guide
	@$(MAKE) -C doc

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
	$(MAKE) -s v17.vcxproj U_VCPROJ=refresh-projects U_VCPROJ_OPTIONS="-j1 -tvc-all"
	$(MAKE) -s v18.vcxproj U_VCPROJ=refresh-projects U_VCPROJ_OPTIONS="-j1 -tvc-all"
	$(MAKE) -s v17.vcxproj U_VCPROJ=all U_VCPROJ_OPTIONS="-j4 -tbuild"
	$(MAKE) -s v18.vcxproj U_VCPROJ=all U_VCPROJ_OPTIONS="-j4 -tbuild"
	$(MAKE) -C code-generators vc-all
	+@$(call G_MAKE_BUILD_DIRS,$(BUILD_DIRS) $(EXTRA_VCPROJ_DIRS),v17.vcxproj,Creating v17.vcxproj for,-j1)
	+@$(call G_MAKE_BUILD_DIRS,$(BUILD_DIRS) $(EXTRA_VCPROJ_DIRS),v18.vcxproj,Creating v18.vcxproj for,-j1)

vc-all-runtime:
	+@$(call G_MAKE_BUILD_DIRS,$(RUNTIME_BUILD_DIRS),v17.vcxproj,Creating v17.vcxproj for,-j1)
	+@$(call G_MAKE_BUILD_DIRS,$(RUNTIME_BUILD_DIRS),v18.vcxproj,Creating v18.vcxproj for,-j1)

WIN32_COMPILERS=msvc17 msvc18

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


vs17.sln:
	$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/translateVS.py --old=18 --new=17 spi-vs18.sln spi-vs17.sln
	$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/translateVS.py --old=18 --new=17 spi-vs18-python.sln spi-vs17-python.sln

vs18.sln:
	$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/translateVS.py --old=17 --new=18 spi-vs17.sln spi-vs18.sln
	$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/translateVS.py --old=17 --new=18 spi-vs17-python.sln spi-vs18-python.sln


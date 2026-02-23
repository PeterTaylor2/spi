#
# Makefile to build the official released binaries using the config compiler
#
# Thus somebody using the code generators can use them from the canonical
# locations
#

default: build

U_SPI_HOME=..

# overriding DEBUG ensures that in this makefike we get the correct definition
# of G_ABI that we need for the step  of copying the executables to the
# relevant bin-<PLATFORM> directory
#
# however if you specify DEBUG=1 at the command line then the build and clean
# targets respect that particular value of DEBUG
#
# the all and clean-all targets always use DEBUG=0 for build and clean

override DEBUG=0

include $(U_SPI_HOME)/makefiles/config/config.mk

BUILD_DIRS=\
../zlib\
../spi_util/lib\
../lib\
../makeXLAddin\
spgtools\
spcl\
spc\
spcs\
sppy\
sptex\
spxl

EXTRA_VCPROJ_DIRS=spcl/types spdoc

U_VCPROJ=all-code-generators
U_VCPROJ_OPTIONS=-j4 -tall --cleanTarget=clean-all --silent
U_SRC_DIR=.
U_INC_DIR=.

include $(U_SPI_HOME)/makefiles/build/vcproj.mk

info::
	@echo G_PLATFORM=$(G_PLATFORM)
	@echo BUILD_DIRS=$(BUILD_DIRS)
	@echo DEBUG=$(DEBUG)
	@echo G_ABI=$(G_ABI)

build::
	@for lib in $(BUILD_DIRS); do \
		echo Building $$lib; \
		$(MAKE) -C $$lib; \
	done

clean::
	@for lib in $(BUILD_DIRS); do \
		$(MAKE) -C $$lib clean; \
	done

# note that after copying to the bin-$(G_PLATFORM) directory we keep the bin/$(G_ABI)
# directory since to use that in code generator makefiles you need to define LOCAL_BUILD
#
# note that all and clean-all are the targets used by the all-code-generators visual
# studio project files

all::
	@$(MAKE) build DEBUG=0
	@mkdir -p bin-$(G_PLATFORM)
	$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/copyFiles.py bin-$(G_PLATFORM) bin/$(G_ABI)/*$(G_EXE)

clean-all::
	@$(MAKE) clean DEBUG=0
	rm -fr $(U_SPI_HOME)/code-generators/bin/$(G_ABI)
	rm -fr $(U_SPI_HOME)/code-generators/bin-$(G_PLATFORM)

rebuild: clean build

code:
	make -C spcl types-code
	make -C spdoc

vc-all:
	@echo Creating vcproject for all-code-generators
	@$(MAKE) -s v16.vcxproj
	@$(MAKE) -s v17.vcxproj
	@$(MAKE) -s v18.vcxproj
	@for lib in $(BUILD_DIRS) $(EXTRA_VCPROJ_DIRS); do \
		echo Creating vcproject for $$lib; \
		$(MAKE) -s -C $$lib v16.vcxproj; \
		$(MAKE) -s -C $$lib v17.vcxproj; \
		$(MAKE) -s -C $$lib v18.vcxproj; \
	done

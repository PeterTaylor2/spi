###########################################################################
# Makefile for the spcl executable.
###########################################################################
target:

ARGS=
U_SPI_HOME=../..
U_TARGET=spcl

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
U_VCPROJ=spcl-exe
U_VCPROJ_OPTIONS=--exe=..\bin\G_ABI\spcl.exe
G_OVERRIDE_NEW_DELETE=0

include $(U_SPI_HOME)/makefiles/build/exe.mk

libs:
	$(MAKE) -C ../spgtools
	$(MAKE) -C ../../lib

debug:
	$(MAKE) run DEBUG=1 ARGS="-w $(ARGS)"

rstrip:
	@$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/rstrip.py src/*.cpp src/*.hpp Makefile

.PHONY: doc doc-debug types-code debug rstrip libs target bin

post_build: bin

bin: ../bin/$(G_ABI)/$(notdir $(I_TARGET))

../bin/$(G_ABI)/$(notdir $(I_TARGET)): $(I_TARGET)
	@mkdir -p ../bin/$(G_ABI)
	cp $(I_TARGET) ../bin/$(G_ABI)

ifdef G_NO_PDFLATEX

doc:
	@echo Cannot create user-guide without pdflatex

else

doc: libs
	$(MAKE) run ARGS="-d ../../doc/spi-user-guide/parser-autodoc.tex"
ifeq ($(G_PLATFORM),win32)
	@cd ../../doc; pdflatex -quiet spi-user-guide.tex
	@cd ../../doc; pdflatex -quiet spi-user-guide.tex
	cd ../../doc; pdflatex -quiet spi-user-guide.tex
else
	@cd ../../doc; pdflatex spi-user-guide.tex > /dev/null
	@cd ../../doc; pdflatex spi-user-guide.tex > /dev/null
	cd ../../doc; pdflatex spi-user-guide.tex > /dev/null
endif

endif

doc-debug:
	$(MAKE) run DEBUG=1 ARGS="-d -w ../../doc/spi-user-guide/parser-autodoc.tex"

types-code:
	@$(MAKE) -C types clean
	@$(MAKE) -C types
	@cp -pf types/types*.h src
	@cp -pf types/types*.hpp src
	@cp -pf types/src/types*.[ch]pp src


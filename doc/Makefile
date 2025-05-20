doc:

U_SPI_HOME=..
include $(U_SPI_HOME)/makefiles/config/config.mk

U_VCPROJ=spi-doc
U_VCPROJ_OPTIONS=-F 'spi-user-guide.tex' -F 'spi-user-guide/*.tex' -tdoc
U_SRC_DIR=.
U_INC_DIR=.

include $(U_SPI_HOME)/makefiles/build/vcproj.mk

ifdef G_NO_PDFLATEX

doc:
	@echo Cannot create user-guide without pdflatex

else

doc:
	@$(MAKE) tex
	@$(MAKE) pdf

endif

.PHONY: tex pdf doc all clean

all: doc

###########################################################################
# Commands to generate the parser_autodoc.tex file
###########################################################################
U_SPCL?=spcl

I_SPCL=$(wildcard $(U_SPI_HOME)/config/bin/$(G_ABI)/$(U_SPCL)$(G_EXE))
ifeq "$(I_SPCL)" ""
I_SPCL:=$(U_SPI_HOME)/config/bin-$(G_PLATFORM)/$(U_SPCL)$(G_EXE)
endif

tex: spi-user-guide/parser-autodoc.tex

spi-user-guide/parser_autodoc.tex: $(I_SPCL)
	$(I_SPCL) -d spi-user-guide/parser_autodoc.tex
	
###########################################################################
# commands for creating the PDF file
###########################################################################

ifdef G_PDFLATEX_BIN_DIR
PATH:=$(G_PDFLATEX_BIN_DIR):$(PATH)
export PATH
endif

pdf: spi-user-guide.pdf

TEX_SOURCE=spi-user-guide.tex $(wildcard spi-user-guide/*.tex) $(I_SPCL)

spi-user-guide.pdf: $(TEX_SOURCE)
	@rm -f *.aux *.bbl *.blg *.log *.out *.toc
	@echo "pdflatex spi-user-guide.tex"
ifeq ($(G_PLATFORM),win32)
	@pdflatex --quiet spi-user-guide.tex
	@pdflatex --quiet spi-user-guide.tex
	@pdflatex --quiet spi-user-guide.tex
else
	@pdflatex spi-user-guide.tex > /dev/null
	@pdflatex spi-user-guide.tex > /dev/null
	@pdflatex spi-user-guide.tex > /dev/null
endif
	@rm -f *.aux *.bbl *.blg *.log *.out *.toc

###########################################################################
# clean-up command
###########################################################################
clean:
	rm -f spi-user-guide.pdf
	@rm -f *.aux *.bbl *.blg *.dvi *.log *.out *.toc

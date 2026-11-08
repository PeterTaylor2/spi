ifdef G_NO_PDFLATEX

doc:
	@echo Cannot create user-guide without pdflatex

else

doc:
	@$(MAKE) tex
	@$(MAKE) pdf

endif

.PHONY: tex pdf doc all

all: doc

include $(U_SPI_HOME)/makefiles/version.mk

###########################################################################
# commands for creating the tex file from the .svo file
###########################################################################
U_SPTEX?=sptex

ifdef LOCAL_BUILD
I_SPTEX=$(wildcard $(U_SPI_HOME)/code-generators/bin/$(G_ABI)/$(U_SPTEX)$(G_EXE))
else
I_SPTEX=
endif

ifeq "$(I_SPTEX)" ""
I_SPTEX:=$(U_SPI_HOME)/code-generators/bin-$(G_PLATFORM)/$(U_SPTEX)$(G_EXE)
endif

U_VCPROJ?=$(U_SERVICE)-$(notdir $(CURDIR))
U_CONFIG_DIR?=../config
U_OUTPUT_DIR?=..

tex: $(U_SERVICE).svo

$(U_SERVICE).svo: $(U_CONFIG_DIR)/$(U_SERVICE).svo $(I_SPTEX) $(U_EXTRA_SERVICES)
	@mkdir -p tex
	@mkdir -p tex_imports
	$(I_SPTEX) $(G_SPTEX_OPTIONS) $(U_SPTEX_OPTIONS) $(SPTEX_OPTIONS) $(U_CONFIG_DIR)/$(U_SERVICE).svo $(U_SERVICE).svo . $(U_EXTRA_SERVICES)

###########################################################################
# commands for creating the PDF file from the generated .tex files and the
# master .tex file
###########################################################################

ifdef G_PDFLATEX_BIN_DIR
PATH:=$(G_PDFLATEX_BIN_DIR):$(PATH)
export PATH
endif

U_SERVICE_DOC?=$(U_SERVICE)

pdf: $(U_OUTPUT_DIR)/$(G_ABI)/$(U_SERVICE_DOC).pdf

ifeq ($(G_PLATFORM),win32)

$(U_SERVICE_DOC).pdf: $(U_SERVICE).svo $(U_SERVICE_DOC).tex
	@rm -f *.aux *.bbl *.blg *.log *.out *.toc
	@echo "`which pdflatex` $(U_SERVICE_DOC).tex"
	@pdflatex --quiet $(U_SERVICE_DOC).tex
	@pdflatex --quiet $(U_SERVICE_DOC).tex
	@pdflatex --quiet $(U_SERVICE_DOC).tex
	@rm -f *.aux *.bbl *.blg *.out *.toc

else

$(U_SERVICE_DOC).pdf: $(U_SERVICE).svo $(U_SERVICE_DOC).tex
	@rm -f *.aux *.bbl *.blg *.log *.out *.toc
	@echo "`which pdflatex` $(U_SERVICE_DOC).tex"
	@pdflatex $(U_SERVICE_DOC).tex > /dev/null
	@pdflatex $(U_SERVICE_DOC).tex > /dev/null
	@pdflatex $(U_SERVICE_DOC).tex > /dev/null
	@rm -f *.aux *.bbl *.blg *.out *.toc

endif

$(U_OUTPUT_DIR)/$(G_ABI)/$(U_SERVICE_DOC).pdf: $(U_SERVICE_DOC).pdf
	@mkdir -p $(U_OUTPUT_DIR)/$(G_ABI)
	cp -f $< $(U_OUTPUT_DIR)/$(G_ABI)

###########################################################################
# clean-up command
###########################################################################
clean:
	rm -f $(U_SERVICE).svo tex/*.tex $(U_SERVICE_DOC).pdf
	@rm -f *.aux *.bbl *.blg *.dvi *.log *.out *.toc

help:: $(I_SPTEX)
	$(I_SPTEX) --help

###########################################################################
# visual studio project files
###########################################################################
v17.vcxproj:
	$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/makeVcproj17Config.py $(U_VCXPROJ_OPTIONS) -B '*.tex' -b $(G_CYGWIN_BIN) -t doc $(U_VCPROJ).v17.vcxproj $(U_VCPROJ) $(U_CONFIG_DIR) $(U_SERVICE_DOC).pdf

v18.vcxproj:
	@$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/makeVcproj18Config.py $(U_VCXPROJ_OPTIONS) -B '*.tex' -b $(G_CYGWIN_BIN) -t doc $(U_VCPROJ).v18.vcxproj $(U_VCPROJ) $(U_CONFIG_DIR) $(U_SERVICE_DOC).pdf



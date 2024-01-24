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

I_SPTEX=$(wildcard $(U_SPI_HOME)/config/bin-win32/$(G_ABI)/$(U_SPTEX)$(G_EXE))
ifeq "$(I_SPTEX)" ""
I_SPTEX:=$(U_SPI_HOME)/config/bin-win32/$(U_SPTEX)$(G_EXE)
endif

I_SVO2TEX_SCRIPTS=$(U_SPI_HOME)/svo/svo2tex.py $(wildcard $(U_SPI_HOME)/svo/lib/*.py)
U_SPDOC_BIN=$(U_SPI_HOME)/svo/spdoc/bin/$(G_ABI)-py$(G_PY_VERSION)
PYTHONPATH=$(U_SPDOC_BIN)
export PYTHONPATH

ifdef G_CYGWIN
PATH:=$(U_SPDOC_BIN):$(PATH)
export PATH
endif

ifeq ($(G_PLATFORM),linux64)
LD_LIBRARY_PATH:=$(U_SPDOC_BIN):$(LD_LIBRARY_PATH)
export LD_LIBRARY_PATH
endif

ifeq ($(G_PLATFORM),macos64)
DYLD_LIBRARY_PATH:=$(U_SPDOC_BIN):$(DYLD_LIBRARY_PATH)
export DYLD_LIBRARY_PATH
endif

U_VCPROJ?=$(U_SERVICE)-doc
U_CONFIG_DIR?=../config
U_OUTPUT_DIR?=..

tex: $(U_SERVICE).svo

ifdef NO_SVO2TEX

$(U_SERVICE).svo: $(U_CONFIG_DIR)/$(U_SERVICE).svo $(I_SPTEX) $(U_EXTRA_SERVICES)
	@mkdir -p tex
	@mkdir -p tex_imports
	$(I_SPTEX) $(G_SPTEX_OPTIONS) $(U_SPTEX_OPTIONS) $(U_CONFIG_DIR)/$(U_SERVICE).svo $(U_SERVICE).svo . $(U_EXTRA_SERVICES)

else

$(U_SERVICE).svo: $(U_CONFIG_DIR)/$(U_SERVICE).svo $(I_SVO2TEX_SCRIPTS) $(U_EXTRA_SERVICES)
	@mkdir -p tex
	@mkdir -p tex_imports
	@echo PYTHONPATH=$(PYTHONPATH)
	$(G_PYTHON) $(U_SPI_HOME)/svo/svo2tex.py $(G_SPTEX_OPTIONS) $(U_SPTEX_OPTIONS) $(U_CONFIG_DIR)/$(U_SERVICE).svo $(U_SERVICE).svo . $(U_EXTRA_SERVICES)

endif

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
	@pdflatex --quiet $(U_SERVICE_DOC).tex
	@pdflatex --quiet $(U_SERVICE_DOC).tex
	pdflatex --quiet $(U_SERVICE_DOC).tex
	@rm -f *.aux *.bbl *.blg *.log *.out *.toc

else

$(U_SERVICE_DOC).pdf: $(U_SERVICE).svo $(U_SERVICE_DOC).tex
	@rm -f *.aux *.bbl *.blg *.log *.out *.toc
	@echo "pdflatex $(U_SERVICE_DOC).tex"
	@pdflatex $(U_SERVICE_DOC).tex > /dev/null
	@pdflatex $(U_SERVICE_DOC).tex > /dev/null
	@pdflatex $(U_SERVICE_DOC).tex > /dev/null
	@rm -f *.aux *.bbl *.blg *.log *.out *.toc

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

###########################################################################
# visual studio project files
###########################################################################
v15.vcxproj:
	@$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/makeVcproj15Config.py $(U_VCXPROJ_OPTIONS) -b $(U_MAKEFILES)/cygwin32/bin -t doc $(U_VCPROJ).v15.vcxproj $(U_VCPROJ) $(U_CONFIG_DIR) $(U_SERVICE_DOC).pdf

v16.vcxproj:
	@$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/makeVcproj16Config.py $(U_VCXPROJ_OPTIONS) -b $(U_MAKEFILES)/cygwin32/bin -t doc $(U_VCPROJ).v16.vcxproj $(U_VCPROJ) $(U_CONFIG_DIR) $(U_SERVICE_DOC).pdf

v17.vcxproj:
	@$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/makeVcproj17Config.py $(U_VCXPROJ_OPTIONS) -b $(U_MAKEFILES)/cygwin32/bin -t doc $(U_VCPROJ).v17.vcxproj $(U_VCPROJ) $(U_CONFIG_DIR) $(U_SERVICE_DOC).pdf


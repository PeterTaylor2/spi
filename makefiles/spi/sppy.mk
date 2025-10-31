############################################################################
# code generation makefile for Python
#
# defines two targets: code-python, clean-code-python
#
# needs the following pre-defined macros
#
# U_CONFIG_DIR
# U_PYTHON_DIR
# U_SPI_HOME
#
# Optional extras:
#
# U_SPPY_OPTIONS
#
# You should have included the generated file properties.mk before invoking
# these commands - these define the following:
#
# U_SERVICE
#
# You should have included the config directory for platform and compiler.
# These define the following:
#
# G_ABI
# G_EXE
#
############################################################################

.PHONY: code-python clean-code-python

I_PYTHON_SRC:=$(U_PYTHON_DIR)/src
I_PYTHON_TARGET:=$(U_PYTHON_DIR)/$(U_SERVICE).svo
I_PYTHON_SOURCE:=$(U_CONFIG_DIR)/$(U_SERVICE).svo

############################################################################
# code generation is driven by the timestamp for I_PYTHON_TARGET
############################################################################

###########################################################################
# If you have the copy of sppy corresponding to your current $(G_ABI)
# then use it - otherwise use the one in bin-$(G_PLATFORM) directory
###########################################################################
I_SPPY:=$(wildcard $(U_SPI_HOME)/code-generators/bin/$(G_ABI)/sppy$(G_EXE))
ifeq "$(I_SPPY)" ""
I_SPPY:=$(U_SPI_HOME)/code-generators/bin-$(G_PLATFORM)/sppy$(G_EXE)
endif

code-python: $(I_PYTHON_TARGET)

$(I_PYTHON_TARGET): $(I_PYTHON_SOURCE) $(I_SPPY)
	@mkdir -p $(I_PYTHON_SRC)
	$(I_SPPY) $(G_SPPY_OPTIONS) $(U_SPPY_OPTIONS) $(I_PYTHON_SOURCE) $(I_PYTHON_TARGET) $(I_PYTHON_SRC)

clean::
	rm -f $(I_PYTHON_TARGET)

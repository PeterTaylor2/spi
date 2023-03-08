############################################################################
# code generation makefile for C#
#
# defines two targets: code-cs, clean-code-cs
#
# needs the following pre-defined macros
#
# U_CONFIG_DIR
# U_CS_DIR
# U_SPI_HOME
# U_COMPANY_NAMESPACE
#
# Optional extras:
#
# U_SPC_OPTIONS
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

.PHONY: code-c clean-code-c

I_C_SRC:=$(U_C_DIR)
I_C_TARGET:=$(U_C_DIR)/$(U_SERVICE).svo
I_C_SOURCE:=$(U_CONFIG_DIR)/$(U_SERVICE).svo

############################################################################
# code generation is driven by the timestamp for I_C_TARGET
############################################################################

###########################################################################
# If you have the copy of spc corresponding to your current $(G_ABI)
# then use it - otherwise use the one in bin directory
###########################################################################
I_SPC=$(wildcard $(U_SPI_HOME)/config/bin/$(G_ABI)/spc$(G_EXE))
ifeq "$(I_SPC)" ""
I_SPC:=$(U_SPI_HOME)/config/bin-$(G_PLATFORM)/spc$(G_EXE)
endif

code-c: $(I_C_TARGET)

spc-info:
	@echo I_SPC=$(I_SPC)
	@echo G_PLATFORM=$(G_PLATFORM)
	@echo I_C_TARGET=$(I_C_TARGET)
	@echo I_C_SOURCE=$(I_C_SOURCE)

$(I_C_TARGET): $(I_C_SOURCE) $(I_SPC)
	@mkdir -p $(I_C_SRC)
	@mkdir -p $(I_C_SRC)/src
	$(I_SPC) $(G_SPC_OPTIONS) $(U_SPC_OPTIONS) $(I_C_SOURCE) $(I_C_TARGET) $(I_C_SRC)

clean::
	rm -f $(I_C_TARGET)

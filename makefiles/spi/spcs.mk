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

.PHONY: code-cs clean-code-cs

I_CS_SRC:=$(U_CS_DIR)
I_CS_TARGET:=$(U_CS_DIR)/$(U_SERVICE).svo
I_CS_SOURCE:=$(U_CONFIG_DIR)/$(U_SERVICE).svo

############################################################################
# code generation is driven by the timestamp for I_CS_TARGET
############################################################################

###########################################################################
# If you have the copy of spcs corresponding to your current $(G_ABI)
# then use it - otherwise use the one in bin directory
###########################################################################
ifdef LOCAL_BUILD
I_SPCS=$(wildcard $(U_SPI_HOME)/code-generators/bin/$(G_ABI)/spcs$(G_EXE))
else
I_SPCS=
endif

ifeq "$(I_SPCS)" ""
I_SPCS:=$(U_SPI_HOME)/code-generators/bin-$(G_PLATFORM)/spcs$(G_EXE)
endif

code-cs: $(I_CS_TARGET)

$(I_CS_TARGET): $(I_CS_SOURCE) $(I_SPCS)
	@mkdir -p $(I_CS_SRC)
	$(I_SPCS) $(G_SPCS_OPTIONS) $(U_SPCS_OPTIONS) $(I_CS_SOURCE) $(I_CS_TARGET) $(I_CS_SRC) $(U_COMPANY_NAMESPACE) $(U_CS_DLL_NAME)

clean::
	rm -f $(I_CS_TARGET)

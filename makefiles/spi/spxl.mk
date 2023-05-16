############################################################################
# code generation makefile for Excel
#
# defines two targets: code-excel, clean-code-excel
#
# needs the following pre-defined macros
#
# U_CONFIG_DIR
# U_EXCEL_DIR
# U_SPI_HOME
#
# Optional extras:
#
# U_SPXL_OPTIONS
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

.PHONY: code-excel clean-code-excel

I_EXCEL_TARGET:=$(U_EXCEL_DIR)/$(U_SERVICE).svo
I_EXCEL_SOURCE:=$(U_CONFIG_DIR)/$(U_SERVICE).svo

############################################################################
# code generation is driven by the timestamp for I_EXCEL_TARGET
############################################################################

###########################################################################
# If you have the copy of spxl corresponding to your current $(G_ABI)
# then use it - otherwise use the one in bin-$(G_PLATFORM) directory
###########################################################################
I_SPXL:=$(wildcard $(U_SPI_HOME)/config/bin/$(G_ABI)/spxl$(G_EXE))
ifeq "$(I_SPXL)" ""
I_SPXL:=$(U_SPI_HOME)/config/bin-$(G_PLATFORM)/spxl$(G_EXE)
endif

code-excel: $(I_EXCEL_TARGET)

I_VBA_SOURCE:=$(U_SPI_HOME)/excel/vba/utils.bas\
$(U_SPI_HOME)/excel/vba/functions.bas\
$(U_SPI_HOME)/excel/vba/ObjectViewer.frm\
$(U_SPI_HOME)/excel/vba/ObjectViewer.frx

ifeq ($(G_PLATFORM),win32)

$(I_EXCEL_TARGET): $(I_EXCEL_SOURCE) $(I_SPXL) $(I_VBA_SOURCE)
	@mkdir -p $(U_EXCEL_DIR)/src
	$(I_SPXL) $(G_SPXL_OPTIONS) $(U_SPXL_OPTIONS) $(I_EXCEL_SOURCE) $(I_EXCEL_TARGET) $(U_EXCEL_DIR)/src $(U_SPI_HOME)/excel/vba

else

$(I_EXCEL_TARGET):

endif


clean::
	rm -f $(I_EXCEL_TARGET)


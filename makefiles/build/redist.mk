#
# if you want to copy the MS windows redist files then use the target redist and set U_DLL_OUTPUT_DIR
#
# for example sometimes the copy is driven by the dll target, so you would do the following
#
# dll: redist
#
# U_DLL_OUTPUT_DIR=wherever
#
# include $(U_SPI_HOME)/makefiles/build/redist.mk
#

ifeq ($(G_PLATFORM),win32)

redist:
	@$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/copyRedist.py $(U_DLL_OUTPUT_DIR) "$(G_WINDOWS_REDIST_DLLS_HOME)"

redist-info:
	@echo "G_WINDOWS_REDIST_DLLS_HOME=$(G_WINDOWS_REDIST_DLLS_HOME)"

else

redist:

redist-info:

endif



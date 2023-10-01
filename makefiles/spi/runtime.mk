###########################################################################
# Defines the location of the SPI run-time libraries
###########################################################################
# if you build the SPI run-time libraries they can be found in bin/$(G_ABI)
# otherwise pre-built versions using optimised code will be used
###########################################################################
ifeq "$(wildcard $(U_SPI_HOME)/bin/$(G_ABI)/*$(G_DLL_EXT))" ""
I_SPI_RUNTIME_BIN_DIR=$(U_SPI_HOME)/bin/$(subst Debug,Release,$(G_ABI))
else
I_SPI_RUNTIME_BIN_DIR=$(U_SPI_HOME)/bin/$(G_ABI)
endif

include $(U_SPI_HOME)/makefiles/version.mk

runtime-info::
	@echo I_SPI_RUNTIME_BIN_DIR=$(I_SPI_RUNTIME_BIN_DIR)
	@echo DLLS=$(wildcard $(I_SPI_RUNTIME_BIN_DIR)/*$(G_DLL_EXT))
	@echo SPI_DLL=$(SPI_DLL)
	@echo SPI_UTIL_DLL=$(SPI_UTIL_DLL)
	@echo SPI_CURL_DLL=$(SPI_CURL_DLL)



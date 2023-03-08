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

runtime-info::
	@echo I_SPI_RUNTIME_BIN_DIR=$(I_SPI_RUNTIME_BIN_DIR)
	@echo DLLS=$(wildcard $(I_SPI_RUNTIME_BIN_DIR)/*$(G_DLL_EXT))


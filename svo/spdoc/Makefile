pyd:

U_SPI_HOME=../..
U_PYTHON_BUILD=1

include $(U_SPI_HOME)/makefiles/config/config.mk

# U_INCLUDES=$(G_BOOST_INCLUDES)

# this is an unusual build since the DLL is actually spi.dll
# since the generated C++ code is built into spi.dll
# hence some unusual paths and we don't use properties.mk

U_INCLUDES+=-I$(U_SPI_HOME)/spi

U_DLL_DIR=$(U_SPI_HOME)/dll
U_OUTPUT_DIR=bin

U_DECLSPEC=SPI
U_SERVICE_DLL=$(SPI_DLL)
U_SERVICE=spdoc
U_SERVICE_NAMESPACE=spdoc
U_SERVICE_SVO=spdoc.svo

info::
	@echo U_SERVICE_DLL=$(U_SERVICE_DLL)
	@echo SPI_DLL=$(SPI_DLL)

bin/$(G_ABI)/spdoc.svo: $(U_SPI_HOME)/spi/spdoc.svo
	@mkdir -p bin/$(G_ABI)
	cp -f $< $@

clean::
	@rm -fr bin/$(G_ABI)

include $(U_SPI_HOME)/makefiles/spi/pyd.mk


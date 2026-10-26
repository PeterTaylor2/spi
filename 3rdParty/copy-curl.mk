help:

.PHONY: all copy clean 

help:
	@echo "First download the curl source code from https://github.com/curl/curl/tags for the version required"
	@echo ""
	@echo "That will provide the code for the curl application and the libcurl library"
	@echo ""
	@echo "We only want the code for libcurl and we use this file to extract it"
	@echo "Hence the name of the source should be something like curl-curl-V1-V2-V3"
	@echo "And the name of the target should be something like curl.V1.V2.V3"
	@echo ""
	@echo "In the build system, to determine which version of curl to be using amend the file"
	@echo "makefiles/config/win32_curl.mk"	
	@echo ""
	@echo "make -f copy-curl.mk copy SOURCE=<path_to_source> TARGET=<path_to_target>"
	@echo "  --- copies curl source code in the directory structure expected by spi_curl build"

U_SPI_HOME=..

include $(U_SPI_HOME)/makefiles/config/config.mk

COPY_MANIFEST=$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/copyManifest.py

clean: 
	rm -fr $(TARGET)

copy:
ifndef TARGET
	@echo "TARGET undefined" && exit 1
endif
ifndef SOURCE
	@echo "SOURCE undefined" && exit 1
endif
	@$(COPY_MANIFEST) curl.man $(SOURCE) $(TARGET)



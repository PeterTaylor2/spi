help:

.PHONY: all copy clean 

help:
	@echo "First download the zlib source code from https://github.com/zlib/zlib/tags for the version required"
	@echo "Copy the files into the downloads directory"
	@echo ""
	@echo "That will provide the code for zlib - but we don't need it all so we will cut it down to size."
	@echo ""
	@echo "In the build system, to determine which version of zlib to be using amend the file"
	@echo "makefiles/config/zlib.mk"	
	@echo ""
	@echo "make -f copy-zlib.mk copy ZLIB_VERSION=x.y.z"
	@echo "  --- copies zlib from downloads/zlib-x.y.z to zlib-x.y.z in directory structure expected by zlib build"

U_SPI_HOME=..

include $(U_SPI_HOME)/makefiles/config/config.mk

COPY_MANIFEST=$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/copyManifest.py

clean: 
	rm -fr $(TARGET)

copy:
ifndef ZLIB_VERSION
	@echo "ZLIB_VERSION undefined" && exit 1
endif
	@$(COPY_MANIFEST) zlib.man downloads/zlib-$(ZLIB_VERSION) zlib-$(ZLIB_VERSION)



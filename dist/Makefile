help:

.PHONY: all copy clean doc zip doc-zip tar build doc-clean

help:
	@echo "make copy-runtime TARGET=<path_to_spi_directory>"
	@echo "  --- copies runtime libraries"
	@echo "make copy-config-source TARGET=<path_to_spi_directory>"
	@echo "  --- copies source code for code generators"
	@echo "make copy-config-bin TARGET=<path_to_spi_directory>"
	@echo "  --- copies binaries for code generators"

all: copy

U_SPI_HOME=..

include $(U_SPI_HOME)/makefiles/config/config.mk
include $(U_SPI_HOME)/makefiles/version.mk

COPY_MANIFEST=$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/copyManifest.py
TARGET=3rdParty/spi

clean: 
	$(MAKE) -C $(U_SPI_HOME)/config -s -j4 config-clean
	rm -fr $(TARGET)

zip: clean $(TARGET).zip

tar: clean $(TARGET)-$(G_PLATFORM).tar.bz2

build:
	$(MAKE) -C $(U_SPI_HOME)/config -s -j4 config-build

$(TARGET).zip: copy
	@rm -f $(TARGET).zip
	7z a $(TARGET).zip $(TARGET)

$(TARGET)-$(G_PLATFORM).tar.bz2: copy
	@rm -f $(TARGET)-$(G_PLATFORM).tar.bz2 $(TARGET)-$(G_PLATFORM).tar
	tar cf $(TARGET)-$(G_PLATFORM).tar $(TARGET)
	bzip2 $(TARGET)-$(G_PLATFORM).tar

copy-runtime:
ifndef TARGET
	@echo "TARGET undefined" && exit 1
endif
	@$(COPY_MANIFEST) spi-common.man $(U_SPI_HOME) $(TARGET)
	@$(COPY_MANIFEST) spi-$(G_PLATFORM).man $(U_SPI_HOME) $(TARGET) SPI_DLL=$(SPI_DLL)

copy-config-source:
ifndef TARGET
	@echo "TARGET undefined" && exit 1
endif
	@$(COPY_MANIFEST) spi-config-source.man $(U_SPI_HOME) $(TARGET)

copy-config-bin: build
ifndef TARGET
	@echo "TARGET undefined" && exit 1
endif
	@$(COPY_MANIFEST) spi-config-$(G_PLATFORM).man $(U_SPI_HOME) $(TARGET)

doc:
	@$(COPY_MANIFEST) spi-doc-source.man $(U_SPI_HOME)/doc $(TARGET)-doc

doc-clean:
	rm -f $(TARGET)-doc.zip
	rm -fr $(TARGET)-doc

doc-zip: doc-clean doc
	@rm -f $(TARGET)-doc.zip
	7z a $(TARGET)-doc.zip $(TARGET)-doc



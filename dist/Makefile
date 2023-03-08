.PHONY: all copy clean doc zip doc-zip tar build doc-clean

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

copy: build
	@$(COPY_MANIFEST) spi-common.man $(U_SPI_HOME) $(TARGET)
	@$(COPY_MANIFEST) spi-$(G_PLATFORM).man $(U_SPI_HOME) $(TARGET) SPI_DLL=$(SPI_DLL)

doc:
	@$(COPY_MANIFEST) spi-doc-source.man $(U_SPI_HOME)/doc $(TARGET)-doc

doc-clean:
	rm -f $(TARGET)-doc.zip
	rm -fr $(TARGET)-doc

doc-zip: doc-clean doc
	@rm -f $(TARGET)-doc.zip
	7z a $(TARGET)-doc.zip $(TARGET)-doc



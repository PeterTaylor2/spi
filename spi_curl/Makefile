target:

U_SPI_HOME=..

include $(U_SPI_HOME)/makefiles/config/config.mk
include $(U_SPI_HOME)/makefiles/version.mk

U_VCPROJ=spi_curl

U_SRC_DIR=src
U_INC_DIR=.
U_INCLUDES=-I$(U_SRC_DIR) -I$(U_SPI_HOME)/zlib
U_TARGET=$(SPI_CURL_DLL)
U_FORCE_REBUILD+=Makefile $(U_SPI_HOME)/makefiles/version.mk
U_CFLAGS+=-DBUILDING_LIBCURL

ifneq ($(G_PLATFORM),win32)
U_NO_BUILD=1
else
U_CFLAGS+=-DWIN32
U_CFLAGS+=-DUSE_WINDOWS_SSPI -DCMAKE_USE_WINSSL
U_CFLAGS+=-DHAVE_ZLIB_H -DHAVE_ZLIB -DHAVE_LIBZ
U_DEP_LIBS+=$(U_SPI_HOME)/zlib/$(G_BUILD_DIR)/zlib.lib
U_LIBS+=$(U_DEP_LIBS)
endif

vpath %.c $(U_SRC_DIR)/vtls $(U_SRC_DIR)/vauth

VTLS_C=$(notdir $(wildcard $(U_SRC_DIR)/vtls/*.c))
VTLS_OBJS=$(VTLS_C:.c=.$(G_OBJ))

VAUTH_C=$(notdir $(wildcard $(U_SRC_DIR)/vauth/*.c))
VAUTH_OBJS=$(VAUTH_C:.c=.$(G_OBJ))

U_EXTRA_OBJS=$(VTLS_OBJS) $(VAUTH_OBJS)

include $(U_SPI_HOME)/makefiles/build/dll.mk

post_build: bin

bin: $(U_SPI_HOME)/bin/$(G_ABI)/$(notdir $(I_TARGET))

$(U_SPI_HOME)/bin/$(G_ABI)/$(notdir $(I_TARGET)): $(I_TARGET)
	@mkdir -p $(U_SPI_HOME)/bin/$(G_ABI)
	cp -f $(I_TARGET) $(U_SPI_HOME)/bin/$(G_ABI)
ifeq ($(G_PLATFORM),win32) 
	cp -f $(G_BUILD_DIR)/$(U_TARGET).lib $(U_SPI_HOME)/bin/$(G_ABI)
	@if [ -f $(basename $<).pdb ]; then cp -f $(basename $<).pdb $(U_SPI_HOME)/bin/$(G_ABI); fi
endif


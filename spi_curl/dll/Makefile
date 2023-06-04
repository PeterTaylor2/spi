target:

U_SPI_HOME=../..

include $(U_SPI_HOME)/makefiles/config/config.mk
include $(U_SPI_HOME)/makefiles/version.mk

U_VCPROJ=spi_curl_dll

U_SRC_DIR=../src
U_INC_DIR=..
U_INCLUDES=-I$(U_SRC_DIR)
U_TARGET=$(SPI_CURL_DLL)
U_FORCE_REBUILD+=Makefile $(U_SPI_HOME)/makefiles/version.mk
U_CFLAGS+=-DBUILDING_LIBCURL

ifneq ($(G_PLATFORM),win32)
U_NO_BUILD=1
else
U_CFLAGS+=-DWIN32
endif

# if we wanted to include ZLIB support we could do it as follows:
# U_CFLAGS+=-DHAVE_ZLIB_H -DHAVE_ZLIB -DHAVE_LIBZ -I../../zlib

vpath %.c $(U_SRC_DIR)/vtls $(U_SRC_DIR)/vauth

VTLS_C=$(notdir $(wildcard $(U_SRC_DIR)/vtls/*.c))
VTLS_OBJS=$(VTLS_C:.c=.$(G_OBJ))

VAUTH_C=$(notdir $(wildcard $(U_SRC_DIR)/vauth/*.c))
VAUTH_OBJS=$(VAUTH_C:.c=.$(G_OBJ))

U_EXTRA_OBJS=$(VTLS_OBJS) $(VAUTH_OBJS)

include $(U_SPI_HOME)/makefiles/build/dll.mk

post_build: bin

bin: ../bin/$(G_ABI)/$(notdir $(I_TARGET))

../bin/$(G_ABI)/$(notdir $(I_TARGET)): $(I_TARGET)
	@mkdir -p ../bin/$(G_ABI)
	cp -f $(I_TARGET) ../bin/$(G_ABI)
ifeq ($(G_PLATFORM),win32) 
	cp -f $(G_BUILD_DIR)/$(U_TARGET).lib ../bin/$(G_ABI)
	@if [ -f $(basename $<).pdb ]; then cp -f $(basename $<).pdb ../bin/$(G_ABI); fi
endif


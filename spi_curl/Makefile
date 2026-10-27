target:

U_SPI_HOME=..

include $(U_SPI_HOME)/makefiles/config/config.mk
include $(U_SPI_HOME)/makefiles/version.mk

U_BUILD_SUFFIX=-curl-$(G_WIN32_CURL_VERSION)

all::

ifneq ($(G_PLATFORM),win32)
U_NO_BUILD=1
else

U_VCPROJ=spi_curl

ifeq ($(G_WIN32_NO_CURL),0)
U_NO_BUILD=1
else

U_SRC_DIR=$(G_WIN32_CURL_HOME)/curl/src
U_INC_DIR=$(G_WIN32_CURL_HOME)/curl
U_INCLUDES=-I$(U_SRC_DIR) -I$(U_SPI_HOME)/zlib -I$(G_WIN32_CURL_HOME)
U_TARGET=$(G_WIN32_CURL_DLL)
U_FORCE_REBUILD+=Makefile $(U_SPI_HOME)/makefiles/config/win32_curl.mk
U_CFLAGS+=-DBUILDING_LIBCURL

U_CFLAGS+=-DWIN32
U_CFLAGS+=-DUSE_WINDOWS_SSPI -DUSE_SCHANNEL -DSSL_ENABLED
U_CFLAGS+=-DHAVE_ZLIB_H -DHAVE_ZLIB -DHAVE_LIBZ
U_CFLAGS+=-DZ_PREFIX
U_DEP_LIBS+=$(U_SPI_HOME)/zlib/$(G_BUILD_DIR)/zlib.lib
U_LIBS+=$(U_DEP_LIBS)

vpath %.c $(U_SRC_DIR)/vtls
vpath %.c $(U_SRC_DIR)/vauth
vpath %.c $(U_SRC_DIR)/curlx
vpath %.c $(U_SRC_DIR)/vquic
vpath %.c $(U_SRC_DIR)/vssh

VTLS_C=$(notdir $(wildcard $(U_SRC_DIR)/vtls/*.c))
VTLS_OBJS=$(VTLS_C:.c=.$(G_OBJ))

VAUTH_C=$(notdir $(wildcard $(U_SRC_DIR)/vauth/*.c))
VAUTH_OBJS=$(VAUTH_C:.c=.$(G_OBJ))

CURLX_C=$(notdir $(wildcard $(U_SRC_DIR)/curlx/*.c))
CURLX_OBJS=$(CURLX_C:.c=.$(G_OBJ))

VQUIC_C=$(notdir $(wildcard $(U_SRC_DIR)/vquic/*.c))
VQUIC_OBJS=$(VQUIC_C:.c=.$(G_OBJ))

VSSH_C=$(notdir $(wildcard $(U_SRC_DIR)/vssh/*.c))
VSSH_OBJS=$(VSSH_C:.c=.$(G_OBJ))

U_EXTRA_OBJS+=$(VTLS_OBJS)
U_EXTRA_OBJS+=$(VAUTH_OBJS) 
U_EXTRA_OBJS+=$(CURLX_OBJS) 
U_EXTRA_OBJS+=$(VQUIC_OBJS)
U_EXTRA_OBJS+=$(VSSH_OBJS)

I_SYS_LIBS+=bcrypt.lib secur32.lib volatileaccessu.lib

include $(U_SPI_HOME)/makefiles/build/dll.mk

post_build: bin

bin: $(U_SPI_HOME)/bin/$(G_ABI)/$(notdir $(I_TARGET))

$(U_SPI_HOME)/bin/$(G_ABI)/$(notdir $(I_TARGET)): $(I_TARGET)
	@mkdir -p $(U_SPI_HOME)/bin/$(G_ABI)
	@rm -f $(U_SPI_HOME)/bin/$(G_ABI)/spi-curl-*.*
	cp -f $(I_TARGET) $(U_SPI_HOME)/bin/$(G_ABI)
ifeq ($(G_PLATFORM),win32) 
	cp -f $(G_BUILD_DIR)$(U_BUILD_SUFFIX)/$(U_TARGET).lib $(U_SPI_HOME)/bin/$(G_ABI)
	@if [ -f $(basename $<).pdb ]; then cp -f $(basename $<).pdb $(U_SPI_HOME)/bin/$(G_ABI); fi
endif

endif
endif


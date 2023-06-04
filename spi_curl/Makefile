target:

U_SPI_HOME=..

include $(U_SPI_HOME)/makefiles/config/config.mk

U_VCPROJ=spi_curl

U_SRC_DIR=src
U_INC_DIR=.
U_INCLUDES=-I$(U_SRC_DIR)
U_TARGET=spi_curl
U_FORCE_REBUILD=Makefile 
U_CFLAGS+=-DCURL_STATICLIB -DBUILDING_LIBCURL

ifneq ($(G_PLATFORM),win32)
U_NO_BUILD=1
else
U_CFLAGS+=-DWIN32
endif

# if we wanted to include ZLIB support we could do it as follows:
# U_CFLAGS+=-DHAVE_ZLIB_H -DHAVE_ZLIB -DHAVE_LIBZ -I../zlib

# we don't want to support SSL
# we are really only using this for simple local web servers

vpath %.c $(U_SRC_DIR)/vtls $(U_SRC_DIR)/vauth

VTLS_OBJS=\
vtls.$(G_OBJ)

VAUTH_OBJS=\
cleartext.$(G_OBJ)\
cram.$(G_OBJ)\
digest.$(G_OBJ)\
ntlm.$(G_OBJ)\
oauth2.$(G_OBJ)\
vauth.$(G_OBJ)

U_EXTRA_OBJS=$(VTLS_OBJS) $(VAUTH_OBJS)

include $(U_SPI_HOME)/makefiles/build/lib.mk


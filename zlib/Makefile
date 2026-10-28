target:

U_SPI_HOME=..

include $(U_SPI_HOME)/makefiles/config/config.mk
include $(U_SPI_HOME)/makefiles/config/zlib.mk

U_BUILD_SUFFIX=-zlib-$(G_ZLIB_VERSION)
U_SRC_DIR=$(G_ZLIB_HOME)/src
U_INC_DIR=$(G_ZLIB_HOME)/zlib
U_INCLUDES=-I$(U_SRC_DIR)
U_TARGET=zlib
U_CFLAGS=
U_FORCE_REBUILD=Makefile 
U_EXCLUDE_OBJS=\
gzclose.$(G_OBJ)\
gzlib.$(G_OBJ)\
gzread.$(G_OBJ)\
gzwrite.$(G_OBJ)

U_CFLAGS+=$(G_ZLIB_CFLAGS)

G_MACOS64_GCC_WARNINGS+=-Wno-shift-negative-value

include $(U_SPI_HOME)/makefiles/build/lib.mk

post_build: bin

bin: ../bin/$(G_ABI)/$(notdir $(I_TARGET))

../bin/$(G_ABI)/$(notdir $(I_TARGET)): $(I_TARGET)
	@mkdir -p ../bin/$(G_ABI)
	cp -f $(I_TARGET) ../bin/$(G_ABI)



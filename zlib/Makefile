target:

U_SPI_HOME=..

include $(U_SPI_HOME)/makefiles/config/config.mk

U_SRC_DIR=src
U_INC_DIR=.
U_INCLUDES=-I$(U_SRC_DIR)
U_TARGET=zlib
U_CFLAGS=
U_FORCE_REBUILD=Makefile 
U_EXCLUDE_OBJS=\
gzclose.$(G_OBJ)\
gzlib.$(G_OBJ)\
gzread.$(G_OBJ)\
gzwrite.$(G_OBJ)

U_CFLAGS+=-DZ_PREFIX

G_MACOS64_GCC_WARNINGS+=-Wno-shift-negative-value

include $(U_SPI_HOME)/makefiles/build/lib.mk



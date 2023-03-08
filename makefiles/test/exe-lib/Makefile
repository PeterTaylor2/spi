target:

U_MAKEFILES=../..

include $(U_MAKEFILES)/config/config.mk

U_SRC_DIR=.
U_INC_DIR=.
U_INCLUDES=-I../lib
U_DEP_LIBS=../lib/$(G_BUILD_DIR)/$(G_LIB_PFX)utils$(G_LIB_EXT)
U_LIBS:=$(U_DEP_LIBS)

U_TARGET=hello

include $(U_MAKEFILES)/build/exe.mk


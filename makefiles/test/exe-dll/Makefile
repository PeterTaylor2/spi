target:

U_MAKEFILES=../..

include $(U_MAKEFILES)/config/config.mk

U_SRC_DIR=.
U_INC_DIR=.
U_INCLUDES=-I../dll
U_DEP_LIBS=../dll/$(G_BUILD_DIR)/$(G_LIB_PFX)utils$(G_DLL_LIB_EXT)

ifeq ($(G_PLATFORM),win32)
	U_LIBS:=$(U_DEP_LIBS)
else
	U_LIBS:=-L../dll/$(G_BUILD_DIR) -lutils
endif

U_TARGET=hello

include $(U_MAKEFILES)/build/exe.mk

post_build: $(I_BUILD_DIR)/$(G_DLL_PFX)utils$(G_DLL_EXT)

$(I_BUILD_DIR)/%$(G_DLL_EXT): ../dll/$(G_BUILD_DIR)/%$(G_DLL_EXT)
	cp $< $@

target:

U_SPI_HOME=../..

include $(U_SPI_HOME)/makefiles/config/config.mk

U_SRC_DIR=sdk$(G_MSVC_VERSION)/Samples/DIA2Dump
U_INC_DIR=$(U_SRC_DIR)
U_INCLUDES=-Isdk$(G_MSVC_VERSION)/include
U_TARGET=dia2dump

U_DEP_LIBS=
U_LIBS=sdk$(G_MSVC_VERSION)/lib/amd64/diaguids.lib\
Ole32.lib\
OleAut32.lib

U_EXCLUDE_OBJS=DIA2Dump.$(G_OBJ)
U_EXTRA_OBJS=main.$(G_OBJ)
U_VCPROJ_OPTIONS=-S .

vpath c++ .

U_FORCE_REBUILD+=Makefile

include $(U_SPI_HOME)/makefiles/build/exe.mk

fetch-code:
	rm -fr sdk$(G_MSVC_VERSION)
	cp -r '$(G_MSVC_HOME)/DIA SDK' sdk$(G_MSVC_VERSION)

ifeq ($(G_PLATFORM),win32)

install: bin/dia2dump.exe

bin/dia2dump.exe: $(G_BUILD_DIR)/dia2dump.exe
	@mkdir -p bin
	cp -f $(G_BUILD_DIR)/dia2dump.exe bin
	cp -f sdk$(G_MSVC_VERSION)/bin/amd64/*.dll bin

endif


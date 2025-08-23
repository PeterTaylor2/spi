target:

U_SPI_HOME=..

include $(U_SPI_HOME)/makefiles/config/config.mk
include $(U_SPI_HOME)/makefiles/version.mk

U_VCPROJ=spi-util

U_SRC_DIR=src
U_INC_DIR=.
U_INCLUDES=-I.. -I$(U_SRC_DIR)
U_TARGET=$(SPI_UTIL_DLL)
U_CFLAGS=-DSPI_UTIL_EXPORT
#U_CFLAGS+=-DTRACE_MEM_UTIL
U_CFLAGS+=-DTRACK_MEMORY_ALLOCATED
U_FORCE_REBUILD=Makefile $(U_SPI_HOME)/makefiles/version.mk

ifneq ($(G_NO_UUID),0)
U_CFLAGS+=-DNO_UUID
else
U_EXCLUDE_OBJS=uuid4.$(G_OBJ)
endif

include extra_objects.mk

ifeq ($(G_PLATFORM),win32)

U_DEP_LIBS=\
$(U_SPI_HOME)/zlib/$(G_BUILD_DIR)/$(G_LIB_PFX)zlib$(G_LIB_EXT)\
$(U_SPI_HOME)/bin/$(G_ABI)/$(G_DLL_PFX)$(SPI_CURL_DLL)$(G_DLL_LIB_EXT)

U_LIBS=$(U_DEP_LIBS)

else

U_DEP_LIBS=\
$(U_SPI_HOME)/zlib/$(G_BUILD_DIR)/$(G_LIB_PFX)zlib$(G_LIB_EXT)

U_LIBS=$(U_DEP_LIBS) $(G_CURL_LIBS)

endif

G_OVERRIDE_NEW_DELETE?=0
ifneq ($(G_OVERRIDE_NEW_DELETE),0)
vpath %.cpp $(U_SPI_HOME)/spi_new_delete/src
U_EXTRA_OBJS+=NewDelete.$(G_OBJ)
G_OVERRIDE_NEW_DELETE:=0
endif

include $(U_SPI_HOME)/makefiles/build/dll.mk

rstrip:
	@$(G_PYTHON) $(U_SPI_HOME)/makefiles/python3/rstrip.py $(U_SRC_DIR)/*.[ch]pp $(U_INC_DIR)/*.h*

post_build: bin

bin: ../bin/$(G_ABI)/$(notdir $(I_TARGET))

../bin/$(G_ABI)/$(notdir $(I_TARGET)): $(I_TARGET)
	@mkdir -p ../bin/$(G_ABI)
	cp -f $(I_TARGET) ../bin/$(G_ABI)
ifeq ($(G_PLATFORM),win32) 
	cp -f $(G_BUILD_DIR)/$(SPI_UTIL_DLL).lib ../bin/$(G_ABI)
	@if [ -f $(basename $<).pdb ]; then cp -f $(basename $<).pdb ../bin/$(G_ABI); fi
endif


target:

U_SPI_HOME=../..

include $(U_SPI_HOME)/makefiles/config/config.mk

U_VCPROJ=spi-util-lib

U_SRC_DIR=../src
U_INC_DIR=..
U_INCLUDES=-I../.. -I$(U_SRC_DIR)
U_TARGET=spi_util
U_CFLAGS=-DSPI_UTIL_STATIC
U_FORCE_REBUILD=Makefile 
#U_CFLAGS+=-DTRACE_MEM_UTIL
#U_CFLAGS+=-DTRACK_MEMORY_ALLOCATED

ifneq ($(G_NO_UUID),0)
U_CFLAGS+=-DNO_UUID
else
U_EXCLUDE_OBJS=uuid4.$(G_OBJ)
endif

U_EXCLUDE_OBJS+=UrlRead.$(G_OBJ)

include ../extra_objects.mk

G_OVERRIDE_NEW_DEFAULT?=0
ifneq ($(G_OVERRIDE_NEW_DEFAULT),0)
vpath %.cpp $(U_SPI_HOME)/spi_new_delete/src
U_EXTRA_OBJS+=NewDelete.$(G_OBJ)
endif

include $(U_SPI_HOME)/makefiles/build/lib.mk

rstrip:
	@$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/rstrip.py $(U_SRC_DIR)/*.[ch]pp $(U_INC_DIR)/*.h*



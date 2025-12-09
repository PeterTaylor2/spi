# tests for the SPI library
#
# use NAME= to define the test that you want to build and run

target:

NAME?=hello
OPTS=
U_SPI_HOME=..
U_TARGET=$(NAME)

include $(U_SPI_HOME)/makefiles/config/config.mk
include $(U_SPI_HOME)/makefiles/version.mk

U_OBJS = $(NAME).$(G_OBJ)
U_VCPROJ=run-all-tests
U_VCPROJ_OPTIONS=-j1 -trun-all

U_FORCE_REBUILD+=Makefile

NAMES=$(basename $(wildcard test*.cpp))

ifeq ($(G_PLATFORM),win32)

U_DEP_LIBS = \
 $(G_ABI)/$(G_LIB_PFX)test-dll$(G_DLL_LIB_EXT)\
 ../dll/$(G_BUILD_DIR)/$(G_LIB_PFX)$(SPI_DLL)$(G_LIB_EXT)\
 ../spi_util/$(G_BUILD_DIR)/$(G_LIB_PFX)$(SPI_UTIL_DLL)$(G_LIB_EXT)\
 ../spi_curl/$(G_BUILD_DIR)/$(G_LIB_PFX)$(SPI_CURL_DLL)$(G_LIB_EXT)\
 ../spi_boost/lib/$(G_BUILD_DIR)/$(G_LIB_PFX)spi_boost$(G_LIB_EXT)

else

U_DEP_LIBS = \
 ../lib/$(G_BUILD_DIR)/$(G_LIB_PFX)spi$(G_LIB_EXT)\
 ../spi_util/lib/$(G_BUILD_DIR)/$(G_LIB_PFX)spi_util$(G_LIB_EXT)\
 ../spi_boost/lib/$(G_BUILD_DIR)/$(G_LIB_PFX)spi_boost$(G_LIB_EXT)

U_CFLAGS = -DSPI_STATIC

endif

U_LIBS = $(U_DEP_LIBS)
U_SRC_DIR = .
U_INC_DIR = .
U_INCLUDES = -I.. -Ilib

include $(U_SPI_HOME)/makefiles/build/exe.mk

post_build:
	$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/copyFiles.py $(G_BUILD_DIR) $(G_ABI)/*$(G_DLL_EXT) 

run-debug:
	$(MAKE) libs DEBUG=1
	$(MAKE) target DEBUG=1
	$(MAKE) run DEBUG=1 ARGS=-w

run-all::
	$(MAKE) clean
	@for nm in $(NAMES); do \
		$(MAKE) -s run NAME=$$nm; \
	done

rstrip:
	@$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/rstrip.py *.cpp

libs:
	$(MAKE) -C $(U_SPI_HOME) -j4 -s
	$(MAKE) -C lib -j4 -s
	$(MAKE) -C config DEBUG=0
	$(MAKE) -C dll -j4 -s


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

ifeq ($(G_PLATFORM),win32)

U_DEP_LIBS = \
 ../isda-util/$(G_BUILD_DIR)/$(G_LIB_PFX)isda-util$(G_LIB_EXT)\
 ../dll/$(G_BUILD_DIR)/$(G_LIB_PFX)$(SPI_DLL)$(G_LIB_EXT)\
 ../spi_util/$(G_BUILD_DIR)/$(G_LIB_PFX)$(SPI_UTIL_DLL)$(G_LIB_EXT)\
 ../spi_boost/lib/$(G_BUILD_DIR)/$(G_LIB_PFX)spi_boost$(G_LIB_EXT)

else

U_DEP_LIBS = \
 ../isda-util/$(G_BUILD_DIR)/$(G_LIB_PFX)isda-util$(G_LIB_EXT)\
 ../lib/$(G_BUILD_DIR)/$(G_LIB_PFX)spi$(G_LIB_EXT)\
 ../spi_util/lib/$(G_BUILD_DIR)/$(G_LIB_PFX)spi_util$(G_LIB_EXT)\
 ../spi_boost/lib/$(G_BUILD_DIR)/$(G_LIB_PFX)spi_boost$(G_LIB_EXT)

U_CFLAGS = -DSPI_STATIC

endif

U_LIBS = $(U_DEP_LIBS)
U_SRC_DIR = .
U_INC_DIR = .
U_INCLUDES = -I.. -Itestrunner
U_INCLUDES+= -I../isda-util

include $(U_SPI_HOME)/makefiles/build/exe.mk

ifeq ($(G_PLATFORM),win32)

pre_build:
	$(MAKE) -C ../isda-util
	$(MAKE) -C ../spi_util
	$(MAKE) -C ../spi_curl
	$(MAKE) -C ../dll

post_build: $(G_BUILD_DIR)/$(G_DLL_PFX)$(SPI_DLL)$(G_DLL_EXT)
post_build: $(G_BUILD_DIR)/$(G_DLL_PFX)$(SPI_UTIL_DLL)$(G_DLL_EXT)
post_build: $(G_BUILD_DIR)/$(G_DLL_PFX)$(SPI_CURL_DLL)$(G_DLL_EXT)

$(G_BUILD_DIR)/%$(G_DLL_EXT): ../dll/$(G_BUILD_DIR)/%$(G_DLL_EXT)
	cp -f $< $@

$(G_BUILD_DIR)/%$(G_DLL_EXT): ../spi_util/$(G_BUILD_DIR)/%$(G_DLL_EXT)
	cp -f $< $@

$(G_BUILD_DIR)/%$(G_DLL_EXT): ../spi_curl/$(G_BUILD_DIR)/%$(G_DLL_EXT)
	cp -f $< $@

else

pre_build:
	$(MAKE) -C ../isda-util
	$(MAKE) -C ../spi_util/lib
	$(MAKE) -C ../lib

endif

debug:
	$(MAKE) run DEBUG=1 ARGS=-w

all::
	$(MAKE) clean
	sleep 1
	@for f in test*.cpp; do \
		name=`basename $$f .cpp`; \
		$(MAKE) -s run NAME=$$name; \
	done

rstrip:
	@$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/rstrip.py *.cpp

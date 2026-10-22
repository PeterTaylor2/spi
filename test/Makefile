# tests for the SPI library
#
# use NAME= to define the test that you want to build and run

help:

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

U_LIBS = $(U_DEP_LIBS)

else

U_DEP_LIBS = \
 $(G_BUILD_DIR)/$(G_LIB_PFX)test-dll$(G_DLL_LIB_EXT)\
 $(G_BUILD_DIR)/$(G_DLL_PFX)$(SPI_DLL)$(G_DLL_LIB_EXT)\
 $(G_BUILD_DIR)/$(G_DLL_PFX)$(SPI_UTIL_DLL)$(G_DLL_LIB_EXT)\
 ../spi_boost/lib/$(G_BUILD_DIR)/$(G_LIB_PFX)spi_boost$(G_LIB_EXT)

U_LIBS=-L$(G_BUILD_DIR) -ltest-dll -l$(SPI_DLL) -l$(SPI_UTIL_DLL) $(G_CURL_LIBS)

endif

U_SRC_DIR = .
U_INC_DIR = .
U_INCLUDES = -I.. -Ilib

include $(U_SPI_HOME)/makefiles/build/exe.mk

help:
	@echo Builds and runs test drivers for SPI
	@echo
	@echo "make run-all"
	@echo "--- builds everything in advance (just in case) and runs all the tests"
	@echo "--- output is sent to run-all.log"
	@echo "--- error messages (sent to stderr) will be echoed to the screen"
	@echo
	@echo "make run-debug NAME="
	@echo "--- builds all debug libraries and then builds and and runs one test"
	@echo "---(by name, e.g. testWalker) in debug mode"
	@echo "--- then pauses at the start to allow you to attach a debugger"
	@echo
	@echo "make run NAME="
	@echo "--- builds and runs one test (by name, e.g. testWalker) in release mode"
	@echo "--- assumes that all libraries have been built in advance"
	@echo
	@echo "make libs"
	@echo "--- builds all libraries"
	@echo "--- this is done by default in run-all and run-debug but not for run"

pre_build:
	@$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/copyFiles.py $(G_BUILD_DIR) $(G_ABI)/*$(G_DLL_EXT) 

run-debug:
	@$(MAKE) -s libs DEBUG=1
	@$(MAKE) -s target DEBUG=1
	@$(MAKE) run DEBUG=1 ARGS=-w

run-all::
	@$(MAKE) -s clean
	@$(MAKE) -s libs
	@echo
	@rm -f run-all.log
	@for nm in $(NAMES); do \
		echo Running $$nm; \
		echo Running $$nm \(`date`\) >> run-all.log; \
		echo "" >> run-all.log; \
		$(MAKE) -s run NAME=$$nm >> run-all.log; \
		echo "" >> run-all.log; \
	done
	@echo
	@echo See file run-all.log for log messages
	@echo Error messages would have appeared on the screen

rstrip:
	@$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/rstrip.py *.cpp

libs:
	@$(MAKE) -C $(U_SPI_HOME) -j4 -s
	@echo Building test/lib
	@$(MAKE) -C lib -j4 -s
	@echo Building test/config
	@$(MAKE) -C config DEBUG=0 -s
	@echo Building test/dll
	@$(MAKE) -C dll -j4 -s


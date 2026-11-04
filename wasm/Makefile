# Standalone browser build; does not use or alter the native site.mk.
EMXX ?= em++
EMCC ?= emcc
EMAR ?= emar
BUILD ?= build
CPPFLAGS += -I.. -I../zlib -DSPI_STATIC -DSPI_UTIL_STATIC -DZ_PREFIX
CXXFLAGS ?= -O2 -std=c++17
CFLAGS ?= -O2
EXCEPTIONS = -fexceptions

# Match lib/Makefile's static runtime, without network recording or URL I/O.
CORE = $(filter-out ../spi/src/ObjectURL.cpp ../spi/src/Recording.cpp,$(wildcard ../spi/src/*.cpp))
UTIL = $(filter-out ../spi_util/src/CurlUtil.cpp ../spi_util/src/UrlRead.cpp ../spi_util/src/UdpUpload.cpp ../spi_util/src/UUID.cpp,$(wildcard ../spi_util/src/*.cpp))
CPP = $(CORE) $(UTIL) ../spi_boost/lib/boost-assertion-failed.cpp
C = ../spi/src/dg_strtod.c $(filter-out ../zlib/src/gzclose.c ../zlib/src/gzlib.c ../zlib/src/gzread.c ../zlib/src/gzwrite.c,$(wildcard ../zlib/src/*.c))
OBJECTS = $(patsubst ../%.cpp,$(BUILD)/%.o,$(CPP)) $(patsubst ../%.c,$(BUILD)/%.o,$(C)) $(BUILD)/UUID.o

.PHONY: all test clean
all: $(BUILD)/spi.mjs

test: all $(BUILD)/test.mjs
	npm test

$(BUILD)/libspi.a: $(OBJECTS)
	rm -f $@
	$(EMAR) rcs $@ $^

$(BUILD)/spi.mjs: bindings.cpp $(BUILD)/libspi.a Makefile
	$(EMXX) $(CPPFLAGS) $(CXXFLAGS) $(EXCEPTIONS) --bind $< $(BUILD)/libspi.a \
		-sMODULARIZE=1 -sEXPORT_NAME=createSPI -sENVIRONMENT=web \
		-sALLOW_MEMORY_GROWTH=1 -sEXPORTED_RUNTIME_METHODS=getExceptionMessage,decrementExceptionRefcount -o $@

$(BUILD)/test.mjs: test.cpp $(BUILD)/libspi.a Makefile
	$(EMXX) $(CPPFLAGS) $(CXXFLAGS) $(EXCEPTIONS) --bind $< $(BUILD)/libspi.a \
		-sMODULARIZE=1 -sENVIRONMENT=web -sALLOW_MEMORY_GROWTH=1 -o $@

$(BUILD)/%.o: ../%.cpp Makefile
	@mkdir -p $(dir $@)
	$(EMXX) -I$(dir $<).. -I$(dir $<) $(CPPFLAGS) $(CXXFLAGS) $(EXCEPTIONS) -MMD -MP -c $< -o $@

$(BUILD)/%.o: ../%.c Makefile
	@mkdir -p $(dir $@)
	$(EMCC) -I$(dir $<).. $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD)/UUID.o: UUID.cpp Makefile
	@mkdir -p $(dir $@)
	$(EMXX) $(CPPFLAGS) $(CXXFLAGS) $(EXCEPTIONS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(BUILD)

-include $(OBJECTS:.o=.d)

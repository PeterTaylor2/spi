all:

U_SPI_HOME=..
include $(U_SPI_HOME)/makefiles/config/config.mk

ifeq ($(G_PLATFORM),win32)

ifeq ($(DEBUG),0)
I_CONFIG=Release
else
I_CONFIG=Debug
endif

I_SOURCES=$(wildcard makeXLAddin/*.config makeXLAddin/*.cs)

all: $(I_CONFIG)/makeXLAddin.exe

$(I_CONFIG)/makeXLAddin.exe: makeXLAddin/bin/$(I_CONFIG)/makeXLAddin.exe
	@mkdir -p $(I_CONFIG)
	cp $< $@

makeXLAddin/bin/$(I_CONFIG)/makeXLAddin.exe: $(I_SOURCES)
	$(G_MSBUILD) -verbosity:minimal -property:Configuration=$(I_CONFIG) makeXLAddin.sln

clean:
	rm -fr makeXLAddin/obj/$(I_CONFIG)
	rm -fr makeXLAddin/bin/$(I_CONFIG)
	rm -fr $(I_CONFIG)

info::
	@echo I_SOURCES=$(I_SOURCES)
	@echo I_CONFIG=$(I_CONFIG)

install: $(U_SPI_HOME)/makefiles/win32/makeXLAddin.exe release-build

release-build:
	@$(MAKE) -s all DEBUG=0

$(U_SPI_HOME)/makefiles/win32/makeXLAddin.exe: Release/makeXLAddin.exe
	@mkdir -p $(U_SPI_HOME)/makefiles/win32
	cp $< $@


v16.vcxproj:

v17.vcxproj:

v18.vcxproj:


else

U_NO_BUILD=1

endif


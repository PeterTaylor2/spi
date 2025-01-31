target:

U_SPI_HOME=..

include $(U_SPI_HOME)/makefiles/config/config.mk
include $(U_SPI_HOME)/makefiles/version.mk

ifeq ($(G_PLATFORM),win32)

all: target

clean-all: clean

target:
	$(G_MSBUILD) spcs.$(G_CSPROJ_EXT) $(G_MSBUILD_PROPERTIES) /v:Minimal

clean:
	$(G_MSBUILD) spcs.$(G_CSPROJ_EXT) /t:Clean $(G_MSBUILD_PROPERTIES) /v:Minimal

else:

U_NO_BUILD=1

endif



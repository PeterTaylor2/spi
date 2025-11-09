target:

U_MAKEFILES=..

include $(U_MAKEFILES)/config/config.mk

U_SRC_DIR=src
U_INC_DIR=src
U_TARGET=gendep

ifneq ($(G_PLATFORM),win32)
U_NO_BUILD=1
endif

include $(U_MAKEFILES)/build/exe.mk

U_VCPROJ=gendep
U_VCPROJ_OPTIONS=-tconfig-install --cleanTarget=config-clean -j1

config-info::
	@echo G_PLATFORM=$(G_PLATFORM)
	@echo G_CONFIG_COMPILER=$(G_CONFIG_COMPILER)
	@echo G_CONFIG_COMPILER_BITS=$(G_CONFIG_COMPILER_BITS)
	@echo G_CONFIG_COMPILER_ABI=$(G_CONFIG_COMPILER_ABI)

ifeq ($(G_PLATFORM),win32)

install: bin/gendep.exe

bin/gendep.exe: $(G_BUILD_DIR)/gendep.exe
	@mkdir -p bin
	@echo Copying $(G_BUILD_DIR)/gendep.exe to bin
	@cp -f $(G_BUILD_DIR)/gendep.exe bin

config-build::
	@$(MAKE) -s config-info
	@$(MAKE) -s target  DEBUG=0 COMPILER=$(G_CONFIG_COMPILER) BITS=$(G_CONFIG_COMPILER_BITS)

config-clean::
	@$(MAKE) -s clean DEBUG=0 COMPILER=$(G_CONFIG_COMPILER) BITS=$(G_CONFIG_COMPILER_BITS)

config-install: 
	@$(MAKE) -s target  DEBUG=0 COMPILER=$(G_CONFIG_COMPILER) BITS=$(G_CONFIG_COMPILER_BITS)
	@$(MAKE) -s install DEBUG=0 COMPILER=$(G_CONFIG_COMPILER) BITS=$(G_CONFIG_COMPILER_BITS)

endif


target:

U_MAKEFILES=..
U_NO_GENDEP=1

include $(U_MAKEFILES)/config/config.mk

U_SRC_DIR=src
U_INC_DIR=src
U_TARGET=gendep

include $(U_MAKEFILES)/build/exe.mk

U_VCPROJ=gendep
U_VCPROJ_OPTIONS=-tconfig-install --cleanTarget=config-clean -j1

config-info::
	@echo G_PLATFORM=$(G_PLATFORM)
	@echo G_ABI=$(G_ABI)

install: bin-$(G_PLATFORM)/gendep$(G_EXE)

bin-$(G_PLATFORM)/gendep$(G_EXE): $(G_BUILD_DIR)/gendep$(G_EXE)
	@mkdir -p bin-$(G_PLATFORM)
	@echo Copying $(G_BUILD_DIR)/gendep$(G_EXE) to bin-$(G_PLATFORM)
	@cp -f $(G_BUILD_DIR)/gendep$(G_EXE) bin-$(G_PLATFORM)

config-build::
	@$(MAKE) -s config-info DEBUG=0
	@$(MAKE) -s target DEBUG=0

config-clean::
	@$(MAKE) -s clean DEBUG=0

config-install: 
	@$(MAKE) -s target DEBUG=0
	@$(MAKE) -s install DEBUG=0



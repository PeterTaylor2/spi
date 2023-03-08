###########################################################################
# Variables for dealing with base service DLLs.
#
# Transforms the variable U_BASE_SERVICE_DLL into I_BASE_SERVICE_DEP_LIBS
# and I_BASE_SERVICE_LIBS.
#
# In addition if the variable is defined then it defines the
# copy_base_service_dll target.
###########################################################################

include $(U_SPI_HOME)/makefiles/version.mk

ifdef U_BASE_SERVICE_DLL

ifeq ($(G_PLATFORM),win32)

I_BASE_SERVICE_DEP_LIBS:=\
$(addsuffix .lib,$(subst :,/$(G_BUILD_DIR)/,$(U_BASE_SERVICE_DLL)))

I_BASE_SERVICE_LIBS:=$(I_BASE_SERVICE_DEP_LIBS)

I_BASE_SERVICE_DLLS_TARGET:=\
$(addprefix ../$(G_ABI)/,$(subst .lib,.dll,$(notdir $(I_BASE_SERVICE_DEP_LIBS))))

I_BASE_SERVICE_DLLS_SOURCE:=\
$(addsuffix .dll,$(basename $(I_BASE_SERVICE_DEP_LIBS)))

else

I_BASE_SERVICE_DEP_LIBS:=\
$(addsuffix $(G_DLL_EXT),$(subst :,/$(G_BUILD_DIR)/lib,$(U_BASE_SERVICE_DLL)))

ifdef G_CYGWIN
I_BASE_SERVICE_LIBS:=$(I_BASE_SERVICE_DEP_LIBS)
else
I_BASE_SERVICE_LIBS:=$(U_BASE_SERVICE_LINK)
endif

I_BASE_SERVICE_DLLS_TARGET:=\
$(addprefix ../$(G_ABI)/,$(notdir $(I_BASE_SERVICE_DEP_LIBS)))

I_BASE_SERVICE_DLLS_SOURCE:=$(I_BASE_SERVICE_DEP_LIBS)

endif

U_DEP_LIBS:=$(I_BASE_SERVICE_DEP_LIBS) $(U_DEP_LIBS)
U_LIBS:=$(I_BASE_SERVICE_LIBS) $(U_LIBS)

copy_base_service_dll: $(I_BASE_SERVICE_DLLS_TARGET)

$(I_BASE_SERVICE_DLLS_TARGET): $(I_BASE_SERVICE_DLLS_SOURCE)
	@mkdir -p ../$(G_ABI)
	cp -f $< ../$(G_ABI)

help-base-service:
	@echo "DEP_LIBS=$(I_BASE_SERVICE_DEP_LIBS)"
	@echo "LIBS=$(I_BASE_SERVICE_LIBS)"
	@echo "TARGET=$(I_BASE_SERVICE_DLLS_TARGET)"
	@echo "SOURCE=$(I_BASE_SERVICE_DLLS_SOURCE)"
	@echo "U_BASE_SERVICE_DLL=$(U_BASE_SERVICE_DLL)"

endif


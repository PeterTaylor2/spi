############################################################################
# Rules for building a shared library using C++.
############################################################################

include $(U_MAKEFILES)/build/objects.mk

###########################################################################
# Internal variables
###########################################################################
I_TARGET=$(I_BUILD_DIR)/$(G_DLL_PFX)$(U_TARGET)$(G_DLL_EXT)

build_target: $(I_TARGET)

clean_target:
	rm -f $(I_TARGET)

############################################################################
# Build target shared library.
#
# Dependency on U_EXPORTS is only relevant for non-windows builds and
# U_EXPORTS would also be incorporated within G_DLL_LFLAGS.
############################################################################
$(I_TARGET): $(I_OBJS) $(U_DEP_LIBS) $(U_EXPORTS)
	$(G_LINK) $(G_DLL_LFLAGS) $(U_LFLAGS) $(I_OBJS) $(U_LIBS) $(G_SYS_LIBS)
ifdef G_EMBED_MANIFEST
	mt -nologo -manifest $(I_TARGET).manifest -outputresource:"$(I_TARGET);2"
endif
ifneq ($(G_PLATFORM),win32)
ifeq ($(DEBUG),0)
	@strip $(I_TARGET)
endif
endif
ifeq ($(G_PLATFORM),macos64)
	install_name_tool -id @loader_path/$(notdir $(I_TARGET)) $(I_TARGET)
endif

############################################################################
# show_vars - Simple debugging help.
############################################################################
show_vars:
	@echo "*** User variables for build of C++ executable: $(I_TARGET)"
	@echo "U_CFLAGS = $(U_CFLAGS)"
	@echo "U_INCLUDES = $(U_INCLUDES)"
	@echo "U_LFLAGS = $(U_LFLAGS)"
	@echo "U_DEP_LIBS = $(U_DEP_LIBS)"
	@echo "U_LIBS = $(U_LIBS)"
	@echo "U_OBJS = $(U_OBJS)"
	@echo "U_SRC_DIR = $(U_SRC_DIR)"
	@echo "U_TARGET = $(U_TARGET)"
	@echo
	@echo "*** Internal and global variables ***"
	@echo "I_TARGET = $(I_TARGET)"
	@echo "I_OBJS = $(I_OBJS)"
	@echo "G_LINK = $(G_LINK)"
	@echo "G_DLL_LFLAGS = $(G_DLL_LFLAGS)"
	@echo "G_SYS_LIBS = $(G_SYS_LIBS)"



############################################################################
# Rules for building an executable using C++.
############################################################################

include $(U_MAKEFILES)/build/objects.mk

###########################################################################
# Internal variables
###########################################################################
I_TARGET=$(I_BUILD_DIR)/$(U_TARGET)$(G_EXE)

build_target: $(I_TARGET)

clean_target:
	rm -f $(I_TARGET)

############################################################################
# Build target executable.
############################################################################
$(I_TARGET): $(I_OBJS) $(U_DEP_LIBS)
	$(G_LINK) $(G_EXE_LFLAGS) $(U_LFLAGS) $(I_OBJS) $(U_LIBS) $(G_SYS_LIBS)
ifdef G_EMBED_MANIFEST
	@mt -nologo -manifest $(I_TARGET).manifest -outputresource:"$(I_TARGET);1"
endif
ifneq ($(G_PLATFORM),win32)
ifeq ($(DEBUG),0)
	@strip $(I_TARGET)
endif
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
	@echo "G_BASE_CONFIG = $(G_BASE_CONFIG)"
	@echo "G_BUILD_DIR = $(G_BUILD_DIR)"
	@echo "G_EXE_LFLAGS = $(G_EXE_LFLAGS)"
	@echo "G_SYS_LIBS = $(G_SYS_LIBS)"

###########################################################################
# run the executable
#
# if the executable needs DLLs then we might need to set the path
# accordingly - for the moment assume that it doesn't
#
# define run-time arguments via ARGS
###########################################################################
run: target
	$(WRAPPER) $(I_TARGET) $(ARGS)

############################################################################
# Extra stuff could be to compile the dependent libraries or to generate
# the visual studio project files.
#
# TBD
############################################################################

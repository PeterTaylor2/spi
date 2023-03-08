############################################################################
# Rules for building a static library.
############################################################################

include $(U_MAKEFILES)/build/objects.mk

###########################################################################
# Internal variables
###########################################################################
I_TARGET=$(I_BUILD_DIR)/$(G_LIB_PFX)$(U_TARGET)$(G_LIB_EXT)

build_target: $(I_TARGET)

clean_target:
	rm -f $(I_TARGET)

############################################################################
# Build target static library.
#
# TBD: Build a big library from the object files from lots of little
#      libraries. This is the sort of thing that might need a pre_build
#      target to build the little libraries.
############################################################################
$(I_TARGET): $(I_OBJS)
	@rm -f $(I_TARGET)
	@echo "Creating library $(I_TARGET)"
	$(G_LIB) $(I_OBJS)

############################################################################
# show_vars - Simple debugging help.
############################################################################
show_vars:
	@echo "*** User variables for build of C++ executable: $(I_TARGET)"
	@echo "U_CFLAGS = $(U_CFLAGS)"
	@echo "U_INCLUDES = $(U_INCLUDES)"
	@echo "U_LFLAGS = $(U_LFLAGS)"
	@echo "U_LIBS = $(U_LIBS)"
	@echo "U_OBJS = $(U_OBJS)"
	@echo "U_SRC_DIR = $(U_SRC_DIR)"
	@echo "U_TARGET = $(U_TARGET)"
	@echo
	@echo "*** Internal and global variables ***"
	@echo "I_TARGET = $(I_TARGET)"
	@echo "I_OBJS = $(I_OBJS)"
	@echo "G_LIB = $(G_LIB)"
	@echo "G_EXE_LFLAGS = $(G_EXE_LFLAGS)"
	@echo "G_SYS_LIBS = $(G_SYS_LIBS)"
	@echo "G_LIB_PFX = $(G_LIB_PFX)"
	@echo "G_LIB_EXT = $(G_LIB_EXT)"

############################################################################
# Extra stuff could be to compile the dependent libraries or to generate
# the visual studio project files.
#
# TBD
############################################################################

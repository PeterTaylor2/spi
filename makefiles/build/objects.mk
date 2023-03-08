############################################################################
# FILE: objects.mk
#
# Defines how we compile object files from source files.
#
# In addition defines the following internal variables to be used by the
# controller file:
#
#  U_OBJS:      if undefined then this will generate from U_SRC_DIR
#  I_BUILD_DIR: build directory
#  I_OBJS:      objects (from U_OBJS) translated from the build directory
############################################################################

############################################################################
# if you want to define your own extra directories then use vpath in the
# relevant user-defined makefile - by default we compile using U_SRC_DIR
#
# in addition you should probably define U_EXTRA_OBJS in this case
# in that way you get all the source files from U_SRC_DIR plus the extra
# objects from other source directories
############################################################################

vpath %.cpp $(U_SRC_DIR)
vpath %.cxx $(U_SRC_DIR)
vpath %.c   $(U_SRC_DIR)

vpath %.hpp $(U_INC_DIR)
vpath %.h   $(U_INC_DIR)

###########################################################################
# Define U_OBJS if not defined, as well as applying U_EXCLUDE_OBJS
###########################################################################

ifndef U_OBJS

I_CPP_SRCS += $(notdir $(wildcard $(U_SRC_DIR)/*.cpp))
U_OBJS = $(I_CPP_SRCS:.cpp=.$(G_OBJ))
I_C_SRCS = $(notdir $(wildcard $(U_SRC_DIR)/*.c))
U_OBJS += $(I_C_SRCS:.c=.$(G_OBJ))

endif

ifdef U_EXCLUDE_OBJS
U_OBJS := $(filter-out $(U_EXCLUDE_OBJS), $(U_OBJS))
endif

U_OBJS += $(U_EXTRA_OBJS)

###########################################################################
# Internal variables
###########################################################################
I_BUILD_DIR=$(G_BUILD_DIR)$(U_BUILD_SUFFIX)
I_OBJS=$(patsubst %.$(G_OBJ),$(I_BUILD_DIR)/%.$(G_OBJ),$(U_OBJS))
I_FORCE_REBUILD=$(U_FORCE_REBUILD) $(U_MAKEFILES)/config/$(G_CONFIG).mk
I_INCLUDES=-I$(U_INC_DIR) $(U_INCLUDES)

ifeq ($(G_PLATFORM),win32)
I_GENDEP_EXE=$(U_MAKEFILES)/gendep/bin/gendep.exe
endif

objects-info:
	@echo I_OBJS=$(I_OBJS)
	@echo U_OBJS=$(U_OBJS)
	@echo U_EXCLUDE_OBJS=$(U_EXCLUDE_OBJS)
	@echo U_EXTRA_OBJS=$(U_EXTRA_OBJS)
	@echo U_SRC_DIR=$(U_SRC_DIR)
	@echo U_INC_DIR=$(U_INC_DIR)
	@echo I_INCLUDES=$(I_INCLUDES)
	@echo I_BUILD_DIR=$(I_BUILD_DIR)
	@echo G_BUILD_DIR=$(G_BUILD_DIR)
	@echo I_FORCE_REBUILD=$(I_FORCE_REBUILD)
	@echo G_CONFIG=$(G_CONFIG)

############################################################################
# In all cases the main target is 'target'
#
# If we have pre_build and post_build defined, then we want to ensure that
# these get invoked in the correct sequence even when doing a parallel
# build, so this explains (I hope) all these combinations defined below.
#
# We do not implement build_target and clean_target in this file since
# these need the definition of I_TARGET which needs the definition of
# I_BUILD_DIR which is provided by this file.
############################################################################
.PHONY: target pre_build post_build build make_build_dir

ifndef U_HAS_ALL_TARGET

all:: target

clean-all:: clean

endif

ifdef U_NO_BUILD

target:
	@echo "No build required"

else

target: pre_build build post_build

pre_build: make_build_dir

post_build: build build_target

build: pre_build build_target

endif

############################################################################
# Well-known 'clean' target - Remove all components built for target.
############################################################################
clean::
	@rm -rf $(I_BUILD_DIR)

############################################################################
# Visual Studio in its wisdom writes a log file to the output directory.
# Hence we cannot clean the output directory when run from within Visual
# Studio. Instead we will remove object files, dependency files and the
# final target file.
############################################################################
vs-clean::
	@rm -f $(I_BUILD_DIR)/*.$(G_OBJ) $(I_TARGET) $(I_BUILD_DIR)/*.d

############################################################################
# relink - A convenience rule to just relink.
############################################################################
relink: clean_target target

###########################################################################
# rules for making the build directory
###########################################################################
make_build_dir: $(I_BUILD_DIR) 

$(I_BUILD_DIR):
	mkdir -p $(I_BUILD_DIR)

ifdef G_PCH

###########################################################################
# TBD: pre-compiled headers
###########################################################################

G_PCH:=

endif

###########################################################################
# rules for compiling files by their extension
#
# use of vpath will find any files in the given directory
###########################################################################

$(I_BUILD_DIR)/%.$(G_OBJ): %.c $(G_PCH) $(I_FORCE_REBUILD)
	@rm -f $@
	$(G_CC) $(G_PCH_CFLAGS) $(G_CFLAGS) $(U_CFLAGS) $(I_INCLUDES) $(G_SYS_INCLUDES) "$<" 
ifeq ($(G_PLATFORM),win32)
	@-$(I_GENDEP_EXE) $(I_INCLUDES) -d$(I_BUILD_DIR) -o$(G_OBJ) -sd $(U_GENDEP_FILTERS) "$<"
endif

$(I_BUILD_DIR)/%.$(G_OBJ): %.cpp $(G_PCH) $(I_FORCE_REBUILD)
	@rm -f $@
	$(G_CPPC) $(G_PCH_CFLAGS) $(G_CPPFLAGS) $(U_CFLAGS) $(U_CPPFLAGS) $(I_INCLUDES) $(G_SYS_INCLUDES) "$<"
ifeq ($(G_PLATFORM),win32)
	@-$(I_GENDEP_EXE) $(I_INCLUDES) -d$(I_BUILD_DIR) -o$(G_OBJ) -sd $(U_GENDEP_FILTERS) "$<"
endif

# Rule to build individual target
%.$(G_OBJ): $(I_BUILD_DIR)/%.$(G_OBJ)
	-

.PRECIOUS: $(I_BUILD_DIR)/%.$(G_OBJ)



###########################################################################
# makefiles are complicated things 
#
# show-var target will enable you to see what is going on (on a good day)
###########################################################################
show-var:
	@echo $($(VAR))

###########################################################################
# strategy for generating dependencies would be a fine thing
#
# is there a case for having a separate run to generate the dependencies?
# 
# what I have tried in the past is including .d files (as recommended in 
# various places) and generating the .d files afresh each time we find
# it necessary to compile the object file
#
# this is a non-trivial task - we need to include the .d files and ensure
# that when we do certain commands (e.g. make clean) that they don't get in
# the way
###########################################################################
I_DEPS=$(patsubst %.$(G_OBJ),%.d,$(I_OBJS))

-include $(I_DEPS)

###########################################################################
# vcproj file
###########################################################################

ifeq ($(G_PLATFORM),win32)

include $(U_MAKEFILES)/build/vcproj.mk

endif

# prevent gmake from trying to get files out of source control

# %: %,v
# %: RCS/%,v
# %: RCS/%
# %: s.%
# %: SCCS/s.%


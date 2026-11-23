##########################################################################
# defines the build_dirs command to build a list of directories $(1)
# using an optional target $(2)
#
# then we check the return type
#
# if we have typed CTRL-C to stop the build the rc will be >= 128 and
# we can stop the entire build
#
# regular failures from make (which usually return rc = 2) will just be
# recorded as a failed build and we go onto the next build
#
# to invoke the build_dirs command this fragment (build_dirs.mk) must be
# included at some point and you should use the following command
#
# +@$(call build_dirs,$(BUILD_DIRS),<target>,<building>,<flags>)
#
# where <target>, <building>, <flags> are optional. If <building> is not
# given then for each directory being built you get the message 
#     Building $dir
# and otherwise <building> replaces the text Building.
#
# Thus if you are running make clean, then you could do the following:
#
# +@$(call build_dirs,$(BUILD_DIRS),clean,Cleaning)
#
# If you particular wanted to run the clean command with -j1 then you
# could do the following:
#
# +@$(call build_dirs,$(BUILD_DIRS),clean,Cleaning,-j1)
##########################################################################


define build_dirs
failed=""; \
for dir in $(1); do \
    echo $(if $(3),$(3),Building) $$dir; \
    $(MAKE) $(4) -C $$dir $(2) || { \
        rc=$$?; \
        if [ $$rc -ge 128 ]; then exit $$rc; fi; \
        failed="$$failed $$dir"; \
    }; \
done; \
if [ -n "$$failed" ]; then echo "FAILED:$$failed"; exit 1; fi
endef


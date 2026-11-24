# makefile fragment to copy files from Windows to WSL
#
# pre-requisites: U_SPI_HOME, SOURCE, WSL_FILTERS
# optionals: FILES, VCS, WSL_COMMON_FILTERS, WSL_DRIVE, PROJECT_NAME, TARGET
# command line variables: WAIT, DRY_RUN, FORCE
#
# see how we define WSL_COMMON_FILTERS below as a template for your definition of WSL_FILTERS
#
# the assumption is that you have a separate directory containing your makefile and backup files
# and this directory is one level down from the root of your development tree
# and that the TARGET on the WSL_DRIVE is home/$(USER)/build/$(PROJECT_NAME)

include $(U_SPI_HOME)/makefiles/config/config.mk

SOURCE ?= ..
VCS    ?= $(if $(wildcard $(SOURCE)/.hg),hg,$(if $(wildcard $(SOURCE)/.git),git,$(error No .hg or .git in $(SOURCE) - set VCS=hg or VCS=git)))

GET_FILES ?= $(if $(filter hg,$(VCS)),hg files,git ls-files)

# File types we never need on the WSL side
ifndef WSL_COMMON_FILTERS
define WSL_COMMON_FILTERS
| grep -v '\.guid$$' \
| grep -v '\.sln$$' \
| grep -v '\.psess$$' \
| grep -v '\.dll$$' \
| grep -v '\.lib$$' \
| grep -v '\.exe$$'
endef
endif

# Project-specific filters, provided by the including makefile (may be empty)
WSL_FILTERS ?=

UPDATE_FILES=$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/updateFiles.py

ifdef DRY_RUN
UPDATE_FILES+=--dry_run
endif

ifdef WAIT
UPDATE_FILES+=--wait
endif

ifndef FORCE
UPDATE_FILES+=--protect_newer
endif

WSL_DRIVE?=W

PROJECT_NAME?=$(shell basename $(dir $(CURDIR)))
TARGET?=$(WSL_DRIVE):/home/$(USER)/build/$(PROJECT_NAME)
FILES?=wsl-copy.txt

.PHONY: help wsl-copy wsl-restore

help:
	@echo Copies files that are recorded as being in source control to/from the WSL native file system
	@echo
	@echo SOURCE=$(SOURCE)
	@echo TARGET=$(TARGET)
	@echo FILES=$(FILES)
	@echo
	@echo 'make wsl-copy'
	@echo '--- copies files from $(SOURCE) to $(TARGET)'
	@echo '--- remembers the files copied, so that next time around you can clean up old files'
	@echo
	@echo 'make wsl-restore'
	@echo '--- copies files from $(TARGET) to $(SOURCE)'
	@echo '--- no files will be deleted in $(SOURCE)'
	@echo
	@echo 'make DRY_RUN=1 wsl-copy   (or wsl-restore) - show what would happen, change nothing'
	@echo 'make FORCE=1 wsl-copy     (or wsl-restore) - overwrite files that have local changes'

wsl-copy:
	cd $(SOURCE) && $(GET_FILES) \
		$(WSL_COMMON_FILTERS) \
		$(WSL_FILTERS) \
		> $(CURDIR)/$(FILES).new
	@$(UPDATE_FILES) $(SOURCE) $(TARGET) $(FILES).new $(FILES) \
		|| { echo "updateFiles failed or found conflicts - see messages above; rerun with FORCE=1 to overwrite conflicts"; exit 1; }
ifndef DRY_RUN
	@mv $(FILES).new $(FILES)
endif

wsl-restore:
	@$(UPDATE_FILES) $(TARGET) $(SOURCE) $(FILES) \
		|| { echo "updateFiles failed or found conflicts - see messages above; rerun with FORCE=1 to overwrite conflicts"; exit 1; }


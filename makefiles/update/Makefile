help:

U_SPI_HOME=../..
include $(U_SPI_HOME)/makefiles/config/config.mk

UPDATE_FILES=$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/updateFiles.py 

ifdef DRY_RUN
UPDATE_FILES+=--dry_run
endif

ifdef WAIT
UPDATE_FILES+=--wait
endif

GET_FILES?=git ls-files

help:
	@echo Copies files that are recorded as being in source control from one directory to another
	@echo
	@echo SOURCE is the name of the source directory
	@echo TARGET is the name of the target directory
	@echo FILES is the name of the list of files to be copied and will be determined by using $(GET_FILES)
	@echo You need to provide SOURCE, TARGET, FILES on the command line
	@echo You can override the command used by defining GET_FILES on the command line
	@echo
	@echo 'make copy SOURCE=<source> TARGET=<target> FILES=<files>'
	@echo '--- copies files from <source> to <target> using the temporary file <files> to drive the copy'
	@echo '--- if you have previously used this command then we have a backup of <files> that is used'
	@echo '--- to remove any files which are no longer available'
	@echo
	@echo 'make restore SOURCE=<source> TARGET=<target> FILES=<files>'
	@echo '--- restores files from <target> to <source>'
	@echo '--- no files will be deleted in <source>'

copy:
ifndef TARGET
	@echo TARGET undefined
	exit 1
endif
ifndef SOURCE
	@echo SOURCE undefined
	exit 1
endif
ifndef FILES
	@echo FILES undefined
	exit 1
endif
	@$(MAKE) --no-print-directory backup
	@cd $(SOURCE) && $(GET_FILES) > $(CURDIR)/$(FILES)
	@$(UPDATE_FILES) $(SOURCE) $(TARGET) $(FILES) $(FILES).bak

restore:
ifndef TARGET
	@echo TARGET undefined
	exit 1
endif
ifndef SOURCE
	@echo SOURCE undefined
	exit 1
endif
ifndef FILES
	@echo FILES undefined
	exit 1
endif
	$(UPDATE_FILES) $(TARGET) $(SOURCE) $(FILES)

backup:
	@if [ -f $(FILES) ]; then cp $(FILES) $(FILES).bak; fi



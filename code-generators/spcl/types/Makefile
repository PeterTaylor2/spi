code:

U_SPI_HOME=../../..
include $(U_SPI_HOME)/makefiles/config/config.mk

U_VCXPROJ_OPTIONS=-t copy
U_SERVICE=types
U_TARGET_DIR=.
#U_SOURCES=$(wildcard config/*.cfg)

U_SPCL_OPTIONS=--noVerbatimLine --noHeaderSplit --licenseFile=../../GPL.h
ifeq ($(DEBUG),1)
U_SPCL_OPTIONS+=-w
endif
U_VCXPROJ_OPTIONS=-tcopy

include $(U_SPI_HOME)/makefiles/spi/spcl.mk

copy: code
	@$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/copyFiles.py ../src types*.h types*.hpp src/types*.[ch]pp

clean::
	rm -f *.h
	rm -f *.hpp
	rm -fr src
	rm -f *.svt
	rm -f properties.mk


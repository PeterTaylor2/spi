###########################################################################
# makefile include for generating the c++ code from configuration files
#
# in addition this will create the compiled service object which can then
# be used as a pre-requisite for builds for other platforms
#
# the main target should be 'code' and you need to define U_SERVICE
# assumption is that $(U_SERVICE).svc is in the build directory
#
# if you have any configuration files in unexpected directories, then you
# can also optional define U_SOURCES in order to only run the code
# generator when any one of the configuration source files has changed
###########################################################################
code: $(U_SERVICE).svo 

###########################################################################
# If you have the copy of spcl corresponding to your current $(G_ABI) 
# then use it - otherwise use the one in bin-$(G_PLATFORM) directory
###########################################################################
U_SPCL?=spcl

ifdef LOCAL_BUILD
I_SPCL=$(wildcard $(U_SPI_HOME)/code-generators/bin/$(G_ABI)/$(U_SPCL)$(G_EXE))
else
I_SPCL=
endif

ifeq "$(I_SPCL)" ""
I_SPCL:=$(U_SPI_HOME)/code-generators/bin-$(G_PLATFORM)/$(U_SPCL)$(G_EXE)
endif

U_SPI_BUILD_DIR?=$(G_BUILD_DIR)
I_SOURCES=$(U_SOURCES) $(U_SERVICE).svc $(wildcard *.cfg) $(wildcard *.api)
U_TARGET_DIR?=../dll
U_VCPROJ?=$(U_SERVICE)-$(notdir $(CURDIR))

I_SPCL_OPTIONS:=$(U_SPCL_OPTIONS) $(G_SPCL_OPTIONS)
ifdef U_TDIRNAME
I_SPCL_OPTIONS+=-t $(U_TDIRNAME)
endif
ifdef U_CDIRNAME
I_SPCL_OPTIONS+=-c $(U_CDIRNAME)
endif
ifdef U_VERSION
I_SPCL_OPTIONS+=--version=$(U_VERSION)
endif

$(U_SERVICE).svo: $(I_SOURCES) $(I_SPCL) $(U_FORCE_REBUILD)
	@mkdir -p $(U_TARGET_DIR)/src
	$(SPCL_WRAPPER) $(I_SPCL) $(I_SPCL_OPTIONS) $(U_SERVICE).svc $(U_SERVICE).svo $(U_SERVICE).svt $(U_TARGET_DIR)

clean::
	rm -f $(U_SERVICE).svo 

v16.vcxproj:
	$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/makeVcproj16Config.py --vcTarget=v16.vcxproj $(U_VCXPROJ_OPTIONS) -b $(G_CYGWIN_BIN) $(U_VCPROJ).v16.vcxproj $(U_VCPROJ) . $(U_SERVICE).svo

v17.vcxproj:
	$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/makeVcproj17Config.py --vcTarget=v17.vcxproj $(U_VCXPROJ_OPTIONS) -b $(G_CYGWIN_BIN) $(U_VCPROJ).v17.vcxproj $(U_VCPROJ) . $(U_SERVICE).svo

v18.vcxproj:
	$(G_PYTHON) $(U_SPI_HOME)/makefiles/python/makeVcproj18Config.py --vcTarget=v18.vcxproj $(U_VCXPROJ_OPTIONS) -b $(G_CYGWIN_BIN) $(U_VCPROJ).v18.vcxproj $(U_VCPROJ) . $(U_SERVICE).svo


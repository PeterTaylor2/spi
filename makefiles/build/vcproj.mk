###########################################################################
# Designed to make Visual C++ project files.
#
# Requires U_TARGET and U_MAKEFILES for the main library builds.
# Requires U_SRC_DIR and U_INC_DIR for the VC project build.
# Optionally uses U_VCPROJ_OPTIONS for the VC project build.
#
# You would need U_VCPROJ_OPTIONS if there are files in the U_SRC_DIR
# that you do not want to appear in your project file. Or if you wanted
# to force a specific compiler independent of the project type.
###########################################################################

I_VCPROJ9_CMD=$(G_PYTHON) $(U_MAKEFILES)/python/makeVcproj9.py
I_VCPROJ16_CMD=$(G_PYTHON) $(U_MAKEFILES)/python/makeVcproj16.py
I_VCPROJ17_CMD=$(G_PYTHON) $(U_MAKEFILES)/python/makeVcproj17.py
I_VCPROJ18_CMD=$(G_PYTHON) $(U_MAKEFILES)/python/makeVcproj18.py

ifndef U_VCPROJ
I_VCPROJ=$(U_TARGET)
else
I_VCPROJ=$(U_VCPROJ)
endif

G_CYGWIN_BIN?=$(U_MAKEFILES)/cygwin32/bin

v16.vcxproj:: 
	@$(I_VCPROJ16_CMD) $(I_INCLUDES) $(U_VCPROJ_OPTIONS) -b $(G_CYGWIN_BIN) --vcTarget=v16.vcxproj $(I_VCPROJ).v16.vcxproj $(I_VCPROJ) $(U_SRC_DIR) $(U_INC_DIR) $(G_WIN32_COMPILER) $(G_WIN32_BITS)

v17.vcxproj:: 
	@$(I_VCPROJ17_CMD) $(I_INCLUDES) $(U_VCPROJ_OPTIONS) -b $(G_CYGWIN_BIN) --vcTarget=v17.vcxproj $(I_VCPROJ).v17.vcxproj $(I_VCPROJ) $(U_SRC_DIR) $(U_INC_DIR) $(G_WIN32_COMPILER) $(G_WIN32_BITS)

v18.vcxproj:: 
	@$(I_VCPROJ18_CMD) $(I_INCLUDES) $(U_VCPROJ_OPTIONS) -b $(G_CYGWIN_BIN) --vcTarget=v18.vcxproj $(I_VCPROJ).v18.vcxproj $(I_VCPROJ) $(U_SRC_DIR) $(U_INC_DIR) $(G_WIN32_COMPILER) $(G_WIN32_BITS)


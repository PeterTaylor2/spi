# makefile responsible for updating the makefiles.vcxproj files

target: v17.vcxproj 

U_VCPROJ=makefiles
U_SRC_DIR=.
U_INC_DIR=.
U_VCPROJ_OPTIONS=-F 'python/*.py'\
-F 'build/*.mk'\
-F 'config/*.mk' -F 'config/configs/*.mk' -F 'config/computers/*.mk'\
-F 'spi/*.mk' -F 'regression-test/*.mk' -F 'regression-test/*.py'

I_TARGET=None

U_SPI_HOME=..
include $(U_SPI_HOME)/makefiles/config/config.mk
include $(U_SPI_HOME)/makefiles/build/vcproj.mk



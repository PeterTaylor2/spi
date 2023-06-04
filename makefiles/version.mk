###########################################################################
# version.mk
#
# Defines the version number of the SPI DLL, i.e. instead of spi.dll it
# will be called spi-100.dll
#
# This will be a three part number.
#
# You will also need to keep the file spi/Namespace.hpp in step with these
# version numbers. It defines the namespace will match SPI_DLL.
# Note that there is a namespace alias (spi) which is defined, so that
# code which uses spi can simple use the "spi" namespace instead of having
# to use the SPI_DLL namespace instead.
#
# The purpose of defining the namespace with a version number is again so
# that different versions of spi.dll can co-exist on a client site without
# any clashing symbols.
###########################################################################

###########################################################################
# SPI_DLL defines the name of the DLL
# we include the version number so that two different versions of spi.dll
# can co-exist on a client site
#
# Note also that any package files will also contain SPI_DLL within the
# name of the file.
###########################################################################
SPI_DLL=spi-100
SPI_UTIL_DLL=spi-util-100
SPI_CURL_DLL=spi-curl-100

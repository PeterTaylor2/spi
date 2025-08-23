SPI_BOOST_OBJS=boost-assertion-failed.$(G_OBJ)

vpath %.cpp $(U_SPI_HOME)/spi_boost/lib

U_EXTRA_OBJS=$(SPI_BOOST_OBJS)
U_INCLUDES+=-I$(U_SPI_HOME)/zlib


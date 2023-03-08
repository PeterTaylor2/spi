ZLIB_OBJS=\
adler32.$(G_OBJ)\
compress.$(G_OBJ)\
crc32.$(G_OBJ)\
deflate.$(G_OBJ)\
infback.$(G_OBJ)\
inffast.$(G_OBJ)\
inflate.$(G_OBJ)\
inftrees.$(G_OBJ)\
trees.$(G_OBJ)\
uncompr.$(G_OBJ)\
zutil.$(G_OBJ)

SPI_BOOST_OBJS=boost-assertion-failed.$(G_OBJ)

vpath %.cpp $(U_SPI_HOME)/spi_boost/lib
vpath %.c $(U_SPI_HOME)/zlib/src

U_EXTRA_OBJS=$(ZLIB_OBJS) $(SPI_BOOST_OBJS)
U_INCLUDES+=-I$(U_SPI_HOME)/zlib


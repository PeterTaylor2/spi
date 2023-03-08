###########################################################################
# Use this makefile when you change configLexer.lex.
#
# This is not part of the regular build since it requires flex to be
# installed. This might not be the case for all build machines.
#
# Finally we remove #line stuff since we move the code into the parent
# directory (also unix2dos since this is run on a Windows machine).
###########################################################################

all: ../configLexer.cpp

../configLexer.cpp: configLexer.lex Makefile ../../../../spi_util/lex/extra.includes
	flex -oconfigLexer.cpp -PyyConfig -+ configLexer.lex
	cat ../../../../spi_util/lex/extra.includes > ../configLexer.cpp
	sed -e "/^#line/d" configLexer.cpp | unix2dos >> ../configLexer.cpp
	python ../../../../../spi/makefiles/python/rstrip.py ../configLexer.cpp

clean:
	rm -f configLexer.cpp


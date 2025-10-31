###########################################################################
# Use this makefile when you change templateLexer.lex.
#
# This is not part of the regular build since it requires flex to be
# installed. This might not be the case for all build machines.
#
# Finally we remove #line stuff since we move the code into the parent
# directory (also unix2dos since this is run on a Windows machine).
###########################################################################

all: ../templateLexer.cpp

../templateLexer.cpp: templateLexer.lex Makefile ../../../../spi_util/lex/extra.includes
	flex -otemplateLexer.cpp -PyyTemplate -+ templateLexer.lex
	cat ../../../../spi_util/lex/extra.includes > ../templateLexer.cpp
	sed -e "/^#line/d" templateLexer.cpp | unix2dos >> ../templateLexer.cpp
	python ../../../../../spi/makefiles/python/rstrip.py ../templateLexer.cpp

clean:
	rm -f templateLexer.cpp


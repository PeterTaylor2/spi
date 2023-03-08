###########################################################################
# Use this makefile when you change Lexer.lex.
#
# This is not part of the regular build since it requires flex to be
# installed. This might not be the case for all developers.
#
# Finally we remove #line stuff since we move the code into the parent
# directory (also unix2dos since this is run on a Windows machine).
###########################################################################

all: ../src/Lexer.cpp ../src/JSONLexer.cpp

../src/Lexer.cpp: Lexer.lex Makefile extra.includes
	flex -oLexer.cpp -+ Lexer.lex
	cat extra.includes > ../src/Lexer.cpp
	sed -e "/^#line/d" Lexer.cpp | unix2dos >> ../src/Lexer.cpp
	python ../../makefiles/python/rstrip.py ../src/Lexer.cpp

../src/JSONLexer.cpp: JSON.lex Makefile extra.includes
	flex -Pzz -oJSONLexer.cpp -+ JSON.lex
	cat extra.includes > ../src/JSONLexer.cpp
	sed -e "/^#line/d" JSONLexer.cpp | unix2dos >> ../src/JSONLexer.cpp
	python ../../makefiles/python/rstrip.py ../src/JSONLexer.cpp

clean:
	rm -f Lexer.cpp JSONLexer.cpp


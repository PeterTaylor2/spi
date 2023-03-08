###########################################################################
# Makefile for generating doxygen-style documentation for SPI.
###########################################################################

help:

all:
	@rm -fr html
	@rm -f WARNINGS
	@echo Running doxygen - please wait...
	@../build-win32/bin/doxygen.exe Doxyfile 2> WARNINGS
	@wc -l WARNINGS

help:
	@echo "Type 'make all' to generate the HTML documentation for SPI"
	@echo "Then you can open the html/index.html file in your browser"


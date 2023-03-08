############################################################################
# given a .dll file this fragment will build a .lib file by creating 
# intermediate .exports and .def files (which make will subsequently delete)
#
# as a side effect the use of LIB will also create the .exp file
#
# this is for Windows only
############################################################################

U_MACHINE?=x86

%.def : %.exports
	$(G_PYTHON) $(U_MAKEFILES)/python/exports2def.py $< $@

%.exports: %.dll
	dumpbin /EXPORTS $< > $@

%.lib: %.def
	lib /def:$< /machine:$(U_MACHINE) /out:$@


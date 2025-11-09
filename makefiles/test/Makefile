help:
	@echo Use 'make all' to build all the libraries and executables
	@echo Use 'make clean' to clean out all the compiled code
	@echo Use 'make run' to build all the libraries and executables and then run the executable
	@echo
	@echo A successful run should print the message Hello World three times
	@echo
	@echo You can use DEBUG=1 on the command line to compile in debug mode.
	@echo You can use BITS=32 to force a 32-bit compilation

all:
	@$(MAKE) -C lib
	@$(MAKE) -C dll
	@$(MAKE) -C exe
	@$(MAKE) -C exe-lib
	@$(MAKE) -C exe-dll

run: all
	@$(MAKE) -s -C exe run
	@$(MAKE) -s -C exe-lib run
	@$(MAKE) -s -C exe-dll run

clean:
	@$(MAKE) -C lib clean
	@$(MAKE) -C dll clean
	@$(MAKE) -C exe clean
	@$(MAKE) -C exe-lib clean
	@$(MAKE) -C exe-dll clean

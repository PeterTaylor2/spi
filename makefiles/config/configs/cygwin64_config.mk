# cygwin64_config.mk

G_BASE_CONFIG:=cygwin64_gcc

ifdef COMPILER
G_BASE_CONFIG:=cygwin64_$(COMPILER)
endif


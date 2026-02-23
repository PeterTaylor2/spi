# cygwin32_config.mk

G_BASE_CONFIG:=cygwin32_gcc

ifdef COMPILER
G_BASE_CONFIG:=cygwin32_$(COMPILER)
endif


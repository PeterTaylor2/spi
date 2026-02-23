# linux64_config.mk

G_BASE_CONFIG:=linux64_gcc

ifdef COMPILER
G_BASE_CONFIG:=linux64_$(COMPILER)
endif


# macos64_config.mk

G_BASE_CONFIG:=macos64_gcc

ifdef COMPILER
G_BASE_CONFIG:=macos64_$(COMPILER)
endif


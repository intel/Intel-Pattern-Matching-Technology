ifeq ("$(strip $(CODK_DIR))", "")
  $(error Please set the CODK_DIR variable.)
endif

ARDUINOSW_DIR ?= $(CODK_DIR)/arc

current_dir = $(shell pwd)

VERBOSE  = true

LIBDIRS  = $(ARDUINOSW_DIR)/corelibs/libraries/Intel-Pattern-Matching-Technology/src \
		   $(ARDUINOSW_DIR)/corelibs/libraries/SPI/src \ 
		   $(ARDUINOSW_DIR)/corelibs/libraries/SerialFlash/src 

include $(ARDUINOSW_DIR)/Makefile.inc

all: compile

.DEFAULT_GOAL := all

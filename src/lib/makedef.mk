CC      = $(PREF)gcc
CPP     = $(PREF)g++
AR      = $(PREF)ar

CFLAGS  = -O2 -Wall -fPIC
LDFLAGS = -s

INCD    = $(ROOT)/lib
OBJD    = obj
BIND    = ../$(ROOT)/bin
LIBD    = ../$(ROOT)/lib

MAKEF   = makefile.st


ifeq ($(CMP),)
CMP = $(CC)
endif

ifeq ($(EXT),)
EXT = c
endif

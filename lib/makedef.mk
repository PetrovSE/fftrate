CC      = gcc
CPP     = g++
AR      = ar rcus

CSHARED = -fPIC
CFLAGS  = -O2 -Wall -fdata-sections -ffunction-sections
LDFLAGS = -s -Wl,--gc-sections

OBJD    = obj
BIND    = ../bin
BINLIBD = $(BIND)/linux

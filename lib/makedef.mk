CC      = gcc
CPP     = g++
AR      = ar rcus

CFLAGS  = -O2 -Wall -fPIC -fdata-sections -ffunction-sections
LDFLAGS = -s -Wl,--gc-sections

OBJD    = obj
BIND    = bin

MAKEF   = makefile.st

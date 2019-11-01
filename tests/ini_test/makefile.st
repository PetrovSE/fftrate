MODULE = ini_test
FILES  = main.o
LIBS   = -lcmdline -lm
ROOT   = ../..

CMP    = $(CC)
EXT    = c

include $(ROOT)/lib/makedef.mk
include $(ROOT)/lib/makeapp.mk

MODULE = arateconf
FILES  = alsa.o conf.o main.o
LIBS   = -lmathex -lcmdline -lthr -lasound -lm -lpthread
ROOT   = ../..

CMP    = $(CC)
EXT    = c

include $(ROOT)/lib/makedef.mk
include $(ROOT)/lib/makeapp.mk

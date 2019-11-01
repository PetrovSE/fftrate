MODULE = pcm_conv
DIRC   = ../pcm_common

FILES  = Main.o $(DIRC)/Common.o $(DIRC)/InfoConv.o
LIBS   = -lconvert -lfft -lmathex -lprofiler -lcmdline -lriffio -lthr -lm -lpthread

ROOT   = ../..

CMP    = $(CPP)
EXT    = cpp

include $(ROOT)/lib/makedef.mk
include $(ROOT)/lib/makeapp.mk

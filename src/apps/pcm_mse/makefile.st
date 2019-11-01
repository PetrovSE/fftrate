MODULE = pcm_mse
DIRC   = ../pcm_common

FILES  = Main.o $(DIRC)/Common.o $(DIRC)/InfoConv.o
LIBS   = -lconvert -lfft -lmathex -lriffio -lcmdline -lprofiler -lm

ROOT   = ../..

CMP    = $(CPP)
EXT    = cpp

include $(ROOT)/lib/makedef.mk
include $(ROOT)/lib/makeapp.mk

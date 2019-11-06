MODULE = pcm_mse

FILES  = Main.o
LIBS   = -lpcm_common -lconvert -lfft -lmathex -lriffio -lcmdline -lprofiler -lm

ROOT   = ../..

CMP    = $(CPP)
EXT    = cpp

include $(ROOT)/lib/makedef.mk
include $(ROOT)/lib/makeapp.mk

MODULE = pcm_stretch
DIRC   = ../pcm_common

FILES  = Main.o
LIBS   = -lpcm_common -lfft -lmathex -lprofiler -lcmdline -lriffio -lthr -lm -lpthread

ROOT   = ../..

CMP    = $(CPP)
EXT    = cpp

include $(ROOT)/lib/makedef.mk
include $(ROOT)/lib/makeapp.mk

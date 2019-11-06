MODULE = pcm_info

FILES  = Main.o
LIBS   = -lpcm_common -lriffio -lmathex -lcmdline

ROOT   = ../..

CMP    = $(CPP)
EXT    = cpp

include $(ROOT)/lib/makedef.mk
include $(ROOT)/lib/makeapp.mk

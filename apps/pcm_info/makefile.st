MODULE = pcm_info
DIRC   = ../pcm_common

FILES  = Main.o $(DIRC)/Common.o
LIBS   = -lriffio -lmathex -lcmdline

ROOT   = ../..

CMP    = $(CPP)
EXT    = cpp

include $(ROOT)/lib/makedef.mk
include $(ROOT)/lib/makeapp.mk

MODULE = pcm_info
DIRC   = ../pcm_common

FILES  = Main.o $(DIRC)/Common.o $(DIRC)/InfoConv.o
LIBS   = -lriffio -lmathex -lcmdline

CMP    = $(CPP)
EXT    = cpp

include ../../lib/makedef.mk
include ../../lib/makeapp.mk

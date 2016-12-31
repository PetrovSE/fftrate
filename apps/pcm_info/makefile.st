MODULE = pcm_info
DIRC   = ../pcm_common

FILES  = Main.o $(DIRC)/Common.o
LIBS   = -lriffio -lmathex -lcmdline

CMP    = $(CPP)
EXT    = cpp

include ../../projects/makedef.mk
include ../maketst.mk

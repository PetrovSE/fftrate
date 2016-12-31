MODULE = pcm_conv
DIRC   = ../pcm_common

FILES  = Main.o $(DIRC)/Common.o $(DIRC)/InfoConv.o
LIBS   = -lconvert -lfft -lmathex -lprofiler -lcmdline -lriffio -lthr -lm -lpthread

CMP    = $(CPP)
EXT    = cpp

include ../../projects/makedef.mk
include ../maketst.mk

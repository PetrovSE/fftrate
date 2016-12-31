MODULE = pcm_mse
DIRC   = ../pcm_common

FILES  = Main.o $(DIRC)/Common.o $(DIRC)/InfoConv.o
LIBS   = -lconvert -lfft -lmathex -lriffio -lcmdline -lprofiler -lm

CMP    = $(CPP)
EXT    = cpp

include ../../lib/makedef.mk
include ../../lib/makeapp.mk

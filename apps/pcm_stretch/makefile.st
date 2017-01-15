MODULE = pcm_stretch
DIRC   = ../pcm_common

FILES  = Main.o $(DIRC)/Common.o
LIBS   = -lfft -lmathex -lprofiler -lcmdline -lriffio -lthr -lm -lpthread

CMP    = $(CPP)
EXT    = cpp

include ../../lib/makedef.mk
include ../../lib/makeapp.mk

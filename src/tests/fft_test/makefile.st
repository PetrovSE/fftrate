MODULE = fft_test
FILES  = main.o
LIBS   = -lmathex -lfft -lprofiler -lcmdline -lm
ROOT   = ../..

CMP    = $(CC)
EXT    = c

include $(ROOT)/lib/makedef.mk
include $(ROOT)/lib/makeapp.mk

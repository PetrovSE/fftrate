MODULE = fft_test
FILES  = main.o
LIBS   = -lmathex -lfft -lprofiler -lcmdline -lm

CMP    = $(CC)
EXT    = c

include ../../lib/makedef.mk
include ../../lib/makeapp.mk

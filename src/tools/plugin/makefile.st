MODULE = asound_module_rate_fftrate
FILES  = plugin.o
LIBS   = -lconvert -lfft -lriffio -lmathex -lcmdline -lm -lasound
ROOT   = ../..

CMP    = $(CC)
EXT    = c

include ../../lib/makedef.mk
include ../../lib/makeso.mk

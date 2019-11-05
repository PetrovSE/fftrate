MODULE = asound_module_rate_fftrate
FILES  = plugin.o
LIBS   = -lconvert -lfft -lriffio -lmathex -lcmdline -lm -lasound
ROOT   = ../../..

CMP    = $(CC)
EXT    = c

include $(ROOT)/lib/makedef.mk
include $(ROOT)/lib/makeso.mk

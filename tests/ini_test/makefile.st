MODULE = ini_test
FILES  = main.o
LIBS   = -lcmdline -lm

CMP    = $(CC)
EXT    = c

include ../../lib/makedef.mk
include ../../lib/makeapp.mk

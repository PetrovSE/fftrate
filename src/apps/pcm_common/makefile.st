MODULE = pcm_common

FILES  = Common.o  InfoConv.o
ROOT   = ../..

CMP    = $(CPP)
EXT    = cpp

include $(ROOT)/lib/makedef.mk
include $(ROOT)/lib/makelib.mk

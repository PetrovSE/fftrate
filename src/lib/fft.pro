include (build.pri)

TARGET   = fft
TEMPLATE = lib

CONFIG += staticlib c++11
CONFIG -= app_bundle
CONFIG -= qt gui

SOURCES += \
	fft/dct4.c	\
	fft/fft.c	\
	fft/mdct.c

HEADERS += \
	dct4.h		\
	fft.h		\
	mdct.h		\
	fft/dct4lib.h	\
	fft/fftlib.h	\
	fft/mdctlib.h	\
	fft/proftodo.inc

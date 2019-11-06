include (build.pri)

TARGET   = riffio
TEMPLATE = lib

CONFIG += staticlib c++11
CONFIG -= app_bundle
CONFIG -= qt gui

SOURCES += \
	riffio/mmsys.c	\
	riffio/waveio.c

HEADERS += \
	mmsys.h		\
	waveio.h	\
	riffio/waveiolib.h

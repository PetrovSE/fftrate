include (../../lib/build.pri)

TARGET   = arateconf
TEMPLATE = app

CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt gui

SOURCES += \
	arateconf/alsa.c	\
	arateconf/conf.c	\
	arateconf/main.c

HEADERS += \
	arateconf/conf.h

INCLUDEPATH += ../../lib

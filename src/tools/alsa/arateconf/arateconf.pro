include (../../lib/build.pri)

TARGET   = arateconf
TEMPLATE = app

CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt gui

SOURCES += \
	alsa.c	\
	conf.c	\
	main.c

HEADERS += \
	conf.h

INCLUDEPATH += ../../lib

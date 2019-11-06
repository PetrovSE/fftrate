include (../../lib/build.pri)

TARGET   = pcm_stretch
TEMPLATE = app

CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt gui

QMAKE_LIBDIR += ../pcm_common/$${DESTDIR}
LIBS += -lpcm_common

SOURCES += Main.cpp

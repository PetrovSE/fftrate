include (build.pri)

TARGET   = stretch
TEMPLATE = lib

CONFIG += staticlib c++11
CONFIG -= app_bundle
CONFIG -= qt gui

SOURCES += \
	stretch/stretch.c

HEADERS += \
	stretch.h				\
	stretch/stretchlib.h

include (build.pri)

TARGET   = mathex
TEMPLATE = lib

CONFIG += staticlib c++11
CONFIG -= app_bundle
CONFIG -= qt gui

SOURCES += \
	mathex/fpux86.c	\
	mathex/mathex.c

HEADERS += \
	complex.h	\
	fpux86.h	\
	mathex.h	\
	mathmac.h	\
	mathex/mathexlib.h

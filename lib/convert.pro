include (build.pri)

TARGET   = convert
TEMPLATE = lib

CONFIG += staticlib c++11
CONFIG -= app_bundle
CONFIG -= qt gui

SOURCES += \
	convert/convert.c	\
	convert/convertlib.c

HEADERS += \
	convert.h				\
	convert/convertlib.h	\
	convert/proftodo.inc

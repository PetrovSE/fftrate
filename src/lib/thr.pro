include (build.pri)

TARGET   = thr
TEMPLATE = lib

CONFIG += staticlib c++11
CONFIG -= app_bundle
CONFIG -= qt gui

SOURCES += \
	thr/thr_posix.c	\
	thr/thr_win.c

HEADERS += \
	thr.h				\
	thr/thrlib.h		\
	thr/thrlib_posix.h	\
	thr/thrlib_win.h

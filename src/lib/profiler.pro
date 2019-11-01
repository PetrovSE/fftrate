include (build.pri)

TARGET   = profiler
TEMPLATE = lib

CONFIG += staticlib c++11
CONFIG -= app_bundle
CONFIG -= qt gui

SOURCES += \
	profiler/clock.c	\
	profiler/profile.c

HEADERS += \
	clock.h		\
	profdata.h	\
	profile.h	\
	profiler/proftodo.inc

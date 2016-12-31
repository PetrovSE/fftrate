include (build.pri)

TARGET   = common
TEMPLATE = lib

CONFIG += staticlib c++11
CONFIG -= app_bundle
CONFIG -= qt gui

SOURCES +=

HEADERS += \
	array.h		\
	interface.h	\
	swin.h		\
	types.h

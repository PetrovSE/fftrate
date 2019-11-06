include (../../lib/build.pri)

TARGET   = pcm_common
TEMPLATE = lib

CONFIG += staticlib c++11
CONFIG -= app_bundle
CONFIG -= qt gui

SOURCES +=		\
	Common.cpp	\
	InfoConv.cpp

HEADERS += \
	Common.h	\
	InfoConv.h

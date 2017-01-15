include (../../lib/build.pri)

TARGET   = pcm_stretch
TEMPLATE = app

CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt gui

SOURCES +=		\
	Main.cpp	\
	../pcm_common/Common.cpp

HEADERS += \
	../pcm_common/Common.h

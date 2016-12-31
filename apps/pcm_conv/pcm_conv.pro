include (../../lib/build.pri)

TARGET   = pcm_conv
TEMPLATE = app

CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt gui

SOURCES +=		\
	Main.cpp	\
	../pcm_common/Common.cpp	\
	../pcm_common/InfoConv.cpp

HEADERS += \
	../pcm_common/Common.h		\
	../pcm_common/InfoConv.h

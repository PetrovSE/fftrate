include (../../lib/build.pri)

TARGET   = ini_test
TEMPLATE = app

CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt gui

SOURCES +=		\
	main.c

HEADERS +=

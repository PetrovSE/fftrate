include (build.pri)

TARGET   = cmdline
TEMPLATE = lib

CONFIG += staticlib c++11
CONFIG -= app_bundle
CONFIG -= qt gui

SOURCES += \
	cmdline/cmdline.c	\
	cmdline/getopt.c	\
	cmdline/inifile.c	\
	cmdline/kbhit.c		\
	cmdline/menu.c		\
	cmdline/str.c

HEADERS += \
	cmdline.h	\
	getoptw.h	\
	inifile.h	\
	kbhit.h		\
	menu.h		\
	str.h		\
	cmdline/getoptlib.h		\
	cmdline/inifilelib.h	\
	cmdline/kbhitlib.h		\
	cmdline/strlib.h

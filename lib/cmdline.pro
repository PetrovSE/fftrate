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
	cmdline/logfile.c	\
	cmdline/menu.c		\
	cmdline/plug.c		\
	cmdline/str.c

HEADERS += \
	cmdline.h	\
	getoptw.h	\
	inifile.h	\
	kbhit.h		\
	logfile.h	\
	menu.h		\
	plug.h		\
	str.h		\
	cmdline/getoptlib.h		\
	cmdline/inifilelib.h	\
	cmdline/kbhitlib.h		\
	cmdline/logfilelib.h	\
	cmdline/pluglib.h		\
	cmdline/strlib.h

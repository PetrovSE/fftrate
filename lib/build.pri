
CONFIG( debug, debug | release ) {
    DESTDIR = debug
}
else {
    DESTDIR = release
}

OBJECTS_DIR = $${DESTDIR}

QMAKE_LIBDIR += ../../lib/$${DESTDIR}

LIBS +=	\
	-lcmdline	\
	-lconvert	\
	-lfft		\
	-lmathex	\
	-lprofiler	\
	-lriffio	\
	-lthr		\
	-lAdvapi32

win32 {
    DEFINES += _UNICODE _CRT_SECURE_NO_WARNINGS
	LIBS	+= -lAdvapi32
}

# common to plugin and built-in forms
INCLUDEPATH *= $$PWD
HEADERS += $$PWD/qjpeghandler_p.h
SOURCES += $$PWD/qjpeghandler.cpp
if(unix|win32-g++*): LIBS += -ljpeg
else:win32:          LIBS += libjpeg.lib

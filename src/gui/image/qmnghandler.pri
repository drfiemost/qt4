# common to plugin and built-in forms
INCLUDEPATH *= $$PWD
HEADERS += $$PWD/qmnghandler_p.h
SOURCES += $$PWD/qmnghandler.cpp
if(unix|win32-g++*):LIBS += -lmng
else:win32:         LIBS += libmng.lib

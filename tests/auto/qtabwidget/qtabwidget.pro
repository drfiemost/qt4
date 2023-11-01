CONFIG += testcase

TARGET = tst_qtabwidget
QT += testlib
SOURCES += tst_qtabwidget.cpp

INCLUDEPATH += ../

HEADERS +=  

win32:!wince*:LIBS += -luser32

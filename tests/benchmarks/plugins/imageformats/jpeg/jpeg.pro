load(qttest_p4)
TEMPLATE = app
TARGET = jpeg
DEPENDPATH += .
INCLUDEPATH += .

CONFIG += release

DEFINES += SRCDIR=\\\"$$PWD/\\\"

# Input
SOURCES += jpeg.cpp

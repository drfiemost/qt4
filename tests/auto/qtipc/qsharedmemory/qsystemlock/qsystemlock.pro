CONFIG += testcase

TARGET = tst_qsystemlock
QT += testlib
SOURCES += tst_qsystemlock.cpp

include(../src/src.pri)
win32: CONFIG += console
mac:CONFIG -= app_bundle

DEFINES	+= SRCDIR=\\\"$$PWD\\\"

DESTDIR = ./

DEFINES	+= QSHAREDMEMORY_DEBUG
DEFINES	+= QSYSTEMSEMAPHORE_DEBUG

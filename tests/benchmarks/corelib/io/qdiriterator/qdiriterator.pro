load(qttest_p4)
TEMPLATE = app
TARGET = tst_bench_qdiriterator
DEPENDPATH += .
INCLUDEPATH += .

QT -= gui

CONFIG += release

SOURCES += main.cpp

SOURCES += qfilesystemiterator.cpp
HEADERS += qfilesystemiterator.h

load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
QT += sql script
macx:CONFIG -= app_bundle

SOURCES += tst_qdeclarativesqldatabase.cpp

DEFINES += SRCDIR=\\\"$$PWD\\\"

CONFIG += parallel_test


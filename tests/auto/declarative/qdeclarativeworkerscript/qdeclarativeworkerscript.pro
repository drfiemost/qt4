load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative script
macx:CONFIG -= app_bundle

SOURCES += tst_qdeclarativeworkerscript.cpp

DEFINES += SRCDIR=\\\"$$PWD\\\"

CONFIG += parallel_test


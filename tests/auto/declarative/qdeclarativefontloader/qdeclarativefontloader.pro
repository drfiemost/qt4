load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative gui network
macx:CONFIG -= app_bundle

HEADERS += ../shared/testhttpserver.h
SOURCES += tst_qdeclarativefontloader.cpp ../shared/testhttpserver.cpp

DEFINES += SRCDIR=\\\"$$PWD\\\"

CONFIG += parallel_test


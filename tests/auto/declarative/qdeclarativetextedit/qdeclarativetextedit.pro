load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative gui network
macx:CONFIG -= app_bundle

SOURCES += tst_qdeclarativetextedit.cpp ../shared/testhttpserver.cpp
HEADERS += ../shared/testhttpserver.h

DEFINES += SRCDIR=\\\"$$PWD\\\"

CONFIG+=insignificant_test # QTQAINFRA-574

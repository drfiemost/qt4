load(qttest_p4)
SOURCES  += ../tst_qfile.cpp

# Activate this define to be able to run the test without test network infrastructure.
# DEFINES += NO_NETWORK_TEST

CONFIG -= app_bundle

QT = core network
DEFINES += SRCDIR=\\\"$$PWD/../\\\"

RESOURCES      += ../qfile.qrc ../rename-fallback.qrc ../copy-fallback.qrc

TARGET = ../tst_qfile

win32 {
    CONFIG(debug, debug|release) {
        TARGET = ../../debug/tst_qfile
    } else {
        TARGET = ../../release/tst_qfile
    }
    LIBS+=-lole32 -luuid
}

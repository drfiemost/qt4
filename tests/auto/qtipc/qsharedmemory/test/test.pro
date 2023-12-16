CONFIG += testcase

TARGET = ../tst_qsharedmemory
QT = core testlib
SOURCES += ../tst_qsharedmemory.cpp

include(../src/src.pri)

DEFINES	+= QSHAREDMEMORY_DEBUG
DEFINES	+= QSYSTEMSEMAPHORE_DEBUG

win32 {
  CONFIG(debug, debug|release) {
    TARGET = ../../debug/tst_qsharedmemory
  } else {
    TARGET = ../../release/tst_qsharedmemory
  }
}


DEFINES += SRCDIR=\\\"$$PWD/../\\\"

CONFIG+=insignificant_test # QTQAINFRA-574

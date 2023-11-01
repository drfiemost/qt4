CONFIG += testcase

TARGET = ../tst_qtextstream
QT = core network testlib
SOURCES  += ../tst_qtextstream.cpp

win32 {
  CONFIG(debug, debug|release) {
    TARGET = ../../debug/tst_qtextstream
  } else {
    TARGET = ../../release/tst_qtextstream
  }
}

RESOURCES += ../qtextstream.qrc

wince*: {
   addFiles.files = ../rfc3261.txt ../shift-jis.txt ../task113817.txt ../qtextstream.qrc ../tst_qtextstream.cpp
   addFiles.path = .
   res.files = ../resources
   res.path = .
   DEPLOYMENT += addFiles
}

wince*: {
    DEFINES += SRCDIR=\\\"\\\"
} else {
    DEFINES += SRCDIR=\\\"$$PWD/../\\\"
}

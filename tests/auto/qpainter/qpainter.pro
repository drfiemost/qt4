CONFIG += testcase

TARGET = tst_qpainter
QT += testlib
SOURCES  += tst_qpainter.cpp


wince*: {
    addFiles.files = drawEllipse drawLine_rop_bitmap drawPixmap_rop drawPixmap_rop_bitmap task217400.png
    addFiles.path = .
    DEPLOYMENT += addFiles
}

wince* {
    DEFINES += SRCDIR=\\\".\\\"
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

macx:CONFIG+=insignificant_test # QTQAINFRA-574

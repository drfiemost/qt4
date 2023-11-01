CONFIG += testcase

TARGET = tst_qimage
QT += testlib
SOURCES  += tst_qimage.cpp

wince*: {
   addImages.files = images/*
   addImages.path = images
   DEPLOYMENT += addImages
   DEFINES += SRCDIR=\\\".\\\"
} else {
   DEFINES += SRCDIR=\\\"$$PWD\\\"
}

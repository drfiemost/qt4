load(qttest_p4)

TEMPLATE = app
!embedded:CONFIG += uitools
TARGET = ../tst_uiloader
DEFINES += SRCDIR=\\\"$$PWD\\\"

win32 {
  CONFIG(debug, debug|release) {
    TARGET = ../../debug/tst_uiloader
} else {
    TARGET = ../../release/tst_uiloader
  }
}

QT += xml svg network

wince*: {
   configuration.files = ../*.ini
   configuration.path = .

   screenapp.files = ../tst_screenshot/tst_screenshot.exe
   screenapp.path = tst_screenshot
   
   DEPLOYMENT += configuration screenapp
}

# Input
HEADERS += uiloader.h
SOURCES += tst_uiloader.cpp uiloader.cpp

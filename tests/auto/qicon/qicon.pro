load(qttest_p4)

SOURCES += tst_qicon.cpp
RESOURCES = tst_qicon.qrc

wince* {
   QT += xml
   addFiles.files += $$_PRO_FILE_PWD_/*.png
   addFiles.files += $$_PRO_FILE_PWD_/tst_qicon.cpp
   addFiles.path = .
   DEPLOYMENT += addFiles

   DEFINES += SRCDIR=\\\".\\\"
} else {
   DEFINES += SRCDIR=\\\"$$PWD\\\"
}


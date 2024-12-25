load(qttest_p4)

SOURCES += tst_qicon_svg.cpp

wince* {
   QT += xml svg
   addFiles.files += $$_PRO_FILE_PWD_/*.svg
   addFiles.files += $$_PRO_FILE_PWD_/*.svgz
   addFiles.files += $$_PRO_FILE_PWD_/tst_qicon_svg.cpp
   addFiles.path = .
   DEPLOYMENT += addFiles

   DEPLOYMENT_PLUGIN += qsvg
   DEFINES += SRCDIR=\\\".\\\"
} else {
   DEFINES += SRCDIR=\\\"$$PWD\\\"
}


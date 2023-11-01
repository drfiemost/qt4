load(qttest_p4)
SOURCES += tst_qtextbrowser.cpp
DEFINES += SRCDIR=\\\"$$PWD\\\"

wince*: {
   addFiles.files = *.html
   addFiles.path = .
   addDir.files = subdir/*
   addDir.path = subdir
   DEPLOYMENT += addFiles addDir
}




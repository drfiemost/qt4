load(qttest_p4)
SOURCES += tst_qdatastream.cpp

!symbian: {
cross_compile: DEFINES += SVGFILE=\\\"tests2.svg\\\"
else: DEFINES += SVGFILE=\\\"gearflowers.svg\\\"
}

# for qpaintdevicemetrics.h
QT += svg


wince*: {
   addFiles.files = datastream.q42 tests2.svg
   addFiles.path = .
   DEPLOYMENT += addFiles
   DEFINES += SRCDIR=\\\"\\\"
}else {
   DEFINES += SRCDIR=\\\"$$PWD/\\\"
}


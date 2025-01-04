load(qttest_p4)
SOURCES += tst_qimagereader.cpp
MOC_DIR=tmp
QT += network
RESOURCES += qimagereader.qrc
DEFINES += SRCDIR=\\\"$$PWD\\\"

!contains(QT_CONFIG, no-gif):DEFINES += QTEST_HAVE_GIF
!contains(QT_CONFIG, no-jpeg):DEFINES += QTEST_HAVE_JPEG
!contains(QT_CONFIG, no-tiff):DEFINES += QTEST_HAVE_TIFF
!contains(QT_CONFIG, no-svg):DEFINES += QTEST_HAVE_SVG
!contains(QT_CONFIG, no-tga):DEFINES += QTEST_HAVE_TGA

win32-msvc:QMAKE_CXXFLAGS -= -Zm200
win32-msvc:QMAKE_CXXFLAGS += -Zm800
win32-msvc.net:QMAKE_CXXFLAGS -= -Zm300
win32-msvc.net:QMAKE_CXXFLAGS += -Zm1100

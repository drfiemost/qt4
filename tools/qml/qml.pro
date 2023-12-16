TEMPLATE = app
CONFIG += qt uic declarative_debug
DESTDIR = ../../bin

include(qml.pri)

SOURCES += main.cpp

INCLUDEPATH += ../../include/QtDeclarative
INCLUDEPATH += ../../src/declarative/util
INCLUDEPATH += ../../src/declarative/graphicsitems

DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII

target.path = $$[QT_INSTALL_BINS]
INSTALLS += target

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

mac {
    QMAKE_INFO_PLIST=Info_mac.plist
    TARGET=QMLViewer
    ICON=qml.icns
} else {
    TARGET=qmlviewer
}

TARGET  = qmlfolderlistmodelplugin
TARGETPATH = Qt/labs/folderlistmodel
include(../qimportbase.pri)

QT += declarative script

SOURCES += qdeclarativefolderlistmodel.cpp plugin.cpp
HEADERS += qdeclarativefolderlistmodel.h

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/imports/$$TARGETPATH
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir

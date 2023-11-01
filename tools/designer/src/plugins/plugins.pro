TEMPLATE = subdirs
CONFIG += ordered

REQUIRES = !CONFIG(static,shared|static)
win32: SUBDIRS += activeqt
# contains(QT_CONFIG, opengl): SUBDIRS += tools/view3d
contains(QT_CONFIG, declarative): SUBDIRS += qdeclarativeview

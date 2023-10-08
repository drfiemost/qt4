TEMPLATE = lib
CONFIG += qt plugin
QT += declarative

DESTDIR = QWidgets
TARGET = qmlqwidgetsplugin

SOURCES += qwidgets.cpp

sources.files += qwidgets.pro qwidgets.cpp qwidgets.qml
sources.path += $$[QT_INSTALL_EXAMPLES]/declarative/cppextensions/plugins
target.path += $$[QT_INSTALL_EXAMPLES]/declarative/cppextensions/plugins

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

INSTALLS += sources target

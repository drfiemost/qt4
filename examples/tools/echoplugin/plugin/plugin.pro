#! [0]
TEMPLATE        = lib
CONFIG         += plugin
INCLUDEPATH    += ../echowindow
HEADERS         = echoplugin.h
SOURCES         = echoplugin.cpp
TARGET          = $$qtLibraryTarget(echoplugin)
DESTDIR         = ../plugins
#! [0]

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tools/echoplugin/plugin
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS plugin.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tools/echoplugin/plugin
INSTALLS += target sources

simulator: warning(This example might not fully work on Simulator platform)

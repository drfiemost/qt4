
QT       += core gui

TARGET = applicationicon
TEMPLATE = app

SOURCES += main.cpp

OTHER_FILES += applicationicon.svg \
               applicationicon.png \
               applicationicon.desktop

simulator: warning(This example might not fully work on Simulator platform)

# Nokia Qt Examples: elided label example

QT += core gui

TARGET = elidedlabel
TEMPLATE = app

SOURCES += \
    main.cpp\
    testwidget.cpp \
    elidedlabel.cpp

HEADERS += \
    testwidget.h \
    elidedlabel.h

CONFIG += mobility
MOBILITY =

simulator: warning(This example might not fully work on Simulator platform)

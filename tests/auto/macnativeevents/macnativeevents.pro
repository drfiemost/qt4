######################################################################
# Automatically generated by qmake (2.01a) Wed Nov 29 22:24:47 2006
######################################################################

load(qttest_p4)
TEMPLATE = app
DEPENDPATH += .
INCLUDEPATH += .
LIBS += -framework Carbon

HEADERS += qnativeevents.h nativeeventlist.h expectedeventlist.h
SOURCES += qnativeevents.cpp qnativeevents_mac.cpp 
SOURCES += expectedeventlist.cpp nativeeventlist.cpp
SOURCES += tst_macnativeevents.cpp

requires(mac)
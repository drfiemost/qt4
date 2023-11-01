load(qttest_p4)
SOURCES  += tst_qsettings.cpp
RESOURCES += qsettings.qrc

CONFIG -= debug
CONFIG += release
win32-msvc*:LIBS += advapi32.lib

CONFIG += parallel_test

win32:CONFIG+=insignificant_test # QTQAINFRA-574

load(qttest_p4)

QT = core
SOURCES += ../tst_quuid.cpp
TARGET = tst_quuid

CONFIG(debug_and_release_target) {
  CONFIG(debug, debug|release) {
    DESTDIR = ../debug
  } else {
    DESTDIR = ../release
  }
} else {
  DESTDIR = ..
}

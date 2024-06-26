# Qt core thread module

# public headers
HEADERS += thread/qmutex.h \
           thread/qreadwritelock.h \
           thread/qsemaphore.h \
           thread/qthread.h \
           thread/qthreadstorage.h \
           thread/qwaitcondition.h \
           thread/qatomic.h \
           thread/qbasicatomic.h \
           thread/qgenericatomic.h

# private headers
HEADERS += thread/qmutex_p.h \
           thread/qmutexpool_p.h \
           thread/qfutex_p.h \
           thread/qorderedmutexlocker_p.h \
           thread/qreadwritelock_p.h \
           thread/qthread_p.h \
           thread/qlocking_p.h

SOURCES += thread/qatomic.cpp \
           thread/qmutex.cpp \
           thread/qreadwritelock.cpp \
           thread/qmutexpool.cpp \
           thread/qsemaphore.cpp \
           thread/qthread.cpp \
           thread/qthreadstorage.cpp

unix:SOURCES += thread/qthread_unix.cpp \
                thread/qwaitcondition_unix.cpp

win32:SOURCES += thread/qmutex_win.cpp \
                 thread/qthread_win.cpp \
		 thread/qwaitcondition_win.cpp

unix: {
    macx-* {
        SOURCES += thread/qmutex_mac.cpp
    } else:linux-*:!linux-lsb-* {
        SOURCES += thread/qmutex_linux.cpp
    } else {
        SOURCES += thread/qmutex_unix.cpp
    }
}

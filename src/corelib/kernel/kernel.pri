# Qt core object module

HEADERS += \
        kernel/qabstracteventdispatcher.h \
        kernel/qabstractitemmodel.h \
        kernel/qabstractitemmodel_p.h \
        kernel/qbasictimer.h \
        kernel/qdeadlinetimer.h \
        kernel/qdeadlinetimer_p.h \
        kernel/qelapsedtimer.h \
        kernel/qeventloop.h\
        kernel/qpointer.h \
        kernel/qcorecmdlineargs_p.h \
        kernel/qcoreapplication.h \
        kernel/qcoreevent.h \
        kernel/qmetaobject.h \
        kernel/qmetaobject_moc_p.h \
        kernel/qmetaobjectbuilder_p.h \
        kernel/qmetatype.h \
        kernel/qmetatype_p.h \
        kernel/qmimedata.h \
        kernel/qobject.h \
        kernel/qobjectdefs.h \
        kernel/qsignalmapper.h \
        kernel/qsocketnotifier.h \
        kernel/qtimer.h \
        kernel/qtranslator.h \
        kernel/qtranslator_p.h \
        kernel/qvariant.h \
        kernel/qabstracteventdispatcher_p.h \
        kernel/qcoreapplication_p.h \
        kernel/qobjectcleanuphandler.h \
        kernel/qvariant_p.h \
        kernel/qmetaobject_p.h \
        kernel/qobject_p.h \
        kernel/qcoreglobaldata_p.h \
        kernel/qsharedmemory.h \
        kernel/qsharedmemory_p.h \
        kernel/qsystemsemaphore.h \
        kernel/qsystemsemaphore_p.h \
        kernel/qfunctions_p.h \
        kernel/qmath.h \
        kernel/qsystemerror_p.h

SOURCES += \
        kernel/qabstracteventdispatcher.cpp \
        kernel/qabstractitemmodel.cpp \
        kernel/qbasictimer.cpp \
        kernel/qeventloop.cpp \
        kernel/qcoreapplication.cpp \
        kernel/qcoreevent.cpp \
        kernel/qdeadlinetimer.cpp \
        kernel/qelapsedtimer.cpp \
        kernel/qmetaobject.cpp \
        kernel/qmetatype.cpp \
        kernel/qmetaobjectbuilder.cpp \
        kernel/qmimedata.cpp \
        kernel/qobject.cpp \
        kernel/qobjectcleanuphandler.cpp \
        kernel/qsignalmapper.cpp \
        kernel/qsocketnotifier.cpp \
        kernel/qtimer.cpp \
        kernel/qtranslator.cpp \
        kernel/qvariant.cpp \
        kernel/qcoreglobaldata.cpp \
        kernel/qsharedmemory.cpp \
        kernel/qsystemsemaphore.cpp \
        kernel/qpointer.cpp \
        kernel/qmath.cpp \
        kernel/qsystemerror.cpp

win32 {
        SOURCES += \
                kernel/qeventdispatcher_win.cpp \
                kernel/qcoreapplication_win.cpp \
                kernel/qelapsedtimer_win.cpp \
                kernel/qwineventnotifier_p.cpp \
                kernel/qsharedmemory_win.cpp \
                kernel/qsystemsemaphore_win.cpp
        HEADERS += \
                kernel/qeventdispatcher_win_p.h \
                kernel/qwineventnotifier_p.h
}

mac:!embedded:!qpa{
    SOURCES += \
        kernel/qcoreapplication_mac.cpp
}

mac {
       SOURCES += \
                kernel/qcore_mac.cpp \
                kernel/qelapsedtimer_mac.cpp
}

unix {
    SOURCES += \
            kernel/qcore_unix.cpp \
            kernel/qcrashhandler.cpp \
            kernel/qelapsedtimer_unix.cpp \
            kernel/qsharedmemory_unix.cpp \
            kernel/qsystemsemaphore_unix.cpp \
            kernel/qeventdispatcher_unix.cpp \
            kernel/qtimerinfo_unix.cpp
    HEADERS += \
            kernel/qcore_unix_p.h \
            kernel/qcrashhandler_p.h \
            kernel/qeventdispatcher_unix_p.h \
            kernel/qtimerinfo_unix_p.h

    contains(QT_CONFIG, glib) {
        SOURCES += \
            kernel/qeventdispatcher_glib.cpp
        HEADERS += \
            kernel/qeventdispatcher_glib_p.h
        QMAKE_CXXFLAGS += $$QT_CFLAGS_GLIB
        LIBS_PRIVATE +=$$QT_LIBS_GLIB
    }

   contains(QT_CONFIG, clock-gettime):include($$QT_SOURCE_TREE/config.tests/unix/clock-gettime/clock-gettime.pri)
}

!darwin:!unix:!win32: SOURCES += kernel/qelapsedtimer_generic.cpp

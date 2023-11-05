#always install the library
win32 {
   dlltarget.path=$$[QT_INSTALL_BINS]
   INSTALLS += dlltarget
}
target.path=$$[QT_INSTALL_LIBS]
INSTALLS += target

#headers
qt_install_headers {
    class_headers.files = $$SYNCQT.HEADER_CLASSES
    class_headers.path = $$[QT_INSTALL_HEADERS]/$$TARGET
    INSTALLS += class_headers

    targ_headers.files = $$SYNCQT.HEADER_FILES
    targ_headers.path = $$[QT_INSTALL_HEADERS]/$$TARGET
    INSTALLS += targ_headers

    contains(QT_CONFIG,private_tests) {
        private_headers.files = $$SYNCQT.PRIVATE_HEADER_FILES
        private_headers.path = $$[QT_INSTALL_HEADERS]/$$TARGET/private
        INSTALLS += private_headers
    }
}

embedded|qpa: equals(TARGET, QtGui) {
    # install fonts for embedded
    INSTALLS += fonts
    fonts.path = $$[QT_INSTALL_LIBS]/fonts
    fonts.files = $$QT_SOURCE_TREE/lib/fonts/*
}

QT          += svg

HEADERS     = mimedata.h \
              sourcewidget.h
RESOURCES   = delayedencoding.qrc
SOURCES     = main.cpp \
              mimedata.cpp \
              sourcewidget.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/draganddrop/delayedencoding
sources.files = $$SOURCES $$HEADERS *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/itemviews/delayedencoding
INSTALLS += target sources

simulator: warning(This example does not work on Simulator platform)

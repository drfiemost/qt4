HEADERS += \
        mouse.h \
        graphicsview.h
SOURCES += \
	main.cpp \
        mouse.cpp \
        graphicsview.cpp

RESOURCES += \
	mice.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/touch/pinchzoom
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS pinchzoom.pro images
sources.path = $$[QT_INSTALL_EXAMPLES]/touch/pinchzoom
INSTALLS += target sources


simulator: warning(This example might not fully work on Simulator platform)

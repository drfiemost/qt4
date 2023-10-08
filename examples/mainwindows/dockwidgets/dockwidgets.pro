HEADERS         = mainwindow.h
SOURCES         = main.cpp \
                  mainwindow.cpp
RESOURCES       = dockwidgets.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/mainwindows/dockwidgets
sources.files = $$SOURCES $$HEADERS $$RESOURCES dockwidgets.pro images
sources.path = $$[QT_INSTALL_EXAMPLES]/mainwindows/dockwidgets
INSTALLS += target sources


simulator: warning(This example might not fully work on Simulator platform)

HEADERS     = dialog.h
SOURCES     = dialog.cpp \
              main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/layouts/dynamiclayouts
sources.files = $$SOURCES $$HEADERS *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/layouts/dynamiclayouts
INSTALLS += target sources


simulator: warning(This example might not fully work on Simulator platform)

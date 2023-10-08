HEADERS     = window.h
SOURCES     = main.cpp \
              window.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/lineedits
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS lineedits.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/widgets/lineedits
INSTALLS += target sources

simulator: warning(This example might not fully work on Simulator platform)

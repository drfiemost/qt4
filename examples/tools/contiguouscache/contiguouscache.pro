HEADERS       = randomlistmodel.h
SOURCES       = randomlistmodel.cpp \
                main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tools/contiguouscache
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS contiguouscache.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tools/contiguouscache
INSTALLS += target sources


simulator: warning(This example might not fully work on Simulator platform)

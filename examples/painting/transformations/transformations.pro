HEADERS     = renderarea.h \
              window.h
SOURCES     = main.cpp \
              renderarea.cpp \
	      window.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/painting/transformations
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS transformations.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/painting/transformations
INSTALLS += target sources

    TARGET.UID3 = 0xA000A64D
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
}

simulator: warning(This example might not fully work on Simulator platform)

SOURCES = main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tutorials/widgets/nestedlayouts
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS nestedlayouts.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tutorials/widgets/nestedlayouts
INSTALLS += target sources


simulator: warning(This example might not fully work on Simulator platform)

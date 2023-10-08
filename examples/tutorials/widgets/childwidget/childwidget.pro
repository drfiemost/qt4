SOURCES = main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tutorials/widgets/childwidget
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS childwidget.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tutorials/widgets/childwidget
INSTALLS += target sources


simulator: warning(This example might not fully work on Simulator platform)

SOURCES = testqstring.cpp
CONFIG  += qtestlib

# install
target.path = $$[QT_INSTALL_EXAMPLES]/qtestlib/tutorial2
sources.files = $$SOURCES *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/qtestlib/tutorial2
INSTALLS += target sources

simulator: warning(This example might not fully work on Simulator platform)

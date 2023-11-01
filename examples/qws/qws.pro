TEMPLATE      = subdirs
# no /dev/fbX
SUBDIRS = framebuffer
SUBDIRS      += mousecalibration simpledecoration

# install
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS README *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/qws
INSTALLS += sources

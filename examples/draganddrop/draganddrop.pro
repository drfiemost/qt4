TEMPLATE    = subdirs
SUBDIRS     = draggableicons \
              draggabletext \
              dropsite \
              fridgemagnets \
              puzzle

contains(QT_CONFIG, svg): SUBDIRS += delayedencoding

# install
sources.files = README *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/draganddrop
INSTALLS += sources

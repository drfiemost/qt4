TEMPLATE  = subdirs
SUBDIRS   = styledemo raycasting flickable digiflip

contains(QT_CONFIG, svg) {
    SUBDIRS += embeddedsvgviewer \
               desktopservices
    fluidlauncher.subdir = fluidlauncher
    fluidlauncher.depends = styledemo desktopservices raycasting flickable digiflip lightmaps flightinfo
    SUBDIRS += fluidlauncher
}

SUBDIRS += lightmaps
SUBDIRS += flightinfo
contains(QT_CONFIG, svg) {
    SUBDIRS += weatherinfo
}

contains(QT_CONFIG, webkit) {
    SUBDIRS += anomaly
}

# install
sources.files = README *.pro
sources.path = $$[QT_INSTALL_DEMOS]/embedded
INSTALLS += sources

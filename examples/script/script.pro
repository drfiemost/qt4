TEMPLATE = subdirs
SUBDIRS = customclass
!contains(QT_CONFIG, no-gui):SUBDIRS += helloscript context2d defaultprototypes

!contains(QT_CONFIG, no-gui):SUBDIRS += qscript marshal
SUBDIRS *= marshal
!cross_compile:!contains(QT_CONFIG, no-gui):SUBDIRS += calculator qstetrix

# install
target.path = $$[QT_INSTALL_EXAMPLES]/script
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS script.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/script
INSTALLS += target sources


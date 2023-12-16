TEMPLATE      = subdirs
# no QSharedMemory
SUBDIRS = sharedmemory
SUBDIRS += localfortuneserver localfortuneclient

# install
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS ipc.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/ipc
INSTALLS += sources


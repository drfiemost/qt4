HEADERS += addtorrentdialog.h \
           bencodeparser.h \
           connectionmanager.h \
           mainwindow.h \
           metainfo.h \
           peerwireclient.h \
           ratecontroller.h \
           filemanager.h \
           torrentclient.h \
           torrentserver.h \
           trackerclient.h

SOURCES += main.cpp \
           addtorrentdialog.cpp \
           bencodeparser.cpp \
           connectionmanager.cpp \
           mainwindow.cpp \
           metainfo.cpp \
           peerwireclient.cpp \
           ratecontroller.cpp \
           filemanager.cpp \
           torrentclient.cpp \
           torrentserver.cpp \
           trackerclient.cpp

# Forms and resources
FORMS += forms/addtorrentform.ui
RESOURCES += icons.qrc

QT += network

# install
target.path = $$[QT_INSTALL_EXAMPLES]/network/torrent
sources.files = $$SOURCES $$HEADERS $$RESOURCES torrent.pro *.torrent
sources.files += icons forms 3rdparty
sources.path = $$[QT_INSTALL_EXAMPLES]/network/torrent
INSTALLS += target sources


simulator: warning(This example might not fully work on Simulator platform)

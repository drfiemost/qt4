TEMPLATE = app
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += qdbus

# Input
HEADERS += chat.h chat_adaptor.h chat_interface.h
SOURCES += chat.cpp chat_adaptor.cpp chat_interface.cpp
FORMS += chatmainwindow.ui chatsetnickname.ui

#DBUS_ADAPTORS += com.trolltech.chat.xml
#DBUS_INTERFACES += com.trolltech.chat.xml

# install
target.path = $$[QT_INSTALL_EXAMPLES]/dbus/chat
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro *.xml
sources.path = $$[QT_INSTALL_EXAMPLES]/dbus/chat
INSTALLS += target sources

simulator: warning(This example does not work on Simulator platform)

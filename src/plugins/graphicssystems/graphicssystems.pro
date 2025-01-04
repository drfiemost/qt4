TEMPLATE = subdirs
SUBDIRS += trace
!wince*:contains(QT_CONFIG, opengl):SUBDIRS += opengl

!win32:!embedded:!mac:CONFIG += x11

x11:contains(QT_CONFIG, opengles2):contains(QT_CONFIG, egl):SUBDIRS += meego

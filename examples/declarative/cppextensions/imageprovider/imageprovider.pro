TEMPLATE = lib
CONFIG += qt plugin
QT += declarative

DESTDIR = ImageProviderCore
TARGET  = qmlimageproviderplugin

SOURCES += imageprovider.cpp

sources.files = $$SOURCES imageprovider.qml imageprovider.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/declarative/cppextensions/imageprovider

target.path = $$[QT_INSTALL_EXAMPLES]/declarative/cppextensions/imageprovider/ImageProviderCore

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

ImageProviderCore_sources.files = \
    ImageProviderCore/qmldir 
ImageProviderCore_sources.path = $$[QT_INSTALL_EXAMPLES]/declarative/cppextensions/imageprovider/ImageProviderCore

INSTALLS = sources ImageProviderCore_sources target

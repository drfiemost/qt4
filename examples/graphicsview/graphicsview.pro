TEMPLATE      = subdirs
SUBDIRS       = \
              elasticnodes \
              collidingmice \
              padnavigator \
	      basicgraphicslayouts

SUBDIRS += \
              diagramscene \
              dragdroprobot \
              flowlayout \
              anchorlayout \
              simpleanchorlayout \
              weatheranchorlayout

contains(DEFINES, QT_NO_CURSOR)|contains(DEFINES, QT_NO_DRAGANDDROP): SUBDIRS -= dragdroprobot

# install
target.path = $$[QT_INSTALL_EXAMPLES]/graphicsview
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS graphicsview.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/graphicsview
INSTALLS += target sources


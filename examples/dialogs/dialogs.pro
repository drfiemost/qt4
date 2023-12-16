TEMPLATE      = subdirs
SUBDIRS       = classwizard \
                configdialog \
                standarddialogs \
                tabdialog \
                trivialwizard

SUBDIRS += licensewizard \
           extension \
           findfiles

SUBDIRS += sipdialog

# install
sources.files = README *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/dialogs
INSTALLS += sources


TEMPLATE      = subdirs

SUBDIRS       =   drilldown
SUBDIRS   +=  cachedtable \
                        relationaltablemodel \
                        sqlwidgetmapper

SUBDIRS   +=  masterdetail

SUBDIRS += \
                        querymodel \
                        tablemodel


# install
sources.files = connection.h sql.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/sql
INSTALLS += sources


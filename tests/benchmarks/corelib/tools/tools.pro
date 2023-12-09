TEMPLATE = subdirs
SUBDIRS = \
        containers-associative \
        containers-sequential \
        qbytearray \
        qcontiguouscache \
        qline \
        qlist \
        qlocale \
        qmap \
        qrect \
        #qregexp \     # FIXME: broken
        qstring \
        qstringbuilder \
        qstringlist \
        qvector

!*g++*: SUBDIRS -= qstring

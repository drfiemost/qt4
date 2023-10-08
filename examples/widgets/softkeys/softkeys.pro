HEADERS =   softkeys.h
SOURCES += \
            main.cpp \
            softkeys.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/softkeys
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS softkeys.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/widgets/softkeys
INSTALLS += target sources

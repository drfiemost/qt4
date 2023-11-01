load(qttest_p4)

INCLUDEPATH += ../

HEADERS +=  
SOURCES += tst_qtextedit.cpp 

wince*: {
    addImages.files = fullWidthSelection/*
    addImages.path = fullWidthSelection
    DEPLOYMENT += addImages
}

wince* {
    DEFINES += SRCDIR=\\\"./\\\"
} else {
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
}

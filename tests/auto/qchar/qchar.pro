load(qttest_p4)
SOURCES  += tst_qchar.cpp

QT = core

wince*: {
deploy.files += NormalizationTest.txt
DEPLOYMENT += deploy
}


wince*: {
    DEFINES += SRCDIR=\\\"\\\"
} else {
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
}
CONFIG += parallel_test

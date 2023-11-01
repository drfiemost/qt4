CONFIG += testcase

TARGET = tst_qsql
QT += sql testlib
SOURCES  += tst_qsql.cpp

wince*: {
   DEPLOYMENT_PLUGIN += qsqlite
}

CONFIG += testcase

TARGET = ../tst_qlocale
QT = core testlib core-private
embedded: QT += gui
SOURCES += ../tst_qlocale.cpp

!wince*: {
  win32: {
    CONFIG(debug, debug|release) {
      TARGET = ../../debug/tst_qlocale
    } else {
      TARGET = ../../release/tst_qlocale
    }
  }
} else {
   TARGET = tst_qlocale
}

wince*: {
   addFiles.files = \
        ../syslocaleapp

   addFiles.path = "\\Program Files\\tst_qlocale"
   DEPLOYMENT += addFiles
}

symbian:contains(S60_VERSION,3.2) {
    # This test case compilation crashes on 3.2 for gcce if paging is on
    MMP_RULES -= PAGED
    custom_paged_rule = "$${LITERAL_HASH}ifndef GCCE"\
        "PAGED" \
        "$${LITERAL_HASH}endif"
    MMP_RULES += custom_paged_rule
}

symbian: INCLUDEPATH *= $$MW_LAYER_SYSTEMINCLUDE  # Needed for e32svr.h in S^3 envs

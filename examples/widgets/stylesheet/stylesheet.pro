HEADERS       = mainwindow.h \
                stylesheeteditor.h
FORMS         = mainwindow.ui \
                stylesheeteditor.ui
RESOURCES     = stylesheet.qrc
SOURCES       = main.cpp \
                mainwindow.cpp \
                stylesheeteditor.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/stylesheet
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro images layouts qss
sources.path = $$[QT_INSTALL_EXAMPLES]/widgets/stylesheet
INSTALLS += target sources


simulator: warning(This example might not fully work on Simulator platform)

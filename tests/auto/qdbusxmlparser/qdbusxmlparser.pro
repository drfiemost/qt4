load(qttest_p4)
QT = core xml
contains(QT_CONFIG,dbus): {
	SOURCES += tst_qdbusxmlparser.cpp
	QT += dbus
} else {
	SOURCES += ../qdbusmarshall/dummy.cpp
}



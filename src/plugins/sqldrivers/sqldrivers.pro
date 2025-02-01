TEMPLATE = subdirs

contains(sql-plugins, psql)	: SUBDIRS += psql
contains(sql-plugins, odbc)	: SUBDIRS += odbc
contains(sql-plugins, sqlite)	: SUBDIRS += sqlite
contains(sql-plugins, sqlite2)	: SUBDIRS += sqlite2
contains(sql-plugins, ibase)	: SUBDIRS += ibase

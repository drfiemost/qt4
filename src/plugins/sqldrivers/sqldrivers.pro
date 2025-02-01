TEMPLATE = subdirs

contains(sql-plugins, psql)	: SUBDIRS += psql
contains(sql-plugins, odbc)	: SUBDIRS += odbc
contains(sql-plugins, sqlite)	: SUBDIRS += sqlite
contains(sql-plugins, ibase)	: SUBDIRS += ibase

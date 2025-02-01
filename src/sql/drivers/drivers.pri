contains(sql-drivers, all):sql-driver += psql odbc sqlite ibase

contains(sql-drivers, psql):include($$PWD/psql/qsql_psql.pri)
contains(sql-drivers, odbc):include($$PWD/odbc/qsql_odbc.pri)
contains(sql-drivers, ibase):include($$PWD/ibase/qsql_ibase.pri)
contains(sql-drivers, sqlite):include($$PWD/sqlite/qsql_sqlite.pri)

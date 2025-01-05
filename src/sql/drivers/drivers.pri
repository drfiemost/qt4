contains(sql-drivers, all):sql-driver += psql mysql odbc sqlite ibase

contains(sql-drivers, psql):include($$PWD/psql/qsql_psql.pri)
contains(sql-drivers, mysql):include($$PWD/mysql/qsql_mysql.pri)
contains(sql-drivers, odbc):include($$PWD/odbc/qsql_odbc.pri)
contains(sql-drivers, ibase):include($$PWD/ibase/qsql_ibase.pri)
contains(sql-drivers, sqlite2):include($$PWD/sqlite2/qsql_sqlite2.pri)
contains(sql-drivers, sqlite):include($$PWD/sqlite/qsql_sqlite.pri)

/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <qtest.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <private/qdeclarativetext_p.h>
#include <private/qdeclarativeengine_p.h>
#include <QtCore/qcryptographichash.h>
#include <QtSql/qsqldatabase.h>
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>

class tst_qdeclarativesqldatabase : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativesqldatabase()
    {
        qApp->setApplicationName("tst_qdeclarativesqldatabase");
        qApp->setOrganizationName("The Qt Company Ltd");
        qApp->setOrganizationDomain("qt.io");
        engine = new QDeclarativeEngine;
    }

    ~tst_qdeclarativesqldatabase()
    {
        delete engine;
    }

private slots:
    void initTestCase();

    void checkDatabasePath();

    void testQml_data();
    void testQml();
    void testQml_cleanopen_data();
    void testQml_cleanopen();
    void totalDatabases();

    void cleanupTestCase();

private:
    QString dbDir() const;
    QDeclarativeEngine *engine;
};

void removeRecursive(const QString& dirname)
{
    QDir dir(dirname);
    QFileInfoList entries(dir.entryInfoList(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot));
    for (int i = 0; i < entries.count(); ++i)
        if (entries[i].isDir())
            removeRecursive(entries[i].filePath());
        else
            dir.remove(entries[i].fileName());
    QDir().rmdir(dirname);
}

void tst_qdeclarativesqldatabase::initTestCase()
{
    removeRecursive(dbDir());
    QDir().mkpath(dbDir());
}

void tst_qdeclarativesqldatabase::cleanupTestCase()
{
    removeRecursive(dbDir());
}

QString tst_qdeclarativesqldatabase::dbDir() const
{
    static QString tmpd = QDir::tempPath()+"/tst_qdeclarativesqldatabase_output-"
        + QDateTime::currentDateTime().toString(QLatin1String("yyyyMMddhhmmss"));
    return tmpd;
}

void tst_qdeclarativesqldatabase::checkDatabasePath()
{
    // Check default storage path (we can't use it since we don't want to mess with user's data)
    QVERIFY(engine->offlineStoragePath().contains("tst_qdeclarativesqldatabase"));
    QVERIFY(engine->offlineStoragePath().contains("OfflineStorage"));
}

static const int total_databases_created_by_tests = 12;
void tst_qdeclarativesqldatabase::testQml_data()
{
    QTest::addColumn<QString>("jsfile"); // The input file

    // Each test should use a newly named DB to avoid inter-test dependencies
    QTest::newRow("creation") << "data/creation.js";
    QTest::newRow("creation-a") << "data/creation-a.js";
    QTest::newRow("creation") << "data/creation.js";
    QTest::newRow("error-creation") << "data/error-creation.js"; // re-uses above DB
    QTest::newRow("changeversion") << "data/changeversion.js";
    QTest::newRow("readonly") << "data/readonly.js";
    QTest::newRow("readonly-error") << "data/readonly-error.js";
    QTest::newRow("selection") << "data/selection.js";
    QTest::newRow("selection-bindnames") << "data/selection-bindnames.js";
    QTest::newRow("iteration") << "data/iteration.js";
    QTest::newRow("iteration-forwardonly") << "data/iteration-forwardonly.js";
    QTest::newRow("error-a") << "data/error-a.js";
    QTest::newRow("error-notransaction") << "data/error-notransaction.js";
    QTest::newRow("error-outsidetransaction") << "data/error-outsidetransaction.js"; // reuse above
    QTest::newRow("reopen1") << "data/reopen1.js";
    QTest::newRow("reopen2") << "data/reopen2.js"; // re-uses above DB

    // If you add a test, you should usually use a new database in the
    // test - in which case increment total_databases_created_by_tests above.
}

void tst_qdeclarativesqldatabase::testQml()
{
    // Tests QML SQL Database support with tests
    // that have been validated against Webkit.
    //
    QFETCH(QString, jsfile);

    QString qml=
        "import QtQuick 1.0\n"
        "import \""+jsfile+"\" as JS\n"
        "Text { text: JS.test() }";

    engine->setOfflineStoragePath(dbDir());
    QDeclarativeComponent component(engine);
    component.setData(qml.toUtf8(), QUrl::fromLocalFile(SRCDIR "/empty.qml")); // just a file for relative local imports
    QVERIFY(!component.isError());
    QDeclarativeText *text = qobject_cast<QDeclarativeText*>(component.create());
    QVERIFY(text != 0);
    QCOMPARE(text->text(),QString("passed"));
}

void tst_qdeclarativesqldatabase::testQml_cleanopen_data()
{
    QTest::addColumn<QString>("jsfile"); // The input file
    QTest::newRow("reopen1") << "data/reopen1.js";
    QTest::newRow("reopen2") << "data/reopen2.js";
    QTest::newRow("error-creation") << "data/error-creation.js"; // re-uses creation DB
}

void tst_qdeclarativesqldatabase::testQml_cleanopen()
{
    // Same as testQml, but clean connections between tests,
    // making it more like the tests are running in new processes.
    testQml();

    QDeclarativeEnginePrivate::getScriptEngine(engine)->collectGarbage(); // close databases
    foreach (QString dbname, QSqlDatabase::connectionNames()) {
        QSqlDatabase::removeDatabase(dbname);
    }
}

void tst_qdeclarativesqldatabase::totalDatabases()
{
    QCOMPARE(QDir(dbDir()+"/Databases").entryInfoList(QDir::Files|QDir::NoDotAndDotDot).count(), total_databases_created_by_tests*2);
}

QTEST_MAIN(tst_qdeclarativesqldatabase)

#include "tst_qdeclarativesqldatabase.moc"

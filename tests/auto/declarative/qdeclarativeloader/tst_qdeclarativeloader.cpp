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
#include <QtGui/QGraphicsWidget>
#include <QtGui/QGraphicsScene>

#include <QSignalSpy>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <private/qdeclarativeloader_p.h>
#include "testhttpserver.h"

#define SERVER_PORT 14450

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

inline QUrl TEST_FILE(const QString &filename)
{
    return QUrl::fromLocalFile(QLatin1String(SRCDIR) + QLatin1String("/data/") + filename);
}

class tst_QDeclarativeLoader : public QObject

{
    Q_OBJECT
public:
    tst_QDeclarativeLoader();

private slots:
    void sourceOrComponent();
    void sourceOrComponent_data();
    void clear();
    void urlToComponent();
    void componentToUrl();
    void anchoredLoader();
    void sizeLoaderToItem();
    void sizeItemToLoader();
    void noResize();
    void sizeLoaderToGraphicsWidget();
    void sizeGraphicsWidgetToLoader();
    void noResizeGraphicsWidget();
    void networkRequestUrl();
    void failNetworkRequest();
//    void networkComponent();

    void deleteComponentCrash();
    void nonItem();
    void vmeErrors();
    void creationContext();
    void QTBUG_16928();
    void implicitSize();
    void QTBUG_17114();

private:
    QDeclarativeEngine engine;
};


tst_QDeclarativeLoader::tst_QDeclarativeLoader()
{
}

void tst_QDeclarativeLoader::sourceOrComponent()
{
    QFETCH(QString, sourceDefinition);
    QFETCH(QUrl, sourceUrl);
    QFETCH(QString, errorString);

    bool error = !errorString.isEmpty();
    if (error)
        QTest::ignoreMessage(QtWarningMsg, errorString.toUtf8().constData());

    QDeclarativeComponent component(&engine);
    component.setData(QByteArray(
            "import QtQuick 1.0\n"
            "Loader {\n"
            "   property int onItemChangedCount: 0\n"
            "   property int onSourceChangedCount: 0\n"
            "   property int onStatusChangedCount: 0\n"
            "   property int onProgressChangedCount: 0\n"
            "   property int onLoadedCount: 0\n")
            + sourceDefinition.toUtf8()
            + QByteArray(
            "   onItemChanged: onItemChangedCount += 1\n"
            "   onSourceChanged: onSourceChangedCount += 1\n"
            "   onStatusChanged: onStatusChangedCount += 1\n"
            "   onProgressChanged: onProgressChangedCount += 1\n"
            "   onLoaded: onLoadedCount += 1\n"
            "}")
        , TEST_FILE(""));

    QDeclarativeLoader *loader = qobject_cast<QDeclarativeLoader*>(component.create());
    QVERIFY(loader != 0);
    QCOMPARE(loader->item() == 0, error);
    QCOMPARE(loader->source(), sourceUrl);
    QCOMPARE(loader->progress(), 1.0);

    QCOMPARE(loader->status(), error ? QDeclarativeLoader::Error : QDeclarativeLoader::Ready);
    QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), error ? 0: 1);

    if (!error) {
        QDeclarativeComponent *c = qobject_cast<QDeclarativeComponent*>(loader->QGraphicsObject::children().at(0));
        QVERIFY(c);
        QCOMPARE(loader->sourceComponent(), c);
    }

    QCOMPARE(loader->property("onSourceChangedCount").toInt(), 1);
    QCOMPARE(loader->property("onStatusChangedCount").toInt(), 1);
    QCOMPARE(loader->property("onProgressChangedCount").toInt(), 1);

    QCOMPARE(loader->property("onItemChangedCount").toInt(), error ? 0 : 1);
    QCOMPARE(loader->property("onLoadedCount").toInt(), error ? 0 : 1);

    delete loader;
}

void tst_QDeclarativeLoader::sourceOrComponent_data()
{
    QTest::addColumn<QString>("sourceDefinition");
    QTest::addColumn<QUrl>("sourceUrl");
    QTest::addColumn<QString>("errorString");

    QTest::newRow("source") << "source: 'Rect120x60.qml'\n" << QUrl::fromLocalFile(SRCDIR "/data/Rect120x60.qml") << "";
    QTest::newRow("sourceComponent") << "Component { id: comp; Rectangle { width: 100; height: 50 } }\n sourceComponent: comp\n" << QUrl() << "";

    QTest::newRow("invalid source") << "source: 'IDontExist.qml'\n" << QUrl::fromLocalFile(SRCDIR "/data/IDontExist.qml")
            << QString(QUrl::fromLocalFile(SRCDIR "/data/IDontExist.qml").toString() + ": File not found");
}

void tst_QDeclarativeLoader::clear()
{
    {
        QDeclarativeComponent component(&engine);
        component.setData(QByteArray(
                    "import QtQuick 1.0\n"
                    " Loader { id: loader\n"
                    "  source: 'Rect120x60.qml'\n"
                    "  Timer { interval: 200; running: true; onTriggered: loader.source = '' }\n"
                    " }")
                , TEST_FILE(""));
        QDeclarativeLoader *loader = qobject_cast<QDeclarativeLoader*>(component.create());
        QVERIFY(loader != 0);
        QVERIFY(loader->item());
        QCOMPARE(loader->progress(), 1.0);
        QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 1);

        QTRY_VERIFY(loader->item() == 0);
        QCOMPARE(loader->progress(), 0.0);
        QCOMPARE(loader->status(), QDeclarativeLoader::Null);
        QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 0);

        delete loader;
    }
    {
        QDeclarativeComponent component(&engine, TEST_FILE("/SetSourceComponent.qml"));
        QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(component.create());
        QVERIFY(item);

        QDeclarativeLoader *loader = qobject_cast<QDeclarativeLoader*>(item->QGraphicsObject::children().at(1)); 
        QVERIFY(loader);
        QVERIFY(loader->item());
        QCOMPARE(loader->progress(), 1.0);
        QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 1);

        loader->setSourceComponent(0);

        QVERIFY(loader->item() == 0);
        QCOMPARE(loader->progress(), 0.0);
        QCOMPARE(loader->status(), QDeclarativeLoader::Null);
        QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 0);

        delete item;
    }
    {
        QDeclarativeComponent component(&engine, TEST_FILE("/SetSourceComponent.qml"));
        QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(component.create());
        QVERIFY(item);

        QDeclarativeLoader *loader = qobject_cast<QDeclarativeLoader*>(item->QGraphicsObject::children().at(1)); 
        QVERIFY(loader);
        QVERIFY(loader->item());
        QCOMPARE(loader->progress(), 1.0);
        QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 1);

        QMetaObject::invokeMethod(item, "clear");

        QVERIFY(loader->item() == 0);
        QCOMPARE(loader->progress(), 0.0);
        QCOMPARE(loader->status(), QDeclarativeLoader::Null);
        QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 0);

        delete item;
    }
}

void tst_QDeclarativeLoader::urlToComponent()
{
    QDeclarativeComponent component(&engine);
    component.setData(QByteArray("import QtQuick 1.0\n"
                "Loader {\n"
                " id: loader\n"
                " Component { id: myComp; Rectangle { width: 10; height: 10 } }\n"
                " source: \"Rect120x60.qml\"\n"
                " Timer { interval: 100; running: true; onTriggered: loader.sourceComponent = myComp }\n"
                "}" )
            , TEST_FILE(""));
    QDeclarativeLoader *loader = qobject_cast<QDeclarativeLoader*>(component.create());
    QTest::qWait(200);
    QTRY_VERIFY(loader != 0);
    QVERIFY(loader->item());
    QCOMPARE(loader->progress(), 1.0);
    QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 1);
    QCOMPARE(loader->width(), 10.0);
    QCOMPARE(loader->height(), 10.0);

    delete loader;
}

void tst_QDeclarativeLoader::componentToUrl()
{
    QDeclarativeComponent component(&engine, TEST_FILE("/SetSourceComponent.qml"));
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item);

    QDeclarativeLoader *loader = qobject_cast<QDeclarativeLoader*>(item->QGraphicsObject::children().at(1)); 
    QVERIFY(loader);
    QVERIFY(loader->item());
    QCOMPARE(loader->progress(), 1.0);
    QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 1);

    loader->setSource(TEST_FILE("/Rect120x60.qml"));
    QVERIFY(loader->item());
    QCOMPARE(loader->progress(), 1.0);
    QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 1);
    QCOMPARE(loader->width(), 120.0);
    QCOMPARE(loader->height(), 60.0);

    delete item;
}

void tst_QDeclarativeLoader::anchoredLoader()
{
    QDeclarativeComponent component(&engine, TEST_FILE("/AnchoredLoader.qml"));
    QDeclarativeItem *rootItem = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(rootItem != 0);
    QDeclarativeItem *loader = rootItem->findChild<QDeclarativeItem*>("loader");
    QDeclarativeItem *sourceElement = rootItem->findChild<QDeclarativeItem*>("sourceElement");

    QVERIFY(loader != 0);
    QVERIFY(sourceElement != 0);

    QCOMPARE(rootItem->width(), 300.0);
    QCOMPARE(rootItem->height(), 200.0);

    QCOMPARE(loader->width(), 300.0);
    QCOMPARE(loader->height(), 200.0);

    QCOMPARE(sourceElement->width(), 300.0);
    QCOMPARE(sourceElement->height(), 200.0);
}

void tst_QDeclarativeLoader::sizeLoaderToItem()
{
    QDeclarativeComponent component(&engine, TEST_FILE("/SizeToItem.qml"));
    QDeclarativeLoader *loader = qobject_cast<QDeclarativeLoader*>(component.create());
    QVERIFY(loader != 0);
    QCOMPARE(loader->width(), 120.0);
    QCOMPARE(loader->height(), 60.0);

    // Check resize
    QDeclarativeItem *rect = qobject_cast<QDeclarativeItem*>(loader->item());
    QVERIFY(rect);
    rect->setWidth(150);
    rect->setHeight(45);
    QCOMPARE(loader->width(), 150.0);
    QCOMPARE(loader->height(), 45.0);

    // Check explicit width
    loader->setWidth(200.0);
    QCOMPARE(loader->width(), 200.0);
    QCOMPARE(rect->width(), 200.0);
    rect->setWidth(100.0); // when rect changes ...
    QCOMPARE(rect->width(), 100.0); // ... it changes
    QCOMPARE(loader->width(), 200.0); // ... but loader stays the same

    // Check explicit height
    loader->setHeight(200.0);
    QCOMPARE(loader->height(), 200.0);
    QCOMPARE(rect->height(), 200.0);
    rect->setHeight(100.0); // when rect changes ...
    QCOMPARE(rect->height(), 100.0); // ... it changes
    QCOMPARE(loader->height(), 200.0); // ... but loader stays the same

    // Switch mode
    loader->setWidth(180);
    loader->setHeight(30);
    QCOMPARE(rect->width(), 180.0);
    QCOMPARE(rect->height(), 30.0);

    delete loader;
}

void tst_QDeclarativeLoader::sizeItemToLoader()
{
    QDeclarativeComponent component(&engine, TEST_FILE("/SizeToLoader.qml"));
    QDeclarativeLoader *loader = qobject_cast<QDeclarativeLoader*>(component.create());
    QVERIFY(loader != 0);
    QCOMPARE(loader->width(), 200.0);
    QCOMPARE(loader->height(), 80.0);

    QDeclarativeItem *rect = qobject_cast<QDeclarativeItem*>(loader->item());
    QVERIFY(rect);
    QCOMPARE(rect->width(), 200.0);
    QCOMPARE(rect->height(), 80.0);

    // Check resize
    loader->setWidth(180);
    loader->setHeight(30);
    QCOMPARE(rect->width(), 180.0);
    QCOMPARE(rect->height(), 30.0);

    // Switch mode
    loader->resetWidth(); // reset explicit size
    loader->resetHeight();
    rect->setWidth(160);
    rect->setHeight(45);
    QCOMPARE(loader->width(), 160.0);
    QCOMPARE(loader->height(), 45.0);

    delete loader;
}

void tst_QDeclarativeLoader::noResize()
{
    QDeclarativeComponent component(&engine, TEST_FILE("/NoResize.qml"));
    QDeclarativeItem* item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item != 0);
    QCOMPARE(item->width(), 200.0);
    QCOMPARE(item->height(), 80.0);

    delete item;
}

void tst_QDeclarativeLoader::sizeLoaderToGraphicsWidget()
{
    QDeclarativeComponent component(&engine, TEST_FILE("/SizeLoaderToGraphicsWidget.qml"));
    QDeclarativeLoader *loader = qobject_cast<QDeclarativeLoader*>(component.create());
    QGraphicsScene scene;
    scene.addItem(loader);

    QVERIFY(loader != 0);
    QCOMPARE(loader->width(), 250.0);
    QCOMPARE(loader->height(), 250.0);

    // Check resize
    QGraphicsWidget *widget = qobject_cast<QGraphicsWidget*>(loader->item());
    QVERIFY(widget);
    widget->resize(QSizeF(150,45));
    QCOMPARE(loader->width(), 150.0);
    QCOMPARE(loader->height(), 45.0);

    // Switch mode
    loader->setWidth(180);
    loader->setHeight(30);
    QCOMPARE(widget->size().width(), 180.0);
    QCOMPARE(widget->size().height(), 30.0);

    delete loader;
}

void tst_QDeclarativeLoader::sizeGraphicsWidgetToLoader()
{
    QDeclarativeComponent component(&engine, TEST_FILE("/SizeGraphicsWidgetToLoader.qml"));
    QDeclarativeLoader *loader = qobject_cast<QDeclarativeLoader*>(component.create());
    QGraphicsScene scene;
    scene.addItem(loader);

    QVERIFY(loader != 0);
    QCOMPARE(loader->width(), 200.0);
    QCOMPARE(loader->height(), 80.0);

    QGraphicsWidget *widget = qobject_cast<QGraphicsWidget*>(loader->item());
    QVERIFY(widget);
    QCOMPARE(widget->size().width(), 200.0);
    QCOMPARE(widget->size().height(), 80.0);

    // Check resize
    loader->setWidth(180);
    loader->setHeight(30);
    QCOMPARE(widget->size().width(), 180.0);
    QCOMPARE(widget->size().height(), 30.0);

    // Switch mode
    loader->resetWidth(); // reset explicit size
    loader->resetHeight();
    widget->resize(QSizeF(160,45));
    QCOMPARE(loader->width(), 160.0);
    QCOMPARE(loader->height(), 45.0);

    delete loader;
}

void tst_QDeclarativeLoader::noResizeGraphicsWidget()
{
    QDeclarativeComponent component(&engine, TEST_FILE("/NoResizeGraphicsWidget.qml"));
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(component.create());
    QGraphicsScene scene;
    scene.addItem(item);

    QVERIFY(item != 0);
    QCOMPARE(item->width(), 200.0);
    QCOMPARE(item->height(), 80.0);

    delete item;
}

void tst_QDeclarativeLoader::networkRequestUrl()
{
    TestHTTPServer server(SERVER_PORT);
    QVERIFY(server.isValid());
    server.serveDirectory(SRCDIR "/data");

    QDeclarativeComponent component(&engine);
    component.setData(QByteArray("import QtQuick 1.0\nLoader { property int signalCount : 0; source: \"http://127.0.0.1:14450/Rect120x60.qml\"; onLoaded: signalCount += 1 }"), QUrl::fromLocalFile(SRCDIR "/dummy.qml"));
    if (component.isError())
        qDebug() << component.errors();
    QDeclarativeLoader *loader = qobject_cast<QDeclarativeLoader*>(component.create());
    QVERIFY(loader != 0);

    QTRY_VERIFY(loader->status() == QDeclarativeLoader::Ready);

    QVERIFY(loader->item());
    QCOMPARE(loader->progress(), 1.0);
    QCOMPARE(loader->property("signalCount").toInt(), 1);
    QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 1);

    delete loader;
}

/* XXX Component waits until all dependencies are loaded.  Is this actually possible?
void tst_QDeclarativeLoader::networkComponent()
{
    TestHTTPServer server(SERVER_PORT);
    QVERIFY(server.isValid());
    server.serveDirectory("slowdata", TestHTTPServer::Delay);

    QDeclarativeComponent component(&engine);
    component.setData(QByteArray(
                "import QtQuick 1.0\n"
                "import \"http://127.0.0.1:14450/\" as NW\n"
                "Item {\n"
                " Component { id: comp; NW.SlowRect {} }\n"
                " Loader { sourceComponent: comp } }")
            , TEST_FILE(""));

    QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item);

    QDeclarativeLoader *loader = qobject_cast<QDeclarativeLoader*>(item->QGraphicsObject::children().at(1)); 
    QVERIFY(loader);
    QTRY_VERIFY(loader->status() == QDeclarativeLoader::Ready);

    QVERIFY(loader->item());
    QCOMPARE(loader->progress(), 1.0);
    QCOMPARE(loader->status(), QDeclarativeLoader::Ready);
    QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 1);

    delete loader;
}
*/

void tst_QDeclarativeLoader::failNetworkRequest()
{
    TestHTTPServer server(SERVER_PORT);
    QVERIFY(server.isValid());
    server.serveDirectory(SRCDIR "/data");

    QTest::ignoreMessage(QtWarningMsg, "http://127.0.0.1:14450/IDontExist.qml: File not found");

    QDeclarativeComponent component(&engine);
    component.setData(QByteArray("import QtQuick 1.0\nLoader { property int did_load: 123; source: \"http://127.0.0.1:14450/IDontExist.qml\"; onLoaded: did_load=456 }"), QUrl::fromLocalFile("http://127.0.0.1:14450/dummy.qml"));
    QDeclarativeLoader *loader = qobject_cast<QDeclarativeLoader*>(component.create());
    QVERIFY(loader != 0);

    QTRY_VERIFY(loader->status() == QDeclarativeLoader::Error);

    QVERIFY(loader->item() == 0);
    QCOMPARE(loader->progress(), 0.0);
    QCOMPARE(loader->property("did_load").toInt(), 123);
    QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 0);

    delete loader;
}

// QTBUG-9241
void tst_QDeclarativeLoader::deleteComponentCrash()
{
    QDeclarativeComponent component(&engine, TEST_FILE("crash.qml"));
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item);

    item->metaObject()->invokeMethod(item, "setLoaderSource");

    QDeclarativeLoader *loader = qobject_cast<QDeclarativeLoader*>(item->QGraphicsObject::children().at(0));
    QVERIFY(loader);
    QVERIFY(loader->item());
    QCOMPARE(loader->item()->objectName(), QLatin1String("blue"));
    QCOMPARE(loader->progress(), 1.0);
    QCOMPARE(loader->status(), QDeclarativeLoader::Ready);
    QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 1);
    QVERIFY(loader->source() == QUrl::fromLocalFile(SRCDIR "/data/BlueRect.qml"));

    delete item;
}

void tst_QDeclarativeLoader::nonItem()
{
    QDeclarativeComponent component(&engine, TEST_FILE("nonItem.qml"));
    QString err = QUrl::fromLocalFile(SRCDIR).toString() + "/data/nonItem.qml:3:1: QML Loader: Loader does not support loading non-visual elements.";

    QTest::ignoreMessage(QtWarningMsg, err.toLatin1().constData());
    QDeclarativeLoader *loader = qobject_cast<QDeclarativeLoader*>(component.create());
    QVERIFY(loader);
    QVERIFY(loader->item() == 0);

    delete loader;
}

void tst_QDeclarativeLoader::vmeErrors()
{
    QDeclarativeComponent component(&engine, TEST_FILE("vmeErrors.qml"));
    QString err = QUrl::fromLocalFile(SRCDIR).toString() + "/data/VmeError.qml:6: Cannot assign object type QObject with no default method";
    QTest::ignoreMessage(QtWarningMsg, err.toLatin1().constData());
    QDeclarativeLoader *loader = qobject_cast<QDeclarativeLoader*>(component.create());
    QVERIFY(loader);
    QVERIFY(loader->item() == 0);

    delete loader;
}

// QTBUG-13481
void tst_QDeclarativeLoader::creationContext()
{
    QDeclarativeComponent component(&engine, TEST_FILE("creationContext.qml"));

    QObject *o = component.create();
    QVERIFY(o != 0);

    QCOMPARE(o->property("test").toBool(), true);

    delete o;
}

void tst_QDeclarativeLoader::QTBUG_16928()
{
    QDeclarativeComponent component(&engine, TEST_FILE("QTBUG_16928.qml"));
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item);

    QCOMPARE(item->width(), 250.);
    QCOMPARE(item->height(), 250.);

    delete item;
}

void tst_QDeclarativeLoader::implicitSize()
{
    QDeclarativeComponent component(&engine, TEST_FILE("implicitSize.qml"));
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item);

    QCOMPARE(item->width(), 150.);
    QCOMPARE(item->height(), 150.);

    QCOMPARE(item->property("implHeight").toReal(), 100.);
    QCOMPARE(item->property("implWidth").toReal(), 100.);

    delete item;
}

void tst_QDeclarativeLoader::QTBUG_17114()
{
    QDeclarativeComponent component(&engine, TEST_FILE("QTBUG_17114.qml"));
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item);

    QCOMPARE(item->property("loaderWidth").toReal(), 32.);
    QCOMPARE(item->property("loaderHeight").toReal(), 32.);

    delete item;
}

QTEST_MAIN(tst_QDeclarativeLoader)

#include "tst_qdeclarativeloader.moc"

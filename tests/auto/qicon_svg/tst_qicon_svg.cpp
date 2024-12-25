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


#include <QtTest/QtTest>

#include <qicon.h>

#include <qiconengine.h>

Q_DECLARE_METATYPE(QSize)

//TESTED_CLASS=
//TESTED_FILES=

class tst_QIcon_svg : public QObject
{
    Q_OBJECT
public:
    tst_QIcon_svg();

private slots:
    void init();
    void cleanup();

    void svgActualSize();
    void svg();
    void availableSizes();
    void isNull();

private:
    QString oldCurrentDir;

    const static QIcon staticIcon;
};

// Creating an icon statically should not cause a crash.
// But we do not officially support this. See QTBUG-8666
const QIcon tst_QIcon_svg::staticIcon = QIcon::fromTheme("edit-find");

void tst_QIcon_svg::init()
{
    QString srcdir(QLatin1String(SRCDIR));
    if (!srcdir.isEmpty()) {
        oldCurrentDir = QDir::current().absolutePath();
        QDir::setCurrent(srcdir);
    }
}

void tst_QIcon_svg::cleanup()
{
    if (!oldCurrentDir.isEmpty()) {
        QDir::setCurrent(oldCurrentDir);
    }
}

tst_QIcon_svg::tst_QIcon_svg()
{
}

void tst_QIcon_svg::svgActualSize()
{
    QIcon icon("rect.svg");
    QCOMPARE(icon.actualSize(QSize(16, 2)), QSize(16, 2));
    QCOMPARE(icon.pixmap(QSize(16, 16)).size(), QSize(16, 2));

    QPixmap p(16, 16);
    p.fill(Qt::cyan);
    icon.addPixmap(p);

    QCOMPARE(icon.actualSize(QSize(16, 16)), QSize(16, 16));
    QCOMPARE(icon.pixmap(QSize(16, 16)).size(), QSize(16, 16));

    QCOMPARE(icon.actualSize(QSize(16, 14)), QSize(16, 2));
    QCOMPARE(icon.pixmap(QSize(16, 14)).size(), QSize(16, 2));
}

void tst_QIcon_svg::svg()
{
    QIcon icon1("heart.svg");

    QVERIFY(!icon1.pixmap(32).isNull());
    QImage img1 = icon1.pixmap(32).toImage();
    QVERIFY(!icon1.pixmap(32, QIcon::Disabled).isNull());
    QImage img2 = icon1.pixmap(32, QIcon::Disabled).toImage();

    icon1.addFile("trash.svg", QSize(), QIcon::Disabled);
    QVERIFY(!icon1.pixmap(32, QIcon::Disabled).isNull());
    QImage img3 = icon1.pixmap(32, QIcon::Disabled).toImage();
    QVERIFY(img3 != img2);
    QVERIFY(img3 != img1);

    QPixmap pm("image.png");
    icon1.addPixmap(pm, QIcon::Normal, QIcon::On);
    QVERIFY(!icon1.pixmap(128, QIcon::Normal, QIcon::On).isNull());
    QImage img4 = icon1.pixmap(128, QIcon::Normal, QIcon::On).toImage();
    QVERIFY(img4 != img3);
    QVERIFY(img4 != img2);
    QVERIFY(img4 != img1);

    QIcon icon2;
    icon2.addFile("heart.svg");
    QVERIFY(icon2.pixmap(57).toImage() == icon1.pixmap(57).toImage());

    QIcon icon3("trash.svg");
    icon3.addFile("heart.svg");
    QVERIFY(icon3.pixmap(57).toImage() == icon1.pixmap(57).toImage());

    QIcon icon4("heart.svg");
    icon4.addFile("image.png", QSize(), QIcon::Active);
    QVERIFY(!icon4.pixmap(32).isNull());
    QVERIFY(!icon4.pixmap(32, QIcon::Active).isNull());
    QVERIFY(icon4.pixmap(32).toImage() == img1);
    QIcon pmIcon(pm);
    QVERIFY(icon4.pixmap(pm.size(), QIcon::Active).toImage() == pmIcon.pixmap(pm.size(), QIcon::Active).toImage());

#ifndef QT_NO_COMPRESS
    QIcon icon5("heart.svgz");
    QVERIFY(!icon5.pixmap(32).isNull());
#endif
}

void tst_QIcon_svg::availableSizes()
{
    {
        // checks that there are no availableSizes for scalable images.
        QIcon icon("heart.svg");
        QList<QSize> availableSizes = icon.availableSizes();
        qDebug() << availableSizes;
        QVERIFY(availableSizes.isEmpty());
    }

    {
        // even if an a scalable image contain added pixmaps,
        // availableSizes still should be empty.
        QIcon icon("heart.svg");
        icon.addFile("image.png", QSize(32,32));
        QList<QSize> availableSizes = icon.availableSizes();
        QVERIFY(availableSizes.isEmpty());
    }

}

void tst_QIcon_svg::isNull()
{
    {
        //checks that an invalid file results in the icon being null
        QIcon icon("nonExistentFile.svg");
        QVERIFY(icon.isNull());
    }
    {
        //valid svg, we're not null
        QIcon icon("heart.svg");
        QVERIFY(!icon.isNull());

        // check for non null of serialized/deserialized valid icon
        QByteArray buf;
        QDataStream out(&buf, QIODevice::WriteOnly);
        out << icon;

        QIcon icon2;
        QDataStream in(buf);
        in >> icon2;
        QVERIFY(!icon2.isNull());
    }
    {
        //invalid svg, but a pixmap added means we're not null
        QIcon icon("nonExistentFile.svg");
        icon.addFile("image.png", QSize(32,32));
        QVERIFY(!icon.isNull());
    }

}


QTEST_MAIN(tst_QIcon_svg)
#include "tst_qicon_svg.moc"

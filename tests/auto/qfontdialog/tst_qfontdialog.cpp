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


#include <qapplication.h>
#include <qfontinfo.h>
#include <qtimer.h>
#include <qmainwindow.h>
#include <qlistview.h>
#include "qfontdialog.h"
#include <private/qfontdialog_p.h>

//TESTED_CLASS=
//TESTED_FILES=

QT_FORWARD_DECLARE_CLASS(QtTestEventThread)

class tst_QFontDialog : public QObject
{
    Q_OBJECT

public:
    tst_QFontDialog();
    virtual ~tst_QFontDialog();


public slots:
    void postKeyReturn();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:
    void defaultOkButton();
    void setFont();
    void task256466_wrongStyle();
};

tst_QFontDialog::tst_QFontDialog()
{
}

tst_QFontDialog::~tst_QFontDialog()
{
}

void tst_QFontDialog::initTestCase()
{
}

void tst_QFontDialog::cleanupTestCase()
{
}

void tst_QFontDialog::init()
{
}

void tst_QFontDialog::cleanup()
{
}


void tst_QFontDialog::postKeyReturn() {
#ifndef Q_WS_MAC
    QWidgetList list = QApplication::topLevelWidgets();
    for (int i=0; i<list.count(); ++i) {
	QFontDialog *dialog = qobject_cast<QFontDialog*>(list[i]);
	if (dialog) {
	    QTest::keyClick( list[i], Qt::Key_Return, Qt::NoModifier );
	    return;
	}
    }
#else
    extern void click_cocoa_button();
    click_cocoa_button();
#endif
}

void tst_QFontDialog::defaultOkButton()
{
    bool ok = FALSE;
    QTimer::singleShot(2000, this, SLOT(postKeyReturn()));
    QFontDialog::getFont(&ok);
    QVERIFY(ok == TRUE);
}


void tst_QFontDialog::setFont()
{
    /* The font should be the same before as it is after if nothing changed
              while the font dialog was open.
	      Task #27662
    */
    bool ok = FALSE;
    QString fontName = "Arial";
    int fontSize = 24;
    QFont f1(fontName, fontSize);
    f1.setPixelSize(QFontInfo(f1).pixelSize());
    QTimer::singleShot(2000, this, SLOT(postKeyReturn()));
    QFont f2 = QFontDialog::getFont(&ok, f1);
    QCOMPARE(QFontInfo(f2).pointSize(), QFontInfo(f1).pointSize());
}


class FriendlyFontDialog : public QFontDialog
{
    friend class tst_QFontDialog;
    Q_DECLARE_PRIVATE(QFontDialog);
};

void tst_QFontDialog::task256466_wrongStyle()
{
    QFontDatabase fdb;
    FriendlyFontDialog dialog;
    QListView *familyList = reinterpret_cast<QListView*>(dialog.d_func()->familyList);
    QListView *styleList = reinterpret_cast<QListView*>(dialog.d_func()->styleList);
    QListView *sizeList = reinterpret_cast<QListView*>(dialog.d_func()->sizeList);
    for (int i = 0; i < familyList->model()->rowCount(); ++i) {
        QModelIndex currentFamily = familyList->model()->index(i, 0);
        familyList->setCurrentIndex(currentFamily);
        const QFont current = dialog.currentFont(),
                    expected = fdb.font(currentFamily.data().toString(),
            styleList->currentIndex().data().toString(), sizeList->currentIndex().data().toInt());
        QCOMPARE(current.family(), expected.family());
        QCOMPARE(current.style(), expected.style());
        QCOMPARE(current.pointSizeF(), expected.pointSizeF());
    }
}




QTEST_MAIN(tst_QFontDialog)
#include "tst_qfontdialog.moc"

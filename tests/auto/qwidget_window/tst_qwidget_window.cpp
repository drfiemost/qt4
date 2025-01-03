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
#include <QtGui/QtGui>
#include <qeventloop.h>
#include <qlist.h>

#include <qlistwidget.h>

#ifdef Q_WS_X11
#include <X11/Xlib.h>
#include <QX11Info>
#endif // Q_WS_X11



class tst_QWidget_window : public QWidget
{
    Q_OBJECT

public:
    tst_QWidget_window(){};

public slots:
    void initTestCase();
    void cleanupTestCase();

private slots:
    void tst_move_show();
    void tst_show_move();
    void tst_show_move_hide_show();

    void tst_resize_show();
    void tst_show_resize();
    void tst_show_resize_hide_show();

    void tst_windowFilePathAndwindowTitle_data();
    void tst_windowFilePathAndwindowTitle();
    void tst_windowFilePath_data();
    void tst_windowFilePath();

    void tst_showWithoutActivating();
    void tst_paintEventOnSecondShow();
};

void tst_QWidget_window::initTestCase()
{
}

void tst_QWidget_window::cleanupTestCase()
{
}

void tst_QWidget_window::tst_move_show()
{
    QWidget w;
    w.move(100, 100);
    w.show();
    QCOMPARE(w.pos(), QPoint(100, 100));
//    QCoreApplication::processEvents(QEventLoop::AllEvents, 3000);
}

void tst_QWidget_window::tst_show_move()
{
    QWidget w;
    w.show();
    w.move(100, 100);
    QCOMPARE(w.pos(), QPoint(100, 100));
//    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
}

void tst_QWidget_window::tst_show_move_hide_show()
{
    QWidget w;
    w.show();
    w.move(100, 100);
    w.hide();
    w.show();
    QCOMPARE(w.pos(), QPoint(100, 100));
//    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
}

void tst_QWidget_window::tst_resize_show()
{
    QWidget w;
    w.resize(200, 200);
    w.show();
    QCOMPARE(w.size(), QSize(200, 200));
//    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
}

void tst_QWidget_window::tst_show_resize()
{
    QWidget w;
    w.show();
    w.resize(200, 200);
    QCOMPARE(w.size(), QSize(200, 200));
//    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
}

void tst_QWidget_window::tst_show_resize_hide_show()
{
    QWidget w;
    w.show();
    w.resize(200, 200);
    w.hide();
    w.show();
    QCOMPARE(w.size(), QSize(200, 200));
//    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
}

class TestWidget : public QWidget
{
public:
    int m_first, m_next;
    bool paintEventReceived;

    void reset(){ m_first = m_next = 0; paintEventReceived = false; }
    bool event(QEvent *event)
    {
        switch (event->type()) {
        case QEvent::WindowActivate:
        case QEvent::WindowDeactivate:
        case QEvent::Hide:
        case QEvent::Show:
            if (m_first)
                m_next = event->type();
            else
                m_first = event->type();
            break;
        case QEvent::Paint:
            paintEventReceived = true;
            break;
        default:
            break;
        }
        return QWidget::event(event);
    }
};

void tst_QWidget_window::tst_windowFilePathAndwindowTitle_data()
{
    QTest::addColumn<bool>("setWindowTitleBefore");
    QTest::addColumn<bool>("setWindowTitleAfter");
    QTest::addColumn<QString>("filePath");
    QTest::addColumn<QString>("applicationName");
    QTest::addColumn<QString>("indyWindowTitle");
    QTest::addColumn<QString>("finalTitleBefore");
    QTest::addColumn<QString>("finalTitleAfter");

    QString validPath = QApplication::applicationFilePath();
    QString appName = QLatin1String("Killer App");
    QString fileNameOnly = QFileInfo(validPath).fileName() + QLatin1String("[*]");
    QString fileAndApp = fileNameOnly + QLatin1String(" ") + QChar(0x2014) + QLatin1String(" ") + appName;
    QString windowTitle = QLatin1String("Here is a Window Title");

    QTest::newRow("never Set Title nor AppName") << false << false << validPath << QString() << windowTitle << fileNameOnly << fileNameOnly;
    QTest::newRow("set title after only, but no AppName") << false << true << validPath << QString() << windowTitle << fileNameOnly << windowTitle;
    QTest::newRow("set title before only, not AppName") << true << false << validPath << QString() << windowTitle << windowTitle << windowTitle;
    QTest::newRow("always set title, not appName") << true << true << validPath << QString() << windowTitle << windowTitle << windowTitle;

    QString platString =
#ifdef Q_WS_MAC
        fileNameOnly;
#else
        fileAndApp;
#endif

    QTest::newRow("never Set Title, yes AppName") << false << false << validPath << appName << windowTitle << platString << platString;
    QTest::newRow("set title after only, yes AppName") << false << true << validPath << appName << windowTitle << platString << windowTitle;
    QTest::newRow("set title before only, yes AppName") << true << false << validPath << appName << windowTitle << windowTitle << windowTitle;
    QTest::newRow("always set title, yes appName") << true << true << validPath << appName << windowTitle << windowTitle << windowTitle;
}

void tst_QWidget_window::tst_windowFilePathAndwindowTitle()
{
    QFETCH(bool, setWindowTitleBefore);
    QFETCH(bool, setWindowTitleAfter);
    QFETCH(QString, filePath);
    QFETCH(QString, applicationName);
    QFETCH(QString, indyWindowTitle);
    QFETCH(QString, finalTitleBefore);
    QFETCH(QString, finalTitleAfter);


    QWidget widget;
    QCOMPARE(widget.windowFilePath(), QString());

    if (!applicationName.isEmpty())
        qApp->setApplicationName(applicationName);
    else
        qApp->setApplicationName(QString());

    if (setWindowTitleBefore) {
        widget.setWindowTitle(indyWindowTitle);
    }
    widget.setWindowFilePath(filePath);
    QCOMPARE(finalTitleBefore, widget.windowTitle());
    QCOMPARE(widget.windowFilePath(), filePath);

    if (setWindowTitleAfter) {
        widget.setWindowTitle(indyWindowTitle);
    }
    QCOMPARE(finalTitleAfter, widget.windowTitle());
    QCOMPARE(widget.windowFilePath(), filePath);
}

void tst_QWidget_window::tst_windowFilePath_data()
{
    QTest::addColumn<QString>("filePath");
    QTest::addColumn<QString>("result");
    QTest::addColumn<bool>("again");
    QTest::addColumn<QString>("filePath2");
    QTest::addColumn<QString>("result2");

    QString validPath = QApplication::applicationFilePath();
    QString invalidPath = QLatin1String("::**Never a Real Path**::");

    QTest::newRow("never Set Path") << QString() << QString() << false << QString() << QString();
    QTest::newRow("never EVER Set Path") << QString() << QString() << true << QString() << QString();
    QTest::newRow("Valid Path") << validPath << validPath << false << QString() << QString();
    QTest::newRow("invalid Path") << invalidPath << invalidPath << false << QString() << QString();
    QTest::newRow("Valid Path then empty") << validPath << validPath << true << QString() << QString();
    QTest::newRow("invalid Path then empty") << invalidPath << invalidPath << true << QString() << QString();
    QTest::newRow("invalid Path then valid") << invalidPath << invalidPath << true << validPath << validPath;
    QTest::newRow("valid Path then invalid") << validPath << validPath << true << invalidPath << invalidPath;
}

void tst_QWidget_window::tst_windowFilePath()
{
    QFETCH(QString, filePath);
    QFETCH(QString, result);
    QFETCH(bool, again);
    QFETCH(QString, filePath2);
    QFETCH(QString, result2);

    QWidget widget;
    QCOMPARE(widget.windowFilePath(), QString());
    widget.setWindowFilePath(filePath);
    QCOMPARE(widget.windowFilePath(), result);
    if (again) {
        widget.setWindowFilePath(filePath2);
        QCOMPARE(widget.windowFilePath(), result2);
    }
}

void tst_QWidget_window::tst_showWithoutActivating()
{
#ifndef Q_WS_X11
    QSKIP("This test is X11-only.", SkipAll);
#else
    QWidget w;
    w.show();
    QTest::qWaitForWindowShown(&w);
    QApplication::processEvents();

    QApplication::clipboard();
    QLineEdit *lineEdit = new QLineEdit;
    lineEdit->setAttribute(Qt::WA_ShowWithoutActivating, true);
    lineEdit->show();
    QTest::qWaitForWindowShown(lineEdit);
    lineEdit->setAttribute(Qt::WA_ShowWithoutActivating, false);
    lineEdit->raise();
    lineEdit->activateWindow();

    Window window;
    int revertto;
    QTRY_COMPARE(lineEdit->winId(),
                 (XGetInputFocus(QX11Info::display(), &window, &revertto), window) );
    // Note the use of the , before window because we want the XGetInputFocus to be re-executed
    //     in each iteration of the inside loop of the QTRY_COMPARE macro

#endif // Q_WS_X11
}

void tst_QWidget_window::tst_paintEventOnSecondShow()
{
    TestWidget w;
    w.show();
    w.hide();

    w.reset();
    w.show();
    QTest::qWaitForWindowShown(&w);
    QApplication::processEvents();
    QTRY_VERIFY(w.paintEventReceived);
}

QTEST_MAIN(tst_QWidget_window)
#include "tst_qwidget_window.moc"

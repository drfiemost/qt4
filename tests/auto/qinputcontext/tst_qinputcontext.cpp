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
#include "../../shared/util.h"

#include <qinputcontext.h>
#include <qlineedit.h>
#include <qplaintextedit.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qwindowsstyle.h>
#include <qdesktopwidget.h>
#include <qpushbutton.h>
#include <qgraphicsview.h>
#include <qgraphicsscene.h>

#ifdef QT_WEBKIT_LIB
#include <qwebview.h>
#include <qgraphicswebview.h>
#endif

#ifdef Q_OS_SYMBIAN
#include <private/qt_s60_p.h>
#include <private/qcoefepinputcontext_p.h>

#include <w32std.h>
#include <coecntrl.h>
#endif

class tst_QInputContext : public QObject
{
Q_OBJECT

public:
    tst_QInputContext() : m_phoneIsQwerty(false) {}
    virtual ~tst_QInputContext() {}

    template <class WidgetType> void symbianTestCoeFepInputContext_addData();

public slots:
    void initTestCase();
    void cleanupTestCase() {}
    void init() {}
    void cleanup() {}
private slots:
    void maximumTextLength();
    void filterMouseEvents();
    void requestSoftwareInputPanel();
    void closeSoftwareInputPanel();
    void selections();
    void focusProxy();
    void contextInheritance();

private:
    bool m_phoneIsQwerty;
};

void tst_QInputContext::initTestCase()
{
}

void tst_QInputContext::maximumTextLength()
{
    QLineEdit le;

    le.setMaxLength(15);
    QVariant variant = le.inputMethodQuery(Qt::ImMaximumTextLength);
    QVERIFY(variant.isValid());
    QCOMPARE(variant.toInt(), 15);

    QPlainTextEdit pte;
    // For BC/historical reasons, QPlainTextEdit::inputMethodQuery is protected.
    variant = static_cast<QWidget *>(&pte)->inputMethodQuery(Qt::ImMaximumTextLength);
    QVERIFY(!variant.isValid());
}

class QFilterInputContext : public QInputContext
{
public:
    QFilterInputContext() {}
    ~QFilterInputContext() {}

    QString identifierName() { return QString(); }
    QString language() { return QString(); }

    void reset() {}

    bool isComposing() const { return false; }

    bool filterEvent( const QEvent *event )
    {
        lastTypes.append(event->type());
        return false;
    }

public:
    QList<QEvent::Type> lastTypes;
};

void tst_QInputContext::filterMouseEvents()
{
    QLineEdit le;
    le.show();
    QApplication::setActiveWindow(&le);

    QFilterInputContext *ic = new QFilterInputContext;
    le.setInputContext(ic);
    QTest::mouseClick(&le, Qt::LeftButton);

    QVERIFY(ic->lastTypes.indexOf(QEvent::MouseButtonRelease) >= 0);

    le.setInputContext(0);
}

class RequestSoftwareInputPanelStyle : public QWindowsStyle
{
public:
    RequestSoftwareInputPanelStyle()
        : m_rsipBehavior(RSIP_OnMouseClickAndAlreadyFocused)
    {
#ifdef Q_OS_WINCE
        qApp->setAutoSipEnabled(true);
#endif
    }
    ~RequestSoftwareInputPanelStyle()
    {
    }

    int styleHint(StyleHint hint, const QStyleOption *opt = 0,
                  const QWidget *widget = 0, QStyleHintReturn* returnData = 0) const
    {
        if (hint == SH_RequestSoftwareInputPanel) {
            return m_rsipBehavior;
        } else {
            return QWindowsStyle::styleHint(hint, opt, widget, returnData);
        }
    }

    RequestSoftwareInputPanel m_rsipBehavior;
};

void tst_QInputContext::requestSoftwareInputPanel()
{
    QStyle *oldStyle = qApp->style();
    oldStyle->setParent(this); // Prevent it being deleted.
    RequestSoftwareInputPanelStyle *newStyle = new RequestSoftwareInputPanelStyle;
    qApp->setStyle(newStyle);

    QWidget w;
    QLayout *layout = new QVBoxLayout;
    QLineEdit *le1, *le2;
    le1 = new QLineEdit;
    le2 = new QLineEdit;
    layout->addWidget(le1);
    layout->addWidget(le2);
    w.setLayout(layout);

    QFilterInputContext *ic1, *ic2;
    ic1 = new QFilterInputContext;
    ic2 = new QFilterInputContext;
    le1->setInputContext(ic1);
    le2->setInputContext(ic2);

    w.show();
    QApplication::setActiveWindow(&w);

    // Testing single click panel activation.
    newStyle->m_rsipBehavior = QStyle::RSIP_OnMouseClick;
    QTest::mouseClick(le2, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QVERIFY(ic2->lastTypes.indexOf(QEvent::RequestSoftwareInputPanel) >= 0);
    ic2->lastTypes.clear();

    // Testing double click panel activation.
    newStyle->m_rsipBehavior = QStyle::RSIP_OnMouseClickAndAlreadyFocused;
    QTest::mouseClick(le1, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QVERIFY(ic1->lastTypes.indexOf(QEvent::RequestSoftwareInputPanel) < 0);
    QTest::mouseClick(le1, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QVERIFY(ic1->lastTypes.indexOf(QEvent::RequestSoftwareInputPanel) >= 0);
    ic1->lastTypes.clear();

    // Testing right mouse button
    QTest::mouseClick(le1, Qt::RightButton, Qt::NoModifier, QPoint(5, 5));
    QVERIFY(ic1->lastTypes.indexOf(QEvent::RequestSoftwareInputPanel) < 0);

    qApp->setStyle(oldStyle);
    oldStyle->setParent(qApp);
}

void tst_QInputContext::closeSoftwareInputPanel()
{
    QWidget w;
    QLayout *layout = new QVBoxLayout;
    QLineEdit *le1, *le2;
    QRadioButton *rb;
    le1 = new QLineEdit;
    le2 = new QLineEdit;
    rb = new QRadioButton;
    layout->addWidget(le1);
    layout->addWidget(le2);
    layout->addWidget(rb);
    w.setLayout(layout);

    QFilterInputContext *ic1, *ic2;
    ic1 = new QFilterInputContext;
    ic2 = new QFilterInputContext;
    le1->setInputContext(ic1);
    le2->setInputContext(ic2);

    w.show();
    QApplication::setActiveWindow(&w);

    // Testing that panel doesn't close between two input methods aware widgets.
    QTest::mouseClick(le1, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QTest::mouseClick(le2, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QVERIFY(ic2->lastTypes.indexOf(QEvent::CloseSoftwareInputPanel) < 0);

    // Testing that panel closes when focusing non-aware widget.
    QTest::mouseClick(rb, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QVERIFY(ic2->lastTypes.indexOf(QEvent::CloseSoftwareInputPanel) >= 0);
}

void tst_QInputContext::selections()
{
    QLineEdit le;
    le.setText("Test text");
    le.setSelection(2, 2);
    QCOMPARE(le.inputMethodQuery(Qt::ImCursorPosition).toInt(), 4);
    QCOMPARE(le.inputMethodQuery(Qt::ImAnchorPosition).toInt(), 2);

    QList<QInputMethodEvent::Attribute> attributes;
    attributes.append(QInputMethodEvent::Attribute(QInputMethodEvent::Selection, 5, 3, QVariant()));
    QInputMethodEvent event("", attributes);
    QApplication::sendEvent(&le, &event);
    QCOMPARE(le.cursorPosition(), 8);
    QCOMPARE(le.selectionStart(), 5);
    QCOMPARE(le.inputMethodQuery(Qt::ImCursorPosition).toInt(), 8);
    QCOMPARE(le.inputMethodQuery(Qt::ImAnchorPosition).toInt(), 5);
}

void tst_QInputContext::focusProxy()
{
    QWidget toplevel(0, Qt::X11BypassWindowManagerHint); toplevel.setObjectName("toplevel");
    QWidget w(&toplevel); w.setObjectName("w");
    QWidget proxy(&w); proxy.setObjectName("proxy");
    QWidget proxy2(&w); proxy2.setObjectName("proxy2");
    w.setFocusProxy(&proxy);
    w.setAttribute(Qt::WA_InputMethodEnabled);
    toplevel.show();
    QApplication::setActiveWindow(&toplevel);
    QTest::qWaitForWindowShown(&toplevel);
    w.setFocus();
    w.setAttribute(Qt::WA_NativeWindow); // we shouldn't crash!

    proxy.setAttribute(Qt::WA_InputMethodEnabled);
    proxy2.setAttribute(Qt::WA_InputMethodEnabled);

    proxy2.setFocus();
    w.setFocus();

    QInputContext *gic = qApp->inputContext();
    QVERIFY(gic);
    QCOMPARE(gic->focusWidget(), &proxy);

    // then change the focus proxy and check that input context is valid
    QVERIFY(w.hasFocus());
    QVERIFY(proxy.hasFocus());
    QVERIFY(!proxy2.hasFocus());
    w.setFocusProxy(&proxy2);
    QVERIFY(!w.hasFocus());
    QVERIFY(proxy.hasFocus());
    QVERIFY(!proxy2.hasFocus());
    QCOMPARE(gic->focusWidget(), &proxy);
}

void tst_QInputContext::contextInheritance()
{
    QWidget parent;
    QWidget child(&parent);

    parent.setAttribute(Qt::WA_InputMethodEnabled, true);
    child.setAttribute(Qt::WA_InputMethodEnabled, true);

    QCOMPARE(parent.inputContext(), qApp->inputContext());
    QCOMPARE(child.inputContext(), qApp->inputContext());

    QInputContext *qic = new QFilterInputContext;
    parent.setInputContext(qic);
    QCOMPARE(parent.inputContext(), qic);
    QCOMPARE(child.inputContext(), qic);

    parent.setAttribute(Qt::WA_InputMethodEnabled, false);
    QVERIFY(!parent.inputContext());
    QCOMPARE(child.inputContext(), qic);
    parent.setAttribute(Qt::WA_InputMethodEnabled, true);

    parent.setInputContext(0);
    QCOMPARE(parent.inputContext(), qApp->inputContext());
    QCOMPARE(child.inputContext(), qApp->inputContext());

    qic = new QFilterInputContext;
    qApp->setInputContext(qic);
    QCOMPARE(parent.inputContext(), qic);
    QCOMPARE(child.inputContext(), qic);
}

QTEST_MAIN(tst_QInputContext)
#include "tst_qinputcontext.moc"

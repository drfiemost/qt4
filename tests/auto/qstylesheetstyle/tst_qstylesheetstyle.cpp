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
#include <QtCore>
#include <QtGui>
#include <QtTest/QtTest>
#include <QtDebug>
#include <QMetaObject>

#include <private/qstylesheetstyle_p.h>
#include "../platformquirks.h"

//TESTED_CLASS=
//TESTED_FILES=

class tst_QStyleSheetStyle : public QObject
{
    Q_OBJECT
public:
    tst_QStyleSheetStyle();
    ~tst_QStyleSheetStyle();

private slots:
    void repolish();
    void numinstances();
    void widgetsBeforeAppStyleSheet();
    void widgetsAfterAppStyleSheet();
    void applicationStyleSheet();
    void windowStyleSheet();
    void widgetStyleSheet();
    void reparentWithNoChildStyleSheet();
    void reparentWithChildStyleSheet();
    void dynamicProperty();
    // NB! Invoking this slot after layoutSpacing crashes on Mac.
    void namespaces();
#ifdef Q_OS_MAC
    void layoutSpacing();
#endif
    void qproperty();
    void palettePropagation();
    void fontPropagation();
    void onWidgetDestroyed();
    void fontPrecedence();
    void focusColors();
    void hoverColors();
    void background();
    void tabAlignement();
    void attributesList();
    void minmaxSizes();
    void task206238_twice();
    void transparent();
    void proxyStyle();
    void dialogButtonBox();
    void emptyStyleSheet();
    void toolTip();
    void embeddedFonts();
    void opaquePaintEvent_data();
    void opaquePaintEvent();
    void complexWidgetFocus();
    void task188195_baseBackground();
    void task232085_spinBoxLineEditBg();
    void changeStyleInChangeEvent();
    void QTBUG15910_crashNullWidget();

    void styleSheetChangeBeforePolish();
    //at the end because it mess with the style.
    void widgetStyle();
    void appStyle();
    void QTBUG11658_cachecrash();
private:
    QColor COLOR(const QWidget& w) {
        w.ensurePolished();
        return w.palette().color(w.foregroundRole());
    }
    QColor APPCOLOR(const QWidget& w) {
        w.ensurePolished();
        return qApp->palette(&w).color(w.foregroundRole());
    }
    QColor BACKGROUND(const QWidget& w) {
        w.ensurePolished();
        return w.palette().color(w.backgroundRole());
    }
    QColor APPBACKGROUND(const QWidget& w) {
        w.ensurePolished();
        return qApp->palette(&w).color(w.backgroundRole());
    }
    int FONTSIZE(const QWidget &w) {
        w.ensurePolished();
        return w.font().pointSize();
    }
    int APPFONTSIZE(const QWidget &w) {
        return qApp->font(&w).pointSize();
    }
};

tst_QStyleSheetStyle::tst_QStyleSheetStyle()
{
}

tst_QStyleSheetStyle::~tst_QStyleSheetStyle()
{
}

void tst_QStyleSheetStyle::numinstances()
{
    /*QWidget w;
    QCommonStyle *style = new QCommonStyle;
    style->setParent(&w);
    QWidget c(&w);
    w.show();

    // set and unset application stylesheet
    QCOMPARE(QStyleSheetStyle::numinstances, 0);
    qApp->setStyleSheet("* { color: red; }");
    QCOMPARE(QStyleSheetStyle::numinstances, 1);
    qApp->setStyleSheet("");
    QCOMPARE(QStyleSheetStyle::numinstances, 0);

    // set and unset application stylesheet+widget
    qApp->setStyleSheet("* { color: red; }");
    w.setStyleSheet("color: red;");
    QCOMPARE(QStyleSheetStyle::numinstances, 2);
    w.setStyle(style);
    QCOMPARE(QStyleSheetStyle::numinstances, 2);
    qApp->setStyleSheet("");
    QCOMPARE(QStyleSheetStyle::numinstances, 1);
    w.setStyleSheet("");
    QCOMPARE(QStyleSheetStyle::numinstances, 0);

    // set and unset widget stylesheet
    w.setStyle(0);
    w.setStyleSheet("color: red");
    QCOMPARE(QStyleSheetStyle::numinstances, 1);
    c.setStyle(style);
    QCOMPARE(QStyleSheetStyle::numinstances, 2);
    w.setStyleSheet("");
    QCOMPARE(QStyleSheetStyle::numinstances, 0);*/
}

void tst_QStyleSheetStyle::widgetsBeforeAppStyleSheet()
{
    QPushButton w1; // widget with no stylesheet
    qApp->setStyleSheet("* { color: red; }");
    QVERIFY(COLOR(w1) == QColor("red"));
    w1.setStyleSheet("color: white");
    QVERIFY(COLOR(w1) == QColor("white"));
    qApp->setStyleSheet("");
    QVERIFY(COLOR(w1) == QColor("white"));
    w1.setStyleSheet("");
    QVERIFY(COLOR(w1) == APPCOLOR(w1));
}

class FriendlySpinBox : public QSpinBox { friend class tst_QStyleSheetStyle; };

void tst_QStyleSheetStyle::widgetsAfterAppStyleSheet()
{
    qApp->setStyleSheet("* { color: red; font-size: 32pt; }");
    QPushButton w1;
    FriendlySpinBox spin;
    QVERIFY(COLOR(w1) == QColor("red"));
    QVERIFY(COLOR(spin) == QColor("red"));
    QVERIFY(COLOR(*spin.lineEdit()) == QColor("red"));
    QCOMPARE(FONTSIZE(w1), 32);
    QCOMPARE(FONTSIZE(spin), 32);
    QCOMPARE(FONTSIZE(*spin.lineEdit()), 32);
    w1.setStyleSheet("color: white");
    QVERIFY(COLOR(w1) == QColor("white"));
    QVERIFY(COLOR(spin) == QColor("red"));
    QVERIFY(COLOR(*spin.lineEdit()) == QColor("red"));
    w1.setStyleSheet("");
    QVERIFY(COLOR(w1) == QColor("red"));
    QVERIFY(COLOR(spin) == QColor("red"));
    QVERIFY(COLOR(*spin.lineEdit()) == QColor("red"));
    w1.setStyleSheet("color: white");
    QVERIFY(COLOR(w1) == QColor("white"));
    qApp->setStyleSheet("");
    QVERIFY(COLOR(w1) == QColor("white"));
    QVERIFY(COLOR(spin) == APPCOLOR(spin));
    QVERIFY(COLOR(*spin.lineEdit()) == APPCOLOR(*spin.lineEdit()));
    w1.setStyleSheet("");
    QVERIFY(COLOR(w1) == APPCOLOR(w1));
    // QCOMPARE(FONTSIZE(w1), APPFONTSIZE(w1));  //### task 244261
    QCOMPARE(FONTSIZE(spin), APPFONTSIZE(spin));
    //QCOMPARE(FONTSIZE(*spin.lineEdit()), APPFONTSIZE(*spin.lineEdit())); //### task 244261
}

void tst_QStyleSheetStyle::applicationStyleSheet()
{
    QPushButton w1;
    qApp->setStyleSheet("* { color: red; }");
    QVERIFY(COLOR(w1) == QColor("red"));
    qApp->setStyleSheet("* { color: white; }");
    QVERIFY(COLOR(w1) == QColor("white"));
    qApp->setStyleSheet("");
    QVERIFY(COLOR(w1) == APPCOLOR(w1));
    qApp->setStyleSheet("* { color: red }");
    QVERIFY(COLOR(w1) == QColor("red"));
}

void tst_QStyleSheetStyle::windowStyleSheet()
{
    QPushButton w1;
    qApp->setStyleSheet("");
    w1.setStyleSheet("* { color: red; }");
    QVERIFY(COLOR(w1) == QColor("red"));
    w1.setStyleSheet("* { color: white; }");
    QVERIFY(COLOR(w1) == QColor("white"));
    w1.setStyleSheet("");
    QVERIFY(COLOR(w1) == APPCOLOR(w1));
    w1.setStyleSheet("* { color: red }");
    QVERIFY(COLOR(w1) == QColor("red"));

    qApp->setStyleSheet("* { color: green }");
    QVERIFY(COLOR(w1) == QColor("red"));
    w1.setStyleSheet("");
    QVERIFY(COLOR(w1) == QColor("green"));
    qApp->setStyleSheet("");
    QVERIFY(COLOR(w1) == APPCOLOR(w1));
}

void tst_QStyleSheetStyle::widgetStyleSheet()
{
    QPushButton w1;
    QPushButton *pb = new QPushButton(&w1);
    QPushButton &w2 = *pb;

    qApp->setStyleSheet("");
    w1.setStyleSheet("* { color: red }");
    QVERIFY(COLOR(w1) == QColor("red"));
    QVERIFY(COLOR(w2) == QColor("red"));

    w2.setStyleSheet("* { color: white }");
    QVERIFY(COLOR(w2) == QColor("white"));

    w1.setStyleSheet("* { color: blue }");
    QVERIFY(COLOR(w1) == QColor("blue"));
    QVERIFY(COLOR(w2) == QColor("white"));

    w1.setStyleSheet("");
    QVERIFY(COLOR(w1) == APPCOLOR(w1));
    QVERIFY(COLOR(w2) == QColor("white"));

    w2.setStyleSheet("");
    QVERIFY(COLOR(w1) == APPCOLOR(w1));
    QVERIFY(COLOR(w2) == APPCOLOR(w2));
}

void tst_QStyleSheetStyle::reparentWithNoChildStyleSheet()
{
    QPushButton p1, p2;
    QPushButton *pb = new QPushButton(&p1);
    QPushButton &c1 = *pb; // child with no stylesheet

    qApp->setStyleSheet("");
    p1.setStyleSheet("* { color: red }");
    QVERIFY(COLOR(c1) == QColor("red"));
    c1.setParent(&p2);
    QVERIFY(COLOR(c1) == APPCOLOR(c1));

    p2.setStyleSheet("* { color: white }");
    QVERIFY(COLOR(c1) == QColor("white"));

    c1.setParent(&p1);
    QVERIFY(COLOR(c1) == QColor("red"));

    qApp->setStyleSheet("* { color: blue }");
    c1.setParent(0);
    QVERIFY(COLOR(c1) == QColor("blue"));
    delete pb;
}

void tst_QStyleSheetStyle::reparentWithChildStyleSheet()
{
    qApp->setStyleSheet("");
    QPushButton p1, p2;
    QPushButton *pb = new QPushButton(&p1);
    QPushButton &c1 = *pb;

    c1.setStyleSheet("background: gray");
    QVERIFY(BACKGROUND(c1) == QColor("gray"));
    c1.setParent(&p2);
    QVERIFY(BACKGROUND(c1) == QColor("gray"));

    qApp->setStyleSheet("* { color: white }");
    c1.setParent(&p1);
    QVERIFY(BACKGROUND(c1) == QColor("gray"));
    QVERIFY(COLOR(c1) == QColor("white"));
}

void tst_QStyleSheetStyle::repolish()
{
    qApp->setStyleSheet("");
    QPushButton p1;
    p1.setStyleSheet("color: red; background: white");
    QVERIFY(BACKGROUND(p1) == QColor("white"));
    p1.setStyleSheet("background: white");
    QVERIFY(COLOR(p1) == APPCOLOR(p1));
    p1.setStyleSheet("color: red");
    QVERIFY(COLOR(p1) == QColor("red"));
    QVERIFY(BACKGROUND(p1) == APPBACKGROUND(p1));
    p1.setStyleSheet("");
    QVERIFY(COLOR(p1) == APPCOLOR(p1));
    QVERIFY(BACKGROUND(p1) == APPBACKGROUND(p1));
}

void tst_QStyleSheetStyle::widgetStyle()
{
    /*qApp->setStyleSheet("");

    QWidget *window1 = new QWidget;
    window1->setObjectName("window1");
    QWidget *widget1 = new QWidget(window1);
    widget1->setObjectName("widget1");
    QWidget *widget2 = new QWidget;
    widget2->setObjectName("widget2");
    QWidget *window2 = new QWidget;
    window2->setObjectName("window2");
    window1->ensurePolished();
    window2->ensurePolished();
    widget1->ensurePolished();
    widget2->ensurePolished();

    QWindowsStyle style1, style2;
    QPointer<QStyle> pstyle1 = &style1;
    QPointer<QStyle> pstyle2 = &style2;

    QStyle *appStyle = qApp->style();

    // Sanity: By default, a window inherits the application style
    QCOMPARE(appStyle, window1->style());

    // Setting a custom style on a widget
    window1->setStyle(&style1);
    QCOMPARE(static_cast<QStyle *>(&style1), window1->style());

    // Setting another style must not delete the older style
    window1->setStyle(&style2);
    QCOMPARE(static_cast<QStyle *>(&style2), window1->style());
    QVERIFY(!pstyle1.isNull()); // case we have not already crashed

    // Setting null style must make it follow the qApp style
    window1->setStyle(0);
    QCOMPARE(window1->style(), appStyle);
    QVERIFY(!pstyle2.isNull()); // case we have not already crashed
    QVERIFY(!pstyle2.isNull()); // case we have not already crashed

    // Sanity: Set the stylesheet
    window1->setStyleSheet(":x { }");

    QPointer<QStyleSheetStyle> proxy = (QStyleSheetStyle *)window1->style();
    QVERIFY(!proxy.isNull());
    QCOMPARE(proxy->metaObject()->className(), "QStyleSheetStyle"); // must be our proxy
    QVERIFY(proxy->base == 0); // and follows the application

    // Set the stylesheet
    window1->setStyle(&style1);
    QVERIFY(proxy.isNull()); // we create a new one each time
    proxy = (QStyleSheetStyle *)window1->style();
    QCOMPARE(proxy->metaObject()->className(), "QStyleSheetStyle"); // it is a proxy
    QCOMPARE(proxy->baseStyle(), static_cast<QStyle *>(&style1)); // must have been replaced with the new one

    // Update the stylesheet and check nothing changes
    window1->setStyleSheet(":y { }");
    QCOMPARE(window1->style()->metaObject()->className(), "QStyleSheetStyle"); // it is a proxy
    QCOMPARE(proxy->baseStyle(), static_cast<QStyle *>(&style1)); // the same guy

    // Remove the stylesheet
    proxy = (QStyleSheetStyle *)window1->style();
    window1->setStyleSheet("");
    QVERIFY(proxy.isNull()); // should have disappeared
    QCOMPARE(window1->style(), static_cast<QStyle *>(&style1)); // its restored

    // Style Sheet existing children propagation
    window1->setStyleSheet(":z { }");
    proxy = (QStyleSheetStyle *)window1->style();
    QCOMPARE(proxy->metaObject()->className(), "QStyleSheetStyle");
    QCOMPARE(window1->style(), widget1->style()); // proxy must have propagated
    QCOMPARE(widget2->style(), appStyle); // widget2 is following the app style

    // Style Sheet automatic propagation to new children
    widget2->setParent(window1); // reparent in!
    QCOMPARE(window1->style(), widget2->style()); // proxy must have propagated

    // Style Sheet automatic removal from children who abandoned their parents
    window2->setStyle(&style2);
    widget2->setParent(window2); // reparent
    QCOMPARE(widget2->style(), appStyle); // widget2 is following the app style

    // Style Sheet propagation on a child widget with a custom style
    widget2->setStyle(&style1);
    window2->setStyleSheet(":x { }");
    proxy = (QStyleSheetStyle *)widget2->style();
    QCOMPARE(proxy->metaObject()->className(), "QStyleSheetStyle");
    QCOMPARE(proxy->baseStyle(), static_cast<QStyle *>(&style1));

    // Style Sheet propagation on a child widget with a custom style already set
    window2->setStyleSheet("");
    QCOMPARE(window2->style(), static_cast<QStyle *>(&style2));
    QCOMPARE(widget2->style(), static_cast<QStyle *>(&style1));
    widget2->setStyle(0);
    window2->setStyleSheet(":x { }");
    widget2->setStyle(&style1);
    proxy = (QStyleSheetStyle *)widget2->style();
    QCOMPARE(proxy->metaObject()->className(), "QStyleSheetStyle");

    // QApplication, QWidget both having a style sheet

    // clean everything out
    window1->setStyle(0);
    window1->setStyleSheet("");
    window2->setStyle(0);
    window2->setStyleSheet("");
    qApp->setStyle(0);

    qApp->setStyleSheet("may_insanity_prevail { }"); // app has stylesheet
    QCOMPARE(window1->style(), qApp->style());
    QCOMPARE(window1->style()->metaObject()->className(), "QStyleSheetStyle");
    QCOMPARE(widget1->style()->metaObject()->className(), "QStyleSheetStyle"); // check the child
    window1->setStyleSheet("may_more_insanity_prevail { }"); // window has stylesheet
    QCOMPARE(window1->style()->metaObject()->className(), "QStyleSheetStyle"); // a new one
    QCOMPARE(widget1->style(), window1->style()); // child follows...
    proxy = (QStyleSheetStyle *) window1->style();
    QWindowsStyle *newStyle = new QWindowsStyle;
    qApp->setStyle(newStyle); // set a custom style on app
    proxy = (QStyleSheetStyle *) window1->style();
    QCOMPARE(proxy->baseStyle(), static_cast<QStyle *>(newStyle)); // magic ;) the widget still follows the application
    QCOMPARE(static_cast<QStyle *>(proxy), widget1->style()); // child still follows...

    window1->setStyleSheet(""); // remove stylesheet
    QCOMPARE(window1->style(), qApp->style()); // is this cool or what
    QCOMPARE(widget1->style(), qApp->style()); // annoying child follows...
    QWindowsStyle wndStyle;
    window1->setStyle(&wndStyle);
    QCOMPARE(window1->style()->metaObject()->className(), "QStyleSheetStyle"); // auto wraps it
    QCOMPARE(widget1->style(), window1->style()); // and auto propagates to child
    qApp->setStyleSheet(""); // remove the app stylesheet
    QCOMPARE(window1->style(), static_cast<QStyle *>(&wndStyle)); // auto dewrap
    QCOMPARE(widget1->style(), qApp->style()); // and child state is restored
    window1->setStyle(0); // let sanity prevail
    qApp->setStyle(0);

    delete window1;
    delete widget2;
    delete window2;*/
}

void tst_QStyleSheetStyle::appStyle()
{
    /*qApp->setStyleSheet("");
    // qApp style can never be 0
    QVERIFY(QApplication::style() != 0);
    QPointer<QStyle> style1 = new QWindowsStyle;
    QPointer<QStyle> style2 = new QWindowsStyle;
    qApp->setStyle(style1);
    // Basic sanity
    QVERIFY(qApp->style() == style1);
    qApp->setStyle(style2);
    QVERIFY(style1.isNull()); // qApp must have taken ownership and deleted it
    // Setting null should not crash
    qApp->setStyle(0);
    QVERIFY(qApp->style() == style2);

    // Set the stylesheet
    qApp->setStyleSheet("whatever");
    QPointer<QStyleSheetStyle> sss = (QStyleSheetStyle *)qApp->style();
    QVERIFY(!sss.isNull());
    QCOMPARE(sss->metaObject()->className(), "QStyleSheetStyle"); // must be our proxy now
    QVERIFY(!style2.isNull()); // this should exist as it is the base of the proxy
    QVERIFY(sss->baseStyle() == style2);
    style1 = new QWindowsStyle;
    qApp->setStyle(style1);
    QVERIFY(style2.isNull()); // should disappear automatically
    QVERIFY(sss.isNull()); // should disappear automatically

    // Update the stylesheet and check nothing changes
    sss = (QStyleSheetStyle *)qApp->style();
    qApp->setStyleSheet("whatever2");
    QVERIFY(qApp->style() == sss);
    QVERIFY(sss->baseStyle() == style1);

    // Revert the stylesheet
    qApp->setStyleSheet("");
    QVERIFY(sss.isNull()); // should have disappeared
    QVERIFY(qApp->style() == style1);

    qApp->setStyleSheet("");
    QVERIFY(qApp->style() == style1);*/
}

void tst_QStyleSheetStyle::dynamicProperty()
{
    qApp->setStyleSheet(QString());

    QString appStyle = qApp->style()->metaObject()->className();
    QPushButton pb1, pb2;
    pb1.setProperty("type", "critical");
    qApp->setStyleSheet("*[class~=\"QPushButton\"] { color: red; } *[type=\"critical\"] { background: white; }");
    QVERIFY(COLOR(pb1) == Qt::red);
    QVERIFY(BACKGROUND(pb1) == Qt::white);

    pb2.setProperty("class", "critical"); // dynamic class
    pb2.setStyleSheet(QLatin1String(".critical[style~=\"") + appStyle + "\"] { color: blue }");
    pb2.show();

    QVERIFY(COLOR(pb2) == Qt::blue);
}

#ifdef Q_OS_MAC
#include <QtGui/QMacStyle>
void tst_QStyleSheetStyle::layoutSpacing()
{
    qApp->setStyleSheet("* { color: red }");
    QCheckBox ck1;
    QCheckBox ck2;
    QWidget window;
    int spacing_widgetstyle = window.style()->layoutSpacing(ck1.sizePolicy().controlType(), ck2.sizePolicy().controlType(), Qt::Vertical);
    int spacing_style = window.style()->layoutSpacing(ck1.sizePolicy().controlType(), ck2.sizePolicy().controlType(), Qt::Vertical);
    QCOMPARE(spacing_widgetstyle, spacing_style);
}
#endif

void tst_QStyleSheetStyle::qproperty()
{
    QPushButton pb;
    pb.setStyleSheet("QPushButton { qproperty-text: hello; qproperty-checkable: true; qproperty-checked: 1}");
    pb.ensurePolished();
    QCOMPARE(pb.text(), QString("hello"));
    QCOMPARE(pb.isCheckable(), true);
    QCOMPARE(pb.isChecked(), true);
}

namespace ns {
    class PushButton1 : public QPushButton {
        Q_OBJECT
    public:
        PushButton1() { }
    };
    class PushButton2 : public PushButton1 {
        Q_OBJECT
    public:
        PushButton2() { setProperty("class", "PushButtonTwo PushButtonDuo"); }
    };
}

void tst_QStyleSheetStyle::namespaces()
{
    ns::PushButton1 pb1;
    qApp->setStyleSheet("ns--PushButton1 { background: white }");
    QVERIFY(BACKGROUND(pb1) == QColor("white"));
    qApp->setStyleSheet(".ns--PushButton1 { background: red }");
    QVERIFY(BACKGROUND(pb1) == QColor("red"));

    ns::PushButton2 pb2;
    qApp->setStyleSheet("ns--PushButton1 { background: blue}");
    QVERIFY(BACKGROUND(pb2) == QColor("blue"));
    qApp->setStyleSheet("ns--PushButton2 { background: magenta }");
    QVERIFY(BACKGROUND(pb2) == QColor("magenta"));
    qApp->setStyleSheet(".PushButtonTwo { background: white; }");
    QVERIFY(BACKGROUND(pb2) == QColor("white"));
    qApp->setStyleSheet(".PushButtonDuo { background: red; }");
    QVERIFY(BACKGROUND(pb2) == QColor("red"));
}

void tst_QStyleSheetStyle::palettePropagation()
{
    qApp->setStyleSheet("");
    QGroupBox gb;
    QPushButton *push = new QPushButton(&gb);
    QPushButton &pb = *push;
    push->setText("AsdF");

    gb.setStyleSheet("QGroupBox { color: red }");
    QVERIFY(COLOR(gb) == Qt::red);
    QVERIFY(COLOR(pb) == APPCOLOR(pb)); // palette shouldn't propagate
    gb.setStyleSheet("QGroupBox * { color: red }");

    QVERIFY(COLOR(pb) == Qt::red);
    QVERIFY(COLOR(gb) == APPCOLOR(gb));

    QWidget window;
    window.setStyleSheet("* { color: white; }");
    pb.setParent(&window);
    QVERIFY(COLOR(pb) == Qt::white);
}

void tst_QStyleSheetStyle::fontPropagation()
{
    qApp->setStyleSheet("");
    QComboBox cb;
    cb.addItem("item1");
    cb.addItem("item2");

    QAbstractItemView *popup = cb.view();
    int viewFontSize = FONTSIZE(*popup);

    cb.setStyleSheet("QComboBox { font-size: 20pt; }");
    QVERIFY(FONTSIZE(cb) == 20);
    QVERIFY(FONTSIZE(*popup) == viewFontSize);
    QGroupBox gb;
    QPushButton *push = new QPushButton(&gb);
    QPushButton &pb = *push;
    int buttonFontSize = FONTSIZE(pb);
    int gbFontSize = FONTSIZE(gb);

    gb.setStyleSheet("QGroupBox { font-size: 20pt }");
    QVERIFY(FONTSIZE(gb) == 20);
    QVERIFY(FONTSIZE(pb) == buttonFontSize); // font does not propagate
    gb.setStyleSheet("QGroupBox * { font-size: 20pt; }");
    QVERIFY(FONTSIZE(gb) == gbFontSize);
    QVERIFY(FONTSIZE(pb) == 20);

    QWidget window;
    window.setStyleSheet("* { font-size: 10pt }");
    pb.setParent(&window);
    QCOMPARE(FONTSIZE(pb), 10);
    window.setStyleSheet("");
    QVERIFY(FONTSIZE(pb) == buttonFontSize);

    QTabWidget tw;
    tw.setStyleSheet("QTabWidget { font-size: 20pt; }");
    QVERIFY(FONTSIZE(tw) == 20);
    QWidget *child = qFindChild<QWidget *>(&tw, "qt_tabwidget_tabbar");
    QVERIFY2(child, "QTabWidget did not contain a widget named \"qt_tabwidget_tabbar\"");
    QVERIFY(FONTSIZE(*child) == 20);
}

void tst_QStyleSheetStyle::onWidgetDestroyed()
{
    qApp->setStyleSheet("");
    QLabel *l = new QLabel;
    l->setStyleSheet("QLabel { color: red }");
    QPointer<QStyleSheetStyle> ss = (QStyleSheetStyle *) l->style();
    delete l;
    QVERIFY(ss.isNull());
}

void tst_QStyleSheetStyle::fontPrecedence()
{
    QLineEdit edit;
    edit.show();
    QFont font;
    QVERIFY(FONTSIZE(edit) != 22); // Sanity check to make sure this test makes sense.
    edit.setStyleSheet("QLineEdit { font-size: 22pt; }");
    QCOMPARE(FONTSIZE(edit), 22);
    font.setPointSize(16);
    edit.setFont(font);
    QCOMPARE(FONTSIZE(edit), 22);
    edit.setStyleSheet("");
    QCOMPARE(FONTSIZE(edit), 16);
    font.setPointSize(18);
    edit.setFont(font);
    QCOMPARE(FONTSIZE(edit), 18);
    edit.setStyleSheet("QLineEdit { font-size: 20pt; }");
    QCOMPARE(FONTSIZE(edit), 20);
    edit.setStyleSheet("");
    QCOMPARE(FONTSIZE(edit), 18);
    edit.hide();

    QLineEdit edit2;
    edit2.setReadOnly(true);
    edit2.setStyleSheet("QLineEdit { font-size: 24pt; } QLineEdit:read-only { font-size: 26pt; }");
    QCOMPARE(FONTSIZE(edit2), 26);
}

// Ensure primary will only return true if the color covers more than 50% of pixels
static bool testForColors(const QImage& image, const QColor& color, bool ensurePrimary=false)
{
    int count = 0;
    QRgb rgb = color.rgba();
    int totalCount = image.height()*image.width();
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            // Because of antialiasing we allow a certain range of errors here.
            QRgb pixel = image.pixel(x, y);

            if (std::abs((int)(pixel & 0xff) - (int)(rgb & 0xff)) +
                    std::abs((int)((pixel & 0xff00) >> 8) - (int)((rgb & 0xff00) >> 8)) +
                    std::abs((int)((pixel & 0xff0000) >> 16) - (int)((rgb & 0xff0000) >> 16)) <= 50) {
                count++;
                if (!ensurePrimary && count >=10 )
                    return true;
                else if (count > totalCount/2)
                    return true;
            }
        }
    }

    return false;
}

void tst_QStyleSheetStyle::focusColors()
{
    // Tests if colors can be changed by altering the focus of the widget.
    // To avoid messy pixel-by-pixel comparison, we assume that the goal
    // is reached if at least ten pixels of the right color can be found in
    // the image.
    // For this reason, we use unusual and extremely ugly colors! :-)

#if !defined(Q_OS_WIN32) && !defined(Q_OS_MAC) && !(defined(Q_OS_LINUX) && defined(Q_CC_GNU) \
    && !defined(Q_CC_INTEL))
    QSKIP("This is a fragile test which fails on many esoteric platforms "
            "because of focus problems. "
            "That doesn't mean that the feature doesn't work in practice.", SkipAll);
#endif
/*  Disabled changing style in order not to mess with the application style
#ifdef Q_OS_MAC
    int styleCount = 3;
    QStyle *styles[3];
#else
    int styleCount = 2;
    QStyle *styles[2];
#endif

    styles[0] = new QPlastiqueStyle;
    styles[1] = new QWindowsStyle;

#ifdef Q_OS_MAC
    styles[2] = new QMacStyle;
#endif


    for (int i = 0; i < styleCount; ++i) {
        qApp->setStyle(styles[i]);
*/
        QList<QWidget *> widgets;
        widgets << new QPushButton("TESTING");
        widgets << new QLineEdit("TESTING");
        widgets << new QLabel("TESTING");
        QSpinBox *spinbox = new QSpinBox;
        spinbox->setValue(8888);
        widgets << spinbox;
        QComboBox *combobox = new QComboBox;
        combobox->setEditable(true);
        combobox->addItems(QStringList() << "TESTING");
        widgets << combobox;
        widgets << new QLabel("TESTING");

#ifdef Q_WS_QWS
        // QWS has its own special focus logic which is slightly different
        // and I don't dare change it at this point, because someone will
        // be relying on it. It means that setFocus() on a NoFocus widget (i.e.
        // a QLabel) will not work before the window is activated.
        widgets[2]->setFocusPolicy(Qt::StrongFocus);
#endif

        foreach (QWidget *widget, widgets) {
            QDialog frame;
            QLayout* layout = new QGridLayout;

            QLineEdit* dummy = new QLineEdit; // Avoids initial focus.

            widget->setStyleSheet("*:focus { border:none; background: #e8ff66; color: #ff0084 }");

            layout->addWidget(dummy);
            layout->addWidget(widget);
            frame.setLayout(layout);

            frame.show();
#ifdef Q_WS_X11
            qt_x11_wait_for_window_manager(&frame);
#endif
            QApplication::setActiveWindow(&frame);
            widget->setFocus();
            QApplication::processEvents();

            QImage image(frame.width(), frame.height(), QImage::Format_ARGB32);
            frame.render(&image);
            if (image.depth() < 24) {
                QSKIP("Test doesn't support color depth < 24", SkipAll);
            }

            QVERIFY2(testForColors(image, QColor(0xe8, 0xff, 0x66)),
                    (QString::fromLatin1(widget->metaObject()->className())
                    + " did not contain background color #e8ff66, using style "
                    + QString::fromLatin1(qApp->style()->metaObject()->className()))
                    .toLocal8Bit().constData());
            QVERIFY2(testForColors(image, QColor(0xff, 0x00, 0x84)),
                    (QString::fromLatin1(widget->metaObject()->className())
                    + " did not contain text color #ff0084, using style "
                    + QString::fromLatin1(qApp->style()->metaObject()->className()))
                    .toLocal8Bit().constData());
        }
  //  }
}


void tst_QStyleSheetStyle::hoverColors()
{
    if (!PlatformQuirks::haveMouseCursor())
        QSKIP("No mouse Cursor on this platform",SkipAll);
    QList<QWidget *> widgets;
    widgets << new QPushButton("TESTING");
    widgets << new QLineEdit("TESTING");
    widgets << new QLabel("TESTING");
    QSpinBox *spinbox = new QSpinBox;
    spinbox->setValue(8888);
    widgets << spinbox;
    QComboBox *combobox = new QComboBox;
    combobox->setEditable(true);
    combobox->addItems(QStringList() << "TESTING");
    widgets << combobox;
    widgets << new QLabel("<b>TESTING</b>");

    foreach (QWidget *widget, widgets) {
        //without Qt::X11BypassWindowManagerHint the window manager may move the window after we moved the cursor
        QDialog frame(0, Qt::X11BypassWindowManagerHint);
        QLayout* layout = new QGridLayout;

        QLineEdit* dummy = new QLineEdit;

        widget->setStyleSheet("*:hover { border:none; background: #e8ff66; color: #ff0084 }");

        layout->addWidget(dummy);
        layout->addWidget(widget);
        frame.setLayout(layout);

        frame.show();
#ifdef Q_WS_QWS
//QWS does not implement enter/leave when windows are shown underneath the cursor
        QCursor::setPos(QPoint(0,0));
#endif

#ifdef Q_WS_X11
        qt_x11_wait_for_window_manager(&frame);
#endif
        QApplication::setActiveWindow(&frame);
        QTest::qWait(60);
        //move the mouse inside the widget, it should be colored
        QTest::mouseMove ( widget, QPoint(5,5));
        QTest::qWait(60);

        QVERIFY(widget->testAttribute(Qt::WA_UnderMouse));

        QImage image(frame.width(), frame.height(), QImage::Format_ARGB32);
        frame.render(&image);

        QVERIFY2(testForColors(image, QColor(0xe8, 0xff, 0x66)),
                  (QString::fromLatin1(widget->metaObject()->className())
                  + " did not contain background color #e8ff66").toLocal8Bit().constData());
        QVERIFY2(testForColors(image, QColor(0xff, 0x00, 0x84)),
                 (QString::fromLatin1(widget->metaObject()->className())
                  + " did not contain text color #ff0084").toLocal8Bit().constData());

        //move the mouse outside the widget, it should NOT be colored
        QTest::mouseMove ( dummy, QPoint(5,5));
        QTest::qWait(60);

        frame.render(&image);

        QVERIFY2(!testForColors(image, QColor(0xe8, 0xff, 0x66)),
                  (QString::fromLatin1(widget->metaObject()->className())
                  + " did contain background color #e8ff66").toLocal8Bit().constData());
        QVERIFY2(!testForColors(image, QColor(0xff, 0x00, 0x84)),
                 (QString::fromLatin1(widget->metaObject()->className())
                  + " did contain text color #ff0084").toLocal8Bit().constData());

        //move the mouse again inside the widget, it should be colored
        QTest::mouseMove (widget, QPoint(5,5));
        QTest::qWait(60);

        QVERIFY(widget->testAttribute(Qt::WA_UnderMouse));

        frame.render(&image);

        QVERIFY2(testForColors(image, QColor(0xe8, 0xff, 0x66)),
                 (QString::fromLatin1(widget->metaObject()->className())
                 + " did not contain background color #e8ff66").toLocal8Bit().constData());
        QVERIFY2(testForColors(image, QColor(0xff, 0x00, 0x84)),
                (QString::fromLatin1(widget->metaObject()->className())
                + " did not contain text color #ff0084").toLocal8Bit().constData());
    }

}

class SingleInheritanceDialog : public QDialog
{
    Q_OBJECT
public:
    SingleInheritanceDialog(QWidget *w = 0) :
        QDialog(w)
    {
    }
};

class DoubleInheritanceDialog : public SingleInheritanceDialog
{
    Q_OBJECT
public:
    DoubleInheritanceDialog(QWidget *w = 0) :
        SingleInheritanceDialog(w)
    {
    }
};

void tst_QStyleSheetStyle::background()
{
    const int number = 4;
    QWidget* widgets[number];
    // Testing inheritance styling of QDialog.
    widgets[0] = new SingleInheritanceDialog;
    widgets[0]->setStyleSheet("* { background-color: #e8ff66; }");
    widgets[1] = new DoubleInheritanceDialog;
    widgets[1]->setStyleSheet("* { background-color: #e8ff66; }");

    // Testing gradients in QComboBox.
    QLayout* layout;
    QComboBox* cb;
    // First color
    widgets[2] = new QDialog;
    layout = new QGridLayout;
    cb = new QComboBox;
    cb->setStyleSheet("* { background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop:0 #e8ff66, stop:1 #000000); }");
    layout->addWidget(cb);
    widgets[2]->setLayout(layout);
    // Second color
    widgets[3] = new QDialog;
    layout = new QGridLayout;
    cb = new QComboBox;
    cb->setStyleSheet("* { background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop:0 #e8ff66, stop:1 #000000); }");
    layout->addWidget(cb);
    widgets[3]->setLayout(layout);

    for (int c = 0; c < number; ++c) {
        QWidget* widget = widgets[c];

        widget->show();
#ifdef Q_WS_X11
        qt_x11_wait_for_window_manager(widget);
#endif

        QImage image(widget->width(), widget->height(), QImage::Format_ARGB32);
        widget->render(&image);
        if (image.depth() < 24) {
            QSKIP("Test doesn't support color depth < 24", SkipAll);
        }

        QVERIFY2(testForColors(image, QColor(0xe8, 0xff, 0x66)),
                (QString::fromLatin1(widget->metaObject()->className())
                + " did not contain background image with color #e8ff66")
                .toLocal8Bit().constData());

        delete widget;
    }
}

void tst_QStyleSheetStyle::tabAlignement()
{
    QWidget topLevel;
    QTabWidget tabWidget(&topLevel);
    tabWidget.addTab(new QLabel("tab1"),"tab1");
    tabWidget.resize(QSize(400,400));
    topLevel.show();
    QTest::qWaitForWindowShown(&tabWidget);
    QTest::qWait(50);
    QTabBar *bar = qFindChild<QTabBar*>(&tabWidget);
    QVERIFY(bar);
    //check the tab is on the right
    tabWidget.setStyleSheet("QTabWidget::tab-bar { alignment: right ; }");
    qApp->processEvents();
    QVERIFY(bar->geometry().right() > 380);
    QVERIFY(bar->geometry().left() > 200);
    //check the tab is on the middle
    tabWidget.setStyleSheet("QTabWidget::tab-bar { alignment: center ; }");
    QVERIFY(bar->geometry().right() < 300);
    QVERIFY(bar->geometry().left() > 100);
    //check the tab is on the left
    tabWidget.setStyleSheet("QTabWidget::tab-bar { alignment: left ; }");
    QVERIFY(bar->geometry().left() < 20);
    QVERIFY(bar->geometry().right() < 200);

    tabWidget.setTabPosition(QTabWidget::West);
    //check the tab is on the top
    QVERIFY(bar->geometry().top() < 20);
    QVERIFY(bar->geometry().bottom() < 200);
    //check the tab is on the bottom
    tabWidget.setStyleSheet("QTabWidget::tab-bar { alignment: right ; }");
    QVERIFY(bar->geometry().bottom() > 380);
    QVERIFY(bar->geometry().top() > 200);
    //check the tab is on the middle
    tabWidget.setStyleSheet("QTabWidget::tab-bar { alignment: center ; }");
    QVERIFY(bar->geometry().bottom() < 300);
    QVERIFY(bar->geometry().top() > 100);
}

void tst_QStyleSheetStyle::attributesList()
{
    QWidget w;
    QPushButton *p1=new QPushButton(&w);
    QPushButton *p2=new QPushButton(&w);
    QPushButton *p3=new QPushButton(&w);
    QPushButton *p4=new QPushButton(&w);
    p1->setProperty("prop", QStringList() << "red");
    p2->setProperty("prop", QStringList() << "foo" << "red");
    p3->setProperty("prop", QStringList() << "foo" << "bar");

    w.setStyleSheet(" QPushButton{ background-color:blue; }  QPushButton[prop~=red] { background-color:red; }");
    QCOMPARE(BACKGROUND(*p1) , QColor("red"));
    QCOMPARE(BACKGROUND(*p2) , QColor("red"));
    QCOMPARE(BACKGROUND(*p3) , QColor("blue"));
    QCOMPARE(BACKGROUND(*p4) , QColor("blue"));
}

void tst_QStyleSheetStyle::minmaxSizes()
{
    QTabWidget tabWidget;
    tabWidget.setObjectName("tabWidget");
    int index1 = tabWidget.addTab(new QLabel("Tab1"),"a");

    QWidget *page2=new QLabel("page2");
    page2->setObjectName("page2");
    page2->setStyleSheet("* {background-color: white; min-width: 250px; max-width:500px }");
    tabWidget.addTab(page2,"Tab2");
    QWidget *page3=new QLabel("plop");
    page3->setObjectName("Page3");
    page3->setStyleSheet("* {background-color: yellow; min-height: 250px; max-height:500px }");
    int index3 = tabWidget.addTab(page3,"very_long_long_long_long_caption");

    tabWidget.setStyleSheet("QTabBar::tab { min-width:100px; max-width:130px; }");

    tabWidget.show();
    QTest::qWait(50);
    //i allow 4px additional border from the native style (hence the -2, <=2)
    QVERIFY(std::abs(page2->maximumSize().width() - 500 - 2) <= 2);
    QVERIFY(std::abs(page2->minimumSize().width() - 250 - 2) <= 2);
    QVERIFY(std::abs(page3->maximumSize().height() - 500 - 2) <= 2);
    QVERIFY(std::abs(page3->minimumSize().height() - 250 - 2) <= 2);
    QVERIFY(std::abs(page3->minimumSize().height() - 250 - 2) <= 2);
    QTabBar *bar = qFindChild<QTabBar*>(&tabWidget);
    QVERIFY(bar);
    QVERIFY(std::abs(bar->tabRect(index1).width() - 100 - 2) <= 2);
    QVERIFY(std::abs(bar->tabRect(index3).width() - 130 - 2) <= 2);
}

void tst_QStyleSheetStyle::task206238_twice()
{
    QMainWindow w;
    QTabWidget* tw = new QTabWidget;
    tw->addTab(new QLabel("foo"), "test");
    w.setCentralWidget(tw);
    w.setStyleSheet("background: red;");
    w.show();
    QTest::qWait(20);
    QCOMPARE(BACKGROUND(w) , QColor("red"));
    QCOMPARE(BACKGROUND(*tw), QColor("red"));
    w.setStyleSheet("background: red;");
    QTest::qWait(20);
    QCOMPARE(BACKGROUND(w) , QColor("red"));
    QCOMPARE(BACKGROUND(*tw), QColor("red"));
}

void tst_QStyleSheetStyle::transparent()
{
    QWidget w;
    QPushButton *p1=new QPushButton(&w);
    QPushButton *p2=new QPushButton(&w);
    QPushButton *p3=new QPushButton(&w);
    p1->setStyleSheet("background:transparent");
    p2->setStyleSheet("background-color:transparent");
    p3->setStyleSheet("background:rgb(0,0,0,0)");
    QCOMPARE(BACKGROUND(*p1) , QColor(0,0,0,0));
    QCOMPARE(BACKGROUND(*p2) , QColor(0,0,0,0));
    QCOMPARE(BACKGROUND(*p3) , QColor(0,0,0,0));
}



class ProxyStyle : public QStyle
{
    public:
        ProxyStyle(QStyle *s)
        {
            style = s;
        }

        void drawControl(ControlElement ce, const QStyleOption *opt,
                         QPainter *painter, const QWidget *widget = 0) const;

        void drawPrimitive(QStyle::PrimitiveElement pe,
                           const QStyleOption* opt,
                           QPainter* p ,
                           const QWidget* w) const
        {
            style->drawPrimitive(pe, opt, p, w);
        }

        QRect subElementRect(QStyle::SubElement se,
                             const QStyleOption* opt,
                             const QWidget* w) const
        {
            Q_UNUSED(se);
            Q_UNUSED(opt);
            Q_UNUSED(w);
            return QRect(0, 0, 3, 3);
        }

        void drawComplexControl(QStyle::ComplexControl cc,
                                const QStyleOptionComplex* opt,
                                QPainter* p,
                                const QWidget* w) const
        {
            style->drawComplexControl(cc, opt, p, w);
        }


        SubControl hitTestComplexControl(QStyle::ComplexControl cc,
                                         const QStyleOptionComplex* opt,
                                         const QPoint& pt,
                                         const QWidget* w) const
        {
            return style->hitTestComplexControl(cc, opt, pt, w);
        }

        QRect subControlRect(QStyle::ComplexControl cc,
                             const QStyleOptionComplex* opt,
                             QStyle::SubControl sc,
                             const QWidget* w) const
        {
            return style->subControlRect(cc, opt, sc, w);
        }

        int pixelMetric(QStyle::PixelMetric pm,
                        const QStyleOption* opt,
                        const QWidget* w) const
        {
            return style->pixelMetric(pm, opt, w);
        }


        QSize sizeFromContents(QStyle::ContentsType ct,
                               const QStyleOption* opt,
                               const QSize& size,
                               const QWidget* w) const
        {
            return style->sizeFromContents(ct, opt, size, w);
        }

        int styleHint(QStyle::StyleHint sh,
                      const QStyleOption* opt,
                      const QWidget* w,
                      QStyleHintReturn* shr) const
        {
            return style->styleHint(sh, opt, w, shr);
        }

        QPixmap standardPixmap(QStyle::StandardPixmap spix,
                               const QStyleOption* opt,
                               const QWidget* w) const
        {
            return style->standardPixmap(spix, opt, w);
        }

        QPixmap generatedIconPixmap(QIcon::Mode mode,
                                    const QPixmap& pix,
                                    const QStyleOption* opt) const
        {
            return style->generatedIconPixmap(mode, pix, opt);
        }

    private:
        QStyle *style;
};

void ProxyStyle::drawControl(ControlElement ce, const QStyleOption *opt,
                             QPainter *painter, const QWidget *widget) const
{
    if(ce == CE_PushButton)
    {
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt))
        {
            QRect r = btn->rect;
            painter->fillRect(r, Qt::green);

            if(btn->state & QStyle::State_HasFocus)
                painter->fillRect(r.adjusted(5, 5, -5, -5), Qt::yellow);


            painter->drawText(r, Qt::AlignCenter, btn->text);
        }
    }
    else
    {
        style->drawControl(ce, opt, painter, widget);
    }
}

void tst_QStyleSheetStyle::proxyStyle()
{
    //Should not crash;   task 158984

    ProxyStyle *proxy = new ProxyStyle(qApp->style());
    QString styleSheet("QPushButton {background-color: red; }");

    QWidget *w = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(w);

    QPushButton *pb1 = new QPushButton(qApp->style()->objectName(), w);
    layout->addWidget(pb1);

    QPushButton *pb2 = new QPushButton("ProxyStyle", w);
    pb2->setStyle(proxy);
    layout->addWidget(pb2);

    QPushButton *pb3 = new QPushButton("StyleSheet", w);
    pb3->setStyleSheet(styleSheet);
    layout->addWidget(pb3);

    QPushButton *pb4 = new QPushButton("StyleSheet then ProxyStyle ", w);
    pb4->setStyleSheet(styleSheet);

    // We are creating our Proxy based on current style...
    // In this case it would be the QStyleSheetStyle that is deleted
    // later on. We need to get access to the "real" QStyle to be able to
    // draw correctly.
    ProxyStyle* newProxy = new ProxyStyle(qApp->style());
    pb4->setStyle(newProxy);

    layout->addWidget(pb4);

    QPushButton *pb5 = new QPushButton("ProxyStyle then StyleSheet ", w);
    pb5->setStyle(proxy);
    pb5->setStyleSheet(styleSheet);
    layout->addWidget(pb5);

    w->show();

    QTest::qWait(100);

    // Test for QTBUG-7198 - style sheet overrides custom element size
    QStyleOptionViewItem opt;
    opt.initFrom(w);
    opt.features |= QStyleOptionViewItem::HasCheckIndicator;
    QVERIFY(pb5->style()->subElementRect(QStyle::SE_ItemViewItemCheckIndicator,
            &opt, pb5).width() == 3);
    delete w;
    delete proxy;
    delete newProxy;
}

void tst_QStyleSheetStyle::dialogButtonBox()
{
    QDialogButtonBox box;
    box.addButton(QDialogButtonBox::Ok);
    box.addButton(QDialogButtonBox::Cancel);
    box.setStyleSheet("/** */ ");
    box.setStyleSheet(QString()); //should not crash
}

void tst_QStyleSheetStyle::emptyStyleSheet()
{
    //empty stylesheet should not change anything
    qApp->setStyleSheet(QString());
    QWidget w;
    QHBoxLayout layout(&w);
    w.setLayout(&layout);
    layout.addWidget(new QPushButton("push", &w));
    layout.addWidget(new QToolButton(&w));
    QLabel label("toto", &w);
    label.setFrameShape(QLabel::Panel);
    label.setFrameShadow(QLabel::Sunken);
    layout.addWidget(&label); //task 231137
    layout.addWidget(new QTableWidget(200,200, &w));
    layout.addWidget(new QProgressBar(&w));
    layout.addWidget(new QLineEdit(&w));
    layout.addWidget(new QSpinBox(&w));
    layout.addWidget(new QComboBox(&w));
    layout.addWidget(new QDateEdit(&w));
    layout.addWidget(new QGroupBox("some text", &w));

    w.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&w);
#endif
    //workaround the fact that the label sizehint is one pixel different the first time.
    label.setIndent(0); //force to recompute the sizeHint:
    w.setFocus();
    QTest::qWait(100);

    QImage img1(w.size(), QImage::Format_ARGB32);
    w.render(&img1);

    w.setStyleSheet("/* */");
    QTest::qWait(100);

    QImage img2(w.size(), QImage::Format_ARGB32);
    w.render(&img2);

    if(img1 != img2) {
        img1.save("emptyStyleSheet_img1.png");
        img2.save("emptyStyleSheet_img2.png");
    }

    QCOMPARE(img1,img2);
}

class ApplicationStyleSetter
{
public:
    explicit inline ApplicationStyleSetter(QStyle *s) : m_oldStyleName(QApplication::style()->objectName())
     { QApplication::setStyle(s); }
    inline ~ApplicationStyleSetter()
     { QApplication::setStyle(QStyleFactory::create(m_oldStyleName)); }

private:
    const QString m_oldStyleName;
};

void tst_QStyleSheetStyle::toolTip()
{
    qApp->setStyleSheet(QString());
    QWidget w;
#ifdef Q_OS_WIN32
    // Ensure plain "Windows" style to prevent the Vista style from clobbering the tooltip palette in polish().
    QStyle *windowsStyle = QStyleFactory::create(QLatin1String("windows"));
    QVERIFY(windowsStyle);
    ApplicationStyleSetter as(windowsStyle);
#endif // Q_OS_WIN32
    QHBoxLayout layout(&w);
    w.setLayout(&layout);

    QWidget *wid1 = new QGroupBox(&w);
    layout.addWidget(wid1);
    wid1->setStyleSheet("QToolTip { background: #ae2; }   #wid3 > QToolTip { background: #0b8; } ");
    QVBoxLayout *layout1 = new QVBoxLayout(wid1);
    wid1->setLayout(layout1);
    wid1->setToolTip("this is wid1");
    wid1->setObjectName("wid1");

    QWidget *wid2 = new QPushButton("wid2", wid1);
    layout1->addWidget(wid2);
    wid2->setStyleSheet("QToolTip { background: #f81; } ");
    wid2->setToolTip("this is wid2");
    wid2->setObjectName("wid2");

    QWidget *wid3 = new QPushButton("wid3", wid1);
    layout1->addWidget(wid3);
    wid3->setToolTip("this is wid3");
    wid3->setObjectName("wid3");

    QWidget *wid4 = new QPushButton("wid4", &w);
    layout.addWidget(wid4);
    wid4->setToolTip("this is wid4");
    wid4->setObjectName("wid4");

    w.show();
    QTest::qWait(100);

    const QColor normalToolTip = QToolTip::palette().color(QPalette::Inactive, QPalette::ToolTipBase);
    QList<QWidget *> widgets;
    QList<QColor> colors;


    //tooltip on the widget without stylesheet, then to othes widget, including one without stylesheet
    //(the tooltip will be reused but his colour must change)
    widgets << wid4          << wid1   << wid2   << wid3   << wid4;
    colors  << normalToolTip << "#ae2" << "#f81" << "#0b8" << normalToolTip;

    for (int i = 0; i < widgets.count() ; i++)
    {
        QWidget *wid = widgets.at(i);
        QColor col = colors.at(i);

        QToolTip::showText( QPoint(0,0) , "This is " + wid->objectName(), wid);

        QWidget *tooltip = 0;
        foreach (QWidget *widget, QApplication::topLevelWidgets()) {
            if (widget->inherits("QTipLabel") && widget->isVisible()) {
                tooltip = widget;
                break;
            }
        }
        QVERIFY(tooltip);
        QCOMPARE(tooltip->palette().color(tooltip->backgroundRole()), col);
    }

    QToolTip::showText( QPoint(0,0) , "This is " + wid3->objectName(), wid3);
    QTest::qWait(100);
    delete wid3; //should not crash;
    QTest::qWait(10);
    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        widget->update(); //should not crash either
    }
}

void tst_QStyleSheetStyle::embeddedFonts()
{
    //task 235622 and 210551
    QSpinBox spin;
    spin.show();
    spin.setStyleSheet("QSpinBox { font-size: 32px; }");
    QTest::qWait(20);
    QLineEdit *embedded = spin.findChild<QLineEdit *>();
    QVERIFY(embedded);
    QCOMPARE(spin.font().pixelSize(), 32);
    QCOMPARE(embedded->font().pixelSize(), 32);

    QMenu *menu = embedded->createStandardContextMenu();
    menu->show();
    QTest::qWait(20);
    QVERIFY(menu);
    QVERIFY(menu->font().pixelSize() != 32);
    QCOMPARE(menu->font().pixelSize(), qApp->font(menu).pixelSize());

    //task 242556
    QComboBox box;
    box.setEditable(true);
    box.addItems(QStringList() << "First" << "Second" << "Third");
    box.setStyleSheet("QComboBox { font-size: 32px; }");
    box.show();
    embedded = box.findChild<QLineEdit *>();
    QVERIFY(embedded);
    QTest::qWait(20);
    QCOMPARE(box.font().pixelSize(), 32);
    QCOMPARE(embedded->font().pixelSize(), 32);
}

void tst_QStyleSheetStyle::opaquePaintEvent_data()
{
    QTest::addColumn<QString>("stylesheet");
    QTest::addColumn<bool>("transparent");
    QTest::addColumn<bool>("styled");

    QTest::newRow("none") << QString::fromLatin1("/* */") << false << false;
    QTest::newRow("background black ") << QString::fromLatin1("background: black;") << false << true;
    QTest::newRow("background qrgba") << QString::fromLatin1("background: rgba(125,0,0,125);") << true << true;
    QTest::newRow("background transparent") << QString::fromLatin1("background: transparent;") << true << true;
    QTest::newRow("border native") << QString::fromLatin1("border: native;") << false << false;
    QTest::newRow("border solid") << QString::fromLatin1("border: 2px solid black;") << true << true;
    QTest::newRow("border transparent") << QString::fromLatin1("background: black; border: 2px solid rgba(125,0,0,125);") << true << true;
    QTest::newRow("margin") << QString::fromLatin1("margin: 25px;") << true << true;
    QTest::newRow("focus") << QString::fromLatin1("*:focus { background: rgba(125,0,0,125) }") << true << true;
}

void tst_QStyleSheetStyle::opaquePaintEvent()
{
    QFETCH(QString, stylesheet);
    QFETCH(bool, transparent);
    QFETCH(bool, styled);

    QWidget tl;
    QWidget cl(&tl);
    cl.setAttribute(Qt::WA_OpaquePaintEvent, true);
    cl.setAutoFillBackground(true);
    cl.setStyleSheet(stylesheet);
    cl.ensurePolished();
    QCOMPARE(cl.testAttribute(Qt::WA_OpaquePaintEvent), !transparent);
    QCOMPARE(cl.testAttribute(Qt::WA_StyledBackground), styled);
    QCOMPARE(cl.autoFillBackground(), !styled );
}

void tst_QStyleSheetStyle::complexWidgetFocus()
{
    // This test is a simplified version of the focusColors() test above.

    // Tests if colors can be changed by altering the focus of the widget.
    // To avoid messy pixel-by-pixel comparison, we assume that the goal
    // is reached if at least ten pixels of the right color can be found in
    // the image.
    // For this reason, we use unusual and extremely ugly colors! :-)

    QDialog frame;
    frame.setStyleSheet("*:focus { background: black; color: black } "
                        "QSpinBox::up-arrow:focus, QSpinBox::down-arrow:focus { width: 7px; height: 7px; background: #ff0084 } "
                        "QComboBox::down-arrow:focus { width: 7px; height: 7px; background: #ff0084 }"
                        "QSlider::handle:horizontal:focus { width: 7px; height: 7px; background: #ff0084 } ");

    QList<QWidget *> widgets;
    widgets << new QSpinBox;
    widgets << new QComboBox;
    widgets << new QSlider(Qt::Horizontal);

    QLayout* layout = new QGridLayout;
    layout->addWidget(new QLineEdit); // Avoids initial focus.
    foreach (QWidget *widget, widgets)
        layout->addWidget(widget);
    frame.setLayout(layout);

    frame.show();
    QTest::qWaitForWindowShown(&frame);
    QApplication::setActiveWindow(&frame);
    foreach (QWidget *widget, widgets) {
        widget->setFocus();
        QApplication::processEvents();

        QImage image(frame.width(), frame.height(), QImage::Format_ARGB32);
        frame.render(&image);
        if (image.depth() < 24) {
            QSKIP("Test doesn't support color depth < 24", SkipAll);
        }

        QVERIFY2(testForColors(image, QColor(0xff, 0x00, 0x84)),
                (QString::fromLatin1(widget->metaObject()->className())
                + " did not contain text color #ff0084, using style "
                + QString::fromLatin1(qApp->style()->metaObject()->className()))
                .toLocal8Bit().constData());
    }
}

void tst_QStyleSheetStyle::task188195_baseBackground()
{
    QTreeView tree;
    tree.setStyleSheet( "QTreeView:disabled { background-color:#ab1251; }" );
    tree.show();
    QTest::qWait(20);
    QImage image(tree.width(), tree.height(), QImage::Format_ARGB32);

    tree.render(&image);
    QVERIFY(testForColors(image, tree.palette().base().color()));
    QVERIFY(!testForColors(image, QColor(0xab, 0x12, 0x51)));

    tree.setEnabled(false);
    tree.render(&image);
    QVERIFY(testForColors(image, QColor(0xab, 0x12, 0x51)));

    tree.setEnabled(true);
    tree.render(&image);
    QVERIFY(testForColors(image, tree.palette().base().color()));
    QVERIFY(!testForColors(image, QColor(0xab, 0x12, 0x51)));

    QTableWidget table(12, 12);
    table.setItem(0, 0, new QTableWidgetItem());
    table.setStyleSheet( "QTableView {background-color: #ff0000}" );
    table.show();
    QTest::qWait(20);
    image = QImage(table.width(), table.height(), QImage::Format_ARGB32);
    table.render(&image);
    QVERIFY(testForColors(image, Qt::red, true));
}

void tst_QStyleSheetStyle::task232085_spinBoxLineEditBg()
{
    // This test is a simplified version of the focusColors() test above.

    // Tests if colors can be changed by altering the focus of the widget.
    // To avoid messy pixel-by-pixel comparison, we assume that the goal
    // is reached if at least ten pixels of the right color can be found in
    // the image.
    // For this reason, we use unusual and extremely ugly colors! :-)

    QSpinBox *spinbox = new QSpinBox;
    spinbox->setValue(8888);

    QDialog frame;
    QLayout* layout = new QGridLayout;

    QLineEdit* dummy = new QLineEdit; // Avoids initial focus.

    // We only want to test the line edit colors.
    spinbox->setStyleSheet("QSpinBox:focus { background: #e8ff66; color: #ff0084 } "
                           "QSpinBox::up-button, QSpinBox::down-button { background: black; }");

    layout->addWidget(dummy);
    layout->addWidget(spinbox);
    frame.setLayout(layout);

    frame.show();
    QTest::qWaitForWindowShown(&frame);
    QApplication::setActiveWindow(&frame);
    spinbox->setFocus();
    QApplication::processEvents();

    QImage image(frame.width(), frame.height(), QImage::Format_ARGB32);
    frame.render(&image);
    if (image.depth() < 24) {
        QSKIP("Test doesn't support color depth < 24", SkipAll);
    }

    QVERIFY2(testForColors(image, QColor(0xe8, 0xff, 0x66)),
            (QString::fromLatin1(spinbox->metaObject()->className())
            + " did not contain background color #e8ff66, using style "
            + QString::fromLatin1(qApp->style()->metaObject()->className()))
            .toLocal8Bit().constData());
    QVERIFY2(testForColors(image, QColor(0xff, 0x00, 0x84)),
            (QString::fromLatin1(spinbox->metaObject()->className())
            + " did not contain text color #ff0084, using style "
            + QString::fromLatin1(qApp->style()->metaObject()->className()))
            .toLocal8Bit().constData());
}

class ChangeEventWidget : public QWidget
{ public:
    void changeEvent(QEvent * event)
    {
        if(event->type() == QEvent::StyleChange) {
            static bool recurse = false;
            if (!recurse) {
                recurse = true;

#ifdef Q_OS_SYMBIAN
                QStyle *style = new QWindowsStyle();
#else
                QStyle *style = new QMotifStyle;
#endif
                style->setParent(this);
                setStyle(style);
                recurse = false;
            }
        }
        QWidget::changeEvent(event);
    }
};

void tst_QStyleSheetStyle::changeStyleInChangeEvent()
{   //must not crash;
    ChangeEventWidget wid;
    wid.ensurePolished();
    wid.setStyleSheet(" /* */ ");
    wid.ensurePolished();
    wid.setStyleSheet(" /* ** */ ");
    wid.ensurePolished();
}

void tst_QStyleSheetStyle::QTBUG11658_cachecrash()
{
    //should not crash
    class Widget : public QWidget
    {
    public:
        Widget(QWidget *parent = 0)
        : QWidget(parent)
        {
            QVBoxLayout* pLayout = new QVBoxLayout(this);
            QCheckBox* pCheckBox = new QCheckBox(this);
            pLayout->addWidget(pCheckBox);
            setLayout(pLayout);

            QString szStyleSheet = QLatin1String("* { color: red; }");
            qApp->setStyleSheet(szStyleSheet);
            qApp->setStyle(QStyleFactory::create(QLatin1String("Windows")));
        }
    };

    Widget *w = new Widget();
    delete w;
    w = new Widget();
    w->show();

    QTest::qWaitForWindowShown(w);
    delete w;
    qApp->setStyleSheet(QString());
}

void tst_QStyleSheetStyle::QTBUG15910_crashNullWidget()
{
    struct : QWidget {
        virtual void paintEvent(QPaintEvent* ) {
            QStyleOption opt;
            opt.init(this);
            QPainter p(this);
            style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, 0);
            style()->drawPrimitive(QStyle::PE_Frame, &opt, &p, 0);
            style()->drawControl(QStyle::CE_PushButton, &opt, &p, 0);
        }
    } w;
    w.setStyleSheet("* { background-color: white; color:black; border 3px solid yellow }");
    w.show();
    QTest::qWaitForWindowShown(&w);
}


void tst_QStyleSheetStyle::styleSheetChangeBeforePolish()
{
    QWidget widget;
    QVBoxLayout *vbox = new QVBoxLayout(&widget);
    QFrame *frame = new QFrame(&widget);
    frame->setFixedSize(200, 200);
    frame->setStyleSheet("background-color: #FF0000;");
    frame->setStyleSheet("background-color: #00FF00;");
    vbox->addWidget(frame);
    QFrame *frame2 = new QFrame(&widget);
    frame2->setFixedSize(200, 200);
    frame2->setStyleSheet("background-color: #FF0000;");
    frame2->setStyleSheet("background-color: #00FF00;");
    vbox->addWidget(frame);
    widget.show();
    QVERIFY(QTest::qWaitForWindowExposed(&widget));
    QImage image(frame->size(), QImage::Format_ARGB32);
    frame->render(&image);
    QVERIFY(testForColors(image, QColor(0x00, 0xFF, 0x00)));
    QImage image2(frame2->size(), QImage::Format_ARGB32);
    frame2->render(&image2);
    QVERIFY(testForColors(image2, QColor(0x00, 0xFF, 0x00)));
}

QTEST_MAIN(tst_QStyleSheetStyle)
#include "tst_qstylesheetstyle.moc"


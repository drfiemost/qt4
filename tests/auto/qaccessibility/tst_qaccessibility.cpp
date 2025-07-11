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
#ifndef Q_OS_WINCE
#include <QtGui>
#include <math.h>


#include "QtTest/qtestaccessible.h"

// Make a widget frameless to prevent size constraints of title bars
// from interfering (Windows).
static inline void setFrameless(QWidget *w)
{
    Qt::WindowFlags flags = w->windowFlags();
    flags |= Qt::FramelessWindowHint;
    flags &= ~(Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    w->setWindowFlags(flags);
}

#if defined(Q_OS_WINCE)
extern "C" bool SystemParametersInfo(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni);
#define SPI_GETPLATFORMTYPE 257
inline bool IsValidCEPlatform() {
    wchar_t tszPlatform[64];
    if (SystemParametersInfo(SPI_GETPLATFORMTYPE, sizeof(tszPlatform) / sizeof(*tszPlatform), tszPlatform, 0)) {
        QString platform = QString::fromWCharArray(tszPlatform);
        if ((platform == QLatin1String("PocketPC")) || (platform == QLatin1String("Smartphone")))
            return false;
    }
    return true;
}
#endif

static inline bool verifyChild(QWidget *child, QAccessibleInterface *interface,
                               int index, const QRect &domain)
{
    if (!child) {
        qWarning("tst_QAccessibility::verifyChild: null pointer to child.");
        return false;
    }

    if (!interface) {
        qWarning("tst_QAccessibility::verifyChild: null pointer to interface.");
        return false;
    }

    // QAccessibleInterface::childAt():
    // Calculate global child position and check that the interface
    // returns the correct index for that position.
    QPoint globalChildPos = child->mapToGlobal(QPoint(0, 0));
    int indexFromChildAt = interface->childAt(globalChildPos.x(), globalChildPos.y());
    if (indexFromChildAt != index) {
        qWarning("tst_QAccessibility::verifyChild (childAt()):");
        qWarning() << "Expected:" << index;
        qWarning() << "Actual:  " << indexFromChildAt;
        return false;
    }

    // QAccessibleInterface::rect():
    // Calculate global child geometry and check that the interface
    // returns a QRect which is equal to the calculated QRect.
    const QRect expectedGlobalRect = QRect(globalChildPos, child->size());
    const QRect rectFromInterface = interface->rect(index);
    if (expectedGlobalRect != rectFromInterface) {
        qWarning("tst_QAccessibility::verifyChild (rect()):");
        qWarning() << "Expected:" << expectedGlobalRect;
        qWarning() << "Actual:  " << rectFromInterface;
        return false;
    }

    // Verify that the child is within its domain.
    if (!domain.contains(rectFromInterface)) {
        qWarning("tst_QAccessibility::verifyChild: Child is not within its domain.");
        return false;
    }

    // Verify that we get a valid QAccessibleInterface for the child.
    QAccessibleInterface *childInterface = QAccessible::queryAccessibleInterface(child);
    if (!childInterface) {
        qWarning("tst_QAccessibility::verifyChild: Failed to retrieve interface for child.");
        return false;
    }

    // QAccessibleInterface::indexOfChild():
    // Verify that indexOfChild() returns an index equal to the index passed by,
    // or -1 if child is "Self" (index == 0).
    int indexFromIndexOfChild = interface->indexOfChild(childInterface);
    delete childInterface;
    int expectedIndex = index == 0 ? -1 : index;
    if (indexFromIndexOfChild != expectedIndex) {
        qWarning("tst_QAccessibility::verifyChild (indexOfChild()):");
        qWarning() << "Expected:" << expectedIndex;
        qWarning() << "Actual:  " << indexFromIndexOfChild;
        return false;
    }

    // Navigate to child, compare its object and role with the interface from queryAccessibleInterface(child).
    {
        QAccessibleInterface *navigatedChildInterface = 0;
        const int status = interface->navigate(QAccessible::Child, index, &navigatedChildInterface);
        // We are navigating to a separate widget/interface, so status should be 0.
        if (status != 0)
            return false;

        if (navigatedChildInterface == 0)
            return false;
        delete navigatedChildInterface;
    }

    return true;
}

static inline int indexOfChild(QAccessibleInterface *parentInterface, QWidget *childWidget)
{
    if (!parentInterface || !childWidget)
        return -1;
    QAccessibleInterface *childInterface = QAccessibleInterface::queryAccessibleInterface(childWidget);
    if (!childInterface)
        return -1;
    int index = parentInterface->indexOfChild(childInterface);
    delete childInterface;
    return index;
}

#define EXPECT(cond) \
    do { \
        if (!errorAt && !(cond)) { \
            errorAt = __LINE__; \
            qWarning("level: %d, middle: %d, role: %d (%s)", treelevel, middle, iface->role(0), #cond); \
        } \
    } while (0)

static int verifyHierarchy(QAccessibleInterface *iface)
{
    int errorAt = 0;
    int entry = 0;
    static int treelevel = 0;   // for error diagnostics
    QAccessibleInterface *middleChild, *if2, *if3;
    middleChild = 0;
    ++treelevel;
    int middle = iface->childCount()/2 + 1;
    if (iface->childCount() >= 2) {
        entry = iface->navigate(QAccessible::Child, middle, &middleChild);
    }
    for (int i = 0; i < iface->childCount() && !errorAt; ++i) {
        entry = iface->navigate(QAccessible::Child, i + 1, &if2);
        if (entry == 0) {
            // navigate Ancestor...
            QAccessibleInterface *parent = 0;
            entry = if2->navigate(QAccessible::Ancestor, 1, &parent);
            EXPECT(entry == 0 && iface->object() == parent->object());
            delete parent;

            // navigate Sibling...
            if (middleChild) {
                entry = if2->navigate(QAccessible::Sibling, middle, &if3);
                EXPECT(entry == 0 && if3->object() == middleChild->object());
                if (entry == 0)
                    delete if3;
                EXPECT(iface->indexOfChild(middleChild) == middle);
            }

            // verify children...
            if (!errorAt)
                errorAt = verifyHierarchy(if2);
            delete if2;
        } else {
            // leaf nodes
        }
    }
    delete middleChild;

    --treelevel;
    return errorAt;
}


//TESTED_FILES=

class tst_QAccessibility : public QObject
{
    Q_OBJECT
public:
    tst_QAccessibility();
    virtual ~tst_QAccessibility();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:
    void eventTest();
    void customWidget();
    void deletedWidget();

    void navigateGeometric();
    void navigateHierarchy();
    void navigateSlider();
    void navigateCovered();
    void textAttributes();
    void hideShowTest();

    void userActionCount();
    void actionText();
    void doAction();

    void applicationTest();
    void mainWindowTest();
    void buttonTest();
    void scrollBarTest();
    void tabTest();
    void tabWidgetTest();
    void menuTest();
    void spinBoxTest();
    void doubleSpinBoxTest();
    void textEditTest();
    void textBrowserTest();
    void listViewTest();
    void mdiAreaTest();
    void mdiSubWindowTest();
    void lineEditTest();
    void groupBoxTest();
    void workspaceTest();
    void dialogButtonBoxTest();
    void dialTest();
    void rubberBandTest();
    void abstractScrollAreaTest();
    void scrollAreaTest();
    void tableWidgetTest();
    void tableViewTest();
    void table2ListTest();
    void table2TreeTest();
    void table2TableTest();
    void calendarWidgetTest();
    void dockWidgetTest();
    void comboBoxTest();
    void accessibleName();
    void treeWidgetTest();
    void labelTest();
    void accelerators();
};

const double Q_PI = 3.14159265358979323846;

QString eventName(const int ev)
{
    switch(ev) {
    case 0x0001: return "SoundPlayed";
    case 0x0002: return "Alert";
    case 0x0003: return "ForegroundChanged";
    case 0x0004: return "MenuStart";
    case 0x0005: return "MenuEnd";
    case 0x0006: return "PopupMenuStart";
    case 0x0007: return "PopupMenuEnd";
    case 0x000C: return "ContextHelpStart";
    case 0x000D: return "ContextHelpEnd";
    case 0x000E: return "DragDropStart";
    case 0x000F: return "DragDropEnd";
    case 0x0010: return "DialogStart";
    case 0x0011: return "DialogEnd";
    case 0x0012: return "ScrollingStart";
    case 0x0013: return "ScrollingEnd";
    case 0x0018: return "MenuCommand";

    case 0x0116: return "TableModelChanged";
    case 0x011B: return "TextCaretMoved";

    case 0x8000: return "ObjectCreated";
    case 0x8001: return "ObjectDestroyed";
    case 0x8002: return "ObjectShow";
    case 0x8003: return "ObjectHide";
    case 0x8004: return "ObjectReorder";
    case 0x8005: return "Focus";
    case 0x8006: return "Selection";
    case 0x8007: return "SelectionAdd";
    case 0x8008: return "SelectionRemove";
    case 0x8009: return "SelectionWithin";
    case 0x800A: return "StateChanged";
    case 0x800B: return "LocationChanged";
    case 0x800C: return "NameChanged";
    case 0x800D: return "DescriptionChanged";
    case 0x800E: return "ValueChanged";
    case 0x800F: return "ParentChanged";
    case 0x80A0: return "HelpChanged";
    case 0x80B0: return "DefaultActionChanged";
    case 0x80C0: return "AcceleratorChanged";
    default: return "Unknown Event";
    }
}

QAccessible::State state(QWidget * const widget)
{
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(widget);
    if (!iface)
        qWarning() << "Cannot get QAccessibleInterface for widget";
    QAccessible::State state = (iface ? iface->state(0) : static_cast<QAccessible::State>(0));
    delete iface;
    return state;
}

class QtTestAccessibleWidget: public QWidget
{
    Q_OBJECT
public:
    QtTestAccessibleWidget(QWidget *parent, const char *name): QWidget(parent)
    {
        setObjectName(name);
        QPalette pal;
        pal.setColor(backgroundRole(), Qt::black);//black is beautiful
        setPalette(pal);
        setFixedSize(5, 5);
    }
};

class QtTestAccessibleWidgetIface: public QAccessibleWidget
{
public:
    QtTestAccessibleWidgetIface(QtTestAccessibleWidget *w): QAccessibleWidget(w) {}
    QString text(Text t, int control) const
    {
        if (t == Help)
            return QString::fromLatin1("Help yourself");
        return QAccessibleWidget::text(t, control);
    }
    static QAccessibleInterface *ifaceFactory(const QString &key, QObject *o)
    {
        if (key == "QtTestAccessibleWidget")
            return new QtTestAccessibleWidgetIface(static_cast<QtTestAccessibleWidget*>(o));
        return 0;
    }
};

tst_QAccessibility::tst_QAccessibility()
{
}

tst_QAccessibility::~tst_QAccessibility()
{
}

void tst_QAccessibility::initTestCase()
{
    QTestAccessibility::initialize();
    QAccessible::installFactory(QtTestAccessibleWidgetIface::ifaceFactory);
}

void tst_QAccessibility::cleanupTestCase()
{
    QTestAccessibility::cleanup();
}

void tst_QAccessibility::init()
{
    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::cleanup()
{
    const EventList list = QTestAccessibility::events();
    if (!list.isEmpty()) {
        qWarning("%d accessibility event(s) were not handled in testfunction '%s':", list.count(),
                 QString(QTest::currentTestFunction()).toAscii().constData());
        for (int i = 0; i < list.count(); ++i)
            qWarning(" %d: Object: %p Event: '%s' (%d) Child: %d", i + 1, list.at(i).object,
                     eventName(list.at(i).event).toAscii().constData(), list.at(i).event, list.at(i).child);
    }
    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::eventTest()
{
    QPushButton* button = new QPushButton(0);
    button->setObjectName(QString("Olaf"));
    setFrameless(button);

    button->show();
    QVERIFY_EVENT(button, 0, QAccessible::ObjectShow);
    button->setFocus(Qt::MouseFocusReason);
    QTestAccessibility::clearEvents();
    QTest::mouseClick(button, Qt::LeftButton, 0);
    QVERIFY_EVENT(button, 0, QAccessible::StateChanged);
    QVERIFY_EVENT(button, 0, QAccessible::StateChanged);

    button->setAccessibleName("Olaf the second");
    QVERIFY_EVENT(button, 0, QAccessible::NameChanged);
    button->setAccessibleDescription("This is a button labeled Olaf");
    QVERIFY_EVENT(button, 0, QAccessible::DescriptionChanged);

    button->hide();
    QVERIFY_EVENT(button, 0, QAccessible::ObjectHide);

    // Destroy a visible widget
    QTestAccessibility::clearEvents();
    button->show();
    QVERIFY_EVENT(button, 0, QAccessible::ObjectShow);

    delete button;

    QVERIFY_EVENT(button, 0, QAccessible::ObjectHide);
    QVERIFY_EVENT(button, 0, QAccessible::ObjectDestroyed);
}

void tst_QAccessibility::customWidget()
{
    QtTestAccessibleWidget* widget = new QtTestAccessibleWidget(0, "Heinz");

    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(widget);
    QVERIFY(iface != 0);
    QVERIFY(iface->isValid());
    QCOMPARE(iface->object(), (QObject*)widget);
    QCOMPARE(iface->object()->objectName(), QString("Heinz"));
    QCOMPARE(iface->text(QAccessible::Help, 0), QString("Help yourself"));

    delete iface;
    delete widget;
}

void tst_QAccessibility::deletedWidget()
{
    QtTestAccessibleWidget *widget = new QtTestAccessibleWidget(0, "Ralf");
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(widget);
    QVERIFY(iface != 0);
    QVERIFY(iface->isValid());
    QCOMPARE(iface->object(), (QObject*)widget);

    delete widget;
    widget = 0;
    QVERIFY(!iface->isValid());
    delete iface;
}

void tst_QAccessibility::navigateGeometric()
{
    {
    static const int skip = 20; //speed the test up significantly
    static const double step = Q_PI / 180;
    QWidget *w = new QWidget(0);
    w->setObjectName(QString("Josef"));
    w->setFixedSize(400, 400);

    // center widget
    QtTestAccessibleWidget *center = new QtTestAccessibleWidget(w, "Sol");
    center->move(200, 200);

    // arrange 360 widgets around it in a circle
    QtTestAccessibleWidget *aw = 0;
    int i;
    for (i = 0; i < 360; i += skip) {
        aw = new QtTestAccessibleWidget(w, QString::number(i).toLatin1());
        aw->move( int(200.0 + 100.0 * sin(step * (double)i)), int(200.0 + 100.0 * cos(step * (double)i)) );
    }

    aw = new QtTestAccessibleWidget(w, "Earth");
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(center);
    QAccessibleInterface *target = 0;
    QVERIFY(iface != 0);
    QVERIFY(iface->isValid());

    w->show();
#if defined(Q_WS_X11)
    qt_x11_wait_for_window_manager(w);
    QTest::qWait(100);
#endif

    // let one widget rotate around center
    for (i = 0; i < 360; i+=skip) {
        aw->move( int(200.0 + 75.0 * sin(step * (double)i)), int(200.0 + 75.0 * cos(step * (double)i)) );

        if (i < 45 || i > 315) {
            QCOMPARE(iface->navigate(QAccessible::Down, 0, &target), 0);
        } else if ( i < 135 ) {
            QCOMPARE(iface->navigate(QAccessible::Right, 0, &target), 0);
        } else if ( i < 225 ) {
            QCOMPARE(iface->navigate(QAccessible::Up, 0, &target), 0);
        } else {
            QCOMPARE(iface->navigate(QAccessible::Left, 0, &target), 0);
        }

        QVERIFY(target);
        QVERIFY(target->isValid());
        QVERIFY(target->object());
        QCOMPARE(target->object()->objectName(), aw->objectName());
        delete target; target = 0;
    }

    // test invisible widget
    target = QAccessible::queryAccessibleInterface(aw);
    QVERIFY(!(target->state(0) & QAccessible::Invisible));
    aw->hide();
    QVERIFY(target->state(0) & QAccessible::Invisible);
    delete target; target = 0;

    aw->move(center->x() + 10, center->y());
    QCOMPARE(iface->navigate(QAccessible::Right, 0, &target), 0);
    QVERIFY(target);
    QVERIFY(target->isValid());
    QVERIFY(target->object());
    QVERIFY(QString(target->object()->objectName()) != "Earth");
    delete target; target = 0;

    aw->move(center->x() - 10, center->y());
    QCOMPARE(iface->navigate(QAccessible::Left, 0, &target), 0);
    QVERIFY(target);
    QVERIFY(target->isValid());
    QVERIFY(target->object());
    QVERIFY(QString(target->object()->objectName()) != "Earth");
    delete target; target = 0;

    aw->move(center->x(), center->y() + 10);
    QCOMPARE(iface->navigate(QAccessible::Down, 0, &target), 0);
    QVERIFY(target);
    QVERIFY(target->isValid());
    QVERIFY(target->object());
    QVERIFY(QString(target->object()->objectName()) != "Earth");
    delete target; target = 0;

    aw->move(center->x(), center->y() - 10);
    QCOMPARE(iface->navigate(QAccessible::Up, 0, &target), 0);
    QVERIFY(target);
    QVERIFY(target->isValid());
    QVERIFY(target->object());
    QVERIFY(QString(target->object()->objectName()) != "Earth");
    delete target; target = 0;

    delete iface;
    delete w;
    }
    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::navigateSlider()
{
    {
    QSlider *slider = new QSlider(0);
    setFrameless(slider);
    slider->setObjectName(QString("Slidy"));
    slider->show();
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(slider);
    QAccessibleInterface *target = 0;
    QVERIFY(iface != 0);
    QVERIFY(iface->isValid());
    QCOMPARE(iface->childCount(), 3);
    QCOMPARE(iface->navigate(QAccessible::Child, 1, &target), 1);
    QVERIFY(target == 0);
    QCOMPARE(iface->navigate(QAccessible::Child, 2, &target), 2);
    QVERIFY(target == 0);
    QCOMPARE(iface->navigate(QAccessible::Child, 3, &target), 3);
    QVERIFY(target == 0);
    QCOMPARE(iface->navigate(QAccessible::Child, 4, &target), -1);
    QVERIFY(target == 0);
    QCOMPARE(iface->navigate(QAccessible::Child, 0, &target), -1);
    QVERIFY(target == 0);
    QCOMPARE(iface->navigate(QAccessible::Child, -42, &target), -1);
    QVERIFY(target == 0);

    delete iface;
    delete slider;
    }
    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::navigateCovered()
{
    {
    QWidget *w = new QWidget(0);
    w->setObjectName(QString("Harry"));
    QWidget *w1 = new QWidget(w);
    w1->setObjectName(QString("1"));
    QWidget *w2 = new QWidget(w);
    w2->setObjectName(QString("2"));
    w->show();
#if defined(Q_WS_X11)
    qt_x11_wait_for_window_manager(w);
    QTest::qWait(100);
#endif

    w->setFixedSize(6, 6);
    w1->setFixedSize(5, 5);
    w2->setFixedSize(5, 5);
    w2->move(0, 0);
    w1->raise();

    QAccessibleInterface *iface1 = QAccessible::queryAccessibleInterface(w1);
    QVERIFY(iface1 != 0);
    QVERIFY(iface1->isValid());
    QAccessibleInterface *iface2 = QAccessible::queryAccessibleInterface(w2);
    QVERIFY(iface2 != 0);
    QVERIFY(iface2->isValid());
    QAccessibleInterface *iface3 = 0;

    QCOMPARE(iface1->navigate(QAccessible::Covers, -42, &iface3), -1);
    QVERIFY(iface3 == 0);
    QCOMPARE(iface1->navigate(QAccessible::Covers, 0, &iface3), -1);
    QVERIFY(iface3 == 0);
    QCOMPARE(iface1->navigate(QAccessible::Covers, 2, &iface3), -1);
    QVERIFY(iface3 == 0);

    for (int loop = 0; loop < 2; ++loop) {
        for (int x = 0; x < w->width(); ++x) {
            for (int y = 0; y < w->height(); ++y) {
                w1->move(x, y);
                if (w1->geometry().intersects(w2->geometry())) {
                    QVERIFY(iface1->relationTo(0, iface2, 0) & QAccessible::Covers);
                    QVERIFY(iface2->relationTo(0, iface1, 0) & QAccessible::Covered);
                    QCOMPARE(iface1->navigate(QAccessible::Covered, 1, &iface3), 0);
                    QVERIFY(iface3 != 0);
                    QVERIFY(iface3->isValid());
                    QCOMPARE(iface3->object(), iface2->object());
                    delete iface3; iface3 = 0;
                    QCOMPARE(iface2->navigate(QAccessible::Covers, 1, &iface3), 0);
                    QVERIFY(iface3 != 0);
                    QVERIFY(iface3->isValid());
                    QCOMPARE(iface3->object(), iface1->object());
                    delete iface3; iface3 = 0;
                } else {
                    QVERIFY(!(iface1->relationTo(0, iface2, 0) & QAccessible::Covers));
                    QVERIFY(!(iface2->relationTo(0, iface1, 0) & QAccessible::Covered));
                    QCOMPARE(iface1->navigate(QAccessible::Covered, 1, &iface3), -1);
                    QVERIFY(iface3 == 0);
                    QCOMPARE(iface1->navigate(QAccessible::Covers, 1, &iface3), -1);
                    QVERIFY(iface3 == 0);
                    QCOMPARE(iface2->navigate(QAccessible::Covered, 1, &iface3), -1);
                    QVERIFY(iface3 == 0);
                    QCOMPARE(iface2->navigate(QAccessible::Covers, 1, &iface3), -1);
                    QVERIFY(iface3 == 0);
                }
            }
        }
        if (!loop) {
            // switch children for second loop
            w2->raise();
            QAccessibleInterface *temp = iface1;
            iface1 = iface2;
            iface2 = temp;
        }
    }
    delete iface1; iface1 = 0;
    delete iface2; iface2 = 0;
    iface1 = QAccessible::queryAccessibleInterface(w1);
    QVERIFY(iface1 != 0);
    QVERIFY(iface1->isValid());
    iface2 = QAccessible::queryAccessibleInterface(w2);
    QVERIFY(iface2 != 0);
    QVERIFY(iface2->isValid());

    w1->move(0,0);
    w2->move(0,0);
    w1->raise();
    QVERIFY(iface1->relationTo(0, iface2, 0) & QAccessible::Covers);
    QVERIFY(iface2->relationTo(0, iface1, 0) & QAccessible::Covered);
    QVERIFY(!(iface1->state(0) & QAccessible::Invisible));
    w1->hide();
    QVERIFY(iface1->state(0) & QAccessible::Invisible);
    QVERIFY(!(iface1->relationTo(0, iface2, 0) & QAccessible::Covers));
    QVERIFY(!(iface2->relationTo(0, iface1, 0) & QAccessible::Covered));
    QCOMPARE(iface2->navigate(QAccessible::Covered, 1, &iface3), -1);
    QVERIFY(iface3 == 0);
    QCOMPARE(iface1->navigate(QAccessible::Covers, 1, &iface3), -1);
    QVERIFY(iface3 == 0);

    delete iface1; iface1 = 0;
    delete iface2; iface2 = 0;
    delete w;
    }
    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::navigateHierarchy()
{
    {
    QWidget *w = new QWidget(0);
    w->setObjectName(QString("Hans"));
    w->show();
    QWidget *w1 = new QWidget(w);
    w1->setObjectName(QString("1"));
    w1->show();
    QWidget *w2 = new QWidget(w);
    w2->setObjectName(QString("2"));
    w2->show();
    QWidget *w3 = new QWidget(w);
    w3->setObjectName(QString("3"));
    w3->show();
    QWidget *w31 = new QWidget(w3);
    w31->setObjectName(QString("31"));
    w31->show();

    QAccessibleInterface *target = 0;
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(w);
    QVERIFY(iface != 0);
    QVERIFY(iface->isValid());

    QCOMPARE(iface->navigate(QAccessible::Sibling, -42, &target), -1);
    QVERIFY(target == 0);
    QCOMPARE(iface->navigate(QAccessible::Sibling, 42, &target), -1);
    QVERIFY(target == 0);
    QCOMPARE(iface->navigate(QAccessible::Child, 15, &target), -1);
    QVERIFY(target == 0);
    QCOMPARE(iface->navigate(QAccessible::Child, 0, &target), -1);
    QVERIFY(target == 0);
    QCOMPARE(iface->navigate(QAccessible::Child, 1, &target), 0);
    QVERIFY(target != 0);
    QVERIFY(target->isValid());
    QCOMPARE(target->object(), (QObject*)w1);
    delete iface; iface = 0;

    QCOMPARE(target->navigate(QAccessible::Sibling, 0, &iface), -1);
    QVERIFY(iface == 0);
    QCOMPARE(target->navigate(QAccessible::Sibling, 42, &iface), -1);
    QVERIFY(iface == 0);
    QCOMPARE(target->navigate(QAccessible::Sibling, -42, &iface), -1);
    QVERIFY(iface == 0);
    QCOMPARE(target->navigate(QAccessible::Sibling, 2, &iface), 0);
    QVERIFY(iface != 0);
    QVERIFY(iface->isValid());
    QCOMPARE(iface->object(), (QObject*)w2);
    delete target; target = 0;
    QCOMPARE(iface->navigate(QAccessible::Sibling, 3, &target), 0);
    QVERIFY(target != 0);
    QVERIFY(target->isValid());
    QCOMPARE(target->object(), (QObject*)w3);
    delete iface; iface = 0;

    QCOMPARE(target->navigate(QAccessible::Child, 1, &iface), 0);
    QVERIFY(iface != 0);
    QVERIFY(iface->isValid());
    QCOMPARE(iface->object(), (QObject*)w31);
    delete target; target = 0;

    QCOMPARE(iface->navigate(QAccessible::Sibling, -1, &target), -1);
    QVERIFY(target == 0);
    QCOMPARE(iface->navigate(QAccessible::Sibling, 0, &target), -1);
    QVERIFY(target == 0);
    QCOMPARE(iface->navigate(QAccessible::Sibling, 1, &target), 0);
    QVERIFY(target != 0);
    QVERIFY(target->isValid());
    QCOMPARE(target->object(), (QObject*)w31);
    delete iface; iface = 0;

    QCOMPARE(target->navigate(QAccessible::Ancestor, 42, &iface), -1);
    QVERIFY(iface == 0);
    QCOMPARE(target->navigate(QAccessible::Ancestor, -1, &iface), -1);
    QVERIFY(iface == 0);
    QCOMPARE(target->navigate(QAccessible::Ancestor, 0, &iface), -1);
    QVERIFY(iface == 0);
    QCOMPARE(target->navigate(QAccessible::Ancestor, 1, &iface), 0);
    QVERIFY(iface != 0);
    QVERIFY(iface->isValid());
    QCOMPARE(iface->object(), (QObject*)w3);
    delete iface; iface = 0;
    QCOMPARE(target->navigate(QAccessible::Ancestor, 2, &iface), 0);
    QVERIFY(iface != 0);
    QVERIFY(iface->isValid());
    QCOMPARE(iface->object(), (QObject*)w);
    delete iface; iface = 0;
    QCOMPARE(target->navigate(QAccessible::Ancestor, 3, &iface), 0);
    QVERIFY(iface != 0);
    QVERIFY(iface->isValid());
    QCOMPARE(iface->object(), (QObject*)qApp);
    delete iface; iface = 0;
    delete target; target = 0;

    delete w;
    }
    QTestAccessibility::clearEvents();
}

#define QSETCOMPARE(thetypename, elements, otherelements) \
    QCOMPARE((QSet<thetypename>() << elements), (QSet<thetypename>() << otherelements))

static QWidget *createWidgets()
{
    QWidget *w = new QWidget();

    QHBoxLayout *box = new QHBoxLayout(w);

    int i = 0;
    box->addWidget(new QComboBox(w));
    box->addWidget(new QPushButton(QString::fromAscii("widget text %1").arg(i++), w));
    box->addWidget(new QHeaderView(Qt::Vertical, w));
    box->addWidget(new QTreeView(w));
    box->addWidget(new QTreeWidget(w));
    box->addWidget(new QListView(w));
    box->addWidget(new QListWidget(w));
    box->addWidget(new QTableView(w));
    box->addWidget(new QTableWidget(w));
    box->addWidget(new QCalendarWidget(w));
    box->addWidget(new QDialogButtonBox(w));
    box->addWidget(new QGroupBox(QString::fromAscii("widget text %1").arg(i++), w));
    box->addWidget(new QFrame(w));
    box->addWidget(new QLineEdit(QString::fromAscii("widget text %1").arg(i++), w));
    box->addWidget(new QProgressBar(w));
    box->addWidget(new QTabWidget(w));
    box->addWidget(new QCheckBox(QString::fromAscii("widget text %1").arg(i++), w));
    box->addWidget(new QRadioButton(QString::fromAscii("widget text %1").arg(i++), w));
    box->addWidget(new QDial(w));
    box->addWidget(new QScrollBar(w));
    box->addWidget(new QSlider(w));
    box->addWidget(new QDateTimeEdit(w));
    box->addWidget(new QDoubleSpinBox(w));
    box->addWidget(new QSpinBox(w));
    box->addWidget(new QLabel(QString::fromAscii("widget text %1").arg(i++), w));
    box->addWidget(new QLCDNumber(w));
    box->addWidget(new QStackedWidget(w));
    box->addWidget(new QToolBox(w));
    box->addWidget(new QLabel(QString::fromAscii("widget text %1").arg(i++), w));
    box->addWidget(new QTextEdit(QString::fromAscii("widget text %1").arg(i++), w));

    /* Not in the list
     * QAbstractItemView, QGraphicsView, QScrollArea,
     * QToolButton, QDockWidget, QFocusFrame, QMainWindow, QMenu, QMenuBar, QSizeGrip, QSplashScreen, QSplitterHandle,
     * QStatusBar, QSvgWidget, QTabBar, QToolBar, QWorkspace, QSplitter
     */
    return w;
}

void tst_QAccessibility::accessibleName()
{
    QWidget *toplevel = createWidgets();
    toplevel->show();
#if defined(Q_WS_X11)
    qt_x11_wait_for_window_manager(toplevel);
    QTest::qWait(100);
#endif
    QLayout *lout = toplevel->layout();
    for (int i = 0; i < lout->count(); i++) {
        QLayoutItem *item = lout->itemAt(i);
        QWidget *child = item->widget();

        QString name = tr("Widget Name %1").arg(i);
        child->setAccessibleName(name);
        QAccessibleInterface *acc = QAccessible::queryAccessibleInterface(child);
        QCOMPARE(acc->text(QAccessible::Name, 0), name);

        QString desc = tr("Widget Description %1").arg(i);
        child->setAccessibleDescription(desc);
        QCOMPARE(acc->text(QAccessible::Description, 0), desc);

    }

    delete toplevel;
    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::textAttributes()
{
    QTextEdit textEdit;
    int startOffset;
    int endOffset;
    QString attributes;
    QString text("<html><head></head><body>"
                 "Hello, <b>this</b> is an <i><b>example</b> text</i>."
                 "<span style=\"font-family: monospace\">Multiple fonts are used.</span>"
                 "Multiple <span style=\"font-size: 8pt\">text sizes</span> are used."
                 "Let's give some color to <span style=\"color:#f0f1f2; background-color:#14f01e\">Qt</span>."
                 "</body></html>");

    textEdit.setText(text);
    QAccessibleInterface *interface = QAccessible::queryAccessibleInterface(&textEdit);

    QAccessibleTextInterface *textInterface=interface->textInterface();

    QVERIFY(textInterface);
    QCOMPARE(textInterface->characterCount(), 112);

    attributes = textInterface->attributes(10, &startOffset, &endOffset);
    QCOMPARE(startOffset, 7);
    QCOMPARE(endOffset, 11);
    attributes.prepend(';');
    QVERIFY(attributes.contains(QLatin1String(";font-weight:bold;")));

    attributes = textInterface->attributes(18, &startOffset, &endOffset);
    QCOMPARE(startOffset, 18);
    QCOMPARE(endOffset, 25);
    attributes.prepend(';');
    QVERIFY(attributes.contains(QLatin1String(";font-weight:bold;")));
    QVERIFY(attributes.contains(QLatin1String(";font-style:italic;")));

    attributes = textInterface->attributes(34, &startOffset, &endOffset);
    QCOMPARE(startOffset, 31);
    QCOMPARE(endOffset, 55);
    attributes.prepend(';');
    QVERIFY(attributes.contains(QLatin1String(";font-family:\"monospace\";")));

    attributes = textInterface->attributes(65, &startOffset, &endOffset);
    QCOMPARE(startOffset, 64);
    QCOMPARE(endOffset, 74);
    attributes.prepend(';');
    QVERIFY(attributes.contains(QLatin1String(";font-size:8pt;")));

    attributes = textInterface->attributes(110, &startOffset, &endOffset);
    QCOMPARE(startOffset, 109);
    QCOMPARE(endOffset, 111);
    attributes.prepend(';');
    QVERIFY(attributes.contains(QLatin1String(";background-color:rgb(20,240,30);")));
    QVERIFY(attributes.contains(QLatin1String(";color:rgb(240,241,242);")));
}

void tst_QAccessibility::hideShowTest()
{
    QWidget * const window = new QWidget();
    window->resize(200, 200);
    QWidget * const child = new QWidget(window);

    QVERIFY(state(window) & QAccessible::Invisible);
    QVERIFY(state(child)  & QAccessible::Invisible);

    QTestAccessibility::clearEvents();

    // show() and veryfy that both window and child are not invisible and get ObjectShow events.
    window->show();
    QVERIFY(state(window) ^ QAccessible::Invisible);
    QVERIFY(state(child)  ^ QAccessible::Invisible);
    QVERIFY(QTestAccessibility::events().contains(QTestAccessibilityEvent(window, 0, QAccessible::ObjectShow)));
    QVERIFY(QTestAccessibility::events().contains(QTestAccessibilityEvent(child, 0, QAccessible::ObjectShow)));
    QTestAccessibility::clearEvents();

    // hide() and veryfy that both window and child are invisible and get ObjectHide events.
    window->hide();
    QVERIFY(state(window) & QAccessible::Invisible);
    QVERIFY(state(child)  & QAccessible::Invisible);
    QVERIFY(QTestAccessibility::events().contains(QTestAccessibilityEvent(window, 0, QAccessible::ObjectHide)));
    QVERIFY(QTestAccessibility::events().contains(QTestAccessibilityEvent(child, 0, QAccessible::ObjectHide)));
    QTestAccessibility::clearEvents();

    delete window;
    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::userActionCount()
{
    QWidget widget;

    QAccessibleInterface *test = QAccessible::queryAccessibleInterface(&widget);
    QVERIFY(test);
    QVERIFY(test->isValid());
    QCOMPARE(test->userActionCount(0), 0);
    QCOMPARE(test->userActionCount(1), 0);
    QCOMPARE(test->userActionCount(-1), 0);
    delete test; test = 0;

    QFrame frame;

    test = QAccessible::queryAccessibleInterface(&frame);
    QVERIFY(test);
    QVERIFY(test->isValid());
    QCOMPARE(test->userActionCount(0), 0);
    QCOMPARE(test->userActionCount(1), 0);
    QCOMPARE(test->userActionCount(-1), 0);
    delete test; test = 0;

    QLineEdit lineEdit;

    test = QAccessible::queryAccessibleInterface(&lineEdit);
    QVERIFY(test);
    QVERIFY(test->isValid());
    QCOMPARE(test->userActionCount(0), 0);
    QCOMPARE(test->userActionCount(1), 0);
    QCOMPARE(test->userActionCount(-1), 0);
    delete test; test = 0;
}

void tst_QAccessibility::actionText()
{
    QWidget *widget = new QWidget;
    widget->show();

    QAccessibleInterface *test = QAccessible::queryAccessibleInterface(widget);
    QVERIFY(test);
    QVERIFY(test->isValid());

    QCOMPARE(test->actionText(1, QAccessible::Name, 0), QString());
    QCOMPARE(test->actionText(0, QAccessible::Name, 1), QString());
    QCOMPARE(test->actionText(1, QAccessible::Name, 1), QString());
    QCOMPARE(test->actionText(QAccessible::SetFocus, QAccessible::Name, -1), QString());

    QCOMPARE(test->actionText(QAccessible::DefaultAction, QAccessible::Name, 0), QString("SetFocus"));
    QCOMPARE(test->actionText(QAccessible::SetFocus, QAccessible::Name, 0), QString("SetFocus"));

    delete test;
    delete widget;

    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::doAction()
{
    QSKIP("TODO: Implement me", SkipAll);
}

void tst_QAccessibility::applicationTest()
{
    QLatin1String name = QLatin1String("My Name");
    qApp->setApplicationName(name);
    QAccessibleInterface *interface = QAccessible::queryAccessibleInterface(qApp);
    QCOMPARE(interface->text(QAccessible::Name, 0), name);
    QCOMPARE(interface->role(0), QAccessible::Application);
    delete interface;
}

void tst_QAccessibility::mainWindowTest()
{
    QMainWindow *mw = new QMainWindow;
    mw->resize(300, 200);
    mw->show(); // triggers layout

    QLatin1String name = QLatin1String("I am the main window");
    mw->setWindowTitle(name);
    QTest::qWaitForWindowShown(mw);
    QVERIFY_EVENT(mw, 0, QAccessible::ObjectShow);

    QAccessibleInterface *interface = QAccessible::queryAccessibleInterface(mw);
    QCOMPARE(interface->text(QAccessible::Name, 0), name);
    QCOMPARE(interface->role(0), QAccessible::Window);
    delete interface;
    delete mw;
    QTestAccessibility::clearEvents();
}

class CounterButton : public QPushButton {
    Q_OBJECT
public:
    CounterButton(const QString& name, QWidget* parent)
        : QPushButton(name, parent), clickCount(0)
    {
        connect(this, SIGNAL(clicked(bool)), SLOT(incClickCount()));
    }
    int clickCount;
public Q_SLOTS:
    void incClickCount() {
        ++clickCount;
    }
};

void tst_QAccessibility::buttonTest()
{
    QWidget window;
    window.setLayout(new QVBoxLayout);

    // Standard push button
    CounterButton pushButton("Ok", &window);

    // toggle button
    QPushButton toggleButton("Toggle", &window);
    toggleButton.setCheckable(true);

    // standard checkbox
    QCheckBox checkBox("Check me!", &window);

    // tristate checkbox
    QCheckBox tristate("Tristate!", &window);
    tristate.setTristate(true);

    // radiobutton
    QRadioButton radio("Radio me!", &window);

    // standard toolbutton
    QToolButton toolbutton(&window);
    toolbutton.setText("Tool");
    toolbutton.setMinimumSize(20,20);

    // standard toolbutton
    QToolButton toggletool(&window);
    toggletool.setCheckable(true);
    toggletool.setText("Toggle");
    toggletool.setMinimumSize(20,20);

    // test push button
    QAccessibleInterface* interface = QAccessible::queryAccessibleInterface(&pushButton);
    QAccessibleActionInterface* actionInterface = interface->actionInterface();
    QVERIFY(actionInterface != 0);

    QCOMPARE(interface->role(0), QAccessible::PushButton);

    // currently our buttons only have click as action, press and release are missing
    QCOMPARE(actionInterface->actionCount(), 1);
    QCOMPARE(actionInterface->name(0), QString("Press"));
    QCOMPARE(pushButton.clickCount, 0);
    actionInterface->doAction(0);
    QTest::qWait(500);
    QCOMPARE(pushButton.clickCount, 1);
    delete interface;

    // test toggle button
    interface = QAccessible::queryAccessibleInterface(&toggleButton);
    actionInterface = interface->actionInterface();
    QCOMPARE(interface->role(0), QAccessible::CheckBox);
    QCOMPARE(actionInterface->description(0), QString("Toggles the button."));
    QCOMPARE(actionInterface->name(0), QString("Check"));
    QVERIFY(!toggleButton.isChecked());
    QVERIFY((interface->state(0) & QAccessible::Checked) == 0);
    actionInterface->doAction(0);
    QTest::qWait(500);
    QCOMPARE(actionInterface->name(0), QString("Uncheck"));
    QVERIFY(toggleButton.isChecked());
    QVERIFY((interface->state(0) & QAccessible::Checked));
    delete interface;

//    // test menu push button
//    QVERIFY(QAccessible::queryAccessibleInterface(&menuButton, &test));
//    QCOMPARE(test->role(0), QAccessible::ButtonMenu);
//    QCOMPARE(test->defaultAction(0), QAccessible::Press);
//    QCOMPARE(test->actionText(test->defaultAction(0), QAccessible::Name, 0), QString("Open"));
//    QCOMPARE(test->state(0), (int)QAccessible::HasPopup);
//    test->release();

    // test check box
    interface = QAccessible::queryAccessibleInterface(&checkBox);
    actionInterface = interface->actionInterface();
    QCOMPARE(interface->role(0), QAccessible::CheckBox);
    QCOMPARE(actionInterface->name(0), QString("Check"));
    QVERIFY((interface->state(0) & QAccessible::Checked) == 0);
    actionInterface->doAction(0);
    QTest::qWait(500);
    QCOMPARE(actionInterface->name(0), QString("Uncheck"));
    QVERIFY(interface->state(0) & QAccessible::Checked);
    QVERIFY(checkBox.isChecked());
    delete interface;

//    // test tristate check box
//    QVERIFY(QAccessible::queryAccessibleInterface(&tristate, &test));
//    QCOMPARE(test->role(0), QAccessible::CheckBox);
//    QCOMPARE(test->defaultAction(0), QAccessible::Press);
//    QCOMPARE(test->actionText(test->defaultAction(0), QAccessible::Name, 0), QString("Toggle"));
//    QCOMPARE(test->state(0), (int)QAccessible::Normal);
//    QVERIFY(test->doAction(QAccessible::Press, 0));
//    QTest::qWait(500);
//    QCOMPARE(test->actionText(test->defaultAction(0), QAccessible::Name, 0), QString("Check"));
//    QCOMPARE(test->state(0), (int)QAccessible::Mixed);
//    QVERIFY(test->doAction(QAccessible::Press, 0));
//    QTest::qWait(500);
//    QCOMPARE(test->actionText(test->defaultAction(0), QAccessible::Name, 0), QString("Uncheck"));
//    QCOMPARE(test->state(0), (int)QAccessible::Checked);
//    test->release();

    // test radiobutton
    interface = QAccessible::queryAccessibleInterface(&radio);
    actionInterface = interface->actionInterface();
    QCOMPARE(interface->role(0), QAccessible::RadioButton);
    QCOMPARE(actionInterface->name(0), QString("Check"));
    QVERIFY((interface->state(0) & QAccessible::Checked) == 0);
    actionInterface->doAction(0);
    QTest::qWait(500);
    QCOMPARE(actionInterface->name(0), QString("Uncheck"));
    QVERIFY(interface->state(0) & QAccessible::Checked);
    QVERIFY(checkBox.isChecked());
    delete interface;

//    // test standard toolbutton
//    QVERIFY(QAccessible::queryAccessibleInterface(&toolbutton, &test));
//    QCOMPARE(test->role(0), QAccessible::PushButton);
//    QCOMPARE(test->defaultAction(0), QAccessible::Press);
//    QCOMPARE(test->actionText(test->defaultAction(0), QAccessible::Name, 0), QString("Press"));
//    QCOMPARE(test->state(0), (int)QAccessible::Normal);
//    test->release();

//    // toggle tool button
//    QVERIFY(QAccessible::queryAccessibleInterface(&toggletool, &test));
//    QCOMPARE(test->role(0), QAccessible::CheckBox);
//    QCOMPARE(test->defaultAction(0), QAccessible::Press);
//    QCOMPARE(test->actionText(test->defaultAction(0), QAccessible::Name, 0), QString("Check"));
//    QCOMPARE(test->state(0), (int)QAccessible::Normal);
//    QVERIFY(test->doAction(QAccessible::Press, 0));
//    QTest::qWait(500);
//    QCOMPARE(test->actionText(test->defaultAction(0), QAccessible::Name, 0), QString("Uncheck"));
//    QCOMPARE(test->state(0), (int)QAccessible::Checked);
//    test->release();

//    // test menu toolbutton
//    QVERIFY(QAccessible::queryAccessibleInterface(&menuToolButton, &test));
//    QCOMPARE(test->role(0), QAccessible::ButtonMenu);
//    QCOMPARE(test->defaultAction(0), 1);
//    QCOMPARE(test->actionText(test->defaultAction(0), QAccessible::Name, 0), QString("Open"));
//    QCOMPARE(test->state(0), (int)QAccessible::HasPopup);
//    QCOMPARE(test->actionCount(0), 1);
//    QCOMPARE(test->actionText(QAccessible::Press, QAccessible::Name, 0), QString("Press"));
//    test->release();

//    // test split menu toolbutton
//    QVERIFY(QAccessible::queryAccessibleInterface(&splitToolButton, &test));
//    QCOMPARE(test->childCount(), 2);
//    QCOMPARE(test->role(0), QAccessible::ButtonDropDown);
//    QCOMPARE(test->role(1), QAccessible::PushButton);
//    QCOMPARE(test->role(2), QAccessible::ButtonMenu);
//    QCOMPARE(test->defaultAction(0), QAccessible::Press);
//    QCOMPARE(test->defaultAction(1), QAccessible::Press);
//    QCOMPARE(test->defaultAction(2), QAccessible::Press);
//    QCOMPARE(test->actionText(test->defaultAction(0), QAccessible::Name, 0), QString("Press"));
//    QCOMPARE(test->state(0), (int)QAccessible::HasPopup);
//    QCOMPARE(test->actionCount(0), 1);
//    QCOMPARE(test->actionText(1, QAccessible::Name, 0), QString("Open"));
//    QCOMPARE(test->actionText(test->defaultAction(1), QAccessible::Name, 1), QString("Press"));
//    QCOMPARE(test->state(1), (int)QAccessible::Normal);
//    QCOMPARE(test->actionText(test->defaultAction(2), QAccessible::Name, 2), QString("Open"));
//    QCOMPARE(test->state(2), (int)QAccessible::HasPopup);
//    test->release();

    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::scrollBarTest()
{
    // Test that when we hide() a slider, the PageLeft, Indicator, and PageRight also gets the
    // Invisible state bit set.
    enum SubControls { LineUp = 1,
        PageUp = 2,
        Position = 3,
        PageDown = 4,
        LineDown = 5
    };

    QScrollBar *scrollBar  = new QScrollBar();
    QAccessibleInterface * const scrollBarInterface = QAccessible::queryAccessibleInterface(scrollBar);
    QVERIFY(scrollBarInterface);

    QVERIFY(scrollBarInterface->state(0)         & QAccessible::Invisible);
    QVERIFY(scrollBarInterface->state(PageUp)    & QAccessible::Invisible);
    QVERIFY(scrollBarInterface->state(Position)  & QAccessible::Invisible);
    QVERIFY(scrollBarInterface->state(PageDown)  & QAccessible::Invisible);

    scrollBar->show();
    QVERIFY(scrollBarInterface->state(0)         ^ QAccessible::Invisible);
    QVERIFY(scrollBarInterface->state(PageUp)    ^ QAccessible::Invisible);
    QVERIFY(scrollBarInterface->state(Position)  ^ QAccessible::Invisible);
    QVERIFY(scrollBarInterface->state(PageDown)  ^ QAccessible::Invisible);
    QVERIFY(QTestAccessibility::events().contains(QTestAccessibilityEvent(scrollBar, 0, QAccessible::ObjectShow)));
    QTestAccessibility::clearEvents();

    scrollBar->hide();
    QVERIFY(scrollBarInterface->state(0)         & QAccessible::Invisible);
    QVERIFY(scrollBarInterface->state(PageUp)    & QAccessible::Invisible);
    QVERIFY(scrollBarInterface->state(Position)  & QAccessible::Invisible);
    QVERIFY(scrollBarInterface->state(PageDown)  & QAccessible::Invisible);
    QVERIFY(QTestAccessibility::events().contains(QTestAccessibilityEvent(scrollBar, 0, QAccessible::ObjectHide)));
    QTestAccessibility::clearEvents();

    // Test that the left/right subcontrols are set to unavailable when the scrollBar is at the minimum/maximum.
    scrollBar->show();
    scrollBar->setMinimum(0);
    scrollBar->setMaximum(100);

    scrollBar->setValue(50);
    QVERIFY(scrollBarInterface->state(PageUp)    ^ QAccessible::Unavailable);
    QVERIFY(scrollBarInterface->state(Position)  ^ QAccessible::Unavailable);
    QVERIFY(scrollBarInterface->state(PageDown)  ^ QAccessible::Unavailable);

    scrollBar->setValue(0);
    QVERIFY(scrollBarInterface->state(PageUp)    & QAccessible::Unavailable);
    QVERIFY(scrollBarInterface->state(Position)  ^ QAccessible::Unavailable);
    QVERIFY(scrollBarInterface->state(PageDown)  ^ QAccessible::Unavailable);

    scrollBar->setValue(100);
    QVERIFY(scrollBarInterface->state(PageUp)   ^ QAccessible::Unavailable);
    QVERIFY(scrollBarInterface->state(Position) ^ QAccessible::Unavailable);
    QVERIFY(scrollBarInterface->state(PageDown) & QAccessible::Unavailable);

    delete scrollBarInterface;
    delete scrollBar;


    // Test that the rects are ok.
    {
        QScrollBar *scrollBar  = new QScrollBar(Qt::Horizontal);
        scrollBar->resize(200, 50);
        scrollBar->show();
#if defined(Q_WS_X11)
        qt_x11_wait_for_window_manager(scrollBar);
        QTest::qWait(100);
#endif
        QAccessibleInterface * const scrollBarInterface = QAccessible::queryAccessibleInterface(scrollBar);
        QVERIFY(scrollBarInterface);

        scrollBar->setMinimum(0);
        scrollBar->setMaximum(100);
        scrollBar->setValue(50);

        QApplication::processEvents();

        const QRect scrollBarRect = scrollBarInterface->rect(0);
        QVERIFY(scrollBarRect.isValid());
        // Verify that the sub-control rects are valid and inside the scrollBar rect.
        for (int i = LineUp; i <= LineDown; ++i) {
            const QRect testRect = scrollBarInterface->rect(i);
            QVERIFY(testRect.isValid());
            QVERIFY(scrollBarRect.contains(testRect));
        }
        delete scrollBarInterface;
        delete scrollBar;
    }

    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::tabTest()
{
    QTabBar *tabBar = new QTabBar();
    setFrameless(tabBar);
    tabBar->show();

    QAccessibleInterface * const interface = QAccessible::queryAccessibleInterface(tabBar);
    QVERIFY(interface);
    QCOMPARE(interface->childCount(), 2);
    interface->doAction(QAccessible::Press, 1);
    interface->doAction(QAccessible::Press, 2);

    // Test that the Invisible bit for the navigation buttons gets set
    // and cleared correctly.
    QVERIFY(interface->state(1) & QAccessible::Invisible);

    const int lots = 10;
    for (int i = 0; i < lots; ++i)
        tabBar->addTab("Foo");

    QVERIFY((interface->state(1) & QAccessible::Invisible) == false);
    tabBar->hide();
    QVERIFY(interface->state(1) & QAccessible::Invisible);

    tabBar->show();
    tabBar->setCurrentIndex(0);

    // Test that sending a focus action to a tab does not select it.
    interface->doAction(QAccessible::Focus, 2, QVariantList());
    QCOMPARE(tabBar->currentIndex(), 0);

    // Test that sending a press action to a tab selects it.
    interface->doAction(QAccessible::Press, 2, QVariantList());
    QCOMPARE(tabBar->currentIndex(), 1);

    delete tabBar;
    delete interface;
    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::tabWidgetTest()
{
    QTabWidget *tabWidget = new QTabWidget();
    tabWidget->show();

    // the interface for the tab is just a container for tabbar and stacked widget
    QAccessibleInterface * const interface = QAccessible::queryAccessibleInterface(tabWidget);
    QVERIFY(interface);
    QCOMPARE(interface->childCount(), 2);
    QCOMPARE(interface->role(0), QAccessible::Client);

    // Create pages, check navigation
    QLabel *label1 = new QLabel("Page 1", tabWidget);
    tabWidget->addTab(label1, "Tab 1");
    QLabel *label2 = new QLabel("Page 2", tabWidget);
    tabWidget->addTab(label2, "Tab 2");

    QCOMPARE(interface->childCount(), 2);

    QAccessibleInterface* tabBarInterface = 0;
    // there is no special logic to sort the children, so the contents will be 1, the tab bar 2
    QCOMPARE(interface->navigate(QAccessible::Child, 2 , &tabBarInterface), 0);
    QVERIFY(tabBarInterface);
    QCOMPARE(tabBarInterface->childCount(), 4);
    QCOMPARE(tabBarInterface->role(0), QAccessible::PageTabList);

    QAccessibleInterface* tabButton1Interface = 0;
    QCOMPARE(tabBarInterface->navigate(QAccessible::Child, 1 , &tabButton1Interface), 1);
    QVERIFY(tabButton1Interface == 0);

    QCOMPARE(tabBarInterface->role(1), QAccessible::PageTab);
    QCOMPARE(tabBarInterface->text(QAccessible::Name, 1), QLatin1String("Tab 1"));
    QCOMPARE(tabBarInterface->role(2), QAccessible::PageTab);
    QCOMPARE(tabBarInterface->text(QAccessible::Name, 2), QLatin1String("Tab 2"));
    QCOMPARE(tabBarInterface->role(3), QAccessible::PushButton);
    QCOMPARE(tabBarInterface->text(QAccessible::Name, 3), QLatin1String("Scroll Left"));
    QCOMPARE(tabBarInterface->role(4), QAccessible::PushButton);
    QCOMPARE(tabBarInterface->text(QAccessible::Name, 4), QLatin1String("Scroll Right"));

    QAccessibleInterface* stackWidgetInterface = 0;
    QCOMPARE(interface->navigate(QAccessible::Child, 1, &stackWidgetInterface), 0);
    QVERIFY(stackWidgetInterface);
    QCOMPARE(stackWidgetInterface->childCount(), 2);
    QCOMPARE(stackWidgetInterface->role(0), QAccessible::LayeredPane);

    QAccessibleInterface* stackChild1Interface = 0;
    QCOMPARE(stackWidgetInterface->navigate(QAccessible::Child, 1, &stackChild1Interface), 0);
    QVERIFY(stackChild1Interface);
#ifndef Q_CC_INTEL
    QCOMPARE(stackChild1Interface->childCount(), 0);
#endif
    QCOMPARE(stackChild1Interface->role(0), QAccessible::StaticText);
    QCOMPARE(stackChild1Interface->text(QAccessible::Name, 0), QLatin1String("Page 1"));
    QCOMPARE(label1, stackChild1Interface->object());

    // Navigation in stack widgets should be consistent
    QAccessibleInterface* parent = 0;
    QCOMPARE(stackChild1Interface->navigate(QAccessible::Ancestor, 1, &parent), 0);
    QVERIFY(parent);
#ifndef Q_CC_INTEL
    QCOMPARE(parent->childCount(), 2);
#endif
    QCOMPARE(parent->role(0), QAccessible::LayeredPane);
    delete parent;

    QAccessibleInterface* stackChild2Interface = 0;
    QCOMPARE(stackWidgetInterface->navigate(QAccessible::Child, 2, &stackChild2Interface), 0);
    QVERIFY(stackChild2Interface);
    QCOMPARE(stackChild2Interface->childCount(), 0);
    QCOMPARE(stackChild2Interface->role(0), QAccessible::StaticText);
    QCOMPARE(label2, stackChild2Interface->object()); // the text will be empty since it is not visible

    QCOMPARE(stackChild2Interface->navigate(QAccessible::Ancestor, 1, &parent), 0);
    QVERIFY(parent);
#ifndef Q_CC_INTEL
    QCOMPARE(parent->childCount(), 2);
#endif
    QCOMPARE(parent->role(0), QAccessible::LayeredPane);
    delete parent;

    delete tabBarInterface;
    delete stackChild1Interface;
    delete stackChild2Interface;
    delete stackWidgetInterface;
    delete interface;
    delete tabWidget;
    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::menuTest()
{
    {
    QMainWindow mw;
    mw.resize(300, 200);
    QMenu *file = mw.menuBar()->addMenu("&File");
    QMenu *fileNew = file->addMenu("&New...");
    fileNew->menuAction()->setShortcut(tr("Ctrl+N"));
    fileNew->addAction("Text file");
    fileNew->addAction("Image file");
    file->addAction("&Open")->setShortcut(tr("Ctrl+O"));
    file->addAction("&Save")->setShortcut(tr("Ctrl+S"));
    file->addSeparator();
    file->addAction("E&xit")->setShortcut(tr("Alt+F4"));

    QMenu *edit = mw.menuBar()->addMenu("&Edit");
    edit->addAction("&Undo")->setShortcut(tr("Ctrl+Z"));
    edit->addAction("&Redo")->setShortcut(tr("Ctrl+Y"));
    edit->addSeparator();
    edit->addAction("Cu&t")->setShortcut(tr("Ctrl+X"));
    edit->addAction("&Copy")->setShortcut(tr("Ctrl+C"));
    edit->addAction("&Paste")->setShortcut(tr("Ctrl+V"));
    edit->addAction("&Delete")->setShortcut(tr("Del"));
    edit->addSeparator();
    edit->addAction("Pr&operties");

    mw.menuBar()->addSeparator();

    QMenu *help = mw.menuBar()->addMenu("&Help");
    help->addAction("&Contents");
    help->addAction("&About");

    mw.menuBar()->addAction("Action!");

    mw.show(); // triggers layout
    QTest::qWait(100);

    QAccessibleInterface *interface = QAccessible::queryAccessibleInterface(mw.menuBar());

    QCOMPARE(verifyHierarchy(interface),  0);

    QVERIFY(interface);
    QCOMPARE(interface->childCount(), 5);
    QCOMPARE(interface->role(0), QAccessible::MenuBar);
    QCOMPARE(interface->role(1), QAccessible::MenuItem);
    QCOMPARE(interface->role(2), QAccessible::MenuItem);
    QCOMPARE(interface->role(3), QAccessible::Separator);
    QCOMPARE(interface->role(4), QAccessible::MenuItem);
    QCOMPARE(interface->role(5), QAccessible::MenuItem);
#ifndef Q_WS_MAC
#ifdef Q_OS_WINCE
    if (!IsValidCEPlatform()) {
        QSKIP("Tests do not work on Mobile platforms due to native menus", SkipAll);
    }
#endif
    QCOMPARE(mw.mapFromGlobal(interface->rect(0).topLeft()), mw.menuBar()->geometry().topLeft());
    QCOMPARE(interface->rect(0).size(), mw.menuBar()->size());

    QVERIFY(interface->rect(0).contains(interface->rect(1)));
    QVERIFY(interface->rect(0).contains(interface->rect(2)));
    // QVERIFY(interface->rect(0).contains(interface->rect(3))); //separator might be invisible
    QVERIFY(interface->rect(0).contains(interface->rect(4)));
    QVERIFY(interface->rect(0).contains(interface->rect(5)));
#endif

    QCOMPARE(interface->text(QAccessible::Name, 1), QString("File"));
    QCOMPARE(interface->text(QAccessible::Name, 2), QString("Edit"));
    QCOMPARE(interface->text(QAccessible::Name, 3), QString());
    QCOMPARE(interface->text(QAccessible::Name, 4), QString("Help"));
    QCOMPARE(interface->text(QAccessible::Name, 5), QString("Action!"));

// TODO: Currently not working, task to fix is #100019.
#ifndef Q_OS_MAC
    QCOMPARE(interface->text(QAccessible::Accelerator, 1), tr("Alt+F"));
    QCOMPARE(interface->text(QAccessible::Accelerator, 2), tr("Alt+E"));
    QCOMPARE(interface->text(QAccessible::Accelerator, 4), tr("Alt+H"));
    QCOMPARE(interface->text(QAccessible::Accelerator, 3), QString());
    QCOMPARE(interface->text(QAccessible::Accelerator, 4), tr("Alt+H"));
    QCOMPARE(interface->text(QAccessible::Accelerator, 5), QString());
#endif

    QCOMPARE(interface->actionText(QAccessible::DefaultAction, QAccessible::Name, 1), QString("Open"));
    QCOMPARE(interface->actionText(QAccessible::DefaultAction, QAccessible::Name, 2), QString("Open"));
    QCOMPARE(interface->actionText(QAccessible::DefaultAction, QAccessible::Name, 3), QString());
    QCOMPARE(interface->actionText(QAccessible::DefaultAction, QAccessible::Name, 4), QString("Open"));
    QCOMPARE(interface->actionText(QAccessible::DefaultAction, QAccessible::Name, 5), QString("Execute"));

    bool menuFade = qApp->isEffectEnabled(Qt::UI_FadeMenu);
    int menuFadeDelay = 300;
    interface->doAction(QAccessible::DefaultAction, 1);
    if(menuFade)
        QTest::qWait(menuFadeDelay);
    QVERIFY(file->isVisible() && !edit->isVisible() && !help->isVisible());
    interface->doAction(QAccessible::DefaultAction, 2);
    if(menuFade)
        QTest::qWait(menuFadeDelay);
    QVERIFY(!file->isVisible() && edit->isVisible() && !help->isVisible());
    interface->doAction(QAccessible::DefaultAction, 3);
    if(menuFade)
        QTest::qWait(menuFadeDelay);
    QVERIFY(!file->isVisible() && !edit->isVisible() && !help->isVisible());
    interface->doAction(QAccessible::DefaultAction, 4);
    if(menuFade)
        QTest::qWait(menuFadeDelay);
    QVERIFY(!file->isVisible() && !edit->isVisible() && help->isVisible());
    interface->doAction(QAccessible::DefaultAction, 5);
    if(menuFade)
        QTest::qWait(menuFadeDelay);
    QVERIFY(!file->isVisible() && !edit->isVisible() && !help->isVisible());

    interface->doAction(QAccessible::DefaultAction, 1);
    delete interface;
    interface = QAccessible::queryAccessibleInterface(file);
    QCOMPARE(interface->childCount(), 5);
    QCOMPARE(interface->role(0), QAccessible::PopupMenu);
    QCOMPARE(interface->role(1), QAccessible::MenuItem);
    QCOMPARE(interface->role(2), QAccessible::MenuItem);
    QCOMPARE(interface->role(3), QAccessible::MenuItem);
    QCOMPARE(interface->role(4), QAccessible::Separator);
    QCOMPARE(interface->role(5), QAccessible::MenuItem);
    QCOMPARE(interface->actionText(QAccessible::DefaultAction, QAccessible::Name, 1), QString("Open"));
    QCOMPARE(interface->actionText(QAccessible::DefaultAction, QAccessible::Name, 2), QString("Execute"));
    QCOMPARE(interface->actionText(QAccessible::DefaultAction, QAccessible::Name, 3), QString("Execute"));
    QCOMPARE(interface->actionText(QAccessible::DefaultAction, QAccessible::Name, 4), QString());
    QCOMPARE(interface->actionText(QAccessible::DefaultAction, QAccessible::Name, 5), QString("Execute"));

    // QTBUG-21578 - after setting accessible name on a menu it would no longer
    // return the right names for it's children.
    QCOMPARE(interface->text(QAccessible::Name, 1), QString("New..."));
    file->setAccessibleName("File");
    QCOMPARE(interface->text(QAccessible::Name, 1), QString("New..."));

    QAccessibleInterface *iface = 0;
    QAccessibleInterface *iface2 = 0;

    // traverse siblings with navigate(Sibling, ...)
    int entry = interface->navigate(QAccessible::Child, 1, &iface);
    QCOMPARE(entry, 0);
    QVERIFY(iface);
    QCOMPARE(iface->role(0), QAccessible::MenuItem);

    QAccessible::Role fileRoles[5] = {
        QAccessible::MenuItem,
        QAccessible::MenuItem,
        QAccessible::MenuItem,
        QAccessible::Separator,
        QAccessible::MenuItem
    };
    for (int child = 0; child < 5; ++child) {
        entry = iface->navigate(QAccessible::Sibling, child + 1, &iface2);
        QCOMPARE(entry, 0);
        QVERIFY(iface2);
        QCOMPARE(iface2->role(0), fileRoles[child]);
        delete iface2;
    }
    delete iface;

    // traverse menu items with navigate(Down, ...)
    entry = interface->navigate(QAccessible::Child, 1, &iface);
    QCOMPARE(entry, 0);
    QVERIFY(iface);
    QCOMPARE(iface->role(0), QAccessible::MenuItem);

    for (int child = 0; child < 4; ++child) {
        entry = iface->navigate(QAccessible::Down, 1, &iface2);
        delete iface;
        iface = iface2;
        QCOMPARE(entry, 0);
        QVERIFY(iface);
        QCOMPARE(iface->role(0), fileRoles[child + 1]);
    }
    delete iface;

    // traverse menu items with navigate(Up, ...)
    entry = interface->navigate(QAccessible::Child, interface->childCount(), &iface);
    QCOMPARE(entry, 0);
    QVERIFY(iface);
    QCOMPARE(iface->role(0), QAccessible::MenuItem);

    for (int child = 3; child >= 0; --child) {
        entry = iface->navigate(QAccessible::Up, 1, &iface2);
        delete iface;
        iface = iface2;
        QCOMPARE(entry, 0);
        QVERIFY(iface);
        QCOMPARE(iface->role(0), fileRoles[child]);
    }
    delete iface;

    // "New" item
    entry = interface->navigate(QAccessible::Child, 1, &iface);
    QCOMPARE(entry, 0);
    QVERIFY(iface);
    QCOMPARE(iface->role(0), QAccessible::MenuItem);

    // "New" menu
    entry = iface->navigate(QAccessible::Child, 1, &iface2);
    delete iface;
    iface = iface2;
    QCOMPARE(entry, 0);
    QVERIFY(iface);
    QCOMPARE(iface->role(0), QAccessible::PopupMenu);

    // "Text file" menu item
    entry = iface->navigate(QAccessible::Child, 1, &iface2);
    delete iface;
    iface = iface2;
    QCOMPARE(entry, 0);
    QVERIFY(iface);
    QCOMPARE(iface->role(0), QAccessible::MenuItem);

    // Traverse to the menubar.
    QAccessibleInterface *ifaceMenuBar = 0;
    entry = iface->navigate(QAccessible::Ancestor, 5, &ifaceMenuBar);
    QCOMPARE(ifaceMenuBar->role(0), QAccessible::MenuBar);
    delete ifaceMenuBar;

    delete iface;

    // move mouse pointer away, since that might influence the
    // subsequent tests
    QTest::mouseMove(&mw, QPoint(-1, -1));
    QTest::qWait(100);
    if (menuFade)
        QTest::qWait(menuFadeDelay);
    interface->doAction(QAccessible::DefaultAction, 1);
    QTestEventLoop::instance().enterLoop(2);

    QVERIFY(file->isVisible());
    QVERIFY(fileNew->isVisible());
    QVERIFY(!edit->isVisible());
    QVERIFY(!help->isVisible());

    QTestAccessibility::clearEvents();
    mw.hide();


    // Do not crash if the menu don't have a parent
    QMenu *menu = new QMenu;
    menu->addAction(QLatin1String("one"));
    menu->addAction(QLatin1String("two"));
    menu->addAction(QLatin1String("three"));
    iface = QAccessible::queryAccessibleInterface(menu);
    QCOMPARE(iface->navigate(QAccessible::Ancestor, 1, &iface2), 0);
    QCOMPARE(iface2->role(0), QAccessible::Application);
    // caused a *crash*
    iface2->state(0);
    delete iface2;
    delete iface;
    delete menu;

    }
    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::spinBoxTest()
{
    QSpinBox * const spinBox = new QSpinBox();
    setFrameless(spinBox);
    spinBox->show();

    QAccessibleInterface * const interface = QAccessible::queryAccessibleInterface(spinBox);
    QVERIFY(interface);

    const QRect widgetRect = spinBox->geometry();
    const QRect accessibleRect = interface->rect(0);
    QCOMPARE(accessibleRect, widgetRect);

    // Test that we get valid rects for all the spinbox child interfaces.
    const int numChildren = interface->childCount();
    for (int i = 1; i <= numChildren; ++i) {
        const QRect childRect = interface->rect(i);
        QVERIFY(childRect.isNull() == false);
    }

    spinBox->setFocus();
    QTestAccessibility::clearEvents();
    QTest::keyPress(spinBox, Qt::Key_Up);
    QTest::qWait(200);
    EventList events = QTestAccessibility::events();
    QTestAccessibilityEvent expectedEvent(spinBox, 0, (int)QAccessible::ValueChanged);
    QVERIFY(events.contains(expectedEvent));
    delete spinBox;
    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::doubleSpinBoxTest()
{
    QDoubleSpinBox *doubleSpinBox = new QDoubleSpinBox;
    setFrameless(doubleSpinBox);
    doubleSpinBox->show();

    QAccessibleInterface *interface = QAccessible::queryAccessibleInterface(doubleSpinBox);
    QVERIFY(interface);

    const QRect widgetRect = doubleSpinBox->geometry();
    const QRect accessibleRect = interface->rect(0);
    QCOMPARE(accessibleRect, widgetRect);

    // Test that we get valid rects for all the spinbox child interfaces.
    const int numChildren = interface->childCount();
    for (int i = 1; i <= numChildren; ++i) {
        const QRect childRect = interface->rect(i);
        QVERIFY(childRect.isValid());
    }

    delete doubleSpinBox;
    QTestAccessibility::clearEvents();
}

static void dumpTextDiagnostics(const QTextEdit &edit, int offset)
{
    qDebug() << "Incorrect result, font:" << edit.currentFont().toString();
    QFontMetricsF fm(edit.currentFont());
    qDebug() << "QFontMetricsF: " << fm.ascent() << fm.descent() << fm.height();

    QTextBlock block = edit.document()->findBlock(offset);
    QVERIFY(block.isValid());
    QTextLayout *layout = block.layout();
    QPointF layoutPosition = layout->position();
    QTextLine line = layout->lineForTextPosition(offset);
    qDebug() << "QTextLine:     " << line.ascent() << line.descent() << line.height() << line.leading();
    qDebug() << block.text();

    // Reported to only be a problem on Ubuntu Oneiric. Verify that.
#if defined(Q_WS_X11) && defined(UBUNTU_ONEIRIC)
    qDebug() << "UBUNTU_ONEIRIC";
#endif

}

void tst_QAccessibility::textEditTest()
{
    {
    QTextEdit edit;
    int startOffset;
    int endOffset;
    QString text = "hello world\nhow are you today?\n";
    edit.setText(text);
    edit.show();

    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&edit);
    QCOMPARE(iface->text(QAccessible::Value, 0), text);
    QCOMPARE(iface->childCount(), 6);
    QCOMPARE(iface->text(QAccessible::Value, 4), QString("hello world"));
    QCOMPARE(iface->text(QAccessible::Value, 5), QString("how are you today?"));
    QCOMPARE(iface->textInterface()->textAtOffset(8, QAccessible2::WordBoundary, &startOffset, &endOffset), QString("world"));
    QCOMPARE(startOffset, 6);
    QCOMPARE(endOffset, 11);
    QCOMPARE(iface->textInterface()->textAtOffset(14, QAccessible2::LineBoundary, &startOffset, &endOffset), QString("how are you today?"));
    QCOMPARE(startOffset, 12);
    QCOMPARE(endOffset, 30);
    QCOMPARE(iface->text(QAccessible::Value, 6), QString());
    QCOMPARE(iface->textInterface()->characterCount(), 31);
    QFontMetrics fm(edit.currentFont());

    // Test for
    // QAccessibleTextInterface::characterRect() and
    ///QAccessibleTextInterface::offsetAtPoint()
    struct {
        int offset;
        char ch;
    } testdata[] = {
        {0, 'h'},
        {4, 'o'},
        // skip space, it might be too narrow to reliably hit it
        {6, 'w'}
    };

    const int expectedHeight = fm.height();   //ascent + descent + 1

    for (int i = 0; i < 3; ++i) {
        int offset = testdata[i].offset;
        QRect rect = iface->textInterface()->characterRect(offset, QAccessible2::RelativeToParent);
        QVERIFY(rect.isValid());
        const QSize actualSize = rect.size();
        const int widthDelta = actualSize.width() - fm.width(QChar(testdata[i].ch));
        const int heightDelta = actualSize.height() - expectedHeight;
        // The deltas should really be 0, but it seems that it fails for some fonts
        // Dump some diagnostics if that is the case
        if (heightDelta || widthDelta)
            dumpTextDiagnostics(edit, offset);

        QVERIFY(std::abs(widthDelta) <= 1);

        if (std::abs(heightDelta) == 1) {
            qDebug() << "Result is off by one, accepted. (" << actualSize.height() << expectedHeight << ")";
        } else {
            QCOMPARE(actualSize.height(), expectedHeight);
        }
        // Width must be >= 3 in order for rect.center() to not end up on one of the edges. They are not reliable.
        if (rect.width() >= 3) {
            QCOMPARE(iface->textInterface()->offsetAtPoint(rect.center(), QAccessible2::RelativeToParent), offset);
        }
    }

    iface->editableTextInterface()->copyText(6, 11);
    QCOMPARE(QApplication::clipboard()->text(), QLatin1String("world"));
    iface->editableTextInterface()->cutText(12, 16);
    QCOMPARE(QApplication::clipboard()->text(), QLatin1String("how "));
    QCOMPARE(iface->textInterface()->text(12, 15), QLatin1String("are"));
    }
    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::textBrowserTest()
{
    {
    QTextBrowser textBrowser;
    QString text = QLatin1String("Hello world\nhow are you today?\n");
    textBrowser.setText(text);
    textBrowser.show();

    QAccessibleInterface *interface = QAccessible::queryAccessibleInterface(&textBrowser);
    QVERIFY(interface);
    QCOMPARE(interface->role(0), QAccessible::StaticText);
    QCOMPARE(interface->text(QAccessible::Value, 0), text);
    QCOMPARE(interface->childCount(), 6);
    QCOMPARE(interface->text(QAccessible::Value, 4), QString("Hello world"));
    QCOMPARE(interface->text(QAccessible::Value, 5), QString("how are you today?"));
    QCOMPARE(interface->text(QAccessible::Value, 6), QString());
    }
    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::listViewTest()
{
#if defined(Q_WS_X11)
    QSKIP( "Accessible table1 interface is no longer supported on X11.", SkipAll);
#else
    {
        QListView listView;
        QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&listView);
        QVERIFY(iface);
        QCOMPARE(iface->childCount(), 1);
        delete iface;
    }
    {
    QListWidget listView;
    listView.addItem(tr("A"));
    listView.addItem(tr("B"));
    listView.addItem(tr("C"));
    listView.resize(400,400);
    listView.show();
    QTest::qWait(1); // Need this for indexOfchild to work.
#if defined(Q_WS_X11)
    qt_x11_wait_for_window_manager(&listView);
    QTest::qWait(100);
#endif

    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&listView);
    QCOMPARE((int)iface->role(0), (int)QAccessible::Client);
    QCOMPARE((int)iface->role(1), (int)QAccessible::List);
    QCOMPARE(iface->childCount(), 1);
    QAccessibleInterface *child;
    iface->navigate(QAccessible::Child, 1, &child);
    delete iface;
    iface = child;
    QCOMPARE(iface->text(QAccessible::Name, 1), QString("A"));
    QCOMPARE(iface->text(QAccessible::Name, 2), QString("B"));
    QCOMPARE(iface->text(QAccessible::Name, 3), QString("C"));

    QCOMPARE(iface->childCount(), 3);

    QAccessibleInterface *childA = 0;
    QCOMPARE(iface->navigate(QAccessible::Child, 1, &childA), 0);
    QVERIFY(childA);
    QCOMPARE(iface->indexOfChild(childA), 1);
    QCOMPARE(childA->text(QAccessible::Name, 1), QString("A"));
    delete childA;

    QAccessibleInterface *childB = 0;
    QCOMPARE(iface->navigate(QAccessible::Child, 2, &childB), 0);
    QVERIFY(childB);
    QCOMPARE(iface->indexOfChild(childB), 2);
    QCOMPARE(childB->text(QAccessible::Name, 1), QString("B"));
    delete childB;

    QAccessibleInterface *childC = 0;
    QCOMPARE(iface->navigate(QAccessible::Child, 3, &childC), 0);
    QVERIFY(childC);
    QCOMPARE(iface->indexOfChild(childC), 3);
    QCOMPARE(childC->text(QAccessible::Name, 1), QString("C"));
    delete childC;
    QTestAccessibility::clearEvents();

    // Check for events
    QTest::mouseClick(listView.viewport(), Qt::LeftButton, 0, listView.visualItemRect(listView.item(1)).center());
    QTest::mouseClick(listView.viewport(), Qt::LeftButton, 0, listView.visualItemRect(listView.item(2)).center());
    QVERIFY(QTestAccessibility::events().contains(QTestAccessibilityEvent(listView.viewport(), 2, QAccessible::Selection)));
    QVERIFY(QTestAccessibility::events().contains(QTestAccessibilityEvent(listView.viewport(), 3, QAccessible::Selection)));
    delete iface;

    }
    QTestAccessibility::clearEvents();
#endif
}


void tst_QAccessibility::mdiAreaTest()
{
    {
    QMdiArea mdiArea;
    mdiArea.resize(400,300);
    mdiArea.show();
    const int subWindowCount = 3;
    for (int i = 0; i < subWindowCount; ++i)
        mdiArea.addSubWindow(new QWidget, Qt::Dialog)->show();

    QList<QMdiSubWindow *> subWindows = mdiArea.subWindowList();
    QCOMPARE(subWindows.count(), subWindowCount);

    QAccessibleInterface *interface = QAccessible::queryAccessibleInterface(&mdiArea);
    QVERIFY(interface);
    QCOMPARE(interface->childCount(), subWindowCount);

    // Right, right, right, ...
    for (int i = 0; i < subWindowCount; ++i) {
        QAccessibleInterface *destination = 0;
        int index = interface->navigate(QAccessible::Right, i + 1, &destination);
        if (i == subWindowCount - 1) {
            QVERIFY(!destination);
            QCOMPARE(index, -1);
        } else {
            QVERIFY(destination);
            QCOMPARE(index, 0);
            QCOMPARE(destination->object(), (QObject*)subWindows.at(i + 1));
            delete destination;
        }
    }

    // Left, left, left, ...
    for (int i = subWindowCount; i > 0; --i) {
        QAccessibleInterface *destination = 0;
        int index = interface->navigate(QAccessible::Left, i, &destination);
        if (i == 1) {
            QVERIFY(!destination);
            QCOMPARE(index, -1);
        } else {
            QVERIFY(destination);
            QCOMPARE(index, 0);
            QCOMPARE(destination->object(), (QObject*)subWindows.at(i - 2));
            delete destination;
        }
    }
    // ### Add test for Up and Down.

    }
    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::mdiSubWindowTest()
{
    {
    QMdiArea mdiArea;
    mdiArea.show();
    qApp->setActiveWindow(&mdiArea);
#if defined(Q_WS_X11)
    qt_x11_wait_for_window_manager(&mdiArea);
    QTest::qWait(150);
#endif

    bool isSubWindowsPlacedNextToEachOther = false;
    const int subWindowCount =  5;
    for (int i = 0; i < subWindowCount; ++i) {
        QMdiSubWindow *window = mdiArea.addSubWindow(new QPushButton("QAccessibilityTest"));
        window->show();
        // Parts of this test requires that the sub windows are placed next
        // to each other. In order to achieve that QMdiArea must have
        // a width which is larger than subWindow->width() * subWindowCount.
        if (i == 0) {
            int minimumWidth = window->width() * subWindowCount + 20;
            mdiArea.resize(mdiArea.size().expandedTo(QSize(minimumWidth, 0)));
#if defined(Q_WS_X11)
            qt_x11_wait_for_window_manager(&mdiArea);
            QTest::qWait(100);
#endif
            if (mdiArea.width() >= minimumWidth)
                isSubWindowsPlacedNextToEachOther = true;
        }
    }

    QList<QMdiSubWindow *> subWindows = mdiArea.subWindowList();
    QCOMPARE(subWindows.count(), subWindowCount);

    QMdiSubWindow *testWindow = subWindows.at(3);
    QVERIFY(testWindow);
    QAccessibleInterface *interface = QAccessible::queryAccessibleInterface(testWindow);

    // childCount
    QVERIFY(interface);
    QCOMPARE(interface->childCount(), 1);

    // setText / text
    QCOMPARE(interface->text(QAccessible::Name, 0), QString());
    QCOMPARE(interface->text(QAccessible::Name, 1), QString());
    testWindow->setWindowTitle(QLatin1String("ReplaceMe"));
    QCOMPARE(interface->text(QAccessible::Name, 0), QLatin1String("ReplaceMe"));
    QCOMPARE(interface->text(QAccessible::Name, 1), QLatin1String("ReplaceMe"));
    interface->setText(QAccessible::Name, 0, QLatin1String("TitleSetOnWindow"));
    QCOMPARE(interface->text(QAccessible::Name, 0), QLatin1String("TitleSetOnWindow"));
    interface->setText(QAccessible::Name, 1, QLatin1String("TitleSetOnChild"));
    QCOMPARE(interface->text(QAccessible::Name, 0), QLatin1String("TitleSetOnChild"));

    mdiArea.setActiveSubWindow(testWindow);

    // state
    QAccessible::State state = QAccessible::Normal | QAccessible::Focusable | QAccessible::Focused
                               | QAccessible::Movable | QAccessible::Sizeable;
    QCOMPARE(interface->state(0), state);
    const QRect originalGeometry = testWindow->geometry();
    testWindow->showMaximized();
    state &= ~QAccessible::Sizeable;
    state &= ~QAccessible::Movable;
    QCOMPARE(interface->state(0), state);
    testWindow->showNormal();
    testWindow->move(-10, 0);
    QVERIFY(interface->state(0) & QAccessible::Offscreen);
    testWindow->setVisible(false);
    QVERIFY(interface->state(0) & QAccessible::Invisible);
    testWindow->setVisible(true);
    testWindow->setEnabled(false);
    QVERIFY(interface->state(0) & QAccessible::Unavailable);
    testWindow->setEnabled(true);
    qApp->setActiveWindow(&mdiArea);
    mdiArea.setActiveSubWindow(testWindow);
    testWindow->setFocus();
    QVERIFY(testWindow->isAncestorOf(qApp->focusWidget()));
    QVERIFY(interface->state(0) & QAccessible::Focused);
    testWindow->setGeometry(originalGeometry);

    if (isSubWindowsPlacedNextToEachOther) {
        // This part of the test can only be run if the sub windows are
        // placed next to each other.
        QAccessibleInterface *destination = 0;
        QCOMPARE(interface->navigate(QAccessible::Child, 1, &destination), 0);
        QVERIFY(destination);
        QCOMPARE(destination->object(), (QObject*)testWindow->widget());
        delete destination;
        QCOMPARE(interface->navigate(QAccessible::Left, 0, &destination), 0);
        QVERIFY(destination);
        QCOMPARE(destination->object(), (QObject*)subWindows.at(2));
        delete destination;
        QCOMPARE(interface->navigate(QAccessible::Right, 0, &destination), 0);
        QVERIFY(destination);
        QCOMPARE(destination->object(), (QObject*)subWindows.at(4));
        delete destination;
    }

    // rect
    const QPoint globalPos = testWindow->mapToGlobal(QPoint(0, 0));
    QCOMPARE(interface->rect(0), QRect(globalPos, testWindow->size()));
    testWindow->hide();
    QCOMPARE(interface->rect(0), QRect());
    QCOMPARE(interface->rect(1), QRect());
    testWindow->showMinimized();
    QCOMPARE(interface->rect(1), QRect());
    testWindow->showNormal();
    testWindow->widget()->hide();
    QCOMPARE(interface->rect(1), QRect());
    testWindow->widget()->show();
    const QRect widgetGeometry = testWindow->contentsRect();
    const QPoint globalWidgetPos = QPoint(globalPos.x() + widgetGeometry.x(),
                                          globalPos.y() + widgetGeometry.y());
    QCOMPARE(interface->rect(1), QRect(globalWidgetPos, widgetGeometry.size()));

    // childAt
    QCOMPARE(interface->childAt(-10, 0), -1);
    QCOMPARE(interface->childAt(globalPos.x(), globalPos.y()), 0);
    QCOMPARE(interface->childAt(globalWidgetPos.x(), globalWidgetPos.y()), 1);
    testWindow->widget()->hide();
    QCOMPARE(interface->childAt(globalWidgetPos.x(), globalWidgetPos.y()), 0);

    }
    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::lineEditTest()
{
    QLineEdit *le = new QLineEdit;
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(le);
    QVERIFY(iface);
    le->show();

    QApplication::processEvents();
    QCOMPARE(iface->childCount(), 0);
    QVERIFY(iface->state(0) & QAccessible::Sizeable);
    QVERIFY(iface->state(0) & QAccessible::Movable);
    QVERIFY(iface->state(0) & QAccessible::Focusable);
    QVERIFY(iface->state(0) & QAccessible::Selectable);
    QVERIFY(iface->state(0) & QAccessible::HasPopup);
    QCOMPARE(bool(iface->state(0) & QAccessible::Focused), le->hasFocus());

    QString secret(QLatin1String("secret"));
    le->setText(secret);
    le->setEchoMode(QLineEdit::Normal);
    QVERIFY(!(iface->state(0) & QAccessible::Protected));
    QCOMPARE(iface->text(QAccessible::Value, 0), secret);
    le->setEchoMode(QLineEdit::NoEcho);
    QVERIFY(iface->state(0) & QAccessible::Protected);
    QVERIFY(iface->text(QAccessible::Value, 0).isEmpty());
    le->setEchoMode(QLineEdit::Password);
    QVERIFY(iface->state(0) & QAccessible::Protected);
    QVERIFY(iface->text(QAccessible::Value, 0).isEmpty());
    le->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    QVERIFY(iface->state(0) & QAccessible::Protected);
    QVERIFY(iface->text(QAccessible::Value, 0).isEmpty());
    le->setEchoMode(QLineEdit::Normal);
    QVERIFY(!(iface->state(0) & QAccessible::Protected));
    QCOMPARE(iface->text(QAccessible::Value, 0), secret);

    QWidget *toplevel = new QWidget;
    le->setParent(toplevel);
    toplevel->show();
    QApplication::processEvents();
    QVERIFY(!(iface->state(0) & QAccessible::Sizeable));
    QVERIFY(!(iface->state(0) & QAccessible::Movable));
    QVERIFY(iface->state(0) & QAccessible::Focusable);
    QVERIFY(iface->state(0) & QAccessible::Selectable);
    QVERIFY(iface->state(0) & QAccessible::HasPopup);
    QCOMPARE(bool(iface->state(0) & QAccessible::Focused), le->hasFocus());

    QLineEdit *le2 = new QLineEdit(toplevel);
    le2->show();
    QTest::qWait(100);
    le2->activateWindow();
    QTest::qWait(100);
    le->setFocus(Qt::TabFocusReason);
    QTestAccessibility::clearEvents();
    le2->setFocus(Qt::TabFocusReason);
    QTRY_VERIFY(QTestAccessibility::events().contains(QTestAccessibilityEvent(le2, 0, QAccessible::Focus)));

    le->setText(QLatin1String("500"));
    le->setValidator(new QIntValidator());
    iface->setText(QAccessible::Value, 0, QLatin1String("This text is not a number"));
    QCOMPARE(le->text(), QLatin1String("500"));

    delete iface;
    delete le;
    delete le2;
    QTestAccessibility::clearEvents();

    // IA2
    QString cite = "I always pass on good advice. It is the only thing to do with it. It is never of any use to oneself. --Oscar Wilde";
    QLineEdit *le3 = new QLineEdit(cite, toplevel);
    iface = QAccessible::queryAccessibleInterface(le3);
    QAccessibleTextInterface* textIface = iface->textInterface();
    le3->deselect();
    le3->setCursorPosition(3);
    QCOMPARE(textIface->cursorPosition(), 3);
    QTRY_VERIFY(QTestAccessibility::events().contains(QTestAccessibilityEvent(le3, 0, QAccessible::TextCaretMoved)));
    QCOMPARE(textIface->selectionCount(), 0);
    QTestAccessibility::clearEvents();

    int start, end;
    QCOMPARE(textIface->text(0, 8), QString::fromLatin1("I always"));
    QCOMPARE(textIface->textAtOffset(0, QAccessible2::CharBoundary,&start,&end), QString::fromLatin1("I"));
    QCOMPARE(start, 0);
    QCOMPARE(end, 1);
    QCOMPARE(textIface->textBeforeOffset(0, QAccessible2::CharBoundary,&start,&end), QString());
    QCOMPARE(textIface->textAfterOffset(0, QAccessible2::CharBoundary,&start,&end), QString::fromLatin1(" "));
    QCOMPARE(start, 1);
    QCOMPARE(end, 2);

    QCOMPARE(textIface->textAtOffset(5, QAccessible2::CharBoundary,&start,&end), QString::fromLatin1("a"));
    QCOMPARE(start, 5);
    QCOMPARE(end, 6);
    QCOMPARE(textIface->textBeforeOffset(5, QAccessible2::CharBoundary,&start,&end), QString::fromLatin1("w"));
    QCOMPARE(textIface->textAfterOffset(5, QAccessible2::CharBoundary,&start,&end), QString::fromLatin1("y"));

    QCOMPARE(textIface->textAtOffset(5, QAccessible2::WordBoundary,&start,&end), QString::fromLatin1("always"));
    QCOMPARE(start, 2);
    QCOMPARE(end, 8);

    QCOMPARE(textIface->textAtOffset(2, QAccessible2::WordBoundary,&start,&end), QString::fromLatin1("always"));
    QCOMPARE(textIface->textAtOffset(7, QAccessible2::WordBoundary,&start,&end), QString::fromLatin1("always"));
    QCOMPARE(textIface->textAtOffset(8, QAccessible2::WordBoundary,&start,&end), QString::fromLatin1(" "));
    QCOMPARE(textIface->textAtOffset(25, QAccessible2::WordBoundary,&start,&end), QString::fromLatin1("advice"));
    QCOMPARE(textIface->textAtOffset(92, QAccessible2::WordBoundary,&start,&end), QString::fromLatin1("oneself"));

    QCOMPARE(textIface->textBeforeOffset(5, QAccessible2::WordBoundary,&start,&end), QString::fromLatin1(" "));
    QCOMPARE(textIface->textAfterOffset(5, QAccessible2::WordBoundary,&start,&end), QString::fromLatin1(" "));
    QCOMPARE(textIface->textAtOffset(5, QAccessible2::SentenceBoundary,&start,&end), QString::fromLatin1("I always pass on good advice. "));
    QCOMPARE(start, 0);
    QCOMPARE(end, 30);

    QCOMPARE(textIface->textBeforeOffset(40, QAccessible2::SentenceBoundary,&start,&end), QString::fromLatin1("I always pass on good advice. "));
    QCOMPARE(textIface->textAfterOffset(5, QAccessible2::SentenceBoundary,&start,&end), QString::fromLatin1("It is the only thing to do with it. "));

    QCOMPARE(textIface->textAtOffset(5, QAccessible2::ParagraphBoundary,&start,&end), cite);
    QCOMPARE(start, 0);
    QCOMPARE(end, cite.length());
    QCOMPARE(textIface->textAtOffset(5, QAccessible2::LineBoundary,&start,&end), cite);
    QCOMPARE(textIface->textAtOffset(5, QAccessible2::NoBoundary,&start,&end), cite);


    // characterRect()
    le3->show();
    QTest::qWaitForWindowShown(le3);
    const QRect lineEditRect = iface->rect(0);
    // Only first 10 characters, check if they are within the bounds of line edit
    for (int i = 0; i < 10; ++i) {
        QVERIFY(lineEditRect.contains(textIface->characterRect(i, QAccessible2::RelativeToScreen)));
    }

    delete iface;
    delete toplevel;
    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::groupBoxTest()
{
    QGroupBox *groupBox = new QGroupBox();
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(groupBox);

    groupBox->setTitle(QLatin1String("Test QGroupBox"));
    groupBox->setToolTip(QLatin1String("This group box will be used to test accessibility"));
    QVBoxLayout *layout = new QVBoxLayout();
    QRadioButton *rbutton = new QRadioButton();
    layout->addWidget(rbutton);
    groupBox->setLayout(layout);
    QAccessibleInterface *rButtonIface = QAccessible::queryAccessibleInterface(rbutton);

    QCOMPARE(iface->childCount(), 1);
    QCOMPARE(iface->role(0), QAccessible::Grouping);
    QCOMPARE(iface->text(QAccessible::Name, 0), QLatin1String("Test QGroupBox"));
    QCOMPARE(iface->text(QAccessible::Description, 0), QLatin1String("This group box will be used to test accessibility"));
    QAccessible::Relation relation = iface->relationTo(0, rButtonIface, 0);
    QVERIFY(relation & QAccessible::Label);

    delete rButtonIface;
    delete iface;
    delete groupBox;

    groupBox = new QGroupBox();
    iface = QAccessible::queryAccessibleInterface(groupBox);

    groupBox->setCheckable(true);
    groupBox->setChecked(false);

    QCOMPARE(iface->role(0), QAccessible::CheckBox);
    QAccessibleActionInterface *actionIface = iface->actionInterface();
    QVERIFY(actionIface);
    QAccessible::State state = iface->state(0);
    QVERIFY(!(state & QAccessible::Checked));
    actionIface->doAction(0);
    QVERIFY(groupBox->isChecked());
    state = iface->state(0);
    QVERIFY(state & QAccessible::Checked);

    delete iface;
    delete groupBox;
}

void tst_QAccessibility::workspaceTest()
{
    {
    QWorkspace workspace;
    workspace.resize(400,300);
    workspace.show();
    const int subWindowCount =  3;
    for (int i = 0; i < subWindowCount; ++i) {
        QWidget *window = workspace.addWindow(new QWidget);
        if (i > 0)
            window->move(window->x() + 1, window->y());
        window->show();
        window->resize(70, window->height());
    }

    QWidgetList subWindows = workspace.windowList();
    QCOMPARE(subWindows.count(), subWindowCount);

    QAccessibleInterface *interface = QAccessible::queryAccessibleInterface(&workspace);
    QVERIFY(interface);
    QCOMPARE(interface->childCount(), subWindowCount);

    // Right, right, right, ...
    for (int i = 0; i < subWindowCount; ++i) {
        QAccessibleInterface *destination = 0;
        int index = interface->navigate(QAccessible::Right, i + 1, &destination);
        if (i == subWindowCount - 1) {
            QVERIFY(!destination);
            QCOMPARE(index, -1);
        } else {
            QVERIFY(destination);
            QCOMPARE(index, 0);
            QCOMPARE(destination->object(), (QObject*)subWindows.at(i + 1));
            delete destination;
        }
    }

    // Left, left, left, ...
    for (int i = subWindowCount; i > 0; --i) {
        QAccessibleInterface *destination = 0;
        int index = interface->navigate(QAccessible::Left, i, &destination);
        if (i == 1) {
            QVERIFY(!destination);
            QCOMPARE(index, -1);
        } else {
            QVERIFY(destination);
            QCOMPARE(index, 0);
            QCOMPARE(destination->object(), (QObject*)subWindows.at(i - 2));
            delete destination;
        }
    }
    // ### Add test for Up and Down.

    }
    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::dialogButtonBoxTest()
{
    {
    QDialogButtonBox box(QDialogButtonBox::Reset |
                         QDialogButtonBox::Help |
                         QDialogButtonBox::Ok, Qt::Horizontal);


    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&box);
    QVERIFY(iface);
    box.show();
#if defined(Q_WS_X11)
    qt_x11_wait_for_window_manager(&box);
    QTest::qWait(100);
#endif

    QApplication::processEvents();
    QCOMPARE(iface->childCount(), 3);
    QCOMPARE(iface->role(0), QAccessible::Grouping);
    QCOMPARE(iface->role(1), QAccessible::PushButton);
    QCOMPARE(iface->role(2), QAccessible::PushButton);
    QCOMPARE(iface->role(3), QAccessible::PushButton);
    QStringList actualOrder;
    QAccessibleInterface *child;
    QAccessibleInterface *leftmost;
    iface->navigate(QAccessible::Child, 1, &child);
    // first find the leftmost button
    while (child->navigate(QAccessible::Left, 1, &leftmost) != -1) {
        delete child;
        child = leftmost;
    }
    leftmost = child;

    // then traverse from left to right to find the correct order of the buttons
    int right = 0;
    while (right != -1) {
        actualOrder << leftmost->text(QAccessible::Name, 0);
        right = leftmost->navigate(QAccessible::Right, 1, &child);
        delete leftmost;
        leftmost = child;
    }

    QStringList expectedOrder;
    QDialogButtonBox::ButtonLayout btnlout =
        QDialogButtonBox::ButtonLayout(QApplication::style()->styleHint(QStyle::SH_DialogButtonLayout));
    switch (btnlout) {
    case QDialogButtonBox::WinLayout:
        expectedOrder << QDialogButtonBox::tr("Reset")
                      << QDialogButtonBox::tr("OK")
                      << QDialogButtonBox::tr("Help");
        break;
    case QDialogButtonBox::GnomeLayout:
    case QDialogButtonBox::KdeLayout:
    case QDialogButtonBox::MacLayout:
        expectedOrder << QDialogButtonBox::tr("Help")
                      << QDialogButtonBox::tr("Reset")
                      << QDialogButtonBox::tr("OK");
        break;
    }
    QCOMPARE(actualOrder, expectedOrder);
    delete iface;
    QApplication::processEvents();
    QTestAccessibility::clearEvents();
    }

    {
    QDialogButtonBox box(QDialogButtonBox::Reset |
                         QDialogButtonBox::Help |
                         QDialogButtonBox::Ok, Qt::Horizontal);
    setFrameless(&box);


    // Test up and down navigation
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&box);
    QVERIFY(iface);
    box.setOrientation(Qt::Vertical);
    box.show();
#if defined(Q_WS_X11)
    qt_x11_wait_for_window_manager(&box);
    QTest::qWait(100);
#endif

    QApplication::processEvents();
    QAccessibleInterface *child;
    QStringList actualOrder;
    iface->navigate(QAccessible::Child, 1, &child);
    // first find the topmost button
    QAccessibleInterface *other;
    while (child->navigate(QAccessible::Up, 1, &other) != -1) {
        delete child;
        child = other;
    }
    other = child;

    // then traverse from top to bottom to find the correct order of the buttons
    actualOrder.clear();
    int right = 0;
    while (right != -1) {
        actualOrder << other->text(QAccessible::Name, 0);
        right = other->navigate(QAccessible::Down, 1, &child);
        delete other;
        other = child;
    }

    QStringList expectedOrder;
    expectedOrder << QDialogButtonBox::tr("OK")
                  << QDialogButtonBox::tr("Reset")
                  << QDialogButtonBox::tr("Help");

    QCOMPARE(actualOrder, expectedOrder);
    delete iface;
    QApplication::processEvents();

    }
    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::dialTest()
{
    {
    QDial dial;
    setFrameless(&dial);
    dial.setValue(20);
    QCOMPARE(dial.value(), 20);
    dial.show();
#if defined(Q_WS_X11)
    qt_x11_wait_for_window_manager(&dial);
    QTest::qWait(100);
#endif

    QAccessibleInterface *interface = QAccessible::queryAccessibleInterface(&dial);
    QVERIFY(interface);

    // Child count; 1 = SpeedoMeter, 2 = SliderHandle.
    QCOMPARE(interface->childCount(), 2);

    QCOMPARE(interface->role(0), QAccessible::Dial);
    QCOMPARE(interface->role(1), QAccessible::Slider);
    QCOMPARE(interface->role(2), QAccessible::Indicator);

    QCOMPARE(interface->text(QAccessible::Value, 0), QString::number(dial.value()));
    QCOMPARE(interface->text(QAccessible::Value, 1), QString::number(dial.value()));
    QCOMPARE(interface->text(QAccessible::Value, 2), QString::number(dial.value()));
    QCOMPARE(interface->text(QAccessible::Name, 0), QLatin1String("QDial"));
    QCOMPARE(interface->text(QAccessible::Name, 1), QLatin1String("SpeedoMeter"));
    QCOMPARE(interface->text(QAccessible::Name, 2), QLatin1String("SliderHandle"));
    QCOMPARE(interface->text(QAccessible::Name, 3), QLatin1String(""));

    QCOMPARE(interface->state(1), interface->state(0));
    QCOMPARE(interface->state(2), interface->state(0) | QAccessible::HotTracked);

    // Rect
    QCOMPARE(interface->rect(0), dial.geometry());
    QVERIFY(interface->rect(1).isValid());
    QVERIFY(dial.geometry().contains(interface->rect(1)));
    QVERIFY(interface->rect(2).isValid());
    QVERIFY(interface->rect(1).contains(interface->rect(2)));
    QVERIFY(!interface->rect(3).isValid());

    }
    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::rubberBandTest()
{
    QRubberBand rubberBand(QRubberBand::Rectangle);
    QAccessibleInterface *interface = QAccessible::queryAccessibleInterface(&rubberBand);
    QVERIFY(interface);
    QCOMPARE(interface->role(0), QAccessible::Border);
    delete interface;
    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::abstractScrollAreaTest()
{
    {
    QAbstractScrollArea abstractScrollArea;

    QAccessibleInterface *interface = QAccessible::queryAccessibleInterface(&abstractScrollArea);
    QVERIFY(interface);
    QVERIFY(!interface->rect(0).isValid());
    QVERIFY(!interface->rect(1).isValid());
    QCOMPARE(interface->childAt(200, 200), -1);

    abstractScrollArea.resize(400, 400);
    abstractScrollArea.show();
#if defined(Q_WS_X11)
    qt_x11_wait_for_window_manager(&abstractScrollArea);
    QTest::qWait(100);
#endif
    const QRect globalGeometry = QRect(abstractScrollArea.mapToGlobal(QPoint(0, 0)),
                                       abstractScrollArea.size());

    // Viewport.
    QCOMPARE(interface->childCount(), 1);
    QWidget *viewport = abstractScrollArea.viewport();
    QVERIFY(viewport);
    QVERIFY(verifyChild(viewport, interface, 1, globalGeometry));

    // Horizontal scrollBar.
    abstractScrollArea.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    QCOMPARE(interface->childCount(), 2);
    QWidget *horizontalScrollBar = abstractScrollArea.horizontalScrollBar();
    QWidget *horizontalScrollBarContainer = horizontalScrollBar->parentWidget();
    QVERIFY(verifyChild(horizontalScrollBarContainer, interface, 2, globalGeometry));

    // Horizontal scrollBar widgets.
    QLabel *secondLeftLabel = new QLabel(QLatin1String("L2"));
    abstractScrollArea.addScrollBarWidget(secondLeftLabel, Qt::AlignLeft);
    QCOMPARE(interface->childCount(), 2);

    QLabel *firstLeftLabel = new QLabel(QLatin1String("L1"));
    abstractScrollArea.addScrollBarWidget(firstLeftLabel, Qt::AlignLeft);
    QCOMPARE(interface->childCount(), 2);

    QLabel *secondRightLabel = new QLabel(QLatin1String("R2"));
    abstractScrollArea.addScrollBarWidget(secondRightLabel, Qt::AlignRight);
    QCOMPARE(interface->childCount(), 2);

    QLabel *firstRightLabel = new QLabel(QLatin1String("R1"));
    abstractScrollArea.addScrollBarWidget(firstRightLabel, Qt::AlignRight);
    QCOMPARE(interface->childCount(), 2);

    // Vertical scrollBar.
    abstractScrollArea.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    QCOMPARE(interface->childCount(), 3);
    QWidget *verticalScrollBar = abstractScrollArea.verticalScrollBar();
    QWidget *verticalScrollBarContainer = verticalScrollBar->parentWidget();
    QVERIFY(verifyChild(verticalScrollBarContainer, interface, 3, globalGeometry));

    // Vertical scrollBar widgets.
    QLabel *secondTopLabel = new QLabel(QLatin1String("T2"));
    abstractScrollArea.addScrollBarWidget(secondTopLabel, Qt::AlignTop);
    QCOMPARE(interface->childCount(), 3);

    QLabel *firstTopLabel = new QLabel(QLatin1String("T1"));
    abstractScrollArea.addScrollBarWidget(firstTopLabel, Qt::AlignTop);
    QCOMPARE(interface->childCount(), 3);

    QLabel *secondBottomLabel = new QLabel(QLatin1String("B2"));
    abstractScrollArea.addScrollBarWidget(secondBottomLabel, Qt::AlignBottom);
    QCOMPARE(interface->childCount(), 3);

    QLabel *firstBottomLabel = new QLabel(QLatin1String("B1"));
    abstractScrollArea.addScrollBarWidget(firstBottomLabel, Qt::AlignBottom);
    QCOMPARE(interface->childCount(), 3);

    // CornerWidget.
    abstractScrollArea.setCornerWidget(new QLabel(QLatin1String("C")));
    QCOMPARE(interface->childCount(), 4);
    QWidget *cornerWidget = abstractScrollArea.cornerWidget();
    QVERIFY(verifyChild(cornerWidget, interface, 4, globalGeometry));

    // Test navigate.
    QAccessibleInterface *target = 0;

    // viewport -> Up -> NOTHING
    const int viewportIndex = indexOfChild(interface, viewport);
    QVERIFY(viewportIndex != -1);
    QCOMPARE(interface->navigate(QAccessible::Up, viewportIndex, &target), -1);
    QVERIFY(!target);

    // viewport -> Left -> NOTHING
    QCOMPARE(interface->navigate(QAccessible::Left, viewportIndex, &target), -1);
    QVERIFY(!target);

    // viewport -> Down -> horizontalScrollBarContainer
    const int horizontalScrollBarContainerIndex = indexOfChild(interface, horizontalScrollBarContainer);
    QVERIFY(horizontalScrollBarContainerIndex != -1);
    QCOMPARE(interface->navigate(QAccessible::Down, viewportIndex, &target), 0);
    QVERIFY(target);
    QCOMPARE(target->object(), static_cast<QObject *>(horizontalScrollBarContainer));
    delete target;
    target = 0;

    // horizontalScrollBarContainer -> Left -> NOTHING
    QCOMPARE(interface->navigate(QAccessible::Left, horizontalScrollBarContainerIndex, &target), -1);
    QVERIFY(!target);

    // horizontalScrollBarContainer -> Down -> NOTHING
    QVERIFY(horizontalScrollBarContainerIndex != -1);
    QCOMPARE(interface->navigate(QAccessible::Down, horizontalScrollBarContainerIndex, &target), -1);
    QVERIFY(!target);

    // horizontalScrollBarContainer -> Right -> cornerWidget
    const int cornerWidgetIndex = indexOfChild(interface, cornerWidget);
    QVERIFY(cornerWidgetIndex != -1);
    QCOMPARE(interface->navigate(QAccessible::Right, horizontalScrollBarContainerIndex, &target), 0);
    QVERIFY(target);
    QCOMPARE(target->object(), static_cast<QObject *>(cornerWidget));
    delete target;
    target = 0;

    // cornerWidget -> Down -> NOTHING
    QCOMPARE(interface->navigate(QAccessible::Down, cornerWidgetIndex, &target), -1);
    QVERIFY(!target);

    // cornerWidget -> Right -> NOTHING
    QVERIFY(cornerWidgetIndex != -1);
    QCOMPARE(interface->navigate(QAccessible::Right, cornerWidgetIndex, &target), -1);
    QVERIFY(!target);

    // cornerWidget -> Up ->  verticalScrollBarContainer
    const int verticalScrollBarContainerIndex = indexOfChild(interface, verticalScrollBarContainer);
    QVERIFY(verticalScrollBarContainerIndex != -1);
    QCOMPARE(interface->navigate(QAccessible::Up, cornerWidgetIndex, &target), 0);
    QVERIFY(target);
    QCOMPARE(target->object(), static_cast<QObject *>(verticalScrollBarContainer));
    delete target;
    target = 0;

    // verticalScrollBarContainer -> Right -> NOTHING
    QCOMPARE(interface->navigate(QAccessible::Right, verticalScrollBarContainerIndex, &target), -1);
    QVERIFY(!target);

    // verticalScrollBarContainer -> Up -> NOTHING
    QCOMPARE(interface->navigate(QAccessible::Up, verticalScrollBarContainerIndex, &target), -1);
    QVERIFY(!target);

    // verticalScrollBarContainer -> Left -> viewport
    QCOMPARE(interface->navigate(QAccessible::Left, verticalScrollBarContainerIndex, &target), 0);
    QVERIFY(target);
    QCOMPARE(target->object(), static_cast<QObject *>(viewport));
    delete target;
    target = 0;

    QCOMPARE(verifyHierarchy(interface), 0);

    delete interface;
    }

    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::scrollAreaTest()
{
    {
    QScrollArea scrollArea;
    scrollArea.show();
#if defined(Q_WS_X11)
    qt_x11_wait_for_window_manager(&scrollArea);
    QTest::qWait(100);
#endif
    QAccessibleInterface *interface = QAccessible::queryAccessibleInterface(&scrollArea);
    QVERIFY(interface);
    QCOMPARE(interface->childCount(), 1); // The viewport.
    delete interface;
    }
    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::tableWidgetTest()
{
#if defined(Q_WS_X11)
    QSKIP( "Accessible table1 interface is no longer supported on X11.", SkipAll);
#else
    {
    QWidget *topLevel = new QWidget;
    QTableWidget *w = new QTableWidget(8,4,topLevel);
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 4; ++c) {
            w->setItem(r, c, new QTableWidgetItem(tr("%1,%2").arg(c).arg(r)));
        }
    }
    w->resize(100, 100);
    topLevel->show();
#if defined(Q_WS_X11)
    qt_x11_wait_for_window_manager(w);
    QTest::qWait(100);
#endif
    QAccessibleInterface *client = QAccessible::queryAccessibleInterface(w);
    QCOMPARE(client->role(0), QAccessible::Client);
    QCOMPARE(client->childCount(), 3);
    QAccessibleInterface *view = 0;
    client->navigate(QAccessible::Child, 1, &view);
    QCOMPARE(view->role(0), QAccessible::Table);
    QAccessibleInterface *ifRow;
    view->navigate(QAccessible::Child, 2, &ifRow);
    QCOMPARE(ifRow->role(0), QAccessible::Row);
    QAccessibleInterface *item;
    int entry = ifRow->navigate(QAccessible::Child, 1, &item);
    QCOMPARE(entry, 1);
    QCOMPARE(item , (QAccessibleInterface*)0);
    QCOMPARE(ifRow->text(QAccessible::Name, 2), QLatin1String("0,0"));
    QCOMPARE(ifRow->text(QAccessible::Name, 3), QLatin1String("1,0"));

    QCOMPARE(verifyHierarchy(client),  0);

    delete ifRow;
    delete view;
    delete client;
    delete w;
    delete topLevel;
    }
    QTestAccessibility::clearEvents();
#endif
}

class QtTestTableModel: public QAbstractTableModel
{
    Q_OBJECT

signals:
    void invalidIndexEncountered() const;

public:
    QtTestTableModel(int rows = 0, int columns = 0, QObject *parent = 0)
        : QAbstractTableModel(parent),
          row_count(rows),
          column_count(columns) {}

    int rowCount(const QModelIndex& = QModelIndex()) const { return row_count; }
    int columnCount(const QModelIndex& = QModelIndex()) const { return column_count; }

    QVariant data(const QModelIndex &idx, int role) const
    {
        if (!idx.isValid() || idx.row() >= row_count || idx.column() >= column_count) {
            qWarning() << "Invalid modelIndex [%d,%d,%p]" << idx;
            emit invalidIndexEncountered();
            return QVariant();
        }

        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return QString("[%1,%2,%3]").arg(idx.row()).arg(idx.column()).arg(0);

        return QVariant();
    }

    void removeLastRow()
    {
        beginRemoveRows(QModelIndex(), row_count - 1, row_count - 1);
        --row_count;
        endRemoveRows();
    }

    void removeAllRows()
    {
        beginRemoveRows(QModelIndex(), 0, row_count - 1);
        row_count = 0;
        endRemoveRows();
    }

    void removeLastColumn()
    {
        beginRemoveColumns(QModelIndex(), column_count - 1, column_count - 1);
        --column_count;
        endRemoveColumns();
    }

    void removeAllColumns()
    {
        beginRemoveColumns(QModelIndex(), 0, column_count - 1);
        column_count = 0;
        endRemoveColumns();
    }

    void reset()
    {
        QAbstractTableModel::reset();
    }

    int row_count;
    int column_count;
};

class QtTestDelegate : public QItemDelegate
{
public:
    QtTestDelegate(QWidget *parent = 0) : QItemDelegate(parent) {}

    virtual QSize sizeHint(const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const
    {
        return QSize(100,50);
    }
};

void tst_QAccessibility::tableViewTest()
{
#if defined(Q_WS_X11)
    QSKIP( "Accessible table1 interface is no longer supported on X11.", SkipAll);
#else
    {
    QtTestTableModel *model = new QtTestTableModel(3, 4);
    QTableView *w = new QTableView();
    w->setModel(model);
    w->setItemDelegate(new QtTestDelegate(w));
    w->resize(450,200);
    w->resizeColumnsToContents();
    w->resizeRowsToContents();
    w->show();
#if defined(Q_WS_X11)
    qt_x11_wait_for_window_manager(w);
    QTest::qWait(100);
#endif
    QAccessibleInterface *client = QAccessible::queryAccessibleInterface(w);
    QAccessibleInterface *table2;
    client->navigate(QAccessible::Child, 1, &table2);
    QVERIFY(table2);
    QCOMPARE(table2->role(1), QAccessible::Row);
    QAccessibleInterface *toprow = 0;
    table2->navigate(QAccessible::Child, 1, &toprow);
    QVERIFY(toprow);
    QCOMPARE(toprow->role(1), QAccessible::RowHeader);
    QCOMPARE(toprow->role(2), QAccessible::ColumnHeader);
    delete toprow;

    // call childAt() for each child until we reach the bottom,
    // and do it for each row in the table
    for (int y = 1; y < 5; ++y) {  // this includes the special header
        for (int x = 1; x < 6; ++x) {
            QCOMPARE(client->role(0), QAccessible::Client);
            QRect globalRect = client->rect(0);
            QVERIFY(globalRect.isValid());
            // make sure we don't hit the vertical header  #####
            QPoint p = globalRect.topLeft() + QPoint(8, 8);
            p.ry() += 50 * (y - 1);
            p.rx() += 100 * (x - 1);
            int index = client->childAt(p.x(), p.y());
            QCOMPARE(index, 1);
            QCOMPARE(client->role(index), QAccessible::Table);

            // navigate to table/viewport
            QAccessibleInterface *table;
            client->navigate(QAccessible::Child, index, &table);
            QVERIFY(table);
            index = table->childAt(p.x(), p.y());
            QCOMPARE(index, y);
            QCOMPARE(table->role(index), QAccessible::Row);
            QAccessibleInterface *row;
            QCOMPARE(table->role(1), QAccessible::Row);

            // navigate to the row
            table->navigate(QAccessible::Child, index, &row);
            QVERIFY(row);
            QCOMPARE(row->role(1), QAccessible::RowHeader);
            index = row->childAt(p.x(), p.y());
            QVERIFY(index > 0);
            if (x == 1 && y == 1) {
                QCOMPARE(row->role(index), QAccessible::RowHeader);
                QCOMPARE(row->text(QAccessible::Name, index), QLatin1String(""));
            } else if (x > 1 && y > 1) {
                QCOMPARE(row->role(index), QAccessible::Cell);
                QCOMPARE(row->text(QAccessible::Name, index), QString::fromAscii("[%1,%2,0]").arg(y - 2).arg(x - 2));
            } else if (x == 1) {
                QCOMPARE(row->role(index), QAccessible::RowHeader);
                QCOMPARE(row->text(QAccessible::Name, index), QString::fromAscii("%1").arg(y - 1));
            } else if (y == 1) {
                QCOMPARE(row->role(index), QAccessible::ColumnHeader);
                QCOMPARE(row->text(QAccessible::Name, index), QString::fromAscii("%1").arg(x - 1));
            }
            delete table;
            delete row;
        }
    }
    delete table2;
    delete client;
    delete w;
    delete model;
    }
    QTestAccessibility::clearEvents();
#endif
}

void tst_QAccessibility::table2ListTest()
{
#if !defined(Q_WS_X11)
    QSKIP( "Accessible table2 interface is currently only supported on X11.", SkipAll);
#else
    QListWidget *listView = new QListWidget;
    listView->addItem("Oslo");
    listView->addItem("Berlin");
    listView->addItem("Brisbane");
    listView->resize(400,400);
    listView->show();
    QTest::qWait(1); // Need this for indexOfchild to work.
#if defined(Q_WS_X11)
    qt_x11_wait_for_window_manager(listView);
    QTest::qWait(100);
#endif

    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(listView);
    QCOMPARE(verifyHierarchy(iface), 0);

    QCOMPARE((int)iface->role(0), (int)QAccessible::List);
    QCOMPARE(iface->childCount(), 3);

    QAccessibleInterface *child1 = 0;
    QCOMPARE(iface->navigate(QAccessible::Child, 1, &child1), 0);
    QVERIFY(child1);
    QCOMPARE(iface->indexOfChild(child1), 1);
    QCOMPARE(child1->text(QAccessible::Name, 0), QString("Oslo"));
    QCOMPARE(child1->role(0), QAccessible::ListItem);
    delete child1;

    QAccessibleInterface *child2 = 0;
    QCOMPARE(iface->navigate(QAccessible::Child, 2, &child2), 0);
    QVERIFY(child2);
    QCOMPARE(iface->indexOfChild(child2), 2);
    QCOMPARE(child2->text(QAccessible::Name, 0), QString("Berlin"));
    delete child2;

    QAccessibleInterface *child3 = 0;
    QCOMPARE(iface->navigate(QAccessible::Child, 3, &child3), 0);
    QVERIFY(child3);
    QCOMPARE(iface->indexOfChild(child3), 3);
    QCOMPARE(child3->text(QAccessible::Name, 0), QString("Brisbane"));
    delete child3;
    QTestAccessibility::clearEvents();

    // Check for events
    QTest::mouseClick(listView->viewport(), Qt::LeftButton, 0, listView->visualItemRect(listView->item(1)).center());
    QVERIFY(QTestAccessibility::events().contains(QTestAccessibilityEvent(listView, 2, QAccessible::Selection)));
    QVERIFY(QTestAccessibility::events().contains(QTestAccessibilityEvent(listView, 2, QAccessible::Focus)));
    QTest::mouseClick(listView->viewport(), Qt::LeftButton, 0, listView->visualItemRect(listView->item(2)).center());
    QVERIFY(QTestAccessibility::events().contains(QTestAccessibilityEvent(listView, 3, QAccessible::Selection)));
    QVERIFY(QTestAccessibility::events().contains(QTestAccessibilityEvent(listView, 3, QAccessible::Focus)));

    listView->addItem("Munich");
    QCOMPARE(iface->childCount(), 4);

    // table 2
    QAccessibleTable2Interface *table2 = iface->table2Interface();
    QVERIFY(table2);
    QCOMPARE(table2->columnCount(), 1);
    QCOMPARE(table2->rowCount(), 4);
    QAccessibleTable2CellInterface *cell1;
    QVERIFY(cell1 = table2->cellAt(0,0));
    QCOMPARE(cell1->text(QAccessible::Name, 0), QString("Oslo"));
    QAccessibleTable2CellInterface *cell4;
    QVERIFY(cell4 = table2->cellAt(3,0));
    QCOMPARE(cell4->text(QAccessible::Name, 0), QString("Munich"));
    QCOMPARE(cell4->role(0), QAccessible::ListItem);
    QCOMPARE(cell4->rowIndex(), 3);
    QCOMPARE(cell4->columnIndex(), 0);
    QVERIFY(!cell4->isExpandable());

    delete cell4;
    delete cell1;
    delete iface;
    delete listView;
    QTestAccessibility::clearEvents();
#endif
}

void tst_QAccessibility::table2TreeTest()
{
#if !defined(Q_WS_X11)
    QSKIP( "Accessible table2 interface is currently only supported on X11.", SkipAll);
#else
    QTreeWidget *treeView = new QTreeWidget;
    treeView->setColumnCount(2);
    QTreeWidgetItem *header = new QTreeWidgetItem;
    header->setText(0, "Artist");
    header->setText(1, "Work");
    treeView->setHeaderItem(header);

    QTreeWidgetItem *root1 = new QTreeWidgetItem;
    root1->setText(0, "Spain");
    treeView->addTopLevelItem(root1);

    QTreeWidgetItem *item1 = new QTreeWidgetItem;
    item1->setText(0, "Picasso");
    item1->setText(1, "Guernica");
    root1->addChild(item1);

    QTreeWidgetItem *item2 = new QTreeWidgetItem;
    item2->setText(0, "Tapies");
    item2->setText(1, "Ambrosia");
    root1->addChild(item2);

    QTreeWidgetItem *root2 = new QTreeWidgetItem;
    root2->setText(0, "Austria");
    treeView->addTopLevelItem(root2);

    QTreeWidgetItem *item3 = new QTreeWidgetItem;
    item3->setText(0, "Klimt");
    item3->setText(1, "The Kiss");
    root2->addChild(item3);

    treeView->resize(400,400);
    treeView->show();
    QTest::qWait(1); // Need this for indexOfchild to work.
#if defined(Q_WS_X11)
    qt_x11_wait_for_window_manager(treeView);
    QTest::qWait(100);
#endif

    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(treeView);
    QEXPECT_FAIL("", "Implement Sibling navigation for table2 cells.", Continue);
    QCOMPARE(verifyHierarchy(iface), 0);

    QCOMPARE((int)iface->role(0), (int)QAccessible::Tree);
    // header and 2 rows (the others are not expanded, thus not visible)
    QCOMPARE(iface->childCount(), 6);

    QAccessibleInterface *header1 = 0;
    QCOMPARE(iface->navigate(QAccessible::Child, 1, &header1), 0);
    QVERIFY(header1);
    QCOMPARE(iface->indexOfChild(header1), 1);
    QCOMPARE(header1->text(QAccessible::Name, 0), QString("Artist"));
    QCOMPARE(header1->role(0), QAccessible::ColumnHeader);
    delete header1;

    QAccessibleInterface *child1 = 0;
    QCOMPARE(iface->navigate(QAccessible::Child, 3, &child1), 0);
    QVERIFY(child1);
    QCOMPARE(iface->indexOfChild(child1), 3);
    QCOMPARE(child1->text(QAccessible::Name, 0), QString("Spain"));
    QCOMPARE(child1->role(0), QAccessible::TreeItem);
    QVERIFY(!(child1->state(0) & QAccessible::Expanded));
    delete child1;

    QAccessibleInterface *child2 = 0;
    QCOMPARE(iface->navigate(QAccessible::Child, 5, &child2), 0);
    QVERIFY(child2);
    QCOMPARE(iface->indexOfChild(child2), 5);
    QCOMPARE(child2->text(QAccessible::Name, 0), QString("Austria"));
    delete child2;

    QTestAccessibility::clearEvents();

    // table 2
    QAccessibleTable2Interface *table2 = iface->table2Interface();
    QVERIFY(table2);
    QCOMPARE(table2->columnCount(), 2);
    QCOMPARE(table2->rowCount(), 2);
    QAccessibleTable2CellInterface *cell1;
    QVERIFY(cell1 = table2->cellAt(0,0));
    QCOMPARE(cell1->text(QAccessible::Name, 0), QString("Spain"));
    QAccessibleTable2CellInterface *cell2;
    QVERIFY(cell2 = table2->cellAt(1,0));
    QCOMPARE(cell2->text(QAccessible::Name, 0), QString("Austria"));
    QCOMPARE(cell2->role(0), QAccessible::TreeItem);
    QCOMPARE(cell2->rowIndex(), 1);
    QCOMPARE(cell2->columnIndex(), 0);
    QVERIFY(cell2->isExpandable());
    QCOMPARE(iface->indexOfChild(cell2), 5);
    QVERIFY(!(cell2->state(0) & QAccessible::Expanded));
    QCOMPARE(table2->columnDescription(1), QString("Work"));
    delete cell2;
    delete cell1;

    treeView->expandAll();

    QTest::qWait(1); // Need this for indexOfchild to work.
#if defined(Q_WS_X11)
    qt_x11_wait_for_window_manager(treeView);
    QTest::qWait(100);
#endif

    QCOMPARE(table2->columnCount(), 2);
    QCOMPARE(table2->rowCount(), 5);
    cell1 = table2->cellAt(1,0);
    QCOMPARE(cell1->text(QAccessible::Name, 0), QString("Picasso"));
    QCOMPARE(iface->indexOfChild(cell1), 5); // 1 based + 2 header + 2 for root item

    cell2 = table2->cellAt(4,0);
    QCOMPARE(cell2->text(QAccessible::Name, 0), QString("Klimt"));
    QCOMPARE(cell2->role(0), QAccessible::TreeItem);
    QCOMPARE(cell2->rowIndex(), 4);
    QCOMPARE(cell2->columnIndex(), 0);
    QVERIFY(!cell2->isExpandable());
    QCOMPARE(iface->indexOfChild(cell2), 11);

    QCOMPARE(table2->columnDescription(0), QString("Artist"));
    QCOMPARE(table2->columnDescription(1), QString("Work"));

    delete iface;
    QTestAccessibility::clearEvents();
#endif
}


void tst_QAccessibility::table2TableTest()
{
#if !defined(Q_WS_X11)
    QSKIP( "Accessible table2 interface is currently only supported on X11.", SkipAll);
#else
    QTableWidget *tableView = new QTableWidget(3, 3);
    tableView->setColumnCount(3);
    QStringList hHeader;
    hHeader << "h1" << "h2" << "h3";
    tableView->setHorizontalHeaderLabels(hHeader);

    QStringList vHeader;
    vHeader << "v1" << "v2" << "v3";
    tableView->setVerticalHeaderLabels(vHeader);

    for (int i = 0; i<9; ++i) {
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText(QString::number(i/3) + QString(".") + QString::number(i%3));
        tableView->setItem(i/3, i%3, item);
    }

    tableView->resize(600,600);
    tableView->show();
    QTest::qWait(1); // Need this for indexOfchild to work.
#if defined(Q_WS_X11)
    qt_x11_wait_for_window_manager(tableView);
    QTest::qWait(100);
#endif

    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(tableView);
    QEXPECT_FAIL("", "Implement Sibling navigation for table2 cells.", Continue);
    QCOMPARE(verifyHierarchy(iface), 0);

    QCOMPARE((int)iface->role(0), (int)QAccessible::Table);
    // header and 2 rows (the others are not expanded, thus not visible)
    QCOMPARE(iface->childCount(), 9+3+3+1); // cell+headers+topleft button

    QAccessibleInterface *cornerButton = 0;
    QCOMPARE(iface->navigate(QAccessible::Child, 1, &cornerButton), 0);
    QVERIFY(cornerButton);
    QCOMPARE(iface->indexOfChild(cornerButton), 1);
    QCOMPARE(cornerButton->role(0), QAccessible::Pane);
    delete cornerButton;

    QAccessibleInterface *child1 = 0;
    QCOMPARE(iface->navigate(QAccessible::Child, 3, &child1), 0);
    QVERIFY(child1);
    QCOMPARE(iface->indexOfChild(child1), 3);
    QCOMPARE(child1->text(QAccessible::Name, 0), QString("h2"));
    QCOMPARE(child1->role(0), QAccessible::ColumnHeader);
    QVERIFY(!(child1->state(0) & QAccessible::Expanded));
    delete child1;

    QAccessibleInterface *child2 = 0;
    QCOMPARE(iface->navigate(QAccessible::Child, 11, &child2), 0);
    QVERIFY(child2);
    QCOMPARE(iface->indexOfChild(child2), 11);
    QCOMPARE(child2->text(QAccessible::Name, 0), QString("1.1"));
    QAccessibleTable2CellInterface *cell2Iface = static_cast<QAccessibleTable2CellInterface*>(child2);
    QCOMPARE(cell2Iface->rowIndex(), 1);
    QCOMPARE(cell2Iface->columnIndex(), 1);
    delete child2;

    QAccessibleInterface *child3 = 0;
    QCOMPARE(iface->navigate(QAccessible::Child, 12, &child3), 0);
    QCOMPARE(iface->indexOfChild(child3), 12);
    QCOMPARE(child3->text(QAccessible::Name, 0), QString("1.2"));
    delete child3;

    QTestAccessibility::clearEvents();

    // table 2
    QAccessibleTable2Interface *table2 = iface->table2Interface();
    QVERIFY(table2);
    QCOMPARE(table2->columnCount(), 3);
    QCOMPARE(table2->rowCount(), 3);
    QAccessibleTable2CellInterface *cell1;
    QVERIFY(cell1 = table2->cellAt(0,0));
    QCOMPARE(cell1->text(QAccessible::Name, 0), QString("0.0"));
    QCOMPARE(iface->indexOfChild(cell1), 6);

    QAccessibleTable2CellInterface *cell2;
    QVERIFY(cell2 = table2->cellAt(0,1));
    QCOMPARE(cell2->text(QAccessible::Name, 0), QString("0.1"));
    QCOMPARE(cell2->role(0), QAccessible::Cell);
    QCOMPARE(cell2->rowIndex(), 0);
    QCOMPARE(cell2->columnIndex(), 1);
    QCOMPARE(iface->indexOfChild(cell2), 7);
    delete cell2;

    QAccessibleTable2CellInterface *cell3;
    QVERIFY(cell3 = table2->cellAt(1,2));
    QCOMPARE(cell3->text(QAccessible::Name, 0), QString("1.2"));
    QCOMPARE(cell3->role(0), QAccessible::Cell);
    QCOMPARE(cell3->rowIndex(), 1);
    QCOMPARE(cell3->columnIndex(), 2);
    QCOMPARE(iface->indexOfChild(cell3), 12);
    delete cell3;

    QCOMPARE(table2->columnDescription(0), QString("h1"));
    QCOMPARE(table2->columnDescription(1), QString("h2"));
    QCOMPARE(table2->columnDescription(2), QString("h3"));
    QCOMPARE(table2->rowDescription(0), QString("v1"));
    QCOMPARE(table2->rowDescription(1), QString("v2"));
    QCOMPARE(table2->rowDescription(2), QString("v3"));

    delete iface;

    delete tableView;

    QTestAccessibility::clearEvents();
#endif
}

void tst_QAccessibility::calendarWidgetTest()
{
#ifndef QT_NO_CALENDARWIDGET
    {
    QCalendarWidget calendarWidget;

    QAccessibleInterface *interface = QAccessible::queryAccessibleInterface(&calendarWidget);
    QVERIFY(interface);
    QCOMPARE(interface->role(0), QAccessible::Table);
    QVERIFY(!interface->rect(0).isValid());
    QVERIFY(!interface->rect(1).isValid());
    QCOMPARE(interface->childAt(200, 200), -1);

    calendarWidget.resize(400, 300);
    calendarWidget.show();
#if defined(Q_WS_X11)
    qt_x11_wait_for_window_manager(&calendarWidget);
    QTest::qWait(100);
#endif

    // 1 = navigationBar, 2 = view.
    QCOMPARE(interface->childCount(), 2);

    const QRect globalGeometry = QRect(calendarWidget.mapToGlobal(QPoint(0, 0)),
                                       calendarWidget.size());
    QCOMPARE(interface->rect(0), globalGeometry);

    QWidget *navigationBar = 0;
    foreach (QObject *child, calendarWidget.children()) {
        if (child->objectName() == QLatin1String("qt_calendar_navigationbar")) {
            navigationBar = static_cast<QWidget *>(child);
            break;
        }
    }
    QVERIFY(navigationBar);
    QVERIFY(verifyChild(navigationBar, interface, 1, globalGeometry));

    QAbstractItemView *calendarView = 0;
    foreach (QObject *child, calendarWidget.children()) {
        if (child->objectName() == QLatin1String("qt_calendar_calendarview")) {
            calendarView = static_cast<QAbstractItemView *>(child);
            break;
        }
    }
    QVERIFY(calendarView);
    QVERIFY(verifyChild(calendarView, interface, 2, globalGeometry));

    // Hide navigation bar.
    calendarWidget.setNavigationBarVisible(false);
    QCOMPARE(interface->childCount(), 1);
    QVERIFY(!navigationBar->isVisible());

    QVERIFY(verifyChild(calendarView, interface, 1, globalGeometry));

    // Show navigation bar.
    calendarWidget.setNavigationBarVisible(true);
    QCOMPARE(interface->childCount(), 2);
    QVERIFY(navigationBar->isVisible());

    // Navigate to the navigation bar via Child.
    QAccessibleInterface *navigationBarInterface = 0;
    QCOMPARE(interface->navigate(QAccessible::Child, 1, &navigationBarInterface), 0);
    QVERIFY(navigationBarInterface);
    QCOMPARE(navigationBarInterface->object(), (QObject*)navigationBar);
    delete navigationBarInterface;
    navigationBarInterface = 0;

    // Navigate to the view via Child.
    QAccessibleInterface *calendarViewInterface = 0;
    QCOMPARE(interface->navigate(QAccessible::Child, 2, &calendarViewInterface), 0);
    QVERIFY(calendarViewInterface);
    QCOMPARE(calendarViewInterface->object(), (QObject*)calendarView);
    delete calendarViewInterface;
    calendarViewInterface = 0;

    QAccessibleInterface *doesNotExistsInterface = 0;
    QCOMPARE(interface->navigate(QAccessible::Child, 3, &doesNotExistsInterface), -1);
    QVERIFY(!doesNotExistsInterface);

    // Navigate from navigation bar -> view (Down).
    QCOMPARE(interface->navigate(QAccessible::Down, 1, &calendarViewInterface), 0);
    QVERIFY(calendarViewInterface);
    QCOMPARE(calendarViewInterface->object(), (QObject*)calendarView);
    delete calendarViewInterface;
    calendarViewInterface = 0;

    // Navigate from view -> navigation bar (Up).
    QCOMPARE(interface->navigate(QAccessible::Up, 2, &navigationBarInterface), 0);
    QVERIFY(navigationBarInterface);
    QCOMPARE(navigationBarInterface->object(), (QObject*)navigationBar);
    delete navigationBarInterface;
    navigationBarInterface = 0;

    }
    QTestAccessibility::clearEvents();
#endif // QT_NO_CALENDARWIDGET
}

void tst_QAccessibility::dockWidgetTest()
{
#ifndef QT_NO_DOCKWIDGET
    // Set up a proper main window with two dock widgets
    QMainWindow *mw = new QMainWindow();
    QFrame *central = new QFrame(mw);
    mw->setCentralWidget(central);
    QMenuBar *mb = new QMenuBar(mw);
    mb->addAction(tr("&File"));
    mw->setMenuBar(mb);

    QDockWidget *dock1 = new QDockWidget(mw);
    mw->addDockWidget(Qt::LeftDockWidgetArea, dock1);
    QPushButton *pb1 = new QPushButton(tr("Push me"), dock1);
    dock1->setWidget(pb1);

    QDockWidget *dock2 = new QDockWidget(mw);
    mw->addDockWidget(Qt::BottomDockWidgetArea, dock2);
    QPushButton *pb2 = new QPushButton(tr("Push me"), dock2);
    dock2->setWidget(pb2);

    mw->resize(600,400);
    mw->show();
#if defined(Q_WS_X11)
    qt_x11_wait_for_window_manager(mw);
    QTest::qWait(100);
#endif

    QAccessibleInterface *accMainWindow = QAccessible::queryAccessibleInterface(mw);
    // 4 children: menu bar, dock1, dock2, and central widget
    QCOMPARE(accMainWindow->childCount(), 4);
    QAccessibleInterface *accDock1 = 0;
    for (int i = 1; i <= 4; ++i) {
        if (accMainWindow->role(i) == QAccessible::Window) {
            accMainWindow->navigate(QAccessible::Child, i, &accDock1);
            if (accDock1 && qobject_cast<QDockWidget*>(accDock1->object()) == dock1) {
                break;
            } else {
                delete accDock1;
            }
        }
    }
    QVERIFY(accDock1);
    QCOMPARE(accDock1->role(0), QAccessible::Window);
    QCOMPARE(accDock1->role(1), QAccessible::TitleBar);
    QVERIFY(accDock1->rect(0).contains(accDock1->rect(1)));

    QPoint globalPos = dock1->mapToGlobal(QPoint(0,0));
    globalPos.rx()+=5;  //### query style
    globalPos.ry()+=5;
    int entry = accDock1->childAt(globalPos.x(), globalPos.y());    //###
    QAccessibleInterface *accTitleBar;
    entry = accDock1->navigate(QAccessible::Child, entry, &accTitleBar);
    QCOMPARE(accTitleBar->role(0), QAccessible::TitleBar);
    QCOMPARE(accDock1->indexOfChild(accTitleBar), 1);
    QAccessibleInterface *acc;
    entry = accTitleBar->navigate(QAccessible::Ancestor, 1, &acc);
    QVERIFY(acc);
    QCOMPARE(entry, 0);
    QCOMPARE(acc->role(0), QAccessible::Window);


    delete accTitleBar;
    delete accDock1;
    delete pb1;
    delete pb2;
    delete dock1;
    delete dock2;
    delete mw;
    QTestAccessibility::clearEvents();
#endif // QT_NO_DOCKWIDGET
}

void tst_QAccessibility::comboBoxTest()
{
#if defined(Q_OS_WINCE)
    if (!IsValidCEPlatform()) {
        QSKIP("Test skipped on Windows Mobile test hardware", SkipAll);
    }
#endif
    QWidget *w = new QWidget();
    QComboBox *cb = new QComboBox(w);
    cb->addItems(QStringList() << "one" << "two" << "three");
    w->show();
#if defined(Q_WS_X11)
    qt_x11_wait_for_window_manager(w);
    QTest::qWait(100);
#endif
    QAccessibleInterface *acc = QAccessible::queryAccessibleInterface(w);
    delete acc;

    acc = QAccessible::queryAccessibleInterface(cb);

    for (int i = 1; i < acc->childCount(); ++i) {
        QTRY_VERIFY(acc->rect(0).contains(acc->rect(i)));
    }
    QCOMPARE(acc->doAction(QAccessible::Press, 2), true);
    QTest::qWait(400);
    QAccessibleInterface *accList = 0;
    int entry = acc->navigate(QAccessible::Child, 3, &accList);
    QCOMPARE(entry, 0);
    QAccessibleInterface *acc2 = 0;
    entry = accList->navigate(QAccessible::Ancestor, 1, &acc2);
    QCOMPARE(entry, 0);
#if defined(Q_WS_X11) && defined(QT_BUILD_INTERNAL)
    QEXPECT_FAIL("", "QTBUG-26499", Abort);
#endif
    QCOMPARE(verifyHierarchy(acc), 0);
    delete acc2;

    delete accList;
    delete acc;
    delete w;

    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::treeWidgetTest()
{
#if defined(Q_WS_X11)
    QSKIP( "Accessible table1 interface is no longer supported on X11.", SkipAll);
#else
    QWidget *w = new QWidget;
    QTreeWidget *tree = new QTreeWidget(w);
    QHBoxLayout *l = new QHBoxLayout(w);
    l->addWidget(tree);
    for (int i = 0; i < 10; ++i) {
        QStringList strings = QStringList() << QString::fromAscii("row: %1").arg(i)
                                            << QString("column 1") << QString("column 2");

        tree->addTopLevelItem(new QTreeWidgetItem(strings));
    }
    w->show();
//    QTest::qWait(1000);
#if defined(Q_WS_X11)
    qt_x11_wait_for_window_manager(w);
    QTest::qWait(100);
#endif

    QAccessibleInterface *acc = QAccessible::queryAccessibleInterface(tree);
    QAccessibleInterface *accViewport = 0;
    int entry = acc->navigate(QAccessible::Child, 1, &accViewport);
    QVERIFY(accViewport);
    QCOMPARE(entry, 0);
    QAccessibleInterface *accTreeItem = 0;
    entry = accViewport->navigate(QAccessible::Child, 1, &accTreeItem);
    QCOMPARE(entry, 0);

    QAccessibleInterface *accTreeItem2 = 0;
    entry = accTreeItem->navigate(QAccessible::Sibling, 3, &accTreeItem2);
    QCOMPARE(entry, 0);
    QCOMPARE(accTreeItem2->text(QAccessible::Name, 0), QLatin1String("row: 1"));

    // test selected/focused state
    QItemSelectionModel *selModel = tree->selectionModel();
    QVERIFY(selModel);
    selModel->select(QItemSelection(tree->model()->index(0, 0), tree->model()->index(3, 0)), QItemSelectionModel::Select);
    selModel->setCurrentIndex(tree->model()->index(1, 0), QItemSelectionModel::Current);

    for (int i = 1; i < 10 ; ++i) {
        QAccessible::State expected;
        if (i <= 5 && i >= 2)
            expected = QAccessible::Selected;
        if (i == 3)
            expected |= QAccessible::Focused;

        QCOMPARE(accViewport->state(i) & (QAccessible::Focused | QAccessible::Selected), expected);
    }

    // Test sanity of its navigation functions
    QCOMPARE(verifyHierarchy(acc), 0);

    delete accTreeItem2;
    delete accTreeItem;
    delete accViewport;
    delete acc;
    delete w;

    QTestAccessibility::clearEvents();
#endif
}

void tst_QAccessibility::labelTest()
{
    QString text = "Hello World";
    QLabel *label = new QLabel(text);
    setFrameless(label);
    label->show();

#if defined(Q_WS_X11)
    qt_x11_wait_for_window_manager(label);
#endif
    QTest::qWait(100);

    QAccessibleInterface *acc_label = QAccessible::queryAccessibleInterface(label);
    QVERIFY(acc_label);

    QCOMPARE(acc_label->text(QAccessible::Name, 0), text);

    delete acc_label;
    delete label;
    QTestAccessibility::clearEvents();

    QPixmap testPixmap(50, 50);
    testPixmap.fill();

    QLabel imageLabel;
    imageLabel.setPixmap(testPixmap);
    imageLabel.setToolTip("Test Description");

    acc_label = QAccessible::queryAccessibleInterface(&imageLabel);
    QVERIFY(acc_label);

    QAccessibleImageInterface *imageInterface = acc_label->imageInterface();
    QVERIFY(imageInterface);

    QCOMPARE(imageInterface->imageSize(), testPixmap.size());
    QCOMPARE(imageInterface->imageDescription(), QString::fromLatin1("Test Description"));
    QCOMPARE(imageInterface->imagePosition(QAccessible2::RelativeToParent), imageLabel.geometry());

    delete acc_label;

    QTestAccessibility::clearEvents();
}

void tst_QAccessibility::accelerators()
{
    QWidget *window = new QWidget;
    QHBoxLayout *lay = new QHBoxLayout(window);
    QLabel *label = new QLabel(tr("&Line edit"), window);
    QLineEdit *le = new QLineEdit(window);
    lay->addWidget(label);
    lay->addWidget(le);
    label->setBuddy(le);

    window->show();

    QAccessibleInterface *accLineEdit = QAccessible::queryAccessibleInterface(le);
    QCOMPARE(accLineEdit->text(QAccessible::Accelerator, 0), QKeySequence(Qt::ALT).toString(QKeySequence::NativeText) + QLatin1String("L"));
    QCOMPARE(accLineEdit->text(QAccessible::Accelerator, 0), QKeySequence(Qt::ALT).toString(QKeySequence::NativeText) + QLatin1String("L"));
    label->setText(tr("Q &"));
    QCOMPARE(accLineEdit->text(QAccessible::Accelerator, 0), QString());
    label->setText(tr("Q &&"));
    QCOMPARE(accLineEdit->text(QAccessible::Accelerator, 0), QString());
    label->setText(tr("Q && A"));
    QCOMPARE(accLineEdit->text(QAccessible::Accelerator, 0), QString());
    label->setText(tr("Q &&&A"));
    QCOMPARE(accLineEdit->text(QAccessible::Accelerator, 0), QKeySequence(Qt::ALT).toString(QKeySequence::NativeText) + QLatin1String("A"));
    label->setText(tr("Q &&A"));
    QCOMPARE(accLineEdit->text(QAccessible::Accelerator, 0), QString());

#if !defined(QT_NO_DEBUG) && !defined(Q_WS_MAC)
    QTest::ignoreMessage(QtWarningMsg, "QKeySequence::mnemonic: \"Q &A&B\" contains multiple occurrences of '&'");
#endif
    label->setText(tr("Q &A&B"));
    QCOMPARE(accLineEdit->text(QAccessible::Accelerator, 0), QKeySequence(Qt::ALT).toString(QKeySequence::NativeText) + QLatin1String("A"));

#if defined(Q_WS_X11)
    qt_x11_wait_for_window_manager(window);
#endif
    QTest::qWait(100);
    delete window;
    QTestAccessibility::clearEvents();
}


QTEST_MAIN(tst_QAccessibility)

#else // Q_OS_WINCE

QTEST_NOOP_MAIN

#endif  // Q_OS_WINCE

#include "tst_qaccessibility.moc"

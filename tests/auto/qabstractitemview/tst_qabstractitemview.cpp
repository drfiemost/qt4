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

#include <qabstractitemview.h>
#include <qstandarditemmodel.h>
#include <qapplication.h>
#include <qlistview.h>
#include <qlistwidget.h>
#include <qtableview.h>
#include <qtablewidget.h>
#include <qtreeview.h>
#include <qtreewidget.h>
#include <qheaderview.h>
#include <qspinbox.h>
#include <qitemdelegate.h>
#include <qpushbutton.h>
#include <qscrollbar.h>
#include <qboxlayout.h>
#include <qitemdelegate.h>
#include <qlineedit.h>

//TESTED_CLASS=
//TESTED_FILES=

// Will try to wait for the condition while allowing event processing
// for a maximum of 5 seconds.
#define TRY_COMPARE(expr, expected) \
    do { \
        const int step = 50; \
        for (int q = 0; q < 5000 && ((expr) != (expected)); q+=step) { \
            QTest::qWait(step); \
        } \
        QCOMPARE(expr, expected); \
    } while(0)

class TestView : public QAbstractItemView
{
    Q_OBJECT
public:
    inline void tst_dataChanged(const QModelIndex &tl, const QModelIndex &br)
        { dataChanged(tl, br); }
    inline void tst_setHorizontalStepsPerItem(int steps)
        { setHorizontalStepsPerItem(steps); }
    inline int tst_horizontalStepsPerItem() const
        { return horizontalStepsPerItem(); }
    inline void tst_setVerticalStepsPerItem(int steps)
        { setVerticalStepsPerItem(steps); }
    inline int tst_verticalStepsPerItem() const
        { return verticalStepsPerItem(); }

    inline void tst_rowsInserted(const QModelIndex &parent, int start, int end)
        { rowsInserted(parent, start, end); }
    inline void tst_rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
        { rowsAboutToBeRemoved(parent, start, end); }
    inline void tst_selectionChanged(const QItemSelection &selected,
                                     const QItemSelection &deselected)
        { selectionChanged(selected, deselected); }
    inline void tst_currentChanged(const QModelIndex &current, const QModelIndex &previous)
        { currentChanged(current, previous); }
    inline void tst_updateEditorData()
        { updateEditorData(); }
    inline void tst_updateEditorGeometries()
        { updateEditorGeometries(); }
    inline void tst_updateGeometries()
        { updateGeometries(); }
    inline void tst_verticalScrollbarAction(int action)
        { verticalScrollbarAction(action); }
    inline void tst_horizontalScrollbarAction(int action)
        { horizontalScrollbarAction(action); }
    inline void tst_verticalScrollbarValueChanged(int value)
        { verticalScrollbarValueChanged(value); }
    inline void tst_horizontalScrollbarValueChanged(int value)
        { horizontalScrollbarValueChanged(value); }
    inline void tst_closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)
        { closeEditor(editor, hint); }
    inline void tst_commitData(QWidget *editor)
        { commitData(editor); }
    inline void tst_editorDestroyed(QObject *editor)
        { editorDestroyed(editor); }
    enum tst_CursorAction {
        MoveUp = QAbstractItemView::MoveUp,
        MoveDown = QAbstractItemView::MoveDown,
        MoveLeft = QAbstractItemView::MoveLeft,
        MoveRight = QAbstractItemView::MoveRight,
        MoveHome = QAbstractItemView::MoveHome,
        MoveEnd = QAbstractItemView::MoveEnd,
        MovePageUp = QAbstractItemView::MovePageUp,
        MovePageDown = QAbstractItemView::MovePageDown,
        MoveNext = QAbstractItemView::MoveNext,
        MovePrevious = QAbstractItemView::MovePrevious
    };
    inline QModelIndex tst_moveCursor(tst_CursorAction cursorAction,
                                      Qt::KeyboardModifiers modifiers)
        { return moveCursor(QAbstractItemView::CursorAction(cursorAction), modifiers); }
    inline int tst_horizontalOffset() const
        { return horizontalOffset(); }
    inline int tst_verticalOffset() const
        { return verticalOffset(); }
    inline bool tst_isIndexHidden(const QModelIndex &index) const
        { return isIndexHidden(index); }
    inline void tst_setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
        { setSelection(rect, command); }
    inline QRegion tst_visualRegionForSelection(const QItemSelection &selection) const
        { return visualRegionForSelection(selection); }
    inline QModelIndexList tst_selectedIndexes() const
        { return selectedIndexes(); }
    inline bool tst_edit(const QModelIndex &index, EditTrigger trigger, QEvent *event)
        { return edit(index, trigger, event); }
    inline QItemSelectionModel::SelectionFlags tst_selectionCommand(const QModelIndex &index,
                                                                    const QEvent *event = 0) const
        { return selectionCommand(index, event); }
#ifndef QT_NO_DRAGANDDROP
    inline void tst_startDrag(Qt::DropActions supportedActions)
        { startDrag(supportedActions); }
#endif
    inline QStyleOptionViewItem tst_viewOptions() const
        { return viewOptions(); }
    enum tst_State {
        NoState = QAbstractItemView::NoState,
        DraggingState = QAbstractItemView::DraggingState,
        DragSelectingState = QAbstractItemView::DragSelectingState,
        EditingState = QAbstractItemView::EditingState,
        ExpandingState = QAbstractItemView::ExpandingState,
        CollapsingState = QAbstractItemView::CollapsingState
    };
    inline tst_State tst_state() const
        { return (tst_State)state(); }
    inline void tst_setState(tst_State state)
        { setState(QAbstractItemView::State(state)); }
    inline void tst_startAutoScroll()
        { startAutoScroll(); }
    inline void tst_stopAutoScroll()
        { stopAutoScroll(); }
    inline void tst_doAutoScroll()
        { doAutoScroll(); }
};

class tst_QAbstractItemView : public QObject
{
    Q_OBJECT

public:

    tst_QAbstractItemView();
    virtual ~tst_QAbstractItemView();
    void basic_tests(TestView *view);

public slots:
    void initTestCase();
    void cleanupTestCase();

private slots:
    void getSetCheck();
    void emptyModels_data();
    void emptyModels();
    void setModel_data();
    void setModel();
    void noModel();
    void dragSelect();
    void rowDelegate();
    void columnDelegate();
    void selectAll();
    void ctrlA();
    void persistentEditorFocus();
    void setItemDelegate();
    void setItemDelegate_data();
	// The dragAndDrop() test doesn't work, and is thus disabled on Mac and Windows
	// for the following reasons:
	//   Mac: use of GetCurrentEventButtonState() in QDragManager::drag()
	//   Win: unknown reason
#if !defined(Q_OS_MAC) && !defined(Q_OS_WIN)
#if 0
    void dragAndDrop();
    void dragAndDropOnChild();
#endif
#endif
    void noFallbackToRoot();
    void setCurrentIndex_data();
    void setCurrentIndex();

    void task221955_selectedEditor();
    void task250754_fontChange();
    void task200665_itemEntered();
    void task257481_emptyEditor();
    void shiftArrowSelectionAfterScrolling();
    void shiftSelectionAfterRubberbandSelection();
    void ctrlRubberbandSelection();
    void QTBUG6407_extendedSelection();
    void QTBUG6753_selectOnSelection();
    void QTBUG46785_mouseout_hover_state();
};

class MyAbstractItemDelegate : public QAbstractItemDelegate
{
public:
    MyAbstractItemDelegate() : QAbstractItemDelegate() {};
    void paint(QPainter *, const QStyleOptionViewItem &, const QModelIndex &) const {}
    QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const { return QSize(); }
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &,
                          const QModelIndex &) const { return new QWidget(parent); }
};

// Testing get/set functions
void tst_QAbstractItemView::getSetCheck()
{
    QListView view;
    TestView *obj1 = reinterpret_cast<TestView*>(&view);
    // QAbstractItemDelegate * QAbstractItemView::itemDelegate()
    // void QAbstractItemView::setItemDelegate(QAbstractItemDelegate *)
    MyAbstractItemDelegate *var1 = new MyAbstractItemDelegate;
    obj1->setItemDelegate(var1);
    QCOMPARE((QAbstractItemDelegate*)var1, obj1->itemDelegate());
    obj1->setItemDelegate((QAbstractItemDelegate *)0);
    QCOMPARE((QAbstractItemDelegate *)0, obj1->itemDelegate());
    delete var1;

    // EditTriggers QAbstractItemView::editTriggers()
    // void QAbstractItemView::setEditTriggers(EditTriggers)
    obj1->setEditTriggers(QAbstractItemView::EditTriggers(QAbstractItemView::NoEditTriggers));
    QCOMPARE(QAbstractItemView::EditTriggers(QAbstractItemView::NoEditTriggers), obj1->editTriggers());
    obj1->setEditTriggers(QAbstractItemView::EditTriggers(QAbstractItemView::CurrentChanged));
    QCOMPARE(QAbstractItemView::EditTriggers(QAbstractItemView::CurrentChanged), obj1->editTriggers());
    obj1->setEditTriggers(QAbstractItemView::EditTriggers(QAbstractItemView::DoubleClicked));
    QCOMPARE(QAbstractItemView::EditTriggers(QAbstractItemView::DoubleClicked), obj1->editTriggers());
    obj1->setEditTriggers(QAbstractItemView::EditTriggers(QAbstractItemView::SelectedClicked));
    QCOMPARE(QAbstractItemView::EditTriggers(QAbstractItemView::SelectedClicked), obj1->editTriggers());
    obj1->setEditTriggers(QAbstractItemView::EditTriggers(QAbstractItemView::EditKeyPressed));
    QCOMPARE(QAbstractItemView::EditTriggers(QAbstractItemView::EditKeyPressed), obj1->editTriggers());
    obj1->setEditTriggers(QAbstractItemView::EditTriggers(QAbstractItemView::AnyKeyPressed));
    QCOMPARE(QAbstractItemView::EditTriggers(QAbstractItemView::AnyKeyPressed), obj1->editTriggers());
    obj1->setEditTriggers(QAbstractItemView::EditTriggers(QAbstractItemView::AllEditTriggers));
    QCOMPARE(QAbstractItemView::EditTriggers(QAbstractItemView::AllEditTriggers), obj1->editTriggers());

    // bool QAbstractItemView::tabKeyNavigation()
    // void QAbstractItemView::setTabKeyNavigation(bool)
    obj1->setTabKeyNavigation(false);
    QCOMPARE(false, obj1->tabKeyNavigation());
    obj1->setTabKeyNavigation(true);
    QCOMPARE(true, obj1->tabKeyNavigation());

    // bool QAbstractItemView::dragEnabled()
    // void QAbstractItemView::setDragEnabled(bool)
#ifndef QT_NO_DRAGANDDROP
    obj1->setDragEnabled(false);
    QCOMPARE(false, obj1->dragEnabled());
    obj1->setDragEnabled(true);
    QCOMPARE(true, obj1->dragEnabled());
#endif
    // bool QAbstractItemView::alternatingRowColors()
    // void QAbstractItemView::setAlternatingRowColors(bool)
    obj1->setAlternatingRowColors(false);
    QCOMPARE(false, obj1->alternatingRowColors());
    obj1->setAlternatingRowColors(true);
    QCOMPARE(true, obj1->alternatingRowColors());

    // State QAbstractItemView::state()
    // void QAbstractItemView::setState(State)
    obj1->tst_setState(TestView::tst_State(TestView::NoState));
    QCOMPARE(TestView::tst_State(TestView::NoState), obj1->tst_state());
    obj1->tst_setState(TestView::tst_State(TestView::DraggingState));
    QCOMPARE(TestView::tst_State(TestView::DraggingState), obj1->tst_state());
    obj1->tst_setState(TestView::tst_State(TestView::DragSelectingState));
    QCOMPARE(TestView::tst_State(TestView::DragSelectingState), obj1->tst_state());
    obj1->tst_setState(TestView::tst_State(TestView::EditingState));
    QCOMPARE(TestView::tst_State(TestView::EditingState), obj1->tst_state());
    obj1->tst_setState(TestView::tst_State(TestView::ExpandingState));
    QCOMPARE(TestView::tst_State(TestView::ExpandingState), obj1->tst_state());
    obj1->tst_setState(TestView::tst_State(TestView::CollapsingState));
    QCOMPARE(TestView::tst_State(TestView::CollapsingState), obj1->tst_state());

    // QWidget QAbstractScrollArea::viewport()
    // void setViewport(QWidget*)
    QWidget *vp = new QWidget;
    obj1->setViewport(vp);
    QCOMPARE(vp, obj1->viewport());

    QCOMPARE(16, obj1->autoScrollMargin());
    obj1->setAutoScrollMargin(20);
    QCOMPARE(20, obj1->autoScrollMargin());
    obj1->setAutoScrollMargin(16);
    QCOMPARE(16, obj1->autoScrollMargin());
}

tst_QAbstractItemView::tst_QAbstractItemView()
{
}

tst_QAbstractItemView::~tst_QAbstractItemView()
{
}

void tst_QAbstractItemView::initTestCase()
{
#ifdef Q_OS_WINCE_WM
    qApp->setAutoMaximizeThreshold(-1);
#endif
}

void tst_QAbstractItemView::cleanupTestCase()
{
}

void tst_QAbstractItemView::emptyModels_data()
{
    QTest::addColumn<QString>("viewType");

    QTest::newRow("QListView") << "QListView";
    QTest::newRow("QTableView") << "QTableView";
    QTest::newRow("QTreeView") << "QTreeView";
    QTest::newRow("QHeaderView") << "QHeaderView";
}

void tst_QAbstractItemView::emptyModels()
{
    QFETCH(QString, viewType);

    TestView *view = 0;
    if (viewType == "QListView")
        view = reinterpret_cast<TestView*>(new QListView());
    else if (viewType == "QTableView")
        view = reinterpret_cast<TestView*>(new QTableView());
    else if (viewType == "QTreeView")
        view = reinterpret_cast<TestView*>(new QTreeView());
    else if (viewType == "QHeaderView")
        view = reinterpret_cast<TestView*>(new QHeaderView(Qt::Vertical));
    else
        QVERIFY(0);
    view->show();

    QVERIFY(!view->model());
    QVERIFY(!view->selectionModel());
    //QVERIFY(view->itemDelegate() != 0);

    basic_tests(view);
    delete view;
}

void tst_QAbstractItemView::setModel_data()
{
    QTest::addColumn<QString>("viewType");

    QTest::newRow("QListView") << "QListView";
    QTest::newRow("QTableView") << "QTableView";
    QTest::newRow("QTreeView") << "QTreeView";
    QTest::newRow("QHeaderView") << "QHeaderView";
}

void tst_QAbstractItemView::setModel()
{
    QFETCH(QString, viewType);
    TestView *view = 0;
    if (viewType == "QListView")
        view = reinterpret_cast<TestView*>(new QListView());
    else if (viewType == "QTableView")
        view = reinterpret_cast<TestView*>(new QTableView());
    else if (viewType == "QTreeView")
        view = reinterpret_cast<TestView*>(new QTreeView());
    else if (viewType == "QHeaderView")
        view = reinterpret_cast<TestView*>(new QHeaderView(Qt::Vertical));
    else
        QVERIFY(0);
    view->show();

    QStandardItemModel model(20,20);
    view->setModel(0);
    view->setModel(&model);
    basic_tests(view);
    delete view;
}

void tst_QAbstractItemView::basic_tests(TestView *view)
{
    // setSelectionModel
    // Will assert as it should
    //view->setSelectionModel(0);
    // setItemDelegate
    //view->setItemDelegate(0);
    // Will asswert as it should

    // setSelectionMode
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    QCOMPARE(view->selectionMode(), QAbstractItemView::SingleSelection);
    view->setSelectionMode(QAbstractItemView::ContiguousSelection);
    QCOMPARE(view->selectionMode(), QAbstractItemView::ContiguousSelection);
    view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    QCOMPARE(view->selectionMode(), QAbstractItemView::ExtendedSelection);
    view->setSelectionMode(QAbstractItemView::MultiSelection);
    QCOMPARE(view->selectionMode(), QAbstractItemView::MultiSelection);
    view->setSelectionMode(QAbstractItemView::NoSelection);
    QCOMPARE(view->selectionMode(), QAbstractItemView::NoSelection);

    // setSelectionBehavior
    view->setSelectionBehavior(QAbstractItemView::SelectItems);
    QCOMPARE(view->selectionBehavior(), QAbstractItemView::SelectItems);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    QCOMPARE(view->selectionBehavior(), QAbstractItemView::SelectRows);
    view->setSelectionBehavior(QAbstractItemView::SelectColumns);
    QCOMPARE(view->selectionBehavior(), QAbstractItemView::SelectColumns);

    // setEditTriggers
    view->setEditTriggers(QAbstractItemView::EditKeyPressed);
    QCOMPARE(view->editTriggers(), QAbstractItemView::EditKeyPressed);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QCOMPARE(view->editTriggers(), QAbstractItemView::NoEditTriggers);
    view->setEditTriggers(QAbstractItemView::CurrentChanged);
    QCOMPARE(view->editTriggers(), QAbstractItemView::CurrentChanged);
    view->setEditTriggers(QAbstractItemView::DoubleClicked);
    QCOMPARE(view->editTriggers(), QAbstractItemView::DoubleClicked);
    view->setEditTriggers(QAbstractItemView::SelectedClicked);
    QCOMPARE(view->editTriggers(), QAbstractItemView::SelectedClicked);
    view->setEditTriggers(QAbstractItemView::AnyKeyPressed);
    QCOMPARE(view->editTriggers(), QAbstractItemView::AnyKeyPressed);
    view->setEditTriggers(QAbstractItemView::AllEditTriggers);
    QCOMPARE(view->editTriggers(), QAbstractItemView::AllEditTriggers);

    // setAutoScroll
    view->setAutoScroll(false);
    QCOMPARE(view->hasAutoScroll(), false);
    view->setAutoScroll(true);
    QCOMPARE(view->hasAutoScroll(), true);

    // setTabKeyNavigation
    view->setTabKeyNavigation(false);
    QCOMPARE(view->tabKeyNavigation(), false);
    view->setTabKeyNavigation(true);
    QCOMPARE(view->tabKeyNavigation(), true);

#ifndef QT_NO_DRAGANDDROP
    // setDropIndicatorShown
    view->setDropIndicatorShown(false);
    QCOMPARE(view->showDropIndicator(), false);
    view->setDropIndicatorShown(true);
    QCOMPARE(view->showDropIndicator(), true);

    // setDragEnabled
    view->setDragEnabled(false);
    QCOMPARE(view->dragEnabled(), false);
    view->setDragEnabled(true);
    QCOMPARE(view->dragEnabled(), true);
#endif

    // setAlternatingRowColors
    view->setAlternatingRowColors(false);
    QCOMPARE(view->alternatingRowColors(), false);
    view->setAlternatingRowColors(true);
    QCOMPARE(view->alternatingRowColors(), true);

    // setIconSize
    view->setIconSize(QSize(16, 16));
    QCOMPARE(view->iconSize(), QSize(16, 16));
    view->setIconSize(QSize(32, 32));
    QCOMPARE(view->iconSize(), QSize(32, 32));
    // Should this happen?
    view->setIconSize(QSize(-1, -1));
    QCOMPARE(view->iconSize(), QSize(-1, -1));

    QCOMPARE(view->currentIndex(), QModelIndex());
    QCOMPARE(view->rootIndex(), QModelIndex());

    view->keyboardSearch("");
    view->keyboardSearch("foo");
    view->keyboardSearch("1");

    QCOMPARE(view->visualRect(QModelIndex()), QRect());

    view->scrollTo(QModelIndex());

    QCOMPARE(view->sizeHintForIndex(QModelIndex()), QSize());
    QCOMPARE(view->indexAt(QPoint(-1, -1)), QModelIndex());

    if (!view->model()){
        QCOMPARE(view->indexAt(QPoint(10, 10)), QModelIndex());
        QCOMPARE(view->sizeHintForRow(0), -1);
        QCOMPARE(view->sizeHintForColumn(0), -1);
    }else if (view->itemDelegate()){
        view->sizeHintForRow(0);
        view->sizeHintForColumn(0);
    }
    view->openPersistentEditor(QModelIndex());
    view->closePersistentEditor(QModelIndex());

    view->reset();
    view->setRootIndex(QModelIndex());
    view->doItemsLayout();
    view->selectAll();
    view->edit(QModelIndex());
    view->clearSelection();
    view->setCurrentIndex(QModelIndex());

    // protected methods
    view->tst_dataChanged(QModelIndex(), QModelIndex());
    view->tst_rowsInserted(QModelIndex(), -1, -1);
    view->tst_rowsAboutToBeRemoved(QModelIndex(), -1, -1);
    view->tst_selectionChanged(QItemSelection(), QItemSelection());
    if (view->model()){
        view->tst_currentChanged(QModelIndex(), QModelIndex());
        view->tst_currentChanged(QModelIndex(), view->model()->index(0,0));
    }
    view->tst_updateEditorData();
    view->tst_updateEditorGeometries();
    view->tst_updateGeometries();
    view->tst_verticalScrollbarAction(QAbstractSlider::SliderSingleStepAdd);
    view->tst_horizontalScrollbarAction(QAbstractSlider::SliderSingleStepAdd);
    view->tst_verticalScrollbarValueChanged(10);
    view->tst_horizontalScrollbarValueChanged(10);
    view->tst_closeEditor(0, QAbstractItemDelegate::NoHint);
    view->tst_commitData(0);
    view->tst_editorDestroyed(0);

    view->tst_setHorizontalStepsPerItem(2);
    view->tst_horizontalStepsPerItem();
    view->tst_setVerticalStepsPerItem(2);
    view->tst_verticalStepsPerItem();

    // Will assert as it should
    // view->setIndexWidget(QModelIndex(), 0);

    view->tst_moveCursor(TestView::MoveUp, Qt::NoModifier);
    view->tst_horizontalOffset();
    view->tst_verticalOffset();

//    view->tst_isIndexHidden(QModelIndex()); // will (correctly) assert
    if(view->model())
        view->tst_isIndexHidden(view->model()->index(0,0));

    view->tst_setSelection(QRect(0, 0, 10, 10), QItemSelectionModel::ClearAndSelect);
    view->tst_setSelection(QRect(-1, -1, -1, -1), QItemSelectionModel::ClearAndSelect);
    view->tst_visualRegionForSelection(QItemSelection());
    view->tst_selectedIndexes();

    view->tst_edit(QModelIndex(), QAbstractItemView::NoEditTriggers, 0);

    view->tst_selectionCommand(QModelIndex(), 0);

#ifndef QT_NO_DRAGANDDROP
    if (!view->model())
        view->tst_startDrag(Qt::CopyAction);

    view->tst_viewOptions();

    view->tst_setState(TestView::NoState);
    QVERIFY(view->tst_state()==TestView::NoState);
    view->tst_setState(TestView::DraggingState);
    QVERIFY(view->tst_state()==TestView::DraggingState);
    view->tst_setState(TestView::DragSelectingState);
    QVERIFY(view->tst_state()==TestView::DragSelectingState);
    view->tst_setState(TestView::EditingState);
    QVERIFY(view->tst_state()==TestView::EditingState);
    view->tst_setState(TestView::ExpandingState);
    QVERIFY(view->tst_state()==TestView::ExpandingState);
    view->tst_setState(TestView::CollapsingState);
    QVERIFY(view->tst_state()==TestView::CollapsingState);
#endif

    view->tst_startAutoScroll();
    view->tst_stopAutoScroll();
    view->tst_doAutoScroll();

    // testing mouseFoo and key functions
//     QTest::mousePress(view, Qt::LeftButton, Qt::NoModifier, QPoint(0,0));
//     mouseMove(view, Qt::LeftButton, Qt::NoModifier, QPoint(10,10));
//     QTest::mouseRelease(view, Qt::LeftButton, Qt::NoModifier, QPoint(10,10));
//     QTest::mouseClick(view, Qt::LeftButton, Qt::NoModifier, QPoint(10,10));
//     mouseDClick(view, Qt::LeftButton, Qt::NoModifier, QPoint(10,10));
//     QTest::keyClick(view, Qt::Key_A);
}

void tst_QAbstractItemView::noModel()
{
    // From task #85415

    QStandardItemModel model(20,20);
    QTreeView view;

    view.setModel(&model);
    // Make the viewport smaller than the contents, so that we can scroll
    view.resize(100,100);
    view.show();

    // make sure that the scrollbars are not at value 0
    view.scrollTo(view.model()->index(10,10));
    QApplication::processEvents();

    view.setModel(0);
    // Due to the model is removed, this will generate a valueChanged signal on both scrollbars. (value to 0)
    QApplication::processEvents();
    QCOMPARE(view.model(), (QAbstractItemModel*)0);
}

void tst_QAbstractItemView::dragSelect()
{
    // From task #86108

    QStandardItemModel model(64,64);

    QTableView view;
    view.setModel(&model);
    view.setVisible(true);

    const int delay = 2;
    for (int i = 0; i < 2; ++i) {
        bool tracking = (i == 1);
        view.setMouseTracking(false);
        QTest::mouseMove(&view, QPoint(0, 0), delay);
        view.setMouseTracking(tracking);
        QTest::mouseMove(&view, QPoint(50, 50), delay);
        QVERIFY(view.selectionModel()->selectedIndexes().isEmpty());
    }
}

void tst_QAbstractItemView::rowDelegate()
{
    QStandardItemModel model(4,4);
    MyAbstractItemDelegate delegate;

    QTableView view;
    view.setModel(&model);
    view.setItemDelegateForRow(3, &delegate);
    view.show();

    QModelIndex index = model.index(3, 0);
    view.openPersistentEditor(index);
    QWidget *w = view.indexWidget(index);
    QVERIFY(w);
    QCOMPARE(w->metaObject()->className(), "QWidget");
}

void tst_QAbstractItemView::columnDelegate()
{
    QStandardItemModel model(4,4);
    MyAbstractItemDelegate delegate;

    QTableView view;
    view.setModel(&model);
    view.setItemDelegateForColumn(3, &delegate);
    view.show();

    QModelIndex index = model.index(0, 3);
    view.openPersistentEditor(index);
    QWidget *w = view.indexWidget(index);
    QVERIFY(w);
    QCOMPARE(w->metaObject()->className(), "QWidget");
}

void tst_QAbstractItemView::selectAll()
{
    QStandardItemModel model(4,4);
    QTableView view;
    view.setModel(&model);

    TestView *tst_view = (TestView*)&view;

    QCOMPARE(tst_view->tst_selectedIndexes().count(), 0);
    view.selectAll();
    QCOMPARE(tst_view->tst_selectedIndexes().count(), 4*4);
}

void tst_QAbstractItemView::ctrlA()
{
    QStandardItemModel model(4,4);
    QTableView view;
    view.setModel(&model);

    TestView *tst_view = (TestView*)&view;

    QCOMPARE(tst_view->tst_selectedIndexes().count(), 0);
    QTest::keyClick(&view, Qt::Key_A, Qt::ControlModifier);
    QCOMPARE(tst_view->tst_selectedIndexes().count(), 4*4);
}

void tst_QAbstractItemView::persistentEditorFocus()
{
    // one row, three columns
    QStandardItemModel model(1, 3);
    for(int i = 0; i < model.columnCount(); ++i)
        model.setData(model.index(0, i), i);
    QTableView view;
    view.setModel(&model);

    view.openPersistentEditor(model.index(0, 1));
    view.openPersistentEditor(model.index(0, 2));

    //these are spinboxes because we put numbers inside
    QList<QSpinBox*> list = qFindChildren<QSpinBox*>(view.viewport());
    QCOMPARE(list.count(), 2); //these should be the 2 editors

    view.setCurrentIndex(model.index(0, 0));
    QCOMPARE(view.currentIndex(), model.index(0, 0));
    view.show();
    QTRY_VERIFY(view.isVisible());

    for (int i = 0; i < list.count(); ++i) {
        TRY_COMPARE(list.at(i)->isVisible(), true);
        QPoint p = QPoint(5, 5);
        QMouseEvent mouseEvent(QEvent::MouseButtonPress, p, Qt::LeftButton,
                               Qt::LeftButton, Qt::NoModifier);
        qApp->sendEvent(list.at(i), &mouseEvent);
        if (!qApp->focusWidget())
            QSKIP("Some window managers don't handle focus that well", SkipAll);
        QTRY_COMPARE(qApp->focusWidget(), static_cast<QWidget *>(list.at(i)));
    }
}


#if !defined(Q_OS_MAC) && !defined(Q_OS_WIN)

#if 0

static void sendMouseMove(QWidget *widget, QPoint pos = QPoint())
{
    if (pos.isNull())
        pos = widget->rect().center();
    QMouseEvent event(QEvent::MouseMove, pos, widget->mapToGlobal(pos), Qt::NoButton, 0, 0);
    QCursor::setPos(widget->mapToGlobal(pos));
    qApp->processEvents();
#if defined(Q_WS_X11)
    qt_x11_wait_for_window_manager(widget);
#endif
    QApplication::sendEvent(widget, &event);
}

static void sendMousePress(
    QWidget *widget, QPoint pos = QPoint(), Qt::MouseButton button = Qt::LeftButton)
{
    if (pos.isNull())
         pos = widget->rect().center();
    QMouseEvent event(QEvent::MouseButtonPress, pos, widget->mapToGlobal(pos), button, 0, 0);
    QApplication::sendEvent(widget, &event);
}

static void sendMouseRelease(
    QWidget *widget, QPoint pos = QPoint(), Qt::MouseButton button = Qt::LeftButton)
{
    if (pos.isNull())
         pos = widget->rect().center();
    QMouseEvent event(QEvent::MouseButtonRelease, pos, widget->mapToGlobal(pos), button, 0, 0);
    QApplication::sendEvent(widget, &event);
}

class DnDTestModel : public QStandardItemModel
{
    Q_OBJECT
    bool dropMimeData(const QMimeData *md, Qt::DropAction action, int r, int c, const QModelIndex &p)
    {
        dropAction_result = action;
        QStandardItemModel::dropMimeData(md, action, r, c, p);
        return true;
    }
    Qt::DropActions supportedDropActions() const { return Qt::CopyAction | Qt::MoveAction; }

    Qt::DropAction dropAction_result;
public:
    DnDTestModel() : QStandardItemModel(20, 20), dropAction_result(Qt::IgnoreAction) {
        for (int i = 0; i < rowCount(); ++i)
            setData(index(i, 0), QString("%1").arg(i));
    }
    Qt::DropAction dropAction() const { return dropAction_result; }
};

class DnDTestView : public QTreeView
{
    Q_OBJECT

    QPoint dropPoint;
    Qt::DropAction dropAction;

    void dragEnterEvent(QDragEnterEvent *event)
    {
        QAbstractItemView::dragEnterEvent(event);
    }

    void dropEvent(QDropEvent *event)
    {
        event->setDropAction(dropAction);
        QTreeView::dropEvent(event);
    }

    void timerEvent(QTimerEvent *event)
    {
        killTimer(event->timerId());
        sendMouseMove(this, dropPoint);
        sendMouseRelease(this);
    }

    void mousePressEvent(QMouseEvent *e)
    {
        QTreeView::mousePressEvent(e);

        startTimer(0);
        setState(DraggingState);
        startDrag(dropAction);
    }

public:
    DnDTestView(Qt::DropAction dropAction, QAbstractItemModel *model)
        : dropAction(dropAction)
    {
        header()->hide();
        setModel(model);
        setDragDropMode(QAbstractItemView::DragDrop);
        setAcceptDrops(true);
        setDragEnabled(true);
    }

    void dragAndDrop(QPoint drag, QPoint drop)
    {
        dropPoint = drop;
        setCurrentIndex(indexAt(drag));
        sendMousePress(viewport(), drag);
    }
};

class DnDTestWidget : public QWidget
{
    Q_OBJECT

    Qt::DropAction dropAction_request;
    Qt::DropAction dropAction_result;
    QWidget *dropTarget;

    void timerEvent(QTimerEvent *event)
    {
        killTimer(event->timerId());
        sendMouseMove(dropTarget);
        sendMouseRelease(dropTarget);
    }

    void mousePressEvent(QMouseEvent *)
    {
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;
        mimeData->setData("application/x-qabstractitemmodeldatalist", QByteArray(""));
        drag->setMimeData(mimeData);
        startTimer(0);
        dropAction_result = drag->start(dropAction_request);
    }

public:
    Qt::DropAction dropAction() const { return dropAction_result; }

    void dragAndDrop(QWidget *dropTarget, Qt::DropAction dropAction)
    {
        this->dropTarget = dropTarget;
        dropAction_request = dropAction;
        sendMousePress(this);
    }
};

void tst_QAbstractItemView::dragAndDrop()
{
    // From Task 137729

#ifdef Q_WS_QWS
    QSKIP("Embedded drag-and-drop not good enough yet...", SkipAll);
#endif

    const int attempts = 10;
    int successes = 0;
    for (int i = 0; i < attempts; ++i) {
        Qt::DropAction dropAction = Qt::MoveAction;

        DnDTestModel model;
        DnDTestView view(dropAction, &model);
        DnDTestWidget widget;

        const int size = 200;
        widget.setFixedSize(size, size);
        view.setFixedSize(size, size);

        widget.move(0, 0);
        view.move(int(size * 1.5), int(size * 1.5));

        widget.show();
        view.show();
#if defined(Q_WS_X11)
        qt_x11_wait_for_window_manager(&widget);
        qt_x11_wait_for_window_manager(&view);
#endif

        widget.dragAndDrop(&view, dropAction);
        if (model.dropAction() == dropAction
            && widget.dropAction() == dropAction)
            ++successes;
    }

    if (successes < attempts) {
        QString msg = QString("# successes (%1) < # attempts (%2)").arg(successes).arg(attempts);
        QWARN(msg.toLatin1());
    }
    QVERIFY(successes > 0); // allow for some "event unstability" (i.e. unless
                            // successes == 0, QAbstractItemView is probably ok!)
}

void tst_QAbstractItemView::dragAndDropOnChild()
{
#ifdef Q_WS_QWS
    QSKIP("Embedded drag-and-drop not good enough yet...", SkipAll);
#endif

    const int attempts = 10;
    int successes = 0;
    for (int i = 0; i < attempts; ++i) {
        Qt::DropAction dropAction = Qt::MoveAction;

        DnDTestModel model;
        QModelIndex parent = model.index(0, 0);
        model.insertRow(0, parent);
        model.insertColumn(0, parent);
        QModelIndex child = model.index(0, 0, parent);
        model.setData(child, "child");
        QCOMPARE(model.rowCount(parent), 1);
        DnDTestView view(dropAction, &model);
        view.setExpanded(parent, true);
        view.setDragDropMode(QAbstractItemView::InternalMove);

        const int size = 200;
        view.setFixedSize(size, size);
        view.move(int(size * 1.5), int(size * 1.5));
        view.show();
#if defined(Q_WS_X11)
        qt_x11_wait_for_window_manager(&view);
#endif

        view.dragAndDrop(view.visualRect(parent).center(),
                         view.visualRect(child).center());
        if (model.dropAction() == dropAction)
            ++successes;
    }

    QVERIFY(successes == 0);
}

#endif // 0
#endif // !Q_OS_MAC && !Q_OS_WIN

class TestModel : public QStandardItemModel
{
public:
    TestModel(int rows, int columns) : QStandardItemModel(rows, columns)
    {
        setData_count = 0;
    }

    virtual bool setData(const QModelIndex &/*index*/, const QVariant &/*value*/, int /*role = Qt::EditRole*/)
    {
        ++setData_count;
        return true;
    }

    int setData_count;
};

typedef QList<int> IntList;
Q_DECLARE_METATYPE(IntList)

void tst_QAbstractItemView::setItemDelegate_data()
{
    // default is rows, a -1 will switch to columns
    QTest::addColumn<IntList>("rowsOrColumnsWithDelegate");
    QTest::addColumn<QPoint>("cellToEdit");
    QTest::newRow("4 columndelegates")
                << (IntList() << -1 << 0 << 1 << 2 << 3)
                << QPoint(0, 0);
    QTest::newRow("2 identical rowdelegates on the same row")
                << (IntList() << 0 << 0)
                << QPoint(0, 0);
    QTest::newRow("2 identical columndelegates on the same column")
                << (IntList() << -1 << 2 << 2)
                << QPoint(2, 0);
    QTest::newRow("2 duplicate delegates, 1 row and 1 column")
                << (IntList() << 0 << -1 << 2)
                << QPoint(2, 0);
    QTest::newRow("4 duplicate delegates, 2 row and 2 column")
                << (IntList() << 0 << 0 << -1 << 2 << 2)
                << QPoint(2, 0);

}

void tst_QAbstractItemView::setItemDelegate()
{
    QFETCH(IntList, rowsOrColumnsWithDelegate);
    QFETCH(QPoint, cellToEdit);
    QTableView v;
    QItemDelegate *delegate = new QItemDelegate(&v);
    TestModel model(5, 5);
    v.setModel(&model);

    bool row = true;
    foreach (int rc, rowsOrColumnsWithDelegate) {
        if (rc == -1) {
            row = !row;
        } else {
            if (row) {
                v.setItemDelegateForRow(rc, delegate);
            } else {
                v.setItemDelegateForColumn(rc, delegate);
            }
        }
    }
    v.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&v);
    QCursor::setPos(v.geometry().center());
    QApplication::syncX();
#endif
    QTest::qWait(20);
    QApplication::setActiveWindow(&v);

    QModelIndex index = model.index(cellToEdit.y(), cellToEdit.x());
    v.edit(index);

    // This will close the editor
    TRY_COMPARE(QApplication::focusWidget() == 0, false);
    QWidget *editor = QApplication::focusWidget();
    QVERIFY(editor);
    editor->hide();
    delete editor;
    QCOMPARE(model.setData_count, 1);
    delete delegate;
}

void tst_QAbstractItemView::noFallbackToRoot()
{
    QStandardItemModel model(0, 1);
    for (int i = 0; i < 5; ++i)
        model.appendRow(new QStandardItem("top" + QString::number(i)));
    QStandardItem *par1 = model.item(1);
    for (int j = 0; j < 15; ++j)
        par1->appendRow(new QStandardItem("sub" + QString::number(j)));
    QStandardItem *par2 = par1->child(2);
    for (int k = 0; k < 10; ++k)
        par2->appendRow(new QStandardItem("bot" + QString::number(k)));
    QStandardItem *it1 = par2->child(5);

    QModelIndex parent1 = model.indexFromItem(par1);
    QModelIndex parent2 = model.indexFromItem(par2);
    QModelIndex item1 = model.indexFromItem(it1);

    QTreeView v;
    v.setModel(&model);
    v.setRootIndex(parent1);
    v.setCurrentIndex(item1);
    QCOMPARE(v.currentIndex(), item1);
    QVERIFY(model.removeRows(0, 10, parent2));
    QCOMPARE(v.currentIndex(), parent2);
    QVERIFY(model.removeRows(0, 15, parent1));
    QCOMPARE(v.currentIndex(), QModelIndex());
}

void tst_QAbstractItemView::setCurrentIndex_data()
{
    QTest::addColumn<QString>("viewType");
    QTest::addColumn<int>("itemFlags");
    QTest::addColumn<bool>("result");

    QStringList widgets;
    widgets << "QListView" << "QTreeView" << "QHeaderView" << "QTableView";

    foreach(QString widget, widgets) {
        QTest::newRow((widget+QLatin1String(": no flags")).toLocal8Bit().constData())
            << widget << (int)0 << false;
        QTest::newRow((widget+QLatin1String(": checkable")).toLocal8Bit().constData())
            << widget << (int)Qt::ItemIsUserCheckable << false;
        QTest::newRow((widget+QLatin1String(": selectable")).toLocal8Bit().constData())
            << widget << (int)Qt::ItemIsSelectable << false;
        QTest::newRow((widget+QLatin1String(": enabled")).toLocal8Bit().constData())
            << widget << (int)Qt::ItemIsEnabled << true;
        QTest::newRow((widget+QLatin1String(": enabled|selectable")).toLocal8Bit().constData())
            << widget << (int)(Qt::ItemIsSelectable|Qt::ItemIsEnabled) << true;
    }
}

void tst_QAbstractItemView::setCurrentIndex()
{
    QFETCH(QString, viewType);
    QFETCH(int, itemFlags);
    QFETCH(bool, result);

    TestView *view = 0;
    if (viewType == "QListView")
        view = reinterpret_cast<TestView*>(new QListView());
    else if (viewType == "QTableView")
        view = reinterpret_cast<TestView*>(new QTableView());
    else if (viewType == "QTreeView")
        view = reinterpret_cast<TestView*>(new QTreeView());
    else if (viewType == "QHeaderView")
        view = reinterpret_cast<TestView*>(new QHeaderView(Qt::Vertical));
    else
        QVERIFY(0);
    view->show();

    QStandardItemModel *model = new QStandardItemModel(view);
    QStandardItem *item = new QStandardItem("first item");
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    model->appendRow(item);

    item = new QStandardItem("test item");
    item->setFlags(Qt::ItemFlags(itemFlags));
    model->appendRow(item);

    view->setModel(model);

    view->setCurrentIndex(model->index(0,0));
    QVERIFY(view->currentIndex() == model->index(0,0));
    view->setCurrentIndex(model->index(1,0));
    QVERIFY(view->currentIndex() == model->index(result ? 1 : 0,0));

    delete view;
}

void tst_QAbstractItemView::task221955_selectedEditor()
{
    QPushButton *button;

    QTreeWidget tree;
    tree.setColumnCount(2);

    tree.addTopLevelItem(new QTreeWidgetItem(QStringList() << "Foo" <<"1"));
    tree.addTopLevelItem(new QTreeWidgetItem(QStringList() << "Bar" <<"2"));
    tree.addTopLevelItem(new QTreeWidgetItem(QStringList() << "Baz" <<"3"));

    QTreeWidgetItem *dummy = new QTreeWidgetItem();
    tree.addTopLevelItem(dummy);
    tree.setItemWidget(dummy, 0, button = new QPushButton("More..."));
    button->setAutoFillBackground(true);  // as recommended in doc

    tree.show();
    tree.setFocus();
    tree.setCurrentIndex(tree.model()->index(1,0));
    QTest::qWait(100);
    QApplication::setActiveWindow(&tree);

    QVERIFY(! tree.selectionModel()->selectedIndexes().contains(tree.model()->index(3,0)));

    //We set the focus to the button, the index need to be selected
    button->setFocus();
    QTest::qWait(100);
    QVERIFY(tree.selectionModel()->selectedIndexes().contains(tree.model()->index(3,0)));

    tree.setCurrentIndex(tree.model()->index(1,0));
    QVERIFY(! tree.selectionModel()->selectedIndexes().contains(tree.model()->index(3,0)));

    //Same thing but with the flag NoSelection,   nothing can be selected.
    tree.setFocus();
    tree.setSelectionMode(QAbstractItemView::NoSelection);
    tree.clearSelection();
    QVERIFY(tree.selectionModel()->selectedIndexes().isEmpty());
    QTest::qWait(10);
    button->setFocus();
    QTest::qWait(50);
    QVERIFY(tree.selectionModel()->selectedIndexes().isEmpty());
}

void tst_QAbstractItemView::task250754_fontChange()
{
    QString app_css = qApp->styleSheet();
    qApp->setStyleSheet("/*  */");

    QWidget w;
    QTreeView tree(&w);
    QVBoxLayout *vLayout = new QVBoxLayout(&w);
    vLayout->addWidget(&tree);

    QStandardItemModel *m = new QStandardItemModel(this);
    for (int i=0; i<5; ++i) {
        QStandardItem *item = new QStandardItem(QString("Item number %1").arg(i));
        for (int j=0; j<5; ++j) {
            QStandardItem *child = new QStandardItem(QString("Child Item number %1").arg(j));
            item->setChild(j, 0, child);
        }
        m->setItem(i, 0, item);
    }
    tree.setModel(m);

    w.show();
    w.resize(150,240);
    QTest::qWait(30);
    QFont font = tree.font();
    font.setPixelSize(10);
    tree.setFont(font);
    QTRY_VERIFY(!tree.verticalScrollBar()->isVisible());

    font.setPixelSize(60);
    tree.setFont(font);
    //now with the huge items, the scrollbar must be visible
    QTRY_VERIFY(tree.verticalScrollBar()->isVisible());

    qApp->setStyleSheet(app_css);
}

void tst_QAbstractItemView::task200665_itemEntered()
{
#ifdef Q_OS_WINCE_WM
    QSKIP("On Windows Mobile the mouse tracking is unavailable at the moment", SkipAll);
#endif
    //we test that view will emit entered
    //when the scrollbar move but not the mouse itself
    QStandardItemModel model(1000,1);
    QListView view;
    view.setModel(&model);
    view.show();
    QTest::qWait(200);
    QRect rect = view.visualRect(model.index(0,0));
    QCursor::setPos( view.viewport()->mapToGlobal(rect.center()) );
    QSignalSpy spy(&view, SIGNAL(entered(QModelIndex)));
    view.verticalScrollBar()->setValue(view.verticalScrollBar()->maximum());
    QCOMPARE(spy.count(), 1);

}

void tst_QAbstractItemView::task257481_emptyEditor()
{
    QIcon icon = qApp->style()->standardIcon(QStyle::SP_ComputerIcon);

    QStandardItemModel model;

    model.appendRow( new QStandardItem(icon, QString()) );
    model.appendRow( new QStandardItem(icon, "Editor works") );
    model.appendRow( new QStandardItem( QString() ) );

    QTreeView treeView;
    treeView.setRootIsDecorated(false);
    treeView.setModel(&model);
    treeView.show();

    treeView.edit(model.index(0,0));
    QList<QLineEdit *> lineEditors = qFindChildren<QLineEdit *>(treeView.viewport());
    QCOMPARE(lineEditors.count(), 1);
    QVERIFY(!lineEditors.first()->size().isEmpty());

    QTest::qWait(30);

    treeView.edit(model.index(1,0));
    lineEditors = qFindChildren<QLineEdit *>(treeView.viewport());
    QCOMPARE(lineEditors.count(), 1);
    QVERIFY(!lineEditors.first()->size().isEmpty());

    QTest::qWait(30);

    treeView.edit(model.index(2,0));
    lineEditors = qFindChildren<QLineEdit *>(treeView.viewport());
    QCOMPARE(lineEditors.count(), 1);
    QVERIFY(!lineEditors.first()->size().isEmpty());
}

void tst_QAbstractItemView::shiftArrowSelectionAfterScrolling()
{
    QStandardItemModel model;
    for (int i=0; i<10; ++i) {
        QStandardItem *item = new QStandardItem(QString("%1").arg(i));
        model.setItem(i, 0, item);
    }

    QListView view;
    view.setFixedSize(150, 250);
    view.setFlow(QListView::LeftToRight);
    view.setGridSize(QSize(100, 100));
    view.setSelectionMode(QListView::ExtendedSelection);
    view.setViewMode(QListView::IconMode);
    view.setModel(&model);
    view.show();
    QTest::qWait(30);

    QModelIndex index0 = model.index(0, 0);
    QModelIndex index1 = model.index(1, 0);
    QModelIndex index9 = model.index(9, 0);

    view.selectionModel()->setCurrentIndex(index0, QItemSelectionModel::NoUpdate);
    QCOMPARE(view.currentIndex(), index0);

    view.scrollTo(index9);
    QTest::keyClick(&view, Qt::Key_Down, Qt::ShiftModifier);

    QCOMPARE(view.currentIndex(), index1);
    QModelIndexList selected = view.selectionModel()->selectedIndexes();
    QCOMPARE(selected.count(), 2);
    QVERIFY(selected.contains(index0));
    QVERIFY(selected.contains(index1));
}

void tst_QAbstractItemView::shiftSelectionAfterRubberbandSelection()
{
    QStandardItemModel model;
    for (int i=0; i<3; ++i) {
        QStandardItem *item = new QStandardItem(QString("%1").arg(i));
        model.setItem(i, 0, item);
    }

    QListView view;
    view.setFixedSize(150, 450);
    view.setFlow(QListView::LeftToRight);
    view.setGridSize(QSize(100, 100));
    view.setSelectionMode(QListView::ExtendedSelection);
    view.setViewMode(QListView::IconMode);
    view.setModel(&model);
    view.show();
    QTest::qWait(30);

    QModelIndex index0 = model.index(0, 0);
    QModelIndex index1 = model.index(1, 0);
    QModelIndex index2 = model.index(2, 0);

    view.setCurrentIndex(index0);
    QCOMPARE(view.currentIndex(), index0);

    // Determine the points where the rubberband selection starts and ends
    QPoint pressPos = view.visualRect(index1).bottomRight() + QPoint(1, 1);
    QPoint releasePos = view.visualRect(index1).center();
    QVERIFY(!view.indexAt(pressPos).isValid());
    QCOMPARE(view.indexAt(releasePos), index1);

    // Select item 1 using a rubberband selection
    // The mouse move event has to be created manually because the QTest framework does not
    // contain a function for mouse moves with buttons pressed
    QTest::mousePress(view.viewport(), Qt::LeftButton, Qt::NoModifier, pressPos);
    QMouseEvent moveEvent(QEvent::MouseMove, releasePos, Qt::NoButton, Qt::LeftButton, Qt::NoModifier);
    bool moveEventReceived = qApp->notify(view.viewport(), &moveEvent);
    QVERIFY(moveEventReceived);
    QTest::mouseRelease(view.viewport(), Qt::LeftButton, Qt::NoModifier, releasePos);
    QCOMPARE(view.currentIndex(), index1);

    // Shift-click item 2
    QPoint item2Pos = view.visualRect(index2).center();
    QTest::mouseClick(view.viewport(), Qt::LeftButton, Qt::ShiftModifier, item2Pos);
    QCOMPARE(view.currentIndex(), index2);

    // Verify that the selection worked OK
    QModelIndexList selected = view.selectionModel()->selectedIndexes();
    QCOMPARE(selected.count(), 2);
    QVERIFY(selected.contains(index1));
    QVERIFY(selected.contains(index2));

    // Select item 0 to revert the selection
    view.setCurrentIndex(index0);
    QCOMPARE(view.currentIndex(), index0);

    // Repeat the same steps as above, but with a Shift-Arrow selection
    QTest::mousePress(view.viewport(), Qt::LeftButton, Qt::NoModifier, pressPos);
    QMouseEvent moveEvent2(QEvent::MouseMove, releasePos, Qt::NoButton, Qt::LeftButton, Qt::NoModifier);
    moveEventReceived = qApp->notify(view.viewport(), &moveEvent2);
    QVERIFY(moveEventReceived);
    QTest::mouseRelease(view.viewport(), Qt::LeftButton, Qt::NoModifier, releasePos);
    QCOMPARE(view.currentIndex(), index1);

    // Press Shift-Down
    QTest::keyClick(&view, Qt::Key_Down, Qt::ShiftModifier);
    QCOMPARE(view.currentIndex(), index2);

    // Verify that the selection worked OK
    selected = view.selectionModel()->selectedIndexes();
    QCOMPARE(selected.count(), 2);
    QVERIFY(selected.contains(index1));
    QVERIFY(selected.contains(index2));
}

void tst_QAbstractItemView::ctrlRubberbandSelection()
{
    QStandardItemModel model;
    for (int i=0; i<3; ++i) {
        QStandardItem *item = new QStandardItem(QString("%1").arg(i));
        model.setItem(i, 0, item);
    }

    QListView view;
    view.setFixedSize(150, 450);
    view.setFlow(QListView::LeftToRight);
    view.setGridSize(QSize(100, 100));
    view.setSelectionMode(QListView::ExtendedSelection);
    view.setViewMode(QListView::IconMode);
    view.setModel(&model);
    view.show();
    QTest::qWait(30);

    QModelIndex index1 = model.index(1, 0);
    QModelIndex index2 = model.index(2, 0);

    // Select item 1
    view.setCurrentIndex(index1);
    QModelIndexList selected = view.selectionModel()->selectedIndexes();
    QCOMPARE(selected.count(), 1);
    QVERIFY(selected.contains(index1));

    // Now press control and draw a rubberband around items 1 and 2.
    // The mouse move event has to be created manually because the QTest framework does not
    // contain a function for mouse moves with buttons pressed.
    QPoint pressPos = view.visualRect(index1).topLeft() - QPoint(1, 1);
    QPoint releasePos = view.visualRect(index2).bottomRight() + QPoint(1, 1);
    QTest::mousePress(view.viewport(), Qt::LeftButton, Qt::ControlModifier, pressPos);
    QMouseEvent moveEvent(QEvent::MouseMove, releasePos, Qt::NoButton, Qt::LeftButton, Qt::ControlModifier);
    bool moveEventReceived = qApp->notify(view.viewport(), &moveEvent);
    QVERIFY(moveEventReceived);
    QTest::mouseRelease(view.viewport(), Qt::LeftButton, Qt::ControlModifier, releasePos);

    // Verify that item 2 is selected now
    selected = view.selectionModel()->selectedIndexes();
    QCOMPARE(selected.count(), 1);
    QVERIFY(selected.contains(index2));
}

void tst_QAbstractItemView::QTBUG6407_extendedSelection()
{
    QListWidget view;
    view.setSelectionMode(QAbstractItemView::ExtendedSelection);
    for(int i = 0; i < 50; ++i)
        view.addItem(QString::number(i));

    QFont font = view.font();
    font.setPixelSize(10);
    view.setFont(font);
    view.resize(200,240);

    view.show();
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(static_cast<QWidget *>(&view), QApplication::activeWindow());

    view.verticalScrollBar()->setValue(view.verticalScrollBar()->maximum());
    QTest::qWait(20);

    QModelIndex index49 = view.model()->index(49,0);
    QPoint p = view.visualRect(index49).center();
    QVERIFY(view.viewport()->rect().contains(p));
    QTest::mouseClick(view.viewport(), Qt::LeftButton, 0, p);
    QCOMPARE(view.currentIndex(), index49);
    QCOMPARE(view.selectedItems().count(), 1);

    QModelIndex index47 = view.model()->index(47,0);
    p = view.visualRect(index47).center();
    QVERIFY(view.viewport()->rect().contains(p));
    QTest::mouseClick(view.viewport(), Qt::LeftButton, Qt::ShiftModifier, p);
    QCOMPARE(view.currentIndex(), index47);
    QCOMPARE(view.selectedItems().count(), 3); //49, 48, 47;

    QModelIndex index44 = view.model()->index(44,0);
    p = view.visualRect(index44).center();
    QVERIFY(view.viewport()->rect().contains(p));
    QTest::mouseClick(view.viewport(), Qt::LeftButton, Qt::ShiftModifier, p);
    QCOMPARE(view.currentIndex(), index44);
    QCOMPARE(view.selectedItems().count(), 6); //49 .. 44;

}

void tst_QAbstractItemView::QTBUG6753_selectOnSelection()
{
    QTableWidget table(5, 5);
    for (int i = 0; i < table.rowCount(); ++i)
        for (int j = 0; j < table.columnCount(); ++j)
            table.setItem(i, j, new QTableWidgetItem("choo-be-doo-wah"));

    table.show();
    table.setSelectionMode(QAbstractItemView::ExtendedSelection);
    table.selectAll();
    QTest::qWaitForWindowShown(&table);
    QModelIndex item = table.model()->index(1,1);
    QRect itemRect = table.visualRect(item);
    QTest::mouseMove(table.viewport(), itemRect.center());
    QTest::mouseClick(table.viewport(), Qt::LeftButton, Qt::NoModifier, itemRect.center());
    QTest::qWait(20);

    QCOMPARE(table.selectedItems().count(), 1);
    QCOMPARE(table.selectedItems().first(), table.item(item.row(), item.column()));
}

class HoverItemDelegate : public QItemDelegate
{
public:
    HoverItemDelegate()
        : QItemDelegate()
        , m_paintedWithoutHover(false)
    { }

    void paint(QPainter *painter, const QStyleOptionViewItem &opt, const QModelIndex &index) const override
    {
        Q_UNUSED(painter);

        if (!(opt.state & QStyle::State_MouseOver)) {

            // We don't want to set m_paintedWithoutHover for any item so check for the item at 0,0
            if (index.row() == 0 && index.column() == 0) {
                m_paintedWithoutHover = true;
            }
        }
    }

    mutable bool m_paintedWithoutHover;
};

void tst_QAbstractItemView::QTBUG46785_mouseout_hover_state()
{
    HoverItemDelegate delegate;

    QTableWidget table(5, 5);
    table.verticalHeader()->hide();
    table.horizontalHeader()->hide();
    table.setMouseTracking(true);
    table.setItemDelegate(&delegate);
    //centerOnScreen(&table);
    table.show();
    QApplication::setActiveWindow(&table);
    QTest::qWaitForWindowShown(&table);
    QTRY_COMPARE(static_cast<QWidget *>(&table), QApplication::activeWindow());

    QModelIndex item = table.model()->index(0, 0);
    QRect itemRect = table.visualRect(item);

    // Move the mouse into the center of the item at 0,0 to cause a paint event to occur
    QTest::mouseMove(table.viewport(), itemRect.center());
    QTest::mouseClick(table.viewport(), Qt::LeftButton, 0, itemRect.center());

    delegate.m_paintedWithoutHover = false;

    QTest::mouseMove(table.viewport(), QPoint(-50, 0));

    QTRY_VERIFY(delegate.m_paintedWithoutHover);
}

QTEST_MAIN(tst_QAbstractItemView)
#include "tst_qabstractitemview.moc"

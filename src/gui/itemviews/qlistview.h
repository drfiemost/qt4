/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QLISTVIEW_H
#define QLISTVIEW_H

#include <QtGui/qabstractitemview.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


#ifndef QT_NO_LISTVIEW

class QListViewPrivate;

class Q_GUI_EXPORT QListView : public QAbstractItemView
{
    Q_OBJECT
    Q_ENUMS(Movement Flow ResizeMode LayoutMode ViewMode)
    Q_PROPERTY(Movement movement READ movement WRITE setMovement)
    Q_PROPERTY(Flow flow READ flow WRITE setFlow)
    Q_PROPERTY(bool isWrapping READ isWrapping WRITE setWrapping)
    Q_PROPERTY(ResizeMode resizeMode READ resizeMode WRITE setResizeMode)
    Q_PROPERTY(LayoutMode layoutMode READ layoutMode WRITE setLayoutMode)
    Q_PROPERTY(int spacing READ spacing WRITE setSpacing)
    Q_PROPERTY(QSize gridSize READ gridSize WRITE setGridSize)
    Q_PROPERTY(ViewMode viewMode READ viewMode WRITE setViewMode)
    Q_PROPERTY(int modelColumn READ modelColumn WRITE setModelColumn)
    Q_PROPERTY(bool uniformItemSizes READ uniformItemSizes WRITE setUniformItemSizes)
    Q_PROPERTY(int batchSize READ batchSize WRITE setBatchSize)
    Q_PROPERTY(bool wordWrap READ wordWrap WRITE setWordWrap)
    Q_PROPERTY(bool selectionRectVisible READ isSelectionRectVisible WRITE setSelectionRectVisible)

public:
    enum Movement { Static, Free, Snap };
    enum Flow { LeftToRight, TopToBottom };
    enum ResizeMode { Fixed, Adjust };
    enum LayoutMode { SinglePass, Batched };
    enum ViewMode { ListMode, IconMode };

    explicit QListView(QWidget *parent = nullptr);
    ~QListView() override;

    void setMovement(Movement movement);
    Movement movement() const;

    void setFlow(Flow flow);
    Flow flow() const;

    void setWrapping(bool enable);
    bool isWrapping() const;

    void setResizeMode(ResizeMode mode);
    ResizeMode resizeMode() const;

    void setLayoutMode(LayoutMode mode);
    LayoutMode layoutMode() const;

    void setSpacing(int space);
    int spacing() const;

    void setBatchSize(int batchSize);
    int batchSize() const;

    void setGridSize(const QSize &size);
    QSize gridSize() const;

    void setViewMode(ViewMode mode);
    ViewMode viewMode() const;

    void clearPropertyFlags();

    bool isRowHidden(int row) const;
    void setRowHidden(int row, bool hide);

    void setModelColumn(int column);
    int modelColumn() const;

    void setUniformItemSizes(bool enable);
    bool uniformItemSizes() const;

    void setWordWrap(bool on);
    bool wordWrap() const;

    void setSelectionRectVisible(bool show);
    bool isSelectionRectVisible() const;

    QRect visualRect(const QModelIndex &index) const override;
    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible) override;
    QModelIndex indexAt(const QPoint &p) const override;

    void doItemsLayout() override;
    void reset() override;
    void setRootIndex(const QModelIndex &index) override;

Q_SIGNALS:
    void indexesMoved(const QModelIndexList &indexes);

protected:
    QListView(QListViewPrivate &, QWidget *parent = nullptr);

    bool event(QEvent *e) override;

    void scrollContentsBy(int dx, int dy) override;

    void resizeContents(int width, int height);
    QSize contentsSize() const;

    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight) override;
    void rowsInserted(const QModelIndex &parent, int start, int end) override;
    void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end) override;

    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

    void timerEvent(QTimerEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
#ifndef QT_NO_DRAGANDDROP
    void dragMoveEvent(QDragMoveEvent *e) override;
    void dragLeaveEvent(QDragLeaveEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void startDrag(Qt::DropActions supportedActions) override;

    void internalDrop(QDropEvent *e);
    void internalDrag(Qt::DropActions supportedActions);
#endif // QT_NO_DRAGANDDROP

    QStyleOptionViewItem viewOptions() const override;
    void paintEvent(QPaintEvent *e) override;

    int horizontalOffset() const override;
    int verticalOffset() const override;
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override;
    QRect rectForIndex(const QModelIndex &index) const;
    void setPositionForIndex(const QPoint &position, const QModelIndex &index);

    void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command) override;
    QRegion visualRegionForSelection(const QItemSelection &selection) const override;
    QModelIndexList selectedIndexes() const override;

    void updateGeometries() override;

    bool isIndexHidden(const QModelIndex &index) const override;

    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;

private:
    friend class QAccessibleItemView;
    int visualIndex(const QModelIndex &index) const;

    Q_DECLARE_PRIVATE(QListView)
    Q_DISABLE_COPY(QListView)
};

#endif // QT_NO_LISTVIEW

QT_END_NAMESPACE

QT_END_HEADER

#endif // QLISTVIEW_H

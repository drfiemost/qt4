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

#ifndef QDYNAMICTOOLBAR_P_H
#define QDYNAMICTOOLBAR_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qtoolbar.h"
#include "QtGui/qaction.h"
#include "private/qwidget_p.h"
#include <QtCore/qbasictimer.h>

QT_BEGIN_NAMESPACE

#ifndef QT_NO_TOOLBAR

class QToolBarLayout;
class QTimer;

class QToolBarPrivate : public QWidgetPrivate
{
    Q_DECLARE_PUBLIC(QToolBar)

public:
    inline QToolBarPrivate()
        : explicitIconSize(false), explicitToolButtonStyle(false), movable(true), floatable(true),
          allowedAreas(Qt::AllToolBarAreas), orientation(Qt::Horizontal),
          toolButtonStyle(Qt::ToolButtonIconOnly),
          layout(nullptr), state(nullptr)
#ifdef Q_WS_MAC
        , macWindowDragging(false)
#endif
    { }

    void init();
    void actionTriggered();
    void _q_toggleView(bool b);
    void _q_updateIconSize(const QSize &sz);
    void _q_updateToolButtonStyle(Qt::ToolButtonStyle style);

    bool explicitIconSize;
    bool explicitToolButtonStyle;
    bool movable;
    bool floatable;
    Qt::ToolBarAreas allowedAreas;
    Qt::Orientation orientation;
    Qt::ToolButtonStyle toolButtonStyle;
    QSize iconSize;

    QAction *toggleViewAction;

    QToolBarLayout *layout;

    struct DragState {
        QPoint pressPos;
        bool dragging;
        bool moving;
        QLayoutItem *widgetItem;
    };
    DragState *state;

#ifdef Q_WS_MAC
    bool macWindowDragging;
    QPoint macWindowDragPressPosition;
#endif

    bool mousePressEvent(QMouseEvent *e);
    bool mouseReleaseEvent(QMouseEvent *e);
    bool mouseMoveEvent(QMouseEvent *e);

    void updateWindowFlags(bool floating, bool unplug = false);
    void setWindowState(bool floating, bool unplug = false, const QRect &rect = QRect());
    void initDrag(const QPoint &pos);
    void startDrag(bool moving = false);
    void endDrag();

    void unplug(const QRect &r);
    void plug(const QRect &r);

    QBasicTimer waitForPopupTimer;
};

#endif // QT_NO_TOOLBAR

QT_END_NAMESPACE

#endif // QDYNAMICTOOLBAR_P_H

/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#ifndef ZOOMTOOL_H
#define ZOOMTOOL_H

#include "abstractliveedittool.h"
#include "liverubberbandselectionmanipulator.h"

QT_FORWARD_DECLARE_CLASS(QAction)

namespace QmlJSDebugger {

class ZoomTool : public AbstractLiveEditTool
{
    Q_OBJECT

public:
    enum ZoomDirection {
        ZoomIn,
        ZoomOut
    };

    explicit ZoomTool(QDeclarativeViewInspector *view);

    ~ZoomTool() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    void hoverMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *keyEvent) override;
    void itemsAboutToRemoved(const QList<QGraphicsItem*> &) override {}

    void clear() override;

protected:
    void selectedItemsChanged(const QList<QGraphicsItem*> &) override {}

private slots:
    void zoomTo100();
    void zoomIn();
    void zoomOut();

private:
    qreal nextZoomScale(ZoomDirection direction) const;
    void scaleView(const QPointF &centerPos);

private:
    bool m_dragStarted;
    QPoint m_mousePos; // in view coords
    QPointF m_dragBeginPos;
    QAction *m_zoomTo100Action;
    QAction *m_zoomInAction;
    QAction *m_zoomOutAction;
    LiveRubberBandSelectionManipulator *m_rubberbandManipulator;

    qreal m_smoothZoomMultiplier;
    qreal m_currentScale;
};

} // namespace QmlJSDebugger

#endif // ZOOMTOOL_H

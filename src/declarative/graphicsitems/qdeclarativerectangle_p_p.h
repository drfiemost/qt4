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

#ifndef QDECLARATIVERECT_P_H
#define QDECLARATIVERECT_P_H

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

#include "private/qdeclarativeitem_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativeGradient;
class QDeclarativeRectangle;
class QDeclarativeRectanglePrivate : public QDeclarativeItemPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeRectangle)

public:
    QDeclarativeRectanglePrivate() :
    color(Qt::white), gradient(nullptr), pen(nullptr), radius(0), paintmargin(0)
    {
        QGraphicsItemPrivate::flags = QGraphicsItemPrivate::flags & ~QGraphicsItem::ItemHasNoContents;
    }

    ~QDeclarativeRectanglePrivate() override
    {
        delete pen;
    }

    QColor color;
    QDeclarativeGradient *gradient;
    QDeclarativePen *pen;
    qreal radius;
    qreal paintmargin;
    QPixmap rectImage;
    static int doUpdateSlotIdx;

    QDeclarativePen *getPen() {
        if (!pen) {
            Q_Q(QDeclarativeRectangle);
            pen = new QDeclarativePen;
            static int penChangedSignalIdx = -1;
            if (penChangedSignalIdx < 0)
                penChangedSignalIdx = QDeclarativePen::staticMetaObject.indexOfSignal("penChanged()");
            if (doUpdateSlotIdx < 0)
                doUpdateSlotIdx = QDeclarativeRectangle::staticMetaObject.indexOfSlot("doUpdate()");
            QMetaObject::connect(pen, penChangedSignalIdx, q, doUpdateSlotIdx);
        }
        return pen;
    }

    void setPaintMargin(qreal margin)
    {
        Q_Q(QDeclarativeRectangle);
        if (margin == paintmargin)
            return;
        q->prepareGeometryChange();
        paintmargin = margin;
    }
};

QT_END_NAMESPACE

#endif // QDECLARATIVERECT_P_H

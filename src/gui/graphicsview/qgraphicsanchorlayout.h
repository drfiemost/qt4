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

#ifndef QGRAPHICSANCHORLAYOUT_H
#define QGRAPHICSANCHORLAYOUT_H

#include <QtGui/qgraphicsitem.h>
#include <QtGui/qgraphicslayout.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


#if !defined(QT_NO_GRAPHICSVIEW)

class QGraphicsAnchorPrivate;
class QGraphicsAnchorLayout;
class QGraphicsAnchorLayoutPrivate;

class Q_GUI_EXPORT QGraphicsAnchor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal spacing READ spacing WRITE setSpacing RESET unsetSpacing)
    Q_PROPERTY(QSizePolicy::Policy sizePolicy READ sizePolicy WRITE setSizePolicy)
public:
    void setSpacing(qreal spacing);
    void unsetSpacing();
    qreal spacing() const;
    void setSizePolicy(QSizePolicy::Policy policy);
    QSizePolicy::Policy sizePolicy() const;
    ~QGraphicsAnchor() override;
private:
    QGraphicsAnchor(QGraphicsAnchorLayout *parent);

    Q_DECLARE_PRIVATE(QGraphicsAnchor)

    friend class QGraphicsAnchorLayoutPrivate;
    friend struct AnchorData;
};

class Q_GUI_EXPORT QGraphicsAnchorLayout : public QGraphicsLayout
{
public:
    QGraphicsAnchorLayout(QGraphicsLayoutItem *parent = nullptr);
    ~QGraphicsAnchorLayout() override;

    QGraphicsAnchor *addAnchor(QGraphicsLayoutItem *firstItem, Qt::AnchorPoint firstEdge,
                               QGraphicsLayoutItem *secondItem, Qt::AnchorPoint secondEdge);
    QGraphicsAnchor *anchor(QGraphicsLayoutItem *firstItem, Qt::AnchorPoint firstEdge,
                            QGraphicsLayoutItem *secondItem, Qt::AnchorPoint secondEdge);

    void addCornerAnchors(QGraphicsLayoutItem *firstItem, Qt::Corner firstCorner,
                          QGraphicsLayoutItem *secondItem, Qt::Corner secondCorner);

    void addAnchors(QGraphicsLayoutItem *firstItem,
                    QGraphicsLayoutItem *secondItem,
                    Qt::Orientations orientations = Qt::Horizontal | Qt::Vertical);

    void setHorizontalSpacing(qreal spacing);
    void setVerticalSpacing(qreal spacing);
    void setSpacing(qreal spacing);
    qreal horizontalSpacing() const;
    qreal verticalSpacing() const;

    void removeAt(int index) override;
    void setGeometry(const QRectF &rect) override;
    int count() const override;
    QGraphicsLayoutItem *itemAt(int index) const override;

    void invalidate() override;
protected:
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const override;

private:
    Q_DISABLE_COPY(QGraphicsAnchorLayout)
    Q_DECLARE_PRIVATE(QGraphicsAnchorLayout)

    friend class QGraphicsAnchor;
};

#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif

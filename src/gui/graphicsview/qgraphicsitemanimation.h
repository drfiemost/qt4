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

#ifndef QGRAPHICSITEMANIMATION_H
#define QGRAPHICSITEMANIMATION_H

#include <QtCore/qobject.h>

#if !defined(QT_NO_GRAPHICSVIEW)

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QGraphicsItem;
class QMatrix;
class QPointF;
class QTimeLine;
template <class T1, class T2> struct QPair;

class QGraphicsItemAnimationPrivate;
class Q_GUI_EXPORT QGraphicsItemAnimation : public QObject
{
    Q_OBJECT
public:
    QGraphicsItemAnimation(QObject *parent = nullptr);
    ~QGraphicsItemAnimation() override;

    QGraphicsItem *item() const;
    void setItem(QGraphicsItem *item);

    QTimeLine *timeLine() const;
    void setTimeLine(QTimeLine *timeLine);

    QPointF posAt(qreal step) const;
    QList<QPair<qreal, QPointF> > posList() const;
    void setPosAt(qreal step, const QPointF &pos);

    QMatrix matrixAt(qreal step) const;

    qreal rotationAt(qreal step) const;
    QList<QPair<qreal, qreal> > rotationList() const;
    void setRotationAt(qreal step, qreal angle);

    qreal xTranslationAt(qreal step) const;
    qreal yTranslationAt(qreal step) const;
    QList<QPair<qreal, QPointF> > translationList() const;
    void setTranslationAt(qreal step, qreal dx, qreal dy);

    qreal verticalScaleAt(qreal step) const;
    qreal horizontalScaleAt(qreal step) const;
    QList<QPair<qreal, QPointF> > scaleList() const;
    void setScaleAt(qreal step, qreal sx, qreal sy);

    qreal verticalShearAt(qreal step) const;
    qreal horizontalShearAt(qreal step) const;
    QList<QPair<qreal, QPointF> > shearList() const;
    void setShearAt(qreal step, qreal sh, qreal sv);

    void clear();

public Q_SLOTS:
    void setStep(qreal x);
    void reset();

protected:
    virtual void beforeAnimationStep(qreal step);
    virtual void afterAnimationStep(qreal step);

private:
    Q_DISABLE_COPY(QGraphicsItemAnimation)
    QGraphicsItemAnimationPrivate *d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_GRAPHICSVIEW
#endif

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

#ifndef QGRAPHICSTRANSFORM_H
#define QGRAPHICSTRANSFORM_H

#include <QtCore/QObject>
#include <QtGui/QVector3D>
#include <QtGui/QTransform>
#include <QtGui/QMatrix4x4>

#ifndef QT_NO_GRAPHICSVIEW
QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QGraphicsItem;
class QGraphicsTransformPrivate;

class Q_GUI_EXPORT QGraphicsTransform : public QObject
{
    Q_OBJECT
public:
    QGraphicsTransform(QObject *parent = nullptr);
    ~QGraphicsTransform() override;

    virtual void applyTo(QMatrix4x4 *matrix) const = 0;

protected Q_SLOTS:
    void update();

protected:
    QGraphicsTransform(QGraphicsTransformPrivate &p, QObject *parent);

private:
    friend class QGraphicsItem;
    friend class QGraphicsItemPrivate;
    Q_DECLARE_PRIVATE(QGraphicsTransform)
};

class QGraphicsScalePrivate;

class Q_GUI_EXPORT QGraphicsScale : public QGraphicsTransform
{
    Q_OBJECT

    Q_PROPERTY(QVector3D origin READ origin WRITE setOrigin NOTIFY originChanged)
    Q_PROPERTY(qreal xScale READ xScale WRITE setXScale NOTIFY xScaleChanged)
    Q_PROPERTY(qreal yScale READ yScale WRITE setYScale NOTIFY yScaleChanged)
    Q_PROPERTY(qreal zScale READ zScale WRITE setZScale NOTIFY zScaleChanged)
public:
    QGraphicsScale(QObject *parent = nullptr);
    ~QGraphicsScale() override;

    QVector3D origin() const;
    void setOrigin(const QVector3D &point);

    qreal xScale() const;
    void setXScale(qreal);

    qreal yScale() const;
    void setYScale(qreal);

    qreal zScale() const;
    void setZScale(qreal);

    void applyTo(QMatrix4x4 *matrix) const override;

Q_SIGNALS:
    void originChanged();
    void xScaleChanged();
    void yScaleChanged();
    void zScaleChanged();
    void scaleChanged();

private:
    Q_DECLARE_PRIVATE(QGraphicsScale)
};

class QGraphicsRotationPrivate;

class Q_GUI_EXPORT QGraphicsRotation : public QGraphicsTransform
{
    Q_OBJECT

    Q_PROPERTY(QVector3D origin READ origin WRITE setOrigin NOTIFY originChanged)
    Q_PROPERTY(qreal angle READ angle WRITE setAngle NOTIFY angleChanged)
    Q_PROPERTY(QVector3D axis READ axis WRITE setAxis NOTIFY axisChanged)
public:
    QGraphicsRotation(QObject *parent = nullptr);
    ~QGraphicsRotation() override;

    QVector3D origin() const;
    void setOrigin(const QVector3D &point);

    qreal angle() const;
    void setAngle(qreal);

    QVector3D axis() const;
    void setAxis(const QVector3D &axis);
    void setAxis(Qt::Axis axis);

    void applyTo(QMatrix4x4 *matrix) const override;

Q_SIGNALS:
    void originChanged();
    void angleChanged();
    void axisChanged();

private:
    Q_DECLARE_PRIVATE(QGraphicsRotation)
};

QT_END_NAMESPACE

QT_END_HEADER
#endif //QT_NO_GRAPHICSVIEW

#endif // QFXTRANSFORM_H

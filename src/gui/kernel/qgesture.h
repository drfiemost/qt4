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

#ifndef QGESTURE_H
#define QGESTURE_H

#include <QtCore/qobject.h>
#include <QtCore/qlist.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qpoint.h>
#include <QtCore/qrect.h>
#include <QtCore/qmetatype.h>

#ifndef QT_NO_GESTURES

QT_BEGIN_HEADER

Q_DECLARE_METATYPE(Qt::GestureState)
Q_DECLARE_METATYPE(Qt::GestureType)

QT_BEGIN_NAMESPACE


class QGesturePrivate;
class Q_GUI_EXPORT QGesture : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGesture)

    Q_PROPERTY(Qt::GestureState state READ state)
    Q_PROPERTY(Qt::GestureType gestureType READ gestureType)
    Q_PROPERTY(QGesture::GestureCancelPolicy gestureCancelPolicy READ gestureCancelPolicy WRITE setGestureCancelPolicy)
    Q_PROPERTY(QPointF hotSpot READ hotSpot WRITE setHotSpot RESET unsetHotSpot)
    Q_PROPERTY(bool hasHotSpot READ hasHotSpot)

public:
    explicit QGesture(QObject *parent = nullptr);
    ~QGesture() override;

    Qt::GestureType gestureType() const;

    Qt::GestureState state() const;

    QPointF hotSpot() const;
    void setHotSpot(const QPointF &value);
    bool hasHotSpot() const;
    void unsetHotSpot();

    enum GestureCancelPolicy {
        CancelNone = 0,
        CancelAllInContext
    };

    void setGestureCancelPolicy(GestureCancelPolicy policy);
    GestureCancelPolicy gestureCancelPolicy() const;

protected:
    QGesture(QGesturePrivate &dd, QObject *parent);

private:
    friend class QGestureEvent;
    friend class QGestureRecognizer;
    friend class QGestureManager;
    friend class QGraphicsScenePrivate;
};

class QPanGesturePrivate;
class Q_GUI_EXPORT QPanGesture : public QGesture
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QPanGesture)

    Q_PROPERTY(QPointF lastOffset READ lastOffset WRITE setLastOffset)
    Q_PROPERTY(QPointF offset READ offset WRITE setOffset)
    Q_PROPERTY(QPointF delta READ delta STORED false)
    Q_PROPERTY(qreal acceleration READ acceleration WRITE setAcceleration)
    Q_PRIVATE_PROPERTY(QPanGesture::d_func(), qreal horizontalVelocity READ horizontalVelocity WRITE setHorizontalVelocity)
    Q_PRIVATE_PROPERTY(QPanGesture::d_func(), qreal verticalVelocity READ verticalVelocity WRITE setVerticalVelocity)

public:
    explicit QPanGesture(QObject *parent = nullptr);
    ~QPanGesture() override;

    QPointF lastOffset() const;
    QPointF offset() const;
    QPointF delta() const;
    qreal acceleration() const;

    void setLastOffset(const QPointF &value);
    void setOffset(const QPointF &value);
    void setAcceleration(qreal value);

    friend class QPanGestureRecognizer;
    friend class QWinNativePanGestureRecognizer;
};

class QPinchGesturePrivate;
class Q_GUI_EXPORT QPinchGesture : public QGesture
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QPinchGesture)
    Q_FLAGS(ChangeFlags ChangeFlag)

public:
    enum ChangeFlag {
        ScaleFactorChanged = 0x1,
        RotationAngleChanged = 0x2,
        CenterPointChanged = 0x4
    };
    Q_DECLARE_FLAGS(ChangeFlags, ChangeFlag)

    Q_PROPERTY(ChangeFlags totalChangeFlags READ totalChangeFlags WRITE setTotalChangeFlags)
    Q_PROPERTY(ChangeFlags changeFlags READ changeFlags WRITE setChangeFlags)

    Q_PROPERTY(qreal totalScaleFactor READ totalScaleFactor WRITE setTotalScaleFactor)
    Q_PROPERTY(qreal lastScaleFactor READ lastScaleFactor WRITE setLastScaleFactor)
    Q_PROPERTY(qreal scaleFactor READ scaleFactor WRITE setScaleFactor)

    Q_PROPERTY(qreal totalRotationAngle READ totalRotationAngle WRITE setTotalRotationAngle)
    Q_PROPERTY(qreal lastRotationAngle READ lastRotationAngle WRITE setLastRotationAngle)
    Q_PROPERTY(qreal rotationAngle READ rotationAngle WRITE setRotationAngle)

    Q_PROPERTY(QPointF startCenterPoint READ startCenterPoint WRITE setStartCenterPoint)
    Q_PROPERTY(QPointF lastCenterPoint READ lastCenterPoint WRITE setLastCenterPoint)
    Q_PROPERTY(QPointF centerPoint READ centerPoint WRITE setCenterPoint)

public:
    explicit QPinchGesture(QObject *parent = nullptr);
    ~QPinchGesture() override;

    ChangeFlags totalChangeFlags() const;
    void setTotalChangeFlags(ChangeFlags value);

    ChangeFlags changeFlags() const;
    void setChangeFlags(ChangeFlags value);

    QPointF startCenterPoint() const;
    QPointF lastCenterPoint() const;
    QPointF centerPoint() const;
    void setStartCenterPoint(const QPointF &value);
    void setLastCenterPoint(const QPointF &value);
    void setCenterPoint(const QPointF &value);

    qreal totalScaleFactor() const;
    qreal lastScaleFactor() const;
    qreal scaleFactor() const;
    void setTotalScaleFactor(qreal value);
    void setLastScaleFactor(qreal value);
    void setScaleFactor(qreal value);

    qreal totalRotationAngle() const;
    qreal lastRotationAngle() const;
    qreal rotationAngle() const;
    void setTotalRotationAngle(qreal value);
    void setLastRotationAngle(qreal value);
    void setRotationAngle(qreal value);

    friend class QPinchGestureRecognizer;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QPinchGesture::ChangeFlags)

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QPinchGesture::ChangeFlags)

QT_BEGIN_NAMESPACE

class QSwipeGesturePrivate;
class Q_GUI_EXPORT QSwipeGesture : public QGesture
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QSwipeGesture)
    Q_ENUMS(SwipeDirection)

    Q_PROPERTY(SwipeDirection horizontalDirection READ horizontalDirection STORED false)
    Q_PROPERTY(SwipeDirection verticalDirection READ verticalDirection STORED false)
    Q_PROPERTY(qreal swipeAngle READ swipeAngle WRITE setSwipeAngle)
    Q_PRIVATE_PROPERTY(QSwipeGesture::d_func(), qreal velocity READ velocity WRITE setVelocity)

public:
    enum SwipeDirection { NoDirection, Left, Right, Up, Down };
    explicit QSwipeGesture(QObject *parent = nullptr);
    ~QSwipeGesture() override;

    SwipeDirection horizontalDirection() const;
    SwipeDirection verticalDirection() const;

    qreal swipeAngle() const;
    void setSwipeAngle(qreal value);

    friend class QSwipeGestureRecognizer;
};

class QTapGesturePrivate;
class Q_GUI_EXPORT QTapGesture : public QGesture
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QTapGesture)

    Q_PROPERTY(QPointF position READ position WRITE setPosition)

public:
    explicit QTapGesture(QObject *parent = nullptr);
    ~QTapGesture() override;

    QPointF position() const;
    void setPosition(const QPointF &pos);

    friend class QTapGestureRecognizer;
};

class QTapAndHoldGesturePrivate;
class Q_GUI_EXPORT QTapAndHoldGesture : public QGesture
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QTapAndHoldGesture)

    Q_PROPERTY(QPointF position READ position WRITE setPosition)

public:
    explicit QTapAndHoldGesture(QObject *parent = nullptr);
    ~QTapAndHoldGesture() override;

    QPointF position() const;
    void setPosition(const QPointF &pos);

    static void setTimeout(int msecs);
    static int timeout();

    friend class QTapAndHoldGestureRecognizer;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGesture::GestureCancelPolicy)
QT_END_HEADER

#endif // QT_NO_GESTURES

#endif // QGESTURE_H

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

#ifndef QVECTOR2D_H
#define QVECTOR2D_H

#include <QtCore/qpoint.h>
#include <QtCore/qmetatype.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QVector3D;
class QVector4D;
class QVariant;

#ifndef QT_NO_VECTOR2D

class Q_GUI_EXPORT QVector2D
{
public:
    QVector2D();
    QVector2D(float xpos, float ypos);
    explicit QVector2D(const QPoint& point);
    explicit QVector2D(const QPointF& point);
#ifndef QT_NO_VECTOR3D
    explicit QVector2D(const QVector3D& vector);
#endif
#ifndef QT_NO_VECTOR4D
    explicit QVector2D(const QVector4D& vector);
#endif

    bool isNull() const;

    float x() const;
    float y() const;

    void setX(float x);
    void setY(float y);

    float length() const;
    float lengthSquared() const;

    QVector2D normalized() const;
    void normalize();

    QVector2D &operator+=(const QVector2D &vector);
    QVector2D &operator-=(const QVector2D &vector);
    QVector2D &operator*=(float factor);
    QVector2D &operator*=(const QVector2D &vector);
    QVector2D &operator/=(float divisor);

    static float dotProduct(const QVector2D& v1, const QVector2D& v2);

    friend inline bool operator==(const QVector2D &v1, const QVector2D &v2);
    friend inline bool operator!=(const QVector2D &v1, const QVector2D &v2);
    friend inline const QVector2D operator+(const QVector2D &v1, const QVector2D &v2);
    friend inline const QVector2D operator-(const QVector2D &v1, const QVector2D &v2);
    friend inline const QVector2D operator*(float factor, const QVector2D &vector);
    friend inline const QVector2D operator*(const QVector2D &vector, float factor);
    friend inline const QVector2D operator*(const QVector2D &v1, const QVector2D &v2);
    friend inline const QVector2D operator-(const QVector2D &vector);
    friend inline const QVector2D operator/(const QVector2D &vector, float divisor);

    friend inline bool qFuzzyCompare(const QVector2D& v1, const QVector2D& v2);

#ifndef QT_NO_VECTOR3D
    QVector3D toVector3D() const;
#endif
#ifndef QT_NO_VECTOR4D
    QVector4D toVector4D() const;
#endif

    QPoint toPoint() const;
    QPointF toPointF() const;

    operator QVariant() const;

private:
    float xp, yp;

    friend class QVector3D;
    friend class QVector4D;
};

Q_DECLARE_TYPEINFO(QVector2D, Q_MOVABLE_TYPE);

inline QVector2D::QVector2D() : xp(0.0f), yp(0.0f) {}

inline QVector2D::QVector2D(float xpos, float ypos) : xp(xpos), yp(ypos) {}

inline QVector2D::QVector2D(const QPoint& point) : xp(point.x()), yp(point.y()) {}

inline QVector2D::QVector2D(const QPointF& point) : xp(point.x()), yp(point.y()) {}

inline bool QVector2D::isNull() const
{
    return qIsNull(xp) && qIsNull(yp);
}

inline float QVector2D::x() const { return xp; }
inline float QVector2D::y() const { return yp; }

inline void QVector2D::setX(float aX) { xp = aX; }
inline void QVector2D::setY(float aY) { yp = aY; }

inline QVector2D &QVector2D::operator+=(const QVector2D &vector)
{
    xp += vector.xp;
    yp += vector.yp;
    return *this;
}

inline QVector2D &QVector2D::operator-=(const QVector2D &vector)
{
    xp -= vector.xp;
    yp -= vector.yp;
    return *this;
}

inline QVector2D &QVector2D::operator*=(float factor)
{
    xp *= factor;
    yp *= factor;
    return *this;
}

inline QVector2D &QVector2D::operator*=(const QVector2D &vector)
{
    xp *= vector.xp;
    yp *= vector.yp;
    return *this;
}

inline QVector2D &QVector2D::operator/=(float divisor)
{
    xp /= divisor;
    yp /= divisor;
    return *this;
}

inline bool operator==(const QVector2D &v1, const QVector2D &v2)
{
    return v1.xp == v2.xp && v1.yp == v2.yp;
}

inline bool operator!=(const QVector2D &v1, const QVector2D &v2)
{
    return v1.xp != v2.xp || v1.yp != v2.yp;
}

inline const QVector2D operator+(const QVector2D &v1, const QVector2D &v2)
{
    return QVector2D(v1.xp + v2.xp, v1.yp + v2.yp);
}

inline const QVector2D operator-(const QVector2D &v1, const QVector2D &v2)
{
    return QVector2D(v1.xp - v2.xp, v1.yp - v2.yp);
}

inline const QVector2D operator*(float factor, const QVector2D &vector)
{
    return QVector2D(vector.xp * factor, vector.yp * factor);
}

inline const QVector2D operator*(const QVector2D &vector, float factor)
{
    return QVector2D(vector.xp * factor, vector.yp * factor);
}

inline const QVector2D operator*(const QVector2D &v1, const QVector2D &v2)
{
    return QVector2D(v1.xp * v2.xp, v1.yp * v2.yp);
}

inline const QVector2D operator-(const QVector2D &vector)
{
    return QVector2D(-vector.xp, -vector.yp);
}

inline const QVector2D operator/(const QVector2D &vector, float divisor)
{
    return QVector2D(vector.xp / divisor, vector.yp / divisor);
}

inline bool qFuzzyCompare(const QVector2D& v1, const QVector2D& v2)
{
    return qFuzzyCompare(v1.xp, v2.xp) && qFuzzyCompare(v1.yp, v2.yp);
}

inline QPoint QVector2D::toPoint() const
{
    return QPoint(qRound(xp), qRound(yp));
}

inline QPointF QVector2D::toPointF() const
{
    return QPointF(qreal(xp), qreal(yp));
}

#ifndef QT_NO_DEBUG_STREAM
Q_GUI_EXPORT QDebug operator<<(QDebug dbg, const QVector2D &vector);
#endif

#ifndef QT_NO_DATASTREAM
Q_GUI_EXPORT QDataStream &operator<<(QDataStream &, const QVector2D &);
Q_GUI_EXPORT QDataStream &operator>>(QDataStream &, QVector2D &);
#endif

#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif

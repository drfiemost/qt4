/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QSIZE_H
#define QSIZE_H

#include <QtCore/qnamespace.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class Q_CORE_EXPORT QSize
{
public:
    constexpr QSize();
    constexpr QSize(int w, int h);

    constexpr bool isNull() const;
    constexpr bool isEmpty() const;
    constexpr bool isValid() const;

    constexpr int width() const;
    constexpr int height() const;
    void setWidth(int w);
    void setHeight(int h);
    void transpose();

    void scale(int w, int h, Qt::AspectRatioMode mode);
    void scale(const QSize &s, Qt::AspectRatioMode mode);

    constexpr QSize expandedTo(const QSize &) const;
    constexpr QSize boundedTo(const QSize &) const;

    int &rwidth();
    int &rheight();

    QSize &operator+=(const QSize &);
    QSize &operator-=(const QSize &);
    QSize &operator*=(qreal c);
    QSize &operator/=(qreal c);

    friend inline constexpr bool operator==(const QSize &, const QSize &);
    friend inline constexpr bool operator!=(const QSize &, const QSize &);
    friend inline constexpr const QSize operator+(const QSize &, const QSize &);
    friend inline constexpr const QSize operator-(const QSize &, const QSize &);
    friend inline constexpr const QSize operator*(const QSize &, qreal);
    friend inline constexpr const QSize operator*(qreal, const QSize &);
    friend inline const QSize operator/(const QSize &, qreal);

private:
    int wd;
    int ht;
};
Q_DECLARE_TYPEINFO(QSize, Q_MOVABLE_TYPE);

/*****************************************************************************
  QSize stream functions
 *****************************************************************************/

#ifndef QT_NO_DATASTREAM
Q_CORE_EXPORT QDataStream &operator<<(QDataStream &, const QSize &);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &, QSize &);
#endif


/*****************************************************************************
  QSize inline functions
 *****************************************************************************/

constexpr inline QSize::QSize() : wd(-1), ht(-1) {}

constexpr inline QSize::QSize(int w, int h) : wd(w), ht(h) {}

constexpr inline bool QSize::isNull() const
{ return wd==0 && ht==0; }

constexpr inline bool QSize::isEmpty() const
{ return wd<1 || ht<1; }

constexpr inline bool QSize::isValid() const
{ return wd>=0 && ht>=0; }

constexpr inline int QSize::width() const
{ return wd; }

constexpr inline int QSize::height() const
{ return ht; }

inline void QSize::setWidth(int w)
{ wd = w; }

inline void QSize::setHeight(int h)
{ ht = h; }

inline void QSize::scale(int w, int h, Qt::AspectRatioMode mode)
{ scale(QSize(w, h), mode); }

inline int &QSize::rwidth()
{ return wd; }

inline int &QSize::rheight()
{ return ht; }

inline QSize &QSize::operator+=(const QSize &s)
{ wd+=s.wd; ht+=s.ht; return *this; }

inline QSize &QSize::operator-=(const QSize &s)
{ wd-=s.wd; ht-=s.ht; return *this; }

inline QSize &QSize::operator*=(qreal c)
{ wd = qRound(wd*c); ht = qRound(ht*c); return *this; }

constexpr inline bool operator==(const QSize &s1, const QSize &s2)
{ return s1.wd == s2.wd && s1.ht == s2.ht; }

constexpr inline bool operator!=(const QSize &s1, const QSize &s2)
{ return s1.wd != s2.wd || s1.ht != s2.ht; }

constexpr inline const QSize operator+(const QSize & s1, const QSize & s2)
{ return QSize(s1.wd+s2.wd, s1.ht+s2.ht); }

constexpr inline const QSize operator-(const QSize &s1, const QSize &s2)
{ return QSize(s1.wd-s2.wd, s1.ht-s2.ht); }

constexpr inline const QSize operator*(const QSize &s, qreal c)
{ return QSize(qRound(s.wd*c), qRound(s.ht*c)); }

constexpr inline const QSize operator*(qreal c, const QSize &s)
{ return QSize(qRound(s.wd*c), qRound(s.ht*c)); }

inline QSize &QSize::operator/=(qreal c)
{
    Q_ASSERT(!qFuzzyIsNull(c));
    wd = qRound(wd/c); ht = qRound(ht/c);
    return *this;
}

inline const QSize operator/(const QSize &s, qreal c)
{
    Q_ASSERT(!qFuzzyIsNull(c));
    return QSize(qRound(s.wd/c), qRound(s.ht/c));
}

constexpr inline QSize QSize::expandedTo(const QSize & otherSize) const
{
    return QSize(std::max(wd,otherSize.wd), std::max(ht,otherSize.ht));
}

constexpr inline QSize QSize::boundedTo(const QSize & otherSize) const
{
    return QSize(std::min(wd,otherSize.wd), std::min(ht,otherSize.ht));
}

#ifndef QT_NO_DEBUG_STREAM
Q_CORE_EXPORT QDebug operator<<(QDebug, const QSize &);
#endif


class Q_CORE_EXPORT QSizeF
{
public:
    constexpr QSizeF();
    constexpr QSizeF(const QSize &sz);
    constexpr QSizeF(qreal w, qreal h);

    bool isNull() const;
    constexpr bool isEmpty() const;
    constexpr bool isValid() const;

    constexpr qreal width() const;
    constexpr qreal height() const;
    void setWidth(qreal w);
    void setHeight(qreal h);
    void transpose();

    void scale(qreal w, qreal h, Qt::AspectRatioMode mode);
    void scale(const QSizeF &s, Qt::AspectRatioMode mode);

    constexpr QSizeF expandedTo(const QSizeF &) const;
    constexpr QSizeF boundedTo(const QSizeF &) const;

    qreal &rwidth();
    qreal &rheight();

    QSizeF &operator+=(const QSizeF &);
    QSizeF &operator-=(const QSizeF &);
    QSizeF &operator*=(qreal c);
    QSizeF &operator/=(qreal c);

    friend constexpr inline bool operator==(const QSizeF &, const QSizeF &);
    friend constexpr inline bool operator!=(const QSizeF &, const QSizeF &);
    friend constexpr inline const QSizeF operator+(const QSizeF &, const QSizeF &);
    friend constexpr inline const QSizeF operator-(const QSizeF &, const QSizeF &);
    friend constexpr inline const QSizeF operator*(const QSizeF &, qreal);
    friend constexpr inline const QSizeF operator*(qreal, const QSizeF &);
    friend inline const QSizeF operator/(const QSizeF &, qreal);

    constexpr inline QSize toSize() const;

private:
    qreal wd;
    qreal ht;
};
Q_DECLARE_TYPEINFO(QSizeF, Q_MOVABLE_TYPE);


/*****************************************************************************
  QSizeF stream functions
 *****************************************************************************/

#ifndef QT_NO_DATASTREAM
Q_CORE_EXPORT QDataStream &operator<<(QDataStream &, const QSizeF &);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &, QSizeF &);
#endif


/*****************************************************************************
  QSizeF inline functions
 *****************************************************************************/

constexpr inline QSizeF::QSizeF() : wd(-1.), ht(-1.) {}

constexpr inline QSizeF::QSizeF(const QSize &sz) : wd(sz.width()), ht(sz.height()) {}

constexpr inline QSizeF::QSizeF(qreal w, qreal h) : wd(w), ht(h) {}

inline bool QSizeF::isNull() const
{ return qIsNull(wd) && qIsNull(ht); }

constexpr inline bool QSizeF::isEmpty() const
{ return wd <= 0. || ht <= 0.; }

constexpr inline bool QSizeF::isValid() const
{ return wd >= 0. && ht >= 0.; }

constexpr inline qreal QSizeF::width() const
{ return wd; }

constexpr inline qreal QSizeF::height() const
{ return ht; }

inline void QSizeF::setWidth(qreal w)
{ wd = w; }

inline void QSizeF::setHeight(qreal h)
{ ht = h; }

inline void QSizeF::scale(qreal w, qreal h, Qt::AspectRatioMode mode)
{ scale(QSizeF(w, h), mode); }

inline qreal &QSizeF::rwidth()
{ return wd; }

inline qreal &QSizeF::rheight()
{ return ht; }

inline QSizeF &QSizeF::operator+=(const QSizeF &s)
{ wd += s.wd; ht += s.ht; return *this; }

inline QSizeF &QSizeF::operator-=(const QSizeF &s)
{ wd -= s.wd; ht -= s.ht; return *this; }

inline QSizeF &QSizeF::operator*=(qreal c)
{ wd *= c; ht *= c; return *this; }

constexpr inline bool operator==(const QSizeF &s1, const QSizeF &s2)
{ return qFuzzyCompare(s1.wd, s2.wd) && qFuzzyCompare(s1.ht, s2.ht); }

constexpr inline bool operator!=(const QSizeF &s1, const QSizeF &s2)
{ return !qFuzzyCompare(s1.wd, s2.wd) || !qFuzzyCompare(s1.ht, s2.ht); }

constexpr inline const QSizeF operator+(const QSizeF & s1, const QSizeF & s2)
{ return QSizeF(s1.wd+s2.wd, s1.ht+s2.ht); }

constexpr inline const QSizeF operator-(const QSizeF &s1, const QSizeF &s2)
{ return QSizeF(s1.wd-s2.wd, s1.ht-s2.ht); }

constexpr inline const QSizeF operator*(const QSizeF &s, qreal c)
{ return QSizeF(s.wd*c, s.ht*c); }

constexpr inline const QSizeF operator*(qreal c, const QSizeF &s)
{ return QSizeF(s.wd*c, s.ht*c); }

inline QSizeF &QSizeF::operator/=(qreal c)
{
    Q_ASSERT(!qFuzzyIsNull(c));
    wd = wd/c; ht = ht/c;
    return *this;
}

inline const QSizeF operator/(const QSizeF &s, qreal c)
{
    Q_ASSERT(!qFuzzyIsNull(c));
    return QSizeF(s.wd/c, s.ht/c);
}

constexpr inline QSizeF QSizeF::expandedTo(const QSizeF & otherSize) const
{
    return QSizeF(std::max(wd,otherSize.wd), std::max(ht,otherSize.ht));
}

constexpr inline QSizeF QSizeF::boundedTo(const QSizeF & otherSize) const
{
    return QSizeF(std::min(wd,otherSize.wd), std::min(ht,otherSize.ht));
}

constexpr inline QSize QSizeF::toSize() const
{
    return QSize(qRound(wd), qRound(ht));
}

#ifndef QT_NO_DEBUG_STREAM
Q_CORE_EXPORT QDebug operator<<(QDebug, const QSizeF &);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSIZE_H

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

#ifndef QPAIR_H
#define QPAIR_H

#include <QtCore/qdatastream.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


template <class T1, class T2>
struct QPair
{
    using first_type = T1;
    using second_type = T2;

    constexpr QPair() : first(), second() {}
    constexpr QPair(const T1 &t1, const T2 &t2) : first(t1), second(t2) {}
    // compiler-generated copy/move ctor/assignment operators are fine!

    template <typename TT1, typename TT2>
    constexpr QPair(const QPair<TT1, TT2> &p) : first(p.first), second(p.second) {}
    template <typename TT1, typename TT2>
    QPair &operator=(const QPair<TT1, TT2> &p)
    { first = p.first; second = p.second; return *this; }
#ifdef Q_COMPILER_RVALUE_REFS
    template <typename TT1, typename TT2>
    constexpr QPair(QPair<TT1, TT2> &&p) : first(std::move(p.first)), second(std::move(p.second)) {}
    template <typename TT1, typename TT2>
    QPair &operator=(QPair<TT1, TT2> &&p)
    { first = std::move(p.first); second = std::move(p.second); return *this; }
#endif

    T1 first;
    T2 second;
};

// mark QPair<T1,T2> as complex/movable/primitive depending on the
// typeinfos of the constituents:
template<class T1, class T2>
class QTypeInfo<QPair<T1, T2> > : public QTypeInfoMerger<QPair<T1, T2>, T1, T2> {}; // Q_DECLARE_TYPEINFO

template <class T1, class T2>
constexpr Q_INLINE_TEMPLATE bool operator==(const QPair<T1, T2> &p1, const QPair<T1, T2> &p2)
{ return p1.first == p2.first && p1.second == p2.second; }

template <class T1, class T2>
constexpr Q_INLINE_TEMPLATE bool operator!=(const QPair<T1, T2> &p1, const QPair<T1, T2> &p2)
{ return !(p1 == p2); }

template <class T1, class T2>
constexpr Q_INLINE_TEMPLATE bool operator<(const QPair<T1, T2> &p1, const QPair<T1, T2> &p2)
{
    return p1.first < p2.first || (!(p2.first < p1.first) && p1.second < p2.second);
}

template <class T1, class T2>
constexpr Q_INLINE_TEMPLATE bool operator>(const QPair<T1, T2> &p1, const QPair<T1, T2> &p2)
{
    return p2 < p1;
}

template <class T1, class T2>
constexpr Q_INLINE_TEMPLATE bool operator<=(const QPair<T1, T2> &p1, const QPair<T1, T2> &p2)
{
    return !(p2 < p1);
}

template <class T1, class T2>
constexpr Q_INLINE_TEMPLATE bool operator>=(const QPair<T1, T2> &p1, const QPair<T1, T2> &p2)
{
    return !(p1 < p2);
}

template <class T1, class T2>
constexpr Q_OUTOFLINE_TEMPLATE QPair<T1, T2> qMakePair(const T1 &x, const T2 &y)
{
    return QPair<T1, T2>(x, y);
}

#ifndef QT_NO_DATASTREAM
template <class T1, class T2>
inline QDataStream& operator>>(QDataStream& s, QPair<T1, T2>& p)
{
    s >> p.first >> p.second;
    return s;
}

template <class T1, class T2>
inline QDataStream& operator<<(QDataStream& s, const QPair<T1, T2>& p)
{
    s << p.first << p.second;
    return s;
}
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QPAIR_H

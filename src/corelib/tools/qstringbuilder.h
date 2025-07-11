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

#ifndef QSTRINGBUILDER_H
#define QSTRINGBUILDER_H

#if 0
// syncqt can not handle the templates in this file, and it doesn't need to
// process them anyway because they are internal.
#pragma qt_class(QStringBuilder)
#pragma qt_sync_stop_processing
#endif

#include <QtCore/qstring.h>
#include <QtCore/qbytearray.h>

#include <string.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


struct Q_CORE_EXPORT QAbstractConcatenable
{
protected:
    static void convertFromAscii(const char *a, int len, QChar *&out);
    static inline void convertFromAscii(char a, QChar *&out)
    {
            *out++ = QLatin1Char(a);
    }
};

template <typename T> struct QConcatenable {};

namespace QtStringBuilder {
    template <typename A, typename B> struct ConvertToTypeHelper
    { typedef A ConvertTo; };
    template <typename T> struct ConvertToTypeHelper<T, QString>
    { typedef QString ConvertTo; };
}

template<typename Builder, typename T>
struct QStringBuilderCommon
{
    T toUpper() const { return resolved().toUpper(); }
    T toLower() const { return resolved().toLower(); }

protected:
    const T resolved() const { return *static_cast<const Builder*>(this); }
};

template<typename Builder, typename T>
struct QStringBuilderBase : public QStringBuilderCommon<Builder, T>
{
};

template<typename Builder>
struct QStringBuilderBase<Builder, QString> : public QStringBuilderCommon<Builder, QString>
{
    QByteArray toLatin1() const { return this->resolved().toLatin1(); }
    QByteArray toLocal8Bit() const { return this->resolved().toLocal8Bit(); }
};

template <typename A, typename B>
class QStringBuilder : public QStringBuilderBase<QStringBuilder<A, B>, typename QtStringBuilder::ConvertToTypeHelper<typename QConcatenable<A>::ConvertTo, typename QConcatenable<B>::ConvertTo>::ConvertTo>
{
public:
    QStringBuilder(const A &a_, const B &b_) : a(a_), b(b_) {}
private:
    friend class QByteArray;
    friend class QString;
    template <typename T> T convertTo() const
    {
        const uint len = QConcatenable< QStringBuilder<A, B> >::size(*this);
        T s(len, Qt::Uninitialized);

        typename T::iterator d = s.data();
        typename T::const_iterator const start = d;
        QConcatenable< QStringBuilder<A, B> >::appendTo(*this, d);

        if (!QConcatenable< QStringBuilder<A, B> >::ExactSize && int(len) != d - start) {
            // this resize is necessary since we allocate a bit too much
            // when dealing with variable sized 8-bit encodings
            s.resize(d - start);
        }
        return s;
    }

    typedef QConcatenable<QStringBuilder<A, B> > Concatenable;
    typedef typename Concatenable::ConvertTo ConvertTo;
public:
    operator ConvertTo() const { return convertTo<ConvertTo>(); }

    int size() const { return Concatenable::size(*this); }

    const A &a;
    const B &b;
};

template <>
class QStringBuilder <QString, QString> : public QStringBuilderBase<QStringBuilder<QString, QString>, QString>
{
    public:
        QStringBuilder(const QString &a_, const QString &b_) : a(a_), b(b_) {}

        operator QString() const
        { QString r(a); r += b; return r; }

        const QString &a;
        const QString &b;
};

template <>
class QStringBuilder <QByteArray, QByteArray> : public QStringBuilderBase<QStringBuilder<QByteArray, QByteArray>, QByteArray>
{
    public:
        QStringBuilder(const QByteArray &a_, const QByteArray &b_) : a(a_), b(b_) {}

        operator QByteArray() const
        { QByteArray r(a); r += b; return r; }

        const QByteArray &a;
        const QByteArray &b;
};


template <> struct QConcatenable<char> : private QAbstractConcatenable
{
    typedef char type;
    typedef QByteArray ConvertTo;
    enum { ExactSize = true };
    static int size(const char) { return 1; }
#ifndef QT_NO_CAST_FROM_ASCII
    QT_ASCII_CAST_WARN static inline void appendTo(const char c, QChar *&out)
    {
        QAbstractConcatenable::convertFromAscii(c, out);
    }
#endif
    static inline void appendTo(const char c, char *&out)
    { *out++ = c; }
};

template <> struct QConcatenable<QLatin1Char>
{
    typedef QLatin1Char type;
    typedef QString ConvertTo;
    enum { ExactSize = true };
    static int size(const QLatin1Char) { return 1; }
    static inline void appendTo(const QLatin1Char c, QChar *&out)
    { *out++ = c; }
    static inline void appendTo(const QLatin1Char c, char *&out)
    { *out++ = c.toLatin1(); }
};

template <> struct QConcatenable<QChar> : private QAbstractConcatenable
{
    typedef QChar type;
    typedef QString ConvertTo;
    enum { ExactSize = true };
    static int size(const QChar) { return 1; }
    static inline void appendTo(const QChar c, QChar *&out)
    { *out++ = c; }
};

template <> struct QConcatenable<QChar::SpecialCharacter> : private QAbstractConcatenable
{
    typedef QChar::SpecialCharacter type;
    typedef QString ConvertTo;
    enum { ExactSize = true };
    static int size(const QChar::SpecialCharacter) { return 1; }
    static inline void appendTo(const QChar::SpecialCharacter c, QChar *&out)
    { *out++ = c; }
};

template <> struct QConcatenable<QCharRef> : private QAbstractConcatenable
{
    typedef QCharRef type;
    typedef QString ConvertTo;
    enum { ExactSize = true };
    static int size(const QCharRef &) { return 1; }
    static inline void appendTo(const QCharRef &c, QChar *&out)
    { *out++ = QChar(c); }
};

template <> struct QConcatenable<QLatin1String>
{
    typedef QLatin1String type;
    typedef QString ConvertTo;
    enum { ExactSize = true };
    static int size(QLatin1String a) { return a.size(); }
    static inline void appendTo(QLatin1String a, QChar *&out)
    {
        for (const char *s = a.data(); *s; )
            *out++ = QLatin1Char(*s++);
    }
    static inline void appendTo(QLatin1String a, char *&out)
    {
        for (const char *s = a.data(); *s; )
            *out++ = *s++;
    }
};

template <> struct QConcatenable<QString> : private QAbstractConcatenable
{
    typedef QString type;
    typedef QString ConvertTo;
    enum { ExactSize = true };
    static int size(const QString &a) { return a.size(); }
    static inline void appendTo(const QString &a, QChar *&out)
    {
        const int n = a.size();
        memcpy(out, reinterpret_cast<const char*>(a.constData()), sizeof(QChar) * n);
        out += n;
    }
};

template <> struct QConcatenable<QStringRef> : private QAbstractConcatenable
{
    typedef QStringRef type;
    typedef QString ConvertTo;
    enum { ExactSize = true };
    static int size(const QStringRef &a) { return a.size(); }
    static inline void appendTo(const QStringRef &a, QChar *&out)
    {
        const int n = a.size();
        memcpy(out, reinterpret_cast<const char*>(a.constData()), sizeof(QChar) * n);
        out += n;
    }

};

template <int N> struct QConcatenable<char[N]> : private QAbstractConcatenable
{
    typedef char type[N];
    typedef QByteArray ConvertTo;
    enum { ExactSize = false };
    static int size(const char[N]) { return N - 1; }
#ifndef QT_NO_CAST_FROM_ASCII
    QT_ASCII_CAST_WARN static inline void appendTo(const char a[N], QChar *&out)
    {
        QAbstractConcatenable::convertFromAscii(a, N, out);
    }
#endif
    static inline void appendTo(const char a[N], char *&out)
    {
        while (*a)
            *out++ = *a++;
    }
};

template <int N> struct QConcatenable<const char[N]> : private QAbstractConcatenable
{
    typedef const char type[N];
    typedef QByteArray ConvertTo;
    enum { ExactSize = false };
    static int size(const char[N]) { return N - 1; }
#ifndef QT_NO_CAST_FROM_ASCII
    QT_ASCII_CAST_WARN static inline void appendTo(const char a[N], QChar *&out)
    {
        QAbstractConcatenable::convertFromAscii(a, N, out);
    }
#endif
    static inline void appendTo(const char a[N], char *&out)
    {
        while (*a)
            *out++ = *a++;
    }
};

template <> struct QConcatenable<const char *> : private QAbstractConcatenable
{
    typedef char const *type;
    typedef QByteArray ConvertTo;
    enum { ExactSize = false };
    static int size(const char *a) { return qstrlen(a); }
#ifndef QT_NO_CAST_FROM_ASCII
    QT_ASCII_CAST_WARN static inline void appendTo(const char *a, QChar *&out)
    { QAbstractConcatenable::convertFromAscii(a, -1, out); }
#endif
    static inline void appendTo(const char *a, char *&out)
    {
        if (!a)
            return;
        while (*a)
            *out++ = *a++;
    }
};

template <> struct QConcatenable<QByteArray> : private QAbstractConcatenable
{
    typedef QByteArray type;
    typedef QByteArray ConvertTo;
    enum { ExactSize = false };
    static int size(const QByteArray &ba) { return ba.size(); }
#ifndef QT_NO_CAST_FROM_ASCII
    static inline void appendTo(const QByteArray &ba, QChar *&out)
    {
        // adding 1 because convertFromAscii expects the size including the null-termination
        QAbstractConcatenable::convertFromAscii(ba.constData(), ba.size() + 1, out);
    }
#endif
    static inline void appendTo(const QByteArray &ba, char *&out)
    {
        const char *a = ba.constData();
        const char * const end = ba.end();
        while (a != end)
            *out++ = *a++;
    }
};

template <typename A, typename B>
struct QConcatenable< QStringBuilder<A, B> >
{
    typedef QStringBuilder<A, B> type;
    typedef typename QtStringBuilder::ConvertToTypeHelper<typename QConcatenable<A>::ConvertTo, typename QConcatenable<B>::ConvertTo>::ConvertTo ConvertTo;
    enum { ExactSize = QConcatenable<A>::ExactSize && QConcatenable<B>::ExactSize };
    static int size(const type &p)
    {
        return QConcatenable<A>::size(p.a) + QConcatenable<B>::size(p.b);
    }
    template<typename T> static inline void appendTo(const type &p, T *&out)
    {
        QConcatenable<A>::appendTo(p.a, out);
        QConcatenable<B>::appendTo(p.b, out);
    }
};

template <typename A, typename B>
QStringBuilder<typename QConcatenable<A>::type, typename QConcatenable<B>::type>
operator%(const A &a, const B &b)
{
   return QStringBuilder<typename QConcatenable<A>::type, typename QConcatenable<B>::type>(a, b);
}

// QT_USE_FAST_OPERATOR_PLUS was introduced in 4.7, QT_USE_QSTRINGBUILDER is to be used from 4.8 onwards
// QT_USE_FAST_OPERATOR_PLUS does not remove the normal operator+ for QByteArray
#if defined(QT_USE_FAST_OPERATOR_PLUS) || defined(QT_USE_QSTRINGBUILDER)
template <typename A, typename B>
QStringBuilder<typename QConcatenable<A>::type, typename QConcatenable<B>::type>
operator+(const A &a, const B &b)
{
   return QStringBuilder<typename QConcatenable<A>::type, typename QConcatenable<B>::type>(a, b);
}
#endif

template <typename A, typename B>
QByteArray &operator+=(QByteArray &a, const QStringBuilder<A, B> &b)
{
#ifndef QT_NO_CAST_TO_ASCII
    if (sizeof(typename QConcatenable< QStringBuilder<A, B> >::ConvertTo::value_type) == sizeof(QChar)) {
        //it is not save to optimize as in utf8 it is not possible to compute the size
        return a += QString(b);
    }
#endif
    int len = a.size() + QConcatenable< QStringBuilder<A, B> >::size(b);
    a.reserve(len);
    char *it = a.data() + a.size();
    QConcatenable< QStringBuilder<A, B> >::appendTo(b, it);
    a.resize(len); //we need to resize after the appendTo for the case str+=foo+str
    return a;
}

template <typename A, typename B>
QString &operator+=(QString &a, const QStringBuilder<A, B> &b)
{
    int len = a.size() + QConcatenable< QStringBuilder<A, B> >::size(b);
    a.reserve(len);
    QChar *it = a.data() + a.size();
    QConcatenable< QStringBuilder<A, B> >::appendTo(b, it);
    a.resize(it - a.constData()); //may be smaller than len if there was conversion from utf8
    return a;
}


QT_END_NAMESPACE

QT_END_HEADER

#endif // QSTRINGBUILDER_H

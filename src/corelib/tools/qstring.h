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

#ifndef QSTRING_H
#define QSTRING_H

#include <QtCore/qchar.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qrefcount.h>
#include <QtCore/qnamespace.h>
#ifdef QT_INCLUDE_COMPAT
#include <Qt3Support/q3cstring.h>
#endif

#include <string>

#include <stdarg.h>

#ifdef truncate
#error qstring.h must be included before any header file that defines truncate
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QCharRef;
class QRegExp;
class QString;
class QStringList;
class QTextCodec;
class QStringRef;
template <typename T> class QVector;

class QLatin1String
{
public:
    constexpr inline explicit QLatin1String(const char *s) : m_size(s ? strlen(s) : 0), m_data(s) {}
    constexpr inline explicit QLatin1String(const char *s, int sz) : m_size(sz), m_data(s) {}

    inline const char *latin1() const { return m_data; }
    constexpr inline int size() const { return m_size; }
    constexpr inline const char *data() const { return m_data; }

    constexpr bool isNull() const noexcept { return !data(); }
    constexpr bool isEmpty() const noexcept { return !size(); }

    [[nodiscard]] constexpr QLatin1Char at(int i) const { return QLatin1Char(m_data[i]); }
    [[nodiscard]] constexpr QLatin1Char operator[](int i) const { return at(i); }

    inline bool operator==(const QString &s) const;
    inline bool operator!=(const QString &s) const;
    inline bool operator>(const QString &s) const;
    inline bool operator<(const QString &s) const;
    inline bool operator>=(const QString &s) const;
    inline bool operator<=(const QString &s) const;

    QT_ASCII_CAST_WARN inline bool operator==(const char *s) const;
    QT_ASCII_CAST_WARN inline bool operator!=(const char *s) const;
    QT_ASCII_CAST_WARN inline bool operator<(const char *s) const;
    QT_ASCII_CAST_WARN inline bool operator>(const char *s) const;
    QT_ASCII_CAST_WARN inline bool operator<=(const char *s) const;
    QT_ASCII_CAST_WARN inline bool operator>=(const char *s) const;

    QT_ASCII_CAST_WARN inline bool operator==(const QByteArray &s) const;
    QT_ASCII_CAST_WARN inline bool operator!=(const QByteArray &s) const;
    QT_ASCII_CAST_WARN inline bool operator<(const QByteArray &s) const;
    QT_ASCII_CAST_WARN inline bool operator>(const QByteArray &s) const;
    QT_ASCII_CAST_WARN inline bool operator<=(const QByteArray &s) const;
    QT_ASCII_CAST_WARN inline bool operator>=(const QByteArray &s) const;

private:
    int m_size;
    const char *m_data;
};

// Qt 4.x compatibility
typedef QLatin1String QLatin1Literal;


typedef QTypedArrayData<ushort> QStringData;

#if defined(Q_COMPILER_UNICODE_STRINGS)

#define QT_UNICODE_LITERAL_II(str) u"" str
typedef char16_t qunicodechar;

#elif defined(Q_OS_WIN) || (defined(__SIZEOF_WCHAR_T__) && __SIZEOF_WCHAR_T__ == 2) || defined(WCHAR_MAX) && (WCHAR_MAX - 0 < 65536)
// wchar_t is 2 bytes


#if defined(Q_CC_MSVC)
#    define QT_UNICODE_LITERAL_II(str) L##str
#else
#    define QT_UNICODE_LITERAL_II(str) L"" str
#endif
typedef wchar_t qunicodechar;

#else

#define QT_NO_UNICODE_LITERAL
typedef ushort qunicodechar;

#endif

static_assert(sizeof(qunicodechar) == 2,
        "qunicodechar must typedef an integral type of size 2");

#ifndef QT_NO_UNICODE_LITERAL
#  define QT_UNICODE_LITERAL(str) QT_UNICODE_LITERAL_II(str)
# if defined(Q_COMPILER_LAMBDA)

#  define QStringLiteral(str) \
    ([]() -> QString { \
        enum { Size = sizeof(QT_UNICODE_LITERAL(str))/2 - 1 }; \
        static const QStaticStringData<Size> qstring_literal = { \
            Q_STATIC_STRING_DATA_HEADER_INITIALIZER(Size), \
            QT_UNICODE_LITERAL(str) }; \
        QStringDataPtr holder = { qstring_literal.data_ptr() }; \
        const QString s(holder); \
        return s; \
    }()) \
    /**/

# endif
#endif // QT_NO_UNICODE_LITERAL

#define Q_STATIC_STRING_DATA_HEADER_INITIALIZER_WITH_OFFSET(size, offset) \
    { Q_REFCOUNT_INITIALIZE_STATIC, size, 0, 0, offset } \
    /**/

#define Q_STATIC_STRING_DATA_HEADER_INITIALIZER(size) \
    Q_STATIC_STRING_DATA_HEADER_INITIALIZER_WITH_OFFSET(size, sizeof(QStringData)) \
    /**/

#ifndef QStringLiteral
// no lambdas, not GCC, or GCC in C++98 mode with 4-byte wchar_t
// fallback, return a temporary QString
// source code is assumed to be encoded in UTF-8
# define QStringLiteral(str) QString::fromUtf8(str, sizeof(str) - 1)
#endif

#ifndef QT_NO_KEYWORDS
#define qs(str) QStringLiteral(str)
#endif

template<int N>
struct QStaticStringData
{
    QArrayData str;
    qunicodechar data[N + 1];

    QStringData *data_ptr() const
    {
        Q_ASSERT(str.ref.isStatic());
        return const_cast<QStringData *>(static_cast<const QStringData*>(&str));
    }
};


struct QStringDataPtr
{
    QStringData *ptr;
};


class Q_CORE_EXPORT QString
{
public:
    typedef QStringData Data;

    inline QString();
    QString(const QChar *unicode, int size); // Qt5: don't cap size < 0
    explicit QString(const QChar *unicode); // Qt5: merge with the above
    QString(QChar c);
    QString(int size, QChar c);
    inline QString(QLatin1String latin1);
    inline QString(const QString &);
    inline ~QString();
    QString &operator=(QChar c);
    QString &operator=(const QString &);
    inline QString &operator=(QLatin1String latin1);
#ifdef Q_COMPILER_RVALUE_REFS
    inline QString(QString && other) : d(other.d) { other.d = Data::sharedNull(); }
    inline QString &operator=(QString &&other)
    { qSwap(d, other.d); return *this; }
#endif
    inline void swap(QString &other) { qSwap(d, other.d); }
    inline int size() const { return d->size; }
    inline int count() const { return d->size; }
    inline int length() const;
    inline bool isEmpty() const;
    void resize(int size);

    QString &fill(QChar c, int size = -1);
    void truncate(int pos);
    void chop(int n);

    int capacity() const;
    inline void reserve(int size);
    inline void squeeze();

    inline const QChar *unicode() const;
    inline QChar *data();
    inline const QChar *data() const;
    inline const QChar *constData() const;

    inline void detach();
    inline bool isDetached() const;
    inline bool isSharedWith(const QString &other) const { return d == other.d; }
    void clear();

    inline const QChar at(int i) const;
    const QChar operator[](int i) const;
    [[nodiscard]] QCharRef operator[](int i);
    const QChar operator[](uint i) const;
    [[nodiscard]] QCharRef operator[](uint i);

    Q_REQUIRED_RESULT QString arg(qlonglong a, int fieldwidth=0, int base=10,
                QChar fillChar = QLatin1Char(' ')) const;
    Q_REQUIRED_RESULT QString arg(qulonglong a, int fieldwidth=0, int base=10,
                QChar fillChar = QLatin1Char(' ')) const;
    Q_REQUIRED_RESULT QString arg(long a, int fieldwidth=0, int base=10,
                QChar fillChar = QLatin1Char(' ')) const;
    Q_REQUIRED_RESULT QString arg(ulong a, int fieldwidth=0, int base=10,
                QChar fillChar = QLatin1Char(' ')) const;
    Q_REQUIRED_RESULT QString arg(int a, int fieldWidth = 0, int base = 10,
                QChar fillChar = QLatin1Char(' ')) const;
    Q_REQUIRED_RESULT QString arg(uint a, int fieldWidth = 0, int base = 10,
                QChar fillChar = QLatin1Char(' ')) const;
    Q_REQUIRED_RESULT QString arg(short a, int fieldWidth = 0, int base = 10,
                QChar fillChar = QLatin1Char(' ')) const;
    Q_REQUIRED_RESULT QString arg(ushort a, int fieldWidth = 0, int base = 10,
                QChar fillChar = QLatin1Char(' ')) const;
    Q_REQUIRED_RESULT QString arg(double a, int fieldWidth = 0, char fmt = 'g', int prec = -1,
                QChar fillChar = QLatin1Char(' ')) const;
    Q_REQUIRED_RESULT QString arg(char a, int fieldWidth = 0,
                QChar fillChar = QLatin1Char(' ')) const;
    Q_REQUIRED_RESULT QString arg(QChar a, int fieldWidth = 0,
                QChar fillChar = QLatin1Char(' ')) const;
    Q_REQUIRED_RESULT QString arg(const QString &a, int fieldWidth = 0,
                QChar fillChar = QLatin1Char(' ')) const;
    Q_REQUIRED_RESULT QString arg(const QString &a1, const QString &a2) const;
    Q_REQUIRED_RESULT QString arg(const QString &a1, const QString &a2, const QString &a3) const;
    Q_REQUIRED_RESULT QString arg(const QString &a1, const QString &a2, const QString &a3,
                const QString &a4) const;
    Q_REQUIRED_RESULT QString arg(const QString &a1, const QString &a2, const QString &a3,
                const QString &a4, const QString &a5) const;
    Q_REQUIRED_RESULT QString arg(const QString &a1, const QString &a2, const QString &a3,
                const QString &a4, const QString &a5, const QString &a6) const;
    Q_REQUIRED_RESULT QString arg(const QString &a1, const QString &a2, const QString &a3,
                const QString &a4, const QString &a5, const QString &a6,
                const QString &a7) const;
    Q_REQUIRED_RESULT QString arg(const QString &a1, const QString &a2, const QString &a3,
                const QString &a4, const QString &a5, const QString &a6,
                const QString &a7, const QString &a8) const;
    Q_REQUIRED_RESULT QString arg(const QString &a1, const QString &a2, const QString &a3,
                const QString &a4, const QString &a5, const QString &a6,
                const QString &a7, const QString &a8, const QString &a9) const;

    static QString vasprintf(const char *format, va_list ap) Q_ATTRIBUTE_FORMAT_PRINTF(1, 0);
    static QString asprintf(const char *format, ...) Q_ATTRIBUTE_FORMAT_PRINTF(1, 2);

    int indexOf(QChar c, int from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int indexOf(const QString &s, int from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int indexOf(QLatin1String s, int from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int indexOf(const QStringRef &s, int from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int lastIndexOf(QChar c, int from = -1, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int lastIndexOf(const QString &s, int from = -1, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int lastIndexOf(QLatin1String s, int from = -1, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int lastIndexOf(const QStringRef &s, int from = -1, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

    inline bool contains(QChar c, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    inline bool contains(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    inline bool contains(const QStringRef &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int count(QChar c, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int count(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int count(const QStringRef &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

#ifndef QT_NO_REGEXP
    int indexOf(const QRegExp &, int from = 0) const;
    int lastIndexOf(const QRegExp &, int from = -1) const;
    inline bool contains(const QRegExp &rx) const { return indexOf(rx) != -1; }
    int count(const QRegExp &) const;

    int indexOf(QRegExp &, int from = 0) const;
    int lastIndexOf(QRegExp &, int from = -1) const;
    inline bool contains(QRegExp &rx) const { return indexOf(rx) != -1; }
#endif

    enum SectionFlag {
        SectionDefault             = 0x00,
        SectionSkipEmpty           = 0x01,
        SectionIncludeLeadingSep   = 0x02,
        SectionIncludeTrailingSep  = 0x04,
        SectionCaseInsensitiveSeps = 0x08
    };
    Q_DECLARE_FLAGS(SectionFlags, SectionFlag)

    QString section(QChar sep, int start, int end = -1, SectionFlags flags = SectionDefault) const;
    QString section(const QString &in_sep, int start, int end = -1, SectionFlags flags = SectionDefault) const;
#ifndef QT_NO_REGEXP
    QString section(const QRegExp &reg, int start, int end = -1, SectionFlags flags = SectionDefault) const;
#endif

    Q_REQUIRED_RESULT QString left(int n) const;
    Q_REQUIRED_RESULT QString right(int n) const;
    Q_REQUIRED_RESULT QString mid(int position, int n = -1) const;
    Q_REQUIRED_RESULT QStringRef leftRef(int n) const;
    Q_REQUIRED_RESULT QStringRef rightRef(int n) const;
    Q_REQUIRED_RESULT QStringRef midRef(int position, int n = -1) const;

    bool startsWith(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool startsWith(const QStringRef &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool startsWith(QLatin1String s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool startsWith(QChar c, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool endsWith(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool endsWith(const QStringRef &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool endsWith(QLatin1String s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool endsWith(QChar c, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

    Q_REQUIRED_RESULT QString leftJustified(int width, QChar fill = QLatin1Char(' '), bool trunc = false) const;
    Q_REQUIRED_RESULT QString rightJustified(int width, QChar fill = QLatin1Char(' '), bool trunc = false) const;

    Q_REQUIRED_RESULT QString toLower() const;
    Q_REQUIRED_RESULT QString toUpper() const;
    Q_REQUIRED_RESULT QString toCaseFolded() const;

    Q_REQUIRED_RESULT QString trimmed() const;
    Q_REQUIRED_RESULT QString simplified() const;
    Q_REQUIRED_RESULT QString toHtmlEscaped() const;

    QString &insert(int i, QChar c);
    QString &insert(int i, const QChar *uc, int len);
    inline QString &insert(int i, const QString &s) { return insert(i, s.constData(), s.length()); }
    QString &insert(int i, QLatin1String s);
    QString &append(QChar c);
    QString &append(const QChar *uc, int len);
    QString &append(const QString &s);
    QString &append(const QStringRef &s);
    QString &append(QLatin1String s);
    inline QString &prepend(QChar c) { return insert(0, c); }
    inline QString &prepend(const QString &s) { return insert(0, s); }
    inline QString &prepend(QLatin1String s) { return insert(0, s); }

    inline QString &operator+=(QChar c) {
        if (d->ref.isShared() || uint(d->size) + 2u > d->alloc)
            reallocData(uint(d->size) + 2u, true);
        d->data()[d->size++] = c.unicode();
        d->data()[d->size] = '\0';
        return *this;
    }

    inline QString &operator+=(QChar::SpecialCharacter c) { return append(QChar(c)); }
    inline QString &operator+=(const QString &s) { return append(s); }
    inline QString &operator+=(const QStringRef &s) { return append(s); }
    inline QString &operator+=(QLatin1String s) { return append(s); }

    QString &remove(int i, int len);
    QString &remove(QChar c, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &remove(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &replace(int i, int len, QChar after);
    QString &replace(int i, int len, const QChar *s, int slen);
    QString &replace(int i, int len, const QString &after);
    QString &replace(QChar before, QChar after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &replace(const QChar *before, int blen, const QChar *after, int alen, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &replace(QLatin1String before, QLatin1String after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &replace(QLatin1String before, const QString &after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &replace(const QString &before, QLatin1String after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &replace(const QString &before, const QString &after,
                     Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &replace(QChar c, const QString &after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
    QString &replace(QChar c, QLatin1String after, Qt::CaseSensitivity cs = Qt::CaseSensitive);
#ifndef QT_NO_REGEXP
    QString &replace(const QRegExp &rx, const QString &after);
    inline QString &remove(const QRegExp &rx)
    { return replace(rx, QString()); }
#endif

    enum SplitBehavior { KeepEmptyParts, SkipEmptyParts };

    Q_REQUIRED_RESULT QStringList split(const QString &sep, SplitBehavior behavior = KeepEmptyParts,
                      Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    Q_REQUIRED_RESULT QStringList split(QChar sep, SplitBehavior behavior = KeepEmptyParts,
                      Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
#ifndef QT_NO_REGEXP
    Q_REQUIRED_RESULT QStringList split(const QRegExp &sep, SplitBehavior behavior = KeepEmptyParts) const;
#endif

    enum NormalizationForm {
        NormalizationForm_D,
        NormalizationForm_C,
        NormalizationForm_KD,
        NormalizationForm_KC
    };
    Q_REQUIRED_RESULT QString normalized(NormalizationForm mode) const;
    Q_REQUIRED_RESULT QString normalized(NormalizationForm mode, QChar::UnicodeVersion version) const;

    QString repeated(int times) const;

    const ushort *utf16() const;

    Q_REQUIRED_RESULT QByteArray toAscii() const;
    Q_REQUIRED_RESULT QByteArray toLatin1() const;
    Q_REQUIRED_RESULT QByteArray toUtf8() const;
    Q_REQUIRED_RESULT QByteArray toLocal8Bit() const;
    Q_REQUIRED_RESULT QVector<uint> toUcs4() const;

    static QString fromAscii(const char *, int size = -1);
    static QString fromLatin1(const char *, int size = -1);
    static QString fromUtf8(const char *, int size = -1);
    static QString fromLocal8Bit(const char *, int size = -1);

    static inline QString fromAscii(const QByteArray &str)
    { return str.isNull() ? QString() : fromAscii(str.data(), qstrnlen(str.constData(), str.size())); }
    static inline QString fromLatin1(const QByteArray &str)
    { return str.isNull() ? QString() : fromLatin1(str.data(), qstrnlen(str.constData(), str.size())); }
    static inline QString fromUtf8(const QByteArray &str)
    { return str.isNull() ? QString() : fromUtf8(str.data(), qstrnlen(str.constData(), str.size())); }
    static inline QString fromLocal8Bit(const QByteArray &str)
    { return str.isNull() ? QString() : fromLocal8Bit(str.data(), qstrnlen(str.constData(), str.size())); }
    static QString fromUtf16(const ushort *, int size = -1);
    static QString fromUcs4(const uint *, int size = -1);
    static QString fromRawData(const QChar *, int size);

    int toWCharArray(wchar_t *array) const;
    static QString fromWCharArray(const wchar_t *, int size = -1);

    QString &setRawData(const QChar *unicode, int size);
    QString &setUnicode(const QChar *unicode, int size);
    inline QString &setUtf16(const ushort *utf16, int size);

    // ### Qt 5: merge these two functions
    int compare(const QString &s) const;
    int compare(const QString &s, Qt::CaseSensitivity cs) const;

    int compare(QLatin1String other, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

    // ### Qt 5: merge these two functions
    static inline int compare(const QString &s1, const QString &s2)
    { return s1.compare(s2); }
    static inline int compare(const QString &s1, const QString &s2, Qt::CaseSensitivity cs)
    { return s1.compare(s2, cs); }

    static inline int compare(const QString &s1, QLatin1String s2,
                              Qt::CaseSensitivity cs = Qt::CaseSensitive)
    { return s1.compare(s2, cs); }
    static inline int compare(QLatin1String s1, const QString &s2,
                              Qt::CaseSensitivity cs = Qt::CaseSensitive)
    { return -s2.compare(s1, cs); }

    int compare(const QStringRef &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    static int compare(const QString &s1, const QStringRef &s2,
                       Qt::CaseSensitivity = Qt::CaseSensitive);

    int localeAwareCompare(const QString& s) const;
    static int localeAwareCompare(const QString& s1, const QString& s2)
    { return s1.localeAwareCompare(s2); }

    int localeAwareCompare(const QStringRef &s) const;
    static int localeAwareCompare(const QString& s1, const QStringRef& s2);

    // ### TODO: make inline except for the long long versions
    short  toShort(bool *ok=nullptr, int base=10) const;
    ushort toUShort(bool *ok=nullptr, int base=10) const;
    int toInt(bool *ok=nullptr, int base=10) const;
    uint toUInt(bool *ok=nullptr, int base=10) const;
    long toLong(bool *ok=nullptr, int base=10) const;
    ulong toULong(bool *ok=nullptr, int base=10) const;
    qlonglong toLongLong(bool *ok=nullptr, int base=10) const;
    qulonglong toULongLong(bool *ok=nullptr, int base=10) const;
    float toFloat(bool *ok=nullptr) const;
    double toDouble(bool *ok=nullptr) const;

    QString &setNum(short, int base=10);
    QString &setNum(ushort, int base=10);
    QString &setNum(int, int base=10);
    QString &setNum(uint, int base=10);
    QString &setNum(long, int base=10);
    QString &setNum(ulong, int base=10);
    QString &setNum(qlonglong, int base=10);
    QString &setNum(qulonglong, int base=10);
    QString &setNum(float, char f='g', int prec=6);
    QString &setNum(double, char f='g', int prec=6);

    static QString number(int, int base=10);
    static QString number(uint, int base=10);
    static QString number(long, int base=10);
    static QString number(ulong, int base=10);
    static QString number(qlonglong, int base=10);
    static QString number(qulonglong, int base=10);
    static QString number(double, char f='g', int prec=6);

    bool operator==(const QString &s) const;
    bool operator<(const QString &s) const;
    inline bool operator>(const QString &s) const { return s < *this; }
    inline bool operator!=(const QString &s) const { return !operator==(s); }
    inline bool operator<=(const QString &s) const { return !operator>(s); }
    inline bool operator>=(const QString &s) const { return !operator<(s); }

    bool operator==(QLatin1String s) const;
    bool operator<(QLatin1String s) const;
    bool operator>(QLatin1String s) const;
    inline bool operator!=(QLatin1String s) const { return !operator==(s); }
    inline bool operator<=(QLatin1String s) const { return !operator>(s); }
    inline bool operator>=(QLatin1String s) const { return !operator<(s); }

    // ASCII compatibility
#ifndef QT_NO_CAST_FROM_ASCII
    QT_ASCII_CAST_WARN_CONSTRUCTOR inline QString(const char *ch) : d(fromAscii_helper(ch))
    {}
    QT_ASCII_CAST_WARN_CONSTRUCTOR inline QString(const QByteArray &a)
        : d(fromAscii_helper(a.constData(), qstrnlen(a.constData(), a.size())))
    {}
    QT_ASCII_CAST_WARN inline QString &operator=(const char *ch)
    { return (*this = fromAscii(ch)); }
    QT_ASCII_CAST_WARN inline QString &operator=(const QByteArray &a)
    { return (*this = fromAscii(a.constData(), qstrnlen(a.constData(), a.size()))); }
    QT_ASCII_CAST_WARN inline QString &operator=(char c)
    { return (*this = QChar::fromAscii(c)); }

    // these are needed, so it compiles with STL support enabled
    QT_ASCII_CAST_WARN inline QString &prepend(const char *s)
    { return prepend(QString::fromAscii(s)); }
    QT_ASCII_CAST_WARN inline QString &prepend(const QByteArray &s)
    { return prepend(QString::fromAscii(s.constData(), qstrnlen(s.constData(), s.size()))); }
    QT_ASCII_CAST_WARN inline QString &append(const char *s)
    { return append(QString::fromAscii(s)); }
    QT_ASCII_CAST_WARN inline QString &append(const QByteArray &s)
    { return append(QString::fromAscii(s.constData(), qstrnlen(s.constData(), s.size()))); }
    QT_ASCII_CAST_WARN inline QString &operator+=(const char *s)
    { return append(QString::fromAscii(s)); }
    QT_ASCII_CAST_WARN inline QString &operator+=(const QByteArray &s)
    { return append(QString::fromAscii(s.constData(), qstrnlen(s.constData(), s.size()))); }
    QT_ASCII_CAST_WARN inline QString &operator+=(char c)
    { return append(QChar::fromAscii(c)); }

    QT_ASCII_CAST_WARN inline bool operator==(const char *s) const;
    QT_ASCII_CAST_WARN inline bool operator!=(const char *s) const;
    QT_ASCII_CAST_WARN inline bool operator<(const char *s) const;
    QT_ASCII_CAST_WARN inline bool operator<=(const char *s2) const;
    QT_ASCII_CAST_WARN inline bool operator>(const char *s2) const;
    QT_ASCII_CAST_WARN inline bool operator>=(const char *s2) const;

    QT_ASCII_CAST_WARN inline bool operator==(const QByteArray &s) const;
    QT_ASCII_CAST_WARN inline bool operator!=(const QByteArray &s) const;
    QT_ASCII_CAST_WARN inline bool operator<(const QByteArray &s) const
    { return *this < QString::fromAscii(s); }
    QT_ASCII_CAST_WARN inline bool operator>(const QByteArray &s) const
    { return *this > QString::fromAscii(s); }
    QT_ASCII_CAST_WARN inline bool operator<=(const QByteArray &s) const
    { return *this <= QString::fromAscii(s); }
    QT_ASCII_CAST_WARN inline bool operator>=(const QByteArray &s) const
    { return *this >= QString::fromAscii(s); }
#endif

    typedef QChar *iterator;
    typedef const QChar *const_iterator;
    typedef iterator Iterator;
    typedef const_iterator ConstIterator;
    iterator begin();
    const_iterator begin() const;
    const_iterator constBegin() const;
    iterator end();
    const_iterator end() const;
    const_iterator constEnd() const;

    // STL compatibility
    typedef const QChar & const_reference;
    typedef QChar & reference;
    typedef QChar value_type;
    inline void push_back(QChar c) { append(c); }
    inline void push_back(const QString &s) { append(s); }
    inline void push_front(QChar c) { prepend(c); }
    inline void push_front(const QString &s) { prepend(s); }

    static inline QString fromStdString(const std::string &s);
    inline std::string toStdString() const;

    static inline QString fromStdWString(const std::wstring &s);
    inline std::wstring toStdWString() const;

    // compatibility
    struct Null { };
    static const Null null;
    inline QString(const Null &): d(Data::sharedNull()) {}
    inline QString &operator=(const Null &) { *this = QString(); return *this; }
    inline bool isNull() const { return d == Data::sharedNull(); }

    bool isSimpleText() const;
    bool isRightToLeft() const;

    QString(int size, Qt::Initialization);
    Q_DECL_CONSTEXPR inline QString(QStringDataPtr dd) : d(dd.ptr) {}

private:
#if defined(QT_NO_CAST_FROM_ASCII) && !defined(Q_NO_DECLARED_NOT_DEFINED)
    QString &operator+=(const char *s);
    QString &operator+=(const QByteArray &s);
    QString(const char *ch);
    QString(const QByteArray &a);
    QString &operator=(const char  *ch);
    QString &operator=(const QByteArray &a);
#endif

    Data *d;

    void reallocData(uint alloc, bool grow = false);
    void expand(int i);
    void updateProperties() const;
    QString multiArg(int numArgs, const QString **args) const;
    static int compare_helper(const QChar *data1, int length1,
                              const QChar *data2, int length2,
                              Qt::CaseSensitivity cs = Qt::CaseSensitive);
    static int compare_helper(const QChar *data1, int length1,
                              QLatin1String s2,
                              Qt::CaseSensitivity cs = Qt::CaseSensitive);
    static int localeAwareCompare_helper(const QChar *data1, int length1,
                                         const QChar *data2, int length2);
    static Data *fromLatin1_helper(const char *str, int size = -1);
    static Data *fromAscii_helper(const char *str, int size = -1);
    static qlonglong toIntegral_helper(const QChar *data, int len, bool *ok, int base);
    static qulonglong toIntegral_helper(const QChar *data, uint len, bool *ok, int base);
    void replace_helper(uint *indices, int nIndices, int blen, const QChar *after, int alen);
    friend class QCharRef;
    friend class QCFString;
    friend class QTextCodec;
    friend class QStringRef;
    friend class QCollator;
    friend struct QAbstractConcatenable;
    friend inline bool qStringComparisonHelper(const QString &s1, const char *s2);
    friend inline bool qStringComparisonHelper(const QStringRef &s1, const char *s2);


    template <typename T> static
    T toIntegral_helper(const QChar *data, int len, bool *ok, int base)
    {
        typedef typename std::conditional<std::is_unsigned<T>::value, qulonglong, qlonglong>::type Int64;
        typedef typename std::conditional<std::is_unsigned<T>::value, uint, int>::type Int32;

        // we select the right overload by casting size() to int or uint
        Int64 val = toIntegral_helper(data, Int32(len), ok, base);
        if (T(val) != val) {
            if (ok)
                *ok = false;
            val = 0;
        }
        return T(val);
    }

public:
    typedef Data * DataPtr;
    inline DataPtr &data_ptr() { return d; }
};


inline QString::QString(QLatin1String aLatin1) : d(fromLatin1_helper(aLatin1.latin1()))
{ }
inline int QString::length() const
{ return d->size; }
inline const QChar QString::at(int i) const
{ Q_ASSERT(uint(i) < uint(size())); return d->data()[i]; }
inline const QChar QString::operator[](int i) const
{ Q_ASSERT(uint(i) < uint(size())); return d->data()[i]; }
inline const QChar QString::operator[](uint i) const
{ Q_ASSERT(i < uint(size())); return d->data()[i]; }
inline bool QString::isEmpty() const
{ return d->size == 0; }
inline const QChar *QString::unicode() const
{ return reinterpret_cast<const QChar*>(d->data()); }
inline const QChar *QString::data() const
{ return reinterpret_cast<const QChar*>(d->data()); }
inline QChar *QString::data()
{ detach(); return reinterpret_cast<QChar*>(d->data()); }
inline const QChar *QString::constData() const
{ return reinterpret_cast<const QChar*>(d->data()); }
inline void QString::detach()
{ if (d->ref.isShared() || (d->offset != sizeof(QStringData))) reallocData(uint(d->size) + 1u); }
inline bool QString::isDetached() const
{ return !d->ref.isShared(); }
inline QString &QString::operator=(QLatin1String s)
{
    *this = fromLatin1(s.latin1());
    return *this;
}
inline void QString::clear()
{ if (!isNull()) *this = QString(); }
inline QString::QString(const QString &other) : d(other.d)
{ Q_ASSERT(&other != this); d->ref.ref(); }
inline int QString::capacity() const
{ return d->alloc ? d->alloc - 1 : 0; }
inline QString &QString::setNum(short n, int base)
{ return setNum(qlonglong(n), base); }
inline QString &QString::setNum(ushort n, int base)
{ return setNum(qulonglong(n), base); }
inline QString &QString::setNum(int n, int base)
{ return setNum(qlonglong(n), base); }
inline QString &QString::setNum(uint n, int base)
{ return setNum(qulonglong(n), base); }
inline QString &QString::setNum(long n, int base)
{ return setNum(qlonglong(n), base); }
inline QString &QString::setNum(ulong n, int base)
{ return setNum(qulonglong(n), base); }
inline QString &QString::setNum(float n, char f, int prec)
{ return setNum(double(n),f,prec); }
inline QString QString::arg(int a, int fieldWidth, int base, QChar fillChar) const
{ return arg(qlonglong(a), fieldWidth, base, fillChar); }
inline QString QString::arg(uint a, int fieldWidth, int base, QChar fillChar) const
{ return arg(qulonglong(a), fieldWidth, base, fillChar); }
inline QString QString::arg(long a, int fieldWidth, int base, QChar fillChar) const
{ return arg(qlonglong(a), fieldWidth, base, fillChar); }
inline QString QString::arg(ulong a, int fieldWidth, int base, QChar fillChar) const
{ return arg(qulonglong(a), fieldWidth, base, fillChar); }
inline QString QString::arg(short a, int fieldWidth, int base, QChar fillChar) const
{ return arg(qlonglong(a), fieldWidth, base, fillChar); }
inline QString QString::arg(ushort a, int fieldWidth, int base, QChar fillChar) const
{ return arg(qulonglong(a), fieldWidth, base, fillChar); }
inline QString QString::arg(const QString &a1, const QString &a2) const
{ const QString *args[2] = { &a1, &a2 }; return multiArg(2, args); }
inline QString QString::arg(const QString &a1, const QString &a2, const QString &a3) const
{ const QString *args[3] = { &a1, &a2, &a3 }; return multiArg(3, args); }
inline QString QString::arg(const QString &a1, const QString &a2, const QString &a3,
                            const QString &a4) const
{ const QString *args[4] = { &a1, &a2, &a3, &a4 }; return multiArg(4, args); }
inline QString QString::arg(const QString &a1, const QString &a2, const QString &a3,
                            const QString &a4, const QString &a5) const
{ const QString *args[5] = { &a1, &a2, &a3, &a4, &a5 }; return multiArg(5, args); }
inline QString QString::arg(const QString &a1, const QString &a2, const QString &a3,
                            const QString &a4, const QString &a5, const QString &a6) const
{ const QString *args[6] = { &a1, &a2, &a3, &a4, &a5, &a6 }; return multiArg(6, args); }
inline QString QString::arg(const QString &a1, const QString &a2, const QString &a3,
                            const QString &a4, const QString &a5, const QString &a6,
                            const QString &a7) const
{ const QString *args[7] = { &a1, &a2, &a3, &a4, &a5, &a6,  &a7 }; return multiArg(7, args); }
inline QString QString::arg(const QString &a1, const QString &a2, const QString &a3,
                            const QString &a4, const QString &a5, const QString &a6,
                            const QString &a7, const QString &a8) const
{ const QString *args[8] = { &a1, &a2, &a3, &a4, &a5, &a6,  &a7, &a8 }; return multiArg(8, args); }
inline QString QString::arg(const QString &a1, const QString &a2, const QString &a3,
                            const QString &a4, const QString &a5, const QString &a6,
                            const QString &a7, const QString &a8, const QString &a9) const
{ const QString *args[9] = { &a1, &a2, &a3, &a4, &a5, &a6,  &a7, &a8, &a9 }; return multiArg(9, args); }

inline QString QString::section(QChar asep, int astart, int aend, SectionFlags aflags) const
{ return section(QString(asep), astart, aend, aflags); }


class Q_CORE_EXPORT QCharRef {
    QString &s;
    int i;
    inline QCharRef(QString &str, int idx)
        : s(str),i(idx) {}
    friend class QString;
public:

    // most QChar operations repeated here

    // all this is not documented: We just say "like QChar" and let it be.
    inline operator QChar() const
    { return i < s.d->size ? s.d->data()[i] : 0; }
    inline QCharRef &operator=(QChar c)
    { if (i >= s.d->size) s.expand(i); else s.detach();
      s.d->data()[i] = c.unicode(); return *this; }

    // An operator= for each QChar cast constructors
#ifndef QT_NO_CAST_FROM_ASCII
    QT_ASCII_CAST_WARN inline QCharRef &operator=(char c)
    { return operator=(QChar::fromAscii(c)); }
    QT_ASCII_CAST_WARN inline QCharRef &operator=(uchar c)
    { return operator=(QChar::fromAscii(c)); }
#endif
    inline QCharRef &operator=(const QCharRef &c) { return operator=(QChar(c)); }
    inline QCharRef &operator=(ushort rc) { return operator=(QChar(rc)); }
    inline QCharRef &operator=(short rc) { return operator=(QChar(rc)); }
    inline QCharRef &operator=(uint rc) { return operator=(QChar(rc)); }
    inline QCharRef &operator=(int rc) { return operator=(QChar(rc)); }

    // each function...
    inline bool isNull() const { return QChar(*this).isNull(); }
    inline bool isPrint() const { return QChar(*this).isPrint(); }
    inline bool isPunct() const { return QChar(*this).isPunct(); }
    inline bool isSpace() const { return QChar(*this).isSpace(); }
    inline bool isMark() const { return QChar(*this).isMark(); }
    inline bool isLetter() const { return QChar(*this).isLetter(); }
    inline bool isNumber() const { return QChar(*this).isNumber(); }
    inline bool isLetterOrNumber() { return QChar(*this).isLetterOrNumber(); }
    inline bool isDigit() const { return QChar(*this).isDigit(); }
    inline bool isLower() const { return QChar(*this).isLower(); }
    inline bool isUpper() const { return QChar(*this).isUpper(); }
    inline bool isTitleCase() const { return QChar(*this).isTitleCase(); }

    inline int digitValue() const { return QChar(*this).digitValue(); }
    QChar toLower() const { return QChar(*this).toLower(); }
    QChar toUpper() const { return QChar(*this).toUpper(); }
    QChar toTitleCase () const { return QChar(*this).toTitleCase(); }

    QChar::Category category() const { return QChar(*this).category(); }
    QChar::Direction direction() const { return QChar(*this).direction(); }
    QChar::Joining joining() const { return QChar(*this).joining(); }
    bool hasMirrored() const { return QChar(*this).hasMirrored(); }
    QChar mirroredChar() const { return QChar(*this).mirroredChar(); }
    QString decomposition() const { return QChar(*this).decomposition(); }
    QChar::Decomposition decompositionTag() const { return QChar(*this).decompositionTag(); }
    uchar combiningClass() const { return QChar(*this).combiningClass(); }

    QChar::UnicodeVersion unicodeVersion() const { return QChar(*this).unicodeVersion(); }

    inline uchar cell() const { return QChar(*this).cell(); }
    inline uchar row() const { return QChar(*this).row(); }
    inline void setCell(uchar cell);
    inline void setRow(uchar row);

    char toAscii() const { return QChar(*this).toAscii(); }
    char toLatin1() const { return QChar(*this).toLatin1(); }
    ushort unicode() const { return QChar(*this).unicode(); }
    ushort& unicode() { return s.data()[i].unicode(); }
};

inline void QCharRef::setRow(uchar arow) { QChar(*this).setRow(arow); }
inline void QCharRef::setCell(uchar acell) { QChar(*this).setCell(acell); }


inline QString::QString() : d(Data::sharedNull()) {}
inline QString::~QString() { if (!d->ref.deref()) Data::deallocate(d); }
inline void QString::reserve(int asize)
{
    if (d->ref.isShared() || uint(asize) >= d->alloc)
        reallocData(std::max(asize, d->size) + 1u);

    if (!d->capacityReserved) {
        // cannot set unconditionally, since d could be the shared_null/shared_empty (which is const)
        d->capacityReserved = true;
    }
}

inline void QString::squeeze()
{
    if (d->ref.isShared() || uint(d->size) + 1u < d->alloc)
        reallocData(uint(d->size) + 1u);

    if (d->capacityReserved) {
        // cannot set unconditionally, since d could be shared_null or
        // otherwise static.
        d->capacityReserved = false;
    }
}
inline QString &QString::setUtf16(const ushort *autf16, int asize)
{ return setUnicode(reinterpret_cast<const QChar *>(autf16), asize); }
inline QCharRef QString::operator[](int i)
{ Q_ASSERT(i >= 0); return QCharRef(*this, i); }
inline QCharRef QString::operator[](uint i)
{ return QCharRef(*this, i); }
inline QString::iterator QString::begin()
{ detach(); return reinterpret_cast<QChar*>(d->data()); }
inline QString::const_iterator QString::begin() const
{ return reinterpret_cast<const QChar*>(d->data()); }
inline QString::const_iterator QString::constBegin() const
{ return reinterpret_cast<const QChar*>(d->data()); }
inline QString::iterator QString::end()
{ detach(); return reinterpret_cast<QChar*>(d->data() + d->size); }
inline QString::const_iterator QString::end() const
{ return reinterpret_cast<const QChar*>(d->data() + d->size); }
inline QString::const_iterator QString::constEnd() const
{ return reinterpret_cast<const QChar*>(d->data() + d->size); }
inline bool QString::contains(const QString &s, Qt::CaseSensitivity cs) const
{ return indexOf(s, 0, cs) != -1; }
inline bool QString::contains(const QStringRef &s, Qt::CaseSensitivity cs) const
{ return indexOf(s, 0, cs) != -1; }
inline bool QString::contains(QChar c, Qt::CaseSensitivity cs) const
{ return indexOf(c, 0, cs) != -1; }


inline bool operator==(QString::Null, QString::Null) { return true; }
inline bool operator==(QString::Null, const QString &s) { return s.isNull(); }
inline bool operator==(const QString &s, QString::Null) { return s.isNull(); }
inline bool operator!=(QString::Null, QString::Null) { return false; }
inline bool operator!=(QString::Null, const QString &s) { return !s.isNull(); }
inline bool operator!=(const QString &s, QString::Null) { return !s.isNull(); }

inline bool QLatin1String::operator==(const QString &s) const
{ return s == *this; }
inline bool QLatin1String::operator!=(const QString &s) const
{ return s != *this; }
inline bool QLatin1String::operator>(const QString &s) const
{ return s < *this; }
inline bool QLatin1String::operator<(const QString &s) const
{ return s > *this; }
inline bool QLatin1String::operator>=(const QString &s) const
{ return s <= *this; }
inline bool QLatin1String::operator<=(const QString &s) const
{ return s >= *this; }

#ifndef QT_NO_CAST_FROM_ASCII
inline bool qStringComparisonHelper(const QString &s1, const char *s2)
{
    return (s1 == QLatin1String(s2));
}
inline bool QString::operator==(const char *s) const
{ return qStringComparisonHelper(*this, s); }
inline bool QString::operator!=(const char *s) const
{ return !qStringComparisonHelper(*this, s); }
inline bool QString::operator<(const char *s) const
{ return *this < QString::fromAscii(s); }
inline bool QString::operator>(const char *s) const
{ return *this > QString::fromAscii(s); }
inline bool QString::operator<=(const char *s) const
{ return *this <= QString::fromAscii(s); }
inline bool QString::operator>=(const char *s) const
{ return *this >= QString::fromAscii(s); }

QT_ASCII_CAST_WARN inline bool operator==(const char *s1, const QString &s2)
{ return qStringComparisonHelper(s2, s1); }
QT_ASCII_CAST_WARN inline bool operator!=(const char *s1, const QString &s2)
{ return !qStringComparisonHelper(s2, s1); }
QT_ASCII_CAST_WARN inline bool operator<(const char *s1, const QString &s2)
{ return (QString::fromAscii(s1) < s2); }
QT_ASCII_CAST_WARN inline bool operator>(const char *s1, const QString &s2)
{ return (QString::fromAscii(s1) > s2); }
QT_ASCII_CAST_WARN inline bool operator<=(const char *s1, const QString &s2)
{ return (QString::fromAscii(s1) <= s2); }
QT_ASCII_CAST_WARN inline bool operator>=(const char *s1, const QString &s2)
{ return (QString::fromAscii(s1) >= s2); }

QT_ASCII_CAST_WARN inline bool operator==(const char *s1, QLatin1String s2)
{ return QString::fromAscii(s1) == s2; }
QT_ASCII_CAST_WARN inline bool operator!=(const char *s1, QLatin1String s2)
{ return QString::fromAscii(s1) != s2; }
QT_ASCII_CAST_WARN inline bool operator<(const char *s1, QLatin1String s2)
{ return (QString::fromAscii(s1) < s2); }
QT_ASCII_CAST_WARN inline bool operator>(const char *s1, QLatin1String s2)
{ return (QString::fromAscii(s1) > s2); }
QT_ASCII_CAST_WARN inline bool operator<=(const char *s1, QLatin1String s2)
{ return (QString::fromAscii(s1) <= s2); }
QT_ASCII_CAST_WARN inline bool operator>=(const char *s1, QLatin1String s2)
{ return (QString::fromAscii(s1) >= s2); }

inline bool operator==(QLatin1String s1, QLatin1String s2)
{ return (qstrcmp(s1.latin1(), s2.latin1()) == 0); }
inline bool operator!=(QLatin1String s1, QLatin1String s2)
{ return (qstrcmp(s1.latin1(), s2.latin1()) != 0); }
inline bool operator<(QLatin1String s1, QLatin1String s2)
{ return (qstrcmp(s1.latin1(), s2.latin1()) < 0); }
inline bool operator<=(QLatin1String s1, QLatin1String s2)
{ return (qstrcmp(s1.latin1(), s2.latin1()) <= 0); }
inline bool operator>(QLatin1String s1, QLatin1String s2)
{ return (qstrcmp(s1.latin1(), s2.latin1()) > 0); }
inline bool operator>=(QLatin1String s1, QLatin1String s2)
{ return (qstrcmp(s1.latin1(), s2.latin1()) >= 0); }
#endif // QT_NO_CAST_FROM_ASCII

QT_ASCII_CAST_WARN inline bool QLatin1String::operator==(const char *s) const
{ return QString::fromUtf8(s) == *this; }
QT_ASCII_CAST_WARN inline bool QLatin1String::operator!=(const char *s) const
{ return QString::fromUtf8(s) != *this; }
QT_ASCII_CAST_WARN inline bool QLatin1String::operator<(const char *s) const
{ return QString::fromUtf8(s) > *this; }
QT_ASCII_CAST_WARN inline bool QLatin1String::operator>(const char *s) const
{ return QString::fromUtf8(s) < *this; }
QT_ASCII_CAST_WARN inline bool QLatin1String::operator<=(const char *s) const
{ return QString::fromUtf8(s) >= *this; }
QT_ASCII_CAST_WARN inline bool QLatin1String::operator>=(const char *s) const
{ return QString::fromUtf8(s) <= *this; }

QT_ASCII_CAST_WARN inline bool QLatin1String::operator==(const QByteArray &s) const
{ return QString::fromUtf8(s) == *this; }
QT_ASCII_CAST_WARN inline bool QLatin1String::operator!=(const QByteArray &s) const
{ return QString::fromUtf8(s) != *this; }
QT_ASCII_CAST_WARN inline bool QLatin1String::operator<(const QByteArray &s) const
{ return QString::fromUtf8(s) > *this; }
QT_ASCII_CAST_WARN inline bool QLatin1String::operator>(const QByteArray &s) const
{ return QString::fromUtf8(s) < *this; }
QT_ASCII_CAST_WARN inline bool QLatin1String::operator<=(const QByteArray &s) const
{ return QString::fromUtf8(s) >= *this; }
QT_ASCII_CAST_WARN inline bool QLatin1String::operator>=(const QByteArray &s) const
{ return QString::fromUtf8(s) <= *this; }

#ifndef QT_NO_CAST_FROM_ASCII
inline bool QString::operator==(const QByteArray &s) const
{ return qStringComparisonHelper(*this, s.constData()); }
inline bool QString::operator!=(const QByteArray &s) const
{ return !qStringComparisonHelper(*this, s.constData()); }

inline bool QByteArray::operator==(const QString &s) const
{ return qStringComparisonHelper(s, constData()); }
inline bool QByteArray::operator!=(const QString &s) const
{ return !qStringComparisonHelper(s, constData()); }
inline bool QByteArray::operator<(const QString &s) const
{ return QString::fromAscii(constData(), size()) < s; }
inline bool QByteArray::operator>(const QString &s) const
{ return QString::fromAscii(constData(), size()) > s; }
inline bool QByteArray::operator<=(const QString &s) const
{ return QString::fromAscii(constData(), size()) <= s; }
inline bool QByteArray::operator>=(const QString &s) const
{ return QString::fromAscii(constData(), size()) >= s; }
#endif   // QT_NO_CAST_FROM_ASCII

#ifndef QT_NO_CAST_TO_ASCII
inline QByteArray &QByteArray::append(const QString &s)
{ return append(s.toAscii()); }
inline QByteArray &QByteArray::insert(int i, const QString &s)
{ return insert(i, s.toAscii()); }
inline QByteArray &QByteArray::replace(char c, const QString &after)
{ return replace(c, after.toAscii()); }
inline QByteArray &QByteArray::replace(const QString &before, const char *after)
{ return replace(before.toAscii(), after); }
inline QByteArray &QByteArray::replace(const QString &before, const QByteArray &after)
{ return replace(before.toAscii(), after); }
inline QByteArray &QByteArray::operator+=(const QString &s)
{ return operator+=(s.toAscii()); }
inline int QByteArray::indexOf(const QString &s, int from) const
{ return indexOf(s.toAscii(), from); }
inline int QByteArray::lastIndexOf(const QString &s, int from) const
{ return lastIndexOf(s.toAscii(), from); }
#endif // QT_NO_CAST_TO_ASCII

#if !defined(QT_USE_FAST_OPERATOR_PLUS) && !defined(QT_USE_QSTRINGBUILDER)
inline const QString operator+(const QString &s1, const QString &s2)
{ QString t(s1); t += s2; return t; }
inline const QString operator+(const QString &s1, QChar s2)
{ QString t(s1); t += s2; return t; }
inline const QString operator+(QChar s1, const QString &s2)
{ QString t(s1); t += s2; return t; }
#  ifndef QT_NO_CAST_FROM_ASCII
QT_ASCII_CAST_WARN inline const QString operator+(const QString &s1, const char *s2)
{ QString t(s1); t += QString::fromAscii(s2); return t; }
QT_ASCII_CAST_WARN inline const QString operator+(const char *s1, const QString &s2)
{ QString t = QString::fromAscii(s1); t += s2; return t; }
QT_ASCII_CAST_WARN inline const QString operator+(char c, const QString &s)
{ QString t = s; t.prepend(QChar::fromAscii(c)); return t; }
QT_ASCII_CAST_WARN inline const QString operator+(const QString &s, char c)
{ QString t = s; t += QChar::fromAscii(c); return t; }
QT_ASCII_CAST_WARN inline const QString operator+(const QByteArray &ba, const QString &s)
{ QString t = QString::fromAscii(ba.constData(), qstrnlen(ba.constData(), ba.size())); t += s; return t; }
QT_ASCII_CAST_WARN inline const QString operator+(const QString &s, const QByteArray &ba)
{ QString t(s); t += QString::fromAscii(ba.constData(), qstrnlen(ba.constData(), ba.size())); return t; }
#  endif // QT_NO_CAST_FROM_ASCII
#endif // QT_USE_QSTRINGBUILDER

inline std::string QString::toStdString() const
{ const QByteArray asc = toAscii(); return std::string(asc.constData(), asc.length()); }

inline QString QString::fromStdString(const std::string &s)
{ return fromAscii(s.data(), int(s.size())); }

inline std::wstring QString::toStdWString() const
{
    std::wstring str;
    str.resize(length());

#if defined(_MSC_VER) && _MSC_VER >= 1400
    // VS2005 crashes if the string is empty
    if (!length())
        return str;
#endif

    str.resize(toWCharArray(&(*str.begin())));
    return str;
}

inline QString QString::fromStdWString(const std::wstring &s)
{ return fromWCharArray(s.data(), int(s.size())); }

#if !defined(QT_NO_DATASTREAM) || (defined(QT_BOOTSTRAPPED) && !defined(QT_BUILD_QMAKE))
Q_CORE_EXPORT QDataStream &operator<<(QDataStream &, const QString &);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &, QString &);
#endif

Q_DECLARE_SHARED(QString)
Q_DECLARE_OPERATORS_FOR_FLAGS(QString::SectionFlags)


class Q_CORE_EXPORT QStringRef {
    const QString *m_string;
    int m_position;
    int m_size;
public:
    inline QStringRef():m_string(nullptr), m_position(0), m_size(0){}
    inline QStringRef(const QString *string, int position, int size);
    inline QStringRef(const QString *string);
    inline QStringRef(const QStringRef &other)
         
        = default;

    inline ~QStringRef()= default;
    inline const QString *string() const { return m_string; }
    inline int position() const { return m_position; }
    inline int size() const { return m_size; }
    inline int count() const { return m_size; }
    inline int length() const { return m_size; }

    inline QStringRef &operator=(const QStringRef &other) = default;

    int indexOf(const QString &str, int from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int indexOf(QChar ch, int from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int indexOf(QLatin1String str, int from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int indexOf(const QStringRef &str, int from = 0, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int lastIndexOf(const QString &str, int from = -1, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int lastIndexOf(QChar ch, int from = -1, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int lastIndexOf(QLatin1String str, int from = -1, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int lastIndexOf(const QStringRef &str, int from = -1, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

    inline bool contains(const QString &str, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    inline bool contains(QChar ch, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    inline bool contains(QLatin1String str, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    inline bool contains(const QStringRef &str, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

    int count(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int count(QChar c, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int count(const QStringRef &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

    bool startsWith(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool startsWith(QLatin1String s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool startsWith(QChar c, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool startsWith(const QStringRef &c, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

    bool endsWith(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool endsWith(QLatin1String s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool endsWith(QChar c, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    bool endsWith(const QStringRef &c, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

    inline QStringRef &operator=(const QString *string);

    inline const QChar *unicode() const {
        if (!m_string)
            return reinterpret_cast<const QChar *>(QString::Data::sharedNull()->data());
        return m_string->unicode() + m_position;
    }
    inline const QChar *data() const { return unicode(); }
    inline const QChar *constData() const {  return unicode(); }

    Q_REQUIRED_RESULT QByteArray toAscii() const;
    Q_REQUIRED_RESULT QByteArray toLatin1() const;
    Q_REQUIRED_RESULT QByteArray toUtf8() const;
    Q_REQUIRED_RESULT QByteArray toLocal8Bit() const;
    Q_REQUIRED_RESULT QVector<uint> toUcs4() const;

    inline void clear() { m_string = nullptr; m_position = m_size = 0; }
    QString toString() const;
    inline bool isEmpty() const { return m_size == 0; }
    inline bool isNull() const { return m_string == nullptr || m_string->isNull(); }

    QStringRef appendTo(QString *string) const;

    inline const QChar at(int i) const
        { Q_ASSERT(uint(i) < uint(size())); return m_string->at(i + m_position); }

    int compare(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int compare(const QStringRef &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    int compare(QLatin1String s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
    static int compare(const QStringRef &s1, const QString &s2,
                       Qt::CaseSensitivity = Qt::CaseSensitive);
    static int compare(const QStringRef &s1, const QStringRef &s2,
                       Qt::CaseSensitivity = Qt::CaseSensitive);
    static int compare(const QStringRef &s1, QLatin1String s2,
                       Qt::CaseSensitivity cs = Qt::CaseSensitive);

    int localeAwareCompare(const QString &s) const;
    int localeAwareCompare(const QStringRef &s) const;
    static int localeAwareCompare(const QStringRef &s1, const QString &s2);
    static int localeAwareCompare(const QStringRef &s1, const QStringRef &s2);

    Q_REQUIRED_RESULT QStringRef trimmed() const;
    short  toShort(bool *ok = nullptr, int base = 10) const;
    ushort toUShort(bool *ok = nullptr, int base = 10) const;
    int toInt(bool *ok = nullptr, int base = 10) const;
    uint toUInt(bool *ok = nullptr, int base = 10) const;
    long toLong(bool *ok = nullptr, int base = 10) const;
    ulong toULong(bool *ok = nullptr, int base = 10) const;
    qlonglong toLongLong(bool *ok = nullptr, int base = 10) const;
    qulonglong toULongLong(bool *ok = nullptr, int base = 10) const;
    float toFloat(bool *ok = nullptr) const;
    double toDouble(bool *ok = nullptr) const;
};

inline QStringRef &QStringRef::operator=(const QString *aString)
{ m_string = aString; m_position = 0; m_size = aString?aString->size():0; return *this; }

inline QStringRef::QStringRef(const QString *aString, int aPosition, int aSize)
        :m_string(aString), m_position(aPosition), m_size(aSize){}

inline QStringRef::QStringRef(const QString *aString)
    :m_string(aString), m_position(0), m_size(aString?aString->size() : 0){}

Q_CORE_EXPORT bool operator==(const QStringRef &s1,const QStringRef &s2);
inline bool operator!=(const QStringRef &s1,const QStringRef &s2)
{ return !(s1 == s2); }
Q_CORE_EXPORT bool operator==(const QString &s1,const QStringRef &s2);
inline bool operator!=(const QString &s1,const QStringRef &s2)
{ return !(s1 == s2); }
inline bool operator==(const QStringRef &s1,const QString &s2)
{ return s2 == s1; }
inline bool operator!=(const QStringRef &s1,const QString &s2)
{ return s2 != s1; }
Q_CORE_EXPORT bool operator==(QLatin1String s1, const QStringRef &s2);
inline bool operator!=(QLatin1String s1,const QStringRef &s2)
{ return !(s1 == s2); }
inline bool operator==(const QStringRef &s1, QLatin1String s2)
{ return s2 == s1; }
inline bool operator!=(const QStringRef &s1, QLatin1String s2)
{ return s2 != s1; }

Q_CORE_EXPORT bool operator<(const QStringRef &s1,const QStringRef &s2);
inline bool operator>(const QStringRef &s1, const QStringRef &s2)
{ return s2 < s1; }
inline bool operator<=(const QStringRef &s1, const QStringRef &s2)
{ return !(s1 > s2); }
inline bool operator>=(const QStringRef &s1, const QStringRef &s2)
{ return !(s1 < s2); }

inline bool qStringComparisonHelper(const QStringRef &s1, const char *s2)
{
    return (s1 == QLatin1String(s2));
}

QT_ASCII_CAST_WARN inline bool operator==(const char *s1, const QStringRef &s2)
{ return qStringComparisonHelper(s2, s1); }
QT_ASCII_CAST_WARN inline bool operator==(const QStringRef &s1, const char *s2)
{ return qStringComparisonHelper(s1, s2); }
QT_ASCII_CAST_WARN inline bool operator!=(const char *s1, const QStringRef &s2)
{ return !qStringComparisonHelper(s2, s1); }
QT_ASCII_CAST_WARN inline bool operator!=(const QStringRef &s1, const char *s2)
{ return !qStringComparisonHelper(s1, s2); }

inline int QString::compare(const QStringRef &s, Qt::CaseSensitivity cs) const
{ return QString::compare_helper(constData(), length(), s.constData(), s.length(), cs); }
inline int QString::compare(const QString &s1, const QStringRef &s2, Qt::CaseSensitivity cs)
{ return QString::compare_helper(s1.constData(), s1.length(), s2.constData(), s2.length(), cs); }
inline int QStringRef::compare(const QString &s, Qt::CaseSensitivity cs) const
{ return QString::compare_helper(constData(), length(), s.constData(), s.length(), cs); }
inline int QStringRef::compare(const QStringRef &s, Qt::CaseSensitivity cs) const
{ return QString::compare_helper(constData(), length(), s.constData(), s.length(), cs); }
inline int QStringRef::compare(QLatin1String s, Qt::CaseSensitivity cs) const
{ return QString::compare_helper(constData(), length(), s, cs); }
inline int QStringRef::compare(const QStringRef &s1, const QString &s2, Qt::CaseSensitivity cs)
{ return QString::compare_helper(s1.constData(), s1.length(), s2.constData(), s2.length(), cs); }
inline int QStringRef::compare(const QStringRef &s1, const QStringRef &s2, Qt::CaseSensitivity cs)
{ return QString::compare_helper(s1.constData(), s1.length(), s2.constData(), s2.length(), cs); }
inline int QStringRef::compare(const QStringRef &s1, QLatin1String s2, Qt::CaseSensitivity cs)
{ return QString::compare_helper(s1.constData(), s1.length(), s2, cs); }

inline int QString::localeAwareCompare(const QStringRef &s) const
{ return localeAwareCompare_helper(constData(), length(), s.constData(), s.length()); }
inline int QString::localeAwareCompare(const QString& s1, const QStringRef& s2)
{ return localeAwareCompare_helper(s1.constData(), s1.length(), s2.constData(), s2.length()); }
inline int QStringRef::localeAwareCompare(const QString &s) const
{ return QString::localeAwareCompare_helper(constData(), length(), s.constData(), s.length()); }
inline int QStringRef::localeAwareCompare(const QStringRef &s) const
{ return QString::localeAwareCompare_helper(constData(), length(), s.constData(), s.length()); }
inline int QStringRef::localeAwareCompare(const QStringRef &s1, const QString &s2)
{ return QString::localeAwareCompare_helper(s1.constData(), s1.length(), s2.constData(), s2.length()); }
inline int QStringRef::localeAwareCompare(const QStringRef &s1, const QStringRef &s2)
{ return QString::localeAwareCompare_helper(s1.constData(), s1.length(), s2.constData(), s2.length()); }

inline bool QStringRef::contains(const QString &s, Qt::CaseSensitivity cs) const
{ return indexOf(s, 0, cs) != -1; }
inline bool QStringRef::contains(QLatin1String s, Qt::CaseSensitivity cs) const
{ return indexOf(s, 0, cs) != -1; }
inline bool QStringRef::contains(QChar c, Qt::CaseSensitivity cs) const
{ return indexOf(c, 0, cs) != -1; }
inline bool QStringRef::contains(const QStringRef &s, Qt::CaseSensitivity cs) const
{ return indexOf(s, 0, cs) != -1; }

namespace QtPrivate {
// used by qPrintable() and qUtf8Printable() macros
inline const QString &asString(const QString &s)    { return s; }
inline QString &&asString(QString &&s)              { return std::move(s); }
}

QT_END_NAMESPACE

QT_END_HEADER

#if defined(QT_USE_FAST_OPERATOR_PLUS) || defined(QT_USE_QSTRINGBUILDER)
#include <QtCore/qstringbuilder.h>
#endif

#endif // QSTRING_H

/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qglobal.h>

#ifndef QFLAGS_H
#define QFLAGS_H

#include <QtCore/qtypeinfo.h>
#include <type_traits>

#ifdef Q_COMPILER_INITIALIZER_LISTS
#include <initializer_list>
#endif


QT_BEGIN_NAMESPACE


class QFlag
{
    int i;
public:
    constexpr inline QFlag(int i);
    constexpr inline operator int() const { return i; }
};
Q_DECLARE_TYPEINFO(QFlag, Q_PRIMITIVE_TYPE);

constexpr inline QFlag::QFlag(int ai) : i(ai) {}

class QIncompatibleFlag
{
    int i;
public:
    constexpr inline explicit QIncompatibleFlag(int i);
    constexpr inline operator int() const { return i; }
};
Q_DECLARE_TYPEINFO(QIncompatibleFlag, Q_PRIMITIVE_TYPE);

constexpr inline QIncompatibleFlag::QIncompatibleFlag(int ai) : i(ai) {}


#ifndef Q_NO_TYPESAFE_FLAGS

template<typename Enum>
class QFlags
{
    static_assert((sizeof(Enum) <= sizeof(int)),
                      "QFlags uses an int as storage, so an enum with underlying "
                      "long long will overflow.");
    static_assert((std::is_enum<Enum>::value), "QFlags is only usable on enumeration types.");
    struct Private;
    typedef int (Private::*Zero);
public:
#ifndef qdoc
    typedef typename std::conditional<
            std::is_unsigned<typename std::underlying_type<Enum>::type>::value,
            unsigned int,
            signed int
        >::type Int;
#else
    typedef int Int; // the real typedef above is too complex for qdoc
#endif
    typedef Enum enum_type;

    Q_DECL_CONSTEXPR inline QFlags(const QFlags &f) : i(f.i) {}
    Q_DECL_CONSTEXPR inline QFlags(Enum f) : i(Int(f)) {}
    Q_DECL_CONSTEXPR inline QFlags(Zero = nullptr) : i(0) {}
    constexpr inline QFlags(QFlag f) : i(f) {}

#ifdef Q_COMPILER_INITIALIZER_LISTS
    Q_DECL_CONSTEXPR inline QFlags(std::initializer_list<Enum> flags)
        : i(initializer_list_helper(flags.begin(), flags.end())) {}
#endif

    inline QFlags &operator=(const QFlags &f) { i = f.i; return *this; }
    inline QFlags &operator&=(int mask) { i &= mask; return *this; }
    inline QFlags &operator&=(uint mask) { i &= mask; return *this; }
    inline QFlags &operator&=(Enum mask) { i &= Int(mask); return *this; }
    inline QFlags &operator|=(QFlags f) { i |= f.i; return *this; }
    inline QFlags &operator|=(Enum f) { i |= Int(f); return *this; }
    inline QFlags &operator^=(QFlags f) { i ^= f.i; return *this; }
    inline QFlags &operator^=(Enum f) { i ^= Int(f); return *this; }

    Q_DECL_CONSTEXPR  inline operator Int() const { return i; }

    Q_DECL_CONSTEXPR inline QFlags operator|(QFlags f) const { return QFlags(Enum(i | f.i)); }
    Q_DECL_CONSTEXPR inline QFlags operator|(Enum f) const { return QFlags(Enum(i | Int(f))); }
    Q_DECL_CONSTEXPR inline QFlags operator^(QFlags f) const { return QFlags(Enum(i ^ f.i)); }
    Q_DECL_CONSTEXPR inline QFlags operator^(Enum f) const { return QFlags(Enum(i ^ Int(f))); }
    Q_DECL_CONSTEXPR inline QFlags operator&(int mask) const { return QFlags(Enum(i & mask)); }
    Q_DECL_CONSTEXPR inline QFlags operator&(uint mask) const { return QFlags(Enum(i & mask)); }
    Q_DECL_CONSTEXPR inline QFlags operator&(Enum f) const { return QFlags(Enum(i & Int(f))); }
    Q_DECL_CONSTEXPR inline QFlags operator~() const { return QFlags(Enum(~i)); }

    Q_DECL_CONSTEXPR inline bool operator!() const { return !i; }

    constexpr inline bool testFlag(Enum flag) const noexcept { return testFlags(flag); }
    constexpr inline bool testFlags(QFlags flags) const noexcept { return flags.i ? ((i & flags.i) == flags.i) : i == Int(0); }
    constexpr inline bool testAnyFlag(Enum flag) const noexcept { return testAnyFlags(flag); }
    constexpr inline bool testAnyFlags(QFlags flags) const noexcept { return (i & flags.i) != Int(0); }

    constexpr inline QFlags &setFlag(Enum f, bool on = true) noexcept
    {
        return on ? (*this |= f) : (*this &= ~f);
    }

private:
#ifdef Q_COMPILER_INITIALIZER_LISTS
    Q_DECL_CONSTEXPR static inline Int initializer_list_helper(typename std::initializer_list<Enum>::const_iterator it,
                                                               typename std::initializer_list<Enum>::const_iterator end)
    {
        return (it == end ? Int(0) : (Int(*it) | initializer_list_helper(it + 1, end)));
    }
#endif

    Int i;
};

#define Q_DECLARE_FLAGS(Flags, Enum)\
typedef QFlags<Enum> Flags;

#define Q_DECLARE_INCOMPATIBLE_FLAGS(Flags) \
constexpr inline QIncompatibleFlag operator|(Flags::enum_type f1, int f2) \
{ return QIncompatibleFlag(int(f1) | f2); }

#define Q_DECLARE_OPERATORS_FOR_FLAGS(Flags) \
Q_DECL_CONSTEXPR inline QFlags<Flags::enum_type> operator|(Flags::enum_type f1, Flags::enum_type f2) \
{ return QFlags<Flags::enum_type>(f1) | f2; } \
Q_DECL_CONSTEXPR inline QFlags<Flags::enum_type> operator|(Flags::enum_type f1, QFlags<Flags::enum_type> f2) \
{ return f2 | f1; } Q_DECLARE_INCOMPATIBLE_FLAGS(Flags)


#else /* Q_NO_TYPESAFE_FLAGS */

#define Q_DECLARE_FLAGS(Flags, Enum)\
typedef uint Flags;
#define Q_DECLARE_OPERATORS_FOR_FLAGS(Flags)

#endif /* Q_NO_TYPESAFE_FLAGS */

QT_END_NAMESPACE


#endif // QFLAGS_H

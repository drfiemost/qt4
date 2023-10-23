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

#ifndef QTYPEINFO_H
#define QTYPEINFO_H

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE


/*
   QTypeInfo     - type trait functionality
*/

/*
  The catch-all template.
*/

template <typename T>
class QTypeInfo
{
public:
    enum {
        isPointer = false,
        isIntegral = false,
        isComplex = true,
        isStatic = true,
        isLarge = (sizeof(T)>sizeof(void*)),
        isDummy = false
    };
};

template <typename T>
class QTypeInfo<T*>
{
public:
    enum {
        isPointer = true,
        isIntegral = false,
        isComplex = false,
        isStatic = false,
        isLarge = false,
        isDummy = false
    };
};

/*
   Specialize a specific type with:

     Q_DECLARE_TYPEINFO(type, flags);

   where 'type' is the name of the type to specialize and 'flags' is
   logically-OR'ed combination of the flags below.
*/
enum { /* TYPEINFO flags */
    Q_COMPLEX_TYPE = 0,
    Q_PRIMITIVE_TYPE = 0x1,
    Q_STATIC_TYPE = 0,
    Q_MOVABLE_TYPE = 0x2,
    Q_DUMMY_TYPE = 0x4,
    Q_INTEGRAL_TYPE = 0x8
};

#define Q_DECLARE_TYPEINFO_BODY(TYPE, FLAGS) \
class QTypeInfo<TYPE > \
{ \
public: \
    enum { \
        isComplex = (((FLAGS) & Q_PRIMITIVE_TYPE) == 0), \
        isStatic = (((FLAGS) & (Q_MOVABLE_TYPE | Q_PRIMITIVE_TYPE)) == 0), \
        isLarge = (sizeof(TYPE)>sizeof(void*)), \
        isPointer = false, \
        isIntegral = ((FLAGS) & Q_INTEGRAL_TYPE) != 0, \
        isDummy = (((FLAGS) & Q_DUMMY_TYPE) != 0) \
    }; \
    static inline const char *name() { return #TYPE; } \
}

#define Q_DECLARE_TYPEINFO(TYPE, FLAGS) \
template<> \
Q_DECLARE_TYPEINFO_BODY(TYPE, FLAGS)

/* Specialize QTypeInfo for QFlags<T> */
template<typename T> class QFlags;
template<typename T>
Q_DECLARE_TYPEINFO_BODY(QFlags<T>, Q_PRIMITIVE_TYPE);

/*
   Specialize a shared type with:

     Q_DECLARE_SHARED(type)

   where 'type' is the name of the type to specialize.  NOTE: shared
   types must define a member-swap, and be defined in the same
   namespace as Qt for this to work.
*/
#define Q_DECLARE_SHARED_STL(TYPE) \
QT_END_NAMESPACE \
namespace std { \
    template<> inline void swap<QT_PREPEND_NAMESPACE(TYPE)>(QT_PREPEND_NAMESPACE(TYPE) &value1, QT_PREPEND_NAMESPACE(TYPE) &value2) \
    { value1.swap(value2); } \
} \
QT_BEGIN_NAMESPACE

#define Q_DECLARE_SHARED(TYPE)                                          \
Q_DECLARE_TYPEINFO(TYPE, Q_MOVABLE_TYPE); \
template <> inline void qSwap<TYPE>(TYPE &value1, TYPE &value2) \
{ value1.swap(value2); } \
Q_DECLARE_SHARED_STL(TYPE)

/*
   QTypeInfo primitive specializations
*/
Q_DECLARE_TYPEINFO(bool, Q_PRIMITIVE_TYPE | Q_INTEGRAL_TYPE);
Q_DECLARE_TYPEINFO(char, Q_PRIMITIVE_TYPE | Q_INTEGRAL_TYPE);
Q_DECLARE_TYPEINFO(signed char, Q_PRIMITIVE_TYPE | Q_INTEGRAL_TYPE);
Q_DECLARE_TYPEINFO(uchar, Q_PRIMITIVE_TYPE | Q_INTEGRAL_TYPE);
Q_DECLARE_TYPEINFO(short, Q_PRIMITIVE_TYPE | Q_INTEGRAL_TYPE);
Q_DECLARE_TYPEINFO(ushort, Q_PRIMITIVE_TYPE | Q_INTEGRAL_TYPE);
Q_DECLARE_TYPEINFO(int, Q_PRIMITIVE_TYPE | Q_INTEGRAL_TYPE);
Q_DECLARE_TYPEINFO(uint, Q_PRIMITIVE_TYPE | Q_INTEGRAL_TYPE);
Q_DECLARE_TYPEINFO(long, Q_PRIMITIVE_TYPE | Q_INTEGRAL_TYPE);
Q_DECLARE_TYPEINFO(ulong, Q_PRIMITIVE_TYPE | Q_INTEGRAL_TYPE);
Q_DECLARE_TYPEINFO(qint64, Q_PRIMITIVE_TYPE | Q_INTEGRAL_TYPE);
Q_DECLARE_TYPEINFO(quint64, Q_PRIMITIVE_TYPE | Q_INTEGRAL_TYPE);
Q_DECLARE_TYPEINFO(float, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(double, Q_PRIMITIVE_TYPE);
#ifndef Q_OS_DARWIN
Q_DECLARE_TYPEINFO(long double, Q_PRIMITIVE_TYPE);
#endif
#ifdef Q_COMPILER_UNICODE_STRINGS
// ### Qt6: define as Q_PRIMITIVE_TYPE
// We can't do it now because it would break BC on QList<char32_t>
Q_DECLARE_TYPEINFO(char16_t, Q_INTEGRAL_TYPE);
Q_DECLARE_TYPEINFO(char32_t, Q_INTEGRAL_TYPE);
#endif
#if !defined(Q_CC_MSVC) || defined(_NATIVE_WCHAR_T_DEFINED)
// ### Qt6: same as above
Q_DECLARE_TYPEINFO(wchar_t, Q_INTEGRAL_TYPE);
#endif

QT_END_NAMESPACE
QT_END_HEADER

#endif // QTYPEINFO_H

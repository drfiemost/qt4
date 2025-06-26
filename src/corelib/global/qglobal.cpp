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

#include "qplatformdefs.h"
#include "qstring.h"
#include "qvector.h"
#include "qlist.h"
#include "qthreadstorage.h"
#include "qdir.h"
#include "qstringlist.h"
#include "qdatetime.h"

#include <private/qsystemlibrary_p.h>

#ifndef QT_NO_QOBJECT
#include <private/qthread_p.h>
#endif

#include <climits>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifndef QT_NO_EXCEPTIONS
#  include <string>
#  include <exception>
#endif

#include <cerrno>
#if defined(Q_CC_MSVC)
#  include <crtdbg.h>
#endif

#if defined(Q_OS_MACX) && !defined(Q_OS_IOS)
#include <CoreServices/CoreServices.h>
#endif

#ifdef QT_USE_SLOG2
#include <slog2.h>
#endif

QT_BEGIN_NAMESPACE


/*!
    \class QFlag
    \brief The QFlag class is a helper data type for QFlags.

    It is equivalent to a plain \c int, except with respect to
    function overloading and type conversions. You should never need
    to use this class in your applications.

    \sa QFlags
*/

/*!
    \fn QFlag::QFlag(int value)

    Constructs a QFlag object that stores the given \a value.
*/

/*!
    \fn QFlag::operator int() const

    Returns the value stored by the QFlag object.
*/

/*!
    \class QFlags
    \brief The QFlags class provides a type-safe way of storing
    OR-combinations of enum values.


    \ingroup tools

    The QFlags<Enum> class is a template class, where Enum is an enum
    type. QFlags is used throughout Qt for storing combinations of
    enum values.

    The traditional C++ approach for storing OR-combinations of enum
    values is to use an \c int or \c uint variable. The inconvenience
    with this approach is that there's no type checking at all; any
    enum value can be OR'd with any other enum value and passed on to
    a function that takes an \c int or \c uint.

    Qt uses QFlags to provide type safety. For example, the
    Qt::Alignment type is simply a typedef for
    QFlags<Qt::AlignmentFlag>. QLabel::setAlignment() takes a
    Qt::Alignment parameter, which means that any combination of
    Qt::AlignmentFlag values,or 0, is legal:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 0

    If you try to pass a value from another enum or just a plain
    integer other than 0, the compiler will report an error. If you
    need to cast integer values to flags in a untyped fashion, you can
    use the explicit QFlags constructor as cast operator.

    If you want to use QFlags for your own enum types, use
    the Q_DECLARE_FLAGS() and Q_DECLARE_OPERATORS_FOR_FLAGS().

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 1

    You can then use the \c MyClass::Options type to store
    combinations of \c MyClass::Option values.

    \section1 Flags and the Meta-Object System

    The Q_DECLARE_FLAGS() macro does not expose the flags to the meta-object
    system, so they cannot be used by Qt Script or edited in Qt Designer.
    To make the flags available for these purposes, the Q_FLAGS() macro must
    be used:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp meta-object flags

    \section1 Naming Convention

    A sensible naming convention for enum types and associated QFlags
    types is to give a singular name to the enum type (e.g., \c
    Option) and a plural name to the QFlags type (e.g., \c Options).
    When a singular name is desired for the QFlags type (e.g., \c
    Alignment), you can use \c Flag as the suffix for the enum type
    (e.g., \c AlignmentFlag).

    \sa QFlag
*/

/*!
    \typedef QFlags::Int

    Typedef for the integer type used for storage as well as for
    implicit conversion. Either \c int or \c{unsigned int}, depending
    on whether the enum's underlying type is signed or unsigned.
*/

/*!
    \typedef QFlags::enum_type

    Typedef for the Enum template type.
*/

/*!
    \fn QFlags::QFlags(const QFlags &other)

    Constructs a copy of \a other.
*/

/*!
    \fn QFlags::QFlags(Enum flag)

    Constructs a QFlags object storing the given \a flag.
*/

/*!
    \fn QFlags::QFlags(Zero zero)

    Constructs a QFlags object with no flags set. \a zero must be a
    literal 0 value.
*/

/*!
    \fn QFlags::QFlags(QFlag value)

    Constructs a QFlags object initialized with the given integer \a
    value.

    The QFlag type is a helper type. By using it here instead of \c
    int, we effectively ensure that arbitrary enum values cannot be
    cast to a QFlags, whereas untyped enum values (i.e., \c int
    values) can.
*/

/*!
    \fn QFlags::QFlags(std::initializer_list<Enum> flags)

    Constructs a QFlags object initialized with all \a flags
    combined using the bitwise OR operator.
    \sa operator|=(), operator|()
*/

/*!
    \fn QFlags &QFlags::operator=(const QFlags &other)

    Assigns \a other to this object and returns a reference to this
    object.
*/

/*!
    \fn QFlags &QFlags::operator&=(int mask)

    Performs a bitwise AND operation with \a mask and stores the
    result in this QFlags object. Returns a reference to this object.

    \sa operator&(), operator|=(), operator^=()
*/

/*!
    \fn QFlags &QFlags::operator&=(uint mask)

    \overload
*/

/*!
    \fn QFlags &QFlags::operator&=(Enum mask)
    \overload
*/

/*!
    \fn QFlags &QFlags::operator|=(QFlags other)

    Performs a bitwise OR operation with \a other and stores the
    result in this QFlags object. Returns a reference to this object.

    \sa operator|(), operator&=(), operator^=()
*/

/*!
    \fn QFlags &QFlags::operator|=(Enum other)

    \overload
*/

/*!
    \fn QFlags &QFlags::operator^=(QFlags other)

    Performs a bitwise XOR operation with \a other and stores the
    result in this QFlags object. Returns a reference to this object.

    \sa operator^(), operator&=(), operator|=()
*/

/*!
    \fn QFlags &QFlags::operator^=(Enum other)

    \overload
*/

/*!
    \fn QFlags::operator Int() const

    Returns the value stored in the QFlags object as an integer.

    \sa Int
*/

/*!
    \fn QFlags QFlags::operator|(QFlags other) const

    Returns a QFlags object containing the result of the bitwise OR
    operation on this object and \a other.

    \sa operator|=(), operator^(), operator&(), operator~()
*/

/*!
    \fn QFlags QFlags::operator|(Enum other) const

    \overload
*/

/*!
    \fn QFlags QFlags::operator^(QFlags other) const

    Returns a QFlags object containing the result of the bitwise XOR
    operation on this object and \a other.

    \sa operator^=(), operator&(), operator|(), operator~()
*/

/*!
    \fn QFlags QFlags::operator^(Enum other) const

    \overload
*/

/*!
    \fn QFlags QFlags::operator&(int mask) const

    Returns a QFlags object containing the result of the bitwise AND
    operation on this object and \a mask.

    \sa operator&=(), operator|(), operator^(), operator~()
*/

/*!
    \fn QFlags QFlags::operator&(uint mask) const

    \overload
*/

/*!
    \fn QFlags QFlags::operator&(Enum mask) const

    \overload
*/

/*!
    \fn QFlags QFlags::operator~() const

    Returns a QFlags object that contains the bitwise negation of
    this object.

    \sa operator&(), operator|(), operator^()
*/

/*!
    \fn bool QFlags::operator!() const

    Returns true if no flag is set (i.e., if the value stored by the
    QFlags object is 0); otherwise returns false.
*/

/*!
    \fn bool QFlags::testFlag(Enum flag) const
    \since 4.2

    Returns true if the \a flag is set, otherwise false.
*/

/*!
    \fn QFlags QFlags::setFlag(Enum flag, bool on) const

    Sets the indicated \a flag if \a on is \c true or unsets it if
    it if \a on is \c false. Returns a reference to this object.
*/

/*!
  \macro Q_DISABLE_COPY(Class)
  \relates QObject

  Disables the use of copy constructors and assignment operators
  for the given \a Class.

  Instances of subclasses of QObject should not be thought of as
  values that can be copied or assigned, but as unique identities.
  This means that when you create your own subclass of QObject
  (director or indirect), you should \e not give it a copy constructor
  or an assignment operator.  However, it may not enough to simply
  omit them from your class, because, if you mistakenly write some code
  that requires a copy constructor or an assignment operator (it's easy
  to do), your compiler will thoughtfully create it for you. You must
  do more.

  The curious user will have seen that the Qt classes derived
  from QObject typically include this macro in a private section:

  \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 43

  It declares a copy constructor and an assignment operator in the
  private section, so that if you use them by mistake, the compiler
  will report an error.

  \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 44

  But even this might not catch absolutely every case. You might be
  tempted to do something like this:

  \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 45

  First of all, don't do that. Most compilers will generate code that
  uses the copy constructor, so the privacy violation error will be
  reported, but your C++ compiler is not required to generate code for
  this statement in a specific way. It could generate code using
  \e{neither} the copy constructor \e{nor} the assignment operator we
  made private. In that case, no error would be reported, but your
  application would probably crash when you called a member function
  of \c{w}.
*/

/*!
    \macro Q_DECLARE_FLAGS(Flags, Enum)
    \relates QFlags

    The Q_DECLARE_FLAGS() macro expands to

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 2

    \a Enum is the name of an existing enum type, whereas \a Flags is
    the name of the QFlags<\e{Enum}> typedef.

    See the QFlags documentation for details.

    \sa Q_DECLARE_OPERATORS_FOR_FLAGS()
*/

/*!
    \macro Q_DECLARE_OPERATORS_FOR_FLAGS(Flags)
    \relates QFlags

    The Q_DECLARE_OPERATORS_FOR_FLAGS() macro declares global \c
    operator|() functions for \a Flags, which is of type QFlags<T>.

    See the QFlags documentation for details.

    \sa Q_DECLARE_FLAGS()
*/

/*!
    \headerfile <QtGlobal>
    \title Global Qt Declarations
    \ingroup funclists

    \brief The <QtGlobal> header file includes the fundamental global
    declarations. It is included by most other Qt header files.

    The global declarations include \l{types}, \l{functions} and
    \l{macros}.

    The type definitions are partly convenience definitions for basic
    types (some of which guarantee certain bit-sizes on all platforms
    supported by Qt), partly types related to Qt message handling. The
    functions are related to generating messages, Qt version handling
    and comparing and adjusting object values. And finally, some of
    the declared macros enable programmers to add compiler or platform
    specific code to their applications, while others are convenience
    macros for larger operations.

    \section1 Types

    The header file declares several type definitions that guarantee a
    specified bit-size on all platforms supported by Qt for various
    basic types, for example \l qint8 which is a signed char
    guaranteed to be 8-bit on all platforms supported by Qt. The
    header file also declares the \l qlonglong type definition for \c
    {long long int } (\c __int64 on Windows).

    Several convenience type definitions are declared: \l qreal for \c
    double, \l uchar for \c unsigned char, \l uint for \c unsigned
    int, \l ulong for \c unsigned long and \l ushort for \c unsigned
    short.

    Finally, the QtMsgType definition identifies the various messages
    that can be generated and sent to a Qt message handler;
    QtMsgHandler is a type definition for a pointer to a function with
    the signature \c {void myMsgHandler(QtMsgType, const char *)}.

    \section1 Functions

    The <QtGlobal> header file contains several functions comparing
    and adjusting an object's value. These functions take a template
    type as argument: You can retrieve the absolute value of an object
    using the qAbs() function, and you can bound a given object's
    value by given minimum and maximum values using the qBound()
    function. You can retrieve the minimum and maximum of two given
    objects using qMin() and qMax() respectively. All these functions
    return a corresponding template type; the template types can be
    replaced by any other type.

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 3

    <QtGlobal> also contains functions that generate messages from the
    given string argument: qCritical(), qDebug(), qFatal() and
    qWarning(). These functions call the message handler with the
    given message.

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 4

    The remaining functions are qRound() and qRound64(), which both
    accept a \l qreal value as their argument returning the value
    rounded up to the nearest integer and 64-bit integer respectively,
    the qInstallMsgHandler() function which installs the given
    QtMsgHandler, and the qVersion() function which returns the
    version number of Qt at run-time as a string.

    \section1 Macros

    The <QtGlobal> header file provides a range of macros (Q_CC_*)
    that are defined if the application is compiled using the
    specified platforms. For example, the Q_CC_SUN macro is defined if
    the application is compiled using Forte Developer, or Sun Studio
    C++.  The header file also declares a range of macros (Q_OS_*)
    that are defined for the specified platforms. For example,
    Q_OS_WIN32 which is defined for Microsoft Windows.

    The purpose of these macros is to enable programmers to add
    compiler or platform specific code to their application.

    The remaining macros are convenience macros for larger operations:
    The QT_TRANSLATE_NOOP() and QT_TR_NOOP() macros provide the
    possibility of marking text for dynamic translation,
    i.e. translation without changing the stored source text. The
    Q_ASSERT() and Q_ASSERT_X() enables warning messages of various
    level of refinement. The Q_FOREACH() and foreach() macros
    implement Qt's foreach loop.

    The Q_INT64_C() and Q_UINT64_C() macros wrap signed and unsigned
    64-bit integer literals in a platform-independent way. The
    Q_CHECK_PTR() macro prints a warning containing the source code's
    file name and line number, saying that the program ran out of
    memory, if the pointer is 0. The qPrintable() and qUtf8Printable()
    macros represent an easy way of printing text.

    Finally, the QT_POINTER_SIZE macro expands to the size of a
    pointer in bytes, and the QT_VERSION and QT_VERSION_STR macros
    expand to a numeric value or a string, respectively, specifying
    Qt's version number, i.e the version the application is compiled
    against.

    \sa <QtAlgorithms>, QSysInfo
*/

/*!
    \typedef qreal
    \relates <QtGlobal>

    Typedef for \c double on all platforms except for those using CPUs with
    ARM architectures.
    On ARM-based platforms, \c qreal is a typedef for \c float for performance
    reasons.
*/

/*! \typedef uchar
    \relates <QtGlobal>

    Convenience typedef for \c{unsigned char}.
*/

/*!
    \fn qt_set_sequence_auto_mnemonic(bool on)
    \relates <QtGlobal>

    Enables automatic mnemonics on Mac if \a on is true; otherwise
    this feature is disabled.

    Note that this function is only available on Mac where mnemonics
    are disabled by default.

    To access to this function, use an extern declaration:
    extern void qt_set_sequence_auto_mnemonic(bool b);

    \sa {QShortcut#mnemonic}{QShortcut}
*/

/*! \typedef ushort
    \relates <QtGlobal>

    Convenience typedef for \c{unsigned short}.
*/

/*! \typedef uint
    \relates <QtGlobal>

    Convenience typedef for \c{unsigned int}.
*/

/*! \typedef ulong
    \relates <QtGlobal>

    Convenience typedef for \c{unsigned long}.
*/

/*! \typedef qint8
    \relates <QtGlobal>

    Typedef for \c{signed char}. This type is guaranteed to be 8-bit
    on all platforms supported by Qt.
*/

/*!
    \typedef quint8
    \relates <QtGlobal>

    Typedef for \c{unsigned char}. This type is guaranteed to
    be 8-bit on all platforms supported by Qt.
*/

/*! \typedef qint16
    \relates <QtGlobal>

    Typedef for \c{signed short}. This type is guaranteed to be
    16-bit on all platforms supported by Qt.
*/

/*!
    \typedef quint16
    \relates <QtGlobal>

    Typedef for \c{unsigned short}. This type is guaranteed to
    be 16-bit on all platforms supported by Qt.
*/

/*! \typedef qint32
    \relates <QtGlobal>

    Typedef for \c{signed int}. This type is guaranteed to be 32-bit
    on all platforms supported by Qt.
*/

/*!
    \typedef quint32
    \relates <QtGlobal>

    Typedef for \c{unsigned int}. This type is guaranteed to
    be 32-bit on all platforms supported by Qt.
*/

/*! \typedef qint64
    \relates <QtGlobal>

    Typedef for \c{long long int} (\c __int64 on Windows). This type
    is guaranteed to be 64-bit on all platforms supported by Qt.

    Literals of this type can be created using the Q_INT64_C() macro:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 5

    \sa Q_INT64_C(), quint64, qlonglong
*/

/*!
    \typedef quint64
    \relates <QtGlobal>

    Typedef for \c{unsigned long long int} (\c{unsigned __int64} on
    Windows). This type is guaranteed to be 64-bit on all platforms
    supported by Qt.

    Literals of this type can be created using the Q_UINT64_C()
    macro:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 6

    \sa Q_UINT64_C(), qint64, qulonglong
*/

/*!
    \typedef quintptr
    \relates <QtGlobal>

    Integral type for representing a pointers (useful for hashing,
    etc.).

    Typedef for either quint32 or quint64. This type is guaranteed to
    be the same size as a pointer on all platforms supported by Qt. On
    a system with 32-bit pointers, quintptr is a typedef for quint32;
    on a system with 64-bit pointers, quintptr is a typedef for
    quint64.

    Note that quintptr is unsigned. Use qptrdiff for signed values.

    \sa qptrdiff, quint32, quint64
*/

/*!
    \typedef qptrdiff
    \relates <QtGlobal>

    Integral type for representing pointer differences.

    Typedef for either qint32 or qint64. This type is guaranteed to be
    the same size as a pointer on all platforms supported by Qt. On a
    system with 32-bit pointers, quintptr is a typedef for quint32; on
    a system with 64-bit pointers, quintptr is a typedef for quint64.

    Note that qptrdiff is signed. Use quintptr for unsigned values.

    \sa quintptr, qint32, qint64
*/

/*!
    \typedef qsizetype
    \relates <QtGlobal>

    Integral type providing Posix' \c ssize_t for all platforms.

    This type is guaranteed to be the same size as a \c size_t on all
    platforms supported by Qt.

    Note that qsizetype is signed. Use \c size_t for unsigned values.

    \sa qptrdiff
*/

/*!
    \typedef QtMsgHandler
    \relates <QtGlobal>

    This is a typedef for a pointer to a function with the following
    signature:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 7

    \sa QtMsgType, qInstallMsgHandler()
*/

/*!
    \enum QtMsgType
    \relates <QtGlobal>

    This enum describes the messages that can be sent to a message
    handler (QtMsgHandler). You can use the enum to identify and
    associate the various message types with the appropriate
    actions.

    \value QtDebugMsg
           A message generated by the qDebug() function.
    \value QtWarningMsg
           A message generated by the qWarning() function.
    \value QtCriticalMsg
           A message generated by the qCritical() function.
    \value QtFatalMsg
           A message generated by the qFatal() function.
    \value QtSystemMsg


    \sa QtMsgHandler, qInstallMsgHandler()
*/

/*! \macro qint64 Q_INT64_C(literal)
    \relates <QtGlobal>

    Wraps the signed 64-bit integer \a literal in a
    platform-independent way.

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 8

    \sa qint64, Q_UINT64_C()
*/

/*! \macro quint64 Q_UINT64_C(literal)
    \relates <QtGlobal>

    Wraps the unsigned 64-bit integer \a literal in a
    platform-independent way.

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 9

    \sa quint64, Q_INT64_C()
*/

/*! \typedef qlonglong
    \relates <QtGlobal>

    Typedef for \c{long long int} (\c __int64 on Windows). This is
    the same as \l qint64.

    \sa qulonglong, qint64
*/

/*!
    \typedef qulonglong
    \relates <QtGlobal>

    Typedef for \c{unsigned long long int} (\c{unsigned __int64} on
    Windows). This is the same as \l quint64.

    \sa quint64, qlonglong
*/

/*! \fn const T &qAbs(const T &value)
    \relates <QtGlobal>

    Compares \a value to the 0 of type T and returns the absolute
    value. Thus if T is \e {double}, then \a value is compared to
    \e{(double) 0}.

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 10
*/

/*! \fn int qRound(qreal value)
    \relates <QtGlobal>

    Rounds \a value to the nearest integer.

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 11
*/

/*! \fn qint64 qRound64(qreal value)
    \relates <QtGlobal>

    Rounds \a value to the nearest 64-bit integer.

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 12
*/

/*! \fn const T &qMin(const T &value1, const T &value2)
    \relates <QtGlobal>

    Returns the minimum of \a value1 and \a value2.

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 13

    \sa qMax(), qBound()
*/

/*! \fn const T &qMax(const T &value1, const T &value2)
    \relates <QtGlobal>

    Returns the maximum of \a value1 and \a value2.

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 14

    \sa qMin(), qBound()
*/

/*! \fn const T &qBound(const T &min, const T &value, const T &max)
    \relates <QtGlobal>

    Returns \a value bounded by \a min and \a max. This is equivalent
    to qMax(\a min, qMin(\a value, \a max)).

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 15

    \sa qMin(), qMax()
*/

/*!
    \typedef Q_INT8
    \relates <QtGlobal>
    \compat

    Use \l qint8 instead.
*/

/*!
    \typedef Q_UINT8
    \relates <QtGlobal>
    \compat

    Use \l quint8 instead.
*/

/*!
    \typedef Q_INT16
    \relates <QtGlobal>
    \compat

    Use \l qint16 instead.
*/

/*!
    \typedef Q_UINT16
    \relates <QtGlobal>
    \compat

    Use \l quint16 instead.
*/

/*!
    \typedef Q_INT32
    \relates <QtGlobal>
    \compat

    Use \l qint32 instead.
*/

/*!
    \typedef Q_UINT32
    \relates <QtGlobal>
    \compat

    Use \l quint32 instead.
*/

/*!
    \typedef Q_INT64
    \relates <QtGlobal>
    \compat

    Use \l qint64 instead.
*/

/*!
    \typedef Q_UINT64
    \relates <QtGlobal>
    \compat

    Use \l quint64 instead.
*/

/*!
    \typedef Q_LLONG
    \relates <QtGlobal>
    \compat

    Use \l qint64 instead.
*/

/*!
    \typedef Q_ULLONG
    \relates <QtGlobal>
    \compat

    Use \l quint64 instead.
*/

/*!
    \typedef Q_LONG
    \relates <QtGlobal>
    \compat

    Use \c{void *} instead.
*/

/*!
    \typedef Q_ULONG
    \relates <QtGlobal>
    \compat

    Use \c{void *} instead.
*/

/*! \fn bool qSysInfo(int *wordSize, bool *bigEndian)
    \relates <QtGlobal>

    Use QSysInfo::WordSize and QSysInfo::ByteOrder instead.
*/

/*!
    \fn bool qt_winUnicode()
    \relates <QtGlobal>

    This function always returns true.

    \sa QSysInfo
*/

/*!
    \fn int qWinVersion()
    \relates <QtGlobal>

    Use QSysInfo::WindowsVersion instead.

    \sa QSysInfo
*/

/*!
    \fn int qMacVersion()
    \relates <QtGlobal>

    Use QSysInfo::MacintoshVersion instead.

    \sa QSysInfo
*/

/*!
    \macro QT_VERSION_CHECK
    \relates <QtGlobal>

    Turns the major, minor and patch numbers of a version into an
    integer, 0xMMNNPP (MM = major, NN = minor, PP = patch). This can
    be compared with another similarly processed version id.

    \sa QT_VERSION
*/

/*!
    \macro QT_VERSION
    \relates <QtGlobal>

    This macro expands a numeric value of the form 0xMMNNPP (MM =
    major, NN = minor, PP = patch) that specifies Qt's version
    number. For example, if you compile your application against Qt
    4.1.2, the QT_VERSION macro will expand to 0x040102.

    You can use QT_VERSION to use the latest Qt features where
    available.

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 16

    \sa QT_VERSION_STR, qVersion()
*/

/*!
    \macro QT_VERSION_STR
    \relates <QtGlobal>

    This macro expands to a string that specifies Qt's version number
    (for example, "4.1.2"). This is the version against which the
    application is compiled.

    \sa qVersion(), QT_VERSION
*/

/*!
    \relates <QtGlobal>

    Returns the version number of Qt at run-time as a string (for
    example, "4.1.2"). This may be a different version than the
    version the application was compiled against.

    \sa QT_VERSION_STR
*/

const char *qVersion()
{
    return QT_VERSION_STR;
}

bool qSharedBuild()
{
#ifdef QT_SHARED
    return true;
#else
    return false;
#endif
}

/*****************************************************************************
  System detection routines
 *****************************************************************************/

/*!
    \class QSysInfo
    \brief The QSysInfo class provides information about the system.

    \list
    \o \l WordSize specifies the size of a pointer for the platform
       on which the application is compiled.
    \o \l ByteOrder specifies whether the platform is big-endian or
       little-endian.
    \o \l WindowsVersion specifies the version of the Windows operating
       system on which the application is run (Windows only)
    \o \l MacintoshVersion specifies the version of the Macintosh
       operating system on which the application is run (Mac only).
    \endlist

    Some constants are defined only on certain platforms. You can use
    the preprocessor symbols Q_WS_WIN and Q_WS_MAC to test that
    the application is compiled under Windows or Mac.

    \sa QLibraryInfo
*/

/*!
    \enum QSysInfo::Sizes

    This enum provides platform-specific information about the sizes of data
    structures used by the underlying architecture.

    \value WordSize The size in bits of a pointer for the platform on which
           the application is compiled (32 or 64).
*/

/*!
    \variable QSysInfo::WindowsVersion
    \brief the version of the Windows operating system on which the
           application is run (Windows only)
*/

/*!
    \fn QSysInfo::WindowsVersion QSysInfo::windowsVersion()
    \since 4.4

    Returns the version of the Windows operating system on which the
    application is run (Windows only).
*/

/*!
    \variable QSysInfo::MacintoshVersion
    \brief the version of the Macintosh operating system on which
           the application is run (Mac only).
*/

/*!
    \fn QSysInfo::SymbianVersion QSysInfo::symbianVersion()
    \since 4.6

    Returns the version of the Symbian operating system on which the
    application is run (Symbian only).
*/

/*!
    \fn QSysInfo::S60Version QSysInfo::s60Version()
    \since 4.6

    Returns the version of the S60 SDK system on which the
    application is run (S60 only).
*/

/*!
    \enum QSysInfo::Endian

    \value BigEndian  Big-endian byte order (also called Network byte order)
    \value LittleEndian  Little-endian byte order
    \value ByteOrder  Equals BigEndian or LittleEndian, depending on
                      the platform's byte order.
*/

/*!
    \enum QSysInfo::WinVersion

    This enum provides symbolic names for the various versions of the
    Windows operating system. On Windows, the
    QSysInfo::WindowsVersion variable gives the version of the system
    on which the application is run.

    MS-DOS-based versions:

    \value WV_32s   Windows 3.1 with Win 32s
    \value WV_95    Windows 95
    \value WV_98    Windows 98
    \value WV_Me    Windows Me

    NT-based versions (note that each operating system version is only represented once rather than each Windows edition):

    \value WV_NT    Windows NT (operating system version 4.0)
    \value WV_2000  Windows 2000 (operating system version 5.0)
    \value WV_XP    Windows XP (operating system version 5.1)
    \value WV_2003  Windows Server 2003, Windows Server 2003 R2, Windows Home Server, Windows XP Professional x64 Edition (operating system version 5.2)
    \value WV_VISTA Windows Vista, Windows Server 2008 (operating system version 6.0)
    \value WV_WINDOWS7 Windows 7, Windows Server 2008 R2 (operating system version 6.1)
    \value WV_WINDOWS8 Windows 8 (operating system version 6.2)
    \value WV_WINDOWS8_1 Windows 8.1 (operating system version 6.3), introduced in Qt 4.8.6
    \value WV_WINDOWS10 Windows 10 (operating system version 10.0), introduced in Qt 4.8.7

    Alternatively, you may use the following macros which correspond directly to the Windows operating system version number:

    \value WV_4_0   Operating system version 4.0, corresponds to Windows NT
    \value WV_5_0   Operating system version 5.0, corresponds to Windows 2000
    \value WV_5_1   Operating system version 5.1, corresponds to Windows XP
    \value WV_5_2   Operating system version 5.2, corresponds to Windows Server 2003, Windows Server 2003 R2, Windows Home Server, and Windows XP Professional x64 Edition
    \value WV_6_0   Operating system version 6.0, corresponds to Windows Vista and Windows Server 2008
    \value WV_6_1   Operating system version 6.1, corresponds to Windows 7 and Windows Server 2008 R2
    \value WV_6_2   Operating system version 6.2, corresponds to Windows 8
    \value WV_6_3   Operating system version 6.3, corresponds to Windows 8.1, introduced in Qt 4.8.6
    \value WV_10_0  Operating system version 10.0, corresponds to Windows 10, introduced in Qt 4.8.7

    CE-based versions:

    \value WV_CE    Windows CE
    \value WV_CENET Windows CE .NET
    \value WV_CE_5  Windows CE 5.x
    \value WV_CE_6  Windows CE 6.x

    The following masks can be used for testing whether a Windows
    version is MS-DOS-based, NT-based, or CE-based:

    \value WV_DOS_based MS-DOS-based version of Windows
    \value WV_NT_based  NT-based version of Windows
    \value WV_CE_based  CE-based version of Windows

    \sa MacVersion, SymbianVersion
*/

/*!
    \enum QSysInfo::MacVersion

    This enum provides symbolic names for the various versions of the
    OS X operating system. On OS X, the
    QSysInfo::MacintoshVersion variable gives the version of the
    system on which the application is run.

    \value MV_9        Mac OS 9 (unsupported)
    \value MV_10_0     Mac OS X 10.0 (unsupported)
    \value MV_10_1     Mac OS X 10.1 (unsupported)
    \value MV_10_2     Mac OS X 10.2 (unsupported)
    \value MV_10_3     Mac OS X 10.3
    \value MV_10_4     Mac OS X 10.4
    \value MV_10_5     Mac OS X 10.5
    \value MV_10_6     Mac OS X 10.6
    \value MV_10_7     OS X 10.7
    \value MV_10_8     OS X 10.8
    \value MV_10_9     OS X 10.9
    \value MV_10_10    OS X 10.10
    \value MV_Unknown  An unknown and currently unsupported platform

    \value MV_CHEETAH  Apple codename for MV_10_0
    \value MV_PUMA     Apple codename for MV_10_1
    \value MV_JAGUAR   Apple codename for MV_10_2
    \value MV_PANTHER  Apple codename for MV_10_3
    \value MV_TIGER    Apple codename for MV_10_4
    \value MV_LEOPARD  Apple codename for MV_10_5
    \value MV_SNOWLEOPARD  Apple codename for MV_10_6
    \value MV_LION     Apple codename for MV_10_7
    \value MV_MOUNTAINLION Apple codename for MV_10_8
    \value MV_MAVERICKS    Apple codename for MV_10_9
    \value MV_YOSEMITE     Apple codename for MV_10_10

    \sa WinVersion, SymbianVersion
*/

/*!
    \enum QSysInfo::SymbianVersion

    This enum provides symbolic names for the various versions of the
    Symbian operating system. On Symbian, the
    QSysInfo::symbianVersion() function gives the version of the
    system on which the application is run.

    \value SV_9_2 Symbian OS v9.2
    \value SV_9_3 Symbian OS v9.3
    \value SV_9_4 Symbian OS v9.4
    \value SV_SF_1 S60 5th Edition (Symbian^1)
    \value SV_SF_2 Symbian^2
    \value SV_SF_3 Symbian^3 or Symbian Anna
    \value SV_SF_4 \e{This enum value is deprecated.}
    \value SV_API_5_3 Symbian/S60 API version 5.3 release
    \value SV_API_5_4 Symbian/S60 API version 5.4 release
    \value SV_API_5_5 Symbian/S60 API version 5.5 release
    \value SV_Unknown An unknown and currently unsupported platform

    \sa S60Version, WinVersion, MacVersion
*/

/*!
    \enum QSysInfo::S60Version

    This enum provides symbolic names for the various versions of the
    S60 SDK. On S60, the
    QSysInfo::s60Version() function gives the version of the
    SDK on which the application is run.

    \value SV_S60_3_1 S60 3rd Edition Feature Pack 1
    \value SV_S60_3_2 S60 3rd Edition Feature Pack 2
    \value SV_S60_5_0 S60 5th Edition
    \value SV_S60_5_1 \e{This enum value is deprecated.}
    \value SV_S60_5_2 Symbian^3 and Symbian Anna
    \value SV_S60_5_3 Symbian/S60 API version 5.3 release
    \value SV_S60_5_4 Symbian/S60 API version 5.4 release
    \value SV_S60_5_5 Symbian/S60 API version 5.5 release
    \value SV_S60_Unknown An unknown and currently unsupported platform
    \omitvalue SV_S60_None

    \sa SymbianVersion, WinVersion, MacVersion
*/

/*!
    \macro Q_WS_MAC
    \relates <QtGlobal>

    Defined on Mac OS X.

    \sa Q_WS_WIN, Q_WS_X11, Q_WS_QWS, Q_WS_QPA
*/

/*!
    \macro Q_WS_WIN
    \relates <QtGlobal>

    Defined on Windows.

    \sa Q_WS_MAC, Q_WS_X11, Q_WS_QWS, Q_WS_QPA
*/

/*!
    \macro Q_WS_X11
    \relates <QtGlobal>

    Defined on X11.

    \sa Q_WS_MAC, Q_WS_WIN, Q_WS_QWS, Q_WS_QPA
*/

/*!
    \macro Q_WS_QWS
    \relates <QtGlobal>

    Defined on Qt for Embedded Linux.

    \sa Q_WS_MAC, Q_WS_WIN, Q_WS_X11, Q_WS_QPA
*/

/*!
    \macro Q_WS_QPA
    \relates <QtGlobal>

    Defined on Qt for Embedded Linux, Lite version.

    \sa Q_WS_MAC, Q_WS_WIN, Q_WS_X11, Q_WS_QWS
*/

/*!
    \macro Q_OS_DARWIN
    \relates <QtGlobal>

    Defined on Darwin OS (synonym for Q_OS_MAC).
*/

/*!
    \macro Q_OS_WIN32
    \relates <QtGlobal>

    Defined on all supported versions of Windows.
*/

/*!
    \macro Q_OS_CYGWIN
    \relates <QtGlobal>

    Defined on Cygwin.
*/

/*!
    \macro Q_OS_LINUX
    \relates <QtGlobal>

    Defined on Linux.
*/

/*!
    \macro Q_OS_FREEBSD
    \relates <QtGlobal>

    Defined on FreeBSD.
*/

/*!
    \macro Q_OS_NETBSD
    \relates <QtGlobal>

    Defined on NetBSD.
*/

/*!
    \macro Q_OS_OPENBSD
    \relates <QtGlobal>

    Defined on OpenBSD.
*/

/*!
    \macro Q_OS_BSDI
    \relates <QtGlobal>

    Defined on BSD/OS.
*/

/*!
    \macro Q_OS_HURD
    \relates <QtGlobal>

    Defined on GNU Hurd.
*/

/*!
    \macro Q_OS_BSD4
    \relates <QtGlobal>

    Defined on Any BSD 4.4 system.
*/

/*!
    \macro Q_OS_UNIX
    \relates <QtGlobal>

    Defined on Any UNIX BSD/SYSV system.
*/

/*!
    \macro Q_CC_MSVC
    \relates <QtGlobal>

    Defined if the application is compiled using Microsoft Visual
    C/C++, Intel C++ for Windows.
*/

/*!
    \macro Q_CC_GNU
    \relates <QtGlobal>

    Defined if the application is compiled using GNU C++.
*/

/*!
    \macro Q_CC_INTEL
    \relates <QtGlobal>

    Defined if the application is compiled using Intel C++ for Linux,
    Intel C++ for Windows.
*/

/*!
  \macro Q_OS_MAC
  \relates <QtGlobal>

  Defined on MAC OS (synonym for Darwin).
 */

#if defined(QT_BUILD_QMAKE)
// needed to bootstrap qmake
static const unsigned int qt_one = 1;
const int QSysInfo::ByteOrder = ((*((unsigned char *) &qt_one) == 0) ? BigEndian : LittleEndian);
#endif

#if !defined(QWS) && defined(Q_OS_MAC)

QT_BEGIN_INCLUDE_NAMESPACE
#include "private/qcore_mac_p.h"
#include "qnamespace.h"
QT_END_INCLUDE_NAMESPACE

static QSysInfo::MacVersion macVersion()
{
#if !defined(Q_OS_IOS)
    SInt32 gestalt_version;
    if (Gestalt(gestaltSystemVersionMinor, &gestalt_version) == noErr) {
        // add 2 because OS X 10.0 is 0x02 in the enum
        return QSysInfo::MacVersion(gestalt_version + 2);
    }
#endif
    return QSysInfo::MV_Unknown;
}
const QSysInfo::MacVersion QSysInfo::MacintoshVersion = macVersion();

#elif defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)

QT_BEGIN_INCLUDE_NAMESPACE
#include "qt_windows.h"
QT_END_INCLUDE_NAMESPACE

// Determine Windows versions >= 8 by querying the version of kernel32.dll.
static inline bool determineWinOsVersionPost8(OSVERSIONINFO *result)
{
    typedef WORD (WINAPI* PtrGetFileVersionInfoSizeW)(LPCWSTR, LPDWORD);
    typedef BOOL (WINAPI* PtrVerQueryValueW)(LPCVOID, LPCWSTR, LPVOID, PUINT);
    typedef BOOL (WINAPI* PtrGetFileVersionInfoW)(LPCWSTR, DWORD, DWORD, LPVOID);

    QSystemLibrary versionLib(QLatin1String("version"));
    if (!versionLib.load())
        return false;
    PtrGetFileVersionInfoSizeW getFileVersionInfoSizeW = (PtrGetFileVersionInfoSizeW)versionLib.resolve("GetFileVersionInfoSizeW");
    PtrVerQueryValueW verQueryValueW = (PtrVerQueryValueW)versionLib.resolve("VerQueryValueW");
    PtrGetFileVersionInfoW getFileVersionInfoW = (PtrGetFileVersionInfoW)versionLib.resolve("GetFileVersionInfoW");
    if (!getFileVersionInfoSizeW || !verQueryValueW || !getFileVersionInfoW)
        return false;

    const wchar_t kernel32Dll[] = L"kernel32.dll";
    DWORD handle;
    const DWORD size = getFileVersionInfoSizeW(kernel32Dll, &handle);
    if (!size)
        return false;
    QScopedArrayPointer<BYTE> versionInfo(new BYTE[size]);
    if (!getFileVersionInfoW(kernel32Dll, handle, size, versionInfo.data()))
        return false;
    UINT uLen;
    VS_FIXEDFILEINFO *fileInfo = 0;
    if (!verQueryValueW(versionInfo.data(), L"\\", (LPVOID *)&fileInfo, &uLen))
        return false;
    const DWORD fileVersionMS = fileInfo->dwFileVersionMS;
    const DWORD fileVersionLS = fileInfo->dwFileVersionLS;
    result->dwMajorVersion = HIWORD(fileVersionMS);
    result->dwMinorVersion = LOWORD(fileVersionMS);
    result->dwBuildNumber = HIWORD(fileVersionLS);
    return true;
}

// Fallback for determining Windows versions >= 8 by looping using the
// version check macros. Note that it will return build number=0 to avoid
// inefficient looping.
static inline void determineWinOsVersionFallbackPost8(OSVERSIONINFO *result)
{
    result->dwBuildNumber = 0;
    DWORDLONG conditionMask = 0;
    VER_SET_CONDITION(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(conditionMask, VER_PLATFORMID, VER_EQUAL);
    OSVERSIONINFOEX checkVersion = { sizeof(OSVERSIONINFOEX), result->dwMajorVersion, 0,
                                     result->dwBuildNumber, result->dwPlatformId, {'\0'}, 0, 0, 0, 0, 0 };
    for ( ; VerifyVersionInfo(&checkVersion, VER_MAJORVERSION | VER_PLATFORMID, conditionMask); ++checkVersion.dwMajorVersion)
        result->dwMajorVersion = checkVersion.dwMajorVersion;
    conditionMask = 0;
    checkVersion.dwMajorVersion = result->dwMajorVersion;
    checkVersion.dwMinorVersion = 0;
    VER_SET_CONDITION(conditionMask, VER_MAJORVERSION, VER_EQUAL);
    VER_SET_CONDITION(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(conditionMask, VER_PLATFORMID, VER_EQUAL);
    for ( ; VerifyVersionInfo(&checkVersion, VER_MAJORVERSION | VER_MINORVERSION | VER_PLATFORMID, conditionMask); ++checkVersion.dwMinorVersion)
        result->dwMinorVersion = checkVersion.dwMinorVersion;
}

static inline OSVERSIONINFO winOsVersion()
{
    OSVERSIONINFO result = { sizeof(OSVERSIONINFO), 0, 0, 0, 0, {'\0'}};
    // GetVersionEx() has been deprecated in Windows 8.1 and will return
    // only Windows 8 from that version on.
#  if defined(_MSC_VER) && _MSC_VER >= 1800
#    pragma warning( push )
#    pragma warning( disable : 4996 )
#  endif
    GetVersionEx(&result);
#  if defined(_MSC_VER) && _MSC_VER >= 1800
#    pragma warning( pop )
#  endif
    if (result.dwMajorVersion == 6 && result.dwMinorVersion == 2) {
        if (!determineWinOsVersionPost8(&result))
            determineWinOsVersionFallbackPost8(&result);
    }
    return result;
}

QSysInfo::WinVersion QSysInfo::windowsVersion()
{
#ifndef VER_PLATFORM_WIN32s
#define VER_PLATFORM_WIN32s            0
#endif
#ifndef VER_PLATFORM_WIN32_WINDOWS
#define VER_PLATFORM_WIN32_WINDOWS  1
#endif
#ifndef VER_PLATFORM_WIN32_NT
#define VER_PLATFORM_WIN32_NT            2
#endif
#ifndef VER_PLATFORM_WIN32_CE
#define VER_PLATFORM_WIN32_CE            3
#endif

    static QSysInfo::WinVersion winver;
    if (winver)
        return winver;
    winver = QSysInfo::WV_NT;
    const OSVERSIONINFO osver = winOsVersion();
    switch (osver.dwPlatformId) {
    case VER_PLATFORM_WIN32s:
        winver = QSysInfo::WV_32s;
        break;
    case VER_PLATFORM_WIN32_WINDOWS:
        // We treat Windows Me (minor 90) the same as Windows 98
        if (osver.dwMinorVersion == 90)
            winver = QSysInfo::WV_Me;
        else if (osver.dwMinorVersion == 10)
            winver = QSysInfo::WV_98;
        else
            winver = QSysInfo::WV_95;
        break;
    default: // VER_PLATFORM_WIN32_NT
        if (osver.dwMajorVersion < 5) {
            winver = QSysInfo::WV_NT;
        } else if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 0) {
            winver = QSysInfo::WV_2000;
        } else if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 1) {
            winver = QSysInfo::WV_XP;
        } else if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 2) {
            winver = QSysInfo::WV_2003;
        } else if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 0) {
            winver = QSysInfo::WV_VISTA;
        } else if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 1) {
            winver = QSysInfo::WV_WINDOWS7;
        } else if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 2) {
            winver = QSysInfo::WV_WINDOWS8;
        } else if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 3) {
            winver = QSysInfo::WV_WINDOWS8_1;
        } else if (osver.dwMajorVersion == 10 && osver.dwMinorVersion == 0) {
            winver = QSysInfo::WV_WINDOWS10;
        } else {
            qWarning("Qt: Untested Windows version %d.%d detected!",
                     int(osver.dwMajorVersion), int(osver.dwMinorVersion));
            winver = QSysInfo::WV_NT_based;
        }
    }

#ifdef QT_DEBUG
    {
        QByteArray override = qgetenv("QT_WINVER_OVERRIDE");
        if (override.isEmpty())
            return winver;

        if (override == "Me")
            winver = QSysInfo::WV_Me;
        if (override == "95")
            winver = QSysInfo::WV_95;
        else if (override == "98")
            winver = QSysInfo::WV_98;
        else if (override == "NT")
            winver = QSysInfo::WV_NT;
        else if (override == "2000")
            winver = QSysInfo::WV_2000;
        else if (override == "2003")
            winver = QSysInfo::WV_2003;
        else if (override == "XP")
            winver = QSysInfo::WV_XP;
        else if (override == "VISTA")
            winver = QSysInfo::WV_VISTA;
        else if (override == "WINDOWS7")
            winver = QSysInfo::WV_WINDOWS7;
        else if (override == "WINDOWS8")
            winver = QSysInfo::WV_WINDOWS8;
    }
#endif

    return winver;
}

const QSysInfo::WinVersion QSysInfo::WindowsVersion = QSysInfo::windowsVersion();

#endif

/*!
    \macro void Q_ASSERT(bool test)
    \relates <QtGlobal>

    Prints a warning message containing the source code file name and
    line number if \a test is false.

    Q_ASSERT() is useful for testing pre- and post-conditions
    during development. It does nothing if \c QT_NO_DEBUG was defined
    during compilation.

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 17

    If \c b is zero, the Q_ASSERT statement will output the following
    message using the qFatal() function:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 18

    \sa Q_ASSERT_X(), qFatal(), {Debugging Techniques}
*/

/*!
    \macro void Q_ASSERT_X(bool test, const char *where, const char *what)
    \relates <QtGlobal>

    Prints the message \a what together with the location \a where,
    the source file name and line number if \a test is false.

    Q_ASSERT_X is useful for testing pre- and post-conditions during
    development. It does nothing if \c QT_NO_DEBUG was defined during
    compilation.

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 19

    If \c b is zero, the Q_ASSERT_X statement will output the following
    message using the qFatal() function:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 20

    \sa Q_ASSERT(), qFatal(), {Debugging Techniques}
*/

/*!
    \macro void Q_ASSUME(bool expr)
    \relates <QtGlobal>

    Causes the compiler to assume that \a expr is true. This macro is useful
    for improving code generation, by providing the compiler with hints about
    conditions that it would not otherwise know about. However, there is no
    guarantee that the compiler will actually use those hints.

    This macro could be considered a "lighter" version of \ref Q_ASSERT. While
    Q_ASSERT will abort the program's execution if the condition is false,
    Q_ASSUME will tell the compiler not to generate code for those conditions.

    Therefore, it is important that the assumptions always hold, otherwise
    undefined behaviour may occur.
    If \a expr is a constantly false condition, Q_ASSUME will tell the compiler
    that the current code execution cannot be reached. That is, Q_ASSUME(false)
    is equivalent to Q_UNREACHABLE().
    \note Q_LIKELY() tells the compiler that the expression is likely, but not
    the only possibility. Q_ASSUME tells the compiler that it is the only
    possibility.

    \sa Q_ASSERT(), Q_UNREACHABLE(), Q_LIKELY()
*/

/*!
    \macro void Q_UNREACHABLE()
    \relates <QtGlobal>

    Tells the compiler that the current point cannot be reached by any
    execution, so it may optimise any code paths leading here as dead code, as
    well as code continuing from here.

    This macro is useful to mark impossible conditions. For example, given the
    following enum:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp qunreachable-enum

    One can write a switch table like so:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp qunreachable-switch

    The advantage of inserting Q_UNREACHABLE() at that point is that the
    compiler is told not to generate code for a shape variable containing that
    value. If the macro is missing, the compiler will still generate the
    necessary comparisons for that value. If the case label were removed, some
    compilers could produce a warning that some enum values were not checked.

    By using this macro in impossible conditions, code coverage may be improved
    as dead code paths may be eliminated.

    \sa Q_ASSERT(), Q_ASSUME(), qFatal()
*/

/*!
    \macro void Q_CHECK_PTR(void *pointer)
    \relates <QtGlobal>

    If \a pointer is 0, prints a warning message containing the source
    code's file name and line number, saying that the program ran out
    of memory.

    Q_CHECK_PTR does nothing if \c QT_NO_DEBUG was defined during
    compilation.

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 21

    \sa qWarning(), {Debugging Techniques}
*/

/*!
    \fn T *q_check_ptr(T *pointer)
    \relates <QtGlobal>

    Users Q_CHECK_PTR on \a pointer, then returns \a pointer.

    This can be used as an inline version of Q_CHECK_PTR.
*/

/*!
    \macro const char* Q_FUNC_INFO()
    \relates <QtGlobal>

    Expands to a string that describe the function the macro resides in. How this string looks
    more specifically is compiler dependent. With GNU GCC it is typically the function signature,
    while with other compilers it might be the line and column number.

    Q_FUNC_INFO can be conveniently used with qDebug(). For example, this function:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 22

    when instantiated with the integer type, will with the GCC compiler produce:

    \tt{const TInputType& myMin(const TInputType&, const TInputType&) [with TInputType = int] was called with value1: 3 value2: 4}

    If this macro is used outside a function, the behavior is undefined.
 */

/*
  The Q_CHECK_PTR macro calls this function if an allocation check
  fails.
*/
void qt_check_pointer(const char *n, int l)
{
    qFatal("In file %s, line %d: Out of memory", n, l);
}

/* \internal
   Allows you to throw an exception without including <new>
   Called internally from Q_CHECK_PTR on certain OS combinations
*/
void qBadAlloc()
{
    QT_THROW(std::bad_alloc());
}

/*
  The Q_ASSERT macro calls this function when the test fails.
*/
void qt_assert(const char *assertion, const char *file, int line)
{
    qFatal("ASSERT: \"%s\" in file %s, line %d", assertion, file, line);
}

/*
  The Q_ASSERT_X macro calls this function when the test fails.
*/
void qt_assert_x(const char *where, const char *what, const char *file, int line)
{
    qFatal("ASSERT failure in %s: \"%s\", file %s, line %d", where, what, file, line);
}


/*
    Dijkstra's bisection algorithm to find the square root of an integer.
    Deliberately not exported as part of the Qt API, but used in both
    qsimplerichtext.cpp and qgfxraster_qws.cpp
*/
Q_CORE_EXPORT unsigned int qt_int_sqrt(unsigned int n)
{
    // n must be in the range 0...UINT_MAX/2-1
    if (n >= (UINT_MAX>>2)) {
        unsigned int r = 2 * qt_int_sqrt(n / 4);
        unsigned int r2 = r + 1;
        return (n >= r2 * r2) ? r2 : r;
    }
    uint h, p= 0, q= 1, r= n;
    while (q <= n)
        q <<= 2;
    while (q != 1) {
        q >>= 2;
        h= p + q;
        p >>= 1;
        if (r >= h) {
            p += q;
            r -= h;
        }
    }
    return p;
}

static QtMsgHandler handler = nullptr;                // pointer to debug handler

#if defined(QT_USE_SLOG2)
#ifndef QT_LOG_CODE
#define QT_LOG_CODE 9000
#endif

extern char *__progname;

static void slog2_default_handler(QtMsgType msgType, const char *message)
{
    if (slog2_set_default_buffer((slog2_buffer_t)-1) == 0) {
        slog2_buffer_set_config_t buffer_config;
        slog2_buffer_t buffer_handle;

        buffer_config.buffer_set_name = __progname;
        buffer_config.num_buffers = 1;
        buffer_config.verbosity_level = SLOG2_INFO;
        buffer_config.buffer_config[0].buffer_name = "default";
        buffer_config.buffer_config[0].num_pages = 8;

        if (slog2_register(&buffer_config, &buffer_handle, 0) == -1) {
            fprintf(stderr, "Error registering slogger2 buffer!\n");
            fprintf(stderr, "%s", message);
            fflush(stderr);
            return;
        }

        // Set as the default buffer
        slog2_set_default_buffer(buffer_handle);
    }
    int severity;
    //Determines the severity level
    switch (msgType) {
    case QtDebugMsg:
        severity = SLOG2_INFO;
        break;
    case QtWarningMsg:
        severity = SLOG2_NOTICE;
        break;
    case QtCriticalMsg:
        severity = SLOG2_WARNING;
        break;
    case QtFatalMsg:
        severity = SLOG2_ERROR;
        break;
    }
    //writes to the slog2 buffer
    slog2c(NULL, QT_LOG_CODE, severity, message);
}
#endif // QT_USE_SLOG2

#if !defined(Q_OS_WIN) && !defined(QT_NO_THREAD) && \
    defined(_POSIX_THREAD_SAFE_FUNCTIONS) && _POSIX_VERSION >= 200112L
namespace {
    // There are two incompatible versions of strerror_r:
    // a) the XSI/POSIX.1 version, which returns an int,
    //    indicating success or not
    // b) the GNU version, which returns a char*, which may or may not
    //    be the beginning of the buffer we used
    // The GNU libc manpage for strerror_r says you should use the the XSI
    // version in portable code. However, it's impossible to do that if
    // _GNU_SOURCE is defined so we use C++ overloading to decide what to do
    // depending on the return type
    static inline QString fromstrerror_helper(int, const QByteArray &buf)
    {
        return QString::fromLocal8Bit(buf);
    }
    static inline QString fromstrerror_helper(const char *str, const QByteArray &)
    {
        return QString::fromLocal8Bit(str);
    }
}
#endif

QString qt_error_string(int errorCode)
{
    const char *s = nullptr;
    QString ret;
    if (errorCode == -1) {
#if defined(Q_OS_WIN)
        errorCode = GetLastError();
#else
        errorCode = errno;
#endif
    }
    switch (errorCode) {
    case 0:
        break;
    case EACCES:
        s = QT_TRANSLATE_NOOP("QIODevice", "Permission denied");
        break;
    case EMFILE:
        s = QT_TRANSLATE_NOOP("QIODevice", "Too many open files");
        break;
    case ENOENT:
        s = QT_TRANSLATE_NOOP("QIODevice", "No such file or directory");
        break;
    case ENOSPC:
        s = QT_TRANSLATE_NOOP("QIODevice", "No space left on device");
        break;
    default: {
#ifdef Q_OS_WIN
        wchar_t *string = 0;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
                      NULL,
                      errorCode,
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                      (LPWSTR)&string,
                      0,
                      NULL);
        ret = QString::fromWCharArray(string);
        LocalFree((HLOCAL)string);

        if (ret.isEmpty() && errorCode == ERROR_MOD_NOT_FOUND)
            ret = QString::fromLatin1("The specified module could not be found.");
#elif !defined(QT_NO_THREAD) && defined(_POSIX_THREAD_SAFE_FUNCTIONS) && _POSIX_VERSION >= 200112L
        QByteArray buf(1024, '\0');
        ret = fromstrerror_helper(strerror_r(errorCode, buf.data(), buf.size()), buf);
#else
        ret = QString::fromLocal8Bit(strerror(errorCode));
#endif
    break; }
    }
    if (s)
        // ######## this breaks moc build currently
//         ret = QCoreApplication::translate("QIODevice", s);
        ret = QString::fromLatin1(s);
    return ret.trimmed();
}


/*!
    \fn QtMsgHandler qInstallMsgHandler(QtMsgHandler handler)
    \relates <QtGlobal>

    Installs a Qt message \a handler which has been defined
    previously. Returns a pointer to the previous message handler
    (which may be 0).

    The message handler is a function that prints out debug messages,
    warnings, critical and fatal error messages. The Qt library (debug
    mode) contains hundreds of warning messages that are printed
    when internal errors (usually invalid function arguments)
    occur. Qt built in release mode also contains such warnings unless
    QT_NO_WARNING_OUTPUT and/or QT_NO_DEBUG_OUTPUT have been set during
    compilation. If you implement your own message handler, you get total
    control of these messages.

    The default message handler prints the message to the standard
    output under X11 or to the debugger under Windows. If it is a
    fatal message, the application aborts immediately.

    Only one message handler can be defined, since this is usually
    done on an application-wide basis to control debug output.

    To restore the message handler, call \c qInstallMsgHandler(0).

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 23

    \sa qDebug(), qWarning(), qCritical(), qFatal(), QtMsgType,
    {Debugging Techniques}
*/
#if defined(Q_OS_WIN) && defined(QT_BUILD_CORE_LIB)
extern bool usingWinMain;
extern Q_CORE_EXPORT void qWinMsgHandler(QtMsgType t, const char* str);
#endif

QtMsgHandler qInstallMsgHandler(QtMsgHandler h)
{
    QtMsgHandler old = handler;
    handler = h;
#if defined(Q_OS_WIN) && defined(QT_BUILD_CORE_LIB)
    if (!handler && usingWinMain)
        handler = qWinMsgHandler;
#endif
    return old;
}

/*!
    \internal
*/
void qt_message_output(QtMsgType msgType, const char *buf)
{
    if (handler) {
        (*handler)(msgType, buf);
    } else {
#if defined(QT_USE_SLOG2)
        slog2_default_handler(msgType, buf);
#else
        fprintf(stderr, "%s\n", buf);
        fflush(stderr);
#endif
    }

    if (msgType == QtFatalMsg
        || (msgType == QtWarningMsg
            && (!qgetenv("QT_FATAL_WARNINGS").isNull())) ) {

#if defined(Q_CC_MSVC) && defined(QT_DEBUG) && defined(_DEBUG) && defined(_CRT_ERROR)
        // get the current report mode
        int reportMode = _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW);
        _CrtSetReportMode(_CRT_ERROR, reportMode);
        int ret = _CrtDbgReport(_CRT_ERROR, __FILE__, __LINE__, QT_VERSION_STR, buf);
        if (ret == 0  && reportMode & _CRTDBG_MODE_WNDW)
            return; // ignore
        else if (ret == 1)
            _CrtDbgBreak();
#endif

#if (defined(Q_OS_UNIX) || defined(Q_CC_MINGW))
        abort(); // trap; generates core dump
#else
        exit(1); // goodbye cruel world
#endif
    }
}

#if !defined(QT_NO_EXCEPTIONS)
/*!
    \internal
    Uses a local buffer to output the message. Not locale safe + cuts off
    everything after character 255, but will work in out of memory situations.
*/
static void qEmergencyOut(QtMsgType msgType, const char *msg, va_list ap)
{
    char emergency_buf[256] = { '\0' };
    emergency_buf[255] = '\0';
    if (msg)
        qvsnprintf(emergency_buf, 255, msg, ap);
    qt_message_output(msgType, emergency_buf);
}
#endif

/*!
    \internal
*/
static void qt_message(QtMsgType msgType, const char *msg, va_list ap)
{
#if !defined(QT_NO_EXCEPTIONS)
    if (std::uncaught_exceptions() > 0) {
        qEmergencyOut(msgType, msg, ap);
        return;
    }
#endif
    QByteArray buf;
    if (msg) {
        QT_TRY {
            buf = QString::vasprintf(msg, ap).toLocal8Bit();
        } QT_CATCH(const std::bad_alloc &) {
#if !defined(QT_NO_EXCEPTIONS)
            qEmergencyOut(msgType, msg, ap);
            // don't rethrow - we use qWarning and friends in destructors.
            return;
#endif
        }
    }
    qt_message_output(msgType, buf.constData());
}

#undef qDebug
/*!
    \relates <QtGlobal>

    Calls the message handler with the debug message \a msg. If no
    message handler has been installed, the message is printed to
    stderr. Under Windows, the message is sent to the console, if it is a
    console application; otherwise, it is sent to the debugger. This
    function does nothing if \c QT_NO_DEBUG_OUTPUT was defined
    during compilation.

    If you pass the function a format string and a list of arguments,
    it works in similar way to the C printf() function. The format
    should be a Latin-1 string.

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 24

    If you include \c <QtDebug>, a more convenient syntax is also
    available:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 25

    With this syntax, the function returns a QDebug object that is
    configured to use the QtDebugMsg message type. It automatically
    puts a single space between each item, and outputs a newline at
    the end. It supports many C++ and Qt types.

    To suppress the output at run-time, install your own message handler
    with qInstallMsgHandler().

    \sa qWarning(), qCritical(), qFatal(), qInstallMsgHandler(),
        {Debugging Techniques}
*/
void qDebug(const char *msg, ...)
{
    va_list ap;
    va_start(ap, msg); // use variable arg list
    qt_message(QtDebugMsg, msg, ap);
    va_end(ap);
}

#undef qWarning
/*!
    \relates <QtGlobal>

    Calls the message handler with the warning message \a msg. If no
    message handler has been installed, the message is printed to
    stderr. Under Windows, the message is sent to the debugger. This
    function does nothing if \c QT_NO_WARNING_OUTPUT was defined
    during compilation; it exits if the environment variable \c
    QT_FATAL_WARNINGS is defined.

    This function takes a format string and a list of arguments,
    similar to the C printf() function. The format should be a Latin-1
    string.

    Example:
    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 26

    If you include <QtDebug>, a more convenient syntax is
    also available:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 27

    This syntax inserts a space between each item, and
    appends a newline at the end.

    To suppress the output at runtime, install your own message handler
    with qInstallMsgHandler().

    \sa qDebug(), qCritical(), qFatal(), qInstallMsgHandler(),
        {Debugging Techniques}
*/
void qWarning(const char *msg, ...)
{
    va_list ap;
    va_start(ap, msg); // use variable arg list
    qt_message(QtWarningMsg, msg, ap);
    va_end(ap);
}

/*!
    \relates <QtGlobal>

    Calls the message handler with the critical message \a msg. If no
    message handler has been installed, the message is printed to
    stderr. Under Windows, the message is sent to the debugger.

    This function takes a format string and a list of arguments,
    similar to the C printf() function. The format should be a Latin-1
    string.

    Example:
    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 28

    If you include <QtDebug>, a more convenient syntax is
    also available:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 29

    A space is inserted between the items, and a newline is
    appended at the end.

    To suppress the output at runtime, install your own message handler
    with qInstallMsgHandler().

    \sa qDebug(), qWarning(), qFatal(), qInstallMsgHandler(),
        {Debugging Techniques}
*/
void qCritical(const char *msg, ...)
{
    va_list ap;
    va_start(ap, msg); // use variable arg list
    qt_message(QtCriticalMsg, msg, ap);
    va_end(ap);
}

void qErrnoWarning(const char *msg, ...)
{
    // qt_error_string() will allocate anyway, so we don't have
    // to be careful here (like we do in plain qWarning())
    QString buf;
    va_list ap;
    va_start(ap, msg);
    if (msg)
        buf = QString::vasprintf(msg, ap);
    va_end(ap);

    qCritical("%s (%s)", buf.toLocal8Bit().constData(), qt_error_string(-1).toLocal8Bit().constData());
}

void qErrnoWarning(int code, const char *msg, ...)
{
    // qt_error_string() will allocate anyway, so we don't have
    // to be careful here (like we do in plain qWarning())
    QString buf;
    va_list ap;
    va_start(ap, msg);
    if (msg)
        buf = QString::vasprintf(msg, ap);
    va_end(ap);

    qCritical("%s (%s)", buf.toLocal8Bit().constData(), qt_error_string(code).toLocal8Bit().constData());
}

/*!
    \relates <QtGlobal>

    Calls the message handler with the fatal message \a msg. If no
    message handler has been installed, the message is printed to
    stderr. Under Windows, the message is sent to the debugger.

    If you are using the \bold{default message handler} this function will
    abort on Unix systems to create a core dump. On Windows, for debug builds,
    this function will report a _CRT_ERROR enabling you to connect a debugger
    to the application.

    This function takes a format string and a list of arguments,
    similar to the C printf() function.

    Example:
    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 30

    To suppress the output at runtime, install your own message handler
    with qInstallMsgHandler().

    \sa qDebug(), qCritical(), qWarning(), qInstallMsgHandler(),
        {Debugging Techniques}
*/
void qFatal(const char *msg, ...)
{
    va_list ap;
    va_start(ap, msg); // use variable arg list
    qt_message(QtFatalMsg, msg, ap);
    va_end(ap);
}

// getenv is declared as deprecated in VS2005. This function
// makes use of the new secure getenv function.
/*!
    \relates <QtGlobal>

    Returns the value of the environment variable with name \a
    varName. To get the variable string, use QByteArray::constData().

    \note qgetenv() was introduced because getenv() from the standard
    C library was deprecated in VC2005 (and later versions). qgetenv()
    uses the new replacement function in VC, and calls the standard C
    library's implementation on all other platforms.

    \sa qputenv()
*/
QByteArray qgetenv(const char *varName)
{
#if defined(_MSC_VER) && _MSC_VER >= 1400
    size_t requiredSize = 0;
    QByteArray buffer;
    getenv_s(&requiredSize, 0, 0, varName);
    if (requiredSize == 0)
        return buffer;
    buffer.resize(int(requiredSize));
    getenv_s(&requiredSize, buffer.data(), requiredSize, varName);
    // requiredSize includes the terminating null, which we don't want.
    Q_ASSERT(buffer.endsWith('\0'));
    buffer.chop(1);
    return buffer;
#else
    return QByteArray(::getenv(varName));
#endif
}

/*!
    \relates <QtGlobal>
    \internal

    This function checks whether the environment variable \a varName
    is empty.

    Equivalent to
    \code
    qgetenv(varName).isEmpty()
    \endcode
    except that it's potentially much faster, and can't throw exceptions.

    \sa qgetenv(), qEnvironmentVariableIsSet()
*/
bool qEnvironmentVariableIsEmpty(const char *varName) noexcept
{
#if defined(_MSC_VER) && _MSC_VER >= 1400
    // we provide a buffer that can only hold the empty string, so
    // when the env.var isn't empty, we'll get an ERANGE error (buffer
    // too small):
    size_t dummy;
    char buffer = '\0';
    return getenv_s(&dummy, &buffer, 1, varName) != ERANGE;
#else
    const char * const value = ::getenv(varName);
    return !value || !*value;
#endif
}

/*!
    \relates <QtGlobal>
    \internal

    This function checks whether the environment variable \a varName
    is set.

    Equivalent to
    \code
    !qgetenv(varName).isNull()
    \endcode
    except that it's potentially much faster, and can't throw exceptions.

    \sa qgetenv(), qEnvironmentVariableIsEmpty()
*/
bool qEnvironmentVariableIsSet(const char *varName) noexcept
{
#if defined(_MSC_VER) && _MSC_VER >= 1400
    size_t requiredSize = 0;
    (void)getenv_s(&requiredSize, 0, 0, varName);
    return requiredSize != 0;
#else
    return ::getenv(varName) != 0;
#endif
}

/*!
    \relates <QtGlobal>

    This function sets the \a value of the environment variable named
    \a varName. It will create the variable if it does not exist. It
    returns 0 if the variable could not be set.

    Calling qputenv with an empty value removes the environment variable on
    Windows, and makes it set (but empty) on Unix. Prefer using qunsetenv()
    for fully portable behavior.

    \note qputenv() was introduced because putenv() from the standard
    C library was deprecated in VC2005 (and later versions). qputenv()
    uses the replacement function in VC, and calls the standard C
    library's implementation on all other platforms.

    \sa qgetenv()
*/
bool qputenv(const char *varName, const QByteArray& value)
{
#if defined(_MSC_VER) && _MSC_VER >= 1400
    return _putenv_s(varName, value.constData()) == 0;
#else
    QByteArray buffer(varName);
    buffer += '=';
    buffer += value;
    char* envVar = qstrdup(buffer.constData());
    int result = putenv(envVar);
    if (result != 0) // error. we have to delete the string.
        delete[] envVar;
    return result == 0;
#endif
}

/*!
    \relates <QtGlobal>

    This function deletes the variable \a varName from the environment.

    Returns true on success.

    \since 5.1

    \sa qputenv(), qgetenv()
*/
bool qunsetenv(const char *varName)
{
#if defined(_MSC_VER) && _MSC_VER >= 1400
    return _putenv_s(varName, "") == 0;
#elif (defined(_POSIX_VERSION) && (_POSIX_VERSION-0) >= 200112L) || defined(Q_OS_BSD4)
    // POSIX.1-2001 and BSD have unsetenv
    return unsetenv(varName) == 0;
#elif defined(Q_CC_MINGW)
    // On mingw, putenv("var=") removes "var" from the environment
    QByteArray buffer(varName);
    buffer += '=';
    return putenv(buffer.constData()) == 0;
#else
    // Fallback to putenv("var=") which will insert an empty var into the
    // environment and leak it
    QByteArray buffer(varName);
    buffer += '=';
    char *envVar = qstrdup(buffer.constData());
    return putenv(envVar) == 0;
#endif
}

#if defined(Q_OS_UNIX) && !defined(QT_NO_THREAD)

typedef uint SeedStorageType;

typedef QThreadStorage<SeedStorageType *> SeedStorage;
Q_GLOBAL_STATIC(SeedStorage, randTLS)  // Thread Local Storage for seed value

#endif

/*!
    \relates <QtGlobal>
    \since 4.2

    Thread-safe version of the standard C++ \c srand() function.

    Sets the argument \a seed to be used to generate a new random number sequence of
    pseudo random integers to be returned by qrand().

    The sequence of random numbers generated is deterministic per thread. For example,
    if two threads call qsrand(1) and subsequently calls qrand(), the threads will get
    the same random number sequence.

    \sa qrand()
*/
void qsrand(uint seed)
{
#if defined(Q_OS_UNIX) && !defined(QT_NO_THREAD)
    SeedStorage *seedStorage = randTLS();
    if (seedStorage) {
        SeedStorageType *pseed = seedStorage->localData();
        if (!pseed)
            seedStorage->setLocalData(pseed = new SeedStorageType);
        *pseed = seed;
    } else {
        //global static seed storage should always exist,
        //except after being deleted by QGlobalStaticDeleter.
        //But since it still can be called from destructor of another
        //global static object, fallback to srand(seed)
        srand(seed);
    }
#else
    // On Windows and Symbian srand() and rand() already use Thread-Local-Storage
    // to store the seed between calls
    // this is also valid for QT_NO_THREAD
    srand(seed);
#endif
}

/*!
    \relates <QtGlobal>
    \since 4.2

    Thread-safe version of the standard C++ \c rand() function.

    Returns a value between 0 and \c RAND_MAX (defined in \c <cstdlib> and
    \c <stdlib.h>), the next number in the current sequence of pseudo-random
    integers.

    Use \c qsrand() to initialize the pseudo-random number generator with
    a seed value.

    \sa qsrand()
*/
int qrand()
{
#if defined(Q_OS_UNIX) && !defined(QT_NO_THREAD)
    SeedStorage *seedStorage = randTLS();
    if (seedStorage) {
        SeedStorageType *pseed = seedStorage->localData();
        if (!pseed) {
            seedStorage->setLocalData(pseed = new SeedStorageType);
            *pseed = 1;
        }
        return rand_r(pseed);
    } else {
        //global static seed storage should always exist,
        //except after being deleted by QGlobalStaticDeleter.
        //But since it still can be called from destructor of another
        //global static object, fallback to rand()
        return rand();
    }
#else
    // On Windows and Symbian srand() and rand() already use Thread-Local-Storage
    // to store the seed between calls
    // this is also valid for QT_NO_THREAD
    return rand();
#endif
}

/*!
    \macro forever
    \relates <QtGlobal>

    This macro is provided for convenience for writing infinite
    loops.

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 31

    It is equivalent to \c{for (;;)}.

    If you're worried about namespace pollution, you can disable this
    macro by adding the following line to your \c .pro file:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 32

    \sa Q_FOREVER
*/

/*!
    \macro Q_FOREVER
    \relates <QtGlobal>

    Same as \l{forever}.

    This macro is available even when \c no_keywords is specified
    using the \c .pro file's \c CONFIG variable.

    \sa foreach()
*/

/*!
    \macro foreach(variable, container)
    \relates <QtGlobal>

    This macro is used to implement Qt's \c foreach loop. The \a
    variable parameter is a variable name or variable definition; the
    \a container parameter is a Qt container whose value type
    corresponds to the type of the variable. See \l{The foreach
    Keyword} for details.

    If you're worried about namespace pollution, you can disable this
    macro by adding the following line to your \c .pro file:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 33

    \sa Q_FOREACH()
*/

/*!
    \macro Q_FOREACH(variable, container)
    \relates <QtGlobal>

    Same as foreach(\a variable, \a container).

    This macro is available even when \c no_keywords is specified
    using the \c .pro file's \c CONFIG variable.

    \sa foreach()
*/

/*!
    \macro QT_TR_NOOP(sourceText)
    \relates <QtGlobal>

    Marks the string literal \a sourceText for dynamic translation in
    the current context (class), i.e the stored \a sourceText will not
    be altered.

    The macro expands to \a sourceText.

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 34

    The macro QT_TR_NOOP_UTF8() is identical except that it tells lupdate
    that the source string is encoded in UTF-8. Corresponding variants
    exist in the QT_TRANSLATE_NOOP() family of macros, too.

    \sa QT_TRANSLATE_NOOP(), {Internationalization with Qt}
*/

/*!
    \macro QT_TRANSLATE_NOOP(context, sourceText)
    \relates <QtGlobal>

    Marks the string literal \a sourceText for dynamic translation in
    the given \a context; i.e, the stored \a sourceText will not be
    altered. The \a context is typically a class and also needs to
    be specified as string literal.

    The macro expands to \a sourceText.

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 35

    \sa QT_TR_NOOP(), QT_TRANSLATE_NOOP3(), {Internationalization with Qt}
*/

/*!
    \macro QT_TRANSLATE_NOOP3(context, sourceText, comment)
    \relates <QtGlobal>
    \since 4.4

    Marks the string literal \a sourceText for dynamic translation in the
    given \a context and with \a comment, i.e the stored \a sourceText will
    not be altered. The \a context is typically a class and also needs to
    be specified as string literal. The string literal \a comment
    will be available for translators using e.g. Qt Linguist.

    The macro expands to anonymous struct of the two string
    literals passed as \a sourceText and \a comment.

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 36

    \sa QT_TR_NOOP(), QT_TRANSLATE_NOOP(), {Internationalization with Qt}
*/

/*!
    \fn QString qtTrId(const char *id, int n = -1)
    \relates <QtGlobal>
    \reentrant
    \since 4.6

    \brief The qtTrId function finds and returns a translated string.

    Returns a translated string identified by \a id.
    If no matching string is found, the id itself is returned. This
    should not happen under normal conditions.

    If \a n >= 0, all occurrences of \c %n in the resulting string
    are replaced with a decimal representation of \a n. In addition,
    depending on \a n's value, the translation text may vary.

    Meta data and comments can be passed as documented for QObject::tr().
    In addition, it is possible to supply a source string template like that:

    \tt{//% <C string>}

    or

    \tt{\begincomment% <C string> \endcomment}

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp qttrid

    Creating QM files suitable for use with this function requires passing
    the \c -idbased option to the \c lrelease tool.

    \warning This method is reentrant only if all translators are
    installed \e before calling this method. Installing or removing
    translators while performing translations is not supported. Doing
    so will probably result in crashes or other undesirable behavior.

    \sa QObject::tr(), QCoreApplication::translate(), {Internationalization with Qt}
*/

/*!
    \macro QT_TRID_NOOP(id)
    \relates <QtGlobal>
    \since 4.6

    \brief The QT_TRID_NOOP macro marks an id for dynamic translation.

    The only purpose of this macro is to provide an anchor for attaching
    meta data like to qtTrId().

    The macro expands to \a id.

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp qttrid_noop

    \sa qtTrId(), {Internationalization with Qt}
*/

/*!
    \macro Q_LIKELY(expr)
    \relates <QtGlobal>
    \since 4.8

    \brief Hints to the compiler that the enclosed condition, \a expr, is
    likely to evaluate to \c true.

    Use of this macro can help the compiler to optimize the code.

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp qlikely

    \sa Q_UNLIKELY()
*/

/*!
    \macro Q_UNLIKELY(expr)
    \relates <QtGlobal>
    \since 4.8

    \brief Hints to the compiler that the enclosed condition, \a expr, is
    likely to evaluate to \c false.

    Use of this macro can help the compiler to optimize the code.

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp qunlikely

    \sa Q_LIKELY()
*/

/*!
    \macro QT_POINTER_SIZE
    \relates <QtGlobal>

    Expands to the size of a pointer in bytes (4 or 8). This is
    equivalent to \c sizeof(void *) but can be used in a preprocessor
    directive.
*/

/*!
    \macro TRUE
    \relates <QtGlobal>
    \obsolete

    Synonym for \c true.

    \sa FALSE
*/

/*!
    \macro FALSE
    \relates <QtGlobal>
    \obsolete

    Synonym for \c false.

    \sa TRUE
*/

/*!
    \macro QABS(n)
    \relates <QtGlobal>
    \obsolete

    Use qAbs(\a n) instead.

    \sa QMIN(), QMAX()
*/

/*!
    \macro QMIN(x, y)
    \relates <QtGlobal>
    \obsolete

    Use std::min(\a x, \a y) instead.

    \sa QMAX(), QABS()
*/

/*!
    \macro QMAX(x, y)
    \relates <QtGlobal>
    \obsolete

    Use std::max(\a x, \a y) instead.

    \sa QMIN(), QABS()
*/

/*!
    \macro const char *qPrintable(const QString &str)
    \relates <QtGlobal>

    Returns \a str as a \c{const char *}. This is equivalent to
    \a{str}.toLocal8Bit().constData().

    The char pointer will be invalid after the statement in which
    qPrintable() is used. This is because the array returned by
    toLocal8Bit() will fall out of scope.
    \note qDebug(), qWarning(), qCritical(), qFatal() expect %s
    arguments to be UTF-8 encoded, while qPrintable() converts to
    local 8-bit encoding. Therefore qUtf8Printable() should be used
    for logging strings instead of qPrintable().
    \sa qUtf8Printable()
*/

/*!
    \macro const char *qUtf8Printable(const QString &str)
    \relates <QtGlobal>
    \since 5.4
    Returns \a str as a \c{const char *}. This is equivalent to
    \a{str}.toUtf8().constData().
    The char pointer will be invalid after the statement in which
    qUtf8Printable() is used. This is because the array returned by
    QString::toUtf8() will fall out of scope.

    Example:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 37


    \sa qDebug(), qWarning(), qCritical(), qFatal()
*/

/*!
    \macro Q_DECLARE_TYPEINFO(Type, Flags)
    \relates <QtGlobal>

    You can use this macro to specify information about a custom type
    \a Type. With accurate type information, Qt's \l{Container Classes}
    {generic containers} can choose appropriate storage methods and
    algorithms.

    \a Flags can be one of the following:

    \list
    \o \c Q_PRIMITIVE_TYPE specifies that \a Type is a POD (plain old
       data) type with no constructor or destructor, and for which memcpy()ing
       creates a valid independent copy of the object.
    \o \c Q_MOVABLE_TYPE specifies that \a Type has a constructor
       and/or a destructor but can be moved in memory using \c
       memcpy().
    \o \c Q_COMPLEX_TYPE (the default) specifies that \a Type has
       constructors and/or a destructor and that it may not be moved
       in memory.
    \endlist

    Example of a "primitive" type:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 38

    Example of a movable type:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 39

    Qt will try to detect the class of a type using std::is_trivial or
    std::is_trivially_copyable. Use this macro to tune the behavior.
    For instance many types would be candidates for Q_MOVABLE_TYPE despite
    not being trivially-copyable. For binary compatibility reasons, QList
    optimizations are only enabled if there is an explicit
    Q_DECLARE_TYPEINFO even for trivially-copyable types.
*/

/*!
    \macro Q_UNUSED(name)
    \relates <QtGlobal>

    Indicates to the compiler that the parameter with the specified
    \a name is not used in the body of a function. This can be used to
    suppress compiler warnings while allowing functions to be defined
    with meaningful parameter names in their signatures.
*/

struct QInternal_CallBackTable {
    QVector<QList<qInternalCallback> > callbacks;
};

Q_GLOBAL_STATIC(QInternal_CallBackTable, global_callback_table)

bool QInternal::registerCallback(Callback cb, qInternalCallback callback)
{
    if (cb >= 0 && cb < QInternal::LastCallback) {
        QInternal_CallBackTable *cbt = global_callback_table();
        cbt->callbacks.resize(cb + 1);
        cbt->callbacks[cb].append(callback);
        return true;
    }
    return false;
}

bool QInternal::unregisterCallback(Callback cb, qInternalCallback callback)
{
    if (cb >= 0 && cb < QInternal::LastCallback) {
        QInternal_CallBackTable *cbt = global_callback_table();
        return (bool) cbt->callbacks[cb].removeAll(callback);
    }
    return false;
}

bool QInternal::activateCallbacks(Callback cb, void **parameters)
{
    Q_ASSERT_X(cb >= 0, "QInternal::activateCallback()", "Callback id must be a valid id");

    QInternal_CallBackTable *cbt = global_callback_table();
    if (cbt && cb < cbt->callbacks.size()) {
        QList<qInternalCallback> callbacks = cbt->callbacks[cb];
        bool ret = false;
        for (auto callback : callbacks)
            ret |= callback(parameters);
        return ret;
    }
    return false;
}

extern void qt_set_current_thread_to_main_thread();

bool QInternal::callFunction(InternalFunction func, void **args)
{
    Q_ASSERT_X(func >= 0,
               "QInternal::callFunction()", "Callback id must be a valid id");
#ifndef QT_NO_QOBJECT
    switch (func) {
#ifndef QT_NO_THREAD
    case QInternal::CreateThreadForAdoption:
        *args = QAdoptedThread::createThreadForAdoption();
        return true;
#endif
    case QInternal::RefAdoptedThread:
        QThreadData::get2((QThread *) *args)->ref();
        return true;
    case QInternal::DerefAdoptedThread:
        QThreadData::get2((QThread *) *args)->deref();
        return true;
    case QInternal::SetCurrentThreadToMainThread:
        qt_set_current_thread_to_main_thread();
        return true;
    case QInternal::SetQObjectSender: {
        QObject *receiver = (QObject *) args[0];
        QObjectPrivate::Sender *sender = new QObjectPrivate::Sender;
        sender->sender = (QObject *) args[1];
        sender->signal = *(int *) args[2];
        sender->ref = 1;

        // Store the old sender as "return value"
        args[3] = QObjectPrivate::setCurrentSender(receiver, sender);
        args[4] = sender;
        return true;
    }
    case QInternal::GetQObjectSender: {
        QObject *receiver = (QObject *) args[0];
        QObjectPrivate *d = QObjectPrivate::get(receiver);
        args[1] = d->currentSender ? d->currentSender->sender : nullptr;
        return true;
    }
    case QInternal::ResetQObjectSender: {
        QObject *receiver = (QObject *) args[0];
        QObjectPrivate::Sender *oldSender = (QObjectPrivate::Sender *) args[1];
        QObjectPrivate::Sender *sender = (QObjectPrivate::Sender *) args[2];
        QObjectPrivate::resetCurrentSender(receiver, sender, oldSender);
        delete sender;
        return true;
    }

    default:
        break;
    }
#else
    Q_UNUSED(args);
    Q_UNUSED(func);
#endif

    return false;
}

/*!
    \macro Q_BYTE_ORDER
    \relates <QtGlobal>

    This macro can be used to determine the byte order your system
    uses for storing data in memory. i.e., whether your system is
    little-endian or big-endian. It is set by Qt to one of the macros
    Q_LITTLE_ENDIAN or Q_BIG_ENDIAN. You normally won't need to worry
    about endian-ness, but you might, for example if you need to know
    which byte of an integer or UTF-16 character is stored in the
    lowest address. Endian-ness is important in networking, where
    computers with different values for Q_BYTE_ORDER must pass data
    back and forth.

    Use this macro as in the following examples.

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 40

    \sa Q_BIG_ENDIAN, Q_LITTLE_ENDIAN
*/

/*!
    \macro Q_LITTLE_ENDIAN
    \relates <QtGlobal>

    This macro represents a value you can compare to the macro
    Q_BYTE_ORDER to determine the endian-ness of your system.  In a
    little-endian system, the least significant byte is stored at the
    lowest address. The other bytes follow in increasing order of
    significance.

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 41

    \sa Q_BYTE_ORDER, Q_BIG_ENDIAN
*/

/*!
    \macro Q_BIG_ENDIAN
    \relates <QtGlobal>

    This macro represents a value you can compare to the macro
    Q_BYTE_ORDER to determine the endian-ness of your system.  In a
    big-endian system, the most significant byte is stored at the
    lowest address. The other bytes follow in decreasing order of
    significance.

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 42

    \sa Q_BYTE_ORDER, Q_LITTLE_ENDIAN
*/

/*!
    \macro Q_GLOBAL_STATIC(type, name)
    \internal

    Declares a global static variable with the given \a type and \a name.

    Use this macro to instantiate an object in a thread-safe way, creating
    a global pointer that can be used to refer to it.

    \warning This macro is subject to a race condition that can cause the object
    to be constructed twice. However, if this occurs, the second instance will
    be immediately deleted.

    See also
    \l{http://www.aristeia.com/publications.html}{"C++ and the perils of Double-Checked Locking"}
    by Scott Meyers and Andrei Alexandrescu.
*/

/*!
    \macro Q_GLOBAL_STATIC_WITH_ARGS(type, name, arguments)
    \internal

    Declares a global static variable with the specified \a type and \a name.

    Use this macro to instantiate an object using the \a arguments specified
    in a thread-safe way, creating a global pointer that can be used to refer
    to it.

    \warning This macro is subject to a race condition that can cause the object
    to be constructed twice. However, if this occurs, the second instance will
    be immediately deleted.

    See also
    \l{http://www.aristeia.com/publications.html}{"C++ and the perils of Double-Checked Locking"}
    by Scott Meyers and Andrei Alexandrescu.
*/

/*!
    \macro QT_NAMESPACE
    \internal

    If this macro is defined to \c ns all Qt classes are put in a namespace
    called \c ns. Also, moc will output code putting metaobjects etc.
    into namespace \c ns.

    \sa QT_BEGIN_NAMESPACE, QT_END_NAMESPACE,
    QT_PREPEND_NAMESPACE, QT_USE_NAMESPACE,
    QT_BEGIN_INCLUDE_NAMESPACE, QT_END_INCLUDE_NAMESPACE,
    QT_BEGIN_MOC_NAMESPACE, QT_END_MOC_NAMESPACE,
*/

/*!
    \macro QT_PREPEND_NAMESPACE(identifier)
    \internal

    This macro qualifies \a identifier with the full namespace.
    It expands to \c{::QT_NAMESPACE::identifier} if \c QT_NAMESPACE is defined
    and only \a identifier otherwise.

    \sa QT_NAMESPACE
*/

/*!
    \macro QT_USE_NAMESPACE
    \internal

    This macro expands to using QT_NAMESPACE if QT_NAMESPACE is defined
    and nothing otherwise.

    \sa QT_NAMESPACE
*/

/*!
    \macro QT_BEGIN_NAMESPACE
    \internal

    This macro expands to

    \snippet snippets/code/src_corelib_global_qglobal.cpp begin namespace macro

    if \c QT_NAMESPACE is defined and nothing otherwise. If should always
    appear in the file-level scope and be followed by \c QT_END_NAMESPACE
    at the same logical level with respect to preprocessor conditionals
    in the same file.

    As a rule of thumb, \c QT_BEGIN_NAMESPACE should appear in all Qt header
    and Qt source files after the last \c{#include} line and before the first
    declaration. In Qt headers using \c QT_BEGIN_HEADER, \c QT_BEGIN_NAMESPACE
    follows \c QT_BEGIN_HEADER immediately.

    If that rule can't be followed because, e.g., \c{#include} lines and
    declarations are wildly mixed, place \c QT_BEGIN_NAMESPACE before
    the first declaration and wrap the \c{#include} lines in
    \c QT_BEGIN_INCLUDE_NAMESPACE and \c QT_END_INCLUDE_NAMESPACE.

    When using the \c QT_NAMESPACE feature in user code
    (e.g., when building plugins statically linked to Qt) where
    the user code is not intended to go into the \c QT_NAMESPACE
    namespace, all forward declarations of Qt classes need to
    be wrapped in \c QT_BEGIN_NAMESPACE and \c QT_END_NAMESPACE.
    After that, a \c QT_USE_NAMESPACE should follow.
    No further changes should be needed.

    \sa QT_NAMESPACE
*/

/*!
    \macro QT_END_NAMESPACE
    \internal

    This macro expands to

    \snippet snippets/code/src_corelib_global_qglobal.cpp end namespace macro

    if \c QT_NAMESPACE is defined and nothing otherwise. It is used to cancel
    the effect of \c QT_BEGIN_NAMESPACE.

    If a source file ends with a \c{#include} directive that includes a moc file,
    \c QT_END_NAMESPACE should be placed before that \c{#include}.

    \sa QT_NAMESPACE
*/

/*!
    \macro QT_BEGIN_INCLUDE_NAMESPACE
    \internal

    This macro is equivalent to \c QT_END_NAMESPACE.
    It only serves as syntactic sugar and is intended
    to be used before #include lines within a
    \c QT_BEGIN_NAMESPACE ... \c QT_END_NAMESPACE block.

    \sa QT_NAMESPACE
*/

/*!
    \macro QT_END_INCLUDE_NAMESPACE
    \internal

    This macro is equivalent to \c QT_BEGIN_NAMESPACE.
    It only serves as syntactic sugar and is intended
    to be used after #include lines within a
    \c QT_BEGIN_NAMESPACE ... \c QT_END_NAMESPACE block.

    \sa QT_NAMESPACE
*/

/*!
    \macro QT_BEGIN_MOC_NAMESPACE
    \internal

    This macro is output by moc at the beginning of
    moc files. It is equivalent to \c QT_USE_NAMESPACE.

    \sa QT_NAMESPACE
*/

/*!
    \macro QT_END_MOC_NAMESPACE
    \internal

    This macro is output by moc at the beginning of
    moc files. It expands to nothing.

    \sa QT_NAMESPACE
*/

/*!
 \fn bool qFuzzyCompare(double p1, double p2)
 \relates <QtGlobal>
 \since 4.4
 \threadsafe

 Compares the floating point value \a p1 and \a p2 and
 returns \c true if they are considered equal, otherwise \c false.

 Note that comparing values where either \a p1 or \a p2 is 0.0 will not work.
 The solution to this is to compare against values greater than or equal to 1.0.

 \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 46

 The two numbers are compared in a relative way, where the
 exactness is stronger the smaller the numbers are.
 */

/*!
 \fn bool qFuzzyCompare(float p1, float p2)
 \relates <QtGlobal>
 \since 4.4
 \threadsafe

 Compares the floating point value \a p1 and \a p2 and
 returns \c true if they are considered equal, otherwise \c false.

 The two numbers are compared in a relative way, where the
 exactness is stronger the smaller the numbers are.
 */

/*!
    \macro QT_REQUIRE_VERSION(int argc, char **argv, const char *version)
    \relates <QtGlobal>

    This macro can be used to ensure that the application is run
    against a recent enough version of Qt. This is especially useful
    if your application depends on a specific bug fix introduced in a
    bug-fix release (e.g., 4.0.2).

    The \a argc and \a argv parameters are the \c main() function's
    \c argc and \c argv parameters. The \a version parameter is a
    string literal that specifies which version of Qt the application
    requires (e.g., "4.0.2").

    Example:

    \snippet doc/src/snippets/code/src_gui_dialogs_qmessagebox.cpp 4
*/

/*!
    \macro Q_DECL_EXPORT
    \relates <QtGlobal>

    This macro marks a symbol for shared library export (see
     \l{sharedlibrary.html}{Creating Shared Libraries}).

    \sa Q_DECL_IMPORT
*/

/*!
    \macro Q_DECL_IMPORT
    \relates <QtGlobal>

    This macro declares a symbol to be an import from a shared library (see
    \l{sharedlibrary.html}{Creating Shared Libraries}).

    \sa Q_DECL_EXPORT
*/

QT_END_NAMESPACE

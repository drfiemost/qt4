/****************************************************************************
**
** Copyright (C) 2011 Thiago Macieira <thiago@kde.org>
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QBASICATOMIC_H
#define QBASICATOMIC_H

#include <QtCore/qglobal.h>

#if defined(QT_BOOTSTRAPPED)
#  include <QtCore/qatomic_bootstrap.h>

// Compiler dependent implementation
#elif defined(Q_CC_MSVC)
  // not ported yet
#  define QT_OLD_ATOMICS

// Operating system dependent implementation
#elif defined(QT_ARCH_INTEGRITY)
#  include "QtCore/qatomic_integrity.h"
#elif defined(QT_ARCH_VXWORKS)
#  include "QtCore/qatomic_vxworks.h"

// Processor dependent implementation
#elif defined(QT_ARCH_ALPHA)
#  include "QtCore/qatomic_alpha.h"
#elif defined(QT_ARCH_ARM)
#  include "QtCore/qatomic_arm.h"
#elif defined(QT_ARCH_ARMV6)
#  include "QtCore/qatomic_armv6.h"
#elif defined(QT_ARCH_BFIN)
#  include "QtCore/qatomic_bfin.h"
#elif defined(QT_ARCH_IA64)
#  include "QtCore/qatomic_ia64.h"
#elif defined(QT_ARCH_MIPS)
#  include "QtCore/qatomic_mips.h"
#elif defined(QT_ARCH_POWERPC)
#  include "QtCore/qatomic_powerpc.h"
#elif defined(QT_ARCH_S390)
#  include "QtCore/qatomic_s390.h"
#elif defined(QT_ARCH_SH4A)
#  include "QtCore/qatomic_sh4a.h"
#elif defined(QT_ARCH_SPARC)
#  include "QtCore/qatomic_sparc.h"
#elif defined(__i386) || defined(__i386__) || defined(__x86_64) || defined(__x86_64__) || defined(__amd64)
#  include <QtCore/qatomic_x86.h>

// Fallback compiler dependent implementation
#elif defined(Q_COMPILER_ATOMICS) && defined(Q_COMPILER_CONSTEXPR)
#  include <QtCore/qatomic_cxx11.h>
#elif defined(Q_CC_GNU)
#  include <QtCore/qatomic_gcc.h>
#else
#  error "Qt has not been ported to this platform"
#endif

// Only include if the implementation has been ported to QAtomicOps
#ifndef QOLDBASICATOMIC_H

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

#if 0
#pragma qt_no_master_include
#pragma qt_sync_stop_processing
#endif

// New atomics

#if defined(Q_COMPILER_CONSTEXPR) && defined(Q_COMPILER_DEFAULT_MEMBERS) && defined(Q_COMPILER_DELETE_MEMBERS)
# define QT_BASIC_ATOMIC_HAS_CONSTRUCTORS
#endif

template <typename T>
class QBasicAtomicInteger
{
public:
    typedef QAtomicOps<T> Ops;
    // static check that this is a valid integer
    static_assert(QAtomicIntegerTraits<T>::IsInteger, "Template parameter is not a supported integer on this platform");

    typename Ops::Type _q_value;

    // Everything below is either implemented in ../arch/qatomic_XXX.h or (as fallback) in qgenericatomic.h
    T load() const noexcept { return Ops::load(_q_value); }
    void store(T newValue) noexcept { Ops::store(_q_value, newValue); }

    T loadAcquire() noexcept { return Ops::loadAcquire(_q_value); }
    void storeRelease(T newValue) noexcept { Ops::storeRelease(_q_value, newValue); }
    operator T() { return loadAcquire(); }
    T operator=(T newValue) { storeRelease(newValue); return newValue; }

    static constexpr bool isReferenceCountingNative() noexcept { return Ops::isReferenceCountingNative(); }
    static constexpr bool isReferenceCountingWaitFree() noexcept { return Ops::isReferenceCountingWaitFree(); }

    bool ref() noexcept { return Ops::ref(_q_value); }
    bool deref() noexcept { return Ops::deref(_q_value); }

    static constexpr bool isTestAndSetNative() noexcept { return Ops::isTestAndSetNative(); }
    static constexpr bool isTestAndSetWaitFree() noexcept { return Ops::isTestAndSetWaitFree(); }

    bool testAndSetRelaxed(T expectedValue, T newValue) noexcept
    { return Ops::testAndSetRelaxed(_q_value, expectedValue, newValue); }
    bool testAndSetAcquire(T expectedValue, T newValue) noexcept
    { return Ops::testAndSetAcquire(_q_value, expectedValue, newValue); }
    bool testAndSetRelease(T expectedValue, T newValue) noexcept
    { return Ops::testAndSetRelease(_q_value, expectedValue, newValue); }
    bool testAndSetOrdered(T expectedValue, T newValue) noexcept
    { return Ops::testAndSetOrdered(_q_value, expectedValue, newValue); }

    static constexpr bool isFetchAndStoreNative() noexcept { return Ops::isFetchAndStoreNative(); }
    static constexpr bool isFetchAndStoreWaitFree() noexcept { return Ops::isFetchAndStoreWaitFree(); }

    T fetchAndStoreRelaxed(T newValue) noexcept
    { return Ops::fetchAndStoreRelaxed(_q_value, newValue); }
    T fetchAndStoreAcquire(T newValue) noexcept
    { return Ops::fetchAndStoreAcquire(_q_value, newValue); }
    T fetchAndStoreRelease(T newValue) noexcept
    { return Ops::fetchAndStoreRelease(_q_value, newValue); }
    T fetchAndStoreOrdered(T newValue) noexcept
    { return Ops::fetchAndStoreOrdered(_q_value, newValue); }

    static constexpr bool isFetchAndAddNative() noexcept { return Ops::isFetchAndAddNative(); }
    static constexpr bool isFetchAndAddWaitFree() noexcept { return Ops::isFetchAndAddWaitFree(); }

    T fetchAndAddRelaxed(T valueToAdd) noexcept
    { return Ops::fetchAndAddRelaxed(_q_value, valueToAdd); }
    T fetchAndAddAcquire(T valueToAdd) noexcept
    { return Ops::fetchAndAddAcquire(_q_value, valueToAdd); }
    T fetchAndAddRelease(T valueToAdd) noexcept
    { return Ops::fetchAndAddRelease(_q_value, valueToAdd); }
    T fetchAndAddOrdered(T valueToAdd) noexcept
    { return Ops::fetchAndAddOrdered(_q_value, valueToAdd); }

#ifdef QT_BASIC_ATOMIC_HAS_CONSTRUCTORS
    QBasicAtomicInteger() = default;
    constexpr QBasicAtomicInteger(T value) noexcept : _q_value(value) {}
    QBasicAtomicInteger(const QBasicAtomicInteger &) = delete;
    QBasicAtomicInteger &operator=(const QBasicAtomicInteger &) = delete;
    QBasicAtomicInteger &operator=(const QBasicAtomicInteger &) volatile = delete;
#endif
};
typedef QBasicAtomicInteger<int> QBasicAtomicInt;

template <typename X>
class QBasicAtomicPointer
{
public:
    typedef X *Type;
    typedef QAtomicOps<Type> Ops;
    typedef typename Ops::Type AtomicType;

    AtomicType _q_value;

    // Non-atomic API
    Type load() const noexcept { return _q_value; }
    void store(Type newValue) noexcept { _q_value = newValue; }

    // Atomic API, implemented in qatomic_XXX.h
    Type loadAcquire() noexcept { return Ops::loadAcquire(_q_value); }
    void storeRelease(Type newValue) noexcept { Ops::storeRelease(_q_value, newValue); }
    operator Type() { return loadAcquire(); }
    Type operator=(Type newValue) { storeRelease(newValue); return newValue; }

    static constexpr bool isTestAndSetNative() noexcept { return Ops::isTestAndSetNative(); }
    static constexpr bool isTestAndSetWaitFree() noexcept { return Ops::isTestAndSetWaitFree(); }

    bool testAndSetRelaxed(Type expectedValue, Type newValue) noexcept
    { return Ops::testAndSetRelaxed(_q_value, expectedValue, newValue); }
    bool testAndSetAcquire(Type expectedValue, Type newValue) noexcept
    { return Ops::testAndSetAcquire(_q_value, expectedValue, newValue); }
    bool testAndSetRelease(Type expectedValue, Type newValue) noexcept
    { return Ops::testAndSetRelease(_q_value, expectedValue, newValue); }
    bool testAndSetOrdered(Type expectedValue, Type newValue) noexcept
    { return Ops::testAndSetOrdered(_q_value, expectedValue, newValue); }

    static constexpr bool isFetchAndStoreNative() noexcept { return Ops::isFetchAndStoreNative(); }
    static constexpr bool isFetchAndStoreWaitFree() noexcept { return Ops::isFetchAndStoreWaitFree(); }

    Type fetchAndStoreRelaxed(Type newValue) noexcept
    { return Ops::fetchAndStoreRelaxed(_q_value, newValue); }
    Type fetchAndStoreAcquire(Type newValue) noexcept
    { return Ops::fetchAndStoreAcquire(_q_value, newValue); }
    Type fetchAndStoreRelease(Type newValue) noexcept
    { return Ops::fetchAndStoreRelease(_q_value, newValue); }
    Type fetchAndStoreOrdered(Type newValue) noexcept
    { return Ops::fetchAndStoreOrdered(_q_value, newValue); }

    static constexpr bool isFetchAndAddNative() noexcept { return Ops::isFetchAndAddNative(); }
    static constexpr bool isFetchAndAddWaitFree() noexcept { return Ops::isFetchAndAddWaitFree(); }

    Type fetchAndAddRelaxed(qptrdiff valueToAdd) noexcept
    { return Ops::fetchAndAddRelaxed(_q_value, valueToAdd); }
    Type fetchAndAddAcquire(qptrdiff valueToAdd) noexcept
    { return Ops::fetchAndAddAcquire(_q_value, valueToAdd); }
    Type fetchAndAddRelease(qptrdiff valueToAdd) noexcept
    { return Ops::fetchAndAddRelease(_q_value, valueToAdd); }
    Type fetchAndAddOrdered(qptrdiff valueToAdd) noexcept
    { return Ops::fetchAndAddOrdered(_q_value, valueToAdd); }

#ifdef QT_BASIC_ATOMIC_HAS_CONSTRUCTORS
    QBasicAtomicPointer() = default;
    constexpr QBasicAtomicPointer(Type value) noexcept : _q_value(value) {}
    QBasicAtomicPointer(const QBasicAtomicPointer &) = delete;
    QBasicAtomicPointer &operator=(const QBasicAtomicPointer &) = delete;
    QBasicAtomicPointer &operator=(const QBasicAtomicPointer &) volatile = delete;
#endif
};

#ifndef Q_BASIC_ATOMIC_INITIALIZER
#  define Q_BASIC_ATOMIC_INITIALIZER(a) { (a) }
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QOLDBASICATOMIC_H

#endif // QBASICATOMIC_H

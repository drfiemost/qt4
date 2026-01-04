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

#ifndef QGENERICATOMIC_H
#define QGENERICATOMIC_H

#include <QtCore/qglobal.h>
#include <QtCore/qtypeinfo.h>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

#if 0
// silence syncqt warnings
QT_END_NAMESPACE
QT_END_HEADER

#pragma qt_sync_skip_header_check
#pragma qt_sync_stop_processing
#endif

#ifdef Q_CC_GNU
// lowercase is fine, we'll undef it below
#define always_inline __attribute__((always_inline, gnu_inline))
#else
#define always_inline
#endif

template<int> struct QAtomicOpsSupport { enum { IsSupported = 0 }; };
template<> struct QAtomicOpsSupport<4> { enum { IsSupported = 1 }; };

template <typename T> struct QAtomicAdditiveType
{
    typedef T AdditiveT;
    static const int AddScale = 1;
};
template <typename T> struct QAtomicAdditiveType<T *>
{
    typedef qptrdiff AdditiveT;
    static const int AddScale = sizeof(T);
};

// not really atomic...
template <typename BaseClass> struct QGenericAtomicOps
{
    template <typename T> struct AtomicType { typedef T Type; typedef T *PointerType; };

    template <typename T> static void acquireMemoryFence(const T &_q_value) noexcept
    {
        BaseClass::orderedMemoryFence(_q_value);
    }
    template <typename T> static void releaseMemoryFence(const T &_q_value) noexcept
    {
        BaseClass::orderedMemoryFence(_q_value);
    }
    template <typename T> static void orderedMemoryFence(const T &) noexcept
    {
    }

    template <typename T> static inline always_inline
    T load(T &_q_value) noexcept
    {
        return _q_value;
    }

    template <typename T> static inline always_inline
    void store(T &_q_value, T newValue) noexcept
    {
        _q_value = newValue;
    }

    template <typename T> static inline always_inline
    T loadRelaxed(const T &_q_value) noexcept
    {
        return _q_value;
    }

    template <typename T, typename X> static inline always_inline
    void storeRelaxed(T &_q_value, X newValue) noexcept
    {
        _q_value = newValue;
    }

    template <typename T> static inline always_inline
    T loadAcquire(T &_q_value) noexcept
    {
        T tmp = *static_cast<volatile T *>(&_q_value);
        BaseClass::acquireMemoryFence(_q_value);
        return tmp;
    }

    template <typename T> static inline always_inline
    void storeRelease(T &_q_value, T newValue) noexcept
    {
        BaseClass::releaseMemoryFence(_q_value);
        *static_cast<volatile T *>(&_q_value) = newValue;
    }

    static inline constexpr bool isReferenceCountingNative() noexcept
    { return BaseClass::isFetchAndAddNative(); }
    static inline constexpr bool isReferenceCountingWaitFree() noexcept
    { return BaseClass::isFetchAndAddWaitFree(); }
    template <typename T> static inline always_inline
    bool ref(T &_q_value) noexcept
    {
        return BaseClass::fetchAndAddRelaxed(_q_value, 1) != T(-1);
    }

    template <typename T> static inline always_inline
    bool deref(T &_q_value) noexcept
    {
         return BaseClass::fetchAndAddRelaxed(_q_value, -1) != 1;
    }

#if 0
    // These functions have no default implementation
    // Archictectures must implement them
    static inline constexpr bool isTestAndSetNative() noexcept;
    static inline constexpr bool isTestAndSetWaitFree() noexcept;
    template <typename T> static inline
    bool testAndSetRelaxed(T &_q_value, T expectedValue, T newValue) noexcept;
    template <typename T, typename X> static inline
    bool testAndSetRelaxed(T &_q_value, X expectedValue, X newValue, X *currentValue) noexcept;
#endif

    template <typename T> static inline always_inline
    bool testAndSetAcquire(T &_q_value, T expectedValue, T newValue) noexcept
    {
        bool tmp = BaseClass::testAndSetRelaxed(_q_value, expectedValue, newValue);
        BaseClass::acquireMemoryFence(_q_value);
        return tmp;
    }

    template <typename T> static inline always_inline
    bool testAndSetRelease(T &_q_value, T expectedValue, T newValue) noexcept
    {
        BaseClass::releaseMemoryFence(_q_value);
        return BaseClass::testAndSetRelaxed(_q_value, expectedValue, newValue);
    }

    template <typename T> static inline always_inline
    bool testAndSetOrdered(T &_q_value, T expectedValue, T newValue) noexcept
    {
        BaseClass::orderedMemoryFence(_q_value);
        return BaseClass::testAndSetRelaxed(_q_value, expectedValue, newValue);
    }

    template <typename T, typename X> static inline always_inline
    bool testAndSetAcquire(T &_q_value, X expectedValue, X newValue, X *currentValue) noexcept
    {
        bool tmp = BaseClass::testAndSetRelaxed(_q_value, expectedValue, newValue, currentValue);
        BaseClass::acquireMemoryFence(_q_value);
        return tmp;
    }

    template <typename T, typename X> static inline always_inline
    bool testAndSetRelease(T &_q_value, X expectedValue, X newValue, X *currentValue) noexcept
    {
        BaseClass::releaseMemoryFence(_q_value);
        return BaseClass::testAndSetRelaxed(_q_value, expectedValue, newValue, currentValue);
    }

    template <typename T, typename X> static inline always_inline
    bool testAndSetOrdered(T &_q_value, X expectedValue, X newValue, X *currentValue) noexcept
    {
        BaseClass::orderedMemoryFence(_q_value);
        return BaseClass::testAndSetRelaxed(_q_value, expectedValue, newValue, currentValue);
    }

    static inline constexpr bool isFetchAndStoreNative() noexcept { return false; }
    static inline constexpr bool isFetchAndStoreWaitFree() noexcept { return false; }

    template <typename T> static inline always_inline
    T fetchAndStoreRelaxed(T &_q_value, T newValue) noexcept
    {
        // implement fetchAndStore on top of testAndSet
        for(;;) {
            T tmp = loadRelaxed(_q_value);
            if (BaseClass::testAndSetRelaxed(_q_value, tmp, newValue))
                return tmp;
        }
    }

    template <typename T> static inline always_inline
    T fetchAndStoreAcquire(T &_q_value, T newValue) noexcept
    {
        T tmp = BaseClass::fetchAndStoreRelaxed(_q_value, newValue);
        BaseClass::acquireMemoryFence(_q_value);
        return tmp;
    }

    template <typename T> static inline always_inline
    T fetchAndStoreRelease(T &_q_value, T newValue) noexcept
    {
        BaseClass::releaseMemoryFence(_q_value);
        return BaseClass::fetchAndStoreRelaxed(_q_value, newValue);
    }

    template <typename T> static inline always_inline
    T fetchAndStoreOrdered(T &_q_value, T newValue) noexcept
    {
        BaseClass::orderedMemoryFence(_q_value);
        return BaseClass::fetchAndStoreRelaxed(_q_value, newValue);
    }

    static inline constexpr bool isFetchAndAddNative() noexcept { return false; }
    static inline constexpr bool isFetchAndAddWaitFree() noexcept { return false; }
    template <typename T> static inline always_inline
    T fetchAndAddRelaxed(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd) noexcept
    {
        // implement fetchAndAdd on top of testAndSet
        for(;;) {
            T tmp = BaseClass::loadRelaxed(_q_value);
            if (BaseClass::testAndSetRelaxed(_q_value, tmp, T(tmp + valueToAdd)))
                return tmp;
        }
    }

    template <typename T> static inline always_inline
    T fetchAndAddAcquire(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd) noexcept
    {
        T tmp = BaseClass::fetchAndAddRelaxed(_q_value, valueToAdd);
        BaseClass::acquireMemoryFence(_q_value);
        return tmp;
    }

    template <typename T> static inline always_inline
    T fetchAndAddRelease(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd) noexcept
    {
        BaseClass::releaseMemoryFence(_q_value);
        return BaseClass::fetchAndAddRelaxed(_q_value, valueToAdd);
    }

    template <typename T> static inline always_inline
    T fetchAndAddOrdered(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd) noexcept
    {
        BaseClass::orderedMemoryFence(_q_value);
        return BaseClass::fetchAndAddRelaxed(_q_value, valueToAdd);
    }

    template <typename T> static inline always_inline
    T fetchAndSubRelaxed(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT operand) noexcept
    {
        // implement fetchAndSub on top of testAndSet
        for(;;) {
            T tmp = BaseClass::load(_q_value);
            if (BaseClass::testAndSetRelaxed(_q_value, tmp, T(tmp - operand)))
                return tmp;
        }
    }

    template <typename T> static inline always_inline
    T fetchAndSubAcquire(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT operand) noexcept
    {
        T tmp = BaseClass::fetchAndSubRelaxed(_q_value, operand);
        BaseClass::acquireMemoryFence(_q_value);
        return tmp;
    }

    template <typename T> static inline always_inline
    T fetchAndSubRelease(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT operand) noexcept
    {
        BaseClass::releaseMemoryFence(_q_value);
        return BaseClass::fetchAndSubRelaxed(_q_value, operand);
    }

    template <typename T> static inline always_inline
    T fetchAndSubOrdered(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT operand) noexcept
    {
        BaseClass::orderedMemoryFence(_q_value);
        return BaseClass::fetchAndSubRelaxed(_q_value, operand);
    }

    template <typename T> static inline always_inline
    T fetchAndAndRelaxed(T &_q_value, typename std::enable_if<QTypeInfo<T>::isIntegral, T>::type operand) noexcept
    {
        // implement fetchAndAnd on top of testAndSet
        for(;;) {
            T tmp = BaseClass::loadRelaxed(_q_value);
            if (BaseClass::testAndSetRelaxed(_q_value, tmp, T(tmp & operand)))
                return tmp;
        }
    }

    template <typename T> static inline always_inline
    T fetchAndAndAcquire(T &_q_value, typename std::enable_if<QTypeInfo<T>::isIntegral, T>::type operand) noexcept
    {
        T tmp = BaseClass::fetchAndAndRelaxed(_q_value, operand);
        BaseClass::acquireMemoryFence(_q_value);
        return tmp;
    }

    template <typename T> static inline always_inline
    T fetchAndAndRelease(T &_q_value, typename std::enable_if<QTypeInfo<T>::isIntegral, T>::type operand) noexcept
    {
        BaseClass::releaseMemoryFence(_q_value);
        return BaseClass::fetchAndAndRelaxed(_q_value, operand);
    }

    template <typename T> static inline always_inline
    T fetchAndAndOrdered(T &_q_value, typename std::enable_if<QTypeInfo<T>::isIntegral, T>::type operand) noexcept
    {
        BaseClass::orderedMemoryFence(_q_value);
        return BaseClass::fetchAndAndRelaxed(_q_value, operand);
    }

    template <typename T> static inline always_inline
    T fetchAndOrRelaxed(T &_q_value, typename std::enable_if<QTypeInfo<T>::isIntegral, T>::type operand) noexcept
    {
        // implement fetchAndOr on top of testAndSet
        for(;;) {
            T tmp = BaseClass::loadRelaxed(_q_value);
            if (BaseClass::testAndSetRelaxed(_q_value, tmp, T(tmp | operand)))
                return tmp;
        }
    }

    template <typename T> static inline always_inline
    T fetchAndOrAcquire(T &_q_value, typename std::enable_if<QTypeInfo<T>::isIntegral, T>::type operand) noexcept
    {
        T tmp = BaseClass::fetchAndOrRelaxed(_q_value, operand);
        BaseClass::acquireMemoryFence(_q_value);
        return tmp;
    }

    template <typename T> static inline always_inline
    T fetchAndOrRelease(T &_q_value, typename std::enable_if<QTypeInfo<T>::isIntegral, T>::type operand) noexcept
    {
        BaseClass::releaseMemoryFence(_q_value);
        return BaseClass::fetchAndOrRelaxed(_q_value, operand);
    }

    template <typename T> static inline always_inline
    T fetchAndOrOrdered(T &_q_value, typename std::enable_if<QTypeInfo<T>::isIntegral, T>::type operand) noexcept
    {
        BaseClass::orderedMemoryFence(_q_value);
        return BaseClass::fetchAndOrRelaxed(_q_value, operand);
    }

    template <typename T> static inline always_inline
    T fetchAndXorRelaxed(T &_q_value, typename std::enable_if<QTypeInfo<T>::isIntegral, T>::type operand) noexcept
    {
        // implement fetchAndXor on top of testAndSet
        for(;;) {
            T tmp = BaseClass::loadRelaxed(_q_value);
            if (BaseClass::testAndSetRelaxed(_q_value, tmp, T(tmp ^ operand)))
                return tmp;
        }
    }

    template <typename T> static inline always_inline
    T fetchAndXorAcquire(T &_q_value, typename std::enable_if<QTypeInfo<T>::isIntegral, T>::type operand) noexcept
    {
        T tmp = BaseClass::fetchAndXorRelaxed(_q_value, operand);
        BaseClass::acquireMemoryFence(_q_value);
        return tmp;
    }

    template <typename T> static inline always_inline
    T fetchAndXorRelease(T &_q_value, typename std::enable_if<QTypeInfo<T>::isIntegral, T>::type operand) noexcept
    {
        BaseClass::releaseMemoryFence(_q_value);
        return BaseClass::fetchAndXorRelaxed(_q_value, operand);
    }

    template <typename T> static inline always_inline
    T fetchAndXorOrdered(T &_q_value, typename std::enable_if<QTypeInfo<T>::isIntegral, T>::type operand) noexcept
    {
        BaseClass::orderedMemoryFence(_q_value);
        return BaseClass::fetchAndXorRelaxed(_q_value, operand);
    }
};

#undef always_inline

QT_END_NAMESPACE
QT_END_HEADER

#endif // QGENERICATOMIC_H

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

#ifndef QATOMIC_IA64_H
#define QATOMIC_IA64_H

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

#define Q_ATOMIC_INT_REFERENCE_COUNTING_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT_REFERENCE_COUNTING_IS_WAIT_FREE

inline bool QBasicAtomicInt::isReferenceCountingNative()
{ return true; }
inline bool QBasicAtomicInt::isReferenceCountingWaitFree()
{ return true; }

#define Q_ATOMIC_INT_TEST_AND_SET_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT_TEST_AND_SET_IS_WAIT_FREE

inline bool QBasicAtomicInt::isTestAndSetNative()
{ return true; }
inline bool QBasicAtomicInt::isTestAndSetWaitFree()
{ return true; }

#define Q_ATOMIC_INT_FETCH_AND_STORE_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT_FETCH_AND_STORE_IS_WAIT_FREE

inline bool QBasicAtomicInt::isFetchAndStoreNative()
{ return true; }
inline bool QBasicAtomicInt::isFetchAndStoreWaitFree()
{ return true; }

#define Q_ATOMIC_INT_FETCH_AND_ADD_IS_ALWAYS_NATIVE

inline bool QBasicAtomicInt::isFetchAndAddNative()
{ return true; }
inline bool QBasicAtomicInt::isFetchAndAddWaitFree()
{ return false; }

#define Q_ATOMIC_POINTER_TEST_AND_SET_IS_ALWAYS_NATIVE
#define Q_ATOMIC_POINTER_TEST_AND_SET_IS_WAIT_FREE

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isTestAndSetNative()
{ return true; }
template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isTestAndSetWaitFree()
{ return true; }

#define Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_ALWAYS_NATIVE
#define Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_WAIT_FREE

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndStoreNative()
{ return true; }
template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndStoreWaitFree()
{ return true; }

#define Q_ATOMIC_POINTER_FETCH_AND_ADD_IS_ALWAYS_NATIVE

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndAddNative()
{ return true; }
template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndAddWaitFree()
{ return false; }

inline bool _q_ia64_fetchadd_immediate(int value)
{
    return value == 1 || value == -1
        || value == 4 || value == -4
        || value == 8 || value == -8
        || value == 16 || value == -16;
}

#if defined(Q_CC_INTEL)

// intrinsics provided by the Intel C++ Compiler
#include <ia64intrin.h>

inline int QBasicAtomicInt::fetchAndStoreAcquire(int newValue)
{
    return static_cast<int>(_InterlockedExchange(&_q_value, newValue));
}

inline int QBasicAtomicInt::fetchAndStoreRelease(int newValue)
{
    __memory_barrier();
    return static_cast<int>(_InterlockedExchange(&_q_value, newValue));
}

inline bool QBasicAtomicInt::testAndSetRelaxed(int expectedValue, int newValue)
{
    int expectedValueCopy = expectedValue;
    return (static_cast<int>(_InterlockedCompareExchange(&_q_value, 
							 newValue, 
							 expectedValueCopy))
	    == expectedValue);
}

inline bool QBasicAtomicInt::testAndSetAcquire(int expectedValue, int newValue)
{
    int expectedValueCopy = expectedValue;
    return (static_cast<int>(_InterlockedCompareExchange_acq(reinterpret_cast<volatile uint *>(&_q_value), 
							     newValue, 
							     expectedValueCopy)) 
	    == expectedValue);
}

inline bool QBasicAtomicInt::testAndSetRelease(int expectedValue, int newValue)
{
    int expectedValueCopy = expectedValue;
    return (static_cast<int>(_InterlockedCompareExchange_rel(reinterpret_cast<volatile uint *>(&_q_value), 
							     newValue, 
							     expectedValueCopy)) 
	    == expectedValue);
}

inline bool QBasicAtomicInt::testAndSetOrdered(int expectedValue, int newValue)
{
    __memory_barrier();
    return testAndSetAcquire(expectedValue, newValue);
}

inline int QBasicAtomicInt::fetchAndAddAcquire(int valueToAdd)
{
    if (__builtin_constant_p(valueToAdd)) {
        if (valueToAdd == 1)
            return __fetchadd4_acq((unsigned int *)&_q_value, 1);
        if (valueToAdd == -1)
            return __fetchadd4_acq((unsigned int *)&_q_value, -1);
    }
    return _InterlockedExchangeAdd(&_q_value, valueToAdd);
}

inline int QBasicAtomicInt::fetchAndAddRelease(int valueToAdd)
{
    if (__builtin_constant_p(valueToAdd)) {
        if (valueToAdd == 1)
            return __fetchadd4_rel((unsigned int *)&_q_value, 1);
        if (valueToAdd == -1)
            return __fetchadd4_rel((unsigned int *)&_q_value, -1);
    }
    __memory_barrier();
    return _InterlockedExchangeAdd(&_q_value, valueToAdd);
}

inline int QBasicAtomicInt::fetchAndAddOrdered(int valueToAdd)
{
    __memory_barrier();
    return fetchAndAddAcquire(valueToAdd);
}

inline bool QBasicAtomicInt::ref()
{
    return _InterlockedIncrement(&_q_value) != 0;
}

inline bool QBasicAtomicInt::deref()
{
    return _InterlockedDecrement(&_q_value) != 0;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreAcquire(T *newValue)
{
    return (T *)_InterlockedExchangePointer(reinterpret_cast<void * volatile*>(&_q_value), newValue);
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreRelease(T *newValue)
{
    __memory_barrier();
    return fetchAndStoreAcquire(newValue);
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetRelaxed(T *expectedValue, T *newValue)
{
    T *expectedValueCopy = expectedValue;
    return (_InterlockedCompareExchangePointer(reinterpret_cast<void * volatile*>(&_q_value), 
					       newValue, 
					       expectedValueCopy)
	    == expectedValue);
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetAcquire(T *expectedValue, T *newValue)
{
    union {
        volatile void *x;
        volatile unsigned long *p;
    };
    x = &_q_value;
    T *expectedValueCopy = expectedValue;
    return (_InterlockedCompareExchange64_acq(p, quintptr(newValue), quintptr(expectedValueCopy)) 
	    == quintptr(expectedValue));
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetRelease(T *expectedValue, T *newValue)
{
    union {
        volatile void *x;
        volatile unsigned long *p;
    };
    x = &_q_value;
    T *expectedValueCopy = expectedValue;
    return (_InterlockedCompareExchange64_rel(p, quintptr(newValue), quintptr(expectedValueCopy))
	    == quintptr(expectedValue));
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetOrdered(T *expectedValue, T *newValue)
{
    __memory_barrier();
    return testAndSetAcquire(expectedValue, newValue);
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddAcquire(qptrdiff valueToAdd)
{
    return (T *)_InterlockedExchangeAdd64((volatile long *)&_q_value,
                                          valueToAdd * sizeof(T));
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddRelease(qptrdiff valueToAdd)
{
    __memory_barrier();
    return (T *)_InterlockedExchangeAdd64((volatile long *)&_q_value,
                                          valueToAdd * sizeof(T));
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddOrdered(qptrdiff valueToAdd)
{
    __memory_barrier();
    return fetchAndAddAcquire(valueToAdd);
}

#else // !Q_CC_INTEL

#  if defined(Q_CC_GNU)

inline int QBasicAtomicInt::fetchAndStoreAcquire(int newValue)
{
    int ret;
    asm volatile("xchg4 %0=%1,%2\n"
                 : "=r" (ret), "+m" (_q_value)
                 : "r" (newValue)
                 : "memory");
    return ret;
}

inline int QBasicAtomicInt::fetchAndStoreRelease(int newValue)
{
    int ret;
    asm volatile("mf\n"
                 "xchg4 %0=%1,%2\n"
                 : "=r" (ret), "+m" (_q_value)
                 : "r" (newValue)
                 : "memory");
    return ret;
}

inline bool QBasicAtomicInt::testAndSetAcquire(int expectedValue, int newValue)
{
    int ret;
    asm volatile("mov ar.ccv=%2\n"
                 ";;\n"
                 "cmpxchg4.acq %0=%1,%3,ar.ccv\n"
                 : "=r" (ret), "+m" (_q_value)
                 : "r" (expectedValue), "r" (newValue)
                 : "memory");
    return ret == expectedValue;
}

inline bool QBasicAtomicInt::testAndSetRelease(int expectedValue, int newValue)
{
    int ret;
    asm volatile("mov ar.ccv=%2\n"
                 ";;\n"
                 "cmpxchg4.rel %0=%1,%3,ar.ccv\n"
                 : "=r" (ret), "+m" (_q_value)
                 : "r" (expectedValue), "r" (newValue)
                 : "memory");
    return ret == expectedValue;
}

inline int QBasicAtomicInt::fetchAndAddAcquire(int valueToAdd)
{
    int ret;

#if (__GNUC__ >= 4)
    // We implement a fast fetch-and-add when we can
    if (__builtin_constant_p(valueToAdd) && _q_ia64_fetchadd_immediate(valueToAdd)) {
        asm volatile("fetchadd4.acq  %0=%1,%2\n"
                     : "=r" (ret), "+m" (_q_value)
                     : "i" (valueToAdd)
                     : "memory");
        return ret;
    }
#endif

    // otherwise, use a loop around test-and-set
    ret = _q_value;
    asm volatile("0:\n"
                 "      mov           r9=%0\n"
                 "      mov           ar.ccv=%0\n"
                 "      add           %0=%0, %2\n"
                 "      ;;\n"
                 "      cmpxchg4.acq %0=%1,%0,ar.ccv\n"
                 "      ;;\n"
                 "      cmp.ne       p6,p0 = %0, r9\n"
                 "(p6)  br.dptk      0b\n"
                 "1:\n"
                 : "+r" (ret), "+m" (_q_value)
                 : "r" (valueToAdd)
                 : "r9", "p6", "memory");
    return ret;
}

inline int QBasicAtomicInt::fetchAndAddRelease(int valueToAdd)
{
    int ret;

#if (__GNUC__ >= 4)
    // We implement a fast fetch-and-add when we can
    if (__builtin_constant_p(valueToAdd) && _q_ia64_fetchadd_immediate(valueToAdd)) {
        asm volatile("fetchadd4.rel  %0=%1,%2\n"
                     : "=r" (ret), "+m" (_q_value)
                     : "i" (valueToAdd)
                     : "memory");
        return ret;
    }
#endif

    // otherwise, use a loop around test-and-set
    ret = _q_value;
    asm volatile("0:\n"
                 "      mov           r9=%0\n"
                 "      mov           ar.ccv=%0\n"
                 "      add           %0=%0, %2\n"
                 "      ;;\n"
                 "      cmpxchg4.rel %0=%1,%0,ar.ccv\n"
                 "      ;;\n"
                 "      cmp.ne       p6,p0 = %0, r9\n"
                 "(p6)  br.dptk      0b\n"
                 "1:\n"
                 : "+r" (ret), "+m" (_q_value)
                 : "r" (valueToAdd)
                 : "r9", "p6", "memory");
    return ret;
}

inline int QBasicAtomicInt::fetchAndAddOrdered(int valueToAdd)
{
    asm volatile("mf" ::: "memory");
    return fetchAndAddRelease(valueToAdd);
}

inline bool QBasicAtomicInt::ref()
{
    int ret;
    asm volatile("fetchadd4.acq %0=%1,1\n"
                 : "=r" (ret), "+m" (_q_value)
                 :
                 : "memory");
    return ret != -1;
}

inline bool QBasicAtomicInt::deref()
{
    int ret;
    asm volatile("fetchadd4.rel %0=%1,-1\n"
                 : "=r" (ret), "+m" (_q_value)
                 :
                 : "memory");
    return ret != 1;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreAcquire(T *newValue)
{
    T *ret;
    asm volatile("xchg8 %0=%1,%2\n"
                 : "=r" (ret), "+m" (_q_value)
                 : "r" (newValue)
                 : "memory");
    return ret;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreRelease(T *newValue)
{
    T *ret;
    asm volatile("mf\n"
                 "xchg8 %0=%1,%2\n"
                 : "=r" (ret), "+m" (_q_value)
                 : "r" (newValue)
                 : "memory");
    return ret;
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetAcquire(T *expectedValue, T *newValue)
{
    T *ret;
    asm volatile("mov ar.ccv=%2\n"
                 ";;\n"
                 "cmpxchg8.acq %0=%1,%3,ar.ccv\n"
                 : "=r" (ret), "+m" (_q_value)
                 : "r" (expectedValue), "r" (newValue)
                 : "memory");
    return ret == expectedValue;
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetRelease(T *expectedValue, T *newValue)
{
    T *ret;
    asm volatile("mov ar.ccv=%2\n"
                 ";;\n"
                 "cmpxchg8.rel %0=%1,%3,ar.ccv\n"
                 : "=r" (ret), "+m" (_q_value)
                 : "r" (expectedValue), "r" (newValue)
                 : "memory");
    return ret == expectedValue;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddAcquire(qptrdiff valueToAdd)
{
    T *ret;

#if (__GNUC__ >= 4)
    // We implement a fast fetch-and-add when we can
    if (__builtin_constant_p(valueToAdd) && _q_ia64_fetchadd_immediate(valueToAdd * sizeof(T))) {
        asm volatile("fetchadd8.acq  %0=%1,%2\n"
                     : "=r" (ret), "+m" (_q_value)
                     : "i" (valueToAdd * sizeof(T))
                     : "memory");
        return ret;
    }
#endif

    // otherwise, use a loop around test-and-set
    ret = _q_value;
    asm volatile("0:\n"
                 "      mov           r9=%0\n"
                 "      mov           ar.ccv=%0\n"
                 "      add           %0=%0, %2\n"
                 "      ;;\n"
                 "      cmpxchg8.acq %0=%1,%0,ar.ccv\n"
                 "      ;;\n"
                 "      cmp.ne       p6,p0 = %0, r9\n"
                 "(p6)  br.dptk      0b\n"
                 "1:\n"
                 : "+r" (ret), "+m" (_q_value)
                 : "r" (valueToAdd * sizeof(T))
                 : "r9", "p6", "memory");
    return ret;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddRelease(qptrdiff valueToAdd)
{
    T *ret;

#if (__GNUC__ >= 4)
    // We implement a fast fetch-and-add when we can
    if (__builtin_constant_p(valueToAdd) && _q_ia64_fetchadd_immediate(valueToAdd * sizeof(T))) {
        asm volatile("fetchadd8.rel  %0=%1,%2\n"
                     : "=r" (ret), "+m" (_q_value)
                     : "i" (valueToAdd * sizeof(T))
                     : "memory");
        return ret;
    }
#endif

    // otherwise, use a loop around test-and-set
    ret = _q_value;
    asm volatile("0:\n"
                 "      mov           r9=%0\n"
                 "      mov           ar.ccv=%0\n"
                 "      add           %0=%0, %2\n"
                 "      ;;\n"
                 "      cmpxchg8.rel %0=%1,%0,ar.ccv\n"
                 "      ;;\n"
                 "      cmp.ne       p6,p0 = %0, r9\n"
                 "(p6)  br.dptk      0b\n"
                 "1:\n"
                 : "+r" (ret), "+m" (_q_value)
                 : "r" (valueToAdd * sizeof(T))
                 : "r9", "p6", "memory");
    return ret;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddOrdered(qptrdiff valueToAdd)
{
    asm volatile("mf" ::: "memory");
    return fetchAndAddRelease(valueToAdd);
}

#else

extern "C" {
    Q_CORE_EXPORT int q_atomic_test_and_set_int(volatile int *ptr, int expected, int newval);
    Q_CORE_EXPORT int q_atomic_test_and_set_ptr(volatile void *ptr, void *expected, void *newval);
} // extern "C"

#endif

inline bool QBasicAtomicInt::testAndSetRelaxed(int expectedValue, int newValue)
{
    return testAndSetAcquire(expectedValue, newValue);
}

inline bool QBasicAtomicInt::testAndSetOrdered(int expectedValue, int newValue)
{
    return testAndSetAcquire(expectedValue, newValue);
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetRelaxed(T *expectedValue, T *newValue)
{
    return testAndSetAcquire(expectedValue, newValue);
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetOrdered(T *expectedValue, T *newValue)
{
    return testAndSetAcquire(expectedValue, newValue);
}

#endif // Q_CC_INTEL

inline int QBasicAtomicInt::fetchAndStoreRelaxed(int newValue)
{
    return fetchAndStoreAcquire(newValue);
}

inline int QBasicAtomicInt::fetchAndStoreOrdered(int newValue)
{
    return fetchAndStoreRelease(newValue);
}

inline int QBasicAtomicInt::fetchAndAddRelaxed(int valueToAdd)
{
    return fetchAndAddAcquire(valueToAdd);
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreRelaxed(T *newValue)
{
    return fetchAndStoreAcquire(newValue);
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreOrdered(T *newValue)
{
    return fetchAndStoreRelaxed(newValue);
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddRelaxed(qptrdiff valueToAdd)
{
    return fetchAndAddAcquire(valueToAdd);
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QATOMIC_IA64_H

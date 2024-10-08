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

#include "qatomic.h"

/*!
    \class QAtomicInt
    \brief The QAtomicInt class provides platform-independent atomic operations on int.
    \since 4.4
    This class is a equivalent to \c{QAtomicInteger<int>}. All other
    functionality is equivalent. Please see that class for more information.
    \sa QAtomicInteger, QAtomicPointer
*/

/*!
    \class QAtomicInteger
    \inmodule QtCore
    \brief The QAtomicInteger class provides platform-independent atomic operations on integers.
    \ingroup thread

    For atomic operations on pointers, see the QAtomicPointer class.

    An \e atomic operation is a complex operation that completes without interruption. 
    The QAtomicInteger class provides atomic reference counting, test-and-set, fetch-and-store,
    and fetch-and-add for integers.

    The template parameter \c T must be a C++ integer type:
    \list
       \li 8-bit: char, signed char, unsigned char, qint8, quint8
       \li 16-bit: short, unsigned short, qint16, quint16, char16_t (C++11)
       \li 32-bit: int, unsigned int, qint32, quint32, char32_t (C++11)
       \li 64-bit: long long, unsigned long long, qint64, quint64
       \li platform-specific size: long, unsigned long
       \li pointer size: qintptr, quintptr, qptrdiff
    \endlist
    Of the list above, only the 32-bit- and pointer-sized instantiations are guaranteed to
    work on all platforms. Support for other sizes depends on the compiler and
    processor architecture the code is being compiled for. To test whether the
    other types are supported, check the macro \c Q_ATOMIC_INT\e{nn}_IS_SUPPORTED,
    where \c{\e{nn}} is the number of bits desired.

    \section1 The Atomic API

    \section2 Reference counting

    The ref() and deref() functions provide an efficient reference
    counting API. The return value of these functions are used to
    indicate when the last reference has been released. These
    functions allow you to implement your own implicitly shared
    classes.

    \snippet doc/src/snippets/code/src_corelib_thread_qatomic.cpp 0

    \section2 Memory ordering

    QAtomicInteger provides several implementations of the atomic
    test-and-set, fetch-and-store, and fetch-and-add functions. Each
    implementation defines a memory ordering semantic that describes
    how memory accesses surrounding the atomic instruction are
    executed by the processor. Since many modern architectures allow
    out-of-order execution and memory ordering, using the correct
    semantic is necessary to ensure that your application functions
    properly on all processors.

    \list

    \o Relaxed - memory ordering is unspecified, leaving the compiler
    and processor to freely reorder memory accesses.

    \o Acquire - memory access following the atomic operation (in
    program order) may not be re-ordered before the atomic operation.

    \o Release - memory access before the atomic operation (in program
    order) may not be re-ordered after the atomic operation.

    \o Ordered - the same Acquire and Release semantics combined.

    \endlist

    \section2 Test-and-set

    If the current value of the QAtomicInteger is an expected value, the
    test-and-set functions assign a new value to the QAtomicInteger and
    return true. If values are \a not the same, these functions do
    nothing and return false. This operation equates to the following
    code:

    \snippet doc/src/snippets/code/src_corelib_thread_qatomic.cpp 1

    There are 4 test-and-set functions: testAndSetRelaxed(),
    testAndSetAcquire(), testAndSetRelease(), and
    testAndSetOrdered(). See above for an explanation of the different
    memory ordering semantics.

    \section2 Fetch-and-store

    The atomic fetch-and-store functions read the current value of the
    QAtomicInteger and then assign a new value, returning the original
    value. This operation equates to the following code:

    \snippet doc/src/snippets/code/src_corelib_thread_qatomic.cpp 2

    There are 4 fetch-and-store functions: fetchAndStoreRelaxed(),
    fetchAndStoreAcquire(), fetchAndStoreRelease(), and
    fetchAndStoreOrdered(). See above for an explanation of the
    different memory ordering semantics.

    \section2 Fetch-and-add

    The atomic fetch-and-add functions read the current value of the
    QAtomicInteger and then add the given value to the current value,
    returning the original value. This operation equates to the
    following code:

    \snippet doc/src/snippets/code/src_corelib_thread_qatomic.cpp 3

    There are 4 fetch-and-add functions: fetchAndAddRelaxed(),
    fetchAndAddAcquire(), fetchAndAddRelease(), and
    fetchAndAddOrdered(). See above for an explanation of the
    different memory ordering semantics.

    \section1 Feature Tests for the Atomic API

    Providing a platform-independent atomic API that works on all
    processors is challenging. The API provided by QAtomicInteger is
    guaranteed to work atomically on all processors. However, since
    not all processors implement support for every operation provided
    by QAtomicInteger, it is necessary to expose information about the
    processor.

    You can check at compile time which features are supported on your
    hardware using various macros. These will tell you if your
    hardware always, sometimes, or does not support a particular
    operation. The macros have the form
    Q_ATOMIC_INT\e{nn}_\e{OPERATION}_IS_\e{HOW}_NATIVE. \e{nn} is the
    size of the integer (in bits), \e{OPERATION}
    is one of REFERENCE_COUNTING, TEST_AND_SET,
    FETCH_AND_STORE, or FETCH_AND_ADD, and \e{HOW} is one of
    ALWAYS, SOMETIMES, or NOT. There will always be exactly one
    defined macro per operation. For example, if
    Q_ATOMIC_INT32_REFERENCE_COUNTING_IS_ALWAYS_NATIVE is defined,
    neither Q_ATOMIC_INT_REFERENCE_COUNTING_IS_SOMETIMES_NATIVE nor
    Q_ATOMIC_INT32_REFERENCE_COUNTING_IS_NOT_NATIVE will be defined.

    An operation that completes in constant time is said to be
    wait-free. Such operations are not implemented using locks or
    loops of any kind. For atomic operations that are always
    supported, and that are wait-free, Qt defines the
    Q_ATOMIC_INT\e{nn}_\e{OPERATION}_IS_WAIT_FREE in addition to the
    Q_ATOMIC_INT\e{nn}_\e{OPERATION}_IS_ALWAYS_NATIVE.

    In cases where an atomic operation is only supported in newer
    generations of the processor, QAtomicInteger also provides a way to
    check at runtime what your hardware supports with the
    isReferenceCountingNative(), isTestAndSetNative(),
    isFetchAndStoreNative(), and isFetchAndAddNative()
    functions. Wait-free implementations can be detected using the
    isReferenceCountingWaitFree(), isTestAndSetWaitFree(),
    isFetchAndStoreWaitFree(), and isFetchAndAddWaitFree() functions.

    Below is a complete list of all feature macros for QAtomicInteger:

    \list

    \li Q_ATOMIC_INT\e{nn}_REFERENCE_COUNTING_IS_ALWAYS_NATIVE
    \li Q_ATOMIC_INT\e{nn}_REFERENCE_COUNTING_IS_SOMETIMES_NATIVE
    \li Q_ATOMIC_INT\e{nn}_REFERENCE_COUNTING_IS_NOT_NATIVE
    \li Q_ATOMIC_INT\e{nn}_REFERENCE_COUNTING_IS_WAIT_FREE

    \li Q_ATOMIC_INT\e{nn}_TEST_AND_SET_IS_ALWAYS_NATIVE
    \li Q_ATOMIC_INT\e{nn}_TEST_AND_SET_IS_SOMETIMES_NATIVE
    \li Q_ATOMIC_INT\e{nn}_TEST_AND_SET_IS_NOT_NATIVE
    \li Q_ATOMIC_INT\e{nn}_TEST_AND_SET_IS_WAIT_FREE

    \li Q_ATOMIC_INT\e{nn}_FETCH_AND_STORE_IS_ALWAYS_NATIVE
    \li Q_ATOMIC_INT\e{nn}_FETCH_AND_STORE_IS_SOMETIMES_NATIVE
    \li Q_ATOMIC_INT\e{nn}_FETCH_AND_STORE_IS_NOT_NATIVE
    \li Q_ATOMIC_INT\e{nn}_FETCH_AND_STORE_IS_WAIT_FREE

    \li Q_ATOMIC_INT\e{nn}_FETCH_AND_ADD_IS_ALWAYS_NATIVE
    \li Q_ATOMIC_INT\e{nn}_FETCH_AND_ADD_IS_SOMETIMES_NATIVE
    \li Q_ATOMIC_INT\e{nn}_FETCH_AND_ADD_IS_NOT_NATIVE
    \li Q_ATOMIC_INT\e{nn}_FETCH_AND_ADD_IS_WAIT_FREE

    \endlist

    \sa QAtomicPointer
*/

/*! \fn QAtomicInt::QAtomicInt(int value)

    Constructs a QAtomicInt with the given \a value.
*/

/*! \fn QAtomicInteger::QAtomicInteger(T value)
    Constructs a QAtomicInteger with the given \a value.
*/

/*! \fn QAtomicInteger::QAtomicInteger(const QAtomicInteger &other)

    Constructs a copy of \a other.
*/

/*! \fn QAtomicInt &QAtomicInt::operator=(int value)

    Assigns the \a value to this QAtomicInt and returns a reference to
    this QAtomicInt.
*/

/*! \fn QAtomicInteger &QAtomicInteger::operator=(const QAtomicInteger &other)

    Assigns \a other to this QAtomicInteger and returns a reference to
    this QAtomicInteger.
*/

/*! \fn bool QAtomicInt::operator==(int value) const

    Returns true if the \a value is equal to the value in this
    QAtomicInt; otherwise returns false.
*/

/*! \fn bool QAtomicInt::operator!=(int value) const

    Returns true if the value of this QAtomicInt is not equal to \a
    value; otherwise returns false.
*/

/*! \fn bool QAtomicInt::operator!() const

    Returns true is the value of this QAtomicInt is zero; otherwise
    returns false.
*/

/*! \fn QAtomicInt::operator int() const

    Returns the value stored by the QAtomicInt object as an integer.
*/

/*! 
    \fn T QAtomicInteger::load() const
    \obsolete
    Use loadRelaxed() instead.

    Atomically loads the value of this QAtomicInteger using relaxed memory
    ordering. The value is not modified in any way, but note that there's no
    guarantee that it remains so.
    \sa storeRelaxed(), loadAcquire()
*/

/*!
    \fn template <typename T> T QAtomicInteger<T>::loadRelaxed() const

    Atomically loads the value of this QAtomicInteger using relaxed memory
    ordering. The value is not modified in any way, but note that there's no
    guarantee that it remains so.
    \sa storeRelaxed(), loadAcquire()
*/

/*!
    \fn T QAtomicInteger::loadAcquire() const

    Atomically loads the value of this QAtomicInteger using the "Acquire" memory
    ordering. The value is not modified in any way, but note that there's no
    guarantee that it remains so.
    \sa storeRelaxed(), loadRelaxed()
*/

/*!
    \fn void QAtomicInteger::store(T newValue)
    \obsolete
    Use storeRelaxed() instead.

    Atomically stores the \a newValue value into this atomic type, using
    relaxed memory ordering.
    \sa storeRelease(), loadRelaxed()
*/

/*!
    \fn template <typename T> void QAtomicInteger<T>::storeRelaxed(T newValue)

    Atomically stores the \a newValue value into this atomic type, using
    relaxed memory ordering.
    \sa storeRelease(), loadRelaxed()
*/

/*!
    \fn void QAtomicInteger::storeRelease(T newValue)

    Atomically stores the \a newValue value into this atomic type, using
    the "Release" memory ordering.
    \sa store(), loadAcquire()
*/

/*!
    \fn QAtomicInteger::operator T() const
    Atomically loads the value of this QAtomicInteger using a sequentially
    consistent memory ordering if possible; or "Acquire" ordering if not. The
    value is not modified in any way, but note that there's no guarantee that
    it remains so.
    \sa loadRelaxed(), loadAcquire()
*/

/*!
    \fn QAtomicInteger &QAtomicInteger::operator=(T newValue)
    Atomically stores the \a newValue value into this atomic type using a
    sequentially consistent memory ordering if possible; or "Release" ordering
    if not. This function returns a reference to this object.
    \sa storeRelaxed(), storeRelease()
*/

/*!
    \fn int QAtomicInt::load() const
    Atomically loads the value of this QAtomicInt using relaxed memory
    ordering. The value is not modified in any way, but note that there's no
    guarantee that it remains so.
    \sa store(), loadAcquire()
*/

/*!
    \fn int QAtomicInt::loadAcquire() const
    Atomically loads the value of this QAtomicInt using the "Acquire" memory
    ordering. The value is not modified in any way, but note that there's no
    guarantee that it remains so.
    \sa store(), load()
*/

/*!
    \fn void QAtomicInt::store(int newValue)
    Atomically stores the \a newValue value into this atomic type, using
    relaxed memory ordering.
    \sa storeRelease(), load()
*/

/*!
    \fn void QAtomicInt::storeRelease(int newValue)
    Atomically stores the \a newValue value into this atomic type, using
    the "Release" memory ordering.
    \sa store(), load()
*/

/*! \fn bool QAtomicInteger::isReferenceCountingNative()

    Returns true if reference counting is implemented using atomic
    processor instructions, false otherwise.
*/

/*! \fn bool QAtomicInteger::isReferenceCountingWaitFree()

    Returns true if atomic reference counting is wait-free, false
    otherwise.
*/

/*! \fn bool QAtomicInteger::ref()
    Atomically increments the value of this QAtomicInteger. Returns true
    if the new value is non-zero, false otherwise.

    This function uses \e ordered \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access before and after the atomic operation (in program order)
    may not be re-ordered.

    \sa deref(), operator++()
*/

/*!
    \fn T QAtomicInteger::operator++()
    Atomically pre-increments the value of this QAtomicInteger. Returns the new
    value of this atomic.
    This function uses a sequentially consistent memory ordering if possible;
    or "Ordered" ordering if not.
    \sa ref(), operator++(int), operator--()
*/

/*!
    \fn T QAtomicInteger::operator++(int)
    Atomically post-increments the value of this QAtomicInteger. Returns the old
    value of this atomic.
    This function uses a sequentially consistent memory ordering if possible;
    or "Ordered" ordering if not.
    \sa ref(), operator++(), operator--(int)
*/

/*! \fn bool QAtomicInteger::deref()
    Atomically decrements the value of this QAtomicInteger. Returns true
    if the new value is non-zero, false otherwise.

    This function uses \e ordered \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access before and after the atomic operation (in program order)
    may not be re-ordered.

    \sa ref(), operator--()
*/

/*!
    \fn T QAtomicInteger::operator--()
    Atomically pre-decrements the value of this QAtomicInteger. Returns the new
    value of this atomic.
    This function uses a sequentially consistent memory ordering if possible;
    or "Ordered" ordering if not.
    \sa deref(), operator--(int), operator++()
*/

/*!
    \fn T QAtomicInteger::operator--(int)
    Atomically post-decrements the value of this QAtomicInteger. Returns the old
    value of this atomic.
    This function uses a sequentially consistent memory ordering if possible;
    or "Ordered" ordering if not.
    \sa deref(), operator--(), operator++(int)
*/

/*!
    \fn T *QAtomicPointer::load() const
    \obsolete
    Use loadRelaxed() instead.
    Atomically loads the value of this QAtomicPointer using relaxed memory
    ordering. The value is not modified in any way, but note that there's no
    guarantee that it remains so.
    \sa storeRelaxed(), loadAcquire()
*/

/*!
    \fn template <typename T> T *QAtomicPointer<T>::loadRelaxed() const

    Atomically loads the value of this QAtomicPointer using relaxed memory
    ordering. The value is not modified in any way, but note that there's no
    guarantee that it remains so.
    \sa storeRelaxed(), loadAcquire()
*/

/*!
    \fn T *QAtomicPointer::loadAcquire() const
    Atomically loads the value of this QAtomicPointerusing the "Acquire" memory
    ordering. The value is not modified in any way, but note that there's no
    guarantee that it remains so.
    \sa storeRelease(), loadRelaxed()
*/

/*!
    \fn void QAtomicPointer::store(T *newValue)
    \obsolete
    Use storeRelaxed() instead.
    Atomically stores the \a newValue value into this atomic type, using
    relaxed memory ordering.
    \sa storeRelease(), loadRelaxed()
*/

/*!
    \fn template <typename T> void QAtomicPointer<T>::storeRelaxed(T *newValue)

    Atomically stores the \a newValue value into this atomic type, using
    relaxed memory ordering.
    \sa storeRelease(), loadRelaxed()
*/

/*!
    \fn void QAtomicPointer::storeRelease(T *newValue)
    Atomically stores the \a newValue value into this atomic type, using
    the "Release" memory ordering.
    \sa storeRelaxed(), loadRelaxed()
*/

/*! \fn bool QAtomicInteger::isTestAndSetNative()

    Returns true if test-and-set is implemented using atomic processor
    instructions, false otherwise.
*/

/*! \fn bool QAtomicInteger::isTestAndSetWaitFree()

    Returns true if atomic test-and-set is wait-free, false otherwise.
*/

/*! \fn bool QAtomicInteger::testAndSetRelaxed(T expectedValue, T newValue)

    Atomic test-and-set.

    If the current value of this QAtomicInteger is the \a expectedValue,
    the test-and-set functions assign the \a newValue to this
    QAtomicInteger and return true. If the values are \e not the same,
    this function does nothing and returns false.

    This function uses \e relaxed \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, leaving the compiler and
    processor to freely reorder memory accesses.
*/

/*! \fn bool QAtomicInteger::testAndSetAcquire(T expectedValue, T newValue)

    Atomic test-and-set.

    If the current value of this QAtomicInteger is the \a expectedValue,
    the test-and-set functions assign the \a newValue to this
    QAtomicInteger and return true. If the values are \e not the same,
    this function does nothing and returns false.

    This function uses \e acquire \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access following the atomic operation (in program order) may not
    be re-ordered before the atomic operation.
*/

/*! \fn bool QAtomicInteger::testAndSetRelease(T expectedValue, T newValue)

    Atomic test-and-set.

    If the current value of this QAtomicInteger is the \a expectedValue,
    the test-and-set functions assign the \a newValue to this
    QAtomicInteger and return true. If the values are \e not the same,
    this function does nothing and returns false.

    This function uses \e release \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access before the atomic operation (in program order) may not be
    re-ordered after the atomic operation.
*/

/*! \fn bool QAtomicInteger::testAndSetOrdered(T expectedValue, T newValue)

    Atomic test-and-set.

    If the current value of this QAtomicInteger is the \a expectedValue,
    the test-and-set functions assign the \a newValue to this
    QAtomicInteger and return true. If the values are \e not the same,
    this function does nothing and returns false.

    This function uses \e ordered \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access before and after the atomic operation (in program order)
    may not be re-ordered.
*/

/*! \fn bool QAtomicInteger::isFetchAndStoreNative()

    Returns true if fetch-and-store is implemented using atomic
    processor instructions, false otherwise.
*/

/*! \fn bool QAtomicInteger::isFetchAndStoreWaitFree()

    Returns true if atomic fetch-and-store is wait-free, false
    otherwise.
*/

/*! \fn T QAtomicInteger::fetchAndStoreRelaxed(T newValue)

    Atomic fetch-and-store.

    Reads the current value of this QAtomicInteger and then assigns it the
    \a newValue, returning the original value.

    This function uses \e relaxed \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, leaving the compiler and
    processor to freely reorder memory accesses.
*/

/*! \fn T QAtomicInteger::fetchAndStoreAcquire(T newValue)

    Atomic fetch-and-store.

    Reads the current value of this QAtomicInteger and then assigns it the
    \a newValue, returning the original value.

    This function uses \e acquire \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access following the atomic operation (in program order) may not
    be re-ordered before the atomic operation.
*/

/*! \fn T QAtomicInteger::fetchAndStoreRelease(T newValue)

    Atomic fetch-and-store.

    Reads the current value of this QAtomicInteger and then assigns it the
    \a newValue, returning the original value.

    This function uses \e release \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access before the atomic operation (in program order) may not be
    re-ordered after the atomic operation.
*/

/*! \fn T QAtomicInteger::fetchAndStoreOrdered(T newValue)

    Atomic fetch-and-store.

    Reads the current value of this QAtomicInteger and then assigns it the
    \a newValue, returning the original value.

    This function uses \e ordered \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access before and after the atomic operation (in program order)
    may not be re-ordered.
*/

/*! \fn bool QAtomicInteger::isFetchAndAddNative()

    Returns true if fetch-and-add is implemented using atomic
    processor instructions, false otherwise.
*/

/*! \fn bool QAtomicInteger::isFetchAndAddWaitFree()

    Returns true if atomic fetch-and-add is wait-free, false
    otherwise.
*/

/*! \fn T QAtomicInteger::fetchAndAddRelaxed(T valueToAdd)

    Atomic fetch-and-add.

    Reads the current value of this QAtomicInteger and then adds
    \a valueToAdd to the current value, returning the original value.

    This function uses \e relaxed \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, leaving the compiler and
    processor to freely reorder memory accesses.

    \sa operator+=(), fetchAndSubRelaxed()
*/

/*! \fn T QAtomicInteger::fetchAndAddAcquire(T valueToAdd)

    Atomic fetch-and-add.

    Reads the current value of this QAtomicInteger and then adds
    \a valueToAdd to the current value, returning the original value.

    This function uses \e acquire \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access following the atomic operation (in program order) may not
    be re-ordered before the atomic operation.

    \sa operator+=(), fetchAndSubAcquire()
*/

/*! \fn T QAtomicInteger::fetchAndAddRelease(T valueToAdd)

    Atomic fetch-and-add.

    Reads the current value of this QAtomicInteger and then adds
    \a valueToAdd to the current value, returning the original value.

    This function uses \e release \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access before the atomic operation (in program order) may not be
    re-ordered after the atomic operation.

    \sa operator+=(), fetchAndSubRelease()
*/

/*! \fn T QAtomicInteger::fetchAndAddOrdered(T valueToAdd)

    Atomic fetch-and-add.

    Reads the current value of this QAtomicInteger and then adds
    \a valueToAdd to the current value, returning the original value.

    This function uses \e ordered \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access before and after the atomic operation (in program order)
    may not be re-ordered.

    \sa operator+=(), fetchAndSubOrdered()
*/

/*! \fn T QAtomicInteger::operator+=(T valueToAdd)

    Atomic add-and-fetch.
    Reads the current value of this QAtomicInteger and then adds
    \a valueToAdd to the current value, returning the new value value.
    This function uses a sequentially consistent memory ordering if possible;
    or "Ordered" ordering if not.
    \sa fetchAndAddOrdered(), operator-=()
*/

/*! \fn T QAtomicInteger::fetchAndSubRelaxed(T valueToSub)

    Atomic fetch-and-sub.
    Reads the current value of this QAtomicInteger and then subtracts
    \a valueToSub to the current value, returning the original value.
    This function uses \e relaxed \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, leaving the compiler and
    processor to freely reorder memory accesses.
    \sa operator-=(), fetchAndAddRelaxed()
*/

/*! \fn T QAtomicInteger::fetchAndSubAcquire(T valueToSub)

    Atomic fetch-and-sub.
    Reads the current value of this QAtomicInteger and then subtracts
    \a valueToSub to the current value, returning the original value.
    This function uses \e acquire \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access following the atomic operation (in program order) may not
    be re-ordered before the atomic operation.
    \sa operator-=(), fetchAndAddAcquire()
*/

/*! \fn T QAtomicInteger::fetchAndSubRelease(T valueToSub)
    Atomic fetch-and-sub.
    Reads the current value of this QAtomicInteger and then subtracts
    \a valueToSub to the current value, returning the original value.
    This function uses \e release \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access before the atomic operation (in program order) may not be
    re-ordered after the atomic operation.
    \sa operator-=(), fetchAndAddRelease()
*/

/*! \fn T QAtomicInteger::fetchAndSubOrdered(T valueToSub)

    Atomic fetch-and-sub.
    Reads the current value of this QAtomicInteger and then subtracts
    \a valueToSub to the current value, returning the original value.
    This function uses \e ordered \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access before and after the atomic operation (in program order)
    may not be re-ordered.
    \sa operator-=(), fetchAndAddOrdered()
*/

/*! \fn T QAtomicInteger::operator-=(T valueToSub)

    Atomic sub-and-fetch.
    Reads the current value of this QAtomicInteger and then subtracts
    \a valueToSub to the current value, returning the new value value.
    This function uses a sequentially consistent memory ordering if possible;
    or "Ordered" ordering if not.
    \sa fetchAndSubOrdered(), operator+=()
*/

/*! \fn T QAtomicInteger::fetchAndOrRelaxed(T valueToOr)

    Atomic fetch-and-or.
    Reads the current value of this QAtomicInteger and then bitwise-ORs
    \a valueToOr to the current value, returning the original value.
    This function uses \e relaxed \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, leaving the compiler and
    processor to freely reorder memory accesses.
    \sa operator|=()
*/

/*! \fn T QAtomicInteger::fetchAndOrAcquire(T valueToOr)

    Atomic fetch-and-or.
    Reads the current value of this QAtomicInteger and then bitwise-ORs
    \a valueToOr to the current value, returning the original value.
    This function uses \e acquire \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access following the atomic operation (in program order) may not
    be re-ordered before the atomic operation.
    \sa operator|=()
*/

/*! \fn T QAtomicInteger::fetchAndOrRelease(T valueToOr)

    Atomic fetch-and-or.
    Reads the current value of this QAtomicInteger and then bitwise-ORs
    \a valueToOr to the current value, returning the original value.
    This function uses \e release \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access before the atomic operation (in program order) may not be
    re-ordered after the atomic operation.
    \sa operator|=()
*/

/*! \fn T QAtomicInteger::fetchAndOrOrdered(T valueToOr)

    Atomic fetch-and-or.
    Reads the current value of this QAtomicInteger and then bitwise-ORs
    \a valueToOr to the current value, returning the original value.
    This function uses \e ordered \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access before and after the atomic operation (in program order)
    may not be re-ordered.
    \sa operator|=()
*/

/*! \fn T QAtomicInteger::operator|=(T valueToOr)
    
    Atomic or-and-fetch.
    Reads the current value of this QAtomicInteger and then bitwise-ORs
    \a valueToOr to the current value, returning the new value value.
    This function uses a sequentially consistent memory ordering if possible;
    or "Ordered" ordering if not.
    \sa fetchAndOrOrdered()
*/

/*! \fn T QAtomicInteger::fetchAndXorRelaxed(T valueToXor)

    Atomic fetch-and-xor.
    Reads the current value of this QAtomicInteger and then bitwise-XORs
    \a valueToXor to the current value, returning the original value.
    This function uses \e relaxed \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, leaving the compiler and
    processor to freely reorder memory accesses.
    \sa operator^=()
*/

/*! \fn T QAtomicInteger::fetchAndXorAcquire(T valueToXor)
    Atomic fetch-and-xor.
    Reads the current value of this QAtomicInteger and then bitwise-XORs
    \a valueToXor to the current value, returning the original value.
    This function uses \e acquire \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access following the atomic operation (in program order) may not
    be re-ordered before the atomic operation.
    \sa operator^=()
*/

/*! \fn T QAtomicInteger::fetchAndXorRelease(T valueToXor)
    Atomic fetch-and-xor.
    Reads the current value of this QAtomicInteger and then bitwise-XORs
    \a valueToXor to the current value, returning the original value.
    This function uses \e release \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access before the atomic operation (in program order) may not be
    re-ordered after the atomic operation.
    \sa operator^=()
*/

/*! \fn T QAtomicInteger::fetchAndXorOrdered(T valueToXor)
    Atomic fetch-and-xor.
    Reads the current value of this QAtomicInteger and then bitwise-XORs
    \a valueToXor to the current value, returning the original value.
    This function uses \e ordered \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access before and after the atomic operation (in program order)
    may not be re-ordered.
    \sa operator^=()
*/

/*! \fn T QAtomicInteger::operator^=(T valueToXor)
    Atomic xor-and-fetch.
    Reads the current value of this QAtomicInteger and then bitwise-XORs
    \a valueToXor to the current value, returning the new value value.
    This function uses a sequentially consistent memory ordering if possible;
    or "Ordered" ordering if not.
    \sa fetchAndXorOrdered()
*/

/*! \fn T QAtomicInteger::fetchAndAndRelaxed(T valueToAnd)
    Atomic fetch-and-and.
    Reads the current value of this QAtomicInteger and then bitwise-ANDs
    \a valueToAnd to the current value, returning the original value.
    This function uses \e relaxed \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, leaving the compiler and
    processor to freely reorder memory accesses.
    \sa operator&=()
*/

/*! \fn T QAtomicInteger::fetchAndAndAcquire(T valueToAnd)

    Atomic fetch-and-and.
    Reads the current value of this QAtomicInteger and then bitwise-ANDs
    \a valueToAnd to the current value, returning the original value.
    This function uses \e acquire \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access following the atomic operation (in program order) may not
    be re-ordered before the atomic operation.
    \sa operator&=()
*/

/*! \fn T QAtomicInteger::fetchAndAndRelease(T valueToAnd)
    Atomic fetch-and-and.
    Reads the current value of this QAtomicInteger and then bitwise-ANDs
    \a valueToAnd to the current value, returning the original value.
    This function uses \e release \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access before the atomic operation (in program order) may not be
    re-ordered after the atomic operation.
    \sa operator&=()
*/

/*! \fn T QAtomicInteger::fetchAndAndOrdered(T valueToAnd)
    Atomic fetch-and-and.
    Reads the current value of this QAtomicInteger and then bitwise-ANDs
    \a valueToAnd to the current value, returning the original value.
    This function uses \e ordered \l {QAtomicInteger#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access before and after the atomic operation (in program order)
    may not be re-ordered.
    \sa operator&=()
*/

/*! \fn T QAtomicInteger::operator&=(T valueToAnd)
    Atomic add-and-fetch.
    Reads the current value of this QAtomicInteger and then bitwise-ANDs
    \a valueToAnd to the current value, returning the new value value.
    This function uses a sequentially consistent memory ordering if possible;
    or "Ordered" ordering if not.
    \sa fetchAndAndOrdered()
*/
/*!
\macro Q_ATOMIC_INTnn_IS_SUPPORTED
    \relates QAtomicInteger
    This macro is defined if atomic integers of size \e{nn} (in bits) are
    supported in this compiler / architecture combination.
    Q_ATOMIC_INT32_IS_SUPPORTED is always defined.
    \e{nn} is the size of the integer, in bits (8, 16, 32 or 64).
*/

/*!
    \macro Q_ATOMIC_INTnn_REFERENCE_COUNTING_IS_ALWAYS_NATIVE
    \relates QAtomicInteger

    This macro is defined if and only if all generations of your
    processor support atomic reference counting.

    \e{nn} is the size of the integer, in bits (8, 16, 32 or 64).
*/

/*!
    \macro Q_ATOMIC_INTnn_REFERENCE_COUNTING_IS_SOMETIMES_NATIVE
    \relates QAtomicInteger

    This macro is defined when only certain generations of the
    processor support atomic reference counting. Use the
    QAtomicInteger::isReferenceCountingNative() function to check what
    your processor supports.

    \e{nn} is the size of the integer, in bits (8, 16, 32 or 64).
*/

/*!
    \macro Q_ATOMIC_INTnn_REFERENCE_COUNTING_IS_NOT_NATIVE
    \relates QAtomicInteger

    This macro is defined when the hardware does not support atomic
    reference counting.

    \e{nn} is the size of the integer, in bits (8, 16, 32 or 64).
*/

/*!
     \macro Q_ATOMIC_INTnn_REFERENCE_COUNTING_IS_WAIT_FREE
    \relates QAtomicInteger

    This macro is defined together with
    Q_ATOMIC_INTnn_REFERENCE_COUNTING_IS_ALWAYS_NATIVE to indicate that
    the reference counting is wait-free.

    \e{nn} is the size of the integer, in bits (8, 16, 32 or 64).
*/

/*!
    \macro Q_ATOMIC_INTnn_TEST_AND_SET_IS_ALWAYS_NATIVE
    \relates QAtomicInteger

    This macro is defined if and only if your processor supports
    atomic test-and-set on integers.

    \e{nn} is the size of the integer, in bits (8, 16, 32 or 64).
*/

/*!
    \macro Q_ATOMIC_INTnn_TEST_AND_SET_IS_SOMETIMES_NATIVE
    \relates QAtomicInteger

    This macro is defined when only certain generations of the
    processor support atomic test-and-set on integers. Use the
    QAtomicInt::isTestAndSetNative() function to check what your
    processor supports.

    \e{nn} is the size of the integer, in bits (8, 16, 32 or 64).
*/

/*!
    \macro Q_ATOMIC_INTnn_TEST_AND_SET_IS_NOT_NATIVE
    \relates QAtomicInteger

    This macro is defined when the hardware does not support atomic
    test-and-set on integers.

    \e{nn} is the size of the integer, in bits (8, 16, 32 or 64).
*/

/*!
    \macro Q_ATOMIC_INTnn_TEST_AND_SET_IS_WAIT_FREE
    \relates QAtomicInteger

    This macro is defined together with
    Q_ATOMIC_INT_TEST_AND_SET_IS_ALWAYS_NATIVE to indicate that the
    atomic test-and-set on integers is wait-free.

    \e{nn} is the size of the integer, in bits (8, 16, 32 or 64).
*/

/*!
    \macro Q_ATOMIC_INTnn_FETCH_AND_STORE_IS_ALWAYS_NATIVE
    \relates QAtomicInteger

    This macro is defined if and only if your processor supports
    atomic fetch-and-store on integers.

    \e{nn} is the size of the integer, in bits (8, 16, 32 or 64).
*/

/*!
    \macro Q_ATOMIC_INTnn_FETCH_AND_STORE_IS_SOMETIMES_NATIVE
    \relates QAtomicInteger

    This macro is defined when only certain generations of the
    processor support atomic fetch-and-store on integers. Use the
    QAtomicInt::isFetchAndStoreNative() function to check what your
    processor supports.

    \e{nn} is the size of the integer, in bits (8, 16, 32 or 64).
*/

/*!
    \macro Q_ATOMIC_INTnn_FETCH_AND_STORE_IS_NOT_NATIVE
    \relates QAtomicInteger

    This macro is defined when the hardware does not support atomic
    fetch-and-store on integers.

    \e{nn} is the size of the integer, in bits (8, 16, 32 or 64).    
*/

/*!
    \macro Q_ATOMIC_INTnn_FETCH_AND_STORE_IS_WAIT_FREE
    \relates QAtomicInteger

    This macro is defined together with
    Q_ATOMIC_INTnn_FETCH_AND_STORE_IS_ALWAYS_NATIVE to indicate that the
    atomic fetch-and-store on integers is wait-free.

    \e{nn} is the size of the integer, in bits (8, 16, 32 or 64).
*/

/*!
    \macro Q_ATOMIC_INTnn_FETCH_AND_ADD_IS_ALWAYS_NATIVE
    \relates QAtomicInteger

    This macro is defined if and only if your processor supports
    atomic fetch-and-add on integers.

    \e{nn} is the size of the integer, in bits (8, 16, 32 or 64).
*/

/*!
    \macro Q_ATOMIC_INTnn_FETCH_AND_ADD_IS_SOMETIMES_NATIVE
    \relates QAtomicInteger

    This macro is defined when only certain generations of the
    processor support atomic fetch-and-add on integers. Use the
    QAtomicInteger::isFetchAndAddNative() function to check what your
    processor supports.

    \e{nn} is the size of the integer, in bits (8, 16, 32 or 64).
*/

/*!
    \macro Q_ATOMIC_INTnn_FETCH_AND_ADD_IS_NOT_NATIVE
    \relates QAtomicInteger

    This macro is defined when the hardware does not support atomic
    fetch-and-add on integers.

    \e{nn} is the size of the integer, in bits (8, 16, 32 or 64).
*/

/*!
    \macro Q_ATOMIC_INTnn_FETCH_AND_ADD_IS_WAIT_FREE
    \relates QAtomicInteger

    This macro is defined together with
    Q_ATOMIC_INTnn_FETCH_AND_ADD_IS_ALWAYS_NATIVE to indicate that the
    atomic fetch-and-add on integers is wait-free.

    \e{nn} is the size of the integer, in bits (8, 16, 32 or 64).
*/




/*!
    \class QAtomicPointer
    \brief The QAtomicPointer class is a template class that provides platform-independent atomic operations on pointers.
    \since 4.4

    \ingroup thread

    For atomic operations on integers, see the QAtomicInteger class.

    An \e atomic operation is a complex operation that completes without interruption.
    The QAtomicPointer class provides atomic test-and-set, fetch-and-store, and fetch-and-add for pointers.

    \section1 Non-atomic convenience operators

    For convenience, QAtomicPointer provides pointer comparison, cast,
    dereference, and assignment operators. Note that these operators
    are \e not atomic.

    \section1 The Atomic API

    \section2 Memory ordering

    QAtomicPointer provides several implementations of the atomic
    test-and-set, fetch-and-store, and fetch-and-add functions. Each
    implementation defines a memory ordering semantic that describes
    how memory accesses surrounding the atomic instruction are
    executed by the processor. Since many modern architectures allow
    out-of-order execution and memory ordering, using the correct
    semantic is necessary to ensure that your application functions
    properly on all processors.

    \list

    \o Relaxed - memory ordering is unspecified, leaving the compiler
    and processor to freely reorder memory accesses.

    \o Acquire - memory access following the atomic operation (in
    program order) may not be re-ordered before the atomic operation.

    \o Release - memory access before the atomic operation (in program
    order) may not be re-ordered after the atomic operation.

    \o Ordered - the same Acquire and Release semantics combined.

    \endlist

    \section2 Test-and-set

    If the current value of the QAtomicPointer is an expected value,
    the test-and-set functions assign a new value to the
    QAtomicPointer and return true. If values are \a not the same,
    these functions do nothing and return false. This operation
    equates to the following code:

    \snippet doc/src/snippets/code/src_corelib_thread_qatomic.cpp 4

    There are 4 test-and-set functions: testAndSetRelaxed(),
    testAndSetAcquire(), testAndSetRelease(), and
    testAndSetOrdered(). See above for an explanation of the different
    memory ordering semantics.

    \section2 Fetch-and-store

    The atomic fetch-and-store functions read the current value of the
    QAtomicPointer and then assign a new value, returning the original
    value. This operation equates to the following code:

    \snippet doc/src/snippets/code/src_corelib_thread_qatomic.cpp 5

    There are 4 fetch-and-store functions: fetchAndStoreRelaxed(),
    fetchAndStoreAcquire(), fetchAndStoreRelease(), and
    fetchAndStoreOrdered(). See above for an explanation of the
    different memory ordering semantics.

    \section2 Fetch-and-add

    The atomic fetch-and-add functions read the current value of the
    QAtomicPointer and then add the given value to the current value,
    returning the original value. This operation equates to the
    following code:

    \snippet doc/src/snippets/code/src_corelib_thread_qatomic.cpp 6

    There are 4 fetch-and-add functions: fetchAndAddRelaxed(),
    fetchAndAddAcquire(), fetchAndAddRelease(), and
    fetchAndAddOrdered(). See above for an explanation of the
    different memory ordering semantics.

    \section1 Feature Tests for the Atomic API

    Providing a platform-independent atomic API that works on all
    processors is challenging. The API provided by QAtomicPointer is
    guaranteed to work atomically on all processors. However, since
    not all processors implement support for every operation provided
    by QAtomicPointer, it is necessary to expose information about the
    processor.

    You can check at compile time which features are supported on your
    hardware using various macros. These will tell you if your
    hardware always, sometimes, or does not support a particular
    operation. The macros have the form
    Q_ATOMIC_POINTER_\e{OPERATION}_IS_\e{HOW}_NATIVE. \e{OPERATION} is
    one of TEST_AND_SET, FETCH_AND_STORE, or FETCH_AND_ADD, and
    \e{HOW} is one of ALWAYS, SOMETIMES, or NOT. There will always be
    exactly one defined macro per operation. For example, if
    Q_ATOMIC_POINTER_TEST_AND_SET_IS_ALWAYS_NATIVE is defined, neither
    Q_ATOMIC_POINTER_TEST_AND_SET_IS_SOMETIMES_NATIVE nor
    Q_ATOMIC_POINTER_TEST_AND_SET_IS_NOT_NATIVE will be defined.

    An operation that completes in constant time is said to be
    wait-free. Such operations are not implemented using locks or
    loops of any kind. For atomic operations that are always
    supported, and that are wait-free, Qt defines the
    Q_ATOMIC_POINTER_\e{OPERATION}_IS_WAIT_FREE in addition to the
    Q_ATOMIC_POINTER_\e{OPERATION}_IS_ALWAYS_NATIVE.

    In cases where an atomic operation is only supported in newer
    generations of the processor, QAtomicPointer also provides a way
    to check at runtime what your hardware supports with the
    isTestAndSetNative(), isFetchAndStoreNative(), and
    isFetchAndAddNative() functions. Wait-free implementations can be
    detected using the isTestAndSetWaitFree(),
    isFetchAndStoreWaitFree(), and isFetchAndAddWaitFree() functions.

    Below is a complete list of all feature macros for QAtomicPointer:

    \list

    \o Q_ATOMIC_POINTER_TEST_AND_SET_IS_ALWAYS_NATIVE
    \o Q_ATOMIC_POINTER_TEST_AND_SET_IS_SOMETIMES_NATIVE
    \o Q_ATOMIC_POINTER_TEST_AND_SET_IS_NOT_NATIVE
    \o Q_ATOMIC_POINTER_TEST_AND_SET_IS_WAIT_FREE

    \o Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_ALWAYS_NATIVE
    \o Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_SOMETIMES_NATIVE
    \o Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_NOT_NATIVE
    \o Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_WAIT_FREE

    \o Q_ATOMIC_POINTER_FETCH_AND_ADD_IS_ALWAYS_NATIVE
    \o Q_ATOMIC_POINTER_FETCH_AND_ADD_IS_SOMETIMES_NATIVE
    \o Q_ATOMIC_POINTER_FETCH_AND_ADD_IS_NOT_NATIVE
    \o Q_ATOMIC_POINTER_FETCH_AND_ADD_IS_WAIT_FREE

    \endlist

    \sa QAtomicInteger
*/

/*! \fn QAtomicPointer::QAtomicPointer(T *value)

    Constructs a QAtomicPointer with the given \a value.
*/

/*! \fn QAtomicPointer::QAtomicPointer(const QAtomicPointer<T> &other)

    Constructs a copy of \a other.
*/

/*! \fn QAtomicPointer<T> &QAtomicPointer::operator=(T *value)

    Assigns the \a value to this QAtomicPointer and returns a
    reference to this QAtomicPointer.
*/

/*! \fn QAtomicPointer<T> &QAtomicPointer::operator=(const QAtomicPointer<T> &other)

    Assigns \a other to this QAtomicPointer and returns a reference to
    this QAtomicPointer.
*/

/*! \fn bool QAtomicPointer::operator==(T *value) const

    Returns true if the \a value is equal to the value in this
    QAtomicPointer; otherwise returns false.
*/

/*! \fn bool QAtomicPointer::operator!=(T *value) const

    Returns true if the value of this QAtomicPointer is not equal to
    \a value; otherwise returns false.
*/

/*! \fn bool QAtomicPointer::operator!() const

    Returns true is the current value of this QAtomicPointer is zero;
    otherwise returns false.
*/

/*! \fn QAtomicPointer::operator T *() const

    Returns the current pointer value stored by this QAtomicPointer
    object.
*/

/*! \fn T *QAtomicPointer::operator->() const

*/

/*! \fn bool QAtomicPointer::isTestAndSetNative()

    Returns true if test-and-set is implemented using atomic processor
    instructions, false otherwise.
*/

/*! \fn bool QAtomicPointer::isTestAndSetWaitFree()

    Returns true if atomic test-and-set is wait-free, false otherwise.
*/

/*! \fn bool QAtomicPointer::testAndSetRelaxed(T *expectedValue, T *newValue)

    Atomic test-and-set.

    If the current value of this QAtomicPointer is the \a expectedValue,
    the test-and-set functions assign the \a newValue to this
    QAtomicPointer and return true. If the values are \e not the same,
    this function does nothing and returns false.

    This function uses \e relaxed \l {QAtomicPointer#Memory
    ordering}{memory ordering} semantics, leaving the compiler and
    processor to freely reorder memory accesses.
*/

/*! \fn bool QAtomicPointer::testAndSetAcquire(T *expectedValue, T *newValue)

    Atomic test-and-set.

    If the current value of this QAtomicPointer is the \a expectedValue,
    the test-and-set functions assign the \a newValue to this
    QAtomicPointer and return true. If the values are \e not the same,
    this function does nothing and returns false.

    This function uses \e acquire \l {QAtomicPointer#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access following the atomic operation (in program order) may not
    be re-ordered before the atomic operation.
*/

/*! \fn bool QAtomicPointer::testAndSetRelease(T *expectedValue, T *newValue)

    Atomic test-and-set.

    If the current value of this QAtomicPointer is the \a expectedValue,
    the test-and-set functions assign the \a newValue to this
    QAtomicPointer and return true. If the values are \e not the same,
    this function does nothing and returns false.

    This function uses \e release \l {QAtomicPointer#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access before the atomic operation (in program order) may not be
    re-ordered after the atomic operation.
*/

/*! \fn bool QAtomicPointer::testAndSetOrdered(T *expectedValue, T *newValue)

    Atomic test-and-set.

    If the current value of this QAtomicPointer is the \a expectedValue,
    the test-and-set functions assign the \a newValue to this
    QAtomicPointer and return true. If the values are \e not the same,
    this function does nothing and returns false.

    This function uses \e ordered \l {QAtomicPointer#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access before and after the atomic operation (in program order)
    may not be re-ordered.
*/

/*! \fn bool QAtomicPointer::isFetchAndStoreNative()

    Returns true if fetch-and-store is implemented using atomic
    processor instructions, false otherwise.
*/

/*! \fn bool QAtomicPointer::isFetchAndStoreWaitFree()

    Returns true if atomic fetch-and-store is wait-free, false
    otherwise.
*/

/*! \fn T *QAtomicPointer::fetchAndStoreRelaxed(T *newValue)

    Atomic fetch-and-store.

    Reads the current value of this QAtomicPointer and then assigns it the
    \a newValue, returning the original value.

    This function uses \e relaxed \l {QAtomicPointer#Memory
    ordering}{memory ordering} semantics, leaving the compiler and
    processor to freely reorder memory accesses.
*/

/*! \fn T *QAtomicPointer::fetchAndStoreAcquire(T *newValue)

    Atomic fetch-and-store.

    Reads the current value of this QAtomicPointer and then assigns it the
    \a newValue, returning the original value.

    This function uses \e acquire \l {QAtomicPointer#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access following the atomic operation (in program order) may not
    be re-ordered before the atomic operation.
*/

/*! \fn T *QAtomicPointer::fetchAndStoreRelease(T *newValue)

    Atomic fetch-and-store.

    Reads the current value of this QAtomicPointer and then assigns it the
    \a newValue, returning the original value.

    This function uses \e release \l {QAtomicPointer#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access before the atomic operation (in program order) may not be
    re-ordered after the atomic operation.
*/

/*! \fn T *QAtomicPointer::fetchAndStoreOrdered(T *newValue)

    Atomic fetch-and-store.

    Reads the current value of this QAtomicPointer and then assigns it the
    \a newValue, returning the original value.

    This function uses \e ordered \l {QAtomicPointer#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access before and after the atomic operation (in program order)
    may not be re-ordered.
*/

/*! \fn bool QAtomicPointer::isFetchAndAddNative()

    Returns true if fetch-and-add is implemented using atomic
    processor instructions, false otherwise.
*/

/*! \fn bool QAtomicPointer::isFetchAndAddWaitFree()

    Returns true if atomic fetch-and-add is wait-free, false
    otherwise.
*/

/*! \fn T *QAtomicPointer::fetchAndAddRelaxed(qptrdiff valueToAdd)

    Atomic fetch-and-add.

    Reads the current value of this QAtomicPointer and then adds
    \a valueToAdd to the current value, returning the original value.

    This function uses \e relaxed \l {QAtomicPointer#Memory
    ordering}{memory ordering} semantics, leaving the compiler and
    processor to freely reorder memory accesses.
*/

/*! \fn T *QAtomicPointer::fetchAndAddAcquire(qptrdiff valueToAdd)

    Atomic fetch-and-add.

    Reads the current value of this QAtomicPointer and then adds
    \a valueToAdd to the current value, returning the original value.

    This function uses \e acquire \l {QAtomicPointer#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access following the atomic operation (in program order) may not
    be re-ordered before the atomic operation.
*/

/*! \fn T *QAtomicPointer::fetchAndAddRelease(qptrdiff valueToAdd)

    Atomic fetch-and-add.

    Reads the current value of this QAtomicPointer and then adds
    \a valueToAdd to the current value, returning the original value.

    This function uses \e release \l {QAtomicPointer#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access before the atomic operation (in program order) may not be
    re-ordered after the atomic operation.
*/

/*! \fn T *QAtomicPointer::fetchAndAddOrdered(qptrdiff valueToAdd)

    Atomic fetch-and-add.

    Reads the current value of this QAtomicPointer and then adds
    \a valueToAdd to the current value, returning the original value.

    This function uses \e ordered \l {QAtomicPointer#Memory
    ordering}{memory ordering} semantics, which ensures that memory
    access before and after the atomic operation (in program order)
    may not be re-ordered.
*/

/*!
    \macro Q_ATOMIC_POINTER_TEST_AND_SET_IS_ALWAYS_NATIVE
    \relates QAtomicPointer

    This macro is defined if and only if your processor supports
    atomic test-and-set on pointers.
*/

/*!
    \macro Q_ATOMIC_POINTER_TEST_AND_SET_IS_SOMETIMES_NATIVE
    \relates QAtomicPointer

    This macro is defined when only certain generations of the
    processor support atomic test-and-set on pointers. Use the
    QAtomicPointer::isTestAndSetNative() function to check what your
    processor supports.
*/

/*!
    \macro Q_ATOMIC_POINTER_TEST_AND_SET_IS_NOT_NATIVE
    \relates QAtomicPointer

    This macro is defined when the hardware does not support atomic
    test-and-set on pointers.
*/

/*!
    \macro Q_ATOMIC_POINTER_TEST_AND_SET_IS_WAIT_FREE
    \relates QAtomicPointer

    This macro is defined together with
    Q_ATOMIC_POINTER_TEST_AND_SET_IS_ALWAYS_NATIVE to indicate that
    the atomic test-and-set on pointers is wait-free.
*/

/*!
    \macro Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_ALWAYS_NATIVE
    \relates QAtomicPointer

    This macro is defined if and only if your processor supports
    atomic fetch-and-store on pointers.
*/

/*!
    \macro Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_SOMETIMES_NATIVE
    \relates QAtomicPointer

    This macro is defined when only certain generations of the
    processor support atomic fetch-and-store on pointers. Use the
    QAtomicPointer::isFetchAndStoreNative() function to check what
    your processor supports.
*/

/*!
    \macro Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_NOT_NATIVE
    \relates QAtomicPointer

    This macro is defined when the hardware does not support atomic
    fetch-and-store on pointers.
*/

/*!
    \macro Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_WAIT_FREE
    \relates QAtomicPointer

    This macro is defined together with
    Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_ALWAYS_NATIVE to indicate that
    the atomic fetch-and-store on pointers is wait-free.
*/

/*!
    \macro Q_ATOMIC_POINTER_FETCH_AND_ADD_IS_ALWAYS_NATIVE
    \relates QAtomicPointer

    This macro is defined if and only if your processor supports
    atomic fetch-and-add on pointers.
*/

/*!
    \macro Q_ATOMIC_POINTER_FETCH_AND_ADD_IS_SOMETIMES_NATIVE
    \relates QAtomicPointer

    This macro is defined when only certain generations of the
    processor support atomic fetch-and-add on pointers. Use the
    QAtomicPointer::isFetchAndAddNative() function to check what your
    processor supports.
*/

/*!
    \macro Q_ATOMIC_POINTER_FETCH_AND_ADD_IS_NOT_NATIVE
    \relates QAtomicPointer

    This macro is defined when the hardware does not support atomic
    fetch-and-add on pointers.
*/

/*!
    \macro Q_ATOMIC_POINTER_FETCH_AND_ADD_IS_WAIT_FREE
    \relates QAtomicPointer

    This macro is defined together with
    Q_ATOMIC_POINTER_FETCH_AND_ADD_IS_ALWAYS_NATIVE to indicate that
    the atomic fetch-and-add on pointers is wait-free.
*/

// static checks
#ifndef Q_ATOMIC_INT32_IS_SUPPORTED
#  error "Q_ATOMIC_INT32_IS_SUPPORTED must be defined"
#endif

#if !defined(Q_ATOMIC_INT64_IS_SUPPORTED) && QT_POINTER_SIZE == 8
// 64-bit platform
#  error "Q_ATOMIC_INT64_IS_SUPPORTED must be defined on a 64-bit platform"
#endif

QT_BEGIN_NAMESPACE

// The following specializations must always be defined
static_assert(sizeof(QAtomicInteger<unsigned>));
static_assert(sizeof(QAtomicInteger<long>));
static_assert(sizeof(QAtomicInteger<unsigned long>));
static_assert(sizeof(QAtomicInteger<quintptr>));
static_assert(sizeof(QAtomicInteger<qptrdiff>));
#ifdef Q_COMPILER_UNICODE_STRINGS
static_assert(sizeof(QAtomicInteger<char32_t>));
#endif

#ifdef Q_ATOMIC_INT16_IS_SUPPORTED
static_assert(sizeof(QAtomicInteger<short>));
static_assert(sizeof(QAtomicInteger<unsigned short>));
#  if WCHAR_MAX < 0x10000
static_assert(sizeof(QAtomicInteger<wchar_t>));
#  endif
#  ifdef Q_COMPILER_UNICODE_STRINGS
static_assert(sizeof(QAtomicInteger<char16_t>));
#  endif
#endif

#ifdef Q_ATOMIC_INT64_IS_SUPPORTED
static_assert(sizeof(QAtomicInteger<qint64>));
static_assert(sizeof(QAtomicInteger<quint64>));
#endif

#if WCHAR_MAX == INT_MAX
static_assert(sizeof(QAtomicInteger<wchar_t>));
#endif

QT_END_NAMESPACE

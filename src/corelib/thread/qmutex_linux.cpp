/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Copyright (C) 2012 Intel Corporation
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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

#include "qplatformdefs.h"
#include "qmutex.h"

#ifndef QT_NO_THREAD
#include "qatomic.h"
#include "qelapsedtimer.h"
#include "qmutex_p.h"
#include "qfutex_p.h"

#ifndef QT_ALWAYS_USE_FUTEX
# error "Qt build is broken: qmutex_linux.cpp is being built but futex support is not wanted"
#endif

#ifndef FUTEX_PRIVATE_FLAG
#  define FUTEX_PRIVATE_FLAG    128
#endif

QT_BEGIN_NAMESPACE

using namespace QtFutex;

/*
 * QBasicMutex implementation on Linux with futexes
 *
 * QBasicMutex contains one pointer value, which can contain one of four
 * different values:
 *    0x0       unlocked, non-recursive mutex
 *    0x1       locked non-recursive mutex, no waiters
 *    0x3       locked non-recursive mutex, at least one waiter
 *   > 0x3      recursive mutex, points to a QMutexPrivate object
 *
 * LOCKING (non-recursive):
 *
 * A non-recursive mutex starts in the 0x0 state, indicating that it's
 * unlocked. When the first thread attempts to lock it, it will perform a
 * testAndSetAcquire from 0x0 to 0x1. If that succeeds, the caller concludes
 * that it successfully locked the mutex. That happens in fastTryLock().
 *
 * If that testAndSetAcquire fails, QBasicMutex::lockInternal is called.
 *
 * lockInternal will examine the value of the pointer. Otherwise, it will use
 * futexes to sleep and wait for another thread to unlock. To do that, it needs
 * to set a pointer value of 0x3, which indicates that thread is waiting. It
 * does that by a simple fetchAndStoreAcquire operation.
 *
 * If the pointer value was 0x0, it means we succeeded in acquiring the mutex.
 * For other values, it will then call FUTEX_WAIT and with an expected value of
 * 0x3.
 *
 * If the pointer value changed before futex(2) managed to sleep, it will
 * return -1 / EWOULDBLOCK, in which case we have to start over. And even if we
 * are woken up directly by a FUTEX_WAKE, we need to acquire the mutex, so we
 * start over again.
 *
 * UNLOCKING (non-recursive):
 *
 * To unlock, we need to set a value of 0x0 to indicate it's unlocked. The
 * first attempt is a testAndSetRelease operation from 0x1 to 0x0. If that
 * succeeds, we're done.
 *
 * If it fails, unlockInternal() is called. The only possibility is that the
 * mutex value was 0x3, which indicates some other thread is waiting or was
 * waiting in the past. We then set the mutex to 0x0 and perform a FUTEX_WAKE.
 */

static inline QMutexData *dummyFutexValue()
{
    return reinterpret_cast<QMutexData *>(quintptr(3));
}


template <bool IsTimed> static inline
bool lockInternal_helper(QBasicAtomicPointer<QMutexData> &d_ptr, int timeout = -1, QElapsedTimer *elapsedTimer = 0) noexcept
{
    if (!IsTimed)
        timeout = -1;

    // we're here because fastTryLock() has just failed
    if (timeout == 0)
        return false;

    // the mutex is locked already, set a bit indicating we're waiting
    if (d_ptr.fetchAndStoreAcquire(dummyFutexValue()) == nullptr)
        return true;

    qint64 nstimeout = timeout * Q_INT64_C(1000) * 1000;
    qint64 remainingTime = nstimeout;
    forever {
        // successfully set the waiting bit, now sleep
        if (IsTimed && nstimeout >= 0) {
            bool r = futexWait(d_ptr, dummyFutexValue(), remainingTime);
            if (!r)
                return false;

            // we got woken up, so try to acquire the mutex
            // note we must set to dummyFutexValue because there could be other threads
            // also waiting
            if (d_ptr.fetchAndStoreAcquire(dummyFutexValue()) == nullptr)
                return true;

            // recalculate the timeout
            remainingTime = nstimeout - elapsedTimer->nsecsElapsed();
            if (remainingTime <= 0)
                return false;
        } else {
            futexWait(d_ptr, dummyFutexValue());

            // we got woken up, so try to acquire the mutex
            // note we must set to dummyFutexValue because there could be other threads
            // also waiting
            if (d_ptr.fetchAndStoreAcquire(dummyFutexValue()) == nullptr)
                return true;
        }
    }

    Q_ASSERT(d_ptr.loadRelaxed());
    return true;
}

void QBasicMutex::lockInternal() noexcept
{
    Q_ASSERT(!isRecursive());
    lockInternal_helper<false>(d_ptr);
}

bool QBasicMutex::lockInternal(int timeout) noexcept
{
    Q_ASSERT(!isRecursive());
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();
    return lockInternal_helper<true>(d_ptr, timeout, &elapsedTimer);
}

void QBasicMutex::unlockInternal() noexcept
{
    QMutexData *d = d_ptr.loadRelaxed();
    Q_ASSERT(d); //we must be locked
    Q_ASSERT(d != dummyLocked()); // testAndSetRelease(dummyLocked(), 0) failed
    Q_UNUSED(d);
    Q_ASSERT(!isRecursive());

    d_ptr.storeRelease(0);
    futexWakeOne(d_ptr);
}


QT_END_NAMESPACE

#endif // QT_NO_THREAD

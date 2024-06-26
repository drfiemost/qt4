/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Copyright (C) 2012 Intel Corporation
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

#ifndef QMUTEX_P_H
#define QMUTEX_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qmutex.cpp, qmutex_unix.cpp, and qmutex_win.cpp.  This header
// file may change from version to version without notice, or even be
// removed.
//
// We mean it.
//

#include <QtCore/qglobal.h>
#include <QtCore/qnamespace.h>
#include <QtCore/qmutex.h>
#include <QtCore/qatomic.h>
#include <QtCore/qdeadlinetimer.h>

#if defined(Q_OS_LINUX) && !defined(QT_LINUXBASE)
// use Linux mutexes everywhere except for LSB builds
#  define QT_LINUX_FUTEX
#endif

struct timespec;

QT_BEGIN_NAMESPACE

class QMutexData
{
public:
    bool recursive;
    QMutexData(QMutex::RecursionMode mode = QMutex::NonRecursive)
        : recursive(mode == QMutex::Recursive) {}
};

#if !defined(Q_OS_LINUX)
class QMutexPrivate : public QMutexData {
public:
    QMutexPrivate();
    ~QMutexPrivate();

    bool wait(int timeout = -1);
    void wakeUp() noexcept;

    // Conrol the lifetime of the privates
    QAtomicInt refCount;
    int id;

    bool ref() {
        Q_ASSERT(refCount.loadRelaxed() >= 0);
        int c;
        do {
            c = refCount.loadRelaxed();
            if (c == 0)
                return false;
        } while (!refCount.testAndSetRelaxed(c, c + 1));
        Q_ASSERT(refCount.loadRelaxed() >= 0);
        return true;
    }
    void deref() {
        Q_ASSERT(refCount.loadRelaxed() >= 0);
        if (!refCount.deref())
            release();
        Q_ASSERT(refCount.loadRelaxed() >= 0);
    }
    void release();
    static QMutexPrivate *allocate();

    QAtomicInt waiters; //number of thread waiting
    QAtomicInt possiblyUnlocked; //bool saying that a timed wait timed out
    enum { BigNumber = 0x100000 }; //Must be bigger than the possible number of waiters (number of threads)
    void derefWaiters(int value) noexcept;

    //platform specific stuff
#if defined(Q_OS_UNIX)
    bool wakeup;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
#elif defined(Q_OS_WIN32)
    HANDLE event;
#endif
};
#endif //QT_LINUX_FUTEX

#ifdef Q_OS_UNIX
// helper functions for qmutex_unix.cpp and qwaitcondition_unix.cpp
// they are in qwaitcondition_unix.cpp actually
void qt_initialize_pthread_cond(pthread_cond_t *cond, const char *where);
void qt_abstime_for_timeout(struct timespec *ts, QDeadlineTimer deadline);
#endif

QT_END_NAMESPACE

#endif // QMUTEX_P_H

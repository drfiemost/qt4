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

#ifndef QTHREAD_P_H
#define QTHREAD_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//
//

#include "qplatformdefs.h"
#include "QtCore/qthread.h"
#include "QtCore/qmutex.h"
#include "QtCore/qstack.h"
#include "QtCore/qwaitcondition.h"
#include "QtCore/qmap.h"
#include "private/qobject_p.h"

#include <algorithm>

QT_BEGIN_NAMESPACE

class QAbstractEventDispatcher;
class QEventLoop;

class QPostEvent
{
public:
    QObject *receiver;
    QEvent *event;
    int priority;
    inline QPostEvent()
        : receiver(nullptr), event(nullptr), priority(0)
    { }
    inline QPostEvent(QObject *r, QEvent *e, int p)
        : receiver(r), event(e), priority(p)
    { }
};
Q_DECLARE_TYPEINFO(QPostEvent, Q_MOVABLE_TYPE);

inline bool operator<(int priority, const QPostEvent &pe)
{
    return pe.priority < priority;
}
inline bool operator<(const QPostEvent &pe, int priority)
{
    return priority < pe.priority;
}

// This class holds the list of posted events.
//  The list has to be kept sorted by priority
class QPostEventList : public QVector<QPostEvent>
{
public:
    // recursion == recursion count for sendPostedEvents()
    int recursion;

    // sendOffset == the current event to start sending
    int startOffset;
    // insertionOffset == set by sendPostedEvents to tell postEvent() where to start insertions
    int insertionOffset;

    QMutex mutex;

    inline QPostEventList()
        : QVector<QPostEvent>(), recursion(0), startOffset(0), insertionOffset(0)
    { }

    void addEvent(const QPostEvent &ev) {
        int priority = ev.priority;
        if (isEmpty() ||
            last().priority >= priority ||
            begin() + insertionOffset >= end()) {
            // optimization: we can simply append if the last event in
            // the queue has higher or equal priority
            append(ev);
        } else {
            // insert event in descending priority order, using upper
            // bound for a given priority (to ensure proper ordering
            // of events with the same priority)
            QPostEventList::iterator at = std::upper_bound(begin() + insertionOffset, end(), priority);
            insert(at, ev);
        }
    }
private:
    //hides because they do not keep that list sorted. addEvent must be used
    using QVector<QPostEvent>::append;
    using QVector<QPostEvent>::insert;
};

#ifndef QT_NO_THREAD

class QThreadPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QThread)

public:
    QThreadPrivate(QThreadData *d = nullptr);
    ~QThreadPrivate() override;

    mutable QMutex mutex;
    QThreadData *data;

    bool running;
    bool finished;
    bool terminated;
    bool isInFinish; //when in QThreadPrivate::finish

    bool exited;
    int returnCode;

    uint stackSize;
    QThread::Priority priority;

    static QThread *threadForId(int id);

#ifdef Q_OS_UNIX
    QWaitCondition thread_done;

    static void *start(void *arg);
    static void finish(void *);

#endif // Q_OS_UNIX

#if defined(Q_OS_WIN32)
    HANDLE handle;
    unsigned int id;
    int waiters;

    static unsigned int __stdcall start(void *);
    static void finish(void *, bool lockAnyway=true);
#endif // Q_OS_WIN32

#if defined(Q_OS_WIN32)
    bool terminationEnabled, terminatePending;
# endif

    static void createEventDispatcher(QThreadData *data);
};

#else // QT_NO_THREAD

class QThreadPrivate : public QObjectPrivate
{
public:
    QThreadPrivate(QThreadData *d = 0) : data(d ? d : new QThreadData) {}
    ~QThreadPrivate() { delete data; }

    QThreadData *data;

    static void setCurrentThread(QThread*) {}
    static QThread *threadForId(int) { return QThread::currentThread(); }
    static void createEventDispatcher(QThreadData *data);

    Q_DECLARE_PUBLIC(QThread)
};

#endif // QT_NO_THREAD

class QThreadData
{
    QAtomicInt _ref;

public:
    QThreadData(int initialRefCount = 1);
    ~QThreadData();

    static QThreadData *current();
    static void clearCurrentThreadData();
    static QThreadData *get2(QThread *thread)
    { Q_ASSERT_X(thread != 0, "QThread", "internal error"); return thread->d_func()->data; }


    void ref();
    void deref();
    inline bool hasEventDispatcher() const
    { return eventDispatcher.loadRelaxed() != nullptr; }

    bool canWaitLocked()
    {
        QMutexLocker locker(&postEventList.mutex);
        return canWait;
    }

    // This class provides per-thread (by way of being a QThreadData
    // member) storage for qFlagLocation()
    class FlaggedDebugSignatures
    {
        static const uint Count = 2;

        uint idx;
        const char* locations[Count];

    public:
        FlaggedDebugSignatures() : idx(0)
        {
            for (auto & location : locations)
                location = nullptr;
        }

        void store(const char* method)
        { locations[idx++ % Count] = method; }

        bool contains(const char *method) const
        {
            for (auto location : locations)
                if (location == method)
                    return true;
            return false;
        }
    };

    QAtomicPointer<QThread> thread;
    QAtomicPointer<void> threadId;
    bool quitNow;
    int loopLevel;
    QAtomicPointer<QAbstractEventDispatcher> eventDispatcher;
    QStack<QEventLoop *> eventLoops;
    QPostEventList postEventList;
    bool canWait;
    QVector<void *> tls;
    bool isAdopted;

    FlaggedDebugSignatures flaggedSignatures;
};

class QScopedLoopLevelCounter
{
    QThreadData *threadData;
public:
    inline QScopedLoopLevelCounter(QThreadData *threadData)
        : threadData(threadData)
    { ++threadData->loopLevel; }
    inline ~QScopedLoopLevelCounter()
    { --threadData->loopLevel; }
};

// thread wrapper for the main() thread
class QAdoptedThread : public QThread
{
    Q_DECLARE_PRIVATE(QThread)

public:
    QAdoptedThread(QThreadData *data = nullptr);
    ~QAdoptedThread() override;
    void init();

    static QThread *createThreadForAdoption();
private:
#ifndef QT_NO_THREAD
    void run() override;
#endif
};

QT_END_NAMESPACE

#endif // QTHREAD_P_H

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

#include "qcoreapplication.h"
#include "qpair.h"
#include "qsocketnotifier.h"
#include "qthread.h"
#include "qelapsedtimer.h"

#include "qeventdispatcher_unix_p.h"
#include <private/qthread_p.h>
#include <private/qcoreapplication_p.h>
#include <private/qcore_unix_p.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>

#if (_POSIX_MONOTONIC_CLOCK-0 <= 0) || defined(QT_BOOTSTRAPPED)
#  include <sys/times.h>
#endif

QT_BEGIN_NAMESPACE

/*****************************************************************************
 UNIX signal handling
 *****************************************************************************/

static sig_atomic_t signal_received;
static sig_atomic_t signals_fired[NSIG];

static void signalHandler(int sig)
{
    signals_fired[sig] = 1;
    signal_received = 1;
}


QEventDispatcherUNIXPrivate::QEventDispatcherUNIXPrivate()
{
    extern void* qt_application_thread_id;
    mainThread = (QThread::currentThreadId() == qt_application_thread_id);
    bool pipefail = false;

    // initialize the common parts of the event loop
    if (qt_safe_pipe(thread_pipe, O_NONBLOCK) == -1) {
        perror("QEventDispatcherUNIXPrivate(): Unable to create thread pipe");
        pipefail = true;
    }

    if (pipefail)
        qFatal("QEventDispatcherUNIXPrivate(): Can not continue without a thread pipe");

    sn_highest = -1;
}

QEventDispatcherUNIXPrivate::~QEventDispatcherUNIXPrivate()
{
    // cleanup the common parts of the event loop
    close(thread_pipe[0]);
    close(thread_pipe[1]);

    // cleanup timers
    qDeleteAll(timerList);
}

int QEventDispatcherUNIXPrivate::doSelect(QEventLoop::ProcessEventsFlags flags, timespec *timeout)
{
    Q_Q(QEventDispatcherUNIX);

    // needed in QEventDispatcherUNIX::select()
    timerList.updateCurrentTime();

    int nsel;
    do {
        if (mainThread) {
            while (signal_received) {
                signal_received = 0;
                for (int i = 0; i < NSIG; ++i) {
                    if (signals_fired[i]) {
                        signals_fired[i] = 0;
                        emit QCoreApplication::instance()->unixSignal(i);
                    }
                }
            }
        }

        // Process timers and socket notifiers - the common UNIX stuff
        int highest = 0;
        if (! (flags & QEventLoop::ExcludeSocketNotifiers) && (sn_highest >= 0)) {
            // return the highest fd we can wait for input on
                sn_vec[0].select_fds = sn_vec[0].enabled_fds;
                sn_vec[1].select_fds = sn_vec[1].enabled_fds;
                sn_vec[2].select_fds = sn_vec[2].enabled_fds;
            highest = sn_highest;
        } else {
            FD_ZERO(&sn_vec[0].select_fds);
            FD_ZERO(&sn_vec[1].select_fds);
            FD_ZERO(&sn_vec[2].select_fds);
        }

        int wakeUpFd = initThreadWakeUp();
        highest = std::max(highest, wakeUpFd);

        nsel = q->select(highest + 1,
                         &sn_vec[0].select_fds,
                         &sn_vec[1].select_fds,
                         &sn_vec[2].select_fds,
                         timeout);
    } while (nsel == -1 && (errno == EINTR || errno == EAGAIN));

    if (nsel == -1) {
        if (errno == EBADF) {
            // it seems a socket notifier has a bad fd... find out
            // which one it is and disable it
            fd_set fdset;
            timeval tm;
            tm.tv_sec = tm.tv_usec = 0l;

            for (int type = 0; type < 3; ++type) {
                QSockNotType::List &list = sn_vec[type].list;
                if (list.size() == 0)
                    continue;

                for (auto sn : list) {
                    FD_ZERO(&fdset);
                    FD_SET(sn->fd, &fdset);

                    int ret = -1;
                    do {
                        switch (type) {
                        case 0: // read
                            ret = select(sn->fd + 1, &fdset, nullptr, nullptr, &tm);
                            break;
                        case 1: // write
                            ret = select(sn->fd + 1, nullptr, &fdset, nullptr, &tm);
                            break;
                        case 2: // except
                            ret = select(sn->fd + 1, nullptr, nullptr, &fdset, &tm);
                            break;
                        }
                    } while (ret == -1 && (errno == EINTR || errno == EAGAIN));

                    if (ret == -1 && errno == EBADF) {
                        // disable the invalid socket notifier
                        static const char *t[] = { "Read", "Write", "Exception" };
                        qWarning("QSocketNotifier: Invalid socket %d and type '%s', disabling...",
                                 sn->fd, t[type]);
                        sn->obj->setEnabled(false);
                    }
                }
            }
        } else {
            // EINVAL... shouldn't happen, so let's complain to stderr
            // and hope someone sends us a bug report
            perror("select");
        }
    }

    int nevents = processThreadWakeUp(nsel);

    // activate socket notifiers
    if (! (flags & QEventLoop::ExcludeSocketNotifiers) && nsel > 0 && sn_highest >= 0) {
        // if select says data is ready on any socket, then set the socket notifier
        // to pending
        for (auto & i : sn_vec) {
            QSockNotType::List &list = i.list;
            for (auto sn : list) {
                if (FD_ISSET(sn->fd, &i.select_fds))
                    q->setSocketNotifierPending(sn->obj);
            }
        }
    }
    return (nevents + q->activateSocketNotifiers());
}

int QEventDispatcherUNIXPrivate::initThreadWakeUp()
{
    FD_SET(thread_pipe[0], &sn_vec[0].select_fds);
    return thread_pipe[0];
}

int QEventDispatcherUNIXPrivate::processThreadWakeUp(int nsel)
{
    if (nsel > 0 && FD_ISSET(thread_pipe[0], &sn_vec[0].select_fds)) {
        // some other thread woke us up... consume the data on the thread pipe so that
        // select doesn't immediately return next time
        char c[16];
        while (::read(thread_pipe[0], c, sizeof(c)) > 0)
            ;
        if (!wakeUps.testAndSetRelease(1, 0)) {
            // hopefully, this is dead code
            qWarning("QEventDispatcherUNIX: internal error, wakeUps.testAndSetRelease(1, 0) failed!");
        }
        return 1;
    }
    return 0;
}

QEventDispatcherUNIX::QEventDispatcherUNIX(QObject *parent)
    : QAbstractEventDispatcher(*new QEventDispatcherUNIXPrivate, parent)
{ }

QEventDispatcherUNIX::QEventDispatcherUNIX(QEventDispatcherUNIXPrivate &dd, QObject *parent)
    : QAbstractEventDispatcher(dd, parent)
{ }

QEventDispatcherUNIX::~QEventDispatcherUNIX()
= default;

int QEventDispatcherUNIX::select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
                                 timespec *timeout)
{
    return qt_safe_select(nfds, readfds, writefds, exceptfds, timeout);
}

/*!
    \internal
*/
void QEventDispatcherUNIX::registerTimer(int timerId, int interval, QObject *obj)
{
#ifndef QT_NO_DEBUG
    if (timerId < 1 || interval < 0 || !obj) {
        qWarning("QEventDispatcherUNIX::registerTimer: invalid arguments");
        return;
    } else if (obj->thread() != thread() || thread() != QThread::currentThread()) {
        qWarning("QObject::startTimer: timers cannot be started from another thread");
        return;
    }
#endif

    Q_D(QEventDispatcherUNIX);
    d->timerList.registerTimer(timerId, interval, obj);
}

/*!
    \internal
*/
bool QEventDispatcherUNIX::unregisterTimer(int timerId)
{
#ifndef QT_NO_DEBUG
    if (timerId < 1) {
        qWarning("QEventDispatcherUNIX::unregisterTimer: invalid argument");
        return false;
    } else if (thread() != QThread::currentThread()) {
        qWarning("QObject::killTimer: timers cannot be stopped from another thread");
        return false;
    }
#endif

    Q_D(QEventDispatcherUNIX);
    return d->timerList.unregisterTimer(timerId);
}

/*!
    \internal
*/
bool QEventDispatcherUNIX::unregisterTimers(QObject *object)
{
#ifndef QT_NO_DEBUG
    if (!object) {
        qWarning("QEventDispatcherUNIX::unregisterTimers: invalid argument");
        return false;
    } else if (object->thread() != thread() || thread() != QThread::currentThread()) {
        qWarning("QObject::killTimers: timers cannot be stopped from another thread");
        return false;
    }
#endif

    Q_D(QEventDispatcherUNIX);
    return d->timerList.unregisterTimers(object);
}

QList<QEventDispatcherUNIX::TimerInfo>
QEventDispatcherUNIX::registeredTimers(QObject *object) const
{
    if (!object) {
        qWarning("QEventDispatcherUNIX:registeredTimers: invalid argument");
        return QList<TimerInfo>();
    }

    Q_D(const QEventDispatcherUNIX);
    return d->timerList.registeredTimers(object);
}

/*****************************************************************************
 Socket notifier type
 *****************************************************************************/
QSockNotType::QSockNotType()
{
    FD_ZERO(&select_fds);
    FD_ZERO(&enabled_fds);
    FD_ZERO(&pending_fds);
}

QSockNotType::~QSockNotType()
{
    for (auto & i : list)
        delete i;
}

/*****************************************************************************
 QEventDispatcher implementations for UNIX
 *****************************************************************************/

void QEventDispatcherUNIX::registerSocketNotifier(QSocketNotifier *notifier)
{
    Q_ASSERT(notifier);
    int sockfd = notifier->socket();
    int type = notifier->type();
#ifndef QT_NO_DEBUG
    if (sockfd < 0
        || unsigned(sockfd) >= FD_SETSIZE) {
        qWarning("QSocketNotifier: Internal error");
        return;
    } else if (notifier->thread() != thread()
               || thread() != QThread::currentThread()) {
        qWarning("QSocketNotifier: socket notifiers cannot be enabled from another thread");
        return;
    }
#endif

    Q_D(QEventDispatcherUNIX);
    QSockNotType::List &list = d->sn_vec[type].list;
    fd_set *fds  = &d->sn_vec[type].enabled_fds;
    QSockNot *sn;

    sn = new QSockNot;
    sn->obj = notifier;
    sn->fd = sockfd;
    sn->queue = &d->sn_vec[type].pending_fds;

    int i;
    for (i = 0; i < list.size(); ++i) {
        QSockNot *p = list[i];
        if (p->fd < sockfd)
            break;
        if (p->fd == sockfd) {
            static const char *t[] = { "Read", "Write", "Exception" };
            qWarning("QSocketNotifier: Multiple socket notifiers for "
                      "same socket %d and type %s", sockfd, t[type]);
        }
    }
    list.insert(i, sn);

    FD_SET(sockfd, fds);
    d->sn_highest = std::max(d->sn_highest, sockfd);
}

void QEventDispatcherUNIX::unregisterSocketNotifier(QSocketNotifier *notifier)
{
    Q_ASSERT(notifier);
    int sockfd = notifier->socket();
    int type = notifier->type();
#ifndef QT_NO_DEBUG
    if (sockfd < 0
        || unsigned(sockfd) >= FD_SETSIZE) {
        qWarning("QSocketNotifier: Internal error");
        return;
    } else if (notifier->thread() != thread()
               || thread() != QThread::currentThread()) {
        qWarning("QSocketNotifier: socket notifiers cannot be disabled from another thread");
        return;
    }
#endif

    Q_D(QEventDispatcherUNIX);
    QSockNotType::List &list = d->sn_vec[type].list;
    fd_set *fds  =  &d->sn_vec[type].enabled_fds;
    QSockNot *sn = nullptr;
    int i;
    for (i = 0; i < list.size(); ++i) {
        sn = list[i];
        if(sn->obj == notifier && sn->fd == sockfd)
            break;
    }
    if (i == list.size()) // not found
        return;

    FD_CLR(sockfd, fds);                        // clear fd bit
    FD_CLR(sockfd, sn->queue);
    d->sn_pending_list.removeAll(sn);                // remove from activation list
    list.removeAt(i);                                // remove notifier found above
    delete sn;

    if (d->sn_highest == sockfd) {                // find highest fd
        d->sn_highest = -1;
        for (auto & i : d->sn_vec) {
            if (!i.list.isEmpty())
                d->sn_highest = std::max(d->sn_highest,  // list is fd-sorted
                                     i.list[0]->fd);
        }
    }
}

void QEventDispatcherUNIX::setSocketNotifierPending(QSocketNotifier *notifier)
{
    Q_ASSERT(notifier);
    int sockfd = notifier->socket();
    int type = notifier->type();
#ifndef QT_NO_DEBUG
    if (sockfd < 0
        || unsigned(sockfd) >= FD_SETSIZE) {
        qWarning("QSocketNotifier: Internal error");
        return;
    }
    Q_ASSERT(notifier->thread() == thread() && thread() == QThread::currentThread());
#endif

    Q_D(QEventDispatcherUNIX);
    QSockNotType::List &list = d->sn_vec[type].list;
    QSockNot *sn = nullptr;
    int i;
    for (i = 0; i < list.size(); ++i) {
        sn = list[i];
        if(sn->obj == notifier && sn->fd == sockfd)
            break;
    }
    if (i == list.size()) // not found
        return;

    // We choose a random activation order to be more fair under high load.
    // If a constant order is used and a peer early in the list can
    // saturate the IO, it might grab our attention completely.
    // Also, if we're using a straight list, the callback routines may
    // delete other entries from the list before those other entries are
    // processed.
    if (! FD_ISSET(sn->fd, sn->queue)) {
        if (d->sn_pending_list.isEmpty()) {
            d->sn_pending_list.append(sn);
        } else {
            d->sn_pending_list.insert((qrand() & 0xff) %
                                      (d->sn_pending_list.size()+1), sn);
        }
        FD_SET(sn->fd, sn->queue);
    }
}

int QEventDispatcherUNIX::activateTimers()
{
    Q_ASSERT(thread() == QThread::currentThread());
    Q_D(QEventDispatcherUNIX);
    return d->timerList.activateTimers();
}

int QEventDispatcherUNIX::activateSocketNotifiers()
{
    Q_D(QEventDispatcherUNIX);
    if (d->sn_pending_list.isEmpty())
        return 0;

    // activate entries
    int n_act = 0;
    QEvent event(QEvent::SockAct);
    while (!d->sn_pending_list.isEmpty()) {
        QSockNot *sn = d->sn_pending_list.takeFirst();
        if (FD_ISSET(sn->fd, sn->queue)) {
            FD_CLR(sn->fd, sn->queue);
            QCoreApplication::sendEvent(sn->obj, &event);
            ++n_act;
        }
    }
    return n_act;
}

bool QEventDispatcherUNIX::processEvents(QEventLoop::ProcessEventsFlags flags)
{
    Q_D(QEventDispatcherUNIX);
    d->interrupt.storeRelaxed(0);

    // we are awake, broadcast it
    emit awake();

    auto threadData = d->threadData.loadRelaxed();
    QEventLoop::ProcessEventsFlags excludeAllFlags
        = QEventLoop::ExcludeUserInputEvents
        | QEventLoop::ExcludeSocketNotifiers
        | QEventLoop::X11ExcludeTimers;
    if ((flags & excludeAllFlags) == excludeAllFlags)
        return false;
    if(( flags & excludeAllFlags ) != excludeAllFlags ) {
        QCoreApplicationPrivate::sendPostedEvents(nullptr, 0, threadData);
    }

    int nevents = 0;
    const bool canWait = (threadData->canWaitLocked()
                          && !d->interrupt.loadRelaxed()
                          && (flags & QEventLoop::WaitForMoreEvents));

    if (canWait)
        emit aboutToBlock();

    if (!d->interrupt.loadRelaxed()) {
        // return the maximum time we can wait for an event.
        timespec *tm = nullptr;
        timespec wait_tm = { 0l, 0l };
        if (!(flags & QEventLoop::X11ExcludeTimers)) {
            if (d->timerList.timerWait(wait_tm))
                tm = &wait_tm;
        }

        if (!canWait) {
            if (!tm)
                tm = &wait_tm;

            // no time to wait
            tm->tv_sec  = 0l;
            tm->tv_nsec = 0l;
        }

        nevents = d->doSelect(flags, tm);

        // activate timers
        if (! (flags & QEventLoop::X11ExcludeTimers)) {
            nevents += activateTimers();
        }
    }
    // return true if we handled events, false otherwise
    return (nevents > 0);
}

bool QEventDispatcherUNIX::hasPendingEvents()
{
    extern uint qGlobalPostedEventsCount(); // from qapplication.cpp
    return qGlobalPostedEventsCount();
}

void QEventDispatcherUNIX::wakeUp()
{
    Q_D(QEventDispatcherUNIX);
    if (d->wakeUps.testAndSetAcquire(0, 1)) {
        char c = 0;
        qt_safe_write( d->thread_pipe[1], &c, 1 );
    }
}

void QEventDispatcherUNIX::interrupt()
{
    Q_D(QEventDispatcherUNIX);
    d->interrupt.storeRelaxed(1);
    wakeUp();
}

void QEventDispatcherUNIX::flush()
{ }




void QCoreApplication::watchUnixSignal(int sig, bool watch)
{
    if (sig < NSIG) {
        struct sigaction sa;
        sigemptyset(&(sa.sa_mask));
        sa.sa_flags = 0;
        if (watch)
            sa.sa_handler = signalHandler;
        else
            sa.sa_handler = SIG_DFL;
        sigaction(sig, &sa, nullptr);
    }
}

QT_END_NAMESPACE

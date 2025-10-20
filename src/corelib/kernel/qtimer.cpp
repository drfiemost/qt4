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

#include "qtimer.h"
#include "qabstracteventdispatcher.h"
#include "qcoreapplication.h"
#include "qobject_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QTimer
    \brief The QTimer class provides repetitive and single-shot timers.

    \ingroup events


    The QTimer class provides a high-level programming interface for
    timers. To use it, create a QTimer, connect its timeout() signal
    to the appropriate slots, and call start(). From then on it will
    emit the timeout() signal at constant intervals.

    Example for a one second (1000 millisecond) timer (from the
    \l{widgets/analogclock}{Analog Clock} example):

    \snippet examples/widgets/analogclock/analogclock.cpp 4
    \snippet examples/widgets/analogclock/analogclock.cpp 5
    \snippet examples/widgets/analogclock/analogclock.cpp 6

    From then on, the \c update() slot is called every second.

    You can set a timer to time out only once by calling
    setSingleShot(true). You can also use the static
    QTimer::singleShot() function to call a slot after a specified
    interval:

    \snippet doc/src/snippets/timers/timers.cpp 3

    In multithreaded applications, you can use QTimer in any thread
    that has an event loop. To start an event loop from a non-GUI
    thread, use QThread::exec(). Qt uses the timer's
    \l{QObject::thread()}{thread affinity} to determine which thread
    will emit the \l{QTimer::}{timeout()} signal. Because of this, you
    must start and stop the timer in its thread; it is not possible to
    start a timer from another thread.

    As a special case, a QTimer with a timeout of 0 will time out as
    soon as all the events in the window system's event queue have
    been processed. This can be used to do heavy work while providing
    a snappy user interface:

    \snippet doc/src/snippets/timers/timers.cpp 4
    \snippet doc/src/snippets/timers/timers.cpp 5
    \snippet doc/src/snippets/timers/timers.cpp 6

    \c processOneThing() will from then on be called repeatedly. It
    should be written in such a way that it always returns quickly
    (typically after processing one data item) so that Qt can deliver
    events to widgets and stop the timer as soon as it has done all
    its work. This is the traditional way of implementing heavy work
    in GUI applications; multithreading is now becoming available on
    more and more platforms, and we expect that zero-millisecond
    QTimers will gradually be replaced by \l{QThread}s.

    \section1 Accuracy and Timer Resolution

    Timers will never time out earlier than the specified timeout value
    and they are not guaranteed to time out at the exact value specified.
    In many situations, they may time out late by a period of time that
    depends on the accuracy of the system timers.

    The accuracy of timers depends on the underlying operating system
    and hardware. Most platforms support a resolution of 1 millisecond,
    though the accuracy of the timer will not equal this resolution
    in many real-world situations.

    If Qt is unable to deliver the requested number of timer clicks,
    it will silently discard some.

    \section1 Alternatives to QTimer

    An alternative to using QTimer is to call QObject::startTimer()
    for your object and reimplement the QObject::timerEvent() event
    handler in your class (which must inherit QObject). The
    disadvantage is that timerEvent() does not support such
    high-level features as single-shot timers or signals.

    Another alternative to using QTimer is to use QBasicTimer. It is
    typically less cumbersome than using QObject::startTimer()
    directly. See \l{Timers} for an overview of all three approaches.

    Some operating systems limit the number of timers that may be
    used; Qt tries to work around these limitations.

    \sa QBasicTimer, QTimerEvent, QObject::timerEvent(), Timers,
        {Analog Clock Example}, {Wiggly Example}
*/

static const int INV_TIMER = -1;                // invalid timer id

/*!
    Constructs a timer with the given \a parent.
*/

QTimer::QTimer(QObject *parent)
    : QObject(parent), id(INV_TIMER), inter(0), del(0), single(0), nulltimer(0), type(Qt::CoarseTimer)
{
}


/*!
    Destroys the timer.
*/

QTimer::~QTimer()
{
    if (id != INV_TIMER)                        // stop running timer
        stop();
}


/*!
    \fn void QTimer::timeout()

    This signal is emitted when the timer times out.

    \sa interval, start(), stop()
*/

/*!
    \property QTimer::active
    \since 4.3

    This boolean property is true if the timer is running; otherwise
    false.
*/

/*!
    \fn bool QTimer::isActive() const

    Returns true if the timer is running (pending); otherwise returns
    false.
*/

/*!
    \fn int QTimer::timerId() const

    Returns the ID of the timer if the timer is running; otherwise returns
    -1.
*/


/*! \overload start()

    Starts or restarts the timer with the timeout specified in \l interval.

    If the timer is already running, it will be
    \l{QTimer::stop()}{stopped} and restarted.

    If \l singleShot is true, the timer will be activated only once.
*/
void QTimer::start()
{
    if (id != INV_TIMER)                        // stop running timer
        stop();
    nulltimer = (!inter && single);
    id = QObject::startTimer(inter, Qt::TimerType(type));
}

/*!
    Starts or restarts the timer with a timeout interval of \a msec
    milliseconds.

    If the timer is already running, it will be
    \l{QTimer::stop()}{stopped} and restarted.

    If \l singleShot is true, the timer will be activated only once.

*/
void QTimer::start(int msec)
{
    inter = msec;
    start();
}


/*!
    Stops the timer.

    \sa start()
*/

void QTimer::stop()
{
    if (id != INV_TIMER) {
        QObject::killTimer(id);
        id = INV_TIMER;
    }
}


/*!
  \reimp
*/
void QTimer::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == id) {
        if (single)
            stop();
        emit timeout();
    }
}

class QSingleShotTimer : public QObject
{
    Q_OBJECT
    int timerId;
    bool hasValidReceiver;
    QWeakPointer<const QObject> receiver;
    QtPrivate::QSlotObjectBase *slotObj;
public:
    ~QSingleShotTimer() override;
    QSingleShotTimer(int msec, Qt::TimerType timerType, const QObject *r, const char * m);
    QSingleShotTimer(int msec, Qt::TimerType timerType, const QObject *r, QtPrivate::QSlotObjectBase *slotObj);

Q_SIGNALS:
    void timeout();
protected:
    void timerEvent(QTimerEvent *) override;
};

QSingleShotTimer::QSingleShotTimer(int msec, Qt::TimerType timerType, const QObject *r, const char *member)
    : QObject(QAbstractEventDispatcher::instance()), hasValidReceiver(true), slotObj(nullptr)
{
    timerId = startTimer(msec, timerType);
    connect(this, SIGNAL(timeout()), r, member);
}

QSingleShotTimer::QSingleShotTimer(int msec, Qt::TimerType timerType, const QObject *r, QtPrivate::QSlotObjectBase *slotObj)
    : QObject(QAbstractEventDispatcher::instance()), hasValidReceiver(r), receiver(r), slotObj(slotObj)
{
    timerId = startTimer(msec, timerType);
    if (r && thread() != r->thread()) {
        // We need the invocation to happen in the receiver object's thread.
        // So, move QSingleShotTimer to the correct thread. Before that occurs, we
        // shall remove the parent from the object.
        setParent(nullptr);
        moveToThread(r->thread());

        // Given we're also parentless now, we should take defence against leaks
        // in case the application quits before we expire.
        connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, &QObject::deleteLater);
    }
}

QSingleShotTimer::~QSingleShotTimer()
{
    if (timerId > 0)
        killTimer(timerId);
    if (slotObj)
        slotObj->destroyIfLastRef();
}

void QSingleShotTimer::timerEvent(QTimerEvent *)
{
    // need to kill the timer _before_ we emit timeout() in case the
    // slot connected to timeout calls processEvents()
    if (timerId > 0)
        killTimer(timerId);
    timerId = -1;

    if (slotObj) {
        // If the receiver was destroyed, skip this part
        if (Q_LIKELY(!receiver.isNull() || !hasValidReceiver)) {
            // We allocate only the return type - we previously checked the function had
            // no arguments.
            void *args[1] = { 0 };
            slotObj->call(const_cast<QObject*>(receiver.data()), args);
        }
    } else {
        emit timeout();
    }

    // we would like to use delete later here, but it feels like a
    // waste to post a new event to handle this event, so we just unset the flag
    // and explicitly delete...
    qDeleteInEventHandler(this);
}

/*!
    \internal

    Implementation of the template version of singleShot

    \a msec is the timer interval
    \a timerType is the timer type
    \a receiver is the receiver object, can be null. In such a case, it will be the same
                as the final sender class.
    \a slot a pointer only used when using Qt::UniqueConnection
    \a slotObj the slot object
 */
void QTimer::singleShotImpl(int msec, Qt::TimerType timerType,
                            const QObject *receiver,
                            QtPrivate::QSlotObjectBase *slotObj)
{
    new QSingleShotTimer(msec, timerType, receiver, slotObj);
}

QT_BEGIN_INCLUDE_NAMESPACE
#include "qtimer.moc"
QT_END_INCLUDE_NAMESPACE

/*!
    \reentrant
    This static function calls a slot after a given time interval.

    It is very convenient to use this function because you do not need
    to bother with a \link QObject::timerEvent() timerEvent\endlink or
    create a local QTimer object.

    Example:
    \snippet doc/src/snippets/code/src_corelib_kernel_qtimer.cpp 0

    This sample program automatically terminates after 10 minutes
    (600,000 milliseconds).

    The \a receiver is the receiving object and the \a member is the
    slot. The time interval is \a msec milliseconds.

    \sa start()
*/

void QTimer::singleShot(int msec, const QObject *receiver, const char *member)
{
    singleShot(msec, Qt::CoarseTimer, receiver, member);
}

/*! \overload
    \reentrant
    This static function calls a slot after a given time interval.
    It is very convenient to use this function because you do not need
    to bother with a \link QObject::timerEvent() timerEvent\endlink or
    create a local QTimer object.
    The \a receiver is the receiving object and the \a member is the slot. The
    time interval is \a msec milliseconds. The \a timerType affects the
    accuracy of the timer.
    \sa start()
*/
void QTimer::singleShot(int msec, Qt::TimerType timerType, const QObject *receiver, const char *member)
{
    if (receiver && member) {
        if (msec == 0) {
            // special code shortpath for 0-timers
            const char* bracketPosition = strchr(member, '(');
            if (!bracketPosition || !(member[0] >= '0' && member[0] <= '3')) {
                qWarning("QTimer::singleShot: Invalid slot specification");
                return;
            }
            QByteArray methodName(member+1, bracketPosition - 1 - member); // extract method name
            QMetaObject::invokeMethod(const_cast<QObject *>(receiver), methodName.constData(), Qt::QueuedConnection);
            return;
        }
        (void) new QSingleShotTimer(msec, timerType, receiver, member);
    }
}

/*!\fn void QTimer::singleShot(int msec, const QObject *receiver, PointerToMemberFunction method)

    \overload
    \reentrant
    This static function calls a member function of a QObject after a given time interval.

    It is very convenient to use this function because you do not need
    to bother with a \l{QObject::timerEvent()}{timerEvent} or
    create a local QTimer object.

    The \a receiver is the receiving object and the \a method is the member function. The
    time interval is \a msec milliseconds.

    If \a receiver is destroyed before the interval occurs, the method will not be called.
    The function will be run in the thread of \a receiver. The receiver's thread must have
    a running Qt event loop.

    \sa start()
*/

/*!\fn void QTimer::singleShot(int msec, Qt::TimerType timerType, const QObject *receiver, PointerToMemberFunction method)

    \overload
    \reentrant
    This static function calls a member function of a QObject after a given time interval.

    It is very convenient to use this function because you do not need
    to bother with a \l{QObject::timerEvent()}{timerEvent} or
    create a local QTimer object.

    The \a receiver is the receiving object and the \a method is the member function. The
    time interval is \a msec milliseconds. The \a timerType affects the
    accuracy of the timer.

    If \a receiver is destroyed before the interval occurs, the method will not be called.
    The function will be run in the thread of \a receiver. The receiver's thread must have
    a running Qt event loop.

    \sa start()
*/

/*!\fn void QTimer::singleShot(int msec, Functor functor)

    \overload
    \reentrant
    This static function calls \a functor after a given time interval.

    It is very convenient to use this function because you do not need
    to bother with a \l{QObject::timerEvent()}{timerEvent} or
    create a local QTimer object.

    The time interval is \a msec milliseconds.

    \sa start()
*/

/*!\fn void QTimer::singleShot(int msec, Qt::TimerType timerType, Functor functor)

    \overload
    \reentrant
    This static function calls \a functor after a given time interval.

    It is very convenient to use this function because you do not need
    to bother with a \l{QObject::timerEvent()}{timerEvent} or
    create a local QTimer object.

    The time interval is \a msec milliseconds. The \a timerType affects the
    accuracy of the timer.

    \sa start()
*/

/*!\fn void QTimer::singleShot(int msec, const QObject *context, Functor functor)

    \overload
    \reentrant
    This static function calls \a functor after a given time interval.

    It is very convenient to use this function because you do not need
    to bother with a \l{QObject::timerEvent()}{timerEvent} or
    create a local QTimer object.

    The time interval is \a msec milliseconds.

    If \a context is destroyed before the interval occurs, the method will not be called.
    The function will be run in the thread of \a context. The context's thread must have
    a running Qt event loop.

    \sa start()
*/

/*!\fn void QTimer::singleShot(int msec, Qt::TimerType timerType, const QObject *context, Functor functor)

    \overload
    \reentrant
    This static function calls \a functor after a given time interval.

    It is very convenient to use this function because you do not need
    to bother with a \l{QObject::timerEvent()}{timerEvent} or
    create a local QTimer object.

    The time interval is \a msec milliseconds. The \a timerType affects the
    accuracy of the timer.

    If \a context is destroyed before the interval occurs, the method will not be called.
    The function will be run in the thread of \a context. The context's thread must have
    a running Qt event loop.

    \sa start()
*/

/*!
    \property QTimer::singleShot
    \brief whether the timer is a single-shot timer

    A single-shot timer fires only once, non-single-shot timers fire
    every \l interval milliseconds.

    \sa interval, singleShot()
*/

/*!
    \property QTimer::interval
    \brief the timeout interval in milliseconds

    The default value for this property is 0.  A QTimer with a timeout
    interval of 0 will time out as soon as all the events in the window
    system's event queue have been processed.

    Setting the interval of an active timer changes its timerId().

    \sa singleShot
*/
void QTimer::setInterval(int msec)
{
    inter = msec;
    if (id != INV_TIMER) {                        // create new timer
        QObject::killTimer(id);                        // restart timer
        id = QObject::startTimer(msec, Qt::TimerType(type));
    }
}

/*! \fn void QTimer::changeInterval(int msec)

   Use setInterval(msec) or start(msec) instead.
*/

/*!
    \property QTimer::timerType
    \brief controls the accuracy of the timer
    The default value for this property is \c Qt::CoarseTimer.
    \sa Qt::TimerType
*/

QT_END_NAMESPACE

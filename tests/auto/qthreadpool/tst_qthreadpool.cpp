/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
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
#include <QtTest/QtTest>
#include <qdatetime.h>
#include <qthreadpool.h>
#include <qstring.h>
#include <qmutex.h>

// From Qt5

#define QTRY_COMPARE_WITH_TIMEOUT(__expr, __expected, __timeout) \
    do { \
            const int __step = 50; \
            const int __timeoutValue = __timeout; \
            if ((__expr) != (__expected)) { \
                        QTest::qWait(0); \
                    } \
            for (int __i = 0; __i < __timeoutValue && ((__expr) != (__expected)); __i+=__step) { \
                        QTest::qWait(__step); \
                    } \
            QCOMPARE(__expr, __expected); \
    } while (0)

#define QTRY_COMPARE(__expr, __expected) QTRY_COMPARE_WITH_TIMEOUT(__expr, __expected, 5000)


typedef void (*FunctionPointer)();

class FunctionPointerTask : public QRunnable
{
public:
    FunctionPointerTask(FunctionPointer function)
    :function(function) {}
    void run() { function(); }
private:
    FunctionPointer function;
};

QRunnable *createTask(FunctionPointer pointer)
{
    return new FunctionPointerTask(pointer);
}

class tst_QThreadPool : public QObject
{
    Q_OBJECT
public:
    tst_QThreadPool();
    ~tst_QThreadPool();

    static QMutex *functionTestMutex;

private slots:
    void runFunction();
    void createThreadRunFunction();
    void runMultiple();
    void waitcomplete();
    void runTask();
    void singleton();
    void destruction();
    void threadRecycling();
    void expiryTimeout();
    void expiryTimeoutRace();
    void exceptions();
    void maxThreadCount();
    void setMaxThreadCount_data();
    void setMaxThreadCount();
    void setMaxThreadCountStartsAndStopsThreads();
    void activeThreadCount();
    void reserveThread_data();
    void reserveThread();
    void releaseThread_data();
    void releaseThread();
    void reserveAndStart();
    void start();
    void tryStart();
    void tryStartPeakThreadCount();
    void tryStartCount();
    void waitForDone();
    void waitForDoneTimeout();
    void destroyingWaitsForTasksToFinish();
    void stressTest();

private:
    QMutex m_functionTestMutex;
};


QMutex *tst_QThreadPool::functionTestMutex = 0;

tst_QThreadPool::tst_QThreadPool()
{
    tst_QThreadPool::functionTestMutex = &m_functionTestMutex;
}

tst_QThreadPool::~tst_QThreadPool()
{
    tst_QThreadPool::functionTestMutex = 0;
}

int testFunctionCount;

void sleepTestFunction()
{
    QTest::qSleep(1000);
    ++testFunctionCount;
}

void emptyFunct()
{

}

void noSleepTestFunction()
{
    ++testFunctionCount;
}

void sleepTestFunctionMutex()
{
    Q_ASSERT(tst_QThreadPool::functionTestMutex);
    QTest::qSleep(1000);
    tst_QThreadPool::functionTestMutex->lock();
    ++testFunctionCount;
    tst_QThreadPool::functionTestMutex->unlock();
}

void noSleepTestFunctionMutex()
{
    Q_ASSERT(tst_QThreadPool::functionTestMutex);
    tst_QThreadPool::functionTestMutex->lock();
    ++testFunctionCount;
    tst_QThreadPool::functionTestMutex->unlock();
}

void tst_QThreadPool::runFunction()
{
    {
        QThreadPool manager;
        testFunctionCount = 0;
        manager.start(createTask(noSleepTestFunction));
    }
    QCOMPARE(testFunctionCount, 1);
}

void tst_QThreadPool::createThreadRunFunction()
{
    {
        QThreadPool manager;
        testFunctionCount = 0;
        manager.start(createTask(noSleepTestFunction));
    }

    QCOMPARE(testFunctionCount, 1);
}

void tst_QThreadPool::runMultiple()
{
    const int runs = 10;

    {
        QThreadPool manager;
        testFunctionCount = 0;
        for (int i = 0; i < runs; ++i) {
            manager.start(createTask(sleepTestFunctionMutex));
        }
    }
    QCOMPARE(testFunctionCount, runs);

    {
        QThreadPool manager;
        testFunctionCount = 0;
        for (int i = 0; i < runs; ++i) {
            manager.start(createTask(noSleepTestFunctionMutex));
        }
    }
    QCOMPARE(testFunctionCount, runs);

    {
        QThreadPool manager;
        for (int i = 0; i < 500; ++i)
            manager.start(createTask(emptyFunct));
    }
}

void tst_QThreadPool::waitcomplete()
{
    testFunctionCount = 0;
    const int runs = 500;
    for (int i = 0; i < 500; ++i) {
        QThreadPool pool;
        pool.start(createTask(noSleepTestFunction));
    }
    QCOMPARE(testFunctionCount, runs);
}

volatile bool ran;
class TestTask : public QRunnable
{
public:
    void run()
    {
        ran = true;
    }
};

void tst_QThreadPool::runTask()
{
    QThreadPool manager;
    ran = false;
    manager.start(new TestTask());
    // Hang if task is not runned.
    while (ran == false)
        QTest::qSleep(100); // no busy loop - this doesn't work with FIFO schedulers
}

/*
    Test running via QThreadPool::globalInstance()
*/
void tst_QThreadPool::singleton()
{
    ran = false;
    QThreadPool::globalInstance()->start(new TestTask());
    while (ran == false)
        QTest::qSleep(100); // no busy loop - this doesn't work with FIFO schedulers
}

int *value = 0;
class IntAccessor : public QRunnable
{
public:
    void run()
    {
        for (int i = 0; i < 100; ++i) {
            ++(*value);
            QTest::qSleep(1);
        }
    }
};

/*
    Test that the ThreadManager destructor waits until
    all threads have completed.
*/
void tst_QThreadPool::destruction()
{
    value = new int;
    QThreadPool *threadManager = new QThreadPool();
    threadManager->start(new IntAccessor());
    threadManager->start(new IntAccessor());
    delete threadManager;
    delete value;
    value = 0;
}

QSemaphore threadRecyclingSemaphore;
QThread *recycledThread = 0;

class ThreadRecorderTask : public QRunnable
{
public:
    void run()
    {
        recycledThread = QThread::currentThread();
        threadRecyclingSemaphore.release();
    }
};

/*
    Test that the thread pool really reuses threads.
*/
void tst_QThreadPool::threadRecycling()
{
    QThreadPool threadPool;

    threadPool.start(new ThreadRecorderTask());
    threadRecyclingSemaphore.acquire();
    QThread *thread1 = recycledThread;

    QTest::qSleep(100);

    threadPool.start(new ThreadRecorderTask());
    threadRecyclingSemaphore.acquire();
    QThread *thread2 = recycledThread;
    QCOMPARE(thread1, thread2);

    QTest::qSleep(100);

    threadPool.start(new ThreadRecorderTask());
    threadRecyclingSemaphore.acquire();
    QThread *thread3 = recycledThread;
    QCOMPARE(thread2, thread3);
}

class ExpiryTimeoutTask : public QRunnable
{
public:
    QThread *thread;
    QAtomicInt runCount;
    QSemaphore semaphore;

    ExpiryTimeoutTask()
        : thread(0), runCount(0)
    {
        setAutoDelete(false);
    }

    void run()
    {
        thread = QThread::currentThread();
        runCount.ref();
        semaphore.release();
    }
};

void tst_QThreadPool::expiryTimeout()
{
    ExpiryTimeoutTask task;

    QThreadPool threadPool;
    threadPool.setMaxThreadCount(1);

    int expiryTimeout = threadPool.expiryTimeout();
    threadPool.setExpiryTimeout(1000);
    QCOMPARE(threadPool.expiryTimeout(), 1000);

    // run the task
    threadPool.start(&task);
    QVERIFY(task.semaphore.tryAcquire(1, 10000));
    QCOMPARE(int(task.runCount), 1);
    QVERIFY(!task.thread->wait(100));
    // thread should expire
    QThread *firstThread = task.thread;
    QVERIFY(task.thread->wait(10000));

    // run task again, thread should be restarted
    threadPool.start(&task);
    QVERIFY(task.semaphore.tryAcquire(1, 10000));
    QCOMPARE(int(task.runCount), 2);
    QVERIFY(!task.thread->wait(100));
    // thread should expire again
    QVERIFY(task.thread->wait(10000));

    // thread pool should have reused the expired thread (instead of
    // starting a new one)
    QCOMPARE(firstThread, task.thread);

    threadPool.setExpiryTimeout(expiryTimeout);
    QCOMPARE(threadPool.expiryTimeout(), expiryTimeout);
}

void tst_QThreadPool::expiryTimeoutRace() // QTBUG-3786
{
    ExpiryTimeoutTask task;

    QThreadPool threadPool;
    threadPool.setMaxThreadCount(1);
    threadPool.setExpiryTimeout(50);
    const int numTasks = 20;
    for (int i = 0; i < numTasks; ++i) {
        threadPool.start(&task);
        QTest::qSleep(50); // exactly the same as the expiry timeout
    }
    QCOMPARE(int(task.runCount), numTasks);
    QVERIFY(threadPool.waitForDone(2000));
}

#ifndef QT_NO_EXCEPTIONS
class ExceptionTask : public QRunnable
{
public:
    void run()
    {
        throw new int;
    }
};
#endif

void tst_QThreadPool::exceptions()
{
#ifndef QT_NO_EXCEPTIONS
    ExceptionTask task;
    {
        QThreadPool threadPool;
//  Uncomment this for a nice crash.
//        threadPool.start(&task);
    }
#else
    QSKIP("No exception support", SkipAll);
#endif
}

void tst_QThreadPool::maxThreadCount()
{
    DEPENDS_ON("setMaxThreadCount()");
}

void tst_QThreadPool::setMaxThreadCount_data()
{
    QTest::addColumn<int>("limit");

    QTest::newRow("") << 1;
    QTest::newRow("") << -1;
    QTest::newRow("") << 2;
    QTest::newRow("") << -2;
    QTest::newRow("") << 4;
    QTest::newRow("") << -4;
    QTest::newRow("") << 0;
    QTest::newRow("") << 12345;
    QTest::newRow("") << -6789;
    QTest::newRow("") << 42;
    QTest::newRow("") << -666;
}

void tst_QThreadPool::setMaxThreadCount()
{
    QFETCH(int, limit);
    QThreadPool *threadPool = QThreadPool::globalInstance();
    int savedLimit = threadPool->maxThreadCount();

    // maxThreadCount() should always return the previous argument to
    // setMaxThreadCount(), regardless of input
    threadPool->setMaxThreadCount(limit);
    QCOMPARE(threadPool->maxThreadCount(), limit);

    // the value returned from maxThreadCount() should always be valid input for setMaxThreadCount()
    threadPool->setMaxThreadCount(savedLimit);
    QCOMPARE(threadPool->maxThreadCount(), savedLimit);

    // setting the limit on children should have no effect on the parent
    {
        QThreadPool threadPool2(threadPool);
        savedLimit = threadPool2.maxThreadCount();

        // maxThreadCount() should always return the previous argument to
        // setMaxThreadCount(), regardless of input
        threadPool2.setMaxThreadCount(limit);
        QCOMPARE(threadPool2.maxThreadCount(), limit);

        // the value returned from maxThreadCount() should always be valid input for setMaxThreadCount()
        threadPool2.setMaxThreadCount(savedLimit);
        QCOMPARE(threadPool2.maxThreadCount(), savedLimit);
    }
}

void tst_QThreadPool::setMaxThreadCountStartsAndStopsThreads()
{
    class WaitingTask : public QRunnable
    {
    public:
        QSemaphore waitForStarted, waitToFinish;

        WaitingTask() { setAutoDelete(false); }

        void run()
        {
            waitForStarted.release();
            waitToFinish.acquire();
        }
    };

    QThreadPool threadPool;
    threadPool.setMaxThreadCount(1);

    WaitingTask *task = new WaitingTask;
    threadPool.start(task);
    QVERIFY(task->waitForStarted.tryAcquire(1, 1000));

    // thread limit is 1, cannot start more tasks
    threadPool.start(task);
    QVERIFY(!task->waitForStarted.tryAcquire(1, 1000));

    // increasing the limit by 1 should start the task immediately
    threadPool.setMaxThreadCount(2);
    QVERIFY(task->waitForStarted.tryAcquire(1, 1000));

    // ... but we still cannot start more tasks
    threadPool.start(task);
    QVERIFY(!task->waitForStarted.tryAcquire(1, 1000));

    // increasing the limit should be able to start more than one at a time
    threadPool.start(task);
    threadPool.setMaxThreadCount(4);
    QVERIFY(task->waitForStarted.tryAcquire(2, 1000));

    // ... but we still cannot start more tasks
    threadPool.start(task);
    threadPool.start(task);
    QVERIFY(!task->waitForStarted.tryAcquire(2, 1000));

    // decreasing the thread limit should cause the active thread count to go down
    threadPool.setMaxThreadCount(2);
    QCOMPARE(threadPool.activeThreadCount(), 4);
    task->waitToFinish.release(2);
    QTest::qWait(1000);
    QCOMPARE(threadPool.activeThreadCount(), 2);

    // ... and we still cannot start more tasks
    threadPool.start(task);
    threadPool.start(task);
    QVERIFY(!task->waitForStarted.tryAcquire(2, 1000));

    // start all remaining tasks
    threadPool.start(task);
    threadPool.start(task);
    threadPool.start(task);
    threadPool.start(task);
    threadPool.setMaxThreadCount(8);
    QVERIFY(task->waitForStarted.tryAcquire(6, 1000));

    task->waitToFinish.release(10);
//    delete task;
}


void tst_QThreadPool::activeThreadCount()
{
    DEPENDS_ON("tryReserveThread()");
    DEPENDS_ON("reserveThread()");
    DEPENDS_ON("releaseThread()");
}

void tst_QThreadPool::reserveThread_data()
{
    setMaxThreadCount_data();
}

void tst_QThreadPool::reserveThread()
{
    QFETCH(int, limit);
    QThreadPool *threadpool = QThreadPool::globalInstance();
    int savedLimit = threadpool->maxThreadCount();
    threadpool->setMaxThreadCount(limit);

    // reserve up to the limit
    for (int i = 0; i < limit; ++i)
        threadpool->reserveThread();

    // reserveThread() should always reserve a thread, regardless of
    // how many have been previously reserved
    threadpool->reserveThread();
    QCOMPARE(threadpool->activeThreadCount(), (limit > 0 ? limit : 0) + 1);
    threadpool->reserveThread();
    QCOMPARE(threadpool->activeThreadCount(), (limit > 0 ? limit : 0) + 2);

    // cleanup
    threadpool->releaseThread();
    threadpool->releaseThread();
    for (int i = 0; i < limit; ++i)
        threadpool->releaseThread();

    // reserving threads in children should not effect the parent
    {
        QThreadPool threadpool2(threadpool);
        threadpool2.setMaxThreadCount(limit);

        // reserve up to the limit
        for (int i = 0; i < limit; ++i)
            threadpool2.reserveThread();

        // reserveThread() should always reserve a thread, regardless
        // of how many have been previously reserved
        threadpool2.reserveThread();
        QCOMPARE(threadpool2.activeThreadCount(), (limit > 0 ? limit : 0) + 1);
        threadpool2.reserveThread();
        QCOMPARE(threadpool2.activeThreadCount(), (limit > 0 ? limit : 0) + 2);

        threadpool->reserveThread();
        QCOMPARE(threadpool->activeThreadCount(), 1);
        threadpool->reserveThread();
        QCOMPARE(threadpool->activeThreadCount(), 2);

        // cleanup
        threadpool2.releaseThread();
        threadpool2.releaseThread();
        threadpool->releaseThread();
        threadpool->releaseThread();
        while (threadpool2.activeThreadCount() > 0)
            threadpool2.releaseThread();
    }

    // reset limit on global QThreadPool
    threadpool->setMaxThreadCount(savedLimit);
}

void tst_QThreadPool::releaseThread_data()
{
    setMaxThreadCount_data();
}

void tst_QThreadPool::releaseThread()
{
    QFETCH(int, limit);
    QThreadPool *threadpool = QThreadPool::globalInstance();
    int savedLimit = threadpool->maxThreadCount();
    threadpool->setMaxThreadCount(limit);

    // reserve up to the limit
    for (int i = 0; i < limit; ++i)
        threadpool->reserveThread();

    // release should decrease the number of reserved threads
    int reserved = threadpool->activeThreadCount();
    while (reserved-- > 0) {
        threadpool->releaseThread();
        QCOMPARE(threadpool->activeThreadCount(), reserved);
    }
    QCOMPARE(threadpool->activeThreadCount(), 0);

    // releaseThread() can release more than have been reserved
    threadpool->releaseThread();
    QCOMPARE(threadpool->activeThreadCount(), -1);
    threadpool->reserveThread();
    QCOMPARE(threadpool->activeThreadCount(), 0);

    // releasing threads in children should not effect the parent
    {
        QThreadPool threadpool2(threadpool);
        threadpool2.setMaxThreadCount(limit);

        // reserve up to the limit
        for (int i = 0; i < limit; ++i)
            threadpool2.reserveThread();

        // release should decrease the number of reserved threads
        int reserved = threadpool2.activeThreadCount();
        while (reserved-- > 0) {
            threadpool2.releaseThread();
            QCOMPARE(threadpool2.activeThreadCount(), reserved);
            QCOMPARE(threadpool->activeThreadCount(), 0);
        }
        QCOMPARE(threadpool2.activeThreadCount(), 0);
        QCOMPARE(threadpool->activeThreadCount(), 0);

        // releaseThread() can release more than have been reserved
        threadpool2.releaseThread();
        QCOMPARE(threadpool2.activeThreadCount(), -1);
        QCOMPARE(threadpool->activeThreadCount(), 0);
        threadpool2.reserveThread();
        QCOMPARE(threadpool2.activeThreadCount(), 0);
        QCOMPARE(threadpool->activeThreadCount(), 0);
    }

    // reset limit on global QThreadPool
    threadpool->setMaxThreadCount(savedLimit);
}

void tst_QThreadPool::reserveAndStart() // QTBUG-21051
{
    class WaitingTask : public QRunnable
    {
    public:
        QAtomicInt count;
        QSemaphore waitForStarted;

        WaitingTask() { setAutoDelete(false); }

        void run()
        {
            count.ref();
            waitForStarted.release();
        }
    };

    // Set up
    QThreadPool *threadpool = QThreadPool::globalInstance();
    int savedLimit = threadpool->maxThreadCount();
    threadpool->setMaxThreadCount(1);
    QCOMPARE(threadpool->activeThreadCount(), 0);

    // reserve
    threadpool->reserveThread();
    QCOMPARE(threadpool->activeThreadCount(), 1);

    // start a task, to get a running thread
    WaitingTask *task = new WaitingTask;
    threadpool->start(task);
    QCOMPARE(threadpool->activeThreadCount(), 2);
    task->waitForStarted.acquire();
    QTRY_COMPARE(int(task->count), 1);
    QTRY_COMPARE(threadpool->activeThreadCount(), 1);

    // now the thread is waiting, but tryStart() will fail since activeThreadCount() >= maxThreadCount()
    QVERIFY(!threadpool->tryStart(task));
    QTRY_COMPARE(threadpool->activeThreadCount(), 1);

    // start() will therefore do a failing tryStart(), followed by enqueueTask()
    // which will actually wake up the waiting thread.
    threadpool->start(task);
    QTRY_COMPARE(threadpool->activeThreadCount(), 2);
    task->waitForStarted.acquire();
    QTRY_COMPARE(int(task->count), 2);
    QTRY_COMPARE(threadpool->activeThreadCount(), 1);

    threadpool->releaseThread();
    QTRY_COMPARE(threadpool->activeThreadCount(), 0);

    delete task;

    threadpool->setMaxThreadCount(savedLimit);
}

QAtomicInt count;
class CountingRunnable : public QRunnable
{
    public: void run()
    {
        count.ref();
    }
};

void tst_QThreadPool::start()
{
    const int runs = 1000;
    count = 0;
    {
        QThreadPool threadPool;
        for (int i = 0; i< runs; ++i) {
            threadPool.start(new CountingRunnable());
        }
    }
    QCOMPARE(int(count), runs);
}

void tst_QThreadPool::tryStart()
{
    class WaitingTask : public QRunnable
    {
    public:
        QSemaphore semaphore;

        WaitingTask() { setAutoDelete(false); }

        void run()
        {
            semaphore.acquire();
            count.ref();
        }
    };

    count = 0;

    WaitingTask task;
    QThreadPool threadPool;
    for (int i = 0; i < threadPool.maxThreadCount(); ++i) {
        threadPool.start(&task);
    }
    QVERIFY(!threadPool.tryStart(&task));
    task.semaphore.release(threadPool.maxThreadCount());
    threadPool.waitForDone();
    QCOMPARE(int(count), threadPool.maxThreadCount());
}

QMutex mutex;
int activeThreads = 0;
int peakActiveThreads = 0;
void tst_QThreadPool::tryStartPeakThreadCount()
{
    class CounterTask : public QRunnable
    {
    public:
        CounterTask() { setAutoDelete(false); }

        void run()
        {
            {
                QMutexLocker lock(&mutex);
                ++activeThreads;
                peakActiveThreads = std::max(peakActiveThreads, activeThreads);
            }

            QTest::qWait(100);
            {
                QMutexLocker lock(&mutex);
                --activeThreads;
            }
        }
    };

    CounterTask task;
    QThreadPool threadPool;

    for (int i = 0; i < 20; ++i) {
        if (threadPool.tryStart(&task) == false)
            QTest::qWait(10);
    }
    QCOMPARE(peakActiveThreads, QThread::idealThreadCount());

    for (int i = 0; i < 20; ++i) {
        if (threadPool.tryStart(&task) == false)
            QTest::qWait(10);
    }
    QCOMPARE(peakActiveThreads, QThread::idealThreadCount());
}

void tst_QThreadPool::tryStartCount()
{
    class SleeperTask : public QRunnable
    {
    public:
        SleeperTask() { setAutoDelete(false); }

        void run()
        {
            QTest::qWait(50);
        }
    };

    SleeperTask task;
    QThreadPool threadPool;
    const int runs = 5;

    for (int i = 0; i < runs; ++i) {
//        qDebug() << "iteration" << i;
        int count = 0;
        while (threadPool.tryStart(&task))
            ++count;
        QCOMPARE(count, QThread::idealThreadCount());

        QTest::qWait(100);
    }
}

void tst_QThreadPool::waitForDone()
{
    QTime total, pass;
    total.start();

    QThreadPool threadPool;
    while (total.elapsed() < 10000) {
        int runs;
        runs = count = 0;
        pass.restart();
        while (pass.elapsed() < 100) {
            threadPool.start(new CountingRunnable());
            ++runs;
        }
        threadPool.waitForDone();
        QCOMPARE(int(count), runs);

        runs = count = 0;
        pass.restart();
        while (pass.elapsed() < 100) {
            threadPool.start(new CountingRunnable());
            threadPool.start(new CountingRunnable());
            runs += 2;
        }
        threadPool.waitForDone();
        QCOMPARE(int(count), runs);
    }
}

void tst_QThreadPool::waitForDoneTimeout()
{
    class BlockedTask : public QRunnable
    {
    public:
      QMutex mutex;
      BlockedTask() { setAutoDelete(false); }
      
      void run()
        {
          mutex.lock();
          mutex.unlock();
          QTest::qSleep(50);
        }
    };

    QThreadPool threadPool;

    BlockedTask *task = new BlockedTask;
    task->mutex.lock();
    threadPool.start(task);
    QVERIFY(!threadPool.waitForDone(100));
    task->mutex.unlock();
    QVERIFY(threadPool.waitForDone(400));
}

void tst_QThreadPool::destroyingWaitsForTasksToFinish()
{
    QTime total, pass;
    total.start();

    while (total.elapsed() < 10000) {
        int runs;
        runs = count = 0;
        {
            QThreadPool threadPool;
            pass.restart();
            while (pass.elapsed() < 100) {
                threadPool.start(new CountingRunnable());
                ++runs;
            }
        }
        QCOMPARE(int(count), runs);

        runs = count = 0;
        {
            QThreadPool threadPool;
            pass.restart();
            while (pass.elapsed() < 100) {
                threadPool.start(new CountingRunnable());
                threadPool.start(new CountingRunnable());
                runs += 2;
            }
        }
        QCOMPARE(int(count), runs);
    }
}

void tst_QThreadPool::stressTest()
{
    class Task : public QRunnable
    {
        QSemaphore semaphore;
    public:
        Task() { setAutoDelete(false); }

        void start()
        {
            QThreadPool::globalInstance()->start(this);
        }

        void wait()
        {
            semaphore.acquire();
        }

        void run()
        {
            semaphore.release();
        }
    };

    QTime total;
    total.start();
    while (total.elapsed() < 30000) {
        Task t;
        t.start();
        t.wait();
    }
}

QTEST_MAIN(tst_QThreadPool);
#include "tst_qthreadpool.moc"

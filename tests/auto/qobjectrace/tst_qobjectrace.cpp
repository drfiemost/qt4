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


#include <QtCore>
#include <QtTest/QtTest>


enum { OneMinute = 60 * 1000,
       TwoMinutes = OneMinute * 2,
       TenMinutes = OneMinute * 10,
       TwentyFiveMinutes = OneMinute * 25 };

class tst_QObjectRace: public QObject
{
    Q_OBJECT
private slots:
    void moveToThreadRace();
    void destroyRace();
};

class RaceObject : public QObject
{
    Q_OBJECT
    QList<QThread *> threads;
    int count;

public:
    RaceObject()
        : count(0)
    { }

    void addThread(QThread *thread)
    { threads.append(thread); }

public slots:
    void theSlot()
    {
        enum { step = 35 };
        if ((++count % step) == 0) {
            QThread *nextThread = threads.at((count / step) % threads.size());
            moveToThread(nextThread);
        }
    }

    void destroSlot() {
        emit theSignal();
    }
signals:
    void theSignal();
};

class RaceThread : public QThread
{
    Q_OBJECT
    RaceObject *object;
    QTime stopWatch;

public:
    RaceThread()
        : object(0)
    { }

    void setObject(RaceObject *o)
    {
        object = o;
        object->addThread(this);
    }

    void start() {
        stopWatch.start();
        QThread::start();
    }

    void run() {
        QTimer zeroTimer;
        connect(&zeroTimer, SIGNAL(timeout()), object, SLOT(theSlot()));
        connect(&zeroTimer, SIGNAL(timeout()), this, SLOT(checkStopWatch()), Qt::DirectConnection);
        zeroTimer.start(0);
        (void) exec();
    }

signals:
    void theSignal();

private slots:
    void checkStopWatch()
    {
#if defined(Q_OS_WINCE)
        if (stopWatch.elapsed() >= OneMinute / 2)
#else
        if (stopWatch.elapsed() >= OneMinute)
#endif
            quit();

        QObject o;
        connect(&o, SIGNAL(destroyed()) , object, SLOT(destroSlot()));
        connect(object, SIGNAL(destroyed()) , &o, SLOT(deleteLater()));
    }
};

void tst_QObjectRace::moveToThreadRace()
{
    RaceObject *object = new RaceObject;

    enum { ThreadCount = 6 };
    RaceThread *threads[ThreadCount];
    for (int i = 0; i < ThreadCount; ++i) {
        threads[i] = new RaceThread;
        threads[i]->setObject(object);
    }

    object->moveToThread(threads[0]);

    for (int i = 0; i < ThreadCount; ++i)
        threads[i]->start();

    while(!threads[0]->isFinished()) {
        QPointer<RaceObject> foo (object);
        QObject o;
        connect(&o, SIGNAL(destroyed()) , object, SLOT(destroSlot()));
        connect(object, SIGNAL(destroyed()) , &o, SLOT(deleteLater()));
        QTest::qWait(10);
    }
    // the other threads should finish pretty quickly now
    for (int i = 1; i < ThreadCount; ++i)
        QVERIFY(threads[i]->wait(300));

    for (int i = 0; i < ThreadCount; ++i)
        delete threads[i];
    delete object;
}


class MyObject : public QObject
{   Q_OBJECT
    public slots:
        void slot1() { emit signal1(); }
        void slot2() { emit signal2(); }
        void slot3() { emit signal3(); }
        void slot4() { emit signal4(); }
        void slot5() { emit signal5(); }
        void slot6() { emit signal6(); }
        void slot7() { emit signal7(); }
    signals:
        void signal1();
        void signal2();
        void signal3();
        void signal4();
        void signal5();
        void signal6();
        void signal7();
};



class DestroyThread : public QThread
{
    Q_OBJECT
    QObject **objects;
    int number;

public:
    void setObjects(QObject **o, int n)
    {
        objects = o;
        number = n;
        for(int i = 0; i < number; i++)
            objects[i]->moveToThread(this);
    }

    void run() {
        for(int i = 0; i < number; i++)
            delete objects[i];
    }
};

#if defined(Q_OS_SYMBIAN)
// Symbian needs "a bit" more time
# define EXTRA_THREAD_WAIT TenMinutes
# define MAIN_THREAD_WAIT TwentyFiveMinutes
#else
# define EXTRA_THREAD_WAIT 3000
# define MAIN_THREAD_WAIT TwoMinutes
#endif

void tst_QObjectRace::destroyRace()
{
    enum { ThreadCount = 10, ObjectCountPerThread = 733,
           ObjectCount = ThreadCount * ObjectCountPerThread };

    const char *_slots[] = { SLOT(slot1()) , SLOT(slot2()) , SLOT(slot3()),
                             SLOT(slot4()) , SLOT(slot5()) , SLOT(slot6()),
                             SLOT(slot7()) };

    const char *_signals[] = { SIGNAL(signal1()), SIGNAL(signal2()), SIGNAL(signal3()),
                               SIGNAL(signal4()), SIGNAL(signal5()), SIGNAL(signal6()),
                               SIGNAL(signal7()) };

    QObject *objects[ObjectCount];
    for (int i = 0; i < ObjectCount; ++i)
        objects[i] = new MyObject;


    for (int i = 0; i < ObjectCount * 11; ++i) {
        connect(objects[(i*13) % ObjectCount], _signals[(2*i)%7],
                objects[((i+2)*17) % ObjectCount],  _slots[(3*i+2)%7] );
        connect(objects[((i+6)*23) % ObjectCount], _signals[(5*i+4)%7],
                objects[((i+8)*41) % ObjectCount],  _slots[(i+6)%7] );
    }

    DestroyThread *threads[ThreadCount];
    for (int i = 0; i < ThreadCount; ++i) {
        threads[i] = new DestroyThread;
        threads[i]->setObjects(objects + i*ObjectCountPerThread, ObjectCountPerThread);
    }

    for (int i = 0; i < ThreadCount; ++i)
        threads[i]->start();

    QVERIFY(threads[0]->wait(MAIN_THREAD_WAIT));
    // the other threads should finish pretty quickly now
    for (int i = 1; i < ThreadCount; ++i)
        QVERIFY(threads[i]->wait(EXTRA_THREAD_WAIT));

    for (int i = 0; i < ThreadCount; ++i)
        delete threads[i];
}


QTEST_MAIN(tst_QObjectRace)
#include "tst_qobjectrace.moc"

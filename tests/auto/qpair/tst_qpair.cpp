/****************************************************************************
**
** Copyright (C) 2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author Marc Mutz <marc.mutz@kdab.com>
** Contact: http://www.qt-project.org/
**
** This file is part of the test suite of the Qt Toolkit.
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>

#include <QPair>
#include <QSize>

class tst_QPair : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void pairOfReferences();
    void testConstexpr();
    void testConversions();
    void taskQTBUG_48780_pairContainingCArray();
};

class C { C() {} char _[4]; };
class M { M() {} char _[4]; };
class P { char _[4]; };

QT_BEGIN_NAMESPACE
Q_DECLARE_TYPEINFO(M, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(P, Q_PRIMITIVE_TYPE);
QT_END_NAMESPACE

// avoid the comma:
typedef QPair<C,C> QPairCC;
typedef QPair<C,M> QPairCM;
typedef QPair<C,P> QPairCP;
typedef QPair<M,C> QPairMC;
typedef QPair<M,M> QPairMM;
typedef QPair<M,P> QPairMP;
typedef QPair<P,C> QPairPC;
typedef QPair<P,M> QPairPM;
typedef QPair<P,P> QPairPP;

static_assert( QTypeInfo<QPairCC>::isComplex);
static_assert( QTypeInfo<QPairCC>::isStatic );

static_assert( QTypeInfo<QPairCM>::isComplex);
static_assert( QTypeInfo<QPairCM>::isStatic );

static_assert( QTypeInfo<QPairCP>::isComplex);
static_assert( QTypeInfo<QPairCP>::isStatic );

static_assert( QTypeInfo<QPairMC>::isComplex);
static_assert( QTypeInfo<QPairMC>::isStatic );

static_assert( QTypeInfo<QPairMM>::isComplex);
static_assert(!QTypeInfo<QPairMM>::isStatic );

static_assert( QTypeInfo<QPairMP>::isComplex);
static_assert(!QTypeInfo<QPairMP>::isStatic );

static_assert( QTypeInfo<QPairPC>::isComplex);
static_assert( QTypeInfo<QPairPC>::isStatic );

static_assert( QTypeInfo<QPairPM>::isComplex);
static_assert(!QTypeInfo<QPairPM>::isStatic );

static_assert(!QTypeInfo<QPairPP>::isComplex);
static_assert(!QTypeInfo<QPairPP>::isStatic );

static_assert(!QTypeInfo<QPairPP>::isPointer);

void tst_QPair::pairOfReferences()
{
    int i = 0;
    QString s;

    QPair<int&, QString&> p(i, s);

    p.first = 1;
    QCOMPARE(i, 1);

    i = 2;
    QCOMPARE(p.first, 2);

    p.second = QLatin1String("Hello");
    QCOMPARE(s, QLatin1String("Hello"));

    s = QLatin1String("olleH");
    QCOMPARE(p.second, QLatin1String("olleH"));

    QPair<int&, QString&> q = p;
    q.first = 3;
    QCOMPARE(i, 3);
    QCOMPARE(p.first, 3);

    q.second = QLatin1String("World");
    QCOMPARE(s, QLatin1String("World"));
    QCOMPARE(p.second, QLatin1String("World"));
}

void tst_QPair::testConstexpr()
{
    constexpr QPair<int, double> pID = qMakePair(0, 0.0);
    Q_UNUSED(pID);

    constexpr QPair<double, double> pDD  = qMakePair(0.0, 0.0);
    constexpr QPair<double, double> pDD2 = qMakePair(0, 0.0);   // involes (rvalue) conversion ctor
    constexpr bool equal = pDD2 == pDD;
    QVERIFY(equal);

    constexpr QPair<QSize, int> pSI = qMakePair(QSize(4, 5), 6);
    Q_UNUSED(pSI);
}

void tst_QPair::testConversions()
{
    // construction from lvalue:
    {
        const QPair<int, double> rhs(42, 4.5);
        const QPair<int, int> pii = rhs;
        QCOMPARE(pii.first, 42);
        QCOMPARE(pii.second, 4);

        const QPair<int, float> pif = rhs;
        QCOMPARE(pif.first, 42);
        QCOMPARE(pif.second, 4.5f);
    }

    // assignment from lvalue:
    {
        const QPair<int, double> rhs(42, 4.5);
        QPair<int, int> pii;
        pii = rhs;
        QCOMPARE(pii.first, 42);
        QCOMPARE(pii.second, 4);

        QPair<int, float> pif;
        pif = rhs;
        QCOMPARE(pif.first, 42);
        QCOMPARE(pif.second, 4.5f);
    }

    // construction from rvalue:
    {
#define rhs qMakePair(42, 4.5)
        const QPair<int, int> pii = rhs;
        QCOMPARE(pii.first, 42);
        QCOMPARE(pii.second, 4);

        const QPair<int, float> pif = rhs;
        QCOMPARE(pif.first, 42);
        QCOMPARE(pif.second, 4.5f);
#undef rhs
    }

    // assignment from rvalue:
    {
#define rhs qMakePair(42, 4.5)
        QPair<int, int> pii;
        pii = rhs;
        QCOMPARE(pii.first, 42);
        QCOMPARE(pii.second, 4);

        QPair<int, float> pif;
        pif = rhs;
        QCOMPARE(pif.first, 42);
        QCOMPARE(pif.second, 4.5f);
#undef rhs
    }
}

void tst_QPair::taskQTBUG_48780_pairContainingCArray()
{
    // compile-only:
    QPair<int[2], int> pair;
    pair.first[0] = 0;
    pair.first[1] = 1;
    pair.second = 2;
    Q_UNUSED(pair);
}

QTEST_APPLESS_MAIN(tst_QPair)
#include "tst_qpair.moc"

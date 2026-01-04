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
#include <QtGlobal>
#include "private/qnumeric_p.h"

#include <math.h>

class tst_QNumeric: public QObject
{
    Q_OBJECT

private slots:
    void fuzzyCompare_data();
    void fuzzyCompare();
    void qNan();
    void addOverflow_data();
    void addOverflow();
    void mulOverflow_data();
    void mulOverflow();
};

void tst_QNumeric::fuzzyCompare_data()
{
    QTest::addColumn<double>("val1");
    QTest::addColumn<double>("val2");
    QTest::addColumn<bool>("isEqual");

    QTest::newRow("zero") << 0.0 << 0.0 << true;
    QTest::newRow("ten") << 10.0 << 10.0 << true;
    QTest::newRow("large") << 1000000000.0 << 1000000000.0 << true;
    QTest::newRow("small") << 0.00000000001 << 0.00000000001 << true;
    QTest::newRow("eps") << 10.000000000000001 << 10.00000000000002 << true;
    QTest::newRow("eps2") << 10.000000000000001 << 10.000000000000009 << true;

    QTest::newRow("mis1") << 0.0 << 1.0 << false;
    QTest::newRow("mis2") << 0.0 << 10000000.0 << false;
    QTest::newRow("mis3") << 0.0 << 0.000000001 << false;
    QTest::newRow("mis4") << 100000000.0 << 0.000000001 << false;
    QTest::newRow("mis4") << 0.0000000001 << 0.000000001 << false;
}

void tst_QNumeric::fuzzyCompare()
{
    QFETCH(double, val1);
    QFETCH(double, val2);
    QFETCH(bool, isEqual);

    QCOMPARE(::qFuzzyCompare(val1, val2), isEqual);
    QCOMPARE(::qFuzzyCompare(val2, val1), isEqual);
    QCOMPARE(::qFuzzyCompare(-val1, -val2), isEqual);
    QCOMPARE(::qFuzzyCompare(-val2, -val1), isEqual);
}

void tst_QNumeric::qNan()
{
    double nan = qQNaN();
#if defined( __INTEL_COMPILER)
    QCOMPARE((0 > nan), false);
    QCOMPARE((0 < nan), false);
    QSKIP("This fails due to a bug in the Intel Compiler", SkipAll);
#else
    if (0 > nan)
        QFAIL("compiler thinks 0 > nan");

    if (0 < nan)
        QFAIL("compiler thinks 0 < nan");
#endif
    QVERIFY(qIsNaN(nan));
    QVERIFY(qIsNaN(nan + 1));
    QVERIFY(qIsNaN(-nan));
    double inf = qInf();
    QVERIFY(inf > 0);
    QVERIFY(-inf < 0);
    QVERIFY(qIsInf(inf));
    QVERIFY(qIsInf(-inf));
    QVERIFY(qIsInf(2*inf));
    QCOMPARE(1/inf, 0.0);
    QVERIFY(qIsNaN(0*nan));
    QVERIFY(qIsNaN(0*inf));
    QVERIFY(qFuzzyCompare(1/inf, 0.0));
}

void tst_QNumeric::addOverflow_data()
{
    QTest::addColumn<int>("size");
    QTest::newRow("quint8") << 8;
    QTest::newRow("quint16") << 16;
    QTest::newRow("quint32") << 32;
    QTest::newRow("quint64") << 64;
    QTest::newRow("ulong") << 48;   // it's either 32- or 64-bit, so on average it's 48 :-)
}

// Note: in release mode, all the tests may be statically determined and only the calls
// to QTest::toString and QTest::qCompare will remain.
template <typename Int> static void addOverflow_template()
{
#if defined(Q_CC_MSVC) && Q_CC_MSVC < 1900
    QSKIP("Test disabled, this test generates an Internal Compiler Error compiling");
#else
    const Int max = std::numeric_limits<Int>::max();
    Int r;

    // basic values
    QCOMPARE(add_overflow(Int(0), Int(0), &r), false);
    QCOMPARE(r, Int(0));
    QCOMPARE(add_overflow(Int(1), Int(0), &r), false);
    QCOMPARE(r, Int(1));
    QCOMPARE(add_overflow(Int(0), Int(1), &r), false);
    QCOMPARE(r, Int(1));

    // half-way through max
    QCOMPARE(add_overflow(Int(max/2), Int(max/2), &r), false);
    QCOMPARE(r, Int(max / 2 * 2));
    QCOMPARE(add_overflow(Int(max/2 - 1), Int(max/2 + 1), &r), false);
    QCOMPARE(r, Int(max / 2 * 2));
    QCOMPARE(add_overflow(Int(max/2 + 1), Int(max/2), &r), false);
    QCOMPARE(r, max);
    QCOMPARE(add_overflow(Int(max/2), Int(max/2 + 1), &r), false);
    QCOMPARE(r, max);

    // more than half
    QCOMPARE(add_overflow(Int(max/4 * 3), Int(max/4), &r), false);
    QCOMPARE(r, Int(max / 4 * 4));

    // max
    QCOMPARE(add_overflow(max, Int(0), &r), false);
    QCOMPARE(r, max);
    QCOMPARE(add_overflow(Int(0), max, &r), false);
    QCOMPARE(r, max);

    // 64-bit issues
    if (max > std::numeric_limits<uint>::max()) {
        QCOMPARE(add_overflow(Int(std::numeric_limits<uint>::max()), Int(std::numeric_limits<uint>::max()), &r), false);
        QCOMPARE(r, Int(2 * Int(std::numeric_limits<uint>::max())));
    }

    // overflows
    QCOMPARE(add_overflow(max, Int(1), &r), true);
    QCOMPARE(add_overflow(Int(1), max, &r), true);
    QCOMPARE(add_overflow(Int(max/2 + 1), Int(max/2 + 1), &r), true);
#endif
}

void tst_QNumeric::addOverflow()
{
    QFETCH(int, size);
    if (size == 8)
        addOverflow_template<quint8>();
    if (size == 16)
        addOverflow_template<quint16>();
    if (size == 32)
        addOverflow_template<quint32>();
    if (size == 48)
        addOverflow_template<ulong>();  // not really 48-bit
    if (size == 64)
        addOverflow_template<quint64>();
}

void tst_QNumeric::mulOverflow_data()
{
    addOverflow_data();
}

// Note: in release mode, all the tests may be statically determined and only the calls
// to QTest::toString and QTest::qCompare will remain.
template <typename Int> static void mulOverflow_template()
{
#if defined(Q_CC_MSVC) && Q_CC_MSVC < 1900
    QSKIP("Test disabled, this test generates an Internal Compiler Error compiling");
#else
    const Int max = std::numeric_limits<Int>::max();
    const Int middle = Int(max >> (sizeof(Int) * CHAR_BIT / 2));
    Int r;

    // basic multiplications
    QCOMPARE(mul_overflow(Int(0), Int(0), &r), false);
    QCOMPARE(r, Int(0));
    QCOMPARE(mul_overflow(Int(1), Int(0), &r), false);
    QCOMPARE(r, Int(0));
    QCOMPARE(mul_overflow(Int(0), Int(1), &r), false);
    QCOMPARE(r, Int(0));
    QCOMPARE(mul_overflow(max, Int(0), &r), false);
    QCOMPARE(r, Int(0));
    QCOMPARE(mul_overflow(Int(0), max, &r), false);
    QCOMPARE(r, Int(0));

    QCOMPARE(mul_overflow(Int(1), Int(1), &r), false);
    QCOMPARE(r, Int(1));
    QCOMPARE(mul_overflow(Int(1), max, &r), false);
    QCOMPARE(r, max);
    QCOMPARE(mul_overflow(max, Int(1), &r), false);
    QCOMPARE(r, max);

    // almost max
    QCOMPARE(mul_overflow(middle, middle, &r), false);
    QCOMPARE(r, Int(max - 2 * middle));
    QCOMPARE(mul_overflow(Int(middle + 1), middle, &r), false);
    QCOMPARE(r, Int(middle << (sizeof(Int) * CHAR_BIT / 2)));
    QCOMPARE(mul_overflow(middle, Int(middle + 1), &r), false);
    QCOMPARE(r, Int(middle << (sizeof(Int) * CHAR_BIT / 2)));
    QCOMPARE(mul_overflow(Int(max / 2), Int(2), &r), false);
    QCOMPARE(r, Int(max & ~Int(1)));
    QCOMPARE(mul_overflow(Int(max / 4), Int(4), &r), false);
    QCOMPARE(r, Int(max & ~Int(3)));

    // overflows
    QCOMPARE(mul_overflow(max, Int(2), &r), true);
    QCOMPARE(mul_overflow(Int(max / 2), Int(3), &r), true);
    QCOMPARE(mul_overflow(Int(middle + 1), Int(middle + 1), &r), true);
#endif
}

template <typename Int, bool enabled = sizeof(Int) <= sizeof(void*)> struct MulOverflowDispatch;
template <typename Int> struct MulOverflowDispatch<Int, true>
{
    void operator()() { mulOverflow_template<Int>(); }
};
template <typename Int> struct MulOverflowDispatch<Int, false>
{
    void operator()() { QSKIP("This type is too big for this architecture", SkipAll); }
};

void tst_QNumeric::mulOverflow()
{
    QFETCH(int, size);
    if (size == 8)
        MulOverflowDispatch<quint8>()();
    if (size == 16)
        MulOverflowDispatch<quint16>()();
    if (size == 32)
        MulOverflowDispatch<quint32>()();
    if (size == 48)
        MulOverflowDispatch<ulong>()();     // not really 48-bit
    if (size == 64)
        MulOverflowDispatch<quint64>()();
}

QTEST_APPLESS_MAIN(tst_QNumeric)
#include "tst_qnumeric.moc"

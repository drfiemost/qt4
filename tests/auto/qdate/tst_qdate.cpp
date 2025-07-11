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
#include <qlocale.h>

//TESTED_CLASS=
//TESTED_FILES=corelib/tools/qdatetime.h corelib/tools/qdatetime.cpp

class tst_QDate : public QObject
{
Q_OBJECT

public:
    tst_QDate();
    virtual ~tst_QDate();

public slots:
    void init();
    void cleanup();
private slots:
    void isNull_data();
    void isNull();
    void isValid_data();
    void isValid();
    void julianDay_data();
    void julianDay();
    void weekNumber_invalid_data();
    void weekNumber_invalid();
    void weekNumber_data();
    void weekNumber();
    void julianDaysLimits();
    void addDays_data();
    void addDays();
    void addMonths_data();
    void addMonths();
    void addYears_data();
    void addYears();
    void operator_eq_eq();
    void operator_not_eq();
    void operator_lt();
    void operator_gt();
    void operator_lt_eq();
    void operator_gt_eq();
    void fromString_data();
    void fromString();
    void fromString_format_data();
    void fromString_format();
    void toString_format_data();
    void toString_format();
    void toStringDateFormat_data();
    void toStringDateFormat();
    void isLeapYear();
    void yearsZeroToNinetyNine();
    void negativeYear() const;
    void printNegativeYear() const;
    void roundtripGermanLocale() const;
    void shortDayName() const;
    void standaloneShortDayName() const;
    void longDayName() const;
    void standaloneLongDayName() const;
    void shortMonthName() const;
    void standaloneShortMonthName() const;
    void longMonthName() const;
    void standaloneLongMonthName() const;
    void roundtrip() const;
};

Q_DECLARE_METATYPE(QDate)
Q_DECLARE_METATYPE(Qt::DateFormat)

tst_QDate::tst_QDate()
{
}

tst_QDate::~tst_QDate()
{

}

void tst_QDate::init()
{
// This will be executed immediately before each test is run.
// TODO: Add initialization code here.
}

void tst_QDate::cleanup()
{
// This will be executed immediately after each test is run.
// TODO: Add cleanup code here.
}

void tst_QDate::isNull_data()
{
    QTest::addColumn<qint64>("jd");
    QTest::addColumn<bool>("null");

    qint64 minJd = Q_INT64_C(-784350574879);
    qint64 maxJd = Q_INT64_C( 784354017364);

    QTest::newRow("qint64 min") << std::numeric_limits<qint64>::min() << true;
    QTest::newRow("minJd - 1")  << minJd - 1                          << true;
    QTest::newRow("minJd")      << minJd                              << false;
    QTest::newRow("minJd + 1")  << minJd + 1                          << false;
    QTest::newRow("maxJd - 1")  << maxJd - 1                          << false;
    QTest::newRow("maxJd")      << maxJd                              << false;
    QTest::newRow("maxJd + 1")  << maxJd + 1                          << true;
    QTest::newRow("qint64 max") << std::numeric_limits<qint64>::max() << true;
}

void tst_QDate::isNull()
{
    QFETCH(qint64, jd);

    QDate d = QDate::fromJulianDay(jd);
    QTEST(d.isNull(), "null");
}

void tst_QDate::isValid_data()
{
    qint64 nullJd = std::numeric_limits<qint64>::min();

    QTest::addColumn<int>("year");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("day");
    QTest::addColumn<qint64>("jd");
    QTest::addColumn<bool>("valid");

    QTest::newRow("0-0-0")    <<    0 <<  0 << 0 << nullJd << false;
    QTest::newRow("month 0")  << 2000 <<  0 << 1 << nullJd << false;
    QTest::newRow("day 0")    << 2000 <<  1 << 0 << nullJd << false;

    QTest::newRow("month 13") << 2000 << 13 << 1 << nullJd << false;

    // test leap years
    QTest::newRow("non-leap")         << 2006 <<  2 << 29 << nullJd  << false;
    QTest::newRow("normal leap")      << 2004 <<  2 << 29 << qint64(2453065) << true;
    QTest::newRow("century leap")     << 1900 <<  2 << 29 << nullJd  << false;
    QTest::newRow("century leap")     << 2100 <<  2 << 29 << nullJd  << false;
    QTest::newRow("400-years leap")   << 2000 <<  2 << 29 << qint64(2451604) << true;
    QTest::newRow("400-years leap 2") << 2400 <<  2 << 29 << qint64(2597701) << true;
    QTest::newRow("400-years leap 3") << 1600 <<  2 << 29 << qint64(2305507) << true;
    QTest::newRow("year 0")           <<    0 <<  2 << 27 << nullJd  << false;

    // test the number of days in months:
    QTest::newRow("jan") << 2000 <<  1 << 31 << qint64(2451575) << true;
    QTest::newRow("feb") << 2000 <<  2 << 29 << qint64(2451604) << true; // same data as 400-years leap
    QTest::newRow("mar") << 2000 <<  3 << 31 << qint64(2451635) << true;
    QTest::newRow("apr") << 2000 <<  4 << 30 << qint64(2451665) << true;
    QTest::newRow("may") << 2000 <<  5 << 31 << qint64(2451696) << true;
    QTest::newRow("jun") << 2000 <<  6 << 30 << qint64(2451726) << true;
    QTest::newRow("jul") << 2000 <<  7 << 31 << qint64(2451757) << true;
    QTest::newRow("aug") << 2000 <<  8 << 31 << qint64(2451788) << true;
    QTest::newRow("sep") << 2000 <<  9 << 30 << qint64(2451818) << true;
    QTest::newRow("oct") << 2000 << 10 << 31 << qint64(2451849) << true;
    QTest::newRow("nov") << 2000 << 11 << 30 << qint64(2451879) << true;
    QTest::newRow("dec") << 2000 << 12 << 31 << qint64(2451910) << true;

    // and invalid dates:
    QTest::newRow("ijan") << 2000 <<  1 << 32 << nullJd << false;
    QTest::newRow("ifeb") << 2000 <<  2 << 30 << nullJd << false;
    QTest::newRow("imar") << 2000 <<  3 << 32 << nullJd << false;
    QTest::newRow("iapr") << 2000 <<  4 << 31 << nullJd << false;
    QTest::newRow("imay") << 2000 <<  5 << 32 << nullJd << false;
    QTest::newRow("ijun") << 2000 <<  6 << 31 << nullJd << false;
    QTest::newRow("ijul") << 2000 <<  7 << 32 << nullJd << false;
    QTest::newRow("iaug") << 2000 <<  8 << 32 << nullJd << false;
    QTest::newRow("isep") << 2000 <<  9 << 31 << nullJd << false;
    QTest::newRow("ioct") << 2000 << 10 << 32 << nullJd << false;
    QTest::newRow("inov") << 2000 << 11 << 31 << nullJd << false;
    QTest::newRow("idec") << 2000 << 12 << 32 << nullJd << false;

    // the beginning of the Julian Day calendar:
    QTest::newRow("jd earliest formula") <<   -4800 <<  1 <<  1 << qint64(   -31738) << true;
    QTest::newRow("jd -1")               <<   -4714 << 11 << 23 << qint64(       -1) << true;
    QTest::newRow("jd 0")                <<   -4714 << 11 << 24 << qint64(        0) << true;
    QTest::newRow("jd 1")                <<   -4714 << 11 << 25 << qint64(        1) << true;
    QTest::newRow("jd latest formula")   << 1400000 << 12 << 31 << qint64(513060925) << true;
}

void tst_QDate::isValid()
{
    QFETCH(int, year);
    QFETCH(int, month);
    QFETCH(int, day);
    QFETCH(qint64, jd);
    QFETCH(bool, valid);

    QTEST(QDate::isValid(year, month, day), "valid");

    QDate d;
    d.setDate(year, month, day);
    QTEST(d.isValid(), "valid");
    QCOMPARE(d.toJulianDay(), jd);
}

void tst_QDate::julianDay_data()
{
    isValid_data();
}

void tst_QDate::julianDay()
{
    QFETCH(int, year);
    QFETCH(int, month);
    QFETCH(int, day);
    QFETCH(qint64, jd);

    {
        QDate d;
        d.setDate(year, month, day);
        QCOMPARE(d.toJulianDay(), jd);
    }

    if (jd != std::numeric_limits<qint64>::min()) {
        QDate d = QDate::fromJulianDay(jd);
        QCOMPARE(d.year(), year);
        QCOMPARE(d.month(), month);
        QCOMPARE(d.day(), day);
    }
}

void tst_QDate::weekNumber_data()
{
    QTest::addColumn<int>("expectedWeekNum");
    QTest::addColumn<int>("expectedYearNum");
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("day");

    //next we fill it with data
    QTest::newRow( "data0" )  << 10 << 2002 << 2002 << 3 << 8;
    QTest::newRow( "data1" )  << 10 << 2002 << 2002 << 3 << 8;
    QTest::newRow( "data2" )  << 52 << 1999 << 2000 << 1 << 1;
    QTest::newRow( "data3" )  << 52 << 1999 << 1999 << 12 << 31;
    QTest::newRow( "data4" )  << 1 << 2001 << 2001 << 1 << 1;
    QTest::newRow( "data5" )  << 53 << 1998 << 1998 << 12 << 31;
    QTest::newRow( "data6" )  << 1 << 1985 << 1984 << 12 << 31;
    QTest::newRow( "data7" )  << 52 << 2006 << 2006 << 12 << 31;
}

void tst_QDate::weekNumber()
{
    int yearNumber;
    QFETCH( int, year );
    QFETCH( int, month );
    QFETCH( int, day );
    QFETCH( int, expectedWeekNum );
    QFETCH( int, expectedYearNum );
    QDate dt1( year, month, day );
    QCOMPARE( dt1.weekNumber( &yearNumber ), expectedWeekNum );
    QCOMPARE( yearNumber, expectedYearNum );
}

void tst_QDate::weekNumber_invalid_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("day");

    //next we fill it with data
    QTest::newRow( "data0" )  << 0 << 0 << 0;
    QTest::newRow( "data1" )  << 2001 << 1 << 32;
    QTest::newRow( "data2" )  << 1999 << 2 << 29;
}

void tst_QDate::weekNumber_invalid()
{
    QDate dt;
    int yearNumber;
    QCOMPARE( dt.weekNumber( &yearNumber ), 0 );
}

void tst_QDate::julianDaysLimits()
{
    qint64 min = std::numeric_limits<qint64>::min();
    qint64 max = std::numeric_limits<qint64>::max();
    qint64 minJd = Q_INT64_C(-784350574879);
    qint64 maxJd = Q_INT64_C( 784354017364);

    QDate maxDate = QDate::fromJulianDay(maxJd);
    QDate minDate = QDate::fromJulianDay(minJd);
    QDate zeroDate = QDate::fromJulianDay(0);

    QDate dt = QDate::fromJulianDay(min);
    QCOMPARE(dt.isValid(), false);
    dt = QDate::fromJulianDay(minJd - 1);
    QCOMPARE(dt.isValid(), false);
    dt = QDate::fromJulianDay(minJd);
    QCOMPARE(dt.isValid(), true);
    dt = QDate::fromJulianDay(minJd + 1);
    QCOMPARE(dt.isValid(), true);
    dt = QDate::fromJulianDay(maxJd - 1);
    QCOMPARE(dt.isValid(), true);
    dt = QDate::fromJulianDay(maxJd);
    QCOMPARE(dt.isValid(), true);
    dt = QDate::fromJulianDay(maxJd + 1);
    QCOMPARE(dt.isValid(), false);
    dt = QDate::fromJulianDay(max);
    QCOMPARE(dt.isValid(), false);

    dt = maxDate.addDays(1);
    QCOMPARE(dt.isValid(), false);
    dt = maxDate.addDays(0);
    QCOMPARE(dt.isValid(), true);
    dt = maxDate.addDays(-1);
    QCOMPARE(dt.isValid(), true);
    dt = maxDate.addDays(max);
    QCOMPARE(dt.isValid(), false);
    dt = maxDate.addDays(min);
    QCOMPARE(dt.isValid(), false);

    dt = minDate.addDays(-1);
    QCOMPARE(dt.isValid(), false);
    dt = minDate.addDays(0);
    QCOMPARE(dt.isValid(), true);
    dt = minDate.addDays(1);
    QCOMPARE(dt.isValid(), true);
    dt = minDate.addDays(min);
    QCOMPARE(dt.isValid(), false);
    dt = minDate.addDays(max);
    QCOMPARE(dt.isValid(), false);

    dt = zeroDate.addDays(-1);
    QCOMPARE(dt.isValid(), true);
    dt = zeroDate.addDays(0);
    QCOMPARE(dt.isValid(), true);
    dt = zeroDate.addDays(1);
    QCOMPARE(dt.isValid(), true);
    dt = zeroDate.addDays(min);
    QCOMPARE(dt.isValid(), false);
    dt = zeroDate.addDays(max);
    QCOMPARE(dt.isValid(), false);
}

void tst_QDate::addDays()
{
    QFETCH( int, year );
    QFETCH( int, month );
    QFETCH( int, day );
    QFETCH( int, amountToAdd );
    QFETCH( int, expectedYear );
    QFETCH( int, expectedMonth );
    QFETCH( int, expectedDay );

    QDate dt( year, month, day );
    dt = dt.addDays( amountToAdd );

    QCOMPARE( dt.year(), expectedYear );
    QCOMPARE( dt.month(), expectedMonth );
    QCOMPARE( dt.day(), expectedDay );
}

void tst_QDate::addDays_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("day");
    QTest::addColumn<int>("amountToAdd");
    QTest::addColumn<int>("expectedYear");
    QTest::addColumn<int>("expectedMonth");
    QTest::addColumn<int>("expectedDay");

    QTest::newRow( "data0" ) << 2000 << 1 << 1 << 1 << 2000 << 1 << 2;
    QTest::newRow( "data1" ) << 2000 << 1 << 31 << 1 << 2000 << 2 << 1;
    QTest::newRow( "data2" ) << 2000 << 2 << 28 << 1 << 2000 << 2 << 29;
    QTest::newRow( "data3" ) << 2000 << 2 << 29 << 1 << 2000 << 3 << 1;
    QTest::newRow( "data4" ) << 2000 << 12 << 31 << 1 << 2001 << 1 << 1;
    QTest::newRow( "data5" ) << 2001 << 2 << 28 << 1 << 2001 << 3 << 1;
    QTest::newRow( "data6" ) << 2001 << 2 << 28 << 30 << 2001 << 3 << 30;
    QTest::newRow( "data7" ) << 2001 << 3 << 30 << 5 << 2001 << 4 << 4;

    QTest::newRow( "data8" ) << 2000 << 1 << 1 << -1 << 1999 << 12 << 31;
    QTest::newRow( "data9" ) << 2000 << 1 << 31 << -1 << 2000 << 1 << 30;
    QTest::newRow( "data10" ) << 2000 << 2 << 28 << -1 << 2000 << 2 << 27;
    QTest::newRow( "data11" ) << 2001 << 2 << 28 << -30 << 2001 << 1 << 29;

    QTest::newRow( "data12" ) << -4713 << 1 << 2 << -2 << -4714 << 12 << 31;
    QTest::newRow( "data13" ) << -4713 << 1 << 2 <<  2 << -4713 <<  1 <<  4;
}

void tst_QDate::addMonths()
{
    QFETCH( int, year );
    QFETCH( int, month );
    QFETCH( int, day );
    QFETCH( int, amountToAdd );
    QFETCH( int, expectedYear );
    QFETCH( int, expectedMonth );
    QFETCH( int, expectedDay );

    QDate dt( year, month, day );
    dt = dt.addMonths( amountToAdd );

    QCOMPARE( dt.year(), expectedYear );
    QCOMPARE( dt.month(), expectedMonth );
    QCOMPARE( dt.day(), expectedDay );
}

void tst_QDate::addMonths_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("day");
    QTest::addColumn<int>("amountToAdd");
    QTest::addColumn<int>("expectedYear");
    QTest::addColumn<int>("expectedMonth");
    QTest::addColumn<int>("expectedDay");

    QTest::newRow( "data0" ) << 2000 << 1 << 1 << 1 << 2000 << 2 << 1;
    QTest::newRow( "data1" ) << 2000 << 1 << 31 << 1 << 2000 << 2 << 29;
    QTest::newRow( "data2" ) << 2000 << 2 << 28 << 1 << 2000 << 3 << 28;
    QTest::newRow( "data3" ) << 2000 << 2 << 29 << 1 << 2000 << 3 << 29;
    QTest::newRow( "data4" ) << 2000 << 12 << 31 << 1 << 2001 << 1 << 31;
    QTest::newRow( "data5" ) << 2001 << 2 << 28 << 1 << 2001 << 3 << 28;
    QTest::newRow( "data6" ) << 2001 << 2 << 28 << 12 << 2002 << 2 << 28;
    QTest::newRow( "data7" ) << 2000 << 2 << 29 << 12 << 2001 << 2 << 28;
    QTest::newRow( "data8" ) << 2000 << 10 << 15 << 4 << 2001 << 2 << 15;

    QTest::newRow( "data9" ) << 2000 << 1 << 1 << -1 << 1999 << 12 << 1;
    QTest::newRow( "data10" ) << 2000 << 1 << 31 << -1 << 1999 << 12 << 31;
    QTest::newRow( "data11" ) << 2000 << 12 << 31 << -1 << 2000 << 11 << 30;
    QTest::newRow( "data12" ) << 2001 << 2 << 28 << -12 << 2000 << 2 << 28;
    QTest::newRow( "data13" ) << 2000 << 1 << 31 << -7 << 1999 << 6 << 30;
    QTest::newRow( "data14" ) << 2000 << 2 << 29 << -12 << 1999 << 2 << 28;

    // year sign change:
    QTest::newRow( "data14" ) << 1 << 1 << 1 << -1 << -1 << 12 << 1;
    QTest::newRow( "data15" ) << 1 << 1 << 1 << -12 << -1 << 1 << 1;
    QTest::newRow( "data16" ) << -1 << 12 << 1 << 1 << 1 << 1 << 1;
    QTest::newRow( "data17" ) << -1 << 1 << 1 << 12 << 1 << 1 << 1;

    // Gregorian/Julian switchover
    QTest::newRow( "data18" ) << 1582 << 9 << 4 << 1 << 1582 << 10 << 4;
    QTest::newRow( "data19" ) << 1582 << 9 << 10 << 1 << 1582 << 10 << 15;
    QTest::newRow( "data20" ) << 1582 << 9 << 20 << 1 << 1582 << 10 << 20;
    QTest::newRow( "data21" ) << 1582 << 11 << 4 << -1 << 1582 << 10 << 4;
    QTest::newRow( "data22" ) << 1582 << 11 << 10 << -1 << 1582 << 10 << 4;
    QTest::newRow( "data23" ) << 1582 << 11 << 20 << -1 << 1582 << 10 << 20;
}

void tst_QDate::addYears()
{
    QFETCH( int, year );
    QFETCH( int, month );
    QFETCH( int, day );
    QFETCH( int, amountToAdd );
    QFETCH( int, expectedYear );
    QFETCH( int, expectedMonth );
    QFETCH( int, expectedDay );

    QDate dt( year, month, day );
    dt = dt.addYears( amountToAdd );

    QCOMPARE( dt.year(), expectedYear );
    QCOMPARE( dt.month(), expectedMonth );
    QCOMPARE( dt.day(), expectedDay );
}

void tst_QDate::addYears_data()
{
    QTest::addColumn<int>("year");
    QTest::addColumn<int>("month");
    QTest::addColumn<int>("day");
    QTest::addColumn<int>("amountToAdd");
    QTest::addColumn<int>("expectedYear");
    QTest::addColumn<int>("expectedMonth");
    QTest::addColumn<int>("expectedDay");

    QTest::newRow( "data0" ) << 2000 << 1 << 1 << 1 << 2001 << 1 << 1;
    QTest::newRow( "data1" ) << 2000 << 1 << 31 << 1 << 2001 << 1 << 31;
    QTest::newRow( "data2" ) << 2000 << 2 << 28 << 1 << 2001 << 2 << 28;
    QTest::newRow( "data3" ) << 2000 << 2 << 29 << 1 << 2001 << 2 << 28;
    QTest::newRow( "data4" ) << 2000 << 12 << 31 << 1 << 2001 << 12 << 31;
    QTest::newRow( "data5" ) << 2001 << 2 << 28 << 3 << 2004 << 2 << 28;
    QTest::newRow( "data6" ) << 2000 << 2 << 29 << 4 << 2004 << 2 << 29;

    QTest::newRow( "data7" ) << 2000 << 1 << 31 << -1 << 1999 << 1 << 31;
    QTest::newRow( "data9" ) << 2000 << 2 << 29 << -1 << 1999 << 2 << 28;
    QTest::newRow( "data10" ) << 2000 << 12 << 31 << -1 << 1999 << 12 << 31;
    QTest::newRow( "data11" ) << 2001 << 2 << 28 << -3 << 1998 << 2 << 28;
    QTest::newRow( "data12" ) << 2000 << 2 << 29 << -4 << 1996 << 2 << 29;
    QTest::newRow( "data13" ) << 2000 << 2 << 29 << -5 << 1995 << 2 << 28;

    QTest::newRow( "data14" ) << 2000 << 1 << 1 << -1999 << 1 << 1 << 1;
    QTest::newRow( "data15" ) << 2000 << 1 << 1 << -2000 << -1 << 1 << 1;
    QTest::newRow( "data16" ) << 2000 << 1 << 1 << -2001 << -2 << 1 << 1;
    QTest::newRow( "data17" ) << -2000 << 1 << 1 << 1999 << -1 << 1 << 1;
    QTest::newRow( "data18" ) << -2000 << 1 << 1 << 2000 << 1 << 1 << 1;
    QTest::newRow( "data19" ) << -2000 << 1 << 1 << 2001 << 2 << 1 << 1;
}

void tst_QDate::operator_eq_eq()
{
    QDate d1(2000,1,2);
    QDate d2(2000,1,2);
    QVERIFY( d1 == d2 );

    d1 = QDate(2001,12,5);
    d2 = QDate(2001,12,5);
    QVERIFY( d1 == d2 );

    d2 = QDate(2002,12,5);
    QVERIFY( !(d1 == d2) );
}

void tst_QDate::operator_not_eq()
{
    QDate d1(2000,1,2);
    QDate d2(2000,1,2);
    QVERIFY( !(d1 != d2) );

    d1 = QDate(2001,12,5);
    d2 = QDate(2001,12,5);
    QVERIFY( !(d1 != d2) );

    d2 = QDate(2002,12,5);
    QVERIFY( d1 != d2 );
}

void tst_QDate::operator_lt()
{
    QDate d1(2000,1,2);
    QDate d2(2000,1,2);
    QVERIFY( !(d1 < d2) );

    d1 = QDate(2001,12,4);
    d2 = QDate(2001,12,5);
    QVERIFY( d1 < d2 );

    d1 = QDate(2001,11,5);
    d2 = QDate(2001,12,5);
    QVERIFY( d1 < d2 );

    d1 = QDate(2000,12,5);
    d2 = QDate(2001,12,5);
    QVERIFY( d1 < d2 );

    d1 = QDate(2002,12,5);
    d2 = QDate(2001,12,5);
    QVERIFY( !(d1 < d2) );

    d1 = QDate(2001,12,5);
    d2 = QDate(2001,11,5);
    QVERIFY( !(d1 < d2) );

    d1 = QDate(2001,12,6);
    d2 = QDate(2001,12,5);
    QVERIFY( !(d1 < d2) );
}

void tst_QDate::operator_gt()
{
    QDate d1(2000,1,2);
    QDate d2(2000,1,2);
    QVERIFY( !(d1 > d2) );

    d1 = QDate(2001,12,4);
    d2 = QDate(2001,12,5);
    QVERIFY( !(d1 > d2) );

    d1 = QDate(2001,11,5);
    d2 = QDate(2001,12,5);
    QVERIFY( !(d1 > d2) );

    d1 = QDate(2000,12,5);
    d2 = QDate(2001,12,5);
    QVERIFY( !(d1 > d2) );

    d1 = QDate(2002,12,5);
    d2 = QDate(2001,12,5);
    QVERIFY( d1 > d2 );

    d1 = QDate(2001,12,5);
    d2 = QDate(2001,11,5);
    QVERIFY( d1 > d2 );

    d1 = QDate(2001,12,6);
    d2 = QDate(2001,12,5);
    QVERIFY( d1 > d2 );
}

void tst_QDate::operator_lt_eq()
{
    QDate d1(2000,1,2);
    QDate d2(2000,1,2);
    QVERIFY( d1 <= d2 );

    d1 = QDate(2001,12,4);
    d2 = QDate(2001,12,5);
    QVERIFY( d1 <= d2 );

    d1 = QDate(2001,11,5);
    d2 = QDate(2001,12,5);
    QVERIFY( d1 <= d2 );

    d1 = QDate(2000,12,5);
    d2 = QDate(2001,12,5);
    QVERIFY( d1 <= d2 );

    d1 = QDate(2002,12,5);
    d2 = QDate(2001,12,5);
    QVERIFY( !(d1 <= d2) );

    d1 = QDate(2001,12,5);
    d2 = QDate(2001,11,5);
    QVERIFY( !(d1 <= d2) );

    d1 = QDate(2001,12,6);
    d2 = QDate(2001,12,5);
    QVERIFY( !(d1 <= d2) );
}

void tst_QDate::operator_gt_eq()
{
    QDate d1(2000,1,2);
    QDate d2(2000,1,2);
    QVERIFY( d1 >= d2 );

    d1 = QDate(2001,12,4);
    d2 = QDate(2001,12,5);
    QVERIFY( !(d1 >= d2) );

    d1 = QDate(2001,11,5);
    d2 = QDate(2001,12,5);
    QVERIFY( !(d1 >= d2) );

    d1 = QDate(2000,12,5);
    d2 = QDate(2001,12,5);
    QVERIFY( !(d1 >= d2) );

    d1 = QDate(2002,12,5);
    d2 = QDate(2001,12,5);
    QVERIFY( d1 >= d2 );

    d1 = QDate(2001,12,5);
    d2 = QDate(2001,11,5);
    QVERIFY( d1 >= d2 );

    d1 = QDate(2001,12,6);
    d2 = QDate(2001,12,5);
    QVERIFY( d1 >= d2 );
}

void tst_QDate::fromString_data()
{
    // Since we can't define an element of Qt::DateFormat, d1 will be the date
    // expected when we have a TextDate, and d2 will be the date expected when
    // we have an ISODate.

    QTest::addColumn<QString>("str1");
    QTest::addColumn<QString>("str2");
    QTest::addColumn<QDate>("d1");

    QTest::newRow( "data0" ) << QString("Sat May 20 1995") << QString("1995-05-20") << QDate(1995,5,20);
    QTest::newRow( "data1" ) << QString("Tue Dec 17 2002") << QString("2002-12-17") << QDate(2002,12,17);
    QDate d( 1999, 11, 14 );
    QTest::newRow( "data2" ) << d.toString( Qt::TextDate ) << d.toString( Qt::ISODate ) << d;

    QTest::newRow( "data3" ) << QString("xxx Jan 1 0999") << QString("0999-01-01") << QDate(999, 1, 1);
    QTest::newRow( "data3b" ) << QString("xxx Jan 1 999") << QString("0999-01-01") << QDate(999, 1, 1);
    QTest::newRow( "data4" ) << QString("xxx Jan 1 12345") << QString() << QDate(12345, 1, 1);
    QTest::newRow( "data5" ) << QString("xxx Jan 1 -0001") << QString() << QDate(-1, 1, 1);
    QTest::newRow( "data6" ) << QString("xxx Jan 1 -4712") << QString() << QDate(-4712, 1, 1);
    QTest::newRow( "data7" ) << QString("xxx Nov 25 -4713") << QString() << QDate(-4713, 11, 25);
}

void tst_QDate::fromString()
{
    QFETCH( QString, str1 );
    QFETCH( QString, str2 );
    QFETCH( QDate, d1 );

    QCOMPARE( QDate::fromString( str1, Qt::TextDate ), d1 );
    if (!str2.isEmpty())
        QCOMPARE( QDate::fromString( str2, Qt::ISODate ), d1 );
}

void tst_QDate::fromString_format_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("format");
    QTest::addColumn<QDate>("date");

    //year with yy is always 19xx for compatibility
    QTest::newRow( "data0" ) << QString("21052006") << QString("ddMMyyyy") << QDate(2006,5,21);
    QTest::newRow( "data1" ) << QString("210506") << QString("ddMMyy") << QDate(1906,5,21);
    QTest::newRow( "data2" ) << QString("21/5/2006") << QString("d/M/yyyy") << QDate(2006,5,21);
    QTest::newRow( "data3" ) << QString("21/5/06") << QString("d/M/yy") << QDate(1906,5,21);
    QTest::newRow( "data4" ) << QString("20060521") << QString("yyyyMMdd") << QDate(2006,5,21);
    QTest::newRow( "data5" ) << QString("060521") << QString("yyMMdd") << QDate(1906,5,21);
}

void tst_QDate::fromString_format()
{
    QFETCH( QString, string );
    QFETCH( QString, format );
    QFETCH( QDate, date );

    QCOMPARE( QDate::fromString( string, format ), date );
}

void tst_QDate::toString_format_data()
{
    QTest::addColumn<QDate>("t");
    QTest::addColumn<QString>("format");
    QTest::addColumn<QString>("str");

    QTest::newRow( "data0" ) << QDate(1995,5,20) << QString("d-M-yy") << QString("20-5-95");
    QTest::newRow( "data1" ) << QDate(2002,12,17) << QString("dd-MM-yyyy") << QString("17-12-2002");
    QTest::newRow( "data2" ) << QDate(1995,5,20) << QString("M-yy") << QString("5-95");
    QTest::newRow( "data3" ) << QDate(2002,12,17) << QString("dd") << QString("17");
    QTest::newRow( "data4" ) << QDate() << QString("dd-mm-yyyy") << QString();
}

void tst_QDate::toString_format()
{
    QFETCH( QDate, t );
    QFETCH( QString, format );
    QFETCH( QString, str );

    QCOMPARE( t.toString( format ), str );
}

void tst_QDate::toStringDateFormat_data()
{
    QTest::addColumn<QDate>("date");
    QTest::addColumn<Qt::DateFormat>("format");
    QTest::addColumn<QString>("expectedStr");

    QTest::newRow("data0") << QDate(1,1,1) << Qt::ISODate << QString("0001-01-01");
    QTest::newRow("data1") << QDate(11,1,1) << Qt::ISODate << QString("0011-01-01");
    QTest::newRow("data2") << QDate(111,1,1) << Qt::ISODate << QString("0111-01-01");
    QTest::newRow("data3") << QDate(1974,12,1) << Qt::ISODate << QString("1974-12-01");
}

void tst_QDate::toStringDateFormat()
{
    QFETCH(QDate, date);
    QFETCH(Qt::DateFormat, format);
    QFETCH(QString, expectedStr);

    QCOMPARE(date.toString(Qt::SystemLocaleShortDate), QLocale::system().toString(date, QLocale::ShortFormat));
    QCOMPARE(date.toString(Qt::LocaleDate), QLocale().toString(date, QLocale::ShortFormat));
    QLocale::setDefault(QLocale::German);
    QCOMPARE(date.toString(Qt::SystemLocaleShortDate), QLocale::system().toString(date, QLocale::ShortFormat));
    QCOMPARE(date.toString(Qt::LocaleDate), QLocale().toString(date, QLocale::ShortFormat));

    QCOMPARE(date.toString(format), expectedStr);
}

void tst_QDate::isLeapYear()
{
    QVERIFY(QDate::isLeapYear(-4801));
    QVERIFY(!QDate::isLeapYear(-4800));
    QVERIFY(QDate::isLeapYear(-4445));
    QVERIFY(!QDate::isLeapYear(-4444));
    QVERIFY(!QDate::isLeapYear(-6));
    QVERIFY(QDate::isLeapYear(-5));
    QVERIFY(!QDate::isLeapYear(-4));
    QVERIFY(!QDate::isLeapYear(-3));
    QVERIFY(!QDate::isLeapYear(-2));
    QVERIFY(QDate::isLeapYear(-1));
    QVERIFY(!QDate::isLeapYear(0)); // Doesn't exist
    QVERIFY(!QDate::isLeapYear(1));
    QVERIFY(!QDate::isLeapYear(2));
    QVERIFY(!QDate::isLeapYear(3));
    QVERIFY(QDate::isLeapYear(4));
    QVERIFY(!QDate::isLeapYear(7));
    QVERIFY(QDate::isLeapYear(8));
    QVERIFY(QDate::isLeapYear(100));
    QVERIFY(QDate::isLeapYear(400));
    QVERIFY(QDate::isLeapYear(700));
    QVERIFY(QDate::isLeapYear(1500));
    QVERIFY(QDate::isLeapYear(1600));
    QVERIFY(!QDate::isLeapYear(1700));
    QVERIFY(!QDate::isLeapYear(1800));
    QVERIFY(!QDate::isLeapYear(1900));
    QVERIFY(QDate::isLeapYear(2000));
    QVERIFY(!QDate::isLeapYear(2100));
    QVERIFY(!QDate::isLeapYear(2200));
    QVERIFY(!QDate::isLeapYear(2300));
    QVERIFY(QDate::isLeapYear(2400));
    QVERIFY(!QDate::isLeapYear(2500));
    QVERIFY(!QDate::isLeapYear(2600));
    QVERIFY(!QDate::isLeapYear(2700));
    QVERIFY(QDate::isLeapYear(2800));

    for (int i = -4713; i <= 10000; ++i) {
        if (i == 0)
            continue;
        QVERIFY(!QDate(i, 2, 29).isValid() == !QDate::isLeapYear(i));
    }
}

void tst_QDate::yearsZeroToNinetyNine()
{
    {
        QDate dt(-1, 2, 3);
        QCOMPARE(dt.year(), -1);
        QCOMPARE(dt.month(), 2);
        QCOMPARE(dt.day(), 3);
    }

    {
        QDate dt(1, 2, 3);
        QCOMPARE(dt.year(), 1);
        QCOMPARE(dt.month(), 2);
        QCOMPARE(dt.day(), 3);
    }

    {
        QDate dt(99, 2, 3);
        QCOMPARE(dt.year(), 99);
        QCOMPARE(dt.month(), 2);
        QCOMPARE(dt.day(), 3);
    }

    QVERIFY(!QDate::isValid(0, 2, 3));
    QVERIFY(QDate::isValid(1, 2, 3));
    QVERIFY(QDate::isValid(-1, 2, 3));

    {
        QDate dt;
        dt.setDate(1, 2, 3);
        QCOMPARE(dt.year(), 1);
        QCOMPARE(dt.month(), 2);
        QCOMPARE(dt.day(), 3);

        dt.setDate(0, 2, 3);
        QVERIFY(!dt.isValid());
    }
}

void tst_QDate::negativeYear() const
{
    QDate y(-20, 3, 4);
    QVERIFY(y.isValid());
    QCOMPARE(y.year(), -20);
}

void tst_QDate::printNegativeYear() const
{
    {
        QDate date(-500, 3, 4);
        QVERIFY(date.isValid());
        QCOMPARE(date.toString(QLatin1String("yyyy")), QString::fromLatin1("-0500"));
    }

    {
        QDate date(-10, 3, 4);
        QVERIFY(date.isValid());
        QCOMPARE(date.toString(QLatin1String("yyyy")), QString::fromLatin1("-0010"));
    }

    {
        QDate date(-2, 3, 4);
        QVERIFY(date.isValid());
        QCOMPARE(date.toString(QLatin1String("yyyy")), QString::fromLatin1("-0002"));
    }
}

void tst_QDate::roundtripGermanLocale() const
{
    /* This code path should not result in warnings. */
    const QDate theDate(QDate::currentDate());
    theDate.fromString(theDate.toString(Qt::TextDate), Qt::TextDate);

    const QDateTime theDateTime(QDateTime::currentDateTime());
    theDateTime.fromString(theDateTime.toString(Qt::TextDate), Qt::TextDate);
}

void tst_QDate::shortDayName() const
{
    if (QLocale::system().language() == QLocale::C) {
        QCOMPARE(QDate::shortDayName(1), QLatin1String("Mon"));
        QCOMPARE(QDate::shortDayName(7), QLatin1String("Sun"));
    }

    QLocale locale = QLocale::system();
    for(int i = 1; i <= 7; ++i) {
        QCOMPARE(QDate::shortDayName(i), locale.dayName(i, QLocale::ShortFormat));
    }
}

void tst_QDate::standaloneShortDayName() const
{
    if (QLocale::system().language() == QLocale::C) {
        QCOMPARE(QDate::shortDayName(1, QDate::StandaloneFormat), QLatin1String("Mon"));
        QCOMPARE(QDate::shortDayName(7, QDate::StandaloneFormat), QLatin1String("Sun"));
    }

    QLocale locale = QLocale::system();
    for(int i = 1; i <= 7; ++i) {
        QCOMPARE(QDate::shortDayName(i, QDate::StandaloneFormat), locale.standaloneDayName(i, QLocale::ShortFormat));
    }
}

void tst_QDate::longDayName() const
{
    if (QLocale::system().language() == QLocale::C) {
        QCOMPARE(QDate::longDayName(1), QLatin1String("Monday"));
        QCOMPARE(QDate::longDayName(7), QLatin1String("Sunday"));
    }

    QLocale locale = QLocale::system();
    for(int i = 1; i <= 7; ++i) {
        QCOMPARE(QDate::longDayName(i), locale.dayName(i, QLocale::LongFormat));
    }
}

void tst_QDate::standaloneLongDayName() const
{
    if (QLocale::system().language() == QLocale::C) {
        QCOMPARE(QDate::longDayName(1, QDate::StandaloneFormat), QLatin1String("Monday"));
        QCOMPARE(QDate::longDayName(7, QDate::StandaloneFormat), QLatin1String("Sunday"));
    }

    QLocale locale = QLocale::system();
    for(int i = 1; i <= 7; ++i) {
        QCOMPARE(QDate::longDayName(i, QDate::StandaloneFormat), locale.standaloneDayName(i, QLocale::LongFormat));
    }
}

void tst_QDate::shortMonthName() const
{
    if (QLocale::system().language() == QLocale::C) {
        QCOMPARE(QDate::shortMonthName(1), QLatin1String("Jan"));
        QCOMPARE(QDate::shortMonthName(8), QLatin1String("Aug"));
    }

    QLocale locale = QLocale::system();
    for(int i = 1; i <= 12; ++i) {
        QCOMPARE(QDate::shortMonthName(i), locale.monthName(i, QLocale::ShortFormat));
    }
}

void tst_QDate::standaloneShortMonthName() const
{
    if (QLocale::system().language() == QLocale::C) {
        QCOMPARE(QDate::shortMonthName(1, QDate::StandaloneFormat), QLatin1String("Jan"));
        QCOMPARE(QDate::shortMonthName(8, QDate::StandaloneFormat), QLatin1String("Aug"));
    }

    QLocale locale = QLocale::system();
    for(int i = 1; i <= 12; ++i) {
        QCOMPARE(QDate::shortMonthName(i, QDate::StandaloneFormat), locale.standaloneMonthName(i, QLocale::ShortFormat));
    }
}

void tst_QDate::longMonthName() const
{
    if (QLocale::system().language() == QLocale::C) {
        QCOMPARE(QDate::longMonthName(1), QLatin1String("January"));
        QCOMPARE(QDate::longMonthName(8), QLatin1String("August"));
    }

    QLocale locale = QLocale::system();
    for(int i = 1; i <= 12; ++i) {
        QCOMPARE(QDate::longMonthName(i), locale.monthName(i, QLocale::LongFormat));
    }
}

void tst_QDate::standaloneLongMonthName() const
{
    if (QLocale::system().language() == QLocale::C) {
        QCOMPARE(QDate::longMonthName(1, QDate::StandaloneFormat), QLatin1String("January"));
        QCOMPARE(QDate::longMonthName(8, QDate::StandaloneFormat), QLatin1String("August"));
    }

    QLocale locale = QLocale::system();
    for(int i = 1; i <= 12; ++i) {
        QCOMPARE(QDate::longMonthName(i, QDate::StandaloneFormat), locale.standaloneMonthName(i, QLocale::LongFormat));
    }
}

void tst_QDate::roundtrip() const
{
    // Test round trip, this exercises setDate(), isValid(), isLeapYear(),
    // year(), month(), day(), julianDayFromDate(), and getDateFromJulianDay()
    // to ensure they are internally consistent (but doesn't guarantee correct)

    // Test Julian round trip around JD 0 and the c++ integer division rounding
    // problem point (eg. negative numbers) in the conversion functions.
    QDate testDate;
    QDate loopDate = QDate::fromJulianDay(-50001); // 1 Jan 4850 BC
    while (loopDate.toJulianDay() <= 5150) {     // 31 Dec 4700 AD
        testDate.setDate(loopDate.year(), loopDate.month(), loopDate.day());
        QCOMPARE(loopDate.toJulianDay(), testDate.toJulianDay());
        loopDate = loopDate.addDays(1);
    }

    // Test Julian round trip in both BC and AD
    loopDate = QDate::fromJulianDay(1684901);       //  1 Jan 100 BC
    while (loopDate.toJulianDay() <= 1757949) {   // 31 Dec 100 AD
        testDate.setDate(loopDate.year(), loopDate.month(), loopDate.day());
        QCOMPARE(loopDate.toJulianDay(), testDate.toJulianDay());
        loopDate = loopDate.addDays(1);
    }

    // Test Julian and Gregorian round trip during changeover period
    loopDate = QDate::fromJulianDay(2378497);     //  1 Jan 1580 AD
    while (loopDate.toJulianDay() <= 2488433) { // 31 Dec 1585 AD
        testDate.setDate(loopDate.year(), loopDate.month(), loopDate.day());
        QCOMPARE(loopDate.toJulianDay(), testDate.toJulianDay());
        loopDate = loopDate.addDays(1);
    }

    // Test Gregorian round trip during current useful period
    loopDate = QDate::fromJulianDay(2378497);     //  1 Jan 1900 AD
    while (loopDate.toJulianDay() <= 2488433) { // 31 Dec 2100 AD
        testDate.setDate(loopDate.year(), loopDate.month(), loopDate.day());
        QCOMPARE(loopDate.toJulianDay(), testDate.toJulianDay());
        loopDate = loopDate.addDays(1);
    }

    // Test Gregorian round trip at top end of widget/format range
    loopDate = QDate::fromJulianDay(5336961);     //  1 Jan 9900 AD
    while (loopDate.toJulianDay() <= 5373484) { // 31 Dec 9999 AD
        testDate.setDate(loopDate.year(), loopDate.month(), loopDate.day());
        QCOMPARE(loopDate.toJulianDay(), testDate.toJulianDay());
        loopDate = loopDate.addDays(1);
    }

    qint64 minJd = Q_INT64_C(-784350574879);
    qint64 maxJd = Q_INT64_C( 784354017364);

    // Test Gregorian round trip at top end of conversion range
    loopDate = QDate::fromJulianDay(maxJd);
    while (loopDate.toJulianDay() >= maxJd - 146397) {
        testDate.setDate(loopDate.year(), loopDate.month(), loopDate.day());
        QCOMPARE(loopDate.toJulianDay(), testDate.toJulianDay());
        loopDate = loopDate.addDays(-1);
    }

    // Test Gregorian round trip at low end of conversion range
    loopDate = QDate::fromJulianDay(minJd);
    while (loopDate.toJulianDay() <= minJd + 146397) {
        testDate.setDate(loopDate.year(), loopDate.month(), loopDate.day());
        QCOMPARE(loopDate.toJulianDay(), testDate.toJulianDay());
        loopDate = loopDate.addDays(1);
    }
}

QTEST_APPLESS_MAIN(tst_QDate)
#include "tst_qdate.moc"

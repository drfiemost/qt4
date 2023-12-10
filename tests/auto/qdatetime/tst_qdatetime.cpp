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
#ifndef Q_OS_WINCE
#include <time.h>
#endif

#include <qdatetime.h>
#include <private/qdatetime_p.h>

#ifdef Q_OS_WIN
# include <windows.h>
#endif

//TESTED_CLASS=
//TESTED_FILES=

class tst_QDateTime : public QObject
{
    Q_OBJECT

public:
    tst_QDateTime();
    virtual ~tst_QDateTime();

    static QString str( int y, int month, int d, int h, int min, int s );
    static QDateTime dt( const QString& str );
public slots:
    void init();
    void cleanup();
private slots:
    void ctor();
    void operator_eq();
    void isNull();
    void isValid();
    void date();
    void time();
    void timeSpec();
    void toTime_t_data();
    void toTime_t();
    void task_137698();
    void setDate();
    void setTime();
    void setTimeSpec();
    void setTime_t();
    void setMSecsSinceEpoch_data();
    void setMSecsSinceEpoch();
    void toString_isoDate_data();
    void toString_isoDate();
    void toString_textDate_data();
    void toString_textDate();
    void toString_enumformat();
    void toString_strformat();
    void addDays();
    void addMonths();
    void addMonths_data();
    void addYears();
    void addYears_data();
    void addSecs_data();
    void addSecs();
    void addMSecs_data();
    void addMSecs();
    void toTimeSpec_data();
    void toTimeSpec();
    void toLocalTime_data();
    void toLocalTime();
    void toUTC_data();
    void toUTC();
    void daysTo();
    void secsTo_data();
    void secsTo();
    void msecsTo_data();
    void msecsTo();
    void operator_eqeq();
    void currentDateTime();
    void currentDateTimeUtc();
    void currentDateTimeUtc2();
    void fromStringDateFormat_data();
    void fromStringDateFormat();
    void fromStringStringFormat_data();
    void fromStringStringFormat();
    void fromString_LOCALE_ILDATE();
    void fromStringToStringLocale_data();
    void fromStringToStringLocale();

    void offsetFromUtc();
    void setOffsetFromUtc();
    void toOffsetFromUtc();

    void timeZoneAbbreviation();

    void getDate();

    void fewDigitsInYear() const;
    void printNegativeYear() const;
    void roundtripGermanLocale() const;
    void utcOffsetLessThan() const;

private:
    bool europeanTimeZone;
    QDate defDate() const { return QDate(1900, 1, 1); }
    QTime defTime() const { return QTime(0, 0, 0); }
    QDateTime defDateTime() const { return QDateTime(defDate(), defTime()); }
    QDateTime invalidDateTime() const { return QDateTime(invalidDate(), invalidTime()); }
    QDate invalidDate() const { return QDate(); }
    QTime invalidTime() const { return QTime(-1, -1, -1); }
};

Q_DECLARE_METATYPE(QDateTime)
Q_DECLARE_METATYPE(QDate)
Q_DECLARE_METATYPE(QTime)
Q_DECLARE_METATYPE(Qt::TimeSpec)
Q_DECLARE_METATYPE(Qt::DateFormat)


tst_QDateTime::tst_QDateTime()
{
    uint x1 = QDateTime(QDate(1990, 1, 1), QTime()).toTime_t();
    uint x2 = QDateTime(QDate(1990, 6, 1), QTime()).toTime_t();
    europeanTimeZone = (x1 == 631148400 && x2 == 644191200);
}

tst_QDateTime::~tst_QDateTime()
{

}

void tst_QDateTime::init()
{
#if defined(Q_OS_WINCE)
	SetUserDefaultLCID(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT));
#elif defined(Q_OS_WIN)
    SetThreadLocale(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT));
#endif

}

void tst_QDateTime::cleanup()
{
}

QString tst_QDateTime::str( int y, int month, int d, int h, int min, int s )
{
    return QDateTime( QDate(y, month, d), QTime(h, min, s) ).toString( Qt::ISODate );
}

QDateTime tst_QDateTime::dt( const QString& str )
{
    if ( str == "INVALID" ) {
        return QDateTime();
    } else {
        return QDateTime::fromString( str, Qt::ISODate );
    }
}

void tst_QDateTime::ctor()
{
    QDateTime dt1(QDate(2004, 1, 2), QTime(1, 2, 3));
    QCOMPARE(dt1.timeSpec(), Qt::LocalTime);
    QDateTime dt2(QDate(2004, 1, 2), QTime(1, 2, 3), Qt::LocalTime);
    QCOMPARE(dt2.timeSpec(), Qt::LocalTime);
    QDateTime dt3(QDate(2004, 1, 2), QTime(1, 2, 3), Qt::UTC);
    QCOMPARE(dt3.timeSpec(), Qt::UTC);


    QVERIFY(dt1 == dt2);
    if (europeanTimeZone) {
        QVERIFY(dt1 != dt3);
        QVERIFY(dt1 < dt3);
        QVERIFY(dt1.addSecs(3600).toUTC() == dt3);
    }

    // Test OffsetFromUTC constructors
    QDate offsetDate(2013, 1, 1);
    QTime offsetTime(1, 2, 3);

    QDateTime offset1(offsetDate, offsetTime, Qt::OffsetFromUTC);
    QCOMPARE(offset1.timeSpec(), Qt::UTC);
    QCOMPARE(offset1.offsetFromUtc(), 0);
    QCOMPARE(offset1.date(), offsetDate);
    QCOMPARE(offset1.time(), offsetTime);

    QDateTime offset2(offsetDate, offsetTime, Qt::OffsetFromUTC, 0);
    QCOMPARE(offset2.timeSpec(), Qt::UTC);
    QCOMPARE(offset2.offsetFromUtc(), 0);
    QCOMPARE(offset2.date(), offsetDate);
    QCOMPARE(offset2.time(), offsetTime);

    QDateTime offset3(offsetDate, offsetTime, Qt::OffsetFromUTC, 60 * 60);
    QCOMPARE(offset3.timeSpec(), Qt::OffsetFromUTC);
    QCOMPARE(offset3.offsetFromUtc(), 60 * 60);
    QCOMPARE(offset3.date(), offsetDate);
    QCOMPARE(offset3.time(), offsetTime);

    QDateTime offset4(offsetDate, QTime(), Qt::OffsetFromUTC, 60 * 60);
    QCOMPARE(offset4.timeSpec(), Qt::OffsetFromUTC);
    QCOMPARE(offset4.offsetFromUtc(), 60 * 60);
    QCOMPARE(offset4.date(), offsetDate);
    QCOMPARE(offset4.time(), QTime(0, 0, 0));
}

void tst_QDateTime::operator_eq()
{
    QDateTime dt1(QDate(2004, 3, 24), QTime(23, 45, 57), Qt::UTC);
    QDateTime dt2(QDate(2005, 3, 11), QTime(), Qt::UTC);
    dt2 = dt1;
    QVERIFY(dt1 == dt2);
}

void tst_QDateTime::isNull()
{
    QDateTime dt1;
    QVERIFY(dt1.isNull());
    dt1.setDate(QDate());
    QVERIFY(dt1.isNull());
    dt1.setTime(QTime());
    QVERIFY(dt1.isNull());
    dt1.setTimeSpec(Qt::UTC);
    QVERIFY(dt1.isNull());   // maybe it should return false?

    dt1.setDate(QDate(2004, 1, 2));
    QVERIFY(!dt1.isNull());
    dt1.setTime(QTime(12, 34, 56));
    QVERIFY(!dt1.isNull());
    dt1.setTime(QTime());
    QVERIFY(!dt1.isNull());
}

void tst_QDateTime::isValid()
{
    QDateTime dt1;
    QVERIFY(!dt1.isValid());
    dt1.setDate(QDate());
    QVERIFY(!dt1.isValid());
    dt1.setTime(QTime());
    QVERIFY(!dt1.isValid());
    dt1.setTimeSpec(Qt::UTC);
    QVERIFY(!dt1.isValid());

    dt1.setDate(QDate(2004, 1, 2));
    QVERIFY(dt1.isValid());
    dt1.setDate(QDate());
    QVERIFY(!dt1.isValid());
    dt1.setTime(QTime(12, 34, 56));
    QVERIFY(!dt1.isValid());
    dt1.setTime(QTime());
    QVERIFY(!dt1.isValid());
}

void tst_QDateTime::date()
{
    QDateTime dt1(QDate(2004, 3, 24), QTime(23, 45, 57), Qt::LocalTime);
    QCOMPARE(dt1.date(), QDate(2004, 3, 24));

    QDateTime dt2(QDate(2004, 3, 25), QTime(0, 45, 57), Qt::LocalTime);
    QCOMPARE(dt2.date(), QDate(2004, 3, 25));

    QDateTime dt3(QDate(2004, 3, 24), QTime(23, 45, 57), Qt::UTC);
    QCOMPARE(dt3.date(), QDate(2004, 3, 24));

    QDateTime dt4(QDate(2004, 3, 25), QTime(0, 45, 57), Qt::UTC);
    QCOMPARE(dt4.date(), QDate(2004, 3, 25));
}

void tst_QDateTime::time()
{
    QDateTime dt1(QDate(2004, 3, 24), QTime(23, 45, 57), Qt::LocalTime);
    QCOMPARE(dt1.time(), QTime(23, 45, 57));

    QDateTime dt2(QDate(2004, 3, 25), QTime(0, 45, 57), Qt::LocalTime);
    QCOMPARE(dt2.time(), QTime(0, 45, 57));

    QDateTime dt3(QDate(2004, 3, 24), QTime(23, 45, 57), Qt::UTC);
    QCOMPARE(dt3.time(), QTime(23, 45, 57));

    QDateTime dt4(QDate(2004, 3, 25), QTime(0, 45, 57), Qt::UTC);
    QCOMPARE(dt4.time(), QTime(0, 45, 57));
}

void tst_QDateTime::timeSpec()
{
    QDateTime dt1(QDate(2004, 1, 24), QTime(23, 45, 57));
    QCOMPARE(dt1.timeSpec(), Qt::LocalTime);
    QCOMPARE(dt1.addDays(0).timeSpec(), Qt::LocalTime);
    QCOMPARE(dt1.addMonths(0).timeSpec(), Qt::LocalTime);
    QCOMPARE(dt1.addMonths(6).timeSpec(), Qt::LocalTime);
    QCOMPARE(dt1.addYears(0).timeSpec(), Qt::LocalTime);
    QCOMPARE(dt1.addSecs(0).timeSpec(), Qt::LocalTime);
    QCOMPARE(dt1.addSecs(86400 * 185).timeSpec(), Qt::LocalTime);
    QCOMPARE(dt1.toTimeSpec(Qt::LocalTime).timeSpec(), Qt::LocalTime);
    QCOMPARE(dt1.toTimeSpec(Qt::UTC).timeSpec(), Qt::UTC);

    QDateTime dt2(QDate(2004, 1, 24), QTime(23, 45, 57), Qt::LocalTime);
    QCOMPARE(dt2.timeSpec(), Qt::LocalTime);

    QDateTime dt3(QDate(2004, 1, 25), QTime(0, 45, 57), Qt::UTC);
    QCOMPARE(dt3.timeSpec(), Qt::UTC);

    QDateTime dt4 = QDateTime::currentDateTime();
    QCOMPARE(dt4.timeSpec(), Qt::LocalTime);
}

void tst_QDateTime::setDate()
{
    QDateTime dt1(QDate(2004, 3, 25), QTime(0, 45, 57), Qt::UTC);
    dt1.setDate(QDate(2004, 6, 25));
    QCOMPARE(dt1.date(), QDate(2004, 6, 25));
    QCOMPARE(dt1.time(), QTime(0, 45, 57));
    QCOMPARE(dt1.timeSpec(), Qt::UTC);

    QDateTime dt2(QDate(2004, 3, 25), QTime(0, 45, 57), Qt::LocalTime);
    dt2.setDate(QDate(2004, 6, 25));
    QCOMPARE(dt2.date(), QDate(2004, 6, 25));
    QCOMPARE(dt2.time(), QTime(0, 45, 57));
    QCOMPARE(dt2.timeSpec(), Qt::LocalTime);

    QDateTime dt3(QDate(4004, 3, 25), QTime(0, 45, 57), Qt::UTC);
    dt3.setDate(QDate(4004, 6, 25));
    QCOMPARE(dt3.date(), QDate(4004, 6, 25));
    QCOMPARE(dt3.time(), QTime(0, 45, 57));
    QCOMPARE(dt3.timeSpec(), Qt::UTC);

    QDateTime dt4(QDate(4004, 3, 25), QTime(0, 45, 57), Qt::LocalTime);
    dt4.setDate(QDate(4004, 6, 25));
    QCOMPARE(dt4.date(), QDate(4004, 6, 25));
    QCOMPARE(dt4.time(), QTime(0, 45, 57));
    QCOMPARE(dt4.timeSpec(), Qt::LocalTime);

    QDateTime dt5(QDate(1760, 3, 25), QTime(0, 45, 57), Qt::UTC);
    dt5.setDate(QDate(1760, 6, 25));
    QCOMPARE(dt5.date(), QDate(1760, 6, 25));
    QCOMPARE(dt5.time(), QTime(0, 45, 57));
    QCOMPARE(dt5.timeSpec(), Qt::UTC);

    QDateTime dt6(QDate(1760, 3, 25), QTime(0, 45, 57), Qt::LocalTime);
    dt6.setDate(QDate(1760, 6, 25));
    QCOMPARE(dt6.date(), QDate(1760, 6, 25));
    QCOMPARE(dt6.time(), QTime(0, 45, 57));
    QCOMPARE(dt6.timeSpec(), Qt::LocalTime);
}

void tst_QDateTime::setTime()
{
    QDateTime dt1(QDate(2004, 3, 25), QTime(0, 45, 57), Qt::UTC);
    dt1.setTime(QTime(23, 11, 22));
    QCOMPARE(dt1.date(), QDate(2004, 3, 25));
    QCOMPARE(dt1.time(), QTime(23, 11, 22));
    QCOMPARE(dt1.timeSpec(), Qt::UTC);

    QDateTime dt2(QDate(2004, 3, 25), QTime(0, 45, 57), Qt::LocalTime);
    dt2.setTime(QTime(23, 11, 22));
    QCOMPARE(dt2.date(), QDate(2004, 3, 25));
    QCOMPARE(dt2.time(), QTime(23, 11, 22));
    QCOMPARE(dt2.timeSpec(), Qt::LocalTime);

    QDateTime dt3(QDate(4004, 3, 25), QTime(0, 45, 57), Qt::UTC);
    dt3.setTime(QTime(23, 11, 22));
    QCOMPARE(dt3.date(), QDate(4004, 3, 25));
    QCOMPARE(dt3.time(), QTime(23, 11, 22));
    QCOMPARE(dt3.timeSpec(), Qt::UTC);

    QDateTime dt4(QDate(4004, 3, 25), QTime(0, 45, 57), Qt::LocalTime);
    dt4.setTime(QTime(23, 11, 22));
    QCOMPARE(dt4.date(), QDate(4004, 3, 25));
    QCOMPARE(dt4.time(), QTime(23, 11, 22));
    QCOMPARE(dt4.timeSpec(), Qt::LocalTime);

    QDateTime dt5(QDate(1760, 3, 25), QTime(0, 45, 57), Qt::UTC);
    dt5.setTime(QTime(23, 11, 22));
    QCOMPARE(dt5.date(), QDate(1760, 3, 25));
    QCOMPARE(dt5.time(), QTime(23, 11, 22));
    QCOMPARE(dt5.timeSpec(), Qt::UTC);

    QDateTime dt6(QDate(1760, 3, 25), QTime(0, 45, 57), Qt::LocalTime);
    dt6.setTime(QTime(23, 11, 22));
    QCOMPARE(dt6.date(), QDate(1760, 3, 25));
    QCOMPARE(dt6.time(), QTime(23, 11, 22));
    QCOMPARE(dt6.timeSpec(), Qt::LocalTime);
}

void tst_QDateTime::setTimeSpec()
{
    QDateTime dt1(QDate(2004, 3, 25), QTime(0, 45, 57), Qt::UTC);
    dt1.setTimeSpec(Qt::UTC);
    QCOMPARE(dt1.date(), QDate(2004, 3, 25));
    QCOMPARE(dt1.time(), QTime(0, 45, 57));
    QCOMPARE(dt1.timeSpec(), Qt::UTC);

    dt1.setTimeSpec(Qt::LocalTime);
    QCOMPARE(dt1.date(), QDate(2004, 3, 25));
    QCOMPARE(dt1.time(), QTime(0, 45, 57));
    QCOMPARE(dt1.timeSpec(), Qt::LocalTime);
}

void tst_QDateTime::setTime_t()
{
    QDateTime dt1;
    dt1.setTime_t(0);
    QCOMPARE(dt1.toUTC(), QDateTime(QDate(1970, 1, 1), QTime(), Qt::UTC));
    QCOMPARE(dt1.timeSpec(), Qt::LocalTime);

    dt1.setTimeSpec(Qt::UTC);
    dt1.setTime_t(0);
    QCOMPARE(dt1, QDateTime(QDate(1970, 1, 1), QTime(), Qt::UTC));
    QCOMPARE(dt1.timeSpec(), Qt::UTC);

    dt1.setTime_t(123456);
    QCOMPARE(dt1, QDateTime(QDate(1970, 1, 2), QTime(10, 17, 36), Qt::UTC));
    if (europeanTimeZone) {
        QDateTime dt2;
        dt2.setTime_t(123456);
        QCOMPARE(dt2, QDateTime(QDate(1970, 1, 2), QTime(11, 17, 36), Qt::LocalTime));
    }

    dt1.setTime_t((uint)(quint32)-123456);
    QCOMPARE(dt1, QDateTime(QDate(2106, 2, 5), QTime(20, 10, 40), Qt::UTC));
    if (europeanTimeZone) {
        QDateTime dt2;
        dt2.setTime_t((uint)(quint32)-123456);
        QCOMPARE(dt2, QDateTime(QDate(2106, 2, 5), QTime(21, 10, 40), Qt::LocalTime));
    }

    dt1.setTime_t(1214567890);
    QCOMPARE(dt1, QDateTime(QDate(2008, 6, 27), QTime(11, 58, 10), Qt::UTC));
    if (europeanTimeZone) {
        QDateTime dt2;
        dt2.setTime_t(1214567890);
        QCOMPARE(dt2, QDateTime(QDate(2008, 6, 27), QTime(13, 58, 10), Qt::LocalTime));
    }

    dt1.setTime_t(0x7FFFFFFF);
    QCOMPARE(dt1, QDateTime(QDate(2038, 1, 19), QTime(3, 14, 7), Qt::UTC));
    if (europeanTimeZone) {
        QDateTime dt2;
        dt2.setTime_t(0x7FFFFFFF);
        QCOMPARE(dt2, QDateTime(QDate(2038, 1, 19), QTime(4, 14, 7), Qt::LocalTime));
    }

    dt1 = QDateTime(QDate(2013, 1, 1), QTime(0, 0, 0), Qt::OffsetFromUTC, 60 * 60);
    dt1.setTime_t(123456);
    QCOMPARE(dt1, QDateTime(QDate(1970, 1, 2), QTime(10, 17, 36), Qt::UTC));
    QCOMPARE(dt1.timeSpec(), Qt::OffsetFromUTC);
    QCOMPARE(dt1.offsetFromUtc(), 60 * 60);
}

void tst_QDateTime::setMSecsSinceEpoch_data()
{
    QTest::addColumn<qint64>("msecs");
    QTest::addColumn<QDateTime>("utc");
    QTest::addColumn<QDateTime>("european");

    QTest::newRow("zero")
            << Q_INT64_C(0)
            << QDateTime(QDate(1970, 1, 1), QTime(), Qt::UTC)
            << QDateTime(QDate(1970, 1, 1), QTime(1, 0));
    QTest::newRow("-1")
            << Q_INT64_C(-1)
            << QDateTime(QDate(1969, 12, 31), QTime(23, 59, 59, 999), Qt::UTC)
            << QDateTime(QDate(1970, 1, 1), QTime(0, 59, 59, 999));
    QTest::newRow("123456789")
            << Q_INT64_C(123456789)
            << QDateTime(QDate(1970, 1, 2), QTime(10, 17, 36, 789), Qt::UTC)
            << QDateTime(QDate(1970, 1, 2), QTime(11, 17, 36, 789), Qt::LocalTime);
    QTest::newRow("-123456789")
            << Q_INT64_C(-123456789)
            << QDateTime(QDate(1969, 12, 30), QTime(13, 42, 23, 211), Qt::UTC)
            << QDateTime(QDate(1969, 12, 30), QTime(14, 42, 23, 211), Qt::LocalTime);
    QTest::newRow("non-time_t")
            << (Q_INT64_C(1000) << 32)
            << QDateTime(QDate(2106, 2, 7), QTime(6, 28, 16), Qt::UTC)
            << QDateTime(QDate(2106, 2, 7), QTime(7, 28, 16));
    QTest::newRow("very-large")
            << (Q_INT64_C(123456) << 32)
            << QDateTime(QDate(18772, 8, 15), QTime(1, 8, 14, 976), Qt::UTC)
            << QDateTime(QDate(18772, 8, 15), QTime(3, 8, 14, 976));
    QTest::newRow("min_date") // julian day 0 is an invalid date for QDate
            << Q_INT64_C(-210866716800000)
            << QDateTime(QDate::fromJulianDay(1), QTime(), Qt::UTC)
            << QDateTime(QDate::fromJulianDay(1), QTime(1, 0));
    QTest::newRow("max_date") // technically jd is unsigned, but fromJulianDay takes int
            << Q_INT64_C(185331720376799999)
            << QDateTime(QDate::fromJulianDay(0x7fffffff), QTime(21, 59, 59, 999), Qt::UTC)
            << QDateTime(QDate::fromJulianDay(0x7fffffff), QTime(23, 59, 59, 999));
}

void tst_QDateTime::setMSecsSinceEpoch()
{
    QFETCH(qint64, msecs);
    QFETCH(QDateTime, utc);
    QFETCH(QDateTime, european);

    QDateTime dt;
    dt.setTimeSpec(Qt::UTC);
    dt.setMSecsSinceEpoch(msecs);

    QCOMPARE(dt, utc);
    QCOMPARE(dt.timeSpec(), Qt::UTC);

    if (europeanTimeZone) {
        QCOMPARE(dt.toLocalTime(), european);
    }

    QCOMPARE(dt.toMSecsSinceEpoch(), msecs);

    if (quint64(msecs / 1000) < 0xFFFFFFFF) {
        QCOMPARE(qint64(dt.toTime_t()), msecs / 1000);
    }

    QDateTime reference(QDate(1970, 1, 1), QTime(), Qt::UTC);
    QCOMPARE(dt, reference.addMSecs(msecs));
}

void tst_QDateTime::toString_isoDate_data()
{
    QTest::addColumn<QDateTime>("datetime");
    QTest::addColumn<QString>("expected");

    QTest::newRow("localtime")
            << QDateTime(QDate(1978, 11, 9), QTime(13, 28, 34))
            << QString("1978-11-09T13:28:34");
    QTest::newRow("UTC")
            << QDateTime(QDate(1978, 11, 9), QTime(13, 28, 34), Qt::UTC)
            << QString("1978-11-09T13:28:34Z");
    QDateTime dt(QDate(1978, 11, 9), QTime(13, 28, 34));
    dt.setOffsetFromUtc(19800);
    QTest::newRow("positive OffsetFromUTC")
            << dt
            << QString("1978-11-09T13:28:34+05:30");
    dt.setOffsetFromUtc(-7200);
    QTest::newRow("negative OffsetFromUTC")
            << dt
            << QString("1978-11-09T13:28:34-02:00");
}

void tst_QDateTime::toString_isoDate()
{
    QFETCH(QDateTime, datetime);
    QFETCH(QString, expected);

    QLocale oldLocale;
    QLocale::setDefault(QLocale("en_US"));

    QString result = datetime.toString(Qt::ISODate);
    QCOMPARE(result, expected);

    QDateTime resultDatetime = QDateTime::fromString(result, Qt::ISODate);
    // If expecting invalid result the datetime may still be valid, i.e. year < 0 or > 9999
    if (!expected.isEmpty()) {
        QCOMPARE(resultDatetime, datetime);
        QCOMPARE(resultDatetime.date(), datetime.date());
        QCOMPARE(resultDatetime.time(), datetime.time());
        QCOMPARE(resultDatetime.timeSpec(), datetime.timeSpec());
        QCOMPARE(resultDatetime.utcOffset(), datetime.utcOffset());
    } else {
        QCOMPARE(resultDatetime, QDateTime());
    }

    QLocale::setDefault(oldLocale);
}

void tst_QDateTime::toString_textDate_data()
{
    QTest::addColumn<QDateTime>("datetime");
    QTest::addColumn<QString>("expected");

    QTest::newRow("localtime")  << QDateTime(QDate(2013, 1, 2), QTime(1, 2, 3), Qt::LocalTime)
                                << QString("Wed Jan 2 01:02:03.000 2013");
    QTest::newRow("utc")        << QDateTime(QDate(2013, 1, 2), QTime(1, 2, 3), Qt::UTC)
                                << QString("Wed Jan 2 01:02:03.000 2013 GMT");
    QTest::newRow("offset+")    << QDateTime(QDate(2013, 1, 2), QTime(1, 2, 3), Qt::OffsetFromUTC,
                                             10 * 60 * 60)
                                << QString("Wed Jan 2 01:02:03.000 2013 GMT+1000");
    QTest::newRow("offset-")    << QDateTime(QDate(2013, 1, 2), QTime(1, 2, 3), Qt::OffsetFromUTC,
                                             -10 * 60 * 60)
                                << QString("Wed Jan 2 01:02:03.000 2013 GMT-1000");
    QTest::newRow("invalid")    << QDateTime()
                                << QString("");
}

void tst_QDateTime::toString_textDate()
{
    QFETCH(QDateTime, datetime);
    QFETCH(QString, expected);

    QLocale oldLocale;
    QLocale::setDefault(QLocale("en_US"));

    QString result = datetime.toString(Qt::TextDate);
    QCOMPARE(result, expected);

    QDateTime resultDatetime = QDateTime::fromString(result, Qt::TextDate);
    QCOMPARE(resultDatetime, datetime);
    QCOMPARE(resultDatetime.date(), datetime.date());
    QCOMPARE(resultDatetime.time(), datetime.time());
    QCOMPARE(resultDatetime.timeSpec(), datetime.timeSpec());
    QCOMPARE(resultDatetime.utcOffset(), datetime.utcOffset());

    QLocale::setDefault(oldLocale);
}

void tst_QDateTime::toString_enumformat()
{
    QDateTime dt1(QDate(1995, 5, 20), QTime(12, 34, 56));


    QString str1 = dt1.toString(Qt::TextDate);
    QVERIFY(!str1.isEmpty()); // It's locale dependent everywhere

    QString str2 = dt1.toString(Qt::ISODate);
    QCOMPARE(str2, QString("1995-05-20T12:34:56"));

    QString str3 = dt1.toString(Qt::LocalDate);
    qDebug() << str3;
    QVERIFY(!str3.isEmpty());
    //check for date/time components in any order
    //year may be 2 or 4 digits
    QVERIFY(str3.contains("95"));
    //day and month may be in numeric or word form
    QVERIFY(str3.contains("12"));
    QVERIFY(str3.contains("34"));
    //seconds may be absent
}

void tst_QDateTime::addDays()
{
    for (int pass = 0; pass < 2; ++pass) {
        QDateTime dt(QDate(2004, 1, 1), QTime(12, 34, 56), pass == 0 ? Qt::LocalTime : Qt::UTC);
        dt = dt.addDays(185);
        QVERIFY(dt.date().year() == 2004 && dt.date().month() == 7 && dt.date().day() == 4);
        QVERIFY(dt.time().hour() == 12 && dt.time().minute() == 34 && dt.time().second() == 56
               && dt.time().msec() == 0);
        QCOMPARE(dt.timeSpec(), (pass == 0 ? Qt::LocalTime : Qt::UTC));

        dt = dt.addDays(-185);
        QCOMPARE(dt.date(), QDate(2004, 1, 1));
        QCOMPARE(dt.time(), QTime(12, 34, 56));
    }

    QDateTime dt(QDate(1752, 9, 14), QTime(12, 34, 56));
    while (dt.date().year() < 8000) {
        int year = dt.date().year();
        if (QDate::isLeapYear(year + 1))
            dt = dt.addDays(366);
        else
            dt = dt.addDays(365);
        QCOMPARE(dt.date(), QDate(year + 1, 9, 14));
        QCOMPARE(dt.time(), QTime(12, 34, 56));
    }

    // Test preserves TimeSpec
    QDateTime dt1(QDate(2013, 1, 1), QTime(0, 0, 0), Qt::UTC);
    QDateTime dt2 = dt1.addDays(2);
    QCOMPARE(dt2.date(), QDate(2013, 1, 3));
    QCOMPARE(dt2.time(), QTime(0, 0, 0));
    QCOMPARE(dt2.timeSpec(), Qt::UTC);

    dt1 = QDateTime(QDate(2013, 1, 1), QTime(0, 0, 0), Qt::LocalTime);
    dt2 = dt1.addDays(2);
    QCOMPARE(dt2.date(), QDate(2013, 1, 3));
    QCOMPARE(dt2.time(), QTime(0, 0, 0));
    QCOMPARE(dt2.timeSpec(), Qt::LocalTime);

    dt1 = QDateTime(QDate(2013, 1, 1), QTime(0, 0, 0), Qt::OffsetFromUTC, 60*60);
    dt2 = dt1.addDays(2);
    QCOMPARE(dt2.date(), QDate(2013, 1, 3));
    QCOMPARE(dt2.time(), QTime(0, 0, 0));
    QCOMPARE(dt2.timeSpec(), Qt::OffsetFromUTC);
    QCOMPARE(dt2.offsetFromUtc(), 60 * 60);

    // ### test invalid QDateTime()
}


void tst_QDateTime::addMonths_data()
{
    QTest::addColumn<int>("months");
    QTest::addColumn<QDate>("resultDate");

    QTest::newRow("-15") << -15 << QDate(2002, 10, 31);
    QTest::newRow("-14") << -14 << QDate(2002, 11, 30);
    QTest::newRow("-13") << -13 << QDate(2002, 12, 31);
    QTest::newRow("-12") << -12 << QDate(2003, 1, 31);

    QTest::newRow("-11") << -11 << QDate(2003, 2, 28);
    QTest::newRow("-10") << -10 << QDate(2003, 3, 31);
    QTest::newRow("-9") << -9 << QDate(2003, 4, 30);
    QTest::newRow("-8") << -8 << QDate(2003, 5, 31);
    QTest::newRow("-7") << -7 << QDate(2003, 6, 30);
    QTest::newRow("-6") << -6 << QDate(2003, 7, 31);
    QTest::newRow("-5") << -5 << QDate(2003, 8, 31);
    QTest::newRow("-4") << -4 << QDate(2003, 9, 30);
    QTest::newRow("-3") << -3 << QDate(2003, 10, 31);
    QTest::newRow("-2") << -2 << QDate(2003, 11, 30);
    QTest::newRow("-1") << -1 << QDate(2003, 12, 31);
    QTest::newRow("0") << 0 << QDate(2004, 1, 31);
    QTest::newRow("1") << 1 << QDate(2004, 2, 29);
    QTest::newRow("2") << 2 << QDate(2004, 3, 31);
    QTest::newRow("3") << 3 << QDate(2004, 4, 30);
    QTest::newRow("4") << 4 << QDate(2004, 5, 31);
    QTest::newRow("5") << 5 << QDate(2004, 6, 30);
    QTest::newRow("6") << 6 << QDate(2004, 7, 31);
    QTest::newRow("7") << 7 << QDate(2004, 8, 31);
    QTest::newRow("8") << 8 << QDate(2004, 9, 30);
    QTest::newRow("9") << 9 << QDate(2004, 10, 31);
    QTest::newRow("10") << 10 << QDate(2004, 11, 30);
    QTest::newRow("11") << 11 << QDate(2004, 12, 31);
    QTest::newRow("12") << 12 << QDate(2005, 1, 31);
    QTest::newRow("13") << 13 << QDate(2005, 2, 28);
    QTest::newRow("14") << 14 << QDate(2005, 3, 31);
    QTest::newRow("15") << 15 << QDate(2005, 4, 30);
}

void tst_QDateTime::addMonths()
{
    QFETCH(int, months);
    QFETCH(QDate, resultDate);

    QDate testDate(2004, 1, 31);
    QTime testTime(12, 34, 56);
    QDateTime start(testDate, testTime);
    QDateTime end = start.addMonths(months);
    QCOMPARE(end.date(), resultDate);
    QCOMPARE(end.time(), testTime);
    QCOMPARE(end.timeSpec(), Qt::LocalTime);

    start = QDateTime(testDate, testTime, Qt::UTC);
    end = start.addMonths(months);
    QCOMPARE(end.date(), resultDate);
    QCOMPARE(end.time(), testTime);
    QCOMPARE(end.timeSpec(), Qt::UTC);

    start = QDateTime(testDate, testTime, Qt::OffsetFromUTC, 60 * 60);
    end = start.addMonths(months);
    QCOMPARE(end.date(), resultDate);
    QCOMPARE(end.time(), testTime);
    QCOMPARE(end.timeSpec(), Qt::OffsetFromUTC);
    QCOMPARE(end.offsetFromUtc(), 60 * 60);
}

void tst_QDateTime::addYears_data()
{
    QTest::addColumn<int>("years1");
    QTest::addColumn<int>("years2");
    QTest::addColumn<QDate>("startDate");
    QTest::addColumn<QDate>("resultDate");

    QTest::newRow("0") << 0 << 0 << QDate(1752, 9, 14) << QDate(1752, 9, 14);
    QTest::newRow("4000 - 4000") << 4000 << -4000 << QDate(1752, 9, 14) << QDate(1752, 9, 14);
    QTest::newRow("10") << 10 << 0 << QDate(1752, 9, 14) << QDate(1762, 9, 14);
    QTest::newRow("0 leap year") << 0 << 0 << QDate(1760, 2, 29) << QDate(1760, 2, 29);
    QTest::newRow("1 leap year") << 1 << 0 << QDate(1760, 2, 29) << QDate(1761, 2, 28);
    QTest::newRow("2 leap year") << 2 << 0 << QDate(1760, 2, 29) << QDate(1762, 2, 28);
    QTest::newRow("3 leap year") << 3 << 0 << QDate(1760, 2, 29) << QDate(1763, 2, 28);
    QTest::newRow("4 leap year") << 4 << 0 << QDate(1760, 2, 29) << QDate(1764, 2, 29);

    QTest::newRow("toNegative1") << -2000 << 0 << QDate(1752, 9, 14) << QDate(-249, 9, 14);
    QTest::newRow("toNegative2") << -1752 << 0 << QDate(1752, 9, 14) << QDate(-1, 9, 14);
    QTest::newRow("toNegative3") << -1751 << 0 << QDate(1752, 9, 14) << QDate(1, 9, 14);
    QTest::newRow("toPositive1") << 2000 << 0 << QDate(-1752, 9, 14) << QDate(249, 9, 14);
    QTest::newRow("toPositive2") << 1752 << 0 << QDate(-1752, 9, 14) << QDate(1, 9, 14);
    QTest::newRow("toPositive3") << 1751 << 0 << QDate(-1752, 9, 14) << QDate(-1, 9, 14);
}

void tst_QDateTime::addYears()
{
    QFETCH(int, years1);
    QFETCH(int, years2);
    QFETCH(QDate, startDate);
    QFETCH(QDate, resultDate);

    QTime testTime(14, 25, 36);
    QDateTime start(startDate, testTime);
    QDateTime end = start.addYears(years1).addYears(years2);
    QCOMPARE(end.date(), resultDate);
    QCOMPARE(end.time(), testTime);
    QCOMPARE(end.timeSpec(), Qt::LocalTime);

    start = QDateTime(startDate, testTime, Qt::UTC);
    end = start.addYears(years1).addYears(years2);
    QCOMPARE(end.date(), resultDate);
    QCOMPARE(end.time(), testTime);
    QCOMPARE(end.timeSpec(), Qt::UTC);

    start = QDateTime(startDate, testTime, Qt::OffsetFromUTC, 60 * 60);
    end = start.addYears(years1).addYears(years2);
    QCOMPARE(end.date(), resultDate);
    QCOMPARE(end.time(), testTime);
    QCOMPARE(end.timeSpec(), Qt::OffsetFromUTC);
    QCOMPARE(end.offsetFromUtc(), 60 * 60);
}

void tst_QDateTime::addSecs_data()
{
    QTest::addColumn<QDateTime>("dt");
    QTest::addColumn<int>("nsecs");
    QTest::addColumn<QDateTime>("result");

    QTime standardTime(12, 34, 56);
    QTime daylightTime(13, 34, 56);

    QTest::newRow("utc0") << QDateTime(QDate(2004, 1, 1), standardTime, Qt::UTC) << 86400
                       << QDateTime(QDate(2004, 1, 2), standardTime, Qt::UTC);
    QTest::newRow("utc1") << QDateTime(QDate(2004, 1, 1), standardTime, Qt::UTC) << (86400 * 185)
                       << QDateTime(QDate(2004, 7, 4), standardTime, Qt::UTC);
    QTest::newRow("utc2") << QDateTime(QDate(2004, 1, 1), standardTime, Qt::UTC) << (86400 * 366)
                       << QDateTime(QDate(2005, 1, 1), standardTime, Qt::UTC);
    QTest::newRow("utc3") << QDateTime(QDate(1760, 1, 1), standardTime, Qt::UTC) << 86400
                       << QDateTime(QDate(1760, 1, 2), standardTime, Qt::UTC);
    QTest::newRow("utc4") << QDateTime(QDate(1760, 1, 1), standardTime, Qt::UTC) << (86400 * 185)
                       << QDateTime(QDate(1760, 7, 4), standardTime, Qt::UTC);
    QTest::newRow("utc5") << QDateTime(QDate(1760, 1, 1), standardTime, Qt::UTC) << (86400 * 366)
                       << QDateTime(QDate(1761, 1, 1), standardTime, Qt::UTC);
    QTest::newRow("utc6") << QDateTime(QDate(4000, 1, 1), standardTime, Qt::UTC) << 86400
                       << QDateTime(QDate(4000, 1, 2), standardTime, Qt::UTC);
    QTest::newRow("utc7") << QDateTime(QDate(4000, 1, 1), standardTime, Qt::UTC) << (86400 * 185)
                       << QDateTime(QDate(4000, 7, 4), standardTime, Qt::UTC);
    QTest::newRow("utc8") << QDateTime(QDate(4000, 1, 1), standardTime, Qt::UTC) << (86400 * 366)
                       << QDateTime(QDate(4001, 1, 1), standardTime, Qt::UTC);
    QTest::newRow("utc9") << QDateTime(QDate(4000, 1, 1), standardTime, Qt::UTC) << 0
                       << QDateTime(QDate(4000, 1, 1), standardTime, Qt::UTC);

    if (europeanTimeZone) {
        QTest::newRow("cet0") << QDateTime(QDate(2004, 1, 1), standardTime, Qt::LocalTime) << 86400
                           << QDateTime(QDate(2004, 1, 2), standardTime, Qt::LocalTime);
        QTest::newRow("cet1") << QDateTime(QDate(2004, 1, 1), standardTime, Qt::LocalTime) << (86400 * 185)
                           << QDateTime(QDate(2004, 7, 4), daylightTime, Qt::LocalTime);
        QTest::newRow("cet2") << QDateTime(QDate(2004, 1, 1), standardTime, Qt::LocalTime) << (86400 * 366)
                           << QDateTime(QDate(2005, 1, 1), standardTime, Qt::LocalTime);
        QTest::newRow("cet3") << QDateTime(QDate(1760, 1, 1), standardTime, Qt::LocalTime) << 86400
                           << QDateTime(QDate(1760, 1, 2), standardTime, Qt::LocalTime);
        QTest::newRow("cet4") << QDateTime(QDate(1760, 1, 1), standardTime, Qt::LocalTime) << (86400 * 185)
                           << QDateTime(QDate(1760, 7, 4), standardTime, Qt::LocalTime);
        QTest::newRow("cet5") << QDateTime(QDate(1760, 1, 1), standardTime, Qt::LocalTime) << (86400 * 366)
                           << QDateTime(QDate(1761, 1, 1), standardTime, Qt::LocalTime);
        QTest::newRow("cet6") << QDateTime(QDate(4000, 1, 1), standardTime, Qt::LocalTime) << 86400
                           << QDateTime(QDate(4000, 1, 2), standardTime, Qt::LocalTime);
        QTest::newRow("cet7") << QDateTime(QDate(4000, 1, 1), standardTime, Qt::LocalTime) << (86400 * 185)
                           << QDateTime(QDate(4000, 7, 4), daylightTime, Qt::LocalTime);
        QTest::newRow("cet8") << QDateTime(QDate(4000, 1, 1), standardTime, Qt::LocalTime) << (86400 * 366)
                           << QDateTime(QDate(4001, 1, 1), standardTime, Qt::LocalTime);
        QTest::newRow("cet9") << QDateTime(QDate(4000, 1, 1), standardTime, Qt::LocalTime) << 0
                           << QDateTime(QDate(4000, 1, 1), standardTime, Qt::LocalTime);
    }

    // Year sign change
    QTest::newRow("toNegative") << QDateTime(QDate(1, 1, 1), QTime(0, 0, 0), Qt::UTC)
                                << -1
                                << QDateTime(QDate(-1, 12, 31), QTime(23, 59, 59), Qt::UTC);
    QTest::newRow("toPositive") << QDateTime(QDate(-1, 12, 31), QTime(23, 59, 59), Qt::UTC)
                                << 1
                                << QDateTime(QDate(1, 1, 1), QTime(0, 0, 0), Qt::UTC);

    // Gregorian/Julian switchover
    QTest::newRow("toGregorian") << QDateTime(QDate(1582, 10, 4), QTime(23, 59, 59))
                                 << 1
                                 << QDateTime(QDate(1582, 10, 15), QTime(0, 0, 0));
    QTest::newRow("toJulian") << QDateTime(QDate(1582, 10, 15), QTime(0, 0, 0))
                              << -1
                              << QDateTime(QDate(1582, 10, 4), QTime(23, 59, 59));

    // Check Offset details are preserved
    QTest::newRow("offset0") << QDateTime(QDate(2013, 1, 1), QTime(1, 2, 3),
                                          Qt::OffsetFromUTC, 60 * 60)
                             << 60 * 60
                             << QDateTime(QDate(2013, 1, 1), QTime(2, 2, 3),
                                          Qt::OffsetFromUTC, 60 * 60);
}

void tst_QDateTime::addSecs()
{
    QFETCH(QDateTime, dt);
    QFETCH(int, nsecs);
    QFETCH(QDateTime, result);

    QDateTime test = dt.addSecs(nsecs);
    QCOMPARE(test, result);
    QCOMPARE(test.timeSpec(), dt.timeSpec());
    if (test.timeSpec() == Qt::OffsetFromUTC)
        QCOMPARE(test.offsetFromUtc(), dt.offsetFromUtc());
    QCOMPARE(result.addSecs(-nsecs), dt);
}

void tst_QDateTime::addMSecs_data()
{
    addSecs_data();
}

void tst_QDateTime::addMSecs()
{
    QFETCH(QDateTime, dt);
    QFETCH(int, nsecs);
    QFETCH(QDateTime, result);

    QDateTime test = dt.addMSecs(qint64(nsecs) * 1000);
    QCOMPARE(test, result);
    QCOMPARE(test.timeSpec(), dt.timeSpec());
    if (test.timeSpec() == Qt::OffsetFromUTC)
        QCOMPARE(test.offsetFromUtc(), dt.offsetFromUtc());
    QCOMPARE(result.addMSecs(qint64(-nsecs) * 1000), dt);
}

void tst_QDateTime::toTimeSpec_data()
{
    QTest::addColumn<QDateTime>("fromUtc");
    QTest::addColumn<QDateTime>("fromLocal");

    QTime utcTime(4, 20, 30);
    QTime localStandardTime(5, 20, 30);
    QTime localDaylightTime(6, 20, 30);

    QTest::newRow("winter1") << QDateTime(QDate(2004, 1, 1), utcTime, Qt::UTC)
                          << QDateTime(QDate(2004, 1, 1), localStandardTime, Qt::LocalTime);
    QTest::newRow("winter2") << QDateTime(QDate(2004, 2, 29), utcTime, Qt::UTC)
                          << QDateTime(QDate(2004, 2, 29), localStandardTime, Qt::LocalTime);
    QTest::newRow("winter3") << QDateTime(QDate(1760, 2, 29), utcTime, Qt::UTC)
                          << QDateTime(QDate(1760, 2, 29), localStandardTime, Qt::LocalTime);
    QTest::newRow("winter4") << QDateTime(QDate(6000, 2, 29), utcTime, Qt::UTC)
                          << QDateTime(QDate(6000, 2, 29), localStandardTime, Qt::LocalTime);

    if (europeanTimeZone) {
        QTest::newRow("summer1") << QDateTime(QDate(2004, 6, 30), utcTime, Qt::UTC)
                                 << QDateTime(QDate(2004, 6, 30), localDaylightTime, Qt::LocalTime);
        QTest::newRow("summer2") << QDateTime(QDate(1760, 6, 30), utcTime, Qt::UTC)
                                 << QDateTime(QDate(1760, 6, 30), localStandardTime, Qt::LocalTime);
        QTest::newRow("summer3") << QDateTime(QDate(4000, 6, 30), utcTime, Qt::UTC)
                                 << QDateTime(QDate(4000, 6, 30), localDaylightTime, Qt::LocalTime);
    }

    QTest::newRow("msec") << QDateTime(QDate(4000, 6, 30), utcTime.addMSecs(1), Qt::UTC)
                       << QDateTime(QDate(4000, 6, 30), localDaylightTime.addMSecs(1), Qt::LocalTime);
}

void tst_QDateTime::toTimeSpec()
{
    if (europeanTimeZone) {
        QFETCH(QDateTime, fromUtc);
        QFETCH(QDateTime, fromLocal);

        QDateTime utcToUtc = fromUtc.toTimeSpec(Qt::UTC);
        QDateTime localToLocal = fromLocal.toTimeSpec(Qt::LocalTime);
        QDateTime utcToLocal = fromUtc.toTimeSpec(Qt::LocalTime);
        QDateTime localToUtc = fromLocal.toTimeSpec(Qt::UTC);
        QDateTime utcToOffset = fromUtc.toTimeSpec(Qt::OffsetFromUTC);
        QDateTime localToOffset = fromLocal.toTimeSpec(Qt::OffsetFromUTC);

        QCOMPARE(utcToUtc, fromUtc);
        QCOMPARE(utcToUtc.date(), fromUtc.date());
        QCOMPARE(utcToUtc.time(), fromUtc.time());
        QCOMPARE(utcToUtc.timeSpec(), Qt::UTC);

        QCOMPARE(localToLocal, fromLocal);
        QCOMPARE(localToLocal.date(), fromLocal.date());
        QCOMPARE(localToLocal.time(), fromLocal.time());
        QCOMPARE(localToLocal.timeSpec(), Qt::LocalTime);

        QCOMPARE(utcToLocal, fromLocal);
        QCOMPARE(utcToLocal.date(), fromLocal.date());
        QCOMPARE(utcToLocal.time(), fromLocal.time());
        QCOMPARE(utcToLocal.timeSpec(), Qt::LocalTime);

        QCOMPARE(localToUtc, fromUtc);
        QCOMPARE(localToUtc.date(), fromUtc.date());
        QCOMPARE(localToUtc.time(), fromUtc.time());
        QCOMPARE(localToUtc.timeSpec(), Qt::UTC);

        QCOMPARE(utcToUtc, localToUtc);
        QCOMPARE(utcToUtc.date(), localToUtc.date());
        QCOMPARE(utcToUtc.time(), localToUtc.time());
        QCOMPARE(utcToUtc.timeSpec(), Qt::UTC);

        QCOMPARE(utcToLocal, localToLocal);
        QCOMPARE(utcToLocal.date(), localToLocal.date());
        QCOMPARE(utcToLocal.time(), localToLocal.time());
        QCOMPARE(utcToLocal.timeSpec(), Qt::LocalTime);

        // OffsetToUTC becomes UTC
        QCOMPARE(utcToOffset, fromUtc);
        QCOMPARE(utcToOffset.date(), fromUtc.date());
        QCOMPARE(utcToOffset.time(), fromUtc.time());
        QCOMPARE(utcToOffset.timeSpec(), Qt::UTC);

        QCOMPARE(localToOffset, fromUtc);
        QCOMPARE(localToOffset.date(), fromUtc.date());
        QCOMPARE(localToOffset.time(), fromUtc.time());
        QCOMPARE(localToOffset.timeSpec(), Qt::UTC);
    } else {
        QSKIP("Not tested with timezone other than Central European (CET/CST)", SkipAll);
    }
}

void tst_QDateTime::toLocalTime_data()
{
    toTimeSpec_data();
}

void tst_QDateTime::toLocalTime()
{
    if (europeanTimeZone) {
        QFETCH(QDateTime, fromUtc);
        QFETCH(QDateTime, fromLocal);

        QCOMPARE(fromLocal.toLocalTime(), fromLocal);

        QCOMPARE(fromUtc.toLocalTime(), fromLocal);
        QCOMPARE(fromUtc.toLocalTime(), fromLocal.toLocalTime());
    } else {
        QSKIP("Not tested with timezone other than Central European (CET/CST)", SkipAll);
    }
}

void tst_QDateTime::toUTC_data()
{
    toTimeSpec_data();
}

void tst_QDateTime::toUTC()
{
    if (europeanTimeZone) {
        QFETCH(QDateTime, fromUtc);
        QFETCH(QDateTime, fromLocal);

        QCOMPARE(fromUtc.toUTC(), fromUtc);

        QCOMPARE(fromLocal.toUTC(), fromUtc);
        QCOMPARE(fromUtc.toUTC(), fromLocal.toUTC());
    } else {
        QSKIP("Not tested with timezone other than Central European (CET/CST)", SkipAll);
    }

    // To make sure bug 72713 never happens again
    QDateTime dt = QDateTime::currentDateTime();
    if(dt.time().msec() == 0){
        dt.setTime(dt.time().addMSecs(1));
    }
    QString s = dt.toString("zzz");
    QString t = dt.toUTC().toString("zzz");
    QCOMPARE(s, t);
}

void tst_QDateTime::daysTo()
{
    QDateTime dt1(QDate(1760, 1, 2), QTime());
    QDateTime dt2(QDate(1760, 2, 2), QTime());
    QDateTime dt3(QDate(1760, 3, 2), QTime());

    QCOMPARE(dt1.daysTo(dt2), 31);
    QCOMPARE(dt1.addDays(31), dt2);

    QCOMPARE(dt2.daysTo(dt3), 29);
    QCOMPARE(dt2.addDays(29), dt3);

    QCOMPARE(dt1.daysTo(dt3), 60);
    QCOMPARE(dt1.addDays(60), dt3);

    QCOMPARE(dt2.daysTo(dt1), -31);
    QCOMPARE(dt2.addDays(-31), dt1);

    QCOMPARE(dt3.daysTo(dt2), -29);
    QCOMPARE(dt3.addDays(-29), dt2);

    QCOMPARE(dt3.daysTo(dt1), -60);
    QCOMPARE(dt3.addDays(-60), dt1);
}

void tst_QDateTime::secsTo_data()
{
    addSecs_data();

    QTest::newRow("disregard milliseconds #1")
        << QDateTime(QDate(2012, 3, 7), QTime(0, 58, 0, 0)) << 60
        << QDateTime(QDate(2012, 3, 7), QTime(0, 59, 0, 400));

    QTest::newRow("disregard milliseconds #2")
        << QDateTime(QDate(2012, 3, 7), QTime(0, 59, 0, 0)) << 60
        << QDateTime(QDate(2012, 3, 7), QTime(1, 0, 0, 400));
}

void tst_QDateTime::secsTo()
{
    QFETCH(QDateTime, dt);
    QFETCH(int, nsecs);
    QFETCH(QDateTime, result);

    if (dt.isValid()) {
        QCOMPARE(dt.secsTo(result), nsecs);
        QCOMPARE(result.secsTo(dt), -nsecs);
        QVERIFY((dt == result) == (0 == nsecs));
        QVERIFY((dt != result) == (0 != nsecs));
        QVERIFY((dt < result) == (0 < nsecs));
        QVERIFY((dt <= result) == (0 <= nsecs));
        QVERIFY((dt > result) == (0 > nsecs));
        QVERIFY((dt >= result) == (0 >= nsecs));
    } else {
        QVERIFY(dt.secsTo(result) == 0);
        QVERIFY(result.secsTo(dt) == 0);
    }
}

void tst_QDateTime::msecsTo_data()
{
    addMSecs_data();
}

void tst_QDateTime::msecsTo()
{
    QFETCH(QDateTime, dt);
    QFETCH(int, nsecs);
    QFETCH(QDateTime, result);

    if (dt.isValid()) {
        QCOMPARE(dt.msecsTo(result), qint64(nsecs) * 1000);
        QCOMPARE(result.msecsTo(dt), -qint64(nsecs) * 1000);
        QVERIFY((dt == result) == (0 == (qint64(nsecs) * 1000)));
        QVERIFY((dt != result) == (0 != (qint64(nsecs) * 1000)));
        QVERIFY((dt < result) == (0 < (qint64(nsecs) * 1000)));
        QVERIFY((dt <= result) == (0 <= (qint64(nsecs) * 1000)));
        QVERIFY((dt > result) == (0 > (qint64(nsecs) * 1000)));
        QVERIFY((dt >= result) == (0 >= (qint64(nsecs) * 1000)));
    } else {
        QVERIFY(dt.msecsTo(result) == 0);
        QVERIFY(result.msecsTo(dt) == 0);
    }
}

void tst_QDateTime::currentDateTime()
{
#if defined(Q_OS_WINCE)
    __time64_t buf1, buf2;
    ::_time64(&buf1);
#else
    time_t buf1, buf2;
    ::time(&buf1);
#endif
    QDateTime lowerBound;
    lowerBound.setTime_t(buf1);

    QDateTime dt1 = QDateTime::currentDateTime();
    QDateTime dt2 = QDateTime::currentDateTime().toLocalTime();
    QDateTime dt3 = QDateTime::currentDateTime().toUTC();

#if defined(Q_OS_WINCE)
    ::_time64(&buf2);
#else
    ::time(&buf2);
#endif
    QDateTime upperBound;
    upperBound.setTime_t(buf2);
    // Note we must add 2 seconds here because time() may return up to
    // 1 second difference from the more accurate method used by QDateTime::currentDateTime()
    upperBound = upperBound.addSecs(2);

    QString details = QString("\n"
        "lowerBound: %1\n"
        "dt1:        %2\n"
        "dt2:        %3\n"
        "dt3:        %4\n"
        "upperBound: %5\n")
        .arg(lowerBound.toTime_t())
        .arg(dt1.toTime_t())
        .arg(dt2.toTime_t())
        .arg(dt3.toTime_t())
        .arg(upperBound.toTime_t());

    QVERIFY2(lowerBound < upperBound, qPrintable(details));

    QVERIFY2(lowerBound <= dt1, qPrintable(details));
    QVERIFY2(dt1 < upperBound, qPrintable(details));
    QVERIFY2(lowerBound <= dt2, qPrintable(details));
    QVERIFY2(dt2 < upperBound, qPrintable(details));
    QVERIFY2(lowerBound <= dt3, qPrintable(details));
    QVERIFY2(dt3 < upperBound, qPrintable(details));

    QVERIFY(dt1.timeSpec() == Qt::LocalTime);
    QVERIFY(dt2.timeSpec() == Qt::LocalTime);
    QVERIFY(dt3.timeSpec() == Qt::UTC);
}

void tst_QDateTime::currentDateTimeUtc()
{
#if defined(Q_OS_WINCE)
    __time64_t buf1, buf2;
    ::_time64(&buf1);
#else
    time_t buf1, buf2;
    ::time(&buf1);
#endif
    QDateTime lowerBound;
    lowerBound.setTime_t(buf1);

    QDateTime dt1 = QDateTime::currentDateTimeUtc();
    QDateTime dt2 = QDateTime::currentDateTimeUtc().toLocalTime();
    QDateTime dt3 = QDateTime::currentDateTimeUtc().toUTC();

#if defined(Q_OS_WINCE)
    ::_time64(&buf2);
#else
    ::time(&buf2);
#endif
    QDateTime upperBound;
    upperBound.setTime_t(buf2);
    // Note we must add 2 seconds here because time() may return up to
    // 1 second difference from the more accurate method used by QDateTime::currentDateTime()
    upperBound = upperBound.addSecs(2);

    QString details = QString("\n"
        "lowerBound: %1\n"
        "dt1:        %2\n"
        "dt2:        %3\n"
        "dt3:        %4\n"
        "upperBound: %5\n")
        .arg(lowerBound.toTime_t())
        .arg(dt1.toTime_t())
        .arg(dt2.toTime_t())
        .arg(dt3.toTime_t())
        .arg(upperBound.toTime_t());

    QVERIFY2(lowerBound < upperBound, qPrintable(details));

    QVERIFY2(lowerBound <= dt1, qPrintable(details));
    QVERIFY2(dt1 < upperBound, qPrintable(details));
    QVERIFY2(lowerBound <= dt2, qPrintable(details));
    QVERIFY2(dt2 < upperBound, qPrintable(details));
    QVERIFY2(lowerBound <= dt3, qPrintable(details));
    QVERIFY2(dt3 < upperBound, qPrintable(details));

    QVERIFY(dt1.timeSpec() == Qt::UTC);
    QVERIFY(dt2.timeSpec() == Qt::LocalTime);
    QVERIFY(dt3.timeSpec() == Qt::UTC);
}

void tst_QDateTime::currentDateTimeUtc2()
{
    QDateTime local, utc;
    qint64 msec;

    // check that we got all down to the same milliseconds
    int i = 20;
    bool ok = false;
    do {
        local = QDateTime::currentDateTime();
        utc = QDateTime::currentDateTimeUtc();
        msec = QDateTime::currentMSecsSinceEpoch();
        ok = local.time().msec() == utc.time().msec()
            && utc.time().msec() == (msec % 1000);
    } while (--i && !ok);

    if (!i)
        QSKIP("Failed to get the dates within 1 ms of each other", SkipAll);

    // seconds and milliseconds should be the same:
    QCOMPARE(utc.time().second(), local.time().second());
    QCOMPARE(utc.time().msec(), local.time().msec());
    QCOMPARE(msec % 1000, qint64(local.time().msec()));
    QCOMPARE(msec / 1000 % 60, qint64(local.time().second()));

    // the two dates should be equal, actually
    QCOMPARE(local.toUTC(), utc);
    QCOMPARE(utc.toLocalTime(), local);

    // and finally, the time_t should equal our number
    QCOMPARE(qint64(utc.toTime_t()), msec / 1000);
    QCOMPARE(qint64(local.toTime_t()), msec / 1000);
    QCOMPARE(utc.toMSecsSinceEpoch(), msec);
    QCOMPARE(local.toMSecsSinceEpoch(), msec);
}

void tst_QDateTime::toTime_t_data()
{
    QTest::addColumn<QString>("dateTimeStr");
    QTest::addColumn<bool>("res");

    QTest::newRow( "data1" ) << str( 1800, 1, 1, 12, 0, 0 ) << false;
    QTest::newRow( "data2" ) << str( 1969, 1, 1, 12, 0, 0 ) << false;
    QTest::newRow( "data3" ) << str( 2002, 1, 1, 12, 0, 0 ) << true;
    QTest::newRow( "data4" ) << str( 2002, 6, 1, 12, 0, 0 ) << true;
    QTest::newRow( "data5" ) << QString("INVALID") << false;
    QTest::newRow( "data6" ) << str( 2038, 1, 1, 12, 0, 0 ) << true;
    QTest::newRow( "data7" ) << str( 2063, 4, 5, 12, 0, 0 ) << true; // the day of First Contact
    QTest::newRow( "data8" ) << str( 2107, 1, 1, 12, 0, 0 )
                          << bool( sizeof(uint) > 32 && sizeof(time_t) > 32 );
}

void tst_QDateTime::toTime_t()
{
    QFETCH( QString, dateTimeStr );
    QDateTime datetime = dt( dateTimeStr );

    uint asTime_t = datetime.toTime_t();
    QFETCH( bool, res );
    if (res) {
        QVERIFY( asTime_t != (uint)-1 );
    } else {
        QVERIFY( asTime_t == (uint)-1 );
    }

    if ( asTime_t != (uint) -1 ) {
        QDateTime datetime2 = QDateTime::fromTime_t( asTime_t );
        QCOMPARE(datetime, datetime2);
    }
}

void tst_QDateTime::task_137698()
{
    // This bug is caused by QDateTime knowing more than it lets show
    // Internally, if it knows, QDateTime stores a flag indicating if the time is
    // DST or not. If it doesn't, it sets to "LocalUnknown"
    //
    // The problem happens if you start with a date in DST and then move to a date
    // outside it (or vice-versa). Some functions did not reset the flag, which caused
    // weird 1-hour jumps in time when addSecs() was called.

    // WARNING: This test only works if there's a Daylight Savings Time change
    // in the current locale between 2006-11-06 and 2006-10-16
    // This is true for Central European Time

    if (!europeanTimeZone)
        QSKIP("Not tested with timezone other than Central European (CET/CEST)", SkipAll);

    QDateTime dt = QDateTime(QDate(2006, 11, 6), QTime(0, 0, 0), Qt::LocalTime);
    dt.setDate(QDate(2006, 10, 16));
    dt = dt.addSecs(1);
    QCOMPARE(dt.date(), QDate(2006, 10, 16));
    QCOMPARE(dt.time(), QTime(0, 0, 1));

    // now using fromTime_t
    dt = QDateTime::fromTime_t(1162767600); // 2006-11-06 00:00:00 +0100
    dt.setDate(QDate(2006, 10, 16));
    dt = dt.addSecs (1);
    QCOMPARE(dt.date(), QDate(2006, 10, 16));
    QCOMPARE(dt.time(), QTime(0, 0, 1));
}

void tst_QDateTime::operator_eqeq()
{
    QDateTime dt1(QDate(2004, 1, 2), QTime(2, 2, 3), Qt::LocalTime);
    QDateTime dt2(QDate(2004, 1, 2), QTime(1, 2, 3), Qt::UTC);

    QVERIFY(dt1 == dt1);
    QVERIFY(!(dt1 != dt1));

    QVERIFY(dt2 == dt2);
    QVERIFY(!(dt2 != dt2));

    QVERIFY(dt1 != QDateTime::currentDateTime());
    QVERIFY(dt2 != QDateTime::currentDateTime());

    QVERIFY(dt1.toUTC() == dt1.toUTC());

    if (europeanTimeZone) {
        QVERIFY(dt1.toUTC() == dt2);
        QVERIFY(dt1 == dt2.toLocalTime());
    }
}

void tst_QDateTime::toString_strformat()
{
    // Most tests are in QLocale, just test that the api works.
    QDate testDate(2013, 1, 1);
    QTime testTime(1, 2, 3);
    QDateTime testDateTime(testDate, testTime, Qt::UTC);
    QCOMPARE(testDate.toString("yyyy-MM-dd"), QString("2013-01-01"));
    QCOMPARE(testTime.toString("hh:mm:ss"), QString("01:02:03"));
    QCOMPARE(testDateTime.toString("yyyy-MM-dd hh:mm:ss t"), QString("2013-01-01 01:02:03 UTC"));
}

void tst_QDateTime::fromStringDateFormat_data()
{
    QTest::addColumn<QString>("dateTimeStr");
    QTest::addColumn<Qt::DateFormat>("dateFormat");
    QTest::addColumn<QDateTime>("expected");
    QTest::addColumn<Qt::TimeSpec>("timeSpec");

    // Test Qt::TextDate format.
    QTest::newRow("text date") << QString::fromLatin1("Tue Jun 17 08:00:10 2003")
        << Qt::TextDate << QDateTime(QDate(2003, 6, 17), QTime(8, 0, 10, 0), Qt::LocalTime);
    QTest::newRow("text date Year 0999") << QString::fromLatin1("Tue Jun 17 08:00:10 0999")
        << Qt::TextDate << QDateTime(QDate(999, 6, 17), QTime(8, 0, 10, 0), Qt::LocalTime);
    QTest::newRow("text date Year 999") << QString::fromLatin1("Tue Jun 17 08:00:10 999")
        << Qt::TextDate << QDateTime(QDate(999, 6, 17), QTime(8, 0, 10, 0), Qt::LocalTime);
    QTest::newRow("text date Year 12345") << QString::fromLatin1("Tue Jun 17 08:00:10 12345")
        << Qt::TextDate << QDateTime(QDate(12345, 6, 17), QTime(8, 0, 10, 0), Qt::LocalTime);
    QTest::newRow("text date Year -4712") << QString::fromLatin1("Tue Jan 1 00:01:02 -4712")
        << Qt::TextDate << QDateTime(QDate(-4712, 1, 1), QTime(0, 1, 2, 0), Qt::LocalTime);
    QTest::newRow("text data0") << QString::fromLatin1("Thu Jan 1 00:00:00 1970")
        << Qt::TextDate << QDateTime(QDate(1970, 1, 1), QTime(0, 0, 0), Qt::LocalTime);
    QTest::newRow("text data1") << QString::fromLatin1("Thu Jan 2 12:34 1970")
        << Qt::TextDate << QDateTime(QDate(1970, 1, 2), QTime(12, 34, 0), Qt::LocalTime);
    QTest::newRow("text data2") << QString::fromLatin1("Thu Jan 1 00 1970")
        << Qt::TextDate << invalidDateTime();
    QTest::newRow("text data3") << QString::fromLatin1("Thu Jan 1 00:00:00:00 1970")
        << Qt::TextDate << invalidDateTime();
    QTest::newRow("text data4") << QString::fromLatin1("Thu 1. Jan 00:00:00 1970")
        << Qt::TextDate << QDateTime(QDate(1970, 1, 1), QTime(0, 0), Qt::LocalTime);
    QTest::newRow("text data5") << QString::fromLatin1(" Thu   Jan   1    00:00:00    1970  ")
        << Qt::TextDate << QDateTime(QDate(1970, 1, 1), QTime(0, 0, 0), Qt::LocalTime);
    QTest::newRow("text data6") << QString::fromLatin1("Thu Jan 1 00:00:00")
        << Qt::TextDate << invalidDateTime();
    QTest::newRow("text data7") << QString::fromLatin1("Thu Jan 1 1970 00:00:00")
        << Qt::TextDate << QDateTime(QDate(1970, 1, 1), QTime(0, 0, 0), Qt::LocalTime);
    QTest::newRow("text data8") << QString::fromLatin1("Thu Jan 1 00:12:34 1970 GMT+foo")
        << Qt::TextDate << invalidDateTime();
    QTest::newRow("text data9") << QString::fromLatin1("Thu Jan 1 00:12:34 1970 GMT")
        << Qt::TextDate << QDateTime(QDate(1970, 1, 1), QTime(0, 12, 34), Qt::UTC);
    QTest::newRow("text data10") << QString::fromLatin1("Thu Jan 1 00:12:34 1970 GMT-0300")
        << Qt::TextDate << QDateTime(QDate(1970, 1, 1), QTime(3, 12, 34), Qt::UTC);
    QTest::newRow("text data11") << QString::fromLatin1("Thu Jan 1 00:12:34 1970 GMT+0300")
        << Qt::TextDate << QDateTime(QDate(1969, 12, 31), QTime(21, 12, 34), Qt::UTC);
    QTest::newRow("text data12") << QString::fromLatin1("Thu Jan 1 00:12:34 1970 gmt")
        << Qt::TextDate << QDateTime(QDate(1970, 1, 1), QTime(0, 12, 34), Qt::UTC);
    QTest::newRow("text data13") << QString::fromLatin1("Thu Jan 1 1970 00:12:34 GMT+0100")
        << Qt::TextDate << QDateTime(QDate(1969, 12, 31), QTime(23, 12, 34), Qt::UTC);
    QTest::newRow("text empty") << QString::fromLatin1("")
        << Qt::TextDate << invalidDateTime();
    QTest::newRow("text too many parts") << QString::fromLatin1("Thu Jan 1 00:12:34 1970 gmt +0100")
        << Qt::TextDate << invalidDateTime();
    QTest::newRow("text invalid month name") << QString::fromLatin1("Thu Jaz 1 1970 00:12:34")
        << Qt::TextDate << invalidDateTime();
    QTest::newRow("text invalid date") << QString::fromLatin1("Thu Jan 32 1970 00:12:34")
        << Qt::TextDate << invalidDateTime();
    QTest::newRow("text invalid day #1") << QString::fromLatin1("Thu Jan XX 1970 00:12:34")
        << Qt::TextDate << invalidDateTime();
    QTest::newRow("text invalid day #2") << QString::fromLatin1("Thu X. Jan 00:00:00 1970")
        << Qt::TextDate << invalidDateTime();
    QTest::newRow("text invalid day #3") << QString::fromLatin1("Thu 1 Jan 00:00:00 1970")
        << Qt::TextDate << invalidDateTime();
    QTest::newRow("text invalid year #1") << QString::fromLatin1("Thu 1. Jan 00:00:00 19X0")
        << Qt::TextDate << invalidDateTime();
    QTest::newRow("text invalid year #2") << QString::fromLatin1("Thu 1. Jan 19X0 00:00:00")
        << Qt::TextDate << invalidDateTime();
    QTest::newRow("text invalid hour") << QString::fromLatin1("Thu 1. Jan 1970 0X:00:00")
        << Qt::TextDate << invalidDateTime();
    QTest::newRow("text invalid minute") << QString::fromLatin1("Thu 1. Jan 1970 00:0X:00")
        << Qt::TextDate << invalidDateTime();
    QTest::newRow("text invalid second") << QString::fromLatin1("Thu 1. Jan 1970 00:00:0X")
        << Qt::TextDate << invalidDateTime();
    QTest::newRow("text invalid gmt specifier #1") << QString::fromLatin1("Thu 1. Jan 1970 00:00:00 DMT")
        << Qt::TextDate << invalidDateTime();
    QTest::newRow("text invalid gmt specifier #2") << QString::fromLatin1("Thu 1. Jan 1970 00:00:00 GMTx0200")
        << Qt::TextDate << invalidDateTime();
    QTest::newRow("text invalid gmt hour") << QString::fromLatin1("Thu 1. Jan 1970 00:00:00 GMT+0X00")
        << Qt::TextDate << invalidDateTime();
    QTest::newRow("text invalid gmt minute") << QString::fromLatin1("Thu 1. Jan 1970 00:00:00 GMT+000X")
        << Qt::TextDate << invalidDateTime();
    QTest::newRow("text second fraction") << QString::fromLatin1("Mon 6. May 2013 01:02:03.456")
        << Qt::TextDate << QDateTime(QDate(2013, 5, 6), QTime(1, 2, 3, 456));


    // Test Qt::ISODate format.
    QTest::newRow("ISO +01:00") << QString::fromLatin1("1987-02-13T13:24:51+01:00")
        << Qt::ISODate << QDateTime(QDate(1987, 2, 13), QTime(12, 24, 51), Qt::UTC);
    QTest::newRow("ISO +00:01") << QString::fromLatin1("1987-02-13T13:24:51+00:01")
        << Qt::ISODate << QDateTime(QDate(1987, 2, 13), QTime(13, 23, 51), Qt::UTC);
    QTest::newRow("ISO -01:00") << QString::fromLatin1("1987-02-13T13:24:51-01:00")
        << Qt::ISODate << QDateTime(QDate(1987, 2, 13), QTime(14, 24, 51), Qt::UTC);
    QTest::newRow("ISO -00:01") << QString::fromLatin1("1987-02-13T13:24:51-00:01")
        << Qt::ISODate << QDateTime(QDate(1987, 2, 13), QTime(13, 25, 51), Qt::UTC);
    QTest::newRow("ISO +0000") << QString::fromLatin1("1970-01-01T00:12:34+0000")
        << Qt::ISODate << QDateTime(QDate(1970, 1, 1), QTime(0, 12, 34), Qt::UTC);
    QTest::newRow("ISO +00:00") << QString::fromLatin1("1970-01-01T00:12:34+00:00")
        << Qt::ISODate << QDateTime(QDate(1970, 1, 1), QTime(0, 12, 34), Qt::UTC);
    // No time specified - defaults to Qt::LocalTime.
    QTest::newRow("ISO data3") << QString::fromLatin1("2002-10-01")
        << Qt::ISODate << QDateTime(QDate(2002, 10, 1), QTime(0, 0, 0, 0), Qt::LocalTime);
    QTest::newRow("ISO") << QString::fromLatin1("2005-06-28T07:57:30.0010000000Z")
        << Qt::ISODate << QDateTime(QDate(2005, 6, 28), QTime(7, 57, 30, 1), Qt::UTC);
    QTest::newRow("ISO with comma 1") << QString::fromLatin1("2005-06-28T07:57:30,0040000000Z")
        << Qt::ISODate << QDateTime(QDate(2005, 6, 28), QTime(7, 57, 30, 4), Qt::UTC);
    QTest::newRow("ISO with comma 2") << QString::fromLatin1("2005-06-28T07:57:30,0015Z")
        << Qt::ISODate << QDateTime(QDate(2005, 6, 28), QTime(7, 57, 30, 2), Qt::UTC);
    QTest::newRow("ISO with comma 3") << QString::fromLatin1("2005-06-28T07:57:30,0014Z")
        << Qt::ISODate << QDateTime(QDate(2005, 6, 28), QTime(7, 57, 30, 1), Qt::UTC);
    QTest::newRow("ISO with comma 4") << QString::fromLatin1("2005-06-28T07:57:30,1Z")
        << Qt::ISODate << QDateTime(QDate(2005, 6, 28), QTime(7, 57, 30, 100), Qt::UTC);
    QTest::newRow("ISO with comma 5") << QString::fromLatin1("2005-06-28T07:57:30,11")
        << Qt::ISODate << QDateTime(QDate(2005, 6, 28), QTime(7, 57, 30, 110), Qt::LocalTime);
    // 24:00:00 Should be next day according to ISO 8601 section 4.2.3.
    QTest::newRow("ISO 24:00") << QString::fromLatin1("2012-06-04T24:00:00")
        << Qt::ISODate << QDateTime(QDate(2012, 6, 5), QTime(0, 0, 0, 0), Qt::LocalTime);
    QTest::newRow("ISO 24:00 end of month") << QString::fromLatin1("2012-06-30T24:00:00")
        << Qt::ISODate << QDateTime(QDate(2012, 7, 1), QTime(0, 0, 0, 0), Qt::LocalTime);
    QTest::newRow("ISO 24:00 end of year") << QString::fromLatin1("2012-12-31T24:00:00")
        << Qt::ISODate << QDateTime(QDate(2013, 1, 1), QTime(0, 0, 0, 0), Qt::LocalTime);
    QTest::newRow("ISO 24:00, fract ms") << QString::fromLatin1("2012-01-01T24:00:00.000")
        << Qt::ISODate << QDateTime(QDate(2012, 1, 2), QTime(0, 0, 0, 0), Qt::LocalTime);
    QTest::newRow("ISO 24:00 end of year, fract ms") << QString::fromLatin1("2012-12-31T24:00:00.000")
        << Qt::ISODate << QDateTime(QDate(2013, 1, 1), QTime(0, 0, 0, 0), Qt::LocalTime);
    // Test fractional seconds.
    QTest::newRow("ISO .0 of a second (period)") << QString::fromLatin1("2012-01-01T08:00:00.0")
        << Qt::ISODate << QDateTime(QDate(2012, 1, 1), QTime(8, 0, 0, 0), Qt::LocalTime);
    QTest::newRow("ISO .00 of a second (period)") << QString::fromLatin1("2012-01-01T08:00:00.00")
        << Qt::ISODate << QDateTime(QDate(2012, 1, 1), QTime(8, 0, 0, 0), Qt::LocalTime);
    QTest::newRow("ISO .000 of a second (period)") << QString::fromLatin1("2012-01-01T08:00:00.000")
        << Qt::ISODate << QDateTime(QDate(2012, 1, 1), QTime(8, 0, 0, 0), Qt::LocalTime);
    QTest::newRow("ISO .1 of a second (comma)") << QString::fromLatin1("2012-01-01T08:00:00,1")
        << Qt::ISODate << QDateTime(QDate(2012, 1, 1), QTime(8, 0, 0, 100), Qt::LocalTime);
    QTest::newRow("ISO .99 of a second (comma)") << QString::fromLatin1("2012-01-01T08:00:00,99")
        << Qt::ISODate << QDateTime(QDate(2012, 1, 1), QTime(8, 0, 0, 990), Qt::LocalTime);
    QTest::newRow("ISO .998 of a second (comma)") << QString::fromLatin1("2012-01-01T08:00:00,998")
        << Qt::ISODate << QDateTime(QDate(2012, 1, 1), QTime(8, 0, 0, 998), Qt::LocalTime);
    QTest::newRow("ISO .999 of a second (comma)") << QString::fromLatin1("2012-01-01T08:00:00,999")
        << Qt::ISODate << QDateTime(QDate(2012, 1, 1), QTime(8, 0, 0, 999), Qt::LocalTime);
    QTest::newRow("ISO .3335 of a second (comma)") << QString::fromLatin1("2012-01-01T08:00:00,3335")
        << Qt::ISODate << QDateTime(QDate(2012, 1, 1), QTime(8, 0, 0, 334), Qt::LocalTime);
    QTest::newRow("ISO .333333 of a second (comma)") << QString::fromLatin1("2012-01-01T08:00:00,333333")
        << Qt::ISODate << QDateTime(QDate(2012, 1, 1), QTime(8, 0, 0, 333), Qt::LocalTime);
    QTest::newRow("ISO .00009 of a second (period)") << QString::fromLatin1("2012-01-01T08:00:00.00009")
        << Qt::ISODate << QDateTime(QDate(2012, 1, 1), QTime(8, 0, 0, 0), Qt::LocalTime);
    QTest::newRow("ISO no fract specified") << QString::fromLatin1("2012-01-01T08:00:00.")
        << Qt::ISODate << QDateTime(QDate(2012, 1, 1), QTime(8, 0, 0, 0), Qt::LocalTime);
    // Test invalid characters (should ignore invalid characters at end of string).
    QTest::newRow("ISO invalid character at end") << QString::fromLatin1("2012-01-01T08:00:00!")
        << Qt::ISODate << QDateTime(QDate(2012, 1, 1), QTime(8, 0, 0, 0), Qt::LocalTime);
    QTest::newRow("ISO invalid character at front") << QString::fromLatin1("!2012-01-01T08:00:00")
        << Qt::ISODate << invalidDateTime();
    QTest::newRow("ISO invalid character both ends") << QString::fromLatin1("!2012-01-01T08:00:00!")
        << Qt::ISODate << invalidDateTime();
    QTest::newRow("ISO invalid character at front, 2 at back") << QString::fromLatin1("!2012-01-01T08:00:00..")
        << Qt::ISODate << invalidDateTime();
    QTest::newRow("ISO invalid character 2 at front") << QString::fromLatin1("!!2012-01-01T08:00:00")
        << Qt::ISODate << invalidDateTime();
    // Test fractional minutes.
    QTest::newRow("ISO .0 of a minute (period)") << QString::fromLatin1("2012-01-01T08:00.0")
        << Qt::ISODate << QDateTime(QDate(2012, 1, 1), QTime(8, 0, 0, 0), Qt::LocalTime);
    QTest::newRow("ISO .8 of a minute (period)") << QString::fromLatin1("2012-01-01T08:00.8")
        << Qt::ISODate << QDateTime(QDate(2012, 1, 1), QTime(8, 0, 48, 0), Qt::LocalTime);
    QTest::newRow("ISO .99999 of a minute (period)") << QString::fromLatin1("2012-01-01T08:00.99999")
        << Qt::ISODate << QDateTime(QDate(2012, 1, 1), QTime(8, 0, 59, 999), Qt::LocalTime);
    QTest::newRow("ISO .0 of a minute (comma)") << QString::fromLatin1("2012-01-01T08:00,0")
        << Qt::ISODate << QDateTime(QDate(2012, 1, 1), QTime(8, 0, 0, 0), Qt::LocalTime);
    QTest::newRow("ISO .8 of a minute (comma)") << QString::fromLatin1("2012-01-01T08:00,8")
        << Qt::ISODate << QDateTime(QDate(2012, 1, 1), QTime(8, 0, 48, 0), Qt::LocalTime);
    QTest::newRow("ISO .99999 of a minute (comma)") << QString::fromLatin1("2012-01-01T08:00,99999")
        << Qt::ISODate << QDateTime(QDate(2012, 1, 1), QTime(8, 0, 59, 999), Qt::LocalTime);
    QTest::newRow("ISO empty") << QString::fromLatin1("") << Qt::ISODate << invalidDateTime();


    // Should be next day according to ISO 8601 section 4.2.3.
    QTest::newRow("ISO date 24:00") << QString::fromLatin1("2012-06-04T24:00:00")
        << Qt::ISODate << QDateTime(QDate(2012, 6, 5), QTime(0, 0, 0, 0)) << Qt::LocalTime;
    QTest::newRow("ISO date 24:00 end of month") << QString::fromLatin1("2012-06-30T24:00:00")
        << Qt::ISODate << QDateTime(QDate(2012, 7, 1), QTime(0, 0, 0, 0)) << Qt::LocalTime;
    QTest::newRow("ISO date 24:00 end of month and year") << QString::fromLatin1("2012-12-31T24:00:00")
        << Qt::ISODate << QDateTime(QDate(2013, 1, 1), QTime(0, 0, 0, 0)) << Qt::LocalTime;
}

void tst_QDateTime::fromStringDateFormat()
{
    QFETCH(QString, dateTimeStr);
    QFETCH(Qt::DateFormat, dateFormat);
    QFETCH(QDateTime, expected);
    QFETCH(Qt::TimeSpec, timeSpec);

    QDateTime dateTime = QDateTime::fromString(dateTimeStr, dateFormat);
    expected.setTimeSpec(timeSpec);
    QCOMPARE(dateTime, expected);
}

void tst_QDateTime::fromStringStringFormat_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("format");
    QTest::addColumn<QDate>("expected");

    //get localized names
    QString january = QDate::longMonthName(1);
    QString february = QDate::longMonthName(2);
    QString march = QDate::longMonthName(3);
    QString august = QDate::longMonthName(8);
    QString mon = QDate::shortDayName(1);
    QString monday = QDate::longDayName(1);
    QString tuesday = QDate::longDayName(2);
    QString wednesday = QDate::longDayName(3);
    QString thursday = QDate::longDayName(4);
    QString friday = QDate::longDayName(5);
    QString saturday = QDate::longDayName(6);
    QString sunday = QDate::longDayName(7);

    QTest::newRow("data0") << QString("") << QString("") << defDate();
    QTest::newRow("data1") << QString(" ") << QString("") << invalidDate();
    QTest::newRow("data2") << QString(" ") << QString(" ") << defDate();
    QTest::newRow("data3") << QString("-%$%#") << QString("$*(#@") << invalidDate();
    QTest::newRow("data4") << QString("d") << QString("'d'") << defDate();
    QTest::newRow("data5") << QString("101010") << QString("dMyy") << QDate(1910, 10, 10);
    QTest::newRow("data6") << QString("101010b") << QString("dMyy") << invalidDate();
    QTest::newRow("data7") << january << QString("MMMM") << defDate();
    QTest::newRow("data8") << QString("ball") << QString("balle") << invalidDate();
    QTest::newRow("data9") << QString("balleh") << QString("balleh") << defDate();
    QTest::newRow("data10") << QString("10.01.1") << QString("M.dd.d") << QDate(defDate().year(), 10, 1);
    QTest::newRow("data11") << QString("-1.01.1") << QString("M.dd.d") << invalidDate();
    QTest::newRow("data12") << QString("11010") << QString("dMMyy") << invalidDate();
    QTest::newRow("data13") << QString("-2") << QString("d") << invalidDate();
    QTest::newRow("data14") << QString("132") << QString("Md") << invalidDate();
    QTest::newRow("data15") << february << QString("MMMM") << QDate(defDate().year(), 2, 1);

    QString date = mon + " " + august + " 8 2005";
    QTest::newRow("data16") << date << QString("ddd MMMM d yyyy") << QDate(2005, 8, 8);
    QTest::newRow("data17") << QString("2000:00") << QString("yyyy:yy") << QDate(2000, 1, 1);
    QTest::newRow("data18") << QString("1999:99") << QString("yyyy:yy") << QDate(1999, 1, 1);
    QTest::newRow("data19") << QString("2099:99") << QString("yyyy:yy") << QDate(2099, 1, 1);
    QTest::newRow("data20") << QString("2001:01") << QString("yyyy:yy") << QDate(2001, 1, 1);
    QTest::newRow("data21") << QString("99") << QString("yy") << QDate(1999, 1, 1);
    QTest::newRow("data22") << QString("01") << QString("yy") << QDate(1901, 1, 1);

    QTest::newRow("data23") << monday << QString("dddd") << QDate(1900, 1, 1);
    QTest::newRow("data24") << tuesday << QString("dddd") << QDate(1900, 1, 2);
    QTest::newRow("data25") << wednesday << QString("dddd") << QDate(1900, 1, 3);
    QTest::newRow("data26") << thursday << QString("dddd") << QDate(1900, 1, 4);
    QTest::newRow("data26") << friday << QString("dddd") << QDate(1900, 1, 5);
    QTest::newRow("data27") << saturday << QString("dddd") << QDate(1900, 1, 6);
    QTest::newRow("data28") << sunday << QString("dddd") << QDate(1900, 1, 7);

    QTest::newRow("data29") << monday + " 2006" << QString("dddd yyyy") << QDate(2006, 1, 2);
    QTest::newRow("data30") << tuesday + " 2006" << QString("dddd yyyy") << QDate(2006, 1, 3);
    QTest::newRow("data31") << wednesday + " 2006" << QString("dddd yyyy") << QDate(2006, 1, 4);
    QTest::newRow("data32") << thursday + " 2006" << QString("dddd yyyy") << QDate(2006, 1, 5);
    QTest::newRow("data33") << friday + " 2006" << QString("dddd yyyy") << QDate(2006, 1, 6);
    QTest::newRow("data34") << saturday + " 2006" << QString("dddd yyyy") << QDate(2006, 1, 7);
    QTest::newRow("data35") << sunday + " 2006" << QString("dddd yyyy") << QDate(2006, 1, 1);

    QTest::newRow("data36 ") << tuesday + " 2007 " + march << QString("dddd yyyy MMMM") << QDate(2007, 3, 6);

}


void tst_QDateTime::fromStringStringFormat()
{
    QFETCH(QString, string);
    QFETCH(QString, format);
    QFETCH(QDate, expected);

    QDate dt = QDate::fromString(string, format);
    QCOMPARE(dt, expected);
}

void tst_QDateTime::fromString_LOCALE_ILDATE()
{
 #ifdef Q_OS_WIN
    QString date1 = QLatin1String("Sun 1. Dec 13:02:00 1974");
    QString date2 = QLatin1String("Sun Dec 1 13:02:00 1974");

    QDateTime ref(QDate(1974, 12, 1), QTime(13, 2));
    QCOMPARE(ref, QDateTime::fromString(date2, Qt::TextDate));
    QCOMPARE(ref, QDateTime::fromString(date1, Qt::TextDate));
#else
    QSKIP("Windows only", SkipAll);
#endif
}

void tst_QDateTime::fromStringToStringLocale_data()
{
    QTest::addColumn<QDateTime>("dateTime");
    QTest::newRow("data0") << QDateTime(QDate(1999, 1, 18), QTime(11, 49, 00));
}

void tst_QDateTime::fromStringToStringLocale()
{
    QFETCH(QDateTime, dateTime);

    QLocale def;
    QLocale::setDefault(QLocale(QLocale::French, QLocale::France));

    QCOMPARE(QDateTime::fromString(dateTime.toString(Qt::DefaultLocaleShortDate), Qt::DefaultLocaleShortDate), dateTime);
    QCOMPARE(QDateTime::fromString(dateTime.toString(Qt::SystemLocaleShortDate), Qt::SystemLocaleShortDate), dateTime);

    // obsolete
    QCOMPARE(QDateTime::fromString(dateTime.toString(Qt::SystemLocaleDate), Qt::SystemLocaleDate), dateTime);
    QCOMPARE(QDateTime::fromString(dateTime.toString(Qt::LocaleDate), Qt::LocaleDate), dateTime);

    QLocale::setDefault(def);
}

void tst_QDateTime::offsetFromUtc()
{
    /* Check default value. */
    QCOMPARE(QDateTime().offsetFromUtc(), 0);

    // Offset constructor
    QDateTime dt1(QDate(2013, 1, 1), QTime(1, 0, 0), Qt::OffsetFromUTC, 60 * 60);
    QCOMPARE(dt1.offsetFromUtc(), 60 * 60);
    dt1 = QDateTime(QDate(2013, 1, 1), QTime(1, 0, 0), Qt::OffsetFromUTC, -60 * 60);
    QCOMPARE(dt1.offsetFromUtc(), -60 * 60);

    // UTC should be 0 offset
    QDateTime dt2(QDate(2013, 1, 1), QTime(0, 0, 0), Qt::UTC);
    QCOMPARE(dt2.offsetFromUtc(), 0);

    // LocalTime should vary
    if (europeanTimeZone) {
        // Time definitely in Standard Time so 1 hour ahead
        QDateTime dt3(QDate(2013, 1, 1), QTime(0, 0, 0), Qt::LocalTime);
        QCOMPARE(dt3.offsetFromUtc(), 1 * 60 * 60);
        // Time definitely in Daylight Time so 2 hours ahead
        QDateTime dt4(QDate(2013, 6, 1), QTime(0, 0, 0), Qt::LocalTime);
        QCOMPARE(dt4.offsetFromUtc(), 2 * 60 * 60);
     } else {
         QSKIP("You must test using Central European (CET/CEST) time zone, e.g. TZ=Europe/Oslo", SkipAll);
     }
}

void tst_QDateTime::setOffsetFromUtc()
{
    /* Basic tests. */
    {
        QDateTime dt(QDateTime::currentDateTime());
        dt.setTimeSpec(Qt::LocalTime);

        dt.setOffsetFromUtc(0);
        QCOMPARE(dt.offsetFromUtc(), 0);
        QCOMPARE(dt.timeSpec(), Qt::UTC);

        dt.setOffsetFromUtc(-100);
        QCOMPARE(dt.offsetFromUtc(), -100);
        QCOMPARE(dt.timeSpec(), Qt::OffsetFromUTC);
    }

    /* Test detaching. */
    {
        QDateTime dt(QDateTime::currentDateTime());
        QDateTime dt2(dt);
        int offset2 = dt2.offsetFromUtc();

        dt.setOffsetFromUtc(501);

        QCOMPARE(dt.offsetFromUtc(), 501);
        QCOMPARE(dt2.offsetFromUtc(), offset2);
    }

    /* Check copying. */
    {
        QDateTime dt(QDateTime::currentDateTime());
        dt.setOffsetFromUtc(502);
        QCOMPARE(dt.offsetFromUtc(), 502);

        QDateTime dt2(dt);
        QCOMPARE(dt2.offsetFromUtc(), 502);
    }

    /* Check assignment. */
    {
        QDateTime dt(QDateTime::currentDateTime());
        dt.setOffsetFromUtc(502);
        QDateTime dt2;
        dt2 = dt;

        QCOMPARE(dt2.offsetFromUtc(), 502);
    }


    // Check spec persists
    QDateTime dt1(QDate(2013, 1, 1), QTime(0, 0, 0), Qt::OffsetFromUTC, 60 * 60);
    dt1.setMSecsSinceEpoch(123456789);
    QCOMPARE(dt1.timeSpec(), Qt::OffsetFromUTC);
    QCOMPARE(dt1.offsetFromUtc(), 60 * 60);
    dt1.setTime_t(123456789);
    QCOMPARE(dt1.timeSpec(), Qt::OffsetFromUTC);
    QCOMPARE(dt1.offsetFromUtc(), 60 * 60);

    // Check datastream serialises the offset seconds
    QByteArray tmp;
    {
        QDataStream ds(&tmp, QIODevice::WriteOnly);
        ds << dt1;
    }
    QDateTime dt2;
    {
        QDataStream ds(&tmp, QIODevice::ReadOnly);
        ds >> dt2;
    }
    QCOMPARE(dt2, dt1);
    QCOMPARE(dt2.timeSpec(), Qt::OffsetFromUTC);
    QCOMPARE(dt2.offsetFromUtc(), 60 * 60);
}

void tst_QDateTime::toOffsetFromUtc()
{
    QDateTime dt1(QDate(2013, 1, 1), QTime(0, 0, 0), Qt::UTC);

    QDateTime dt2 = dt1.toOffsetFromUtc(60 * 60);
    QCOMPARE(dt2, dt1);
    QCOMPARE(dt2.timeSpec(), Qt::OffsetFromUTC);
    QCOMPARE(dt2.date(), QDate(2013, 1, 1));
    QCOMPARE(dt2.time(), QTime(1, 0, 0));

    dt2 = dt1.toOffsetFromUtc(0);
    QCOMPARE(dt2, dt1);
    QCOMPARE(dt2.timeSpec(), Qt::UTC);
    QCOMPARE(dt2.date(), QDate(2013, 1, 1));
    QCOMPARE(dt2.time(), QTime(0, 0, 0));

    dt2 = dt1.toTimeSpec(Qt::OffsetFromUTC);
    QCOMPARE(dt2, dt1);
    QCOMPARE(dt2.timeSpec(), Qt::UTC);
    QCOMPARE(dt2.date(), QDate(2013, 1, 1));
    QCOMPARE(dt2.time(), QTime(0, 0, 0));
}

void tst_QDateTime::timeZoneAbbreviation()
{
    QDateTime dt1(QDate(2013, 1, 1), QTime(1, 0, 0), Qt::OffsetFromUTC, 60 * 60);
    QCOMPARE(dt1.timeZoneAbbreviation(), QString("UTC+01:00"));
    QDateTime dt2(QDate(2013, 1, 1), QTime(1, 0, 0), Qt::OffsetFromUTC, -60 * 60);
    QCOMPARE(dt2.timeZoneAbbreviation(), QString("UTC-01:00"));

    QDateTime dt3(QDate(2013, 1, 1), QTime(0, 0, 0), Qt::UTC);
    QCOMPARE(dt3.timeZoneAbbreviation(), QString("UTC"));

    // LocalTime should vary
    if (europeanTimeZone) {
        // Time definitely in Standard Time
        QDateTime dt4(QDate(2013, 1, 1), QTime(0, 0, 0), Qt::LocalTime);
#ifdef Q_OS_WIN
        QEXPECT_FAIL("", "Windows only returns long name (QTBUG-32759)", Continue);
#endif // Q_OS_WIN
        QCOMPARE(dt4.timeZoneAbbreviation(), QString("CET"));
        // Time definitely in Daylight Time
        QDateTime dt5(QDate(2013, 6, 1), QTime(0, 0, 0), Qt::LocalTime);
#ifdef Q_OS_WIN
        QEXPECT_FAIL("", "Windows only returns long name (QTBUG-32759)", Continue);
#endif // Q_OS_WIN
        QCOMPARE(dt5.timeZoneAbbreviation(), QString("CEST"));
    } else {
        QSKIP("You must test using Central European (CET/CEST) time zone, e.g. TZ=Europe/Oslo", SkipAll);
    }
}

void tst_QDateTime::getDate()
{
    {
    int y = -33, m = -44, d = -55;
    QDate date;
    date.getDate(&y, &m, &d);
    QVERIFY(date.year() == y);
    QVERIFY(date.month() == m);
    QVERIFY(date.day() == d);

    date.getDate(0, 0, 0);
    }

    {
    int y = -33, m = -44, d = -55;
    QDate date(1998, 5, 24);
    date.getDate(0, &m, 0);
    date.getDate(&y, 0, 0);
    date.getDate(0, 0, &d);

    QVERIFY(date.year() == y);
    QVERIFY(date.month() == m);
    QVERIFY(date.day() == d);
    }
}

void tst_QDateTime::fewDigitsInYear() const
{
    const QDateTime three(QDate(300, 10, 11), QTime());
    QCOMPARE(three.toString(QLatin1String("yyyy-MM-dd")), QString::fromLatin1("0300-10-11"));

    const QDateTime two(QDate(20, 10, 11), QTime());
    QCOMPARE(two.toString(QLatin1String("yyyy-MM-dd")), QString::fromLatin1("0020-10-11"));

    const QDateTime yyTwo(QDate(30, 10, 11), QTime());
    QCOMPARE(yyTwo.toString(QLatin1String("yy-MM-dd")), QString::fromLatin1("30-10-11"));

    const QDateTime yyOne(QDate(4, 10, 11), QTime());
    QCOMPARE(yyOne.toString(QLatin1String("yy-MM-dd")), QString::fromLatin1("04-10-11"));
}

void tst_QDateTime::printNegativeYear() const
{
    {
        QDateTime date(QDate(-20, 10, 11));
        QVERIFY(date.isValid());
        QCOMPARE(date.toString(QLatin1String("yyyy")), QString::fromLatin1("-0020"));
    }

    {
        QDateTime date(QDate(-3, 10, 11));
        QVERIFY(date.isValid());
        QCOMPARE(date.toString(QLatin1String("yyyy")), QString::fromLatin1("-0003"));
    }

    {
        QDateTime date(QDate(-400, 10, 11));
        QVERIFY(date.isValid());
        QCOMPARE(date.toString(QLatin1String("yyyy")), QString::fromLatin1("-0400"));
    }
}

void tst_QDateTime::roundtripGermanLocale() const
{
    /* This code path should not result in warnings. */
    const QDateTime theDateTime(QDateTime::currentDateTime());
    theDateTime.fromString(theDateTime.toString(Qt::TextDate), Qt::TextDate);
}

void tst_QDateTime::utcOffsetLessThan() const
{
    QDateTime dt1(QDate(2002, 10, 10), QTime(0, 0, 0));
    QDateTime dt2(dt1);

    dt1.setOffsetFromUtc(-(2 * 60 * 60)); // Minus two hours.
    dt2.setOffsetFromUtc(-(3 * 60 * 60)); // Minus three hours.

    QVERIFY(dt1 != dt2);
    QVERIFY(!(dt1 == dt2));
    QVERIFY(dt1 < dt2);
    QVERIFY(!(dt2 < dt1));
}

QTEST_APPLESS_MAIN(tst_QDateTime)
#include "tst_qdatetime.moc"

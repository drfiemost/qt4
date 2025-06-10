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
#include "private/qdatetime_p.h"

#include "qdatastream.h"
#include "qset.h"
#include "qlocale.h"
#include "qdatetime.h"
#include "qregexp.h"
#include "qdebug.h"
#if defined(Q_OS_WIN32)
#include <qt_windows.h>
#endif
#ifndef Q_WS_WIN
#include <clocale>
#endif

#include <cmath>
#include <ctime>

//#define QDATETIMEPARSER_DEBUG
#if defined (QDATETIMEPARSER_DEBUG) && !defined(QT_NO_DEBUG_STREAM)
#  define QDTPDEBUG qDebug() << QString("%1:%2").arg(__FILE__).arg(__LINE__)
#  define QDTPDEBUGN qDebug
#else
#  define QDTPDEBUG if (false) qDebug()
#  define QDTPDEBUGN if (false) qDebug
#endif

#if defined(Q_WS_MAC)
#include <private/qcore_mac_p.h>
#endif

#include  <algorithm>

QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC_WITH_ARGS(QSharedDataPointer<QDateTimePrivate>, defaultDateTimePrivate, (new QDateTimePrivate()))

enum {
    SECS_PER_DAY = 86400,
    MSECS_PER_DAY = 86400000,
    SECS_PER_HOUR = 3600,
    MSECS_PER_HOUR = 3600000,
    SECS_PER_MIN = 60,
    MSECS_PER_MIN = 60000,
    TIME_T_MAX = 2145916799,  // int maximum 2037-12-31T23:59:59 UTC
    JULIAN_DAY_FOR_EPOCH = 2440588 // result of julianDayFromGregorianDate(1970, 1, 1)
};

static inline QDate fixedDate(int y, int m, int d)
{
    QDate result(y, m, 1);
    result.setDate(y, m, std::min(d, result.daysInMonth()));
    return result;
}

/*
  Division, rounding down (rather than towards zero).

  From C++11 onwards, integer division is defined to round towards zero, so weMore actions
  can rely on that when implementing this.  This is only used with denominator b
  > 0, so we only have to treat negative numerator, a, specially.
 */
static inline qint64 floordiv(qint64 a, int b)
{
    return (a - (a < 0 ? b - 1 : 0)) / b;
}

static inline int floordiv(int a, int b)
{
    return (a - (a < 0 ? b - 1 : 0)) / b;
}

static inline qint64 julianDayFromDate(int year, int month, int day)
{
    // Adjust for no year 0
    if (year < 0)
        ++year;

/*
 * Math from The Calendar FAQ at http://www.tondering.dk/claus/cal/julperiod.php
 * This formula is correct for all julian days, when using mathematical integer
 * division (round to negative infinity), not c++11 integer division (round to zero)
 */
    int    a = floordiv(14 - month, 12);
    qint64 y = (qint64)year + 4800 - a;
    int    m = month + 12 * a - 3;
    return day + floordiv(153 * m + 2, 5) + 365 * y + floordiv(y, 4) - floordiv(y, 100) + floordiv(y, 400) - 32045;
}

struct ParsedDate
{
    int year, month, day;
};

static ParsedDate getDateFromJulianDay(qint64 julianDay)
{
/*
 * Math from The Calendar FAQ at http://www.tondering.dk/claus/cal/julperiod.php
 * This formula is correct for all julian days, when using mathematical integer
 * division (round to negative infinity), not c++11 integer division (round to zero)
 */
    qint64 a = julianDay + 32044;
    qint64 b = floordiv(4 * a + 3, 146097);
    int    c = a - floordiv(146097 * b, 4);

    int    d = floordiv(4 * c + 3, 1461);
    int    e = c - floordiv(1461 * d, 4);
    int    m = floordiv(5 * e + 2, 153);

    int    day = e - floordiv(153 * m + 2, 5) + 1;
    int    month = m + 3 - 12 * floordiv(m, 10);
    int    year = 100 * b + d - 4800 + floordiv(m, 10);

    // Adjust for no year 0
    if (year <= 0)
        --year ;

    const ParsedDate result = { year, month, day };
    return result;
}


static const char monthDays[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

#ifndef QT_NO_TEXTDATE
static const char qt_shortMonthNames[][4] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

static int qt_monthNumberFromShortName(const QString &shortName)
{
    for (unsigned int i = 0; i < sizeof(qt_shortMonthNames) / sizeof(qt_shortMonthNames[0]); ++i) {
        if (shortName == QLatin1String(qt_shortMonthNames[i]))
            return i + 1;
    }
    return -1;
}

static int fromShortMonthName(const QString &monthName)
{
    // Assume that English monthnames are the default
    int month = qt_monthNumberFromShortName(monthName);
    if (month != -1)
        return month;
    // If English names can't be found, search the localized ones
    for (int i = 1; i <= 12; ++i) {
        if (monthName == QDate::shortMonthName(i))
            return i;
    }
    return -1;
}
#endif // QT_NO_TEXTDATE

// Return offset in [+-]HH:MM format
// Qt::ISODate puts : between the hours and minutes, but Qt:TextDate does not
static QString toOffsetString(Qt::DateFormat format, int offset)
{
    return QString::asprintf("%c%02d%s%02d",
                             offset >= 0 ? '+' : '-',
                             std::abs(offset) / SECS_PER_HOUR,
                             // Qt::ISODate puts : between the hours and minutes, but Qt:TextDate does not:
                             format == Qt::TextDate ? "" : ":",
                             (std::abs(offset) / 60) % 60);
}

// Parse offset in [+-]HH[:]MM format
static int fromOffsetString(const QString &offsetString, bool *valid)
{
    *valid = false;

    const int size = offsetString.size();
    if (size < 2 || size > 6)
        return 0;

    // sign will be +1 for a positive and -1 for a negative offset
    int sign;

    // First char must be + or -
    const QChar signChar = offsetString.at(0);
    if (signChar == QLatin1Char('+'))
        sign = 1;
    else if (signChar == QLatin1Char('-'))
        sign = -1;
    else
        return 0;

    // Split the hour and minute parts
    const QString time = offsetString.mid(1);
    int hhLen = time.indexOf(QLatin1Char(':'));
    int mmIndex;
    if (hhLen == -1)
        mmIndex = hhLen = 2; // [+-]HHmm or [+-]HH format
    else
        mmIndex = hhLen + 1;

    const QString hhRef = time.left(hhLen);
    bool ok = false;
    const int hour = hhRef.toInt(&ok);
    if (!ok)
        return 0;

    const QString mmRef = time.mid(mmIndex);
    const int minute = mmRef.isEmpty() ? 0 : mmRef.toInt(&ok);
    if (!ok || minute < 0 || minute > 59)
        return 0;

    *valid = true;
    return sign * ((hour * 60) + minute) * 60;
}

// Returns the tzname, assume tzset has been called already
static QString qt_tzname(QDateTimePrivate::DaylightStatus daylightStatus)
{
    int isDst = (daylightStatus == QDateTimePrivate::DaylightTime) ? 1 : 0;
#if defined(_MSC_VER) && _MSC_VER >= 1400
    size_t s = 0;
    char name[512];
    if (_get_tzname(&s, name, 512, isDst))
        return QString();
    return QString::fromLocal8Bit(name);
#else
    return QString::fromLocal8Bit(tzname[isDst]);
#endif // Q_OS_WIN
}

// Calls the platform variant of mktime for the given date and time,
// and updates the date, time, daylightStatus and abbreviation with the returned values
// If the date falls outside the 1970 to 2037 range supported by mktime / time_t
// then null date/time will be returned, you should adjust the date first if
// you need a guaranteed result.
static qint64 qt_mktime(QDate *date, QTime *time, QDateTimePrivate::DaylightStatus *daylightStatus,
                        QString *abbreviation, bool *ok = nullptr)
{
    const qint64 msec = time->msec();
    int yy, mm, dd;
    date->getDate(&yy, &mm, &dd);
    std::tm local;
    std::memset(&local, 0, sizeof(local)); // tm_[wy]day plus any non-standard fields
    local.tm_sec = time->second();
    local.tm_min = time->minute();
    local.tm_hour = time->hour();
    local.tm_mday = dd;
    local.tm_mon = mm - 1;
    local.tm_year = yy - 1900;
    local.tm_wday = 0;
    local.tm_yday = 0;
    local.tm_isdst = -1;
    const std::time_t secsSinceEpoch = std::mktime(&local);
    if (secsSinceEpoch != (std::time_t)-1) {
        *date = QDate(local.tm_year + 1900, local.tm_mon + 1, local.tm_mday);
        *time = QTime(local.tm_hour, local.tm_min, local.tm_sec, msec);
        if (local.tm_isdst >= 1) {
            if (daylightStatus)
                *daylightStatus = QDateTimePrivate::DaylightTime;
            if (abbreviation)
                *abbreviation = qt_tzname(QDateTimePrivate::DaylightTime);
        } else if (local.tm_isdst == 0) {
            if (daylightStatus)
                *daylightStatus = QDateTimePrivate::StandardTime;
            if (abbreviation)
                *abbreviation = qt_tzname(QDateTimePrivate::StandardTime);
        } else {
            if (daylightStatus)
                *daylightStatus = QDateTimePrivate::UnknownDaylightTime;
            if (abbreviation)
                *abbreviation = qt_tzname(QDateTimePrivate::StandardTime);
        }
        if (ok)
            *ok = true;
    } else {
        *date = QDate();
        *time = QTime();
        if (daylightStatus)
            *daylightStatus = QDateTimePrivate::UnknownDaylightTime;
        if (abbreviation)
            *abbreviation = QString();
        if (ok)
            *ok = false;
    }
    return ((qint64)secsSinceEpoch * 1000) + msec;
}

/*****************************************************************************
  QDate member functions
 *****************************************************************************/

/*!
    \since 4.5

    \enum QDate::MonthNameType

    This enum describes the types of the string representation used
    for the month name.

    \value DateFormat This type of name can be used for date-to-string formatting.
    \value StandaloneFormat This type is used when you need to enumerate months or weekdays.
           Usually standalone names are represented in singular forms with
           capitalized first letter.
*/

/*!
    \class QDate
    \reentrant
    \brief The QDate class provides date functions.


    A QDate object contains a calendar date, i.e. year, month, and day
    numbers, in the Gregorian calendar. (see \l{QDate G and J} {Use of
    Gregorian and Julian Calendars} for dates prior to 15 October
    1582). It can read the current date from the system clock. It
    provides functions for comparing dates, and for manipulating
    dates. For example, it is possible to add and subtract days,
    months, and years to dates.

    A QDate object is typically created either by giving the year,
    month, and day numbers explicitly. Note that QDate interprets two
    digit years as is, i.e., years 0 - 99. A QDate can also be
    constructed with the static function currentDate(), which creates
    a QDate object containing the system clock's date.  An explicit
    date can also be set using setDate(). The fromString() function
    returns a QDate given a string and a date format which is used to
    interpret the date within the string.

    The year(), month(), and day() functions provide access to the
    year, month, and day numbers. Also, dayOfWeek() and dayOfYear()
    functions are provided. The same information is provided in
    textual format by the toString(), shortDayName(), longDayName(),
    shortMonthName(), and longMonthName() functions.

    QDate provides a full set of operators to compare two QDate
    objects where smaller means earlier, and larger means later.

    You can increment (or decrement) a date by a given number of days
    using addDays(). Similarly you can use addMonths() and addYears().
    The daysTo() function returns the number of days between two
    dates.

    The daysInMonth() and daysInYear() functions return how many days
    there are in this date's month and year, respectively. The
    isLeapYear() function indicates whether a date is in a leap year.

    \section1

    \target QDate G and J
    \section2 Use of Gregorian and Julian Calendars

    QDate uses the Gregorian calendar in all locales, beginning
    on the date 15 October 1582. For dates up to and including 4
    October 1582, the Julian calendar is used.  This means there is a
    10-day gap in the internal calendar between the 4th and the 15th
    of October 1582. When you use QDateTime for dates in that epoch,
    the day after 4 October 1582 is 15 October 1582, and the dates in
    the gap are invalid.

    The Julian to Gregorian changeover date used here is the date when
    the Gregorian calendar was first introduced, by Pope Gregory
    XIII. That change was not universally accepted and some localities
    only executed it at a later date (if at all).  QDateTime
    doesn't take any of these historical facts into account. If an
    application must support a locale-specific dating system, it must
    do so on its own, remembering to convert the dates using the
    Julian day.

    \section2 No Year 0

    There is no year 0. Dates in that year are considered invalid. The
    year -1 is the year "1 before Christ" or "1 before current era."
    The day before 1 January 1 CE is 31 December 1 BCE.

    \section2 Range of Valid Dates

    Dates are stored internally as a Julian Day number, an interger count ofMore actions
    every day in a contiguous range, with 24 November 4714 BCE in the Gregorian
    calendar being Julian Day 0 (1 January 4713 BCE in the Julian calendar).
    As well as being an efficient and accurate way of storing an absolute date,
    it is suitable for converting a Date into other calendar systems such as
    Hebrew, Islamic or Chinese. The Julian Day number can be obtained using
    QDate::toJulianDay() and can be set using QDate::fromJulianDay().

    The range of dates able to be stored by QDate as a Julian Day number is
    limited for convenience from std::numeric_limits<qint64>::min() / 2 to
    std::numeric_limits<qint64>::max() / 2, which on most platforms means
    from around 2.5 quadrillion BCE to around 2.5 quadrillion CE, effectively
    covering the full range of astronomical time. The range of Julian Days
    able to be accurately converted to and from valid YMD form Dates is
    restricted to 1 January 4800 BCE to 31 December 1400000 CE due to
    shortcomings in the available conversion formulas. Conversions outside this
    range are not guaranteed to be correct. This may change in the future.

    \sa QTime, QDateTime, QDateEdit, QDateTimeEdit, QCalendarWidget
*/

/*!
    \fn QDate::QDate()

    Constructs a null date. Null dates are invalid.

    \sa isNull(), isValid()
*/

/*!
    Constructs a date with year \a y, month \a m and day \a d.

    If the specified date is invalid, the date is not set and
    isValid() returns false.

    \warning Years 0 to 99 are interpreted as is, i.e., years
             0-99.

    \sa isValid()
*/

QDate::QDate(int y, int m, int d)
{
    setDate(y, m, d);
}


/*!
    \fn bool QDate::isNull() const

    Returns true if the date is null; otherwise returns false. A null
    date is invalid.

    \note The behavior of this function is equivalent to isValid().

    \sa isValid()
*/


/*!
    \fn bool isValid() const

    Returns true if this date is valid; otherwise returns false.

    \sa isNull()
*/


/*!
    Returns the year of this date. Negative numbers indicate years
    before 1 CE, such that year -44 is 44 BCE.

    \sa month(), day()
*/

int QDate::year() const
{
    if (isNull())
        return 0;

    return getDateFromJulianDay(jd).year;
}

/*!
    Returns the number corresponding to the month of this date, using
    the following convention:

    \list
    \i 1 = "January"
    \i 2 = "February"
    \i 3 = "March"
    \i 4 = "April"
    \i 5 = "May"
    \i 6 = "June"
    \i 7 = "July"
    \i 8 = "August"
    \i 9 = "September"
    \i 10 = "October"
    \i 11 = "November"
    \i 12 = "December"
    \endlist

    \sa year(), day()
*/

int QDate::month() const
{
    if (isNull())
        return 0;

    return getDateFromJulianDay(jd).month;
}

/*!
    Returns the day of the month (1 to 31) of this date.

    \sa year(), month(), dayOfWeek()
*/

int QDate::day() const
{
    if (isNull())
        return 0;

    return getDateFromJulianDay(jd).day;
}

/*!
    Returns the weekday (1 = Monday to 7 = Sunday) for this date.

    \sa day(), dayOfYear(), Qt::DayOfWeek
*/

int QDate::dayOfWeek() const
{
    if (isNull())
        return 0;

    if (jd >= 0)
        return (jd % 7) + 1;
    else
        return ((jd + 1) % 7) + 7;
}

/*!
    Returns the day of the year (1 to 365 or 366 on leap years) for
    this date.

    \sa day(), dayOfWeek()
*/

int QDate::dayOfYear() const
{
    if (isNull())
        return 0;

    return jd - julianDayFromDate(year(), 1, 1) + 1;
}

/*!
    Returns the number of days in the month (28 to 31) for this date.

    \sa day(), daysInYear()
*/

int QDate::daysInMonth() const
{
    if (isNull())
        return 0;

    const ParsedDate pd = getDateFromJulianDay(jd);
    if (pd.month == 2 && isLeapYear(pd.year))
        return 29;
    else
        return monthDays[pd.month];
}

/*!
    Returns the number of days in the year (365 or 366) for this date.

    \sa day(), daysInMonth()
*/

int QDate::daysInYear() const
{
    if (isNull())
        return 0;

    return isLeapYear(getDateFromJulianDay(jd).year) ? 366 : 365;
}

/*!
    Returns the week number (1 to 53), and stores the year in
    *\a{yearNumber} unless \a yearNumber is null (the default).

    Returns 0 if the date is invalid.

    In accordance with ISO 8601, weeks start on Monday and the first
    Thursday of a year is always in week 1 of that year. Most years
    have 52 weeks, but some have 53.

    *\a{yearNumber} is not always the same as year(). For example, 1
    January 2000 has week number 52 in the year 1999, and 31 December
    2002 has week number 1 in the year 2003.

    \sa isValid()
*/

int QDate::weekNumber(int *yearNumber) const
{
    if (!isValid())
        return 0;

    int year = QDate::year();
    int yday = dayOfYear();
    int wday = dayOfWeek();

    int week = (yday - wday + 10) / 7;

    if (week == 0) {
        // last week of previous year
        --year;
        week = (yday + 365 + (QDate::isLeapYear(year) ? 1 : 0) - wday + 10) / 7;
        Q_ASSERT(week == 52 || week == 53);
    } else if (week == 53) {
        // maybe first week of next year
        int w = (yday - 365 - (QDate::isLeapYear(year + 1) ? 1 : 0) - wday + 10) / 7;
        if (w > 0) {
            ++year;
            week = w;
        }
        Q_ASSERT(week == 53 || week == 1);
    }

    if (yearNumber != nullptr)
        *yearNumber = year;
    return week;
}

#ifndef QT_NO_TEXTDATE
/*!
    \since 4.5

    Returns the short name of the \a month for the representation specified
    by \a type.

    The months are enumerated using the following convention:

    \list
    \i 1 = "Jan"
    \i 2 = "Feb"
    \i 3 = "Mar"
    \i 4 = "Apr"
    \i 5 = "May"
    \i 6 = "Jun"
    \i 7 = "Jul"
    \i 8 = "Aug"
    \i 9 = "Sep"
    \i 10 = "Oct"
    \i 11 = "Nov"
    \i 12 = "Dec"
    \endlist

    The month names will be localized according to the system's default
    locale settings.

    \sa toString(), longMonthName(), shortDayName(), longDayName()
*/

QString QDate::shortMonthName(int month, QDate::MonthNameType type)
{
    switch (type) {
    case QDate::DateFormat:
        return QLocale::system().monthName(month, QLocale::ShortFormat);
    case QDate::StandaloneFormat:
        return QLocale::system().standaloneMonthName(month, QLocale::ShortFormat);
    default:
        break;
    }
    return QString();
}

/*!
    \since 4.5

    Returns the long name of the \a month for the representation specified
    by \a type.

    The months are enumerated using the following convention:

    \list
    \i 1 = "January"
    \i 2 = "February"
    \i 3 = "March"
    \i 4 = "April"
    \i 5 = "May"
    \i 6 = "June"
    \i 7 = "July"
    \i 8 = "August"
    \i 9 = "September"
    \i 10 = "October"
    \i 11 = "November"
    \i 12 = "December"
    \endlist

    The month names will be localized according to the system's default
    locale settings.

    \sa toString(), shortMonthName(), shortDayName(), longDayName()
*/

QString QDate::longMonthName(int month, MonthNameType type)
{
    switch (type) {
    case QDate::DateFormat:
        return QLocale::system().monthName(month, QLocale::LongFormat);
    case QDate::StandaloneFormat:
        return QLocale::system().standaloneMonthName(month, QLocale::LongFormat);
    default:
        break;
    }
    return QString();
}

/*!
    \since 4.5

    Returns the short name of the \a weekday for the representation specified
    by \a type.

    The days are enumerated using the following convention:

    \list
    \i 1 = "Mon"
    \i 2 = "Tue"
    \i 3 = "Wed"
    \i 4 = "Thu"
    \i 5 = "Fri"
    \i 6 = "Sat"
    \i 7 = "Sun"
    \endlist

    The day names will be localized according to the system's default
    locale settings.

    \sa toString(), shortMonthName(), longMonthName(), longDayName()
*/

QString QDate::shortDayName(int weekday, MonthNameType type)
{
    switch (type) {
    case QDate::DateFormat:
        return QLocale::system().dayName(weekday, QLocale::ShortFormat);
    case QDate::StandaloneFormat:
        return QLocale::system().standaloneDayName(weekday, QLocale::ShortFormat);
    default:
        break;
    }
    return QString();
}

/*!
    \since 4.5

    Returns the long name of the \a weekday for the representation specified
    by \a type.

    The days are enumerated using the following convention:

    \list
    \i 1 = "Monday"
    \i 2 = "Tuesday"
    \i 3 = "Wednesday"
    \i 4 = "Thursday"
    \i 5 = "Friday"
    \i 6 = "Saturday"
    \i 7 = "Sunday"
    \endlist

    The day names will be localized according to the system's default
    locale settings.

    \sa toString(), shortDayName(), shortMonthName(), longMonthName()
*/

QString QDate::longDayName(int weekday, MonthNameType type)
{
    switch (type) {
    case QDate::DateFormat:
        return QLocale::system().dayName(weekday, QLocale::LongFormat);
    case QDate::StandaloneFormat:
        return QLocale::system().standaloneDayName(weekday, QLocale::LongFormat);
    default:
        break;
    }
    return QString();
}

#endif //QT_NO_TEXTDATE

#ifndef QT_NO_DATESTRING

/*!
    \fn QString QDate::toString(Qt::DateFormat format) const

    \overload

    Returns the date as a string. The \a format parameter determines
    the format of the string.

    If the \a format is Qt::TextDate, the string is formatted in
    the default way. QDate::shortDayName() and QDate::shortMonthName()
    are used to generate the string, so the day and month names will
    be localized names using the default locale from the system. An
    example of this formatting is "Sat May 20 1995".

    If the \a format is Qt::ISODate, the string format corresponds
    to the ISO 8601 extended specification for representations of
    dates and times, taking the form YYYY-MM-DD, where YYYY is the
    year, MM is the month of the year (between 01 and 12), and DD is
    the day of the month between 01 and 31.

    If the \a format is Qt::SystemLocaleShortDate or
    Qt::SystemLocaleLongDate, the string format depends on the locale
    settings of the system. Identical to calling
    QLocale::system().toString(date, QLocale::ShortFormat) or
    QLocale::system().toString(date, QLocale::LongFormat).

    If the \a format is Qt::DefaultLocaleShortDate or
    Qt::DefaultLocaleLongDate, the string format depends on the
    default application locale. This is the locale set with
    QLocale::setDefault(), or the system locale if no default locale
    has been set. Identical to calling QLocale().toString(date,
    QLocale::ShortFormat) or QLocale().toString(date,
    QLocale::LongFormat).

    If the date is invalid, an empty string will be returned.

    \warning The Qt::ISODate format is only valid for years in the
    range 0 to 9999. This restriction may apply to locale-aware
    formats as well, depending on the locale settings.

    \sa shortDayName(), shortMonthName()
*/
QString QDate::toString(Qt::DateFormat format) const
{
    if (!isValid())
        return QString();

    ParsedDate pd;

    switch (format) {
    case Qt::SystemLocaleDate:
    case Qt::SystemLocaleShortDate:
        return QLocale::system().toString(*this, QLocale::ShortFormat);
    case Qt::SystemLocaleLongDate:
        return QLocale::system().toString(*this, QLocale::LongFormat);
    case Qt::LocaleDate:
    case Qt::DefaultLocaleShortDate:
        return QLocale().toString(*this, QLocale::ShortFormat);
    case Qt::DefaultLocaleLongDate:
        return QLocale().toString(*this, QLocale::LongFormat);
    default:
#ifndef QT_NO_TEXTDATE
    case Qt::TextDate:
        pd = getDateFromJulianDay(jd);
        return QString::fromUtf8("%1 %2 %3 %4").arg(shortDayName(dayOfWeek()))
                                               .arg(shortMonthName(pd.month))
                                               .arg(pd.day)
                                               .arg(pd.year);
#endif
    case Qt::ISODate:
        pd = getDateFromJulianDay(jd);
        if (pd.year >= 0 && pd.year <= 9999)
            return QString::asprintf("%04d-%02d-%02d", pd.year, pd.month, pd.day);
        else
            return QString();
    }
}

/*!
    Returns the date as a string. The \a format parameter determines
    the format of the result string.

    These expressions may be used:

    \table
    \header \i Expression \i Output
    \row \i d \i the day as number without a leading zero (1 to 31)
    \row \i dd \i the day as number with a leading zero (01 to 31)
    \row \i ddd
         \i the abbreviated localized day name (e.g. 'Mon' to 'Sun').
            Uses QDate::shortDayName().
    \row \i dddd
         \i the long localized day name (e.g. 'Monday' to 'Sunday').
            Uses QDate::longDayName().
    \row \i M \i the month as number without a leading zero (1 to 12)
    \row \i MM \i the month as number with a leading zero (01 to 12)
    \row \i MMM
         \i the abbreviated localized month name (e.g. 'Jan' to 'Dec').
            Uses QDate::shortMonthName().
    \row \i MMMM
         \i the long localized month name (e.g. 'January' to 'December').
            Uses QDate::longMonthName().
    \row \i yy \i the year as two digit number (00 to 99)
    \row \i yyyy \i the year as four digit number. If the year is negative,
            a minus sign is prepended in addition.
    \endtable

    All other input characters will be ignored. Any sequence of characters that
    are enclosed in singlequotes will be treated as text and not be used as an
    expression. Two consecutive singlequotes ("''") are replaced by a singlequote
    in the output.

    Example format strings (assuming that the QDate is the 20 July
    1969):

    \table
    \header \o Format            \o Result
    \row    \o dd.MM.yyyy        \o 20.07.1969
    \row    \o ddd MMMM d yy     \o Sun July 20 69
    \row    \o 'The day is' dddd \o The day is Sunday
    \endtable

    If the datetime is invalid, an empty string will be returned.

    \sa QDateTime::toString() QTime::toString(), QLocale::toString()

*/
QString QDate::toString(const QString& format) const
{
    return QLocale::system().toString(*this, format);
}
#endif //QT_NO_DATESTRING

/*!
    \fn bool setYMD(int y, int m, int d)

    \deprecated, use setDate() instead.

    Sets the date's year \a y, month \a m, and day \a d.

    If \a y is in the range 0 to 99, it is interpreted as 1900 to
    1999.

    Use setDate() instead.
*/

/*!
    \since 4.2

    Sets the date's \a year, \a month, and \a day. Returns true if
    the date is valid; otherwise returns false.

    If the specified date is invalid, the QDate object is set to be
    invalid.

    Note that any date before 4800 BCE or after about 1.4 million CE
    may not be accurately stored.

    \sa isValid()
*/
bool QDate::setDate(int year, int month, int day)
{
    if (isValid(year, month, day))
        jd = julianDayFromDate(year, month, day);
    else
        jd = nullJd();

    return isValid();
}

/*!
    \since 4.5

    Extracts the date's year, month, and day, and assigns them to
    *\a year, *\a month, and *\a day. The pointers may be null.

    \sa year(), month(), day(), isValid()
*/
void QDate::getDate(int *year, int *month, int *day) const
{
    ParsedDate pd = { 0, 0, 0 };
    if (isValid())
        pd = getDateFromJulianDay(jd);

    if (year)
        *year = pd.year;
    if (month)
        *month = pd.month;
    if (day)
        *day = pd.day;
}

/*!
    Returns a QDate object containing a date \a ndays later than the
    date of this object (or earlier if \a ndays is negative).

    \sa addMonths() addYears() daysTo()
*/

QDate QDate::addDays(qint64 ndays) const
{
    if (isNull())
        return QDate();

    // Due to limits on minJd() and maxJd() we know that any overflow
    // will be invalid and caught by fromJulianDay().
    return fromJulianDay(jd + ndays);
}

/*!
    Returns a QDate object containing a date \a nmonths later than the
    date of this object (or earlier if \a nmonths is negative).

    \note If the ending day/month combination does not exist in the
    resulting month/year, this function will return a date that is the
    latest valid date.

    \warning QDate has a date hole around the days introducing the
    Gregorian calendar (the days 5 to 14 October 1582, inclusive, do
    not exist). If the calculation ends in one of those days, QDate
    will return either October 4 or October 15.

    \sa addDays() addYears()
*/

QDate QDate::addMonths(int nmonths) const
{
    if (!isValid())
        return QDate();
    if (!nmonths)
        return *this;

    int old_y, y, m, d;
    {
        const ParsedDate pd = getDateFromJulianDay(jd);
        y = pd.year;
        m = pd.month;
        d = pd.day;
    }
    old_y = y;

    bool increasing = nmonths > 0;

    while (nmonths != 0) {
        if (nmonths < 0 && nmonths + 12 <= 0) {
            y--;
            nmonths+=12;
        } else if (nmonths < 0) {
            m+= nmonths;
            nmonths = 0;
            if (m <= 0) {
                --y;
                m += 12;
            }
        } else if (nmonths - 12 >= 0) {
            y++;
            nmonths -= 12;
        } else if (m == 12) {
            y++;
            m = 0;
        } else {
            m += nmonths;
            nmonths = 0;
            if (m > 12) {
                ++y;
                m -= 12;
            }
        }
    }

    // was there a sign change?
    if ((old_y > 0 && y <= 0) ||
        (old_y < 0 && y >= 0))
        // yes, adjust the date by +1 or -1 years
        y += increasing ? +1 : -1;

    // did we end up in the Gregorian/Julian conversion hole?
    if (y == 1582 && m == 10 && d > 4 && d < 15)
        d = increasing ? 15 : 4;

    return fixedDate(y, m, d);
}

/*!
    Returns a QDate object containing a date \a nyears later than the
    date of this object (or earlier if \a nyears is negative).

    \note If the ending day/month combination does not exist in the
    resulting year (i.e., if the date was Feb 29 and the final year is
    not a leap year), this function will return a date that is the
    latest valid date (that is, Feb 28).

    \sa addDays(), addMonths()
*/

QDate QDate::addYears(int nyears) const
{
    if (!isValid())
        return QDate();

    ParsedDate pd = getDateFromJulianDay(jd);

    int old_y = pd.year;
    pd.year += nyears;

    // was there a sign change?
    if ((old_y > 0 && pd.year <= 0) ||
        (old_y < 0 && pd.year >= 0))
        // yes, adjust the date by +1 or -1 years
        pd.year += nyears > 0 ? +1 : -1;

    return fixedDate(pd.year, pd.month, pd.day);
}

/*!
    Returns the number of days from this date to \a d (which is
    negative if \a d is earlier than this date).

    Example:
    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 0

    \sa addDays()
*/

qint64 QDate::daysTo(const QDate &d) const
{
    if (isNull() || d.isNull())
        return 0;

    // Due to limits on minJd() and maxJd() we know this will never overflow
    return d.jd - jd;
}


/*!
    \fn bool QDate::operator==(const QDate &d) const

    Returns true if this date is equal to \a d; otherwise returns
    false.

*/

/*!
    \fn bool QDate::operator!=(const QDate &d) const

    Returns true if this date is different from \a d; otherwise
    returns false.
*/

/*!
    \fn bool QDate::operator<(const QDate &d) const

    Returns true if this date is earlier than \a d; otherwise returns
    false.
*/

/*!
    \fn bool QDate::operator<=(const QDate &d) const

    Returns true if this date is earlier than or equal to \a d;
    otherwise returns false.
*/

/*!
    \fn bool QDate::operator>(const QDate &d) const

    Returns true if this date is later than \a d; otherwise returns
    false.
*/

/*!
    \fn bool QDate::operator>=(const QDate &d) const

    Returns true if this date is later than or equal to \a d;
    otherwise returns false.
*/

/*!
    \fn QDate::currentDate()
    Returns the current date, as reported by the system clock.

    \sa QTime::currentTime(), QDateTime::currentDateTime()
*/

#ifndef QT_NO_DATESTRING
/*!
    \fn QDate QDate::fromString(const QString &string, Qt::DateFormat format)

    Returns the QDate represented by the \a string, using the
    \a format given, or an invalid date if the string cannot be
    parsed.

    Note for Qt::TextDate: It is recommended that you use the
    English short month names (e.g. "Jan"). Although localized month
    names can also be used, they depend on the user's locale settings.
*/
QDate QDate::fromString(const QString& string, Qt::DateFormat format)
{
    if (string.isEmpty())
        return QDate();

    switch (format) {
    case Qt::SystemLocaleDate:
    case Qt::SystemLocaleShortDate:
        return QLocale::system().toDate(string, QLocale::ShortFormat);
    case Qt::SystemLocaleLongDate:
        return QLocale::system().toDate(string, QLocale::LongFormat);
    case Qt::LocaleDate:
    case Qt::DefaultLocaleShortDate:
        return QLocale().toDate(string, QLocale::ShortFormat);
    case Qt::DefaultLocaleLongDate:
        return QLocale().toDate(string, QLocale::LongFormat);
    default:
#ifndef QT_NO_TEXTDATE
    case Qt::TextDate: {
        QStringList parts = string.split(QLatin1Char(' '), QString::SkipEmptyParts);

        if (parts.count() != 4)
            return QDate();

        QString monthName = parts.at(1);
        const int month = fromShortMonthName(monthName);
        if (month == -1) {
            // Month name matches neither English nor other localised name.
            return QDate();
        }

        bool ok = false;
        int year = parts.at(3).toInt(&ok);
        if (!ok)
            return QDate();

        return QDate(year, month, parts.at(2).toInt());
        }
#endif // QT_NO_TEXTDATE
    case Qt::ISODate: {
        // Semi-strict parsing, must be long enough and have non-numeric separators
        if (string.size() < 10 || string.at(4).isDigit() || string.at(7).isDigit()
            || (string.size() > 10 && string.at(10).isDigit())) {
            return QDate();
        }
        const int year = string.mid(0, 4).toInt();
        if (year <= 0 || year > 9999)
            return QDate();
        return QDate(year, string.mid(5, 2).toInt(), string.mid(8, 2).toInt());
        }
    }
    return QDate();
}

/*!
    \fn QDate::fromString(const QString &string, const QString &format)

    Returns the QDate represented by the \a string, using the \a
    format given, or an invalid date if the string cannot be parsed.

    These expressions may be used for the format:

    \table
    \header \i Expression \i Output
    \row \i d \i The day as a number without a leading zero (1 to 31)
    \row \i dd \i The day as a number with a leading zero (01 to 31)
    \row \i ddd
         \i The abbreviated localized day name (e.g. 'Mon' to 'Sun').
            Uses QDate::shortDayName().
    \row \i dddd
         \i The long localized day name (e.g. 'Monday' to 'Sunday').
            Uses QDate::longDayName().
    \row \i M \i The month as a number without a leading zero (1 to 12)
    \row \i MM \i The month as a number with a leading zero (01 to 12)
    \row \i MMM
         \i The abbreviated localized month name (e.g. 'Jan' to 'Dec').
            Uses QDate::shortMonthName().
    \row \i MMMM
         \i The long localized month name (e.g. 'January' to 'December').
            Uses QDate::longMonthName().
    \row \i yy \i The year as two digit number (00 to 99)
    \row \i yyyy \i The year as four digit number. If the year is negative,
            a minus sign is prepended in addition.
    \endtable

    All other input characters will be treated as text. Any sequence
    of characters that are enclosed in single quotes will also be
    treated as text and will not be used as an expression. For example:

    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 1

    If the format is not satisfied, an invalid QDate is returned. The
    expressions that don't expect leading zeroes (d, M) will be
    greedy. This means that they will use two digits even if this
    will put them outside the accepted range of values and leaves too
    few digits for other sections. For example, the following format
    string could have meant January 30 but the M will grab two
    digits, resulting in an invalid date:

    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 2

    For any field that is not represented in the format the following
    defaults are used:

    \table
    \header \i Field  \i Default value
    \row    \i Year   \i 1900
    \row    \i Month  \i 1
    \row    \i Day    \i 1
    \endtable

    The following examples demonstrate the default values:

    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 3

    \sa QDateTime::fromString(), QTime::fromString(), QDate::toString(),
        QDateTime::toString(), QTime::toString()
*/

QDate QDate::fromString(const QString &string, const QString &format)
{
    QDate date;
#ifndef QT_BOOTSTRAPPED
    QDateTimeParser dt(QVariant::Date, QDateTimeParser::FromString);
    if (dt.parseFormat(format))
        dt.fromString(string, &date, nullptr);
#else
    Q_UNUSED(string);
    Q_UNUSED(format);
#endif
    return date;
}
#endif // QT_NO_DATESTRING

/*!
    \overload

    Returns true if the specified date (\a year, \a month, and \a
    day) is valid; otherwise returns false.

    Example:
    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 4

    \sa isNull(), setDate()
*/

bool QDate::isValid(int year, int month, int day)
{
    // there is no year 0 in the Julian calendarMore actions
    if (year == 0)
        return false;

    // passage from Julian to Gregorian calendar
    if (year == 1582 && month == 10 && day > 4 && day < 15)
        return false;

    return (day > 0 && month > 0 && month <= 12) &&
           (day <= monthDays[month] || (day == 29 && month == 2 && isLeapYear(year)));
}

/*!
    \fn bool QDate::isLeapYear(int year)

    Returns true if the specified \a year is a leap year; otherwise
    returns false.
*/

bool QDate::isLeapYear(int y)
{
    if (y < 1582) {
        if ( y < 1) {  // No year 0 in Julian calendar, so -1, -5, -9 etc are leap years
            ++y;
        }
        return y % 4 == 0;
    } else {
        return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0;
    }
}

/*! \fn static QDate QDate::fromJulianDay(int jd)

    Converts the Julian day \a jd to a QDate.

    \sa toJulianDay()
*/

/*! \fn int QDate::toJulianDay() const

    Converts the date to a Julian day.

    \sa fromJulianDay()
*/

/*****************************************************************************
  QTime member functions
 *****************************************************************************/

/*!
    \class QTime
    \reentrant

    \brief The QTime class provides clock time functions.


    A QTime object contains a clock time, i.e. the number of hours,
    minutes, seconds, and milliseconds since midnight. It can read the
    current time from the system clock and measure a span of elapsed
    time. It provides functions for comparing times and for
    manipulating a time by adding a number of milliseconds.

    QTime uses the 24-hour clock format; it has no concept of AM/PM.
    Unlike QDateTime, QTime knows nothing about time zones or
    daylight savings time (DST).

    A QTime object is typically created either by giving the number
    of hours, minutes, seconds, and milliseconds explicitly, or by
    using the static function currentTime(), which creates a QTime
    object that contains the system's local time. Note that the
    accuracy depends on the accuracy of the underlying operating
    system; not all systems provide 1-millisecond accuracy.

    The hour(), minute(), second(), and msec() functions provide
    access to the number of hours, minutes, seconds, and milliseconds
    of the time. The same information is provided in textual format by
    the toString() function.

    QTime provides a full set of operators to compare two QTime
    objects. One time is considered smaller than another if it is
    earlier than the other.

    The time a given number of seconds or milliseconds later than a
    given time can be found using the addSecs() or addMSecs()
    functions. Correspondingly, the number of seconds or milliseconds
    between two times can be found using secsTo() or msecsTo().

    QTime can be used to measure a span of elapsed time using the
    start(), restart(), and elapsed() functions.

    \sa QDate, QDateTime
*/

/*!
    \fn QTime::QTime()

    Constructs a null time object. A null time can be a QTime(0, 0, 0, 0)
    (i.e., midnight) object, except that isNull() returns true and isValid()
    returns false.

    \sa isNull(), isValid()
*/

/*!
    Constructs a time with hour \a h, minute \a m, seconds \a s and
    milliseconds \a ms.

    \a h must be in the range 0 to 23, \a m and \a s must be in the
    range 0 to 59, and \a ms must be in the range 0 to 999.

    \sa isValid()
*/

QTime::QTime(int h, int m, int s, int ms)
{
    setHMS(h, m, s, ms);
}


/*!
    \fn bool QTime::isNull() const

    Returns true if the time is null (i.e., the QTime object was
    constructed using the default constructor); otherwise returns
    false. A null time is also an invalid time.

    \sa isValid()
*/

/*!
    Returns true if the time is valid; otherwise returns false. For example,
    the time 23:30:55.746 is valid, but 24:12:30 is invalid.

    \sa isNull()
*/

bool QTime::isValid() const
{
    return mds > NullTime && mds < MSECS_PER_DAY;
}


/*!
    Returns the hour part (0 to 23) of the time.

    \sa minute(), second(), msec()
*/

int QTime::hour() const
{
    if (!isValid())
        return -1;

    return ds() / MSECS_PER_HOUR;
}

/*!
    Returns the minute part (0 to 59) of the time.

    \sa hour(), second(), msec()
*/

int QTime::minute() const
{
    if (!isValid())
        return -1;

    return (ds() % MSECS_PER_HOUR) / MSECS_PER_MIN;
}

/*!
    Returns the second part (0 to 59) of the time.

    \sa hour(), minute(), msec()
*/

int QTime::second() const
{
    if (!isValid())
        return -1;

    return (ds() / 1000)%SECS_PER_MIN;
}

/*!
    Returns the millisecond part (0 to 999) of the time.

    \sa hour(), minute(), second()
*/

int QTime::msec() const
{
    if (!isValid())
        return -1;

    return ds() % 1000;
}

#ifndef QT_NO_DATESTRING
/*!
    \overload

    Returns the time as a string. Milliseconds are not included. The
    \a format parameter determines the format of the string.

    If \a format is Qt::TextDate, the string format is HH:mm:ss; e.g. 1
    second before midnight would be "23:59:59".

    If \a format is Qt::ISODate, the string format corresponds to the
    ISO 8601 extended specification for representations of dates,
    which is also HH:mm:ss. (However, contrary to ISO 8601, dates
    before 15 October 1582 are handled as Julian dates, not Gregorian
    dates. See \l{QDate G and J} {Use of Gregorian and Julian
    Calendars}. This might change in a future version of Qt.)

    If the \a format is Qt::SystemLocaleShortDate or
    Qt::SystemLocaleLongDate, the string format depends on the locale
    settings of the system. Identical to calling
    QLocale::system().toString(time, QLocale::ShortFormat) or
    QLocale::system().toString(time, QLocale::LongFormat).

    If the \a format is Qt::DefaultLocaleShortDate or
    Qt::DefaultLocaleLongDate, the string format depends on the
    default application locale. This is the locale set with
    QLocale::setDefault(), or the system locale if no default locale
    has been set. Identical to calling QLocale().toString(time,
    QLocale::ShortFormat) or QLocale().toString(time,
    QLocale::LongFormat).

    If the time is invalid, an empty string will be returned.

    \sa QDate::toString(), QDateTime::toString()
*/

QString QTime::toString(Qt::DateFormat format) const
{
    if (!isValid())
        return QString();

    switch (format) {
    case Qt::SystemLocaleDate:
    case Qt::SystemLocaleShortDate:
        return QLocale::system().toString(*this, QLocale::ShortFormat);
    case Qt::SystemLocaleLongDate:
        return QLocale::system().toString(*this, QLocale::LongFormat);
    case Qt::LocaleDate:
    case Qt::DefaultLocaleShortDate:
        return QLocale().toString(*this, QLocale::ShortFormat);
    case Qt::DefaultLocaleLongDate:
        return QLocale().toString(*this, QLocale::LongFormat);
    case Qt::ISODate:
    case Qt::TextDate:
    default:
        return QString::asprintf("%02d:%02d:%02d", hour(), minute(), second());
    }
}

/*!
    Returns the time as a string. The \a format parameter determines
    the format of the result string.

    These expressions may be used:

    \table
    \header \i Expression \i Output
    \row \i h
         \i the hour without a leading zero (0 to 23 or 1 to 12 if AM/PM display)
    \row \i hh
         \i the hour with a leading zero (00 to 23 or 01 to 12 if AM/PM display)
    \row \i H
         \i the hour without a leading zero (0 to 23, even with AM/PM display)
    \row \i HH
         \i the hour with a leading zero (00 to 23, even with AM/PM display)
    \row \i m \i the minute without a leading zero (0 to 59)
    \row \i mm \i the minute with a leading zero (00 to 59)
    \row \i s \i the second without a leading zero (0 to 59)
    \row \i ss \i the second with a leading zero (00 to 59)
    \row \i z \i the milliseconds without leading zeroes (0 to 999)
    \row \i zzz \i the milliseconds with leading zeroes (000 to 999)
    \row \i AP or A
         \i use AM/PM display. \e AP will be replaced by either "AM" or "PM".
    \row \i ap or a
         \i use am/pm display. \e ap will be replaced by either "am" or "pm".
    \row \i t \i the timezone (for example "CEST")
    \endtable

    All other input characters will be ignored. Any sequence of characters that
    are enclosed in singlequotes will be treated as text and not be used as an
    expression. Two consecutive singlequotes ("''") are replaced by a singlequote
    in the output.

    Example format strings (assuming that the QTime is 14:13:09.042)

    \table
    \header \i Format \i Result
    \row \i hh:mm:ss.zzz \i 14:13:09.042
    \row \i h:m:s ap     \i 2:13:9 pm
    \row \i H:m:s a      \i 14:13:9 pm
    \endtable

    If the datetime is invalid, an empty string will be returned.
    If \a format is empty, the default format "hh:mm:ss" is used.

    \sa QDate::toString() QDateTime::toString(), QLocale::toString()
*/
QString QTime::toString(const QString& format) const
{
    return QLocale::system().toString(*this, format);
}
#endif //QT_NO_DATESTRING
/*!
    Sets the time to hour \a h, minute \a m, seconds \a s and
    milliseconds \a ms.

    \a h must be in the range 0 to 23, \a m and \a s must be in the
    range 0 to 59, and \a ms must be in the range 0 to 999.
    Returns true if the set time is valid; otherwise returns false.

    \sa isValid()
*/

bool QTime::setHMS(int h, int m, int s, int ms)
{
    if (!isValid(h,m,s,ms)) {
        mds = NullTime;                // make this invalid
        return false;
    }
    mds = (h*SECS_PER_HOUR + m*SECS_PER_MIN + s)*1000 + ms;
    return true;
}

/*!
    Returns a QTime object containing a time \a s seconds later
    than the time of this object (or earlier if \a s is negative).

    Note that the time will wrap if it passes midnight.

    Example:

    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 5

    \sa addMSecs(), secsTo(), QDateTime::addSecs()
*/

QTime QTime::addSecs(int s) const
{
    s %= SECS_PER_DAY;
    return addMSecs(s * 1000);
}

/*!
    Returns the number of seconds from this time to \a t.
    If \a t is earlier than this time, the number of seconds returned
    is negative.

    Because QTime measures time within a day and there are 86400
    seconds in a day, the result is always between -86400 and 86400.

    secsTo() does not take into account any milliseconds.

    \sa addSecs(), QDateTime::secsTo()
*/

int QTime::secsTo(const QTime &t) const
{
    if (!isValid() || !t.isValid())
        return 0;

    // Truncate milliseconds as we do not want to consider them.
    int ourSeconds = ds() / 1000;
    int theirSeconds = t.ds() / 1000;
    return theirSeconds - ourSeconds;
}

/*!
    Returns a QTime object containing a time \a ms milliseconds later
    than the time of this object (or earlier if \a ms is negative).

    Note that the time will wrap if it passes midnight. See addSecs()
    for an example.

    \sa addSecs(), msecsTo(), QDateTime::addMSecs()
*/

QTime QTime::addMSecs(int ms) const
{
    QTime t;
    if (isValid()) {
        if (ms < 0) {
            // % not well-defined for -ve, but / is.
            int negdays = (MSECS_PER_DAY - ms) / MSECS_PER_DAY;
            t.mds = (ds() + ms + negdays * MSECS_PER_DAY) % MSECS_PER_DAY;
        } else {
            t.mds = (ds() + ms) % MSECS_PER_DAY;
        }
    }
    return t;
}

/*!
    Returns the number of milliseconds from this time to \a t.
    If \a t is earlier than this time, the number of milliseconds returned
    is negative.

    Because QTime measures time within a day and there are 86400
    seconds in a day, the result is always between -86400000 and
    86400000 ms.

    \sa secsTo(), addMSecs(), QDateTime::msecsTo()
*/

int QTime::msecsTo(const QTime &t) const
{
    if (!isValid() || !t.isValid())
        return 0;
    return t.ds() - ds();
}


/*!
    \fn bool QTime::operator==(const QTime &t) const

    Returns true if this time is equal to \a t; otherwise returns false.
*/

/*!
    \fn bool QTime::operator!=(const QTime &t) const

    Returns true if this time is different from \a t; otherwise returns false.
*/

/*!
    \fn bool QTime::operator<(const QTime &t) const

    Returns true if this time is earlier than \a t; otherwise returns false.
*/

/*!
    \fn bool QTime::operator<=(const QTime &t) const

    Returns true if this time is earlier than or equal to \a t;
    otherwise returns false.
*/

/*!
    \fn bool QTime::operator>(const QTime &t) const

    Returns true if this time is later than \a t; otherwise returns false.
*/

/*!
    \fn bool QTime::operator>=(const QTime &t) const

    Returns true if this time is later than or equal to \a t;
    otherwise returns false.
*/

/*!
    \fn QTime QTime::fromMSecsSinceStartOfDay(int msecs)

    Returns a new QTime instance with the time set to the number of \a msecs
    since the start of the day, i.e. since 00:00:00.

    If \a msecs falls outside the valid range an invalid QTime will be returned.

    \sa msecsSinceStartOfDay()
*/

/*!
    \fn int QTime::msecsSinceStartOfDay() const

    Returns the number of msecs since the start of the day, i.e. since 00:00:00.

    \sa fromMSecsSinceStartOfDay()
*/

/*!
    \fn QTime::currentTime()

    Returns the current time as reported by the system clock.

    Note that the accuracy depends on the accuracy of the underlying
    operating system; not all systems provide 1-millisecond accuracy.
*/

#ifndef QT_NO_DATESTRING
static QTime fromIsoTimeString(const QString &string, Qt::DateFormat format, bool *isMidnight24)
{
    if (isMidnight24)
        *isMidnight24 = false;

    const int size = string.size();
    if (size < 5)
        return QTime();

    bool ok = false;
    int hour = string.mid(0, 2).toInt(&ok);
    if (!ok)
        return QTime();
    const int minute = string.mid(3, 2).toInt(&ok);
    if (!ok)
        return QTime();
    int second = 0;
    int msec = 0;

    if (size == 5) {
        // HH:MM format
        second = 0;
        msec = 0;
    } else if (string.at(5) == QLatin1Char(',') || string.at(5) == QLatin1Char('.')) {
        if (format == Qt::TextDate)
            return QTime();
        // ISODate HH:MM.SSSSSS format
        // We only want 5 digits worth of fraction of minute. This follows the existing
        // behavior that determines how milliseconds are read; 4 millisecond digits are
        // read and then rounded to 3. If we read at most 5 digits for fraction of minute,
        // the maximum amount of millisecond digits it will expand to once converted to
        // seconds is 4. E.g. 12:34,99999 will expand to 12:34:59.9994. The milliseconds
        // will then be rounded up AND clamped to 999.
        const float minuteFraction = QString::fromUtf8("0.%1").arg(string.mid(6, 5)).toFloat(&ok);
        if (!ok)
            return QTime();
        const float secondWithMs = minuteFraction * 60;
        const float secondNoMs = std::floor(secondWithMs);
        const float secondFraction = secondWithMs - secondNoMs;
        second = secondNoMs;
        msec = std::min(qRound(secondFraction * 1000.0), 999);
    } else {
        // HH:MM:SS or HH:MM:SS.sssss
        second = string.mid(6, 2).toInt(&ok);
        if (!ok)
            return QTime();
        if (size > 8 && (string.at(8) == QLatin1Char(',') || string.at(8) == QLatin1Char('.'))) {
            const double secondFraction = QString::fromUtf8("0.%1").arg(string.mid(9, 4)).toDouble(&ok);
            if (!ok)
                return QTime();
            msec = std::min(qRound(secondFraction * 1000.0), 999);
        }
    }

    if (format == Qt::ISODate && hour == 24 && minute == 0 && second == 0 && msec == 0) {
        if (isMidnight24)
            *isMidnight24 = true;
        hour = 0;
    }

    return QTime(hour, minute, second, msec);
}


/*!
    \fn QTime QTime::fromString(const QString &string, Qt::DateFormat format)
    Returns the time represented in the \a string as a QTime using the
    \a format given, or an invalid time if this is not possible.
    Note that fromString() uses a "C" locale encoded string to convert
    milliseconds to a float value. If the default locale is not "C",
    this may result in two conversion attempts (if the conversion
    fails for the default locale). This should be considered an
    implementation detail.
*/
QTime QTime::fromString(const QString& string, Qt::DateFormat format)
{
    if (string.isEmpty())
        return QTime();

    switch (format) {
    case Qt::SystemLocaleDate:
    case Qt::SystemLocaleShortDate:
        return QLocale::system().toTime(string, QLocale::ShortFormat);
    case Qt::SystemLocaleLongDate:
        return QLocale::system().toTime(string, QLocale::LongFormat);
    case Qt::LocaleDate:
    case Qt::DefaultLocaleShortDate:
        return QLocale().toTime(string, QLocale::ShortFormat);
    case Qt::DefaultLocaleLongDate:
        return QLocale().toTime(string, QLocale::LongFormat);
    case Qt::ISODate:
    case Qt::TextDate:
    default:
        return fromIsoTimeString(string, format, nullptr);
    }
}

/*!
    \fn QTime::fromString(const QString &string, const QString &format)

    Returns the QTime represented by the \a string, using the \a
    format given, or an invalid time if the string cannot be parsed.

    These expressions may be used for the format:

    \table
    \header \i Expression \i Output
    \row \i h
         \i the hour without a leading zero (0 to 23 or 1 to 12 if AM/PM display)
    \row \i hh
         \i the hour with a leading zero (00 to 23 or 01 to 12 if AM/PM display)
    \row \i m \i the minute without a leading zero (0 to 59)
    \row \i mm \i the minute with a leading zero (00 to 59)
    \row \i s \i the second without a leading zero (0 to 59)
    \row \i ss \i the second with a leading zero (00 to 59)
    \row \i z \i the milliseconds without leading zeroes (0 to 999)
    \row \i zzz \i the milliseconds with leading zeroes (000 to 999)
    \row \i AP
         \i interpret as an AM/PM time. \e AP must be either "AM" or "PM".
    \row \i ap
         \i Interpret as an AM/PM time. \e ap must be either "am" or "pm".
    \endtable

    All other input characters will be treated as text. Any sequence
    of characters that are enclosed in single quotes will also be
    treated as text and not be used as an expression.

    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 6

    If the format is not satisfied an invalid QTime is returned.
    Expressions that do not expect leading zeroes to be given (h, m, s
    and z) are greedy. This means that they will use two digits even if
    this puts them outside the range of accepted values and leaves too
    few digits for other sections. For example, the following string
    could have meant 00:07:10, but the m will grab two digits, resulting
    in an invalid time:

    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 7

    Any field that is not represented in the format will be set to zero.
    For example:

    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 8

    \sa QDateTime::fromString() QDate::fromString() QDate::toString()
    QDateTime::toString() QTime::toString()
*/

QTime QTime::fromString(const QString &string, const QString &format)
{
    QTime time;
#ifndef QT_BOOTSTRAPPED
    QDateTimeParser dt(QVariant::Time, QDateTimeParser::FromString);
    if (dt.parseFormat(format))
        dt.fromString(string, nullptr, &time);
#else
    Q_UNUSED(string);
    Q_UNUSED(format);
#endif
    return time;
}

#endif // QT_NO_DATESTRING


/*!
    \overload

    Returns true if the specified time is valid; otherwise returns
    false.

    The time is valid if \a h is in the range 0 to 23, \a m and
    \a s are in the range 0 to 59, and \a ms is in the range 0 to 999.

    Example:

    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 9
*/

bool QTime::isValid(int h, int m, int s, int ms)
{
    return (uint)h < 24 && (uint)m < 60 && (uint)s < 60 && (uint)ms < 1000;
}


/*!
    Sets this time to the current time. This is practical for timing:

    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 10

    \sa restart(), elapsed(), currentTime()
*/

void QTime::start()
{
    *this = currentTime();
}

/*!
    Sets this time to the current time and returns the number of
    milliseconds that have elapsed since the last time start() or
    restart() was called.

    This function is guaranteed to be atomic and is thus very handy
    for repeated measurements. Call start() to start the first
    measurement, and restart() for each later measurement.

    Note that the counter wraps to zero 24 hours after the last call
    to start() or restart().

    \warning If the system's clock setting has been changed since the
    last time start() or restart() was called, the result is
    undefined. This can happen when daylight savings time is turned on
    or off.

    \sa start(), elapsed(), currentTime()
*/

int QTime::restart()
{
    QTime t = currentTime();
    int n = msecsTo(t);
    if (n < 0)                                // passed midnight
        n += 86400*1000;
    *this = t;
    return n;
}

/*!
    Returns the number of milliseconds that have elapsed since the
    last time start() or restart() was called.

    Note that the counter wraps to zero 24 hours after the last call
    to start() or restart.

    Note that the accuracy depends on the accuracy of the underlying
    operating system; not all systems provide 1-millisecond accuracy.

    \warning If the system's clock setting has been changed since the
    last time start() or restart() was called, the result is
    undefined. This can happen when daylight savings time is turned on
    or off.

    \sa start(), restart()
*/

int QTime::elapsed() const
{
    int n = msecsTo(currentTime());
    if (n < 0)                                // passed midnight
        n += 86400 * 1000;
    return n;
}


/*****************************************************************************
  QDateTimePrivate private helper functions
 *****************************************************************************/

// Calls the platform variant of tzset
static void qt_tzset()
{
#if defined(Q_OS_WIN)
    _tzset();
#else
    tzset();
#endif // Q_OS_WIN
}

// Returns the platform variant of timezone, i.e. the standard time offset
// The timezone external variable is documented as always holding the
// Standard Time offset as seconds west of Greenwich, i.e. UTC+01:00 is -3600
// Note this may not be historicaly accurate.
// Relies on tzset, mktime, or localtime having been called to populate timezone
static int qt_timezone()
{
#if defined(_MSC_VER) && _MSC_VER >= 1400
        long offset;
        _get_timezone(&offset);
        return offset;
#else
        return timezone;
#endif // Q_OS_WIN
}

// Calls the platform variant of localtime for the given msecs, and updates
// the date, time, and daylight status with the returned values.
static bool qt_localtime(qint64 msecsSinceEpoch, QDate *localDate, QTime *localTime,
                         QDateTimePrivate::DaylightStatus *daylightStatus)
{
    const time_t secsSinceEpoch = msecsSinceEpoch / 1000;
    const int msec = msecsSinceEpoch % 1000;

    tm local;
    bool valid = false;

#if  !defined(QT_NO_THREAD) && defined(_POSIX_THREAD_SAFE_FUNCTIONS)
    // Use the reentrant version of localtime() where available
    // as is thread-safe and doesn't use a shared static data area
    tm *res = nullptr;
    res = localtime_r(&secsSinceEpoch, &local);
    if (res)
        valid = true;
#elif defined(_MSC_VER) && _MSC_VER >= 1400
    if (!_localtime64_s(&local, &secsSinceEpoch))
        valid = true;
#else
    // Returns shared static data which may be overwritten at any time
    // So copy the result asap
    tm *res = nullptr;
    res = localtime(&secsSinceEpoch);
    if (res) {
        local = *res;
        valid = true;
    }
#endif
    if (valid) {
        *localDate = QDate(local.tm_year + 1900, local.tm_mon + 1, local.tm_mday);
        *localTime = QTime(local.tm_hour, local.tm_min, local.tm_sec, msec);
        if (daylightStatus) {
            if (local.tm_isdst > 0)
                *daylightStatus = QDateTimePrivate::DaylightTime;
            else if (local.tm_isdst < 0)
                *daylightStatus = QDateTimePrivate::UnknownDaylightTime;
            else
                *daylightStatus = QDateTimePrivate::StandardTime;
        }
        return true;
    } else {
        *localDate = QDate();
        *localTime = QTime();
        if (daylightStatus)
            *daylightStatus = QDateTimePrivate::UnknownDaylightTime;
        return false;
    }
}

// Converts an msecs value into a date and time
static void msecsToTime(qint64 msecs, QDate *date, QTime *time)
{
    qint64 jd = JULIAN_DAY_FOR_EPOCH;
    qint64 ds = 0;

    if (std::abs(msecs) >= MSECS_PER_DAY) {
        jd += (msecs / MSECS_PER_DAY);
        msecs %= MSECS_PER_DAY;
    }

    if (msecs < 0) {
        ds = MSECS_PER_DAY - msecs - 1;
        jd -= ds / MSECS_PER_DAY;
        ds = ds % MSECS_PER_DAY;
        ds = MSECS_PER_DAY - ds - 1;
    } else {
        ds = msecs;
    }

    if (date)
        *date = QDate::fromJulianDay(jd);
    if (time)
        *time = QTime::fromMSecsSinceStartOfDay(ds);
}

// Converts a date/time value into msecs
static qint64 timeToMSecs(const QDate &date, const QTime &time)
{
    return ((date.toJulianDay() - JULIAN_DAY_FOR_EPOCH) * MSECS_PER_DAY)
           + time.msecsSinceStartOfDay();
}

// Convert an MSecs Since Epoch into Local Time
static bool epochMSecsToLocalTime(qint64 msecs, QDate *localDate, QTime *localTime,
                                  QDateTimePrivate::DaylightStatus *daylightStatus = 0)
{
    if (msecs < 0) {
        // Docs state any LocalTime before 1970-01-01 will *not* have any Daylight Time applied
        // Instead just use the standard offset from UTC to convert to UTC time
        qt_tzset();
        msecsToTime(msecs - qt_timezone() * 1000, localDate, localTime);
        if (daylightStatus)
            *daylightStatus = QDateTimePrivate::StandardTime;
        return true;
    } else if (msecs > (qint64(TIME_T_MAX) * 1000)) {
        // Docs state any LocalTime after 2037-12-31 *will* have any Daylight Time applied
        // but this may fall outside the supported time_t range, so need to fake it.
        // Use existing method to fake the conversion, but this is deeply flawed as it may
        // apply the conversion from the wrong day number, e.g. if rule is last Sunday of month
        // TODO Use QTimeZone when available to apply the future rule correctly
        QDate utcDate;
        QTime utcTime;
        msecsToTime(msecs, &utcDate, &utcTime);
        int year, month, day;
        utcDate.getDate(&year, &month, &day);
        // 2037 is not a leap year, so make sure date isn't Feb 29
        if (month == 2 && day == 29)
            --day;
        QDate fakeDate(2037, month, day);
        qint64 fakeMsecs = QDateTime(fakeDate, utcTime, Qt::UTC).toMSecsSinceEpoch();
        bool res = qt_localtime(fakeMsecs, localDate, localTime, daylightStatus);
        *localDate = localDate->addDays(fakeDate.daysTo(utcDate));
        return res;
    } else {
        // Falls inside time_t suported range so can use localtime
        return qt_localtime(msecs, localDate, localTime, daylightStatus);
    }
}

// Convert a LocalTime expressed in local msecs encoding into a UTC epoch msecs
// Optionally populate the returned values from mktime for the adjusted local
// date and time and daylight status
static qint64 localMSecsToEpochMSecs(qint64 localMsecs, QDate *localDate = nullptr, QTime *localTime = nullptr,
                                     QDateTimePrivate::DaylightStatus *daylightStatus = nullptr,
                                     QString *abbreviation = nullptr)
{
    QDate dt;
    QTime tm;
    msecsToTime(localMsecs, &dt, &tm);

    constexpr qint64 msecsMax = qint64(TIME_T_MAX) * 1000;

    if (localMsecs <= qint64(MSECS_PER_DAY)) {

        // Docs state any LocalTime before 1970-01-01 will *not* have any Daylight Time applied

        // First, if localMsecs is within +/- 1 day of minimum time_t try mktime in case it does
        // fall after minimum and needs proper daylight conversion
        if (localMsecs >= -qint64(MSECS_PER_DAY)) {
            bool valid;
            qint64 utcMsecs = qt_mktime(&dt, &tm, daylightStatus, abbreviation, &valid);
            if (valid && utcMsecs >= 0) {
                // mktime worked and falls in valid range, so use it
                if (localDate)
                    *localDate = dt;
                if (localTime)
                    *localTime = tm;
                return utcMsecs;
            }
        } else {
            // If we don't call mktime then need to call tzset to get offset
            qt_tzset();
        }
        // Time is clearly before 1970-01-01 so just use standard offset to convert
        qint64 utcMsecs = localMsecs + qt_timezone() * 1000;
        if (localDate || localTime)
            msecsToTime(localMsecs, localDate, localTime);
        if (daylightStatus)
            *daylightStatus = QDateTimePrivate::StandardTime;
        if (abbreviation)
            *abbreviation = qt_tzname(QDateTimePrivate::StandardTime);
        return utcMsecs;

    } else if (localMsecs >= msecsMax - MSECS_PER_DAY) {

        // Docs state any LocalTime after 2037-12-31 *will* have any Daylight Time applied
        // but this may fall outside the supported time_t range, so need to fake it.

        // First, if localMsecs is within +/- 1 day of maximum time_t try mktime in case it does
        // fall before maximum and can use proper daylight conversion
        if (localMsecs <= msecsMax + MSECS_PER_DAY) {
            bool valid;
            qint64 utcMsecs = qt_mktime(&dt, &tm, daylightStatus, abbreviation, &valid);
            if (valid && utcMsecs <= msecsMax) {
                // mktime worked and falls in valid range, so use it
                if (localDate)
                    *localDate = dt;
                if (localTime)
                    *localTime = tm;
                return utcMsecs;
            }
        }
        // Use existing method to fake the conversion, but this is deeply flawed as it may
        // apply the conversion from the wrong day number, e.g. if rule is last Sunday of month
        // TODO Use QTimeZone when available to apply the future rule correctly
        int year, month, day;
        dt.getDate(&year, &month, &day);
        // 2037 is not a leap year, so make sure date isn't Feb 29
        if (month == 2 && day == 29)
            --day;
        QDate fakeDate(2037, month, day);
        qint64 fakeDiff = fakeDate.daysTo(dt);
        qint64 utcMsecs = qt_mktime(&fakeDate, &tm, daylightStatus, abbreviation);
        if (localDate)
            *localDate = fakeDate.addDays(fakeDiff);
        if (localTime)
            *localTime = tm;
        QDate utcDate;
        QTime utcTime;
        msecsToTime(utcMsecs, &utcDate, &utcTime);
        utcDate = utcDate.addDays(fakeDiff);
        utcMsecs = timeToMSecs(utcDate, utcTime);
        return utcMsecs;

    } else {

        // Clearly falls inside 1970-2037 suported range so can use mktime
        qint64 utcMsecs = qt_mktime(&dt, &tm, daylightStatus, abbreviation);
        if (localDate)
            *localDate = dt;
        if (localTime)
            *localTime = tm;
        return utcMsecs;

    }
}

/*****************************************************************************
  QDateTimePrivate member functions
 *****************************************************************************/

void QDateTimePrivate::setTimeSpec(Qt::TimeSpec spec, int offsetSeconds)
{
    switch (spec) {
    case Qt::OffsetFromUTC:
        if (offsetSeconds == 0) {
            m_spec = Qt::UTC;
            m_offsetFromUtc = 0;
        } else {
            m_spec = Qt::OffsetFromUTC;
            m_offsetFromUtc = offsetSeconds;
        }
        break;
    case Qt::UTC:
    case Qt::LocalTime:
        m_spec = spec;
        m_offsetFromUtc = 0;
        break;
    }
}

void QDateTimePrivate::setDateTime(const QDate &date, const QTime &time)
{
    // If the date is valid and the time is not we set time to 00:00:00
    QTime useTime = time;
    if (!useTime.isValid() && date.isValid())
        useTime = QTime::fromMSecsSinceStartOfDay(0);

    // Reset the status
    m_status = 0;

    // Set date value and status
    qint64 days = 0;
    if (date.isValid()) {
        days = date.toJulianDay() - JULIAN_DAY_FOR_EPOCH;
        m_status = ValidDate;
    } else if (date.isNull()) {
        m_status = NullDate;
    }

    // Set time value and status
    int ds = 0;
    if (useTime.isValid()) {
        ds = useTime.msecsSinceStartOfDay();
        m_status = m_status | ValidTime;
    } else if (time.isNull()) {
        m_status = m_status | NullTime;
    }

    // Set msecs serial value
    m_msecs = (days * MSECS_PER_DAY) + ds;
}

void QDateTimePrivate::getDateTime(QDate *date, QTime *time) const
{
    msecsToTime(m_msecs, date, time);

    if (isNullDate())
        *date = QDate();

    if (isNullTime())
        *time = QTime();
}

/*****************************************************************************
  QDateTime member functions
 *****************************************************************************/

/*!
    \class QDateTime
    \reentrant
    \brief The QDateTime class provides date and time functions.


    A QDateTime object contains a calendar date and a clock time (a
    "datetime"). It is a combination of the QDate and QTime classes.
    It can read the current datetime from the system clock. It
    provides functions for comparing datetimes and for manipulating a
    datetime by adding a number of seconds, days, months, or years.

    A QDateTime object is typically created either by giving a date
    and time explicitly in the constructor, or by using the static
    function currentDateTime() that returns a QDateTime object set
    to the system clock's time. The date and time can be changed with
    setDate() and setTime(). A datetime can also be set using the
    setTime_t() function that takes a POSIX-standard "number of
    seconds since 00:00:00 on January 1, 1970" value. The fromString()
    function returns a QDateTime, given a string and a date format
    used to interpret the date within the string.

    The date() and time() functions provide access to the date and
    time parts of the datetime. The same information is provided in
    textual format by the toString() function.

    QDateTime provides a full set of operators to compare two
    QDateTime objects where smaller means earlier and larger means
    later.

    You can increment (or decrement) a datetime by a given number of
    milliseconds using addMSecs(), seconds using addSecs(), or days
    using addDays(). Similarly you can use addMonths() and addYears().
    The daysTo() function returns the number of days between two datetimes,
    secsTo() returns the number of seconds between two datetimes, and
    msecsTo() returns the number of milliseconds between two datetimes.

    QDateTime can store datetimes as \l{Qt::LocalTime}{local time} or
    as \l{Qt::UTC}{UTC}. QDateTime::currentDateTime() returns a
    QDateTime expressed as local time; use toUTC() to convert it to
    UTC. You can also use timeSpec() to find out if a QDateTime
    object stores a UTC time or a local time. Operations such as
    addSecs() and secsTo() are aware of daylight saving time (DST).

    \note QDateTime does not account for leap seconds.

    \section1

    \target QDateTime G and J
    \section2 Use of Gregorian and Julian Calendars

    QDate uses the Gregorian calendar in all locales, beginning
    on the date 15 October 1582. For dates up to and including 4
    October 1582, the Julian calendar is used.  This means there is a
    10-day gap in the internal calendar between the 4th and the 15th
    of October 1582. When you use QDateTime for dates in that epoch,
    the day after 4 October 1582 is 15 October 1582, and the dates in
    the gap are invalid.

    The Julian to Gregorian changeover date used here is the date when
    the Gregorian calendar was first introduced, by Pope Gregory
    XIII. That change was not universally accepted and some localities
    only executed it at a later date (if at all).  QDateTime
    doesn't take any of these historical facts into account. If an
    application must support a locale-specific dating system, it must
    do so on its own, remembering to convert the dates using the
    Julian day.

    \section2 No Year 0

    There is no year 0. Dates in that year are considered invalid. The
    year -1 is the year "1 before Christ" or "1 before current era."
    The day before 1 January 1 CE is 31 December 1 BCE.

    \section2 Range of Valid Dates

    The range of valid values able to be stored in QDateTime is dependent onMore actions
    the internal storage implementation. QDateTime is currently stored in a
    qint64 as a serial msecs value encoding the date and time.  This restricts
    the date range to about +/- 292 million years, compared to the QDate range
    of +/- 2 billion years.  Care must be taken when creating a QDateTime with
    extreme values that you do not overflow the storage.  The exact range of
    supported values varies depending on the Qt::TimeSpec and time zone.

    The Gregorian calendar was introduced in different places around
    the world on different dates. QDateTime uses QDate to store the
    date, so it uses the Gregorian calendar for all locales, beginning
    on the date 15 October 1582. For dates up to and including 4
    October 1582, QDateTime uses the Julian calendar.  This means
    there is a 10-day gap in the QDateTime calendar between the 4th
    and the 15th of October 1582. When you use QDateTime for dates in
    that epoch, the day after 4 October 1582 is 15 October 1582, and
    the dates in the gap are invalid.

    \section2
    Use of System Timezone

    QDateTime uses the system's time zone information to determine the
    offset of local time from UTC. If the system is not configured
    correctly or not up-to-date, QDateTime will give wrong results as
    well.

    \section2 Daylight Savings Time (DST)

    QDateTime takes into account the system's time zone information
    when dealing with DST. On modern Unix systems, this means it
    applies the correct historical DST data whenever possible. On
    Windows and Windows CE, where the system doesn't support
    historical DST data, historical accuracy is not maintained with
    respect to DST.

    The range of valid dates taking DST into account is 1970-01-01 to
    the present, and rules are in place for handling DST correctly
    until 2037-12-31, but these could change. For dates falling
    outside that range, QDateTime makes a \e{best guess} using the
    rules for year 1970 or 2037, but we can't guarantee accuracy. This
    means QDateTime doesn't take into account changes in a locale's
    time zone before 1970, even if the system's time zone database
    supports that information.

    \section2 Offset From UTC

    A Qt::TimeSpec of Qt::OffsetFromUTC is also supported. This allows you
    to define a QDateTime relative to UTC at a fixed offset of a given number
    of seconds from UTC.  For example, an offset of +3600 seconds is one hour
    ahead of UTC and is usually written in ISO standard notation as
    "UTC+01:00".  Daylight Savings Time never applies with this TimeSpec.

    There is no explicit size restriction to the offset seconds, but there is
    an implicit limit imposed when using the toString() and fromString()
    methods which use a format of [+|-]hh:mm, effectively limiting the range
    to +/- 99 hours and 59 minutes and whole minutes only.  Note that currently
    no time zone lies outside the range of +/- 14 hours.

    \sa QDate QTime QDateTimeEdit
*/

/*!
    Constructs a null datetime (i.e. null date and null time). A null
    datetime is invalid, since the date is invalid.

    \sa isValid()
*/
QDateTime::QDateTime()
    : d(*defaultDateTimePrivate())
{
}


/*!
    Constructs a datetime with the given \a date, a valid
    time(00:00:00.000), and sets the timeSpec() to Qt::LocalTime.
*/

QDateTime::QDateTime(const QDate &date)
    : d(new QDateTimePrivate(date, QTime(0, 0, 0), Qt::LocalTime, 0))
{
}

/*!

    Constructs a datetime with the given \a date and \a time, using
    the time specification defined by \a spec and \a offsetSeconds seconds.

    If \a date is valid and \a time is not, the time will be set to midnight.

    If the \a spec is not Qt::OffsetFromUTC then \a offsetSeconds will be ignored.

    If the \a spec is Qt::OffsetFromUTC and \a offsetSeconds is 0 then the
    timeSpec() will be set to Qt::UTC, i.e. an offset of 0 seconds.
*/

QDateTime::QDateTime(const QDate &date, const QTime &time, Qt::TimeSpec spec, int offsetSeconds)
         : d(new QDateTimePrivate(date, time, spec, offsetSeconds))
{
}

/*!
    \internal

    Private.

    Create a datetime with the given \a date, \a time, \a spec and \a offsetSeconds
*/

QDateTimePrivate::QDateTimePrivate(const QDate &toDate, const QTime &toTime, Qt::TimeSpec toSpec,
                                   int offsetSeconds)
    : m_msecs(0),
      m_spec(Qt::LocalTime),
      m_offsetFromUtc(0),
      m_status(0)
{
    setTimeSpec(toSpec, offsetSeconds);
    setDateTime(toDate, toTime);
}

/*!
    Constructs a copy of the \a other datetime.
*/

QDateTime::QDateTime(const QDateTime &other)
= default;

/*!
    Destroys the datetime.
*/
QDateTime::~QDateTime()
= default;

/*!
    Makes a copy of the \a other datetime and returns a reference to the
    copy.
*/

QDateTime &QDateTime::operator=(const QDateTime &other)
= default;

/*!
    \fn void QDateTime::swap(QDateTime &other)
    \since 5.0
    Swaps this date-time with \a other. This operation is very fast
    and never fails.
*/

/*!
    Returns true if both the date and the time are null; otherwise
    returns false. A null datetime is invalid.

    \sa QDate::isNull(), QTime::isNull(), isValid()
*/

bool QDateTime::isNull() const
{
    return d->isNullDate() && d->isNullTime();
}

/*!
    Returns true if both the date and the time are valid; otherwise
    returns false.

    \sa QDate::isValid(), QTime::isValid()
*/

bool QDateTime::isValid() const
{
    return d->isValidDate() && d->isValidTime();
}

/*!
    Returns the date part of the datetime.

    \sa setDate(), time(), timeSpec()
*/

QDate QDateTime::date() const
{
    QDate dt;
    QTime tm;
    d->getDateTime(&dt, &tm);
    return dt;
}

/*!
    Returns the time part of the datetime.

    \sa setTime(), date(), timeSpec()
*/

QTime QDateTime::time() const
{
    QDate dt;
    QTime tm;
    d->getDateTime(&dt, &tm);
    return tm;
}

/*!
    Returns the time specification of the datetime.

    \sa setTimeSpec(), date(), time(), Qt::TimeSpec
*/

Qt::TimeSpec QDateTime::timeSpec() const
{
    return d->m_spec;
}

/*!
    Returns the Time Zone Abbreviation for the datetime.

    If the timeSpec() is Qt::UTC this will be "UTC".

    If the timeSpec() is Qt::OffsetFromUTC this will be in the format
    "UTC[+-]00:00".

    If the timeSpec() is Qt::LocalTime then the host system is queried for the
    correct abbreviation.

    Note that abbreviations may or may not be localized.

    Note too that the abbreviation is not guaranteed to be a unique value,
    i.e. different time zones may have the same abbreviation.
    \sa timeSpec()
*/

QString QDateTime::timeZoneAbbreviation() const
{
    switch (d->m_spec) {
    case Qt::UTC:
        return QStringLiteral("UTC");
    case Qt::OffsetFromUTC:
        return QLatin1String("UTC") + toOffsetString(Qt::ISODate, d->m_offsetFromUtc);
    case Qt::LocalTime: {
        QString abbrev;
        localMSecsToEpochMSecs(d->m_msecs, nullptr, nullptr, nullptr, &abbrev);
        return abbrev;
        }
    }
    return QString();
}

/*!
    Returns the current Offset From UTC in seconds.

    If the timeSpec() is Qt::OffsetFromUTC this will be the value originally set.

    If the timeSpec() is Qt::LocalTime this will be the difference between the

    Local Time and UTC including any Daylight Saving Offset.

    If the timeSpec() is Qt::UTC this will be 0.
    \sa setOffsetFromUtc()
*/

int QDateTime::offsetFromUtc() const
{
    if (!isValid())
        return 0;

    switch (d->m_spec) {
    case Qt::OffsetFromUTC:
        return d->m_offsetFromUtc;
    case Qt::UTC:
        return 0;
    case Qt::LocalTime:
        if (isValid())
            return (d->m_msecs - toMSecsSinceEpoch()) / 1000;
    }
    return 0;
}

/*!
    Sets the date part of this datetime to \a date.
    If no time is set, it is set to midnight.

    \sa date(), setTime(), setTimeSpec()
*/

void QDateTime::setDate(const QDate &date)
{
    d->setDateTime(date, time());
}

/*!
    Sets the time part of this datetime to \a time.

    \sa time(), setDate(), setTimeSpec()
*/

void QDateTime::setTime(const QTime &time)
{
    d->setDateTime(date(), time);
}

/*!
    Sets the time specification used in this datetime to \a spec.

    \sa timeSpec(), setDate(), setTime(), Qt::TimeSpec
*/

void QDateTime::setTimeSpec(Qt::TimeSpec spec)
{
    QDateTimePrivate *d = this->d.data(); // detaches (and shadows d)

    d->setTimeSpec(spec, 0);
}

/*!

    Sets the timeSpec() to Qt::OffsetFromUTC and the offset to \a offsetSeconds.

    The datetime will refer to a different point in time.

    The maximum and minimum offset is 14 positive or negative hours.  If
    \a offsetSeconds is larger or smaller than that, then the result is
    undefined.

    If \a offsetSeconds is 0 then the timeSpec() will be set to Qt::UTC.
    \sa isValid(), offsetFromUtc()
*/

void QDateTime::setOffsetFromUtc(int offsetSeconds)
{
    QDateTimePrivate *d = this->d.data(); // detaches (and shadows d)

    d->setTimeSpec(Qt::OffsetFromUTC, offsetSeconds);
}

/*!
    \since 4.7

    Returns the datetime as the number of milliseconds that have passed
    since 1970-01-01T00:00:00.000, Coordinated Universal Time (Qt::UTC).

    On systems that do not support time zones, this function will
    behave as if local time were Qt::UTC.

    The behavior for this function is undefined if the datetime stored in
    this object is not valid. However, for all valid dates, this function
    returns a unique value.

    \sa toTime_t(), setMSecsSinceEpoch()
*/
qint64 QDateTime::toMSecsSinceEpoch() const
{
    switch (d->m_spec) {
    case Qt::UTC:
        return d->m_msecs;
    case Qt::OffsetFromUTC:
        return d->m_msecs - (d->m_offsetFromUtc * 1000);
    case Qt::LocalTime:
        return localMSecsToEpochMSecs(d->m_msecs);
    }
    return 0;
}

/*!
    Returns the datetime as the number of seconds that have passed
    since 1970-01-01T00:00:00, Coordinated Universal Time (Qt::UTC).

    On systems that do not support time zones, this function will
    behave as if local time were Qt::UTC.

    \note This function returns a 32-bit unsigned integer, so it does not
    support dates before 1970, but it does support dates after
    2038-01-19T03:14:06, which may not be valid time_t values. Be careful
    when passing those time_t values to system functions, which could
    interpret them as negative dates.

    If the date is outside the range 1970-01-01T00:00:00 to
    2106-02-07T06:28:14, this function returns -1 cast to an unsigned integer
    (i.e., 0xFFFFFFFF).

    To get an extended range, use toMSecsSinceEpoch().

    \sa toMSecsSinceEpoch(), setTime_t()
*/

uint QDateTime::toTime_t() const
{
    if (!isValid())
        return uint(-1);
    qint64 retval = toMSecsSinceEpoch() / 1000;
    if (quint64(retval) >= Q_UINT64_C(0xFFFFFFFF))
        return uint(-1);
    return uint(retval);
}

/*!
    \since 4.7

    Sets the date and time given the number of milliseconds,\a msecs, that have
    passed since 1970-01-01T00:00:00.000, Coordinated Universal Time
    (Qt::UTC). On systems that do not support time zones this function
    will behave as if local time were Qt::UTC.

    Note that passing the minimum of \c qint64More actions
    (\c{std::numeric_limits<qint64>::min()}) to \a msecs will result in
    undefined behavior.

    \sa toMSecsSinceEpoch(), setTime_t()
*/
void QDateTime::setMSecsSinceEpoch(qint64 msecs)
{
    QDateTimePrivate *d = this->d.data(); // detaches (and shadows d)

    d->m_status = 0;
    switch (d->m_spec) {
    case Qt::UTC:
        d->m_msecs = msecs;
        d->m_status = d->m_status | QDateTimePrivate::ValidDate | QDateTimePrivate::ValidTime;
        break;
    case Qt::OffsetFromUTC:
        d->m_msecs = msecs + (d->m_offsetFromUtc * 1000);
        d->m_status = d->m_status | QDateTimePrivate::ValidDate | QDateTimePrivate::ValidTime;
        break;
    case Qt::LocalTime: {
        QDate dt;
        QTime tm;
        epochMSecsToLocalTime(msecs, &dt, &tm);
        d->setDateTime(dt, tm);
        break;
        }
    }
}

/*!
    \fn void QDateTime::setTime_t(uint seconds)

    Sets the date and time given the number of \a seconds that have
    passed since 1970-01-01T00:00:00, Coordinated Universal Time
    (Qt::UTC). On systems that do not support time zones this function
    will behave as if local time were Qt::UTC.

    \sa toTime_t()
*/

void QDateTime::setTime_t(uint secsSince1Jan1970UTC)
{
    setMSecsSinceEpoch((qint64)secsSince1Jan1970UTC * 1000);
}

#ifndef QT_NO_DATESTRING
/*!
    \fn QString QDateTime::toString(Qt::DateFormat format) const

    \overload

    Returns the datetime as a string in the \a format given.

    If the \a format is Qt::TextDate, the string is formatted in
    the default way. QDate::shortDayName(), QDate::shortMonthName(),
    and QTime::toString() are used to generate the string, so the
    day and month names will be localized names. An example of this
    formatting is "Wed May 20 03:40:13 1998".

    If the \a format is Qt::ISODate, the string format corresponds
    to the ISO 8601 extended specification for representations of
    dates and times, taking the form YYYY-MM-DDTHH:mm:ss[Z|[+|-]HH:mm],
    depending on the timeSpec() of the QDateTime. If the timeSpec()
    is Qt::UTC, Z will be appended to the string; if the timeSpec() is
    Qt::OffsetFromUTC the offset in hours and minutes from UTC will
    be appended to the string.

    If the \a format is Qt::SystemLocaleShortDate or
    Qt::SystemLocaleLongDate, the string format depends on the locale
    settings of the system. Identical to calling
    QLocale::system().toString(datetime, QLocale::ShortFormat) or
    QLocale::system().toString(datetime, QLocale::LongFormat).

    If the \a format is Qt::DefaultLocaleShortDate or
    Qt::DefaultLocaleLongDate, the string format depends on the
    default application locale. This is the locale set with
    QLocale::setDefault(), or the system locale if no default locale
    has been set. Identical to calling QLocale().toString(datetime,
    QLocale::ShortFormat) or QLocale().toString(datetime,
    QLocale::LongFormat).

    If the datetime is invalid, an empty string will be returned.

    \warning The Qt::ISODate format is only valid for years in the
    range 0 to 9999. This restriction may apply to locale-aware
    formats as well, depending on the locale settings.

    \sa QDate::toString() QTime::toString() Qt::DateFormat
*/

QString QDateTime::toString(Qt::DateFormat format) const
{
    QString buf;
    if (!isValid())
        return buf;

    switch (format) {
    case Qt::SystemLocaleDate:
    case Qt::SystemLocaleShortDate:
        return QLocale::system().toString(*this, QLocale::ShortFormat);
    case Qt::SystemLocaleLongDate:
        return QLocale::system().toString(*this, QLocale::LongFormat);
    case Qt::LocaleDate:
    case Qt::DefaultLocaleShortDate:
        return QLocale().toString(*this, QLocale::ShortFormat);
    case Qt::DefaultLocaleLongDate:
        return QLocale().toString(*this, QLocale::LongFormat);
    default:
#ifndef QT_NO_TEXTDATE
    case Qt::TextDate: {
        QDate dt;
        QTime tm;
        d->getDateTime(&dt, &tm);
        buf = dt.toString(Qt::TextDate);
        // Insert time between date's day and year:
        buf.insert(buf.lastIndexOf(QLatin1Char(' ')),
                   QLatin1Char(' ') + tm.toString(Qt::TextDate));
        if (timeSpec() != Qt::LocalTime) {
            buf += QStringLiteral(" GMT");
            if (d->m_spec == Qt::OffsetFromUTC)
                buf += toOffsetString(Qt::TextDate, d->m_offsetFromUtc);
        }
        return buf;
    }
#endif
    case Qt::ISODate: {
        QDate dt;
        QTime tm;
        d->getDateTime(&dt, &tm);
        buf = dt.toString(Qt::ISODate);
        if (buf.isEmpty())
            return QString();   // failed to convert
        buf += QLatin1Char('T');
        buf += tm.toString(Qt::ISODate);
        switch (d->m_spec) {
        case Qt::UTC:
            buf += QLatin1Char('Z');
            break;
        case Qt::OffsetFromUTC:
            buf += toOffsetString(Qt::ISODate, d->m_offsetFromUtc);
            break;
        default:
            break;
        }
        return buf;
    }
    }
}

/*!
    Returns the datetime as a string. The \a format parameter
    determines the format of the result string.

    These expressions may be used for the date:

    \table
    \header \i Expression \i Output
    \row \i d \i the day as number without a leading zero (1 to 31)
    \row \i dd \i the day as number with a leading zero (01 to 31)
    \row \i ddd
            \i the abbreviated localized day name (e.g. 'Mon' to 'Sun').
            Uses QDate::shortDayName().
    \row \i dddd
            \i the long localized day name (e.g. 'Monday' to 'Qt::Sunday').
            Uses QDate::longDayName().
    \row \i M \i the month as number without a leading zero (1-12)
    \row \i MM \i the month as number with a leading zero (01-12)
    \row \i MMM
            \i the abbreviated localized month name (e.g. 'Jan' to 'Dec').
            Uses QDate::shortMonthName().
    \row \i MMMM
            \i the long localized month name (e.g. 'January' to 'December').
            Uses QDate::longMonthName().
    \row \i yy \i the year as two digit number (00-99)
    \row \i yyyy \i the year as four digit number
    \endtable

    These expressions may be used for the time:

    \table
    \header \i Expression \i Output
    \row \i h
         \i the hour without a leading zero (0 to 23 or 1 to 12 if AM/PM display)
    \row \i hh
         \i the hour with a leading zero (00 to 23 or 01 to 12 if AM/PM display)
    \row \i m \i the minute without a leading zero (0 to 59)
    \row \i mm \i the minute with a leading zero (00 to 59)
    \row \i s \i the second without a leading zero (0 to 59)
    \row \i ss \i the second with a leading zero (00 to 59)
    \row \i z \i the milliseconds without leading zeroes (0 to 999)
    \row \i zzz \i the milliseconds with leading zeroes (000 to 999)
    \row \i AP
            \i use AM/PM display. \e AP will be replaced by either "AM" or "PM".
    \row \i ap
            \i use am/pm display. \e ap will be replaced by either "am" or "pm".
    \endtable

    All other input characters will be ignored. Any sequence of characters that
    are enclosed in singlequotes will be treated as text and not be used as an
    expression. Two consecutive singlequotes ("''") are replaced by a singlequote
    in the output.

    Example format strings (assumed that the QDateTime is 21 May 2001
    14:13:09):

    \table
    \header \i Format       \i Result
    \row \i dd.MM.yyyy      \i 21.05.2001
    \row \i ddd MMMM d yy   \i Tue May 21 01
    \row \i hh:mm:ss.zzz    \i 14:13:09.042
    \row \i h:m:s ap        \i 2:13:9 pm
    \endtable

    If the datetime is invalid, an empty string will be returned.

    \sa QDate::toString() QTime::toString(), QLocale::toString()
*/
QString QDateTime::toString(const QString& format) const
{
    return QLocale::system().toString(*this, format);
}
#endif //QT_NO_DATESTRING

/*!
    Returns a QDateTime object containing a datetime \a ndays days
    later than the datetime of this object (or earlier if \a ndays is
    negative).

    \sa daysTo(), addMonths(), addYears(), addSecs()
*/

QDateTime QDateTime::addDays(qint64 ndays) const
{
    QDateTime dt(*this);
    QDate date;
    QTime time;
    d->getDateTime(&date, &time);
    dt.d->setDateTime(date.addDays(ndays), time);
    return dt;
}

/*!
    Returns a QDateTime object containing a datetime \a nmonths months
    later than the datetime of this object (or earlier if \a nmonths
    is negative).

    \sa daysTo(), addDays(), addYears(), addSecs()
*/

QDateTime QDateTime::addMonths(int nmonths) const
{
    QDateTime dt(*this);
    QDate date;
    QTime time;
    d->getDateTime(&date, &time);
    dt.d->setDateTime(date.addMonths(nmonths), time);
    return dt;
}

/*!
    Returns a QDateTime object containing a datetime \a nyears years
    later than the datetime of this object (or earlier if \a nyears is
    negative).

    \sa daysTo(), addDays(), addMonths(), addSecs()
*/

QDateTime QDateTime::addYears(int nyears) const
{
    QDateTime dt(*this);
    QDate date;
    QTime time;
    d->getDateTime(&date, &time);
    dt.d->setDateTime(date.addYears(nyears), time);
    return dt;
}

/*!
    Returns a QDateTime object containing a datetime \a s seconds
    later than the datetime of this object (or earlier if \a s is
    negative).

    If this datetime is invalid, an invalid datetime will be returned.

    \sa addMSecs(), secsTo(), addDays(), addMonths(), addYears()
*/

QDateTime QDateTime::addSecs(qint64 s) const
{
    return addMSecs(s * 1000);
}

/*!
    Returns a QDateTime object containing a datetime \a msecs miliseconds
    later than the datetime of this object (or earlier if \a msecs is
    negative).

    If this datetime is invalid, an invalid datetime will be returned.

    \sa addSecs(), msecsTo(), addDays(), addMonths(), addYears()
*/
QDateTime QDateTime::addMSecs(qint64 msecs) const
{
    if (!isValid())
        return QDateTime();

    QDateTime dt(*this);
    if (d->m_spec == Qt::LocalTime)
        // Convert to real UTC first in case crosses daylight transition
        dt.setMSecsSinceEpoch(toMSecsSinceEpoch() + msecs);
    else {
        // No need to convert, just add on
        dt.d.detach();
        dt.d->m_msecs += msecs;
    }
    return dt;
}

/*!
    Returns the number of days from this datetime to the \a other
    datetime. If the \a other datetime is earlier than this datetime,
    the value returned is negative.

    \sa addDays(), secsTo(), msecsTo()
*/

qint64 QDateTime::daysTo(const QDateTime &other) const
{
    return date().daysTo(other.date());
}

/*!
    Returns the number of seconds from this datetime to the \a other
    datetime. If the \a other datetime is earlier than this datetime,
    the value returned is negative.

    Before performing the comparison, the two datetimes are converted
    to Qt::UTC to ensure that the result is correct if one of the two
    datetimes has daylight saving time (DST) and the other doesn't.

    Example:
    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 11

    \sa addSecs(), daysTo(), QTime::secsTo()
*/

int QDateTime::secsTo(const QDateTime &other) const
{
    return (msecsTo(other) / 1000);
}

/*!
    \since 4.7

    Returns the number of milliseconds from this datetime to the \a other
    datetime. If the \a other datetime is earlier than this datetime,
    the value returned is negative.

    Before performing the comparison, the two datetimes are converted
    to Qt::UTC to ensure that the result is correct if one of the two
    datetimes has daylight saving time (DST) and the other doesn't.

    \sa addMSecs(), daysTo(), QTime::msecsTo()
*/

qint64 QDateTime::msecsTo(const QDateTime &other) const
{
    if (!isValid() || !other.isValid())
        return 0;

    return other.toMSecsSinceEpoch() - toMSecsSinceEpoch();
}


/*!
    \fn QDateTime QDateTime::toTimeSpec(Qt::TimeSpec spec) const

    Returns a copy of this datetime configured to use the given time
    \a spec.

    If \a spec is Qt::OffsetFromUTC then it is set to Qt::UTC.  To set to a
    spec of Qt::OffsetFromUTC use toOffsetFromUtc().

    \sa timeSpec(), toUTC(), toLocalTime()
*/

QDateTime QDateTime::toTimeSpec(Qt::TimeSpec spec) const
{
    if (d->m_spec == spec && (spec == Qt::UTC || spec == Qt::LocalTime))
        return *this;

    if (!isValid()) {
        QDateTime ret = *this;
        ret.setTimeSpec(spec);
        return ret;
    }

    return fromMSecsSinceEpoch(toMSecsSinceEpoch(), spec, 0);
}

/*!

    \fn QDateTime QDateTime::toOffsetFromUtc(int offsetSeconds) const

    Returns a copy of this datetime converted to a spec of Qt::OffsetFromUTC
    with the given \a offsetSeconds.

    If the \a offsetSeconds equals 0 then a UTC datetime will be returned
    \sa setOffsetFromUtc(), offsetFromUtc(), toTimeSpec()
*/

QDateTime QDateTime::toOffsetFromUtc(int offsetSeconds) const
{
    if (d->m_spec == Qt::OffsetFromUTC
            && d->m_offsetFromUtc == offsetSeconds)
        return *this;

    if (!isValid()) {
        QDateTime ret = *this;
        ret.setOffsetFromUtc(offsetSeconds);
        return ret;
    }

    return fromMSecsSinceEpoch(toMSecsSinceEpoch(), Qt::OffsetFromUTC, offsetSeconds);
}

/*!
    Returns true if this datetime is equal to the \a other datetime;
    otherwise returns false.

    \sa operator!=()
*/

bool QDateTime::operator==(const QDateTime &other) const
{
    if (d->m_spec == Qt::LocalTime
        && other.d->m_spec == Qt::LocalTime
        && d->m_status == other.d->m_status) {
        return (d->m_msecs == other.d->m_msecs);
    }
    // Convert to UTC and compare
    return (toMSecsSinceEpoch() == other.toMSecsSinceEpoch());
}

/*!
    \fn bool QDateTime::operator!=(const QDateTime &other) const

    Returns true if this datetime is different from the \a other
    datetime; otherwise returns false.

    Two datetimes are different if either the date, the time, or the
    time zone components are different.

    \sa operator==()
*/

/*!
    Returns true if this datetime is earlier than the \a other
    datetime; otherwise returns false.
*/

bool QDateTime::operator<(const QDateTime &other) const
{
    if (d->m_spec == Qt::LocalTime
        && other.d->m_spec == Qt::LocalTime
        && d->m_status == other.d->m_status) {
        return (d->m_msecs < other.d->m_msecs);
    }
    // Convert to UTC and compare
    return (toMSecsSinceEpoch() < other.toMSecsSinceEpoch());
}

/*!
    \fn bool QDateTime::operator<=(const QDateTime &other) const

    Returns true if this datetime is earlier than or equal to the
    \a other datetime; otherwise returns false.
*/

/*!
    \fn bool QDateTime::operator>(const QDateTime &other) const

    Returns true if this datetime is later than the \a other datetime;
    otherwise returns false.
*/

/*!
    \fn bool QDateTime::operator>=(const QDateTime &other) const

    Returns true if this datetime is later than or equal to the
    \a other datetime; otherwise returns false.
*/

/*!
    \fn QDateTime QDateTime::currentDateTime()
    Returns the current datetime, as reported by the system clock, in
    the local time zone.

    \sa currentDateTimeUtc(), QDate::currentDate(), QTime::currentTime(), toTimeSpec()
*/

/*!
    \fn QDateTime QDateTime::currentDateTimeUtc()
    \since 4.7
    Returns the current datetime, as reported by the system clock, in
    UTC.

    \sa currentDateTime(), QDate::currentDate(), QTime::currentTime(), toTimeSpec()
*/

/*!
    \fn qint64 QDateTime::currentMSecsSinceEpoch()
    \since 4.7

    Returns the number of milliseconds since 1970-01-01T00:00:00 Universal
    Coordinated Time. This number is like the POSIX time_t variable, but
    expressed in milliseconds instead.

    \sa currentDateTime(), currentDateTimeUtc(), toTime_t(), toTimeSpec()
*/

static inline uint msecsFromDecomposed(int hour, int minute, int sec, int msec = 0)
{
    return MSECS_PER_HOUR * hour + MSECS_PER_MIN * minute + 1000 * sec + msec;
}

#if defined(Q_OS_WIN)
QDate QDate::currentDate()
{
    QDate d;
    SYSTEMTIME st;
    std::memset(&st, 0, sizeof(SYSTEMTIME));
    GetLocalTime(&st);
    d.jd = julianDayFromDate(st.wYear, st.wMonth, st.wDay);
    return d;
}

QTime QTime::currentTime()
{
    QTime ct;
    SYSTEMTIME st;
    std::memset(&st, 0, sizeof(SYSTEMTIME));
    GetLocalTime(&st);
    ct.setHMS(st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    return ct;
}

QDateTime QDateTime::currentDateTime()
{
    QDate d;
    QTime t;
    SYSTEMTIME st;
    std::memset(&st, 0, sizeof(SYSTEMTIME));
    GetLocalTime(&st);
    d.jd = julianDayFromDate(st.wYear, st.wMonth, st.wDay);
    t.mds = msecsFromDecomposed(st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    return QDateTime(d, t);
}

QDateTime QDateTime::currentDateTimeUtc()
{
    QDate d;
    QTime t;
    SYSTEMTIME st;
    std::memset(&st, 0, sizeof(SYSTEMTIME));
    GetSystemTime(&st);
    d.jd = julianDayFromDate(st.wYear, st.wMonth, st.wDay);
    t.mds = msecsFromDecomposed(st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    return QDateTime(d, t, Qt::UTC);
}

qint64 QDateTime::currentMSecsSinceEpoch() noexcept
{
    SYSTEMTIME st;
    std::memset(&st, 0, sizeof(SYSTEMTIME));
    GetSystemTime(&st);

    return msecsFromDecomposed(st.wHour, st.wMinute, st.wSecond, st.wMilliseconds) +
            qint64(julianDayFromGregorianDate(st.wYear, st.wMonth, st.wDay)
                   - julianDayFromGregorianDate(1970, 1, 1)) * Q_INT64_C(86400000);
}

#elif defined(Q_OS_UNIX)
QDate QDate::currentDate()
{
    return QDateTime::currentDateTime().date();
}

QTime QTime::currentTime()
{
    return QDateTime::currentDateTime().time();
}

QDateTime QDateTime::currentDateTime()
{
    return fromMSecsSinceEpoch(currentMSecsSinceEpoch(), Qt::LocalTime);
}

QDateTime QDateTime::currentDateTimeUtc()
{
    return fromMSecsSinceEpoch(currentMSecsSinceEpoch(), Qt::UTC);
}

qint64 QDateTime::currentMSecsSinceEpoch() noexcept
{
    // posix compliant system
    // we have milliseconds
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return qint64(tv.tv_sec) * Q_INT64_C(1000) + tv.tv_usec / 1000;
}

#else
#error "What system is this?"
#endif

/*!
  \since 4.2

  Returns a datetime whose date and time are the number of \a seconds
  that have passed since 1970-01-01T00:00:00, Coordinated Universal
  Time (Qt::UTC). On systems that do not support time zones, the time
  will be set as if local time were Qt::UTC.

  \sa toTime_t(), setTime_t()
*/
QDateTime QDateTime::fromTime_t(uint seconds)
{
    return fromMSecsSinceEpoch((qint64)seconds * 1000, Qt::LocalTime);
}

/*!
  \since 4.7

  Returns a datetime whose date and time are the number of milliseconds, \a msecs,
  that have passed since 1970-01-01T00:00:00.000, Coordinated Universal
  Time (Qt::UTC). On systems that do not support time zones, the time
  will be set as if local time were Qt::UTC.

  Note that there are possible values for \a msecs that lie outside the valid
  range of QDateTime, both negative and positive. The behavior of this
  function is undefined for those values.

  \sa toTime_t(), setTime_t()
*/
QDateTime QDateTime::fromMSecsSinceEpoch(qint64 msecs, Qt::TimeSpec spec, int offsetSeconds)
{
    QDateTime dt;
    dt.d->setTimeSpec(spec, offsetSeconds);
    dt.setMSecsSinceEpoch(msecs);
    return dt;
}

/*!
 \since 4.4
 \internal
 \obsolete

 Sets the offset from UTC to \a seconds, and also sets timeSpec() to
 Qt::OffsetFromUTC.

 This method was added in 4.4 but never documented as public. It was replaced
 with public method setOffsetFromUtc() for consistency with QTimeZone.

 This method should never be made public.

 \sa setOffsetFromUtc()
 */
void QDateTime::setUtcOffset(int seconds)
{
    setOffsetFromUtc(seconds);
}

/*!
 \since 4.4
 \internal
 \obsolete

 Returns the UTC offset in seconds. If the timeSpec() isn't
 Qt::OffsetFromUTC, 0 is returned. However, since 0 is a valid UTC
 offset the return value of this function cannot be used to determine
 whether a utcOffset() is used or is valid, timeSpec() must be
 checked.

 This method was added in 4.4 but never documented as public. It was replaced
 with public method offsetFromUTC() for consistency with QTimeZone.

 This method should never be made public.

 \sa offsetFromUTC()
 */
int QDateTime::utcOffset() const
{
    return offsetFromUtc();
}

#ifndef QT_NO_DATESTRING

/*!
    \fn QDateTime QDateTime::fromString(const QString &string, Qt::DateFormat format)

    Returns the QDateTime represented by the \a string, using the
    \a format given, or an invalid datetime if this is not possible.

    Note for Qt::TextDate: It is recommended that you use the
    English short month names (e.g. "Jan"). Although localized month
    names can also be used, they depend on the user's locale settings.
*/
QDateTime QDateTime::fromString(const QString& string, Qt::DateFormat format)
{
    if (string.isEmpty())
        return QDateTime();

    switch (format) {
    case Qt::SystemLocaleDate:
    case Qt::SystemLocaleShortDate:
        return QLocale::system().toDateTime(string, QLocale::ShortFormat);
    case Qt::SystemLocaleLongDate:
        return QLocale::system().toDateTime(string, QLocale::LongFormat);
    case Qt::LocaleDate:
    case Qt::DefaultLocaleShortDate:
        return QLocale().toDateTime(string, QLocale::ShortFormat);
    case Qt::DefaultLocaleLongDate:
        return QLocale().toDateTime(string, QLocale::LongFormat);
    case Qt::ISODate: {
        const int size = string.size();
        if (size < 10)
            return QDateTime();

        QDate date = QDate::fromString(string.left(10), Qt::ISODate);
        if (!date.isValid())
            return QDateTime();
        if (size == 10)
            return QDateTime(date);

        Qt::TimeSpec spec = Qt::LocalTime;
        QString isoString = string.mid(10); // trim "yyyy-MM-dd"

        // Must be left with T and at least one digit for the hour:
        if (isoString.size() < 2
            || !(isoString.startsWith(QLatin1Char('T'))
                 // FIXME: QSql relies on QVariant::toDateTime() accepting a space here:
                 || isoString.startsWith(QLatin1Char(' ')))) {
            return QDateTime();
        }
        isoString = isoString.mid(1); // trim 'T' (or space)

        int offset = 0;
        // Check end of string for Time Zone definition, either Z for UTC or [+-]HH:MM for Offset
        if (isoString.endsWith(QLatin1Char('Z'))) {
            spec = Qt::UTC;
            isoString.chop(1); // trim 'Z'
        } else {
            // the loop below is faster but functionally equal to:
            // const int signIndex = isoString.indexOf(QRegExp(QStringLiteral("[+-]")));
            int signIndex = isoString.size() - 1;
            Q_ASSERT(signIndex >= 0);
            bool found = false;
            {
                const QChar plus = QLatin1Char('+');
                const QChar minus = QLatin1Char('-');
                do {
                    QChar character(isoString.at(signIndex));
                    found = character == plus || character == minus;
                } while (!found && --signIndex >= 0);
            }

            if (found) {
                bool ok;
                offset = fromOffsetString(isoString.mid(signIndex), &ok);
                if (!ok)
                    return QDateTime();
                isoString = isoString.left(signIndex);
                spec = Qt::OffsetFromUTC;
            }
        }

        // Might be end of day (24:00, including variants), which QTime considers invalid.
        // ISO 8601 (section 4.2.3) says that 24:00 is equivalent to 00:00 the next day.
        bool isMidnight24 = false;
        QTime time = fromIsoTimeString(isoString, format, &isMidnight24);
        if (!time.isValid())
            return QDateTime();
        if (isMidnight24)
            date = date.addDays(1);
        return QDateTime(date, time, spec, offset);
    }
#if !defined(QT_NO_TEXTDATE)
    case Qt::TextDate: {
        QStringList parts = string.split(QLatin1Char(' '), QString::SkipEmptyParts);

        if ((parts.count() < 5) || (parts.count() > 6))
            return QDateTime();

        // Accept "Sun Dec 1 13:02:00 1974" and "Sun 1. Dec 13:02:00 1974"
        int month = 0;
        int day = 0;
        bool ok = false;

        // First try month then day
        month = fromShortMonthName(parts.at(1));
        if (month)
            day = parts.at(2).toInt();

        // If failed try day then month
        if (!month || !day) {
            month = fromShortMonthName(parts.at(2));
            if (month) {
                QString dayStr = parts.at(1);
                if (dayStr.endsWith(QLatin1Char('.'))) {
                    dayStr.chop(1);
                    day = dayStr.toInt();
                }
            }
        }

        // If both failed, give up
        if (!month || !day)
            return QDateTime();

        // Year can be before or after time, "Sun Dec 1 1974 13:02:00" or "Sun Dec 1 13:02:00 1974"
        // Guess which by looking for ':' in the time
        int year = 0;
        int yearPart = 0;
        int timePart = 0;
        if (parts.at(3).contains(QLatin1Char(':'))) {
            yearPart = 4;
            timePart = 3;
        } else if (parts.at(4).contains(QLatin1Char(':'))) {
            yearPart = 3;
            timePart = 4;
        } else {
            return QDateTime();
        }

        year = parts.at(yearPart).toInt(&ok);
        if (!ok)
            return QDateTime();

        QDate date(year, month, day);
        if (!date.isValid())
            return QDateTime();

        QStringList timeParts = parts.at(timePart).split(QLatin1Char(':'));
        if (timeParts.count() < 2 || timeParts.count() > 3)
            return QDateTime();

        int hour = timeParts.at(0).toInt(&ok);
        if (!ok)
            return QDateTime();

        int minute = timeParts.at(1).toInt(&ok);
        if (!ok)
            return QDateTime();

        int second = 0;
        int millisecond = 0;
        if (timeParts.count() > 2) {
            QStringList secondParts = timeParts.at(2).split(QLatin1Char('.'));
            if (secondParts.size() > 2) {
                return QDateTime();
            }

            second = secondParts.first().toInt(&ok);
            if (!ok) {
                return QDateTime();
            }

            if (secondParts.size() > 1) {
                millisecond = secondParts.last().toInt(&ok);
                if (!ok) {
                    return QDateTime();
                }
            }
        }

        QTime time(hour, minute, second, millisecond);
        if (!time.isValid())
            return QDateTime();

        if (parts.count() == 5)
            return QDateTime(date, time, Qt::LocalTime);

        QString tz = parts.at(5);
        if (!tz.startsWith(QLatin1String("GMT"), Qt::CaseInsensitive))
            return QDateTime();
        tz.remove(0, 3);
        if (!tz.isEmpty()) {
            int offset = fromOffsetString(tz, &ok);
            if (!ok)
                return QDateTime();
            return QDateTime(date, time, Qt::OffsetFromUTC, offset);
        } else {
            return QDateTime(date, time, Qt::UTC);
        }
    }
#endif //QT_NO_TEXTDATE
    }

    return QDateTime();
}

/*!
    \fn QDateTime::fromString(const QString &string, const QString &format)

    Returns the QDateTime represented by the \a string, using the \a
    format given, or an invalid datetime if the string cannot be parsed.

    These expressions may be used for the date part of the format string:

    \table
    \header \i Expression \i Output
    \row \i d \i the day as number without a leading zero (1 to 31)
    \row \i dd \i the day as number with a leading zero (01 to 31)
    \row \i ddd
            \i the abbreviated localized day name (e.g. 'Mon' to 'Sun').
            Uses QDate::shortDayName().
    \row \i dddd
            \i the long localized day name (e.g. 'Monday' to 'Sunday').
            Uses QDate::longDayName().
    \row \i M \i the month as number without a leading zero (1-12)
    \row \i MM \i the month as number with a leading zero (01-12)
    \row \i MMM
            \i the abbreviated localized month name (e.g. 'Jan' to 'Dec').
            Uses QDate::shortMonthName().
    \row \i MMMM
            \i the long localized month name (e.g. 'January' to 'December').
            Uses QDate::longMonthName().
    \row \i yy \i the year as two digit number (00-99)
    \row \i yyyy \i the year as four digit number
    \endtable

    \note Unlike the other version of this function, day and month names must
    be given in the user's local language. It is only possible to use the English
    names if the user's language is English.

    These expressions may be used for the time part of the format string:

    \table
    \header \i Expression \i Output
    \row \i h
            \i the hour without a leading zero (0 to 23 or 1 to 12 if AM/PM display)
    \row \i hh
            \i the hour with a leading zero (00 to 23 or 01 to 12 if AM/PM display)
    \row \i H
            \i the hour without a leading zero (0 to 23, even with AM/PM display)
    \row \i HH
            \i the hour with a leading zero (00 to 23, even with AM/PM display)
    \row \i m \i the minute without a leading zero (0 to 59)
    \row \i mm \i the minute with a leading zero (00 to 59)
    \row \i s \i the second without a leading zero (0 to 59)
    \row \i ss \i the second with a leading zero (00 to 59)
    \row \i z \i the milliseconds without leading zeroes (0 to 999)
    \row \i zzz \i the milliseconds with leading zeroes (000 to 999)
    \row \i AP or A
         \i interpret as an AM/PM time. \e AP must be either "AM" or "PM".
    \row \i ap or a
         \i Interpret as an AM/PM time. \e ap must be either "am" or "pm".
    \endtable

    All other input characters will be treated as text. Any sequence
    of characters that are enclosed in singlequotes will also be
    treated as text and not be used as an expression.

    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 12

    If the format is not satisfied an invalid QDateTime is returned.
    The expressions that don't have leading zeroes (d, M, h, m, s, z) will be
    greedy. This means that they will use two digits even if this will
    put them outside the range and/or leave too few digits for other
    sections.

    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 13

    This could have meant 1 January 00:30.00 but the M will grab
    two digits.

    For any field that is not represented in the format the following
    defaults are used:

    \table
    \header \i Field  \i Default value
    \row    \i Year   \i 1900
    \row    \i Month  \i 1 (January)
    \row    \i Day    \i 1
    \row    \i Hour   \i 0
    \row    \i Minute \i 0
    \row    \i Second \i 0
    \endtable

    For example:

    \snippet doc/src/snippets/code/src_corelib_tools_qdatetime.cpp 14

    \sa QDate::fromString() QTime::fromString() QDate::toString()
    QDateTime::toString() QTime::toString()
*/

QDateTime QDateTime::fromString(const QString &string, const QString &format)
{
#ifndef QT_BOOTSTRAPPED
    QTime time;
    QDate date;

    QDateTimeParser dt(QVariant::DateTime, QDateTimeParser::FromString);
    if (dt.parseFormat(format) && dt.fromString(string, &date, &time))
        return QDateTime(date, time);
#else
    Q_UNUSED(string);
    Q_UNUSED(format);
#endif
    return QDateTime(QDate(), QTime(-1, -1, -1));
}

#endif // QT_NO_DATESTRING
/*!
    \fn QDateTime QDateTime::toLocalTime() const

    Returns a datetime containing the date and time information in
    this datetime, but specified using the Qt::LocalTime definition.

    \sa toTimeSpec()
*/

/*!
    \fn QDateTime QDateTime::toUTC() const

    Returns a datetime containing the date and time information in
    this datetime, but specified using the Qt::UTC definition.

    \sa toTimeSpec()
*/

/*****************************************************************************
  Date/time stream functions
 *****************************************************************************/

#ifndef QT_NO_DATASTREAM
/*!
    \relates QDate

    Writes the \a date to stream \a out.

    \sa {Serializing Qt Data Types}
*/

QDataStream &operator<<(QDataStream &out, const QDate &date)
{
    return out << (qint64)(date.jd);
}

/*!
    \relates QDate

    Reads a date from stream \a in into the \a date.

    \sa {Serializing Qt Data Types}
*/

QDataStream &operator>>(QDataStream &in, QDate &date)
{
    qint64 jd;
    in >> jd;
    date.jd = jd;
    return in;
}

/*!
    \relates QTime

    Writes \a time to stream \a out.

    \sa {Serializing Qt Data Types}
*/

QDataStream &operator<<(QDataStream &out, const QTime &time)
{
    return out << quint32(time.mds);
}

/*!
    \relates QTime

    Reads a time from stream \a in into the given \a time.

    \sa {Serializing Qt Data Types}
*/

QDataStream &operator>>(QDataStream &in, QTime &time)
{
    quint32 ds;
    in >> ds;
    time.mds = int(ds);
    return in;
}

/*!
    \relates QDateTime

    Writes \a dateTime to the \a out stream.

    \sa {Serializing Qt Data Types}
*/
QDataStream &operator<<(QDataStream &out, const QDateTime &dateTime)
{
    QDate dt;
    QTime tm;
    if (out.version() == 13) {
        // This approach is wrong and should not be used again; it breaks
        // the guarantee that a deserialised local datetime is the same time
        // of day, regardless of which timezone it was serialised in.
        if (dateTime.isValid())
            dateTime.toUTC().d->getDateTime(&dt, &tm);
        else
            dateTime.d->getDateTime(&dt, &tm);
        out << dt << tm << (qint8)dateTime.timeSpec();
    } else {
        dateTime.d->getDateTime(&dt, &tm);
        out << dt << tm;
        if (out.version() >= 7) {
            switch (dateTime.d->m_spec) {
            case Qt::UTC:
                out << (qint8)QDateTimePrivate::UTC;
                break;
            case Qt::OffsetFromUTC:
                out << (qint8)QDateTimePrivate::OffsetFromUTC;
                break;
            case Qt::LocalTime:
                out << (qint8)QDateTimePrivate::LocalUnknown;
                break;
            }
        }
        if (out.version() >= 14
            && dateTime.d->m_spec == Qt::OffsetFromUTC) {
            out << qint32(dateTime.offsetFromUtc());
        }
    }
    return out;
}

/*!
    \relates QDateTime

    Reads a datetime from the stream \a in into \a dateTime.

    \sa {Serializing Qt Data Types}
*/

QDataStream &operator>>(QDataStream &in, QDateTime &dateTime)
{
    QDate dt;
    QTime tm;
    in >> dt >> tm;

    if (in.version() == 13) {
        qint8 ts = 0;
        in >> ts;
        // We incorrectly stored the datetime as UTC in 13 onwards.
        dateTime.setTimeSpec(Qt::UTC);
        dateTime.d->setDateTime(dt, tm);
        dateTime = dateTime.toTimeSpec(static_cast<Qt::TimeSpec>(ts));
    } else {
        qint8 ts = (qint8)QDateTimePrivate::LocalUnknown;
        if (in.version() >= 7)
            in >> ts;
        qint32 offset = 0;
        if (in.version() >= 14 && ts == qint8(QDateTimePrivate::OffsetFromUTC))
            in >> offset;
        switch ((QDateTimePrivate::Spec)ts) {
        case QDateTimePrivate::UTC:
            dateTime.d->m_spec = Qt::UTC;
            break;
        case QDateTimePrivate::OffsetFromUTC:
            dateTime.d->m_spec = Qt::OffsetFromUTC;
            break;
        case QDateTimePrivate::LocalUnknown:
        case QDateTimePrivate::LocalStandard:
        case QDateTimePrivate::LocalDST:
            dateTime.d->m_spec = Qt::LocalTime;
            break;
        }
        dateTime.d->m_offsetFromUtc = offset;
        dateTime.d->setDateTime(dt, tm);
    }

    return in;
}
#endif // QT_NO_DATASTREAM


/*!
    \fn QString QDate::monthName(int month)

    Use shortMonthName() instead.
*/

/*!
    \fn QString QDate::dayName(int weekday)

    Use shortDayName() instead.
*/

/*!
    \fn bool QDate::leapYear(int year)

    Use isLeapYear() instead.
*/

/*!
    \fn QDate QDate::currentDate(Qt::TimeSpec spec)

    If \a spec is Qt::LocalTime, use the currentDate() overload that
    takes no parameters instead; otherwise, use
    QDateTime::currentDateTime().

    \oldcode
        QDate localDate = QDate::currentDate(Qt::LocalTime);
        QDate utcDate = QDate::currentDate(Qt::UTC);
    \newcode
        QDate localDate = QDate::currentDate();
        QDate utcDate = QDateTime::currentDateTime().toUTC().date();
    \endcode

    \sa QDateTime::toUTC()
*/

/*!
    \fn QTime QTime::currentTime(Qt::TimeSpec specification)

    Returns the current time for the given \a specification.

    To replace uses of this function where the \a specification is Qt::LocalTime,
    use the currentDate() overload that takes no parameters instead; otherwise,
    use QDateTime::currentDateTime() and convert the result to a UTC measurement.

    \oldcode
        QTime localTime = QTime::currentTime(Qt::LocalTime);
        QTime utcTime = QTime::currentTime(Qt::UTC);
    \newcode
        QTime localTime = QTime::currentTime();
        QTime utcTime = QTimeTime::currentDateTime().toUTC().time();
    \endcode

    \sa QDateTime::toUTC()
*/

/*!
    \fn void QDateTime::setTime_t(uint secsSince1Jan1970UTC, Qt::TimeSpec spec)

    Use the single-argument overload of setTime_t() instead.
*/

/*!
    \fn QDateTime QDateTime::currentDateTime(Qt::TimeSpec spec)

    Use the currentDateTime() overload that takes no parameters
    instead.
*/

/*****************************************************************************
  Some static function used by QDate, QTime and QDateTime
*****************************************************************************/

#if !defined(QT_NO_DEBUG_STREAM) && !defined(QT_NO_DATESTRING)
QDebug operator<<(QDebug dbg, const QDate &date)
{
    dbg.nospace() << "QDate(" << date.toString() << ')';
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const QTime &time)
{
    dbg.nospace() << "QTime(" << time.toString() << ')';
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const QDateTime &date)
{
    dbg.nospace() << "QDateTime(" << date.toString() << ')';
    return dbg.space();
}
#endif

#ifndef QT_BOOTSTRAPPED

/*!
  \internal
  Gets the digit from a datetime. E.g.

  QDateTime var(QDate(2004, 02, 02));
  int digit = getDigit(var, Year);
  // digit = 2004
*/

int QDateTimeParser::getDigit(const QDateTime &t, int index) const
{
    if (index < 0 || index >= sectionNodes.size()) {
#ifndef QT_NO_DATESTRING
        qWarning("QDateTimeParser::getDigit() Internal error (%s %d)",
                 qPrintable(t.toString()), index);
#else
        qWarning("QDateTimeParser::getDigit() Internal error (%d)", index);
#endif
        return -1;
    }
    const SectionNode &node = sectionNodes.at(index);
    switch (node.type) {
    case Hour24Section: case Hour12Section: return t.time().hour();
    case MinuteSection: return t.time().minute();
    case SecondSection: return t.time().second();
    case MSecSection: return t.time().msec();
    case YearSection2Digits:
    case YearSection: return t.date().year();
    case MonthSection: return t.date().month();
    case DaySection: return t.date().day();
    case DayOfWeekSection: return t.date().day();
    case AmPmSection: return t.time().hour() > 11 ? 1 : 0;

    default: break;
    }

#ifndef QT_NO_DATESTRING
    qWarning("QDateTimeParser::getDigit() Internal error 2 (%s %d)",
             qPrintable(t.toString()), index);
#else
    qWarning("QDateTimeParser::getDigit() Internal error 2 (%d)", index);
#endif
    return -1;
}

/*!
  \internal
  Sets a digit in a datetime. E.g.

  QDateTime var(QDate(2004, 02, 02));
  int digit = getDigit(var, Year);
  // digit = 2004
  setDigit(&var, Year, 2005);
  digit = getDigit(var, Year);
  // digit = 2005
*/

bool QDateTimeParser::setDigit(QDateTime &v, int index, int newVal) const
{
    if (index < 0 || index >= sectionNodes.size()) {
#ifndef QT_NO_DATESTRING
        qWarning("QDateTimeParser::setDigit() Internal error (%s %d %d)",
                 qPrintable(v.toString()), index, newVal);
#else
        qWarning("QDateTimeParser::setDigit() Internal error (%d %d)", index, newVal);
#endif
        return false;
    }
    const SectionNode &node = sectionNodes.at(index);

    int year, month, day, hour, minute, second, msec;
    year = v.date().year();
    month = v.date().month();
    day = v.date().day();
    hour = v.time().hour();
    minute = v.time().minute();
    second = v.time().second();
    msec = v.time().msec();

    switch (node.type) {
    case Hour24Section: case Hour12Section: hour = newVal; break;
    case MinuteSection: minute = newVal; break;
    case SecondSection: second = newVal; break;
    case MSecSection: msec = newVal; break;
    case YearSection2Digits:
    case YearSection: year = newVal; break;
    case MonthSection: month = newVal; break;
    case DaySection:
    case DayOfWeekSection:
        if (newVal > 31) {
            // have to keep legacy behavior. setting the
            // date to 32 should return false. Setting it
            // to 31 for february should return true
            return false;
        }
        day = newVal;
        break;
    case AmPmSection: hour = (newVal == 0 ? hour % 12 : (hour % 12) + 12); break;
    default:
        qWarning("QDateTimeParser::setDigit() Internal error (%s)",
                 qPrintable(sectionName(node.type)));
        break;
    }

    if (!(node.type & (DaySection|DayOfWeekSection))) {
        if (day < cachedDay)
            day = cachedDay;
        const int max = QDate(year, month, 1).daysInMonth();
        if (day > max) {
            day = max;
        }
    }
    if (QDate::isValid(year, month, day) && QTime::isValid(hour, minute, second, msec)) {
        v = QDateTime(QDate(year, month, day), QTime(hour, minute, second, msec), spec);
        return true;
    }
    return false;
}



/*!
  \

  Returns the absolute maximum for a section
*/

int QDateTimeParser::absoluteMax(int s, const QDateTime &cur) const
{
    const SectionNode &sn = sectionNode(s);
    switch (sn.type) {
    case Hour24Section:
    case Hour12Section: return 23; // this is special-cased in
                                   // parseSection. We want it to be
                                   // 23 for the stepBy case.
    case MinuteSection:
    case SecondSection: return 59;
    case MSecSection: return 999;
    case YearSection2Digits:
    case YearSection: return 9999; // sectionMaxSize will prevent
                                   // people from typing in a larger
                                   // number in count == 2 sections.
                                   // stepBy() will work on real years anyway
    case MonthSection: return 12;
    case DaySection:
    case DayOfWeekSection: return cur.isValid() ? cur.date().daysInMonth() : 31;
    case AmPmSection: return 1;
    default: break;
    }
    qWarning("QDateTimeParser::absoluteMax() Internal error (%s)",
             qPrintable(sectionName(sn.type)));
    return -1;
}

/*!
  \internal

  Returns the absolute minimum for a section
*/

int QDateTimeParser::absoluteMin(int s) const
{
    const SectionNode &sn = sectionNode(s);
    switch (sn.type) {
    case Hour24Section:
    case Hour12Section:
    case MinuteSection:
    case SecondSection:
    case MSecSection:
    case YearSection2Digits:
    case YearSection: return 0;
    case MonthSection:
    case DaySection:
    case DayOfWeekSection: return 1;
    case AmPmSection: return 0;
    default: break;
    }
    qWarning("QDateTimeParser::absoluteMin() Internal error (%s, %0x)",
             qPrintable(sectionName(sn.type)), sn.type);
    return -1;
}

/*!
  \internal

  Returns the sectionNode for the Section \a s.
*/

const QDateTimeParser::SectionNode &QDateTimeParser::sectionNode(int sectionIndex) const
{
    if (sectionIndex < 0) {
        switch (sectionIndex) {
        case FirstSectionIndex:
            return first;
        case LastSectionIndex:
            return last;
        case NoSectionIndex:
            return none;
        }
    } else if (sectionIndex < sectionNodes.size()) {
        return sectionNodes.at(sectionIndex);
    }

    qWarning("QDateTimeParser::sectionNode() Internal error (%d)",
             sectionIndex);
    return none;
}

QDateTimeParser::Section QDateTimeParser::sectionType(int sectionIndex) const
{
    return sectionNode(sectionIndex).type;
}


/*!
  \internal

  Returns the starting position for section \a s.
*/

int QDateTimeParser::sectionPos(int sectionIndex) const
{
    return sectionPos(sectionNode(sectionIndex));
}

int QDateTimeParser::sectionPos(const SectionNode &sn) const
{
    switch (sn.type) {
    case FirstSection: return 0;
    case LastSection: return displayText().size() - 1;
    default: break;
    }
    if (sn.pos == -1) {
        qWarning("QDateTimeParser::sectionPos Internal error (%s)", qPrintable(sectionName(sn.type)));
        return -1;
    }
    return sn.pos;
}


/*!
  \internal helper function for parseFormat. removes quotes that are
  not escaped and removes the escaping on those that are escaped

*/

static QString unquote(const QString &str)
{
    const QChar quote(QLatin1Char('\''));
    const QChar slash(QLatin1Char('\\'));
    const QChar zero(QLatin1Char('0'));
    QString ret;
    QChar status(zero);
    const int max = str.size();
    for (int i=0; i<max; ++i) {
        if (str.at(i) == quote) {
            if (status != quote) {
                status = quote;
            } else if (!ret.isEmpty() && str.at(i - 1) == slash) {
                ret[ret.size() - 1] = quote;
            } else {
                status = zero;
            }
        } else {
            ret += str.at(i);
        }
    }
    return ret;
}
/*!
  \internal

  Parses the format \a newFormat. If successful, returns true and
  sets up the format. Else keeps the old format and returns false.

*/

static inline int countRepeat(const QString &str, int index, int maxCount)
{
    int count = 1;
    const QChar ch(str.at(index));
    const int max = std::min(index + maxCount, str.size());
    while (index + count < max && str.at(index + count) == ch) {
        ++count;
    }
    return count;
}

static inline void appendSeparator(QStringList *list, const QString &string, int from, int size, int lastQuote)
{
    QString str(string.mid(from, size));
    if (lastQuote >= from)
        str = unquote(str);
    list->append(str);
}


bool QDateTimeParser::parseFormat(const QString &newFormat)
{
    const QLatin1Char quote('\'');
    const QLatin1Char slash('\\');
    const QLatin1Char zero('0');
    if (newFormat == displayFormat && !newFormat.isEmpty()) {
        return true;
    }

    QDTPDEBUGN("parseFormat: %s", newFormat.toLatin1().constData());

    QVector<SectionNode> newSectionNodes;
    Sections newDisplay = nullptr;
    QStringList newSeparators;
    int i, index = 0;
    int add = 0;
    QChar status(zero);
    const int max = newFormat.size();
    int lastQuote = -1;
    for (i = 0; i<max; ++i) {
        if (newFormat.at(i) == quote) {
            lastQuote = i;
            ++add;
            if (status != quote) {
                status = quote;
            } else if (newFormat.at(i - 1) != slash) {
                status = zero;
            }
        } else if (status != quote) {
            const char sect = newFormat.at(i).toLatin1();
            switch (sect) {
            case 'H':
            case 'h':
                if (parserType != QVariant::Date) {
                    const Section hour = (sect == 'h') ? Hour12Section : Hour24Section;
                    const SectionNode sn = { hour, i - add, countRepeat(newFormat, i, 2), 0 };
                    newSectionNodes.append(sn);
                    appendSeparator(&newSeparators, newFormat, index, i - index, lastQuote);
                    i += sn.count - 1;
                    index = i + 1;
                    newDisplay |= hour;
                }
                break;
            case 'm':
                if (parserType != QVariant::Date) {
                    const SectionNode sn = { MinuteSection, i - add, countRepeat(newFormat, i, 2), 0 };
                    newSectionNodes.append(sn);
                    appendSeparator(&newSeparators, newFormat, index, i - index, lastQuote);
                    i += sn.count - 1;
                    index = i + 1;
                    newDisplay |= MinuteSection;
                }
                break;
            case 's':
                if (parserType != QVariant::Date) {
                    const SectionNode sn = { SecondSection, i - add, countRepeat(newFormat, i, 2), 0 };
                    newSectionNodes.append(sn);
                    appendSeparator(&newSeparators, newFormat, index, i - index, lastQuote);
                    i += sn.count - 1;
                    index = i + 1;
                    newDisplay |= SecondSection;
                }
                break;

            case 'z':
                if (parserType != QVariant::Date) {
                    const SectionNode sn = { MSecSection, i - add, countRepeat(newFormat, i, 3) < 3 ? 1 : 3, 0 };
                    newSectionNodes.append(sn);
                    appendSeparator(&newSeparators, newFormat, index, i - index, lastQuote);
                    i += sn.count - 1;
                    index = i + 1;
                    newDisplay |= MSecSection;
                }
                break;
            case 'A':
            case 'a':
                if (parserType != QVariant::Date) {
                    const bool cap = (sect == 'A');
                    const SectionNode sn = { AmPmSection, i - add, (cap ? 1 : 0), 0 };
                    newSectionNodes.append(sn);
                    appendSeparator(&newSeparators, newFormat, index, i - index, lastQuote);
                    newDisplay |= AmPmSection;
                    if (i + 1 < newFormat.size()
                        && newFormat.at(i+1) == (cap ? QLatin1Char('P') : QLatin1Char('p'))) {
                        ++i;
                    }
                    index = i + 1;
                }
                break;
            case 'y':
                if (parserType != QVariant::Time) {
                    const int repeat = countRepeat(newFormat, i, 4);
                    if (repeat >= 2) {
                        const SectionNode sn = { repeat == 4 ? YearSection : YearSection2Digits,
                                                 i - add, repeat == 4 ? 4 : 2, 0 };
                        newSectionNodes.append(sn);
                        appendSeparator(&newSeparators, newFormat, index, i - index, lastQuote);
                        i += sn.count - 1;
                        index = i + 1;
                        newDisplay |= sn.type;
                    }
                }
                break;
            case 'M':
                if (parserType != QVariant::Time) {
                    const SectionNode sn = { MonthSection, i - add, countRepeat(newFormat, i, 4), 0 };
                    newSectionNodes.append(sn);
                    newSeparators.append(unquote(newFormat.mid(index, i - index)));
                    i += sn.count - 1;
                    index = i + 1;
                    newDisplay |= MonthSection;
                }
                break;
            case 'd':
                if (parserType != QVariant::Time) {
                    const int repeat = countRepeat(newFormat, i, 4);
                    const SectionNode sn = { repeat >= 3 ? DayOfWeekSection : DaySection, i - add, repeat, 0 };
                    newSectionNodes.append(sn);
                    appendSeparator(&newSeparators, newFormat, index, i - index, lastQuote);
                    i += sn.count - 1;
                    index = i + 1;
                    newDisplay |= sn.type;
                }
                break;

            default:
                break;
            }
        }
    }
    if (newSectionNodes.isEmpty() && context == DateTimeEdit) {
        return false;
    }

    if ((newDisplay & (AmPmSection|Hour12Section)) == Hour12Section) {
        const int max = newSectionNodes.size();
        for (int i=0; i<max; ++i) {
            SectionNode &node = newSectionNodes[i];
            if (node.type == Hour12Section)
                node.type = Hour24Section;
        }
    }

    if (index < newFormat.size()) {
        appendSeparator(&newSeparators, newFormat, index, index - max, lastQuote);
    } else {
        newSeparators.append(QString());
    }

    displayFormat = newFormat;
    separators = newSeparators;
    sectionNodes = newSectionNodes;
    display = newDisplay;
    last.pos = -1;

//     for (int i=0; i<sectionNodes.size(); ++i) {
//         QDTPDEBUG << sectionName(sectionNodes.at(i).type) << sectionNodes.at(i).count;
//     }

    QDTPDEBUG << newFormat << displayFormat;
    QDTPDEBUGN("separators:\n'%s'", separators.join(QLatin1String("\n")).toLatin1().constData());

    return true;
}

/*!
  \internal

  Returns the size of section \a s.
*/

int QDateTimeParser::sectionSize(int sectionIndex) const
{
    if (sectionIndex < 0)
        return 0;

    if (sectionIndex >= sectionNodes.size()) {
        qWarning("QDateTimeParser::sectionSize Internal error (%d)", sectionIndex);
        return -1;
    }

    if (sectionIndex == sectionNodes.size() - 1) {
        // In some cases there is a difference between displayText() and text.
        // e.g. when text is 2000/01/31 and displayText() is "2000/2/31" - text
        // is the previous value and displayText() is the new value.
        // The size difference is always due to leading zeroes.
        int sizeAdjustment = 0;
        if (displayText().size() != text.size()) {
            // Any zeroes added before this section will affect our size.
            int preceedingZeroesAdded = 0;
            if (sectionNodes.size() > 1 && context == DateTimeEdit) {
                for (QVector<SectionNode>::ConstIterator sectionIt = sectionNodes.constBegin();
                    sectionIt != sectionNodes.constBegin() + sectionIndex; ++sectionIt) {
                    preceedingZeroesAdded += sectionIt->zeroesAdded;
                }
            }
            sizeAdjustment = preceedingZeroesAdded;
        }

        return displayText().size() + sizeAdjustment - sectionPos(sectionIndex) - separators.last().size();
    } else {
        return sectionPos(sectionIndex + 1) - sectionPos(sectionIndex)
            - separators.at(sectionIndex + 1).size();
    }
}


int QDateTimeParser::sectionMaxSize(Section s, int count) const
{
#ifndef QT_NO_TEXTDATE
    int mcount = 12;
#endif

    switch (s) {
    case FirstSection:
    case NoSection:
    case LastSection: return 0;

    case AmPmSection: {
        const int lowerMax = std::min(getAmPmText(AmText, LowerCase).size(),
                                  getAmPmText(PmText, LowerCase).size());
        const int upperMax = std::min(getAmPmText(AmText, UpperCase).size(),
                                  getAmPmText(PmText, UpperCase).size());
        return std::min(4, std::min(lowerMax, upperMax));
    }

    case Hour24Section:
    case Hour12Section:
    case MinuteSection:
    case SecondSection:
    case DaySection: return 2;
    case DayOfWeekSection:
#ifdef QT_NO_TEXTDATE
        return 2;
#else
        mcount = 7;
        [[fallthrough]];
#endif
    case MonthSection:
        if (count <= 2)
            return 2;

#ifdef QT_NO_TEXTDATE
        return 2;
#else
        {
            int ret = 0;
            const QLocale l = locale();
            for (int i=1; i<=mcount; ++i) {
                const QString str = (s == MonthSection
                                     ? l.monthName(i, count == 4 ? QLocale::LongFormat : QLocale::ShortFormat)
                                     : l.dayName(i, count == 4 ? QLocale::LongFormat : QLocale::ShortFormat));
                ret = std::max(str.size(), ret);
            }
            return ret;
        }
#endif
    case MSecSection: return 3;
    case YearSection: return 4;
    case YearSection2Digits: return 2;

    case CalendarPopupSection:
    case Internal:
    case TimeSectionMask:
    case DateSectionMask:
        qWarning("QDateTimeParser::sectionMaxSize: Invalid section %s",
                 sectionName(s).toLatin1().constData());

    case NoSectionIndex:
    case FirstSectionIndex:
    case LastSectionIndex:
    case CalendarPopupIndex:
        // these cases can't happen
        break;
    }
    return -1;
}


int QDateTimeParser::sectionMaxSize(int index) const
{
    const SectionNode &sn = sectionNode(index);
    return sectionMaxSize(sn.type, sn.count);
}

/*!
  \internal

  Returns the text of section \a s. This function operates on the
  arg text rather than edit->text().
*/


QString QDateTimeParser::sectionText(const QString &text, int sectionIndex, int index) const
{
    const SectionNode &sn = sectionNode(sectionIndex);
    switch (sn.type) {
    case NoSectionIndex:
    case FirstSectionIndex:
    case LastSectionIndex:
        return QString();
    default: break;
    }

    return text.mid(index, sectionSize(sectionIndex));
}

QString QDateTimeParser::sectionText(int sectionIndex) const
{
    const SectionNode &sn = sectionNode(sectionIndex);
    switch (sn.type) {
    case NoSectionIndex:
    case FirstSectionIndex:
    case LastSectionIndex:
        return QString();
    default: break;
    }

    return displayText().mid(sn.pos, sectionSize(sectionIndex));
}


#ifndef QT_NO_TEXTDATE
/*!
  \internal:skipToNextSection

  Parses the part of \a text that corresponds to \a s and returns
  the value of that field. Sets *stateptr to the right state if
  stateptr != 0.
*/

int QDateTimeParser::parseSection(const QDateTime &currentValue, int sectionIndex,
                                  QString &text, int &cursorPosition, int index,
                                  State &state, int *usedptr) const
{
    state = Invalid;
    int num = 0;
    const SectionNode &sn = sectionNode(sectionIndex);
    if ((sn.type & Internal) == Internal) {
        qWarning("QDateTimeParser::parseSection Internal error (%s %d)",
                 qPrintable(sectionName(sn.type)), sectionIndex);
        return -1;
    }

    const int sectionmaxsize = sectionMaxSize(sectionIndex);
    QString sectiontext = text.mid(index, sectionmaxsize);
    int sectiontextSize = sectiontext.size();

    QDTPDEBUG << "sectionValue for" << sectionName(sn.type)
              << "with text" << text << "and st" << sectiontext
              << text.mid(index, sectionmaxsize)
              << index;

    int used = 0;
    switch (sn.type) {
    case AmPmSection: {
        const int ampm = findAmPm(sectiontext, sectionIndex, &used);
        switch (ampm) {
        case AM: // sectiontext == AM
        case PM: // sectiontext == PM
            num = ampm;
            state = Acceptable;
            break;
        case PossibleAM: // sectiontext => AM
        case PossiblePM: // sectiontext => PM
            num = ampm - 2;
            state = Intermediate;
            break;
        case PossibleBoth: // sectiontext => AM|PM
            num = 0;
            state = Intermediate;
            break;
        case Neither:
            state = Invalid;
            QDTPDEBUG << "invalid because findAmPm(" << sectiontext << ") returned -1";
            break;
        default:
            QDTPDEBUGN("This should never happen (findAmPm returned %d)", ampm);
            break;
        }
        if (state != Invalid) {
            QString str = text;
            text.replace(index, used, sectiontext.left(used));
        }
        break; }
    case MonthSection:
    case DayOfWeekSection:
        if (sn.count >= 3) {
            if (sn.type == MonthSection) {
                int min = 1;
                const QDate minDate = getMinimum().date();
                if (currentValue.date().year() == minDate.year()) {
                    min = minDate.month();
                }
                num = findMonth(sectiontext.toLower(), min, sectionIndex, &sectiontext, &used);
            } else {
                num = findDay(sectiontext.toLower(), 1, sectionIndex, &sectiontext, &used);
            }

            if (num != -1) {
                state = (used == sectiontext.size() ? Acceptable : Intermediate);
                QString str = text;
                text.replace(index, used, sectiontext.left(used));
            } else {
                state = Intermediate;
            }
            break; }
        // fall through
    case DaySection:
    case YearSection:
    case YearSection2Digits:
    case Hour12Section:
    case Hour24Section:
    case MinuteSection:
    case SecondSection:
    case MSecSection: {
        if (sectiontextSize == 0) {
            num = 0;
            used = 0;
            state = Intermediate;
        } else {
            const int absMax = absoluteMax(sectionIndex);
            QLocale loc;
            bool ok = true;
            int last = -1;
            used = -1;

            QString digitsStr(sectiontext);
            for (int i = 0; i < sectiontextSize; ++i) {
                if (digitsStr.at(i).isSpace()) {
                    sectiontextSize = i;
                    break;
                }
            }

            const int max = std::min(sectionmaxsize, sectiontextSize);
            for (int digits = max; digits >= 1; --digits) {
                digitsStr.truncate(digits);
                int tmp = (int)loc.toUInt(digitsStr, &ok);
                if (ok && sn.type == Hour12Section) {
                    if (tmp > 12) {
                        tmp = -1;
                        ok = false;
                    } else if (tmp == 12) {
                        tmp = 0;
                    }
                }
                if (ok && tmp <= absMax) {
                    QDTPDEBUG << sectiontext.left(digits) << tmp << digits;
                    last = tmp;
                    used = digits;
                    break;
                }
            }

            if (last == -1) {
                QChar first(sectiontext.at(0));
                if (separators.at(sectionIndex + 1).startsWith(first)) {
                    used = 0;
                    state = Intermediate;
                } else {
                    state = Invalid;
                    QDTPDEBUG << "invalid because" << sectiontext << "can't become a uint" << last << ok;
                }
            } else {
                num += last;
                const FieldInfo fi = fieldInfo(sectionIndex);
                const bool done = (used == sectionmaxsize);
                if (!done && fi & Fraction) { // typing 2 in a zzz field should be .200, not .002
                    for (int i=used; i<sectionmaxsize; ++i) {
                        num *= 10;
                    }
                }
                const int absMin = absoluteMin(sectionIndex);
                if (num < absMin) {
                    state = done ? Invalid : Intermediate;
                    if (done)
                        QDTPDEBUG << "invalid because" << num << "is less than absoluteMin" << absMin;
                } else if (num > absMax) {
                    state = Intermediate;
                } else if (!done && (fi & (FixedWidth|Numeric)) == (FixedWidth|Numeric)) {
                    if (skipToNextSection(sectionIndex, currentValue, digitsStr)) {
                        state = Acceptable;
                        const int missingZeroes = sectionmaxsize - digitsStr.size();
                        text.insert(index, QString().fill(QLatin1Char('0'), missingZeroes));
                        used = sectionmaxsize;
                        cursorPosition += missingZeroes;
                        ++(const_cast<QDateTimeParser*>(this)->sectionNodes[sectionIndex].zeroesAdded);
                    } else {
                        state = Intermediate;;
                    }
                } else {
                    state = Acceptable;
                }
            }
        }
        break; }
    default:
        qWarning("QDateTimeParser::parseSection Internal error (%s %d)",
                 qPrintable(sectionName(sn.type)), sectionIndex);
        return -1;
    }

    if (usedptr)
        *usedptr = used;

    return (state != Invalid ? num : -1);
}
#endif // QT_NO_TEXTDATE

#ifndef QT_NO_DATESTRING
/*!
  \internal
*/

QDateTimeParser::StateNode QDateTimeParser::parse(QString &input, int &cursorPosition,
                                                  const QDateTime &currentValue, bool fixup) const
{
    const QDateTime minimum = getMinimum();
    const QDateTime maximum = getMaximum();

    State state = Acceptable;

    QDateTime newCurrentValue;
    int pos = 0;
    bool conflicts = false;
    const int sectionNodesCount = sectionNodes.size();

    QDTPDEBUG << "parse" << input;
    {
        int  hour12, hour, minute, second, msec, ampm, dayofweek, year2digits;
        ParsedDate pd = getDateFromJulianDay(currentValue.date().toJulianDay());
        year2digits = pd.year % 100;
        hour = currentValue.time().hour();
        hour12 = -1;
        minute = currentValue.time().minute();
        second = currentValue.time().second();
        msec = currentValue.time().msec();
        dayofweek = currentValue.date().dayOfWeek();

        ampm = -1;
        Sections isSet = NoSection;
        int num;
        State tmpstate;

        for (int index=0; state != Invalid && index<sectionNodesCount; ++index) {
            if (QStringRef(&input, pos, separators.at(index).size()) != separators.at(index)) {
                QDTPDEBUG << "invalid because" << input.mid(pos, separators.at(index).size())
                          << "!=" << separators.at(index)
                          << index << pos << currentSectionIndex;
                state = Invalid;
                goto end;
            }
            pos += separators.at(index).size();
            sectionNodes[index].pos = pos;
            int *current = nullptr;
            const SectionNode sn = sectionNodes.at(index);
            int used;

            num = parseSection(currentValue, index, input, cursorPosition, pos, tmpstate, &used);
            QDTPDEBUG << "sectionValue" << sectionName(sectionType(index)) << input
                      << "pos" << pos << "used" << used << stateName(tmpstate);
            if (fixup && tmpstate == Intermediate && used < sn.count) {
                const FieldInfo fi = fieldInfo(index);
                if ((fi & (Numeric|FixedWidth)) == (Numeric|FixedWidth)) {
                    const QString newText = QString::fromLatin1("%1").arg(num, sn.count, 10, QLatin1Char('0'));
                    input.replace(pos, used, newText);
                    used = sn.count;
                }
            }
            pos += std::max(0, used);

            state = std::min<State>(state, tmpstate);
            if (state == Intermediate && context == FromString) {
                state = Invalid;
                break;
            }

            QDTPDEBUG << index << sectionName(sectionType(index)) << "is set to"
                      << pos << "state is" << stateName(state);


            if (state != Invalid) {
                switch (sn.type) {
                case Hour24Section: current = &hour; break;
                case Hour12Section: current = &hour12; break;
                case MinuteSection: current = &minute; break;
                case SecondSection: current = &second; break;
                case MSecSection: current = &msec; break;
                case YearSection: current = &pd.year; break;
                case YearSection2Digits: current = &year2digits; break;
                case MonthSection: current = &pd.month; break;
                case DayOfWeekSection: current = &dayofweek; break;
                case DaySection: current = &pd.day; num = std::max<int>(1, num); break;
                case AmPmSection: current = &ampm; break;
                default:
                    qWarning("QDateTimeParser::parse Internal error (%s)",
                             qPrintable(sectionName(sn.type)));
                    break;
                }
                if (!current) {
                    qWarning("QDateTimeParser::parse Internal error 2");
                    return StateNode();
                }
                if (isSet & sn.type && *current != num) {
                    QDTPDEBUG << "CONFLICT " << sectionName(sn.type) << *current << num;
                    conflicts = true;
                    if (index != currentSectionIndex || num == -1) {
                        continue;
                    }
                }
                if (num != -1)
                    *current = num;
                isSet |= sn.type;
            }
        }

        if (state != Invalid && QStringRef(&input, pos, input.size() - pos) != separators.last()) {
            QDTPDEBUG << "invalid because" << input.mid(pos)
                      << "!=" << separators.last() << pos;
            state = Invalid;
        }

        if (state != Invalid) {
            if (parserType != QVariant::Time) {
                if (pd.year % 100 != year2digits) {
                    switch (isSet & (YearSection2Digits|YearSection)) {
                    case YearSection2Digits:
                        pd.year = (pd.year / 100) * 100;
                        pd.year += year2digits;
                        break;
                    case ((uint)YearSection2Digits|(uint)YearSection): {
                        conflicts = true;
                        const SectionNode &sn = sectionNode(currentSectionIndex);
                        if (sn.type == YearSection2Digits) {
                            pd.year = (pd.year / 100) * 100;
                            pd.year += year2digits;
                        }
                        break; }
                    default:
                        break;
                    }
                }

                const QDate date(pd.year, pd.month, pd.day);
                const int diff = dayofweek - date.dayOfWeek();
                if (diff != 0 && state == Acceptable && isSet & DayOfWeekSection) {
                    conflicts = isSet & DaySection;
                    const SectionNode &sn = sectionNode(currentSectionIndex);
                    if (sn.type == DayOfWeekSection || currentSectionIndex == -1) {
                        // dayofweek should be preferred
                        pd.day += diff;
                        if (pd.day <= 0) {
                            pd.day += 7;
                        } else if (pd.day > date.daysInMonth()) {
                            pd.day -= 7;
                        }
                        QDTPDEBUG << pd.year << pd.month << pd.day << dayofweek
                                  << diff << QDate(pd.year, pd.month, pd.day).dayOfWeek();
                    }
                }
                bool needfixday = false;
                if (sectionType(currentSectionIndex) & (DaySection|DayOfWeekSection)) {
                    cachedDay = pd.day;
                } else if (cachedDay > pd.day) {
                    pd.day = cachedDay;
                    needfixday = true;
                }

                if (!QDate::isValid(pd.year, pd.month, pd.day)) {
                    if (pd.day < 32) {
                        cachedDay = pd.day;
                    }
                    if (pd.day > 28 && QDate::isValid(pd.year, pd.month, 1)) {
                        needfixday = true;
                    }
                }
                if (needfixday) {
                    if (context == FromString) {
                        state = Invalid;
                        goto end;
                    }
                    if (state == Acceptable && fixday) {
                        pd.day = std::min<int>(pd.day, QDate(pd.year, pd.month, 1).daysInMonth());

                        const QLocale loc = locale();
                        for (int i=0; i<sectionNodesCount; ++i) {
                            if (sectionType(i) & (DaySection|DayOfWeekSection)) {
                                input.replace(sectionPos(i), sectionSize(i), loc.toString(pd.day));
                            }
                        }
                    } else {
                        state = std::min(Intermediate, state);
                    }
                }
            }

            if (parserType != QVariant::Date) {
                if (isSet & Hour12Section) {
                    const bool hasHour = isSet & Hour24Section;
                    if (ampm == -1) {
                        if (hasHour) {
                            ampm = (hour < 12 ? 0 : 1);
                        } else {
                            ampm = 0; // no way to tell if this is am or pm so I assume am
                        }
                    }
                    hour12 = (ampm == 0 ? hour12 % 12 : (hour12 % 12) + 12);
                    if (!hasHour) {
                        hour = hour12;
                    } else if (hour != hour12) {
                        conflicts = true;
                    }
                } else if (ampm != -1) {
                    if (!(isSet & (Hour24Section))) {
                        hour = (12 * ampm); // special case. Only ap section
                    } else if ((ampm == 0) != (hour < 12)) {
                        conflicts = true;
                    }
                }

            }

            newCurrentValue = QDateTime(QDate(pd.year, pd.month, pd.day), QTime(hour, minute, second, msec), spec);
            QDTPDEBUG << pd.year << pd.month << pd.day << hour << minute << second << msec;
        }
        QDTPDEBUGN("'%s' => '%s'(%s)", input.toLatin1().constData(),
                   newCurrentValue.toString(QLatin1String("yyyy/MM/dd hh:mm:ss.zzz")).toLatin1().constData(),
                   stateName(state).toLatin1().constData());
    }
end:
    if (newCurrentValue.isValid()) {
        if (context != FromString && state != Invalid && newCurrentValue < minimum) {
            const QLatin1Char space(' ');
            if (newCurrentValue >= minimum)
                qWarning("QDateTimeParser::parse Internal error 3 (%s %s)",
                         qPrintable(newCurrentValue.toString()), qPrintable(minimum.toString()));

            bool done = false;
            state = Invalid;
            for (int i=0; i<sectionNodesCount && !done; ++i) {
                const SectionNode &sn = sectionNodes.at(i);
                QString t = sectionText(input, i, sn.pos).toLower();
                if ((t.size() < sectionMaxSize(i) && (((int)fieldInfo(i) & (FixedWidth|Numeric)) != Numeric))
                    || t.contains(space)) {
                    switch (sn.type) {
                    case AmPmSection:
                        switch (findAmPm(t, i)) {
                        case AM:
                        case PM:
                            state = Acceptable;
                            done = true;
                            break;
                        case Neither:
                            state = Invalid;
                            done = true;
                            break;
                        case PossibleAM:
                        case PossiblePM:
                        case PossibleBoth: {
                            const QDateTime copy(newCurrentValue.addSecs(12 * 60 * 60));
                            if (copy >= minimum && copy <= maximum) {
                                state = Intermediate;
                                done = true;
                            }
                            break; }
                        }
                        // fall through
                    case MonthSection:
                        if (sn.count >= 3) {
                            int tmp = newCurrentValue.date().month();
                            // I know the first possible month makes the date too early
                            while ((tmp = findMonth(t, tmp + 1, i)) != -1) {
                                const QDateTime copy(newCurrentValue.addMonths(tmp - newCurrentValue.date().month()));
                                if (copy >= minimum && copy <= maximum)
                                    break; // break out of while
                            }
                            if (tmp == -1) {
                                break;
                            }
                            state = Intermediate;
                            done = true;
                            break;
                        }
                        // fallthrough
                    default: {
                        int toMin;
                        int toMax;

                        if (sn.type & TimeSectionMask) {
                            if (newCurrentValue.daysTo(minimum) != 0) {
                                break;
                            }
                            toMin = newCurrentValue.time().msecsTo(minimum.time());
                            if (newCurrentValue.daysTo(maximum) > 0) {
                                toMax = -1; // can't get to max
                            } else {
                                toMax = newCurrentValue.time().msecsTo(maximum.time());
                            }
                        } else {
                            toMin = newCurrentValue.daysTo(minimum);
                            toMax = newCurrentValue.daysTo(maximum);
                        }
                        const int maxChange = QDateTimeParser::maxChange(i);
                        if (toMin > maxChange) {
                            QDTPDEBUG << "invalid because toMin > maxChange" << toMin
                                      << maxChange << t << newCurrentValue << minimum;
                            state = Invalid;
                            done = true;
                            break;
                        } else if (toMax > maxChange) {
                            toMax = -1; // can't get to max
                        }

                        const int min = getDigit(minimum, i);
                        if (min == -1) {
                            qWarning("QDateTimeParser::parse Internal error 4 (%s)",
                                     qPrintable(sectionName(sn.type)));
                            state = Invalid;
                            done = true;
                            break;
                        }

                        int max = toMax != -1 ? getDigit(maximum, i) : absoluteMax(i, newCurrentValue);
                        int pos = cursorPosition - sn.pos;
                        if (pos < 0 || pos >= t.size())
                            pos = -1;
                        if (!potentialValue(t.simplified(), min, max, i, newCurrentValue, pos)) {
                            QDTPDEBUG << "invalid because potentialValue(" << t.simplified() << min << max
                                      << sectionName(sn.type) << "returned" << toMax << toMin << pos;
                            state = Invalid;
                            done = true;
                            break;
                        }
                        state = Intermediate;
                        done = true;
                        break; }
                    }
                }
            }
        } else {
            if (context == FromString) {
                // optimization
                Q_ASSERT(getMaximum().date().toJulianDay() == 4642999);
                if (newCurrentValue.date().toJulianDay() > 4642999)
                    state = Invalid;
            } else {
                if (newCurrentValue > getMaximum())
                    state = Invalid;
            }

            QDTPDEBUG << "not checking intermediate because newCurrentValue is" << newCurrentValue << getMinimum() << getMaximum();
        }
    }
    StateNode node;
    node.input = input;
    node.state = state;
    node.conflicts = conflicts;
    node.value = newCurrentValue.toTimeSpec(spec);
    text = input;
    return node;
}
#endif // QT_NO_DATESTRING

#ifndef QT_NO_TEXTDATE
/*!
  \internal finds the first possible monthname that \a str1 can
  match. Starting from \a index; str should already by lowered
*/

int QDateTimeParser::findMonth(const QString &str1, int startMonth, int sectionIndex,
                               QString *usedMonth, int *used) const
{
    int bestMatch = -1;
    int bestCount = 0;
    if (!str1.isEmpty()) {
        const SectionNode &sn = sectionNode(sectionIndex);
        if (sn.type != MonthSection) {
            qWarning("QDateTimeParser::findMonth Internal error");
            return -1;
        }

        QLocale::FormatType type = sn.count == 3 ? QLocale::ShortFormat : QLocale::LongFormat;
        QLocale l = locale();

        for (int month=startMonth; month<=12; ++month) {
            QString str2 = l.monthName(month, type).toLower();

            if (str1.startsWith(str2)) {
                if (used) {
                    QDTPDEBUG << "used is set to" << str2.size();
                    *used = str2.size();
                }
                if (usedMonth)
                    *usedMonth = l.monthName(month, type);

                return month;
            }
            if (context == FromString)
                continue;

            const int limit = std::min(str1.size(), str2.size());

            QDTPDEBUG << "limit is" << limit << str1 << str2;
            bool equal = true;
            for (int i=0; i<limit; ++i) {
                if (str1.at(i) != str2.at(i)) {
                    equal = false;
                    if (i > bestCount) {
                        bestCount = i;
                        bestMatch = month;
                    }
                    break;
                }
            }
            if (equal) {
                if (used)
                    *used = limit;
                if (usedMonth)
                    *usedMonth = l.monthName(month, type);
                return month;
            }
        }
        if (usedMonth && bestMatch != -1)
            *usedMonth = l.monthName(bestMatch, type);
    }
    if (used) {
        QDTPDEBUG << "used is set to" << bestCount;
        *used = bestCount;
    }
    return bestMatch;
}

int QDateTimeParser::findDay(const QString &str1, int startDay, int sectionIndex, QString *usedDay, int *used) const
{
    int bestMatch = -1;
    int bestCount = 0;
    if (!str1.isEmpty()) {
        const SectionNode &sn = sectionNode(sectionIndex);
        if (!(sn.type & (DaySection|DayOfWeekSection))) {
            qWarning("QDateTimeParser::findDay Internal error");
            return -1;
        }
        const QLocale l = locale();
        for (int day=startDay; day<=7; ++day) {
            const QString str2 = l.dayName(day, sn.count == 4 ? QLocale::LongFormat : QLocale::ShortFormat);

            if (str1.startsWith(str2.toLower())) {
                if (used)
                    *used = str2.size();
                if (usedDay) {
                    *usedDay = str2;
                }
                return day;
            }
            if (context == FromString)
                continue;

            const int limit = std::min(str1.size(), str2.size());
            bool found = true;
            for (int i=0; i<limit; ++i) {
                if (str1.at(i) != str2.at(i) && !str1.at(i).isSpace()) {
                    if (i > bestCount) {
                        bestCount = i;
                        bestMatch = day;
                    }
                    found = false;
                    break;
                }

            }
            if (found) {
                if (used)
                    *used = limit;
                if (usedDay)
                    *usedDay = str2;

                return day;
            }
        }
        if (usedDay && bestMatch != -1) {
            *usedDay = l.dayName(bestMatch, sn.count == 4 ? QLocale::LongFormat : QLocale::ShortFormat);
        }
    }
    if (used)
        *used = bestCount;

    return bestMatch;
}
#endif // QT_NO_TEXTDATE

/*!
  \internal

  returns
  0 if str == QDateTimeEdit::tr("AM")
  1 if str == QDateTimeEdit::tr("PM")
  2 if str can become QDateTimeEdit::tr("AM")
  3 if str can become QDateTimeEdit::tr("PM")
  4 if str can become QDateTimeEdit::tr("PM") and can become QDateTimeEdit::tr("AM")
  -1 can't become anything sensible

*/

int QDateTimeParser::findAmPm(QString &str, int index, int *used) const
{
    const SectionNode &s = sectionNode(index);
    if (s.type != AmPmSection) {
        qWarning("QDateTimeParser::findAmPm Internal error");
        return -1;
    }
    if (used)
        *used = str.size();
    if (str.trimmed().isEmpty()) {
        return PossibleBoth;
    }
    const QLatin1Char space(' ');
    int size = sectionMaxSize(index);

    enum {
        amindex = 0,
        pmindex = 1
    };
    QString ampm[2];
    ampm[amindex] = getAmPmText(AmText, s.count == 1 ? UpperCase : LowerCase);
    ampm[pmindex] = getAmPmText(PmText, s.count == 1 ? UpperCase : LowerCase);
    for (auto & i : ampm)
        i.truncate(size);

    QDTPDEBUG << "findAmPm" << str << ampm[0] << ampm[1];

    if (str.indexOf(ampm[amindex], 0, Qt::CaseInsensitive) == 0) {
        str = ampm[amindex];
        return AM;
    } else if (str.indexOf(ampm[pmindex], 0, Qt::CaseInsensitive) == 0) {
        str = ampm[pmindex];
        return PM;
    } else if (context == FromString || (str.count(space) == 0 && str.size() >= size)) {
        return Neither;
    }
    size = std::min(size, str.size());

    bool broken[2] = {false, false};
    for (int i=0; i<size; ++i) {
        if (str.at(i) != space) {
            for (int j=0; j<2; ++j) {
                if (!broken[j]) {
                    int index = ampm[j].indexOf(str.at(i));
                    QDTPDEBUG << "looking for" << str.at(i)
                              << "in" << ampm[j] << "and got" << index;
                    if (index == -1) {
                        if (str.at(i).category() == QChar::Letter_Uppercase) {
                            index = ampm[j].indexOf(str.at(i).toLower());
                            QDTPDEBUG << "trying with" << str.at(i).toLower()
                                      << "in" << ampm[j] << "and got" << index;
                        } else if (str.at(i).category() == QChar::Letter_Lowercase) {
                            index = ampm[j].indexOf(str.at(i).toUpper());
                            QDTPDEBUG << "trying with" << str.at(i).toUpper()
                                      << "in" << ampm[j] << "and got" << index;
                        }
                        if (index == -1) {
                            broken[j] = true;
                            if (broken[amindex] && broken[pmindex]) {
                                QDTPDEBUG << str << "didn't make it";
                                return Neither;
                            }
                            continue;
                        } else {
                            str[i] = ampm[j].at(index); // fix case
                        }
                    }
                    ampm[j].remove(index, 1);
                }
            }
        }
    }
    if (!broken[pmindex] && !broken[amindex])
        return PossibleBoth;
    return (!broken[amindex] ? PossibleAM : PossiblePM);
}

/*!
  \internal
  Max number of units that can be changed by this section.
*/

int QDateTimeParser::maxChange(int index) const
{
    const SectionNode &sn = sectionNode(index);
    switch (sn.type) {
        // Time. unit is msec
    case MSecSection: return 999;
    case SecondSection: return 59 * 1000;
    case MinuteSection: return 59 * 60 * 1000;
    case Hour24Section: case Hour12Section: return 59 * 60 * 60 * 1000;

        // Date. unit is day
    case DayOfWeekSection: return 7;
    case DaySection: return 30;
    case MonthSection: return 365 - 31;
    case YearSection: return 9999 * 365;
    case YearSection2Digits: return 100 * 365;
    default:
        qWarning("QDateTimeParser::maxChange() Internal error (%s)",
                 qPrintable(sectionName(sectionType(index))));
    }

    return -1;
}

QDateTimeParser::FieldInfo QDateTimeParser::fieldInfo(int index) const
{
    FieldInfo ret = nullptr;
    const SectionNode &sn = sectionNode(index);
    const Section s = sn.type;
    switch (s) {
    case MSecSection:
        ret |= Fraction;
        // fallthrough
    case SecondSection:
    case MinuteSection:
    case Hour24Section:
    case Hour12Section:
    case YearSection:
    case YearSection2Digits:
        ret |= Numeric;
        if (s != YearSection) {
            ret |= AllowPartial;
        }
        if (sn.count != 1) {
            ret |= FixedWidth;
        }
        break;
    case MonthSection:
    case DaySection:
        switch (sn.count) {
        case 2:
            ret |= FixedWidth;
            // fallthrough
        case 1:
            ret |= (Numeric|AllowPartial);
            break;
        }
        break;
    case DayOfWeekSection:
        if (sn.count == 3)
            ret |= FixedWidth;
        break;
    case AmPmSection:
        ret |= FixedWidth;
        break;
    default:
        qWarning("QDateTimeParser::fieldInfo Internal error 2 (%d %s %d)",
                 index, qPrintable(sectionName(sn.type)), sn.count);
        break;
    }
    return ret;
}

/*!
  \internal Get a number that str can become which is between min
  and max or -1 if this is not possible.
*/


QString QDateTimeParser::sectionFormat(int index) const
{
    const SectionNode &sn = sectionNode(index);
    return sectionFormat(sn.type, sn.count);
}

QString QDateTimeParser::sectionFormat(Section s, int count) const
{
    QChar fillChar;
    switch (s) {
    case AmPmSection: return count == 1 ? QLatin1String("AP") : QLatin1String("ap");
    case MSecSection: fillChar = QLatin1Char('z'); break;
    case SecondSection: fillChar = QLatin1Char('s'); break;
    case MinuteSection: fillChar = QLatin1Char('m'); break;
    case Hour24Section: fillChar = QLatin1Char('H'); break;
    case Hour12Section: fillChar = QLatin1Char('h'); break;
    case DayOfWeekSection:
    case DaySection: fillChar = QLatin1Char('d'); break;
    case MonthSection: fillChar = QLatin1Char('M'); break;
    case YearSection2Digits:
    case YearSection: fillChar = QLatin1Char('y'); break;
    default:
        qWarning("QDateTimeParser::sectionFormat Internal error (%s)",
                 qPrintable(sectionName(s)));
        return QString();
    }
    if (fillChar.isNull()) {
        qWarning("QDateTimeParser::sectionFormat Internal error 2");
        return QString();
    }

    QString str;
    str.fill(fillChar, count);
    return str;
}


/*! \internal Returns true if str can be modified to represent a
  number that is within min and max.
*/

bool QDateTimeParser::potentialValue(const QString &str, int min, int max, int index,
                                     const QDateTime &currentValue, int insert) const
{
    if (str.isEmpty()) {
        return true;
    }
    const int size = sectionMaxSize(index);
    int val = (int)locale().toUInt(str);
    const SectionNode &sn = sectionNode(index);
    if (sn.type == YearSection2Digits) {
        val += currentValue.date().year() - (currentValue.date().year() % 100);
    }
    if (val >= min && val <= max && str.size() == size) {
        return true;
    } else if (val > max) {
        return false;
    } else if (str.size() == size && val < min) {
        return false;
    }

    const int len = size - str.size();
    for (int i=0; i<len; ++i) {
        for (int j=0; j<10; ++j) {
            if (potentialValue(str + QLatin1Char('0' + j), min, max, index, currentValue, insert)) {
                return true;
            } else if (insert >= 0) {
                QString tmp = str;
                tmp.insert(insert, QLatin1Char('0' + j));
                if (potentialValue(tmp, min, max, index, currentValue, insert))
                    return true;
            }
        }
    }

    return false;
}

bool QDateTimeParser::skipToNextSection(int index, const QDateTime &current, const QString &text) const
{
    Q_ASSERT(current >= getMinimum() && current <= getMaximum());

    const SectionNode &node = sectionNode(index);
    Q_ASSERT(text.size() < sectionMaxSize(index));

    const QDateTime maximum = getMaximum();
    const QDateTime minimum = getMinimum();
    QDateTime tmp = current;
    int min = absoluteMin(index);
    setDigit(tmp, index, min);
    if (tmp < minimum) {
        min = getDigit(minimum, index);
    }

    int max = absoluteMax(index, current);
    setDigit(tmp, index, max);
    if (tmp > maximum) {
        max = getDigit(maximum, index);
    }
    int pos = cursorPosition() - node.pos;
    if (pos < 0 || pos >= text.size())
        pos = -1;

    const bool potential = potentialValue(text, min, max, index, current, pos);
    return !potential;

    /* If the value potentially can become another valid entry we
     * don't want to skip to the next. E.g. In a M field (month
     * without leading 0 if you type 1 we don't want to autoskip but
     * if you type 3 we do
    */
}

/*!
  \internal
  For debugging. Returns the name of the section \a s.
*/

QString QDateTimeParser::sectionName(int s) const
{
    switch (s) {
    case QDateTimeParser::AmPmSection: return QLatin1String("AmPmSection");
    case QDateTimeParser::DaySection: return QLatin1String("DaySection");
    case QDateTimeParser::DayOfWeekSection: return QLatin1String("DayOfWeekSection");
    case QDateTimeParser::Hour24Section: return QLatin1String("Hour24Section");
    case QDateTimeParser::Hour12Section: return QLatin1String("Hour12Section");
    case QDateTimeParser::MSecSection: return QLatin1String("MSecSection");
    case QDateTimeParser::MinuteSection: return QLatin1String("MinuteSection");
    case QDateTimeParser::MonthSection: return QLatin1String("MonthSection");
    case QDateTimeParser::SecondSection: return QLatin1String("SecondSection");
    case QDateTimeParser::YearSection: return QLatin1String("YearSection");
    case QDateTimeParser::YearSection2Digits: return QLatin1String("YearSection2Digits");
    case QDateTimeParser::NoSection: return QLatin1String("NoSection");
    case QDateTimeParser::FirstSection: return QLatin1String("FirstSection");
    case QDateTimeParser::LastSection: return QLatin1String("LastSection");
    default: return QLatin1String("Unknown section ") + QString::number(s);
    }
}

/*!
  \internal
  For debugging. Returns the name of the state \a s.
*/

QString QDateTimeParser::stateName(int s) const
{
    switch (s) {
    case Invalid: return QLatin1String("Invalid");
    case Intermediate: return QLatin1String("Intermediate");
    case Acceptable: return QLatin1String("Acceptable");
    default: return QLatin1String("Unknown state ") + QString::number(s);
    }
}

#ifndef QT_NO_DATESTRING
bool QDateTimeParser::fromString(const QString &t, QDate *date, QTime *time) const
{
    QDateTime val(QDate(1900, 1, 1), QDATETIMEEDIT_TIME_MIN);
    QString text = t;
    int copy = -1;
    const StateNode tmp = parse(text, copy, val, false);
    if (tmp.state != Acceptable || tmp.conflicts) {
        return false;
    }
    if (time) {
        const QTime t = tmp.value.time();
        if (!t.isValid()) {
            return false;
        }
        *time = t;
    }

    if (date) {
        const QDate d = tmp.value.date();
        if (!d.isValid()) {
            return false;
        }
        *date = d;
    }
    return true;
}
#endif // QT_NO_DATESTRING

QDateTime QDateTimeParser::getMinimum() const
{
    return QDateTime(QDATETIMEEDIT_DATE_MIN, QDATETIMEEDIT_TIME_MIN, spec);
}

QDateTime QDateTimeParser::getMaximum() const
{
    return QDateTime(QDATETIMEEDIT_DATE_MAX, QDATETIMEEDIT_TIME_MAX, spec);
}

QString QDateTimeParser::getAmPmText(AmPm ap, Case cs) const
{
    if (ap == AmText) {
        return (cs == UpperCase ? QLatin1String("AM") : QLatin1String("am"));
    } else {
        return (cs == UpperCase ? QLatin1String("PM") : QLatin1String("pm"));
    }
}

/*
  \internal

  I give arg2 preference because arg1 is always a QDateTime.
*/

bool operator==(const QDateTimeParser::SectionNode &s1, const QDateTimeParser::SectionNode &s2)
{
    return (s1.type == s2.type) && (s1.pos == s2.pos) && (s1.count == s2.count);
}

#endif // QT_BOOTSTRAPPED

QT_END_NAMESPACE

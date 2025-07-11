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

#ifndef QDATETIME_H
#define QDATETIME_H

#include <QtCore/qstring.h>
#include <QtCore/qnamespace.h>
#include <QtCore/qsharedpointer.h>

#include <limits>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class Q_CORE_EXPORT QDate
{
public:
    enum MonthNameType {
        DateFormat = 0,
        StandaloneFormat
    };
public:
    QDate() { jd = nullJd(); }
    QDate(int y, int m, int d);

    bool isNull() const { return !isValid(); }
    bool isValid() const { return jd >= minJd() && jd <= maxJd(); }

    int year() const;
    int month() const;
    int day() const;
    int dayOfWeek() const;
    int dayOfYear() const;
    int daysInMonth() const;
    int daysInYear() const;
    int weekNumber(int *yearNum = nullptr) const;

#ifndef QT_NO_TEXTDATE
    static QString shortMonthName(int month, MonthNameType type = DateFormat);
    static QString shortDayName(int weekday, MonthNameType type = DateFormat);
    static QString longMonthName(int month, MonthNameType type = DateFormat);
    static QString longDayName(int weekday, MonthNameType type = DateFormat);
#endif // QT_NO_TEXTDATE
#ifndef QT_NO_DATESTRING
    QString toString(Qt::DateFormat f = Qt::TextDate) const;
    QString toString(const QString &format) const;
#endif
    [[deprecated("Use setDate")]]
    inline bool setYMD(int y, int m, int d)
    { if (uint(y) <= 99) y += 1900; return setDate(y, m, d); }
    bool setDate(int year, int month, int day);

    void getDate(int *year, int *month, int *day) const;

    QDate addDays(qint64 days) const;
    QDate addMonths(int months) const;
    QDate addYears(int years) const;
    qint64 daysTo(const QDate &) const;

    bool operator==(const QDate &other) const { return jd == other.jd; }
    bool operator!=(const QDate &other) const { return jd != other.jd; }
    bool operator<(const QDate &other) const { return jd < other.jd; }
    bool operator<=(const QDate &other) const { return jd <= other.jd; }
    bool operator>(const QDate &other) const { return jd > other.jd; }
    bool operator>=(const QDate &other) const { return jd >= other.jd; }

    static QDate currentDate();
#ifndef QT_NO_DATESTRING
    static QDate fromString(const QString &s, Qt::DateFormat f = Qt::TextDate);
    static QDate fromString(const QString &s, const QString &format);
#endif
    static bool isValid(int y, int m, int d);
    static bool isLeapYear(int year);

    static inline QDate fromJulianDay(qint64 jd)
    { QDate d; if (jd >= minJd() && jd <= maxJd()) d.jd = jd; return d; }
    inline qint64 toJulianDay() const { return jd; }

private:
    static inline qint64 nullJd() { return std::numeric_limits<qint64>::min(); }
    static inline qint64 minJd() { return Q_INT64_C(-784350574879); }
    static inline qint64 maxJd() { return Q_INT64_C( 784354017364); }

    qint64 jd;

    friend class QDateTime;
    friend class QDateTimePrivate;
#ifndef QT_NO_DATASTREAM
    friend Q_CORE_EXPORT QDataStream &operator<<(QDataStream &, const QDate &);
    friend Q_CORE_EXPORT QDataStream &operator>>(QDataStream &, QDate &);
#endif
};
Q_DECLARE_TYPEINFO(QDate, Q_MOVABLE_TYPE);

class Q_CORE_EXPORT QTime
{
public:
    QTime(): mds(NullTime)
    {}
    QTime(int h, int m, int s = 0, int ms = 0);

    bool isNull() const { return mds == NullTime; }
    bool isValid() const;

    int hour() const;
    int minute() const;
    int second() const;
    int msec() const;
#ifndef QT_NO_DATESTRING
    QString toString(Qt::DateFormat f = Qt::TextDate) const;
    QString toString(const QString &format) const;
#endif
    bool setHMS(int h, int m, int s, int ms = 0);

    QTime addSecs(int secs) const;
    int secsTo(const QTime &) const;
    QTime addMSecs(int ms) const;
    int msecsTo(const QTime &) const;

    constexpr bool operator==(const QTime &other) const { return mds == other.mds; }
    constexpr bool operator!=(const QTime &other) const { return mds != other.mds; }
    constexpr bool operator<(const QTime &other) const { return mds < other.mds; }
    constexpr bool operator<=(const QTime &other) const { return mds <= other.mds; }
    constexpr bool operator>(const QTime &other) const { return mds > other.mds; }
    constexpr bool operator>=(const QTime &other) const { return mds >= other.mds; }

    static inline QTime fromMSecsSinceStartOfDay(int msecs) { QTime t; t.mds = msecs; return t; }
    inline int msecsSinceStartOfDay() const { return mds == NullTime ? 0 : mds; }

    static QTime currentTime();
#ifndef QT_NO_DATESTRING
    static QTime fromString(const QString &s, Qt::DateFormat f = Qt::TextDate);
    static QTime fromString(const QString &s, const QString &format);
#endif
    static bool isValid(int h, int m, int s, int ms = 0);

    void start();
    int restart();
    int elapsed() const;
private:
    enum TimeFlag { NullTime = -1 };
    constexpr inline int ds() const { return mds == -1 ? 0 : mds; }
    int mds;

    friend class QDateTime;
    friend class QDateTimePrivate;
#ifndef QT_NO_DATASTREAM
    friend Q_CORE_EXPORT QDataStream &operator<<(QDataStream &, const QTime &);
    friend Q_CORE_EXPORT QDataStream &operator>>(QDataStream &, QTime &);
#endif
};
Q_DECLARE_TYPEINFO(QTime, Q_MOVABLE_TYPE);

class QDateTimePrivate;

class Q_CORE_EXPORT QDateTime
{
public:
    QDateTime();
    explicit QDateTime(const QDate &);
    QDateTime(const QDate &date, const QTime &time, Qt::TimeSpec spec = Qt::LocalTime, int offsetSeconds = 0);
    QDateTime(const QDateTime &other);
    ~QDateTime();

    QDateTime &operator=(const QDateTime &other);

    inline void swap(QDateTime &other) { qSwap(d, other.d); }

    bool isNull() const;
    bool isValid() const;

    QDate date() const;
    QTime time() const;
    Qt::TimeSpec timeSpec() const;
    int offsetFromUtc() const;
    QString timeZoneAbbreviation() const;

    qint64 toMSecsSinceEpoch() const;
    uint toTime_t() const;

    void setDate(const QDate &date);
    void setTime(const QTime &time);
    void setTimeSpec(Qt::TimeSpec spec);
    void setOffsetFromUtc(int offsetSeconds);
    void setMSecsSinceEpoch(qint64 msecs);
    void setTime_t(uint secsSince1Jan1970UTC);

#ifndef QT_NO_DATESTRING
    QString toString(Qt::DateFormat f = Qt::TextDate) const;
    QString toString(const QString &format) const;
#endif
    QDateTime addDays(qint64 days) const;
    QDateTime addMonths(int months) const;
    QDateTime addYears(int years) const;
    QDateTime addSecs(qint64 secs) const;
    QDateTime addMSecs(qint64 msecs) const;

    QDateTime toTimeSpec(Qt::TimeSpec spec) const;
    inline QDateTime toLocalTime() const { return toTimeSpec(Qt::LocalTime); }
    inline QDateTime toUTC() const { return toTimeSpec(Qt::UTC); }
    QDateTime toOffsetFromUtc(int offsetSeconds) const;

    qint64 daysTo(const QDateTime &) const;
    int secsTo(const QDateTime &) const;
    qint64 msecsTo(const QDateTime &) const;

    bool operator==(const QDateTime &other) const;
    inline bool operator!=(const QDateTime &other) const { return !(*this == other); }
    bool operator<(const QDateTime &other) const;
    inline bool operator<=(const QDateTime &other) const { return !(other < *this); }
    inline bool operator>(const QDateTime &other) const { return other < *this; }
    inline bool operator>=(const QDateTime &other) const { return !(*this < other); }

    [[deprecated("Use setOffsetFromUtc")]] void setUtcOffset(int seconds);
    [[deprecated("Use offsetFromUtc")]] int utcOffset() const;

    static QDateTime currentDateTime();
    static QDateTime currentDateTimeUtc();
#ifndef QT_NO_DATESTRING
    static QDateTime fromString(const QString &s, Qt::DateFormat f = Qt::TextDate);
    static QDateTime fromString(const QString &s, const QString &format);
#endif
    static QDateTime fromTime_t(uint secsSince1Jan1970UTC);
    static QDateTime fromMSecsSinceEpoch(qint64 msecs, Qt::TimeSpec spec = Qt::LocalTime, int offsetFromUtc = 0);
    static qint64 currentMSecsSinceEpoch() noexcept;

private:
    friend class QDateTimePrivate;
    QSharedDataPointer<QDateTimePrivate> d;

#ifndef QT_NO_DATASTREAM
    friend Q_CORE_EXPORT QDataStream &operator<<(QDataStream &, const QDateTime &);
    friend Q_CORE_EXPORT QDataStream &operator>>(QDataStream &, QDateTime &);
#endif
};
Q_DECLARE_TYPEINFO(QDateTime, Q_MOVABLE_TYPE);

#ifndef QT_NO_DATASTREAM
Q_CORE_EXPORT QDataStream &operator<<(QDataStream &, const QDate &);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &, QDate &);
Q_CORE_EXPORT QDataStream &operator<<(QDataStream &, const QTime &);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &, QTime &);
Q_CORE_EXPORT QDataStream &operator<<(QDataStream &, const QDateTime &);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &, QDateTime &);
#endif // QT_NO_DATASTREAM

#if !defined(QT_NO_DEBUG_STREAM) && !defined(QT_NO_DATESTRING)
Q_CORE_EXPORT QDebug operator<<(QDebug, const QDate &);
Q_CORE_EXPORT QDebug operator<<(QDebug, const QTime &);
Q_CORE_EXPORT QDebug operator<<(QDebug, const QDateTime &);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QDATETIME_H

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

#ifndef QDATETIME_P_H
#define QDATETIME_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qplatformdefs.h"
#include "QtCore/qatomic.h"
#include "QtCore/qdatetime.h"
#include "QtCore/qstringlist.h"
#include "QtCore/qlocale.h"
#ifndef QT_BOOTSTRAPPED
# include "QtCore/qvariant.h"
#endif
#include "QtCore/qvector.h"


#define QDATETIMEEDIT_TIME_MIN QTime(0, 0, 0, 0)
#define QDATETIMEEDIT_TIME_MAX QTime(23, 59, 59, 999)
#define QDATETIMEEDIT_DATE_MIN QDate(100, 1, 1)
#define QDATETIMEEDIT_COMPAT_DATE_MIN QDate(1752, 9, 14)
#define QDATETIMEEDIT_DATE_MAX QDate(7999, 12, 31)
#define QDATETIMEEDIT_DATETIME_MIN QDateTime(QDATETIMEEDIT_DATE_MIN, QDATETIMEEDIT_TIME_MIN)
#define QDATETIMEEDIT_COMPAT_DATETIME_MIN QDateTime(QDATETIMEEDIT_COMPAT_DATE_MIN, QDATETIMEEDIT_TIME_MIN)
#define QDATETIMEEDIT_DATETIME_MAX QDateTime(QDATETIMEEDIT_DATE_MAX, QDATETIMEEDIT_TIME_MAX)
#define QDATETIMEEDIT_DATE_INITIAL QDate(2000, 1, 1)

QT_BEGIN_NAMESPACE

class QDateTimePrivate : public QSharedData
{
public:
    // Never change or delete this enum, it is required for backwards compatible
    // serialization of QDateTime before 5.2, so is essentially public API
    enum Spec {
        LocalUnknown = -1,
        LocalStandard = 0,
        LocalDST = 1,
        UTC = 2,
        OffsetFromUTC = 3
    };

    // Daylight Time Status
    enum DaylightStatus {
        NoDaylightTime,
        UnknownDaylightTime,
        StandardTime,
        DaylightTime
    };

    // Status of date/time
    enum StatusFlag {
        NullDate            = 0x01,
        NullTime            = 0x02,
        ValidDate           = 0x04,
        ValidTime           = 0x08,
    };
    Q_DECLARE_FLAGS(StatusFlags, StatusFlag)

    QDateTimePrivate() : m_msecs(0),
                         m_spec(Qt::LocalTime),
                         m_offsetFromUtc(0),
                         m_status(NullDate | NullTime)
    {}

    QDateTimePrivate(const QDate &toDate, const QTime &toTime, Qt::TimeSpec toSpec,
                     int offsetSeconds);

    QDateTimePrivate(const QDateTimePrivate &other) : QSharedData(other),
                                                      m_msecs(other.m_msecs),
                                                      m_spec(other.m_spec),
                                                      m_offsetFromUtc(other.m_offsetFromUtc),
                                                      m_status(other.m_status)
    {}

    qint64 m_msecs;
    Qt::TimeSpec m_spec;
    int m_offsetFromUtc;

    StatusFlags m_status;

    void setTimeSpec(Qt::TimeSpec spec, int offsetSeconds);
    void setDateTime(const QDate &date, const QTime &time);
    void getDateTime(QDate *date, QTime *time) const;

    // Get/set date and time status
    inline bool isNullDate() const { return (m_status & NullDate) == NullDate; }
    inline bool isNullTime() const { return (m_status & NullTime) == NullTime; }
    inline bool isValidDate() const { return (m_status & ValidDate) == ValidDate; }
    inline bool isValidTime() const { return (m_status & ValidTime) == ValidTime; }

    static inline qint64 minJd() { return QDate::minJd(); }
    static inline qint64 maxJd() { return QDate::maxJd(); }
};

#ifndef QT_BOOTSTRAPPED

class Q_CORE_EXPORT QDateTimeParser
{
public:
    enum Context {
        FromString,
        DateTimeEdit
    };
    QDateTimeParser(QVariant::Type t, Context ctx)
        : currentSectionIndex(-1), display(nullptr), cachedDay(-1), parserType(t),
        fixday(false), spec(Qt::LocalTime), context(ctx)
    {
        defaultLocale = QLocale::system();
        first.type = FirstSection;
        first.pos = -1;
        first.count = -1;
        first.zeroesAdded = 0;
        last.type = FirstSection;
        last.pos = -1;
        last.count = -1;
        last.zeroesAdded = 0;
        none.type = NoSection;
        none.pos = -1;
        none.count = -1;
        none.zeroesAdded = 0;
    }
    virtual ~QDateTimeParser() = default;
    enum {
        Neither = -1,
        AM = 0,
        PM = 1,
        PossibleAM = 2,
        PossiblePM = 3,
        PossibleBoth = 4
    };

    enum Section {
        NoSection = 0x00000,
        AmPmSection = 0x00001,
        MSecSection = 0x00002,
        SecondSection = 0x00004,
        MinuteSection = 0x00008,
        Hour12Section   = 0x00010,
        Hour24Section   = 0x00020,
        TimeSectionMask = (AmPmSection|MSecSection|SecondSection|MinuteSection|Hour12Section|Hour24Section),
        Internal = 0x10000,
        DaySection = 0x00100,
        MonthSection = 0x00200,
        YearSection = 0x00400,
        YearSection2Digits = 0x00800,
        DayOfWeekSection = 0x01000,
        DateSectionMask = (DaySection|MonthSection|YearSection|YearSection2Digits|DayOfWeekSection),
        FirstSection = 0x02000|Internal,
        LastSection = 0x04000|Internal,
        CalendarPopupSection = 0x08000|Internal,

        NoSectionIndex = -1,
        FirstSectionIndex = -2,
        LastSectionIndex = -3,
        CalendarPopupIndex = -4
    }; // duplicated from qdatetimeedit.h
    Q_DECLARE_FLAGS(Sections, Section)

    struct SectionNode {
        Section type;
        mutable int pos;
        int count;
        int zeroesAdded;
    };

    enum State { // duplicated from QValidator
        Invalid,
        Intermediate,
        Acceptable
    };

    struct StateNode {
        StateNode() : state(Invalid), conflicts(false) {}
        QString input;
        State state;
        bool conflicts;
        QDateTime value;
    };

    enum AmPm {
        AmText,
        PmText
    };

    enum Case {
        UpperCase,
        LowerCase
    };

#ifndef QT_NO_DATESTRING
    StateNode parse(QString &input, int &cursorPosition, const QDateTime &currentValue, bool fixup) const;
#endif
    int sectionMaxSize(int index) const;
    int sectionSize(int index) const;
    int sectionMaxSize(Section s, int count) const;
    int sectionPos(int index) const;
    int sectionPos(const SectionNode &sn) const;

    const SectionNode &sectionNode(int index) const;
    Section sectionType(int index) const;
    QString sectionText(int sectionIndex) const;
    QString sectionText(const QString &text, int sectionIndex, int index) const;
    int getDigit(const QDateTime &dt, int index) const;
    bool setDigit(QDateTime &t, int index, int newval) const;
    int parseSection(const QDateTime &currentValue, int sectionIndex, QString &txt, int &cursorPosition,
                     int index, QDateTimeParser::State &state, int *used = nullptr) const;
    int absoluteMax(int index, const QDateTime &value = QDateTime()) const;
    int absoluteMin(int index) const;
    bool parseFormat(const QString &format);
#ifndef QT_NO_DATESTRING
    bool fromString(const QString &text, QDate *date, QTime *time) const;
#endif

#ifndef QT_NO_TEXTDATE
    int findMonth(const QString &str1, int monthstart, int sectionIndex,
                  QString *monthName = nullptr, int *used = nullptr) const;
    int findDay(const QString &str1, int intDaystart, int sectionIndex,
                QString *dayName = nullptr, int *used = nullptr) const;
#endif
    int findAmPm(QString &str1, int index, int *used = nullptr) const;
    int maxChange(int s) const;
    bool potentialValue(const QString &str, int min, int max, int index,
                        const QDateTime &currentValue, int insert) const;
    bool skipToNextSection(int section, const QDateTime &current, const QString &sectionText) const;
    QString sectionName(int s) const;
    QString stateName(int s) const;

    QString sectionFormat(int index) const;
    QString sectionFormat(Section s, int count) const;

    enum FieldInfoFlag {
        Numeric = 0x01,
        FixedWidth = 0x02,
        AllowPartial = 0x04,
        Fraction = 0x08
    };
    Q_DECLARE_FLAGS(FieldInfo, FieldInfoFlag)

    FieldInfo fieldInfo(int index) const;

    virtual QDateTime getMinimum() const;
    virtual QDateTime getMaximum() const;
    virtual int cursorPosition() const { return -1; }
    virtual QString displayText() const { return text; }
    virtual QString getAmPmText(AmPm ap, Case cs) const;
    virtual QLocale locale() const { return defaultLocale; }

    mutable int currentSectionIndex;
    Sections display;
    mutable int cachedDay;
    mutable QString text;
    QVector<SectionNode> sectionNodes;
    SectionNode first, last, none, popup;
    QStringList separators;
    QString displayFormat;
    QLocale defaultLocale;
    QVariant::Type parserType;

    bool fixday;

    Qt::TimeSpec spec; // spec if used by QDateTimeEdit
    Context context;
};

Q_CORE_EXPORT bool operator==(const QDateTimeParser::SectionNode &s1, const QDateTimeParser::SectionNode &s2);

Q_DECLARE_OPERATORS_FOR_FLAGS(QDateTimeParser::Sections)
Q_DECLARE_OPERATORS_FOR_FLAGS(QDateTimeParser::FieldInfo)

#endif // QT_BOOTSTRAPPED

QT_END_NAMESPACE

#endif // QDATETIME_P_H

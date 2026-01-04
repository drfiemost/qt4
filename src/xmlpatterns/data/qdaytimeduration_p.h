/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtXmlPatterns module of the Qt Toolkit.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_DayTimeDuration_H
#define Patternist_DayTimeDuration_H

#include "qabstractduration_p.h"
#include "qitem_p.h"


QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Implements the value instance of the @c xs:dayTimeDuration type.
     *
     * @author Frans Englich <frans.englich@nokia.com>
     * @ingroup Patternist_xdm
     */
    class DayTimeDuration : public AbstractDuration
    {
    public:

        typedef QExplicitlySharedDataPointer<DayTimeDuration> Ptr;

        /**
         * Creates an instance from the lexical representation @p string.
         */
        static DayTimeDuration::Ptr fromLexical(const QString &string);

        static DayTimeDuration::Ptr fromComponents(const bool isPositive,
                                                   const DayCountProperty days,
                                                   const HourProperty hours,
                                                   const MinuteProperty minutes,
                                                   const SecondProperty seconds,
                                                   const MSecondProperty mseconds);
        /**
         * Creates a DayTimeDuration that has the value expressed in seconds @p secs
         * and milli seconds @p msecs. The signedness of @p secs communicates
         * whether this DayTimeDuration is positive or negative. @p msecs must always
         * be positive.
         */
        static DayTimeDuration::Ptr fromSeconds(const SecondCountProperty secs,
                                                const MSecondProperty msecs = 0);

        ItemType::Ptr type() const override;
        QString stringValue() const override;

        /**
         * @returns always 0.
         */
        YearProperty years() const override;

        /**
         * @returns always 0.
         */
        MonthProperty months() const override;
        DayCountProperty days() const override;
        HourProperty hours() const override;
        MinuteProperty minutes() const override;
        MSecondProperty mseconds() const override;
        SecondProperty seconds() const override;

        /**
         * @returns the value of this xs:dayTimeDuration
         * in milli seconds.
         * @see <a href="http://www.w3.org/TR/xpath-functions/#dt-dayTimeDuration">XQuery 1.0
         * and XPath 2.0 Functions and Operators, 10.3.2.2 Calculating the value of a
         * xs:dayTimeDuration from the lexical representation</a>
         */
        Value value() const override;

        /**
         * Creates a DayTimeDuration containing the value @p val. @p val is
         * expressed in milli seconds.
         *
         * If @p val is zero, is CommonValues::DayTimeDurationZero returned.
         */
        Item fromValue(const Value val) const override;

    protected:
        friend class CommonValues;

        DayTimeDuration(const bool isPositive,
                        const DayCountProperty days,
                        const HourProperty hours,
                        const MinuteProperty minutes,
                        const SecondProperty seconds,
                        const MSecondProperty mseconds);

    private:
        const DayCountProperty  m_days;
        const HourProperty      m_hours;
        const MinuteProperty    m_minutes;
        const SecondProperty    m_seconds;
        const MSecondProperty   m_mseconds;
    };
}

QT_END_NAMESPACE


#endif

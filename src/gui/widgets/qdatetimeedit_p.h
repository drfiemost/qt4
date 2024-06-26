/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QDATETIMEEDIT_P_H
#define QDATETIMEEDIT_P_H

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

#include "QtGui/qcombobox.h"
#include "QtGui/qcalendarwidget.h"
#include "QtGui/qspinbox.h"
#include "QtGui/qtoolbutton.h"
#include "QtGui/qmenu.h"
#include "QtGui/qlabel.h"
#include "QtGui/qdatetimeedit.h"
#include "QtGui/private/qabstractspinbox_p.h"
#include "QtCore/private/qdatetime_p.h"

#include "qdebug.h"

#ifndef QT_NO_DATETIMEEDIT

QT_BEGIN_NAMESPACE

class QCalendarPopup;
class QDateTimeEditPrivate : public QAbstractSpinBoxPrivate, public QDateTimeParser
{
    Q_DECLARE_PUBLIC(QDateTimeEdit)
public:
    QDateTimeEditPrivate();

    void init(const QVariant &var);
    void readLocaleSettings();

    void emitSignals(EmitPolicy ep, const QVariant &old) override;
    QString textFromValue(const QVariant &f) const override;
    QVariant valueFromText(const QString &f) const override;
    void _q_editorCursorPositionChanged(int oldpos, int newpos) override;
    void interpret(EmitPolicy ep) override;
    void clearCache() const override;

    QDateTime validateAndInterpret(QString &input, int &, QValidator::State &state,
                                   bool fixup = false) const;
    void clearSection(int index);
    QString displayText() const override { return edit->text(); } // this is from QDateTimeParser

    int absoluteIndex(QDateTimeEdit::Section s, int index) const;
    int absoluteIndex(const SectionNode &s) const;
    void updateEdit() override;
    QDateTime stepBy(int index, int steps, bool test = false) const;
    int sectionAt(int pos) const;
    int closestSection(int index, bool forward) const;
    int nextPrevSection(int index, bool forward) const;
    void setSelected(int index, bool forward = false);

    void updateCache(const QVariant &val, const QString &str) const;

    void updateTimeSpec();
    QDateTime getMinimum() const override { return minimum.toDateTime(); }
    QDateTime getMaximum() const override { return maximum.toDateTime(); }
    QLocale locale() const override { return q_func()->locale(); }
    QString valueToText(const QVariant &var) const { return textFromValue(var); }
    QString getAmPmText(AmPm ap, Case cs) const override;
    int cursorPosition() const override { return edit ? edit->cursorPosition() : -1; }

    QStyle::SubControl newHoverControl(const QPoint &pos) override;
    void updateEditFieldGeometry() override;
    QVariant getZeroVariant() const override;
    void setRange(const QVariant &min, const QVariant &max) override;

    void _q_resetButton();
    void updateArrow(QStyle::StateFlag state);
    bool calendarPopupEnabled() const;
    void syncCalendarWidget();

    bool isSeparatorKey(const QKeyEvent *k) const;

    static QDateTimeEdit::Sections convertSections(QDateTimeParser::Sections s);
    static QDateTimeEdit::Section convertToPublic(QDateTimeParser::Section s);

    void initCalendarPopup(QCalendarWidget *cw = nullptr);
    void positionCalendarPopup();

    QDateTimeEdit::Sections sections;
    mutable bool cacheGuard;

    QString defaultDateFormat, defaultTimeFormat, defaultDateTimeFormat, unreversedFormat;
    mutable QVariant conflictGuard;
    bool hasHadFocus, formatExplicitlySet, calendarPopup;
    QStyle::StateFlag arrowState;
    QCalendarPopup *monthCalendar;

#ifdef QT_KEYPAD_NAVIGATION
    bool focusOnButton;
#endif
};


class QCalendarPopup : public QWidget
{
    Q_OBJECT
public:
    QCalendarPopup(QWidget *parent = nullptr, QCalendarWidget *cw = nullptr);
    QDate selectedDate() { return verifyCalendarInstance()->selectedDate(); }
    void setDate(const QDate &date);
    void setDateRange(const QDate &min, const QDate &max);
    void setFirstDayOfWeek(Qt::DayOfWeek dow) { verifyCalendarInstance()->setFirstDayOfWeek(dow); }
    QCalendarWidget *calendarWidget() const { return const_cast<QCalendarPopup*>(this)->verifyCalendarInstance(); }
    void setCalendarWidget(QCalendarWidget *cw);
Q_SIGNALS:
    void activated(const QDate &date);
    void newDateSelected(const QDate &newDate);
    void hidingCalendar(const QDate &oldDate);
    void resetButton();

private Q_SLOTS:
    void dateSelected(const QDate &date);
    void dateSelectionChanged();

protected:
    void hideEvent(QHideEvent *) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    bool event(QEvent *e) override;

private:
    QCalendarWidget *verifyCalendarInstance();

    QWeakPointer<QCalendarWidget> calendar;
    QDate oldDate;
    bool dateChanged;
};

QT_END_NAMESPACE

#endif // QT_NO_DATETIMEEDIT

#endif // QDATETIMEEDIT_P_H

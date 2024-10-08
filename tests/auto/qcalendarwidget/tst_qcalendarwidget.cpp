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

#include <qcalendarwidget.h>
#include <qtoolbutton.h>
#include <qspinbox.h>
#include <qmenu.h>
#include <qdebug.h>
#include <qdatetime.h>
#include <qtextformat.h>


//TESTED_CLASS=
//TESTED_FILES=

class tst_QCalendarWidget : public QObject
{
    Q_OBJECT

public:
    tst_QCalendarWidget();
    virtual ~tst_QCalendarWidget();
public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void getSetCheck();
    void buttonClickCheck();

    void setTextFormat();
    void resetTextFormat();

    void setWeekdayFormat();
    void showPrevNext_data();
    void showPrevNext();
};

// Testing get/set functions
void tst_QCalendarWidget::getSetCheck()
{
    QWidget topLevel;
    QCalendarWidget object(&topLevel);

    //horizontal header formats
    object.setHorizontalHeaderFormat(QCalendarWidget::NoHorizontalHeader);
    QCOMPARE(QCalendarWidget::NoHorizontalHeader, object.horizontalHeaderFormat());
    object.setHorizontalHeaderFormat(QCalendarWidget::SingleLetterDayNames);
    QCOMPARE(QCalendarWidget::SingleLetterDayNames, object.horizontalHeaderFormat());
    object.setHorizontalHeaderFormat(QCalendarWidget::ShortDayNames);
    QCOMPARE(QCalendarWidget::ShortDayNames, object.horizontalHeaderFormat());
    object.setHorizontalHeaderFormat(QCalendarWidget::LongDayNames);
    QCOMPARE(QCalendarWidget::LongDayNames, object.horizontalHeaderFormat());
    //vertical header formats
    object.setVerticalHeaderFormat(QCalendarWidget::ISOWeekNumbers);
    QCOMPARE(QCalendarWidget::ISOWeekNumbers, object.verticalHeaderFormat());
    object.setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    QCOMPARE(QCalendarWidget::NoVerticalHeader, object.verticalHeaderFormat());
    //maximum Date
    QDate maxDate(2006, 7, 3);
    object.setMaximumDate(maxDate);
    QCOMPARE(maxDate, object.maximumDate());
    //minimum date
    QDate minDate(2004, 7, 3);
    object.setMinimumDate(minDate);
    QCOMPARE(minDate, object.minimumDate());
    //day of week
    object.setFirstDayOfWeek(Qt::Thursday);
    QCOMPARE(Qt::Thursday, object.firstDayOfWeek());
    //grid visible
    object.setGridVisible(true);
    QVERIFY(object.isGridVisible());
    object.setGridVisible(false);
    QVERIFY(!object.isGridVisible());
    //header visible
    object.setHeaderVisible(true);
    QVERIFY(object.isHeaderVisible());
    object.setHeaderVisible(false);
    QVERIFY(!object.isHeaderVisible());
    //selection mode
    QCOMPARE(QCalendarWidget::SingleSelection, object.selectionMode());
    object.setSelectionMode(QCalendarWidget::NoSelection);
    QCOMPARE(QCalendarWidget::NoSelection, object.selectionMode());
    object.setSelectionMode(QCalendarWidget::SingleSelection);
    QCOMPARE(QCalendarWidget::SingleSelection, object.selectionMode());
   //selected date
    QDate selectedDate(2005, 7, 3);
    QSignalSpy spy(&object, SIGNAL(selectionChanged()));
    object.setSelectedDate(selectedDate);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(selectedDate, object.selectedDate());
    //month and year
    object.setCurrentPage(2004, 1);
    QCOMPARE(1, object.monthShown());
    QCOMPARE(2004, object.yearShown());
    object.showNextMonth();
    QCOMPARE(2, object.monthShown());
    object.showPreviousMonth();
    QCOMPARE(1, object.monthShown());
    object.showNextYear();
    QCOMPARE(2005, object.yearShown());
    object.showPreviousYear();
    QCOMPARE(2004, object.yearShown());
    //date range
    minDate = QDate(2006,1,1);
    maxDate = QDate(2010,12,31);
    object.setDateRange(minDate, maxDate);
    QCOMPARE(maxDate, object.maximumDate());
    QCOMPARE(minDate, object.minimumDate());

    //date should not go beyond the minimum.
    selectedDate = minDate.addDays(-10);
    object.setSelectedDate(selectedDate);
    QCOMPARE(minDate, object.selectedDate());
    QVERIFY(selectedDate != object.selectedDate());
    //date should not go beyond the maximum.
    selectedDate = maxDate.addDays(10);
    object.setSelectedDate(selectedDate);
    QCOMPARE(maxDate, object.selectedDate());
    QVERIFY(selectedDate != object.selectedDate());
    //show today
    QDate today = QDate::currentDate();
    object.showToday();
    QCOMPARE(today.month(), object.monthShown());
    QCOMPARE(today.year(), object.yearShown());
    //slect a different date and move.
    object.setSelectedDate(minDate);
    object.showSelectedDate();
    QCOMPARE(minDate.month(), object.monthShown());
    QCOMPARE(minDate.year(), object.yearShown());
}

void tst_QCalendarWidget::buttonClickCheck()
{
    QCalendarWidget object;
    QSize size = object.sizeHint();
    object.setGeometry(0,0,size.width(), size.height());
    object.show();

    QDate selectedDate(2005, 1, 1);
    //click on the month buttons
    int month = object.monthShown();
    QToolButton *button = qFindChild<QToolButton *>(&object, "qt_calendar_prevmonth");
    QTest::mouseClick(button, Qt::LeftButton);
	QCOMPARE(month > 1 ? month-1 : 12, object.monthShown());
    button = qFindChild<QToolButton *>(&object, "qt_calendar_nextmonth");
    QTest::mouseClick(button, Qt::LeftButton);
    QCOMPARE(month, object.monthShown());

    button = qFindChild<QToolButton *>(&object, "qt_calendar_yearbutton");
    QTest::mouseClick(button, Qt::LeftButton, Qt::NoModifier, button->rect().center(), 2);
    QVERIFY(!button->isVisible());
    QSpinBox *spinbox = qFindChild<QSpinBox *>(&object, "qt_calendar_yearedit");
    QTest::qWait(500);
    QTest::keyClick(spinbox, '2');
    QTest::keyClick(spinbox, '0');
    QTest::keyClick(spinbox, '0');
    QTest::keyClick(spinbox, '6');
    QTest::qWait(500);
    QWidget *widget = qFindChild<QWidget *>(&object, "qt_calendar_calendarview");
    QTest::mouseMove(widget);
    QTest::mouseClick(widget, Qt::LeftButton);
    QCOMPARE(2006, object.yearShown());
    object.setSelectedDate(selectedDate);
    object.showSelectedDate();
    QTest::keyClick(widget, Qt::Key_Down);
    QVERIFY(selectedDate != object.selectedDate());

    object.setDateRange(QDate(2006,1,1), QDate(2006,2,28));
    object.setSelectedDate(QDate(2006,1,1));
    object.showSelectedDate();
    button = qFindChild<QToolButton *>(&object, "qt_calendar_prevmonth");
    QTest::mouseClick(button, Qt::LeftButton);
    QCOMPARE(1, object.monthShown());

    button = qFindChild<QToolButton *>(&object, "qt_calendar_nextmonth");
    QTest::mouseClick(button, Qt::LeftButton);
    QCOMPARE(2, object.monthShown());
    QTest::mouseClick(button, Qt::LeftButton);
    QCOMPARE(2, object.monthShown());

}

void tst_QCalendarWidget::setTextFormat()
{
    QCalendarWidget calendar;
    QTextCharFormat format;
    format.setFontItalic(true);
    format.setForeground(Qt::green);

    const QDate date(1984, 10, 20);
    calendar.setDateTextFormat(date, format);
    QCOMPARE(calendar.dateTextFormat(date), format);
}

void tst_QCalendarWidget::resetTextFormat()
{
    QCalendarWidget calendar;
    QTextCharFormat format;
    format.setFontItalic(true);
    format.setForeground(Qt::green);

    const QDate date(1984, 10, 20);
    calendar.setDateTextFormat(date, format);

    calendar.setDateTextFormat(QDate(), QTextCharFormat());
    QCOMPARE(calendar.dateTextFormat(date), QTextCharFormat());
}

void tst_QCalendarWidget::setWeekdayFormat()
{
    QCalendarWidget calendar;

    QTextCharFormat format;
    format.setFontItalic(true);
    format.setForeground(Qt::green);

    calendar.setWeekdayTextFormat(Qt::Wednesday, format);

    // check the format of the a given month
    for (int i = 1; i <= 31; ++i) {
        const QDate date(1984, 10, i);
        const Qt::DayOfWeek dayOfWeek = static_cast<Qt::DayOfWeek>(date.dayOfWeek());
        if (dayOfWeek == Qt::Wednesday)
            QCOMPARE(calendar.weekdayTextFormat(dayOfWeek), format);
        else
            QVERIFY(calendar.weekdayTextFormat(dayOfWeek) != format);
    }
}

tst_QCalendarWidget::tst_QCalendarWidget()
{
}

tst_QCalendarWidget::~tst_QCalendarWidget()
{
}

void tst_QCalendarWidget::initTestCase()
{
}

void tst_QCalendarWidget::cleanupTestCase()
{
}

void tst_QCalendarWidget::init()
{
}

void tst_QCalendarWidget::cleanup()
{
}


typedef void (QCalendarWidget::*ShowFunc)();
Q_DECLARE_METATYPE(ShowFunc)

void tst_QCalendarWidget::showPrevNext_data()
{
    QTest::addColumn<ShowFunc>("function");
    QTest::addColumn<QDate>("dateOrigin");
    QTest::addColumn<QDate>("expectedDate");

    QTest::newRow("showNextMonth") << &QCalendarWidget::showNextMonth << QDate(1984,7,30) << QDate(1984,8,30);
    QTest::newRow("showPrevMonth") << &QCalendarWidget::showPreviousMonth << QDate(1984,7,30) << QDate(1984,6,30);
    QTest::newRow("showNextYear") << &QCalendarWidget::showNextYear << QDate(1984,7,30) << QDate(1985,7,30);
    QTest::newRow("showPrevYear") << &QCalendarWidget::showPreviousYear << QDate(1984,7,30) << QDate(1983,7,30);

    QTest::newRow("showNextMonth limit") << &QCalendarWidget::showNextMonth << QDate(2007,12,4) << QDate(2008,1,4);
    QTest::newRow("showPreviousMonth limit") << &QCalendarWidget::showPreviousMonth << QDate(2006,1,23) << QDate(2005,12,23);

    QTest::newRow("showNextMonth now") << &QCalendarWidget::showNextMonth << QDate() << QDate::currentDate().addMonths(1);
    QTest::newRow("showNextYear now") << &QCalendarWidget::showNextYear << QDate() << QDate::currentDate().addYears(1);
    QTest::newRow("showPrevieousMonth now") << &QCalendarWidget::showPreviousMonth << QDate() << QDate::currentDate().addMonths(-1);
    QTest::newRow("showPreviousYear now") << &QCalendarWidget::showPreviousYear << QDate() << QDate::currentDate().addYears(-1);

    QTest::newRow("showToday now") << &QCalendarWidget::showToday << QDate(2000,1,31) << QDate::currentDate();
    QTest::newRow("showNextMonth 31") << &QCalendarWidget::showNextMonth << QDate(2000,1,31) << QDate(2000,2,28);
    QTest::newRow("selectedDate") << &QCalendarWidget::showSelectedDate << QDate(2008,2,29) << QDate(2008,2,29);

}

void tst_QCalendarWidget::showPrevNext()
{
    QFETCH(ShowFunc, function);
    QFETCH(QDate, dateOrigin);
    QFETCH(QDate, expectedDate);

    QCalendarWidget calWidget;
    calWidget.show();
    QTest::qWaitForWindowShown(&calWidget);
    if(!dateOrigin.isNull()) {
        calWidget.setSelectedDate(dateOrigin);
        calWidget.setCurrentPage(dateOrigin.year(), dateOrigin.month());

        QCOMPARE(calWidget.yearShown(), dateOrigin.year());
        QCOMPARE(calWidget.monthShown(), dateOrigin.month());
    } else {
        QCOMPARE(calWidget.yearShown(), QDate::currentDate().year());
        QCOMPARE(calWidget.monthShown(), QDate::currentDate().month());
    }

    (calWidget.*function)();

    QCOMPARE(calWidget.yearShown(), expectedDate.year());
    QCOMPARE(calWidget.monthShown(), expectedDate.month());

    // QTBUG-4058
    QTest::qWait(20);
    QToolButton *button = qFindChild<QToolButton *>(&calWidget, "qt_calendar_prevmonth");
    QTest::mouseClick(button, Qt::LeftButton);
    expectedDate = expectedDate.addMonths(-1);
    QCOMPARE(calWidget.yearShown(), expectedDate.year());
    QCOMPARE(calWidget.monthShown(), expectedDate.month());

    if(!dateOrigin.isNull()) {
        //the selectedDate should not have changed
        QCOMPARE(calWidget.selectedDate(), dateOrigin);
    }
}

QTEST_MAIN(tst_QCalendarWidget)
#include "tst_qcalendarwidget.moc"

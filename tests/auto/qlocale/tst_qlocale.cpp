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
#include <math.h>
#include <qglobal.h>
#include <qdir.h>
#include <qtextcodec.h>
#include <qdatetime.h>
#include <float.h>
#include <locale.h>

#include <qlocale.h>
#include <qnumeric.h>

#ifdef Q_OS_LINUX
#    include <fenv.h>
#endif

#ifdef Q_OS_WINCE
#include <qcoreapplication.h>
#include <windows.h> // needed for GetUserDefaultLCID
#define _control87 _controlfp
extern "C" DWORD GetThreadLocale(void) {
	return GetUserDefaultLCID();
}

#endif

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
#    include <stdlib.h>
#endif

Q_DECLARE_METATYPE(qlonglong)
Q_DECLARE_METATYPE(QDate)
Q_DECLARE_METATYPE(QLocale::FormatType)

//TESTED_CLASS=
//TESTED_FILES=

class tst_QLocale : public QObject
{
    Q_OBJECT

public:
    tst_QLocale();

private slots:
    void initTestCase();
    void windowsDefaultLocale();
#ifdef Q_OS_MAC
    void macDefaultLocale();
#endif

    void ctor();
    void emptyCtor();
    void stringToFloat_data();
    void stringToFloat();
    void consistentC();
    void unixLocaleName();
    void stringToDouble_data();
    void stringToDouble();
    void doubleToString_data();
    void doubleToString();
    void long_long_conversion_data();
    void long_long_conversion();
    void long_long_conversion_extra();
    void testInfAndNan();
    void fpExceptions();
    void negativeZero();
    void dayOfWeek();
    void dayOfWeek_data();
    void formatDate();
    void formatDate_data();
    void formatTime();
    void formatTime_data();
    void formatDateTime();
    void formatDateTime_data();
    void formatTimeZone();
    void toDateTime_data();
    void toDateTime();
    void negativeNumbers();
    void numberOptions();
    void testNames_data();
    void testNames();
    void dayName_data();
    void dayName();
    void standaloneDayName_data();
    void standaloneDayName();
    void underflowOverflow();
    void measurementSystems();
    void systemMeasurementSystems_data();
    void systemMeasurementSystems();

    void dateFormat();
    void timeFormat();
    void dateTimeFormat();
    void monthName();
    void standaloneMonthName();

    // QSystemLocale tests
    void queryDateTime();
    void queryMeasureSystem_data();
    void queryMeasureSystem();

    void defaultNumberingSystem_data();
    void defaultNumberingSystem();

    void ampm_data();
    void ampm();
    void currency();
    void quoteString();
    void uiLanguages();
    void weekendDays();
    void listPatterns();

    void QTBUG_26035_positivesign();

    void textDirection_data();
    void textDirection();

    void bcp47Name_data();
    void bcp47Name();

private:
    QString m_decimal, m_thousand, m_sdate, m_ldate, m_time;
    QString m_sysLocaleApp;
    bool europeanTimeZone;
    void toReal_data();
    
    class TransientLocale
    {
        const int m_category;
        const char *const m_prior;
    public:
        TransientLocale(int category, const char *locale)
            : m_category(category), m_prior(setlocale(category, locale)) {}
        ~TransientLocale() { setlocale(m_category, m_prior); }
    };
};

tst_QLocale::tst_QLocale()
{
    qRegisterMetaType<QLocale::FormatType>("QLocale::FormatType");

    // Test if in Central European Time zone
    uint x1 = QDateTime(QDate(1990, 1, 1), QTime()).toTime_t();
    uint x2 = QDateTime(QDate(1990, 6, 1), QTime()).toTime_t();
    europeanTimeZone = (x1 == 631148400 && x2 == 644191200);
}

void tst_QLocale::initTestCase()
{
    QDir workingDirectory = QDir::current();
    QString sysLocaleApp = QLatin1String("syslocaleapp/syslocaleapp");
    // Windows: cd up to be able to locate the binary of the sub-process.
#ifdef Q_OS_WIN
    sysLocaleApp.append(QLatin1String(".exe"));
    if (workingDirectory.absolutePath().endsWith(QLatin1String("/debug"), Qt::CaseInsensitive)
        || workingDirectory.absolutePath().endsWith(QLatin1String("/release"), Qt::CaseInsensitive)) {
        QVERIFY(workingDirectory.cdUp());
        QVERIFY(QDir::setCurrent(workingDirectory.absolutePath()));
    }
#endif
    m_sysLocaleApp = workingDirectory.absoluteFilePath(sysLocaleApp);
    QVERIFY2(QFileInfo(m_sysLocaleApp).exists(),
             qPrintable(QString::fromLatin1("SysLocalApp executable '%1' does not exist!")
                        .arg(QDir::toNativeSeparators(m_sysLocaleApp))));
}

void tst_QLocale::ctor()
{
    QLocale default_locale = QLocale::system();
    QLocale::Language default_lang = default_locale.language();
    QLocale::Country default_country = default_locale.country();

    qDebug("Default: %s/%s", QLocale::languageToString(default_lang).toLatin1().constData(),
            QLocale::countryToString(default_country).toLatin1().constData());

    {
        QLocale l;
        QVERIFY(l.language() == default_lang);
        QVERIFY(l.country() == default_country);
    }

#define TEST_CTOR(req_lang, req_country, exp_lang, exp_country) \
    { \
        QLocale l(QLocale::req_lang, QLocale::req_country); \
        QCOMPARE(l.language(), exp_lang); \
        QCOMPARE(l.country(), exp_country); \
    }
    {
        QLocale l(QLocale::C, QLocale::AnyCountry);
        QCOMPARE(l.language(), QLocale::C);
        QCOMPARE(l.country(), QLocale::AnyCountry);
    }
    TEST_CTOR(AnyLanguage, AnyCountry, default_lang, default_country)
    TEST_CTOR(C, AnyCountry, QLocale::C, QLocale::AnyCountry)
    TEST_CTOR(Aymara, AnyCountry, default_lang, default_country)
    TEST_CTOR(Aymara, France, default_lang, default_country)

    TEST_CTOR(English, AnyCountry, QLocale::English, QLocale::UnitedStates)
    TEST_CTOR(English, UnitedStates, QLocale::English, QLocale::UnitedStates)
    TEST_CTOR(English, France, QLocale::English, QLocale::UnitedStates)
    TEST_CTOR(English, UnitedKingdom, QLocale::English, QLocale::UnitedKingdom)

    TEST_CTOR(French, France, QLocale::French, QLocale::France)
    TEST_CTOR(C, France, QLocale::C, QLocale::AnyCountry)
    TEST_CTOR(Spanish, LatinAmericaAndTheCaribbean, QLocale::Spanish, QLocale::LatinAmericaAndTheCaribbean)

    QLocale::setDefault(QLocale(QLocale::English, QLocale::France));

    {
        QLocale l;
        QVERIFY(l.language() == QLocale::English);
        QVERIFY(l.country() == QLocale::UnitedStates);
    }

    TEST_CTOR(French, France, QLocale::French, QLocale::France)
    TEST_CTOR(English, UnitedKingdom, QLocale::English, QLocale::UnitedKingdom)

    TEST_CTOR(French, France, QLocale::French, QLocale::France)
    TEST_CTOR(C, AnyCountry, QLocale::C, QLocale::AnyCountry)
    TEST_CTOR(C, France, QLocale::C, QLocale::AnyCountry)
    TEST_CTOR(Aymara, AnyCountry, QLocale::English, QLocale::UnitedStates)

    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedKingdom));

    {
        QLocale l;
        QVERIFY(l.language() == QLocale::English);
        QVERIFY(l.country() == QLocale::UnitedKingdom);
    }

    TEST_CTOR(French, France, QLocale::French, QLocale::France)
    TEST_CTOR(English, UnitedKingdom, QLocale::English, QLocale::UnitedKingdom)

    TEST_CTOR(C, AnyCountry, QLocale::C, QLocale::AnyCountry)
    TEST_CTOR(C, France, QLocale::C, QLocale::AnyCountry)

    QLocale::setDefault(QLocale(QLocale::Aymara, QLocale::France));

    {
        QLocale l;
        QVERIFY(l.language() == QLocale::English);
        QVERIFY(l.country() == QLocale::UnitedKingdom);
    }

    TEST_CTOR(Aymara, AnyCountry, QLocale::English, QLocale::UnitedKingdom)
    TEST_CTOR(Aymara, France, QLocale::English, QLocale::UnitedKingdom)

    TEST_CTOR(English, AnyCountry, QLocale::English, QLocale::UnitedStates)
    TEST_CTOR(English, UnitedStates, QLocale::English, QLocale::UnitedStates)
    TEST_CTOR(English, France, QLocale::English, QLocale::UnitedStates)
    TEST_CTOR(English, UnitedKingdom, QLocale::English, QLocale::UnitedKingdom)

    TEST_CTOR(French, France, QLocale::French, QLocale::France)
    TEST_CTOR(C, AnyCountry, QLocale::C, QLocale::AnyCountry)
    TEST_CTOR(C, France, QLocale::C, QLocale::AnyCountry)

    QLocale::setDefault(QLocale(QLocale::Aymara, QLocale::AnyCountry));

    {
        QLocale l;
        QVERIFY(l.language() == QLocale::English);
        QVERIFY(l.country() == QLocale::UnitedKingdom);
    }

    TEST_CTOR(Aymara, AnyCountry, QLocale::English, QLocale::UnitedKingdom)
    TEST_CTOR(Aymara, France, QLocale::English, QLocale::UnitedKingdom)

    TEST_CTOR(English, AnyCountry, QLocale::English, QLocale::UnitedStates)
    TEST_CTOR(English, UnitedStates, QLocale::English, QLocale::UnitedStates)
    TEST_CTOR(English, France, QLocale::English, QLocale::UnitedStates)
    TEST_CTOR(English, UnitedKingdom, QLocale::English, QLocale::UnitedKingdom)

    TEST_CTOR(French, France, QLocale::French, QLocale::France)
    TEST_CTOR(C, AnyCountry, QLocale::C, QLocale::AnyCountry)
    TEST_CTOR(C, France, QLocale::C, QLocale::AnyCountry)

    TEST_CTOR(Arabic, AnyCountry, QLocale::Arabic, QLocale::Egypt)
    TEST_CTOR(Dutch, AnyCountry, QLocale::Dutch, QLocale::Netherlands)
    TEST_CTOR(German, AnyCountry, QLocale::German, QLocale::Germany)
    TEST_CTOR(Greek, AnyCountry, QLocale::Greek, QLocale::Greece)
    TEST_CTOR(Malay, AnyCountry, QLocale::Malay, QLocale::Malaysia)
    TEST_CTOR(Persian, AnyCountry, QLocale::Persian, QLocale::Iran)
    TEST_CTOR(Portuguese, AnyCountry, QLocale::Portuguese, QLocale::Brazil)
    TEST_CTOR(Serbian, AnyCountry, QLocale::Serbian, QLocale::Serbia)
    TEST_CTOR(Somali, AnyCountry, QLocale::Somali, QLocale::Somalia)
    TEST_CTOR(Spanish, AnyCountry, QLocale::Spanish, QLocale::Spain)
    TEST_CTOR(Swedish, AnyCountry, QLocale::Swedish, QLocale::Sweden)
    TEST_CTOR(Uzbek, AnyCountry, QLocale::Uzbek, QLocale::Uzbekistan)

#undef TEST_CTOR

#define TEST_CTOR(req_lc, exp_lang, exp_country) \
    { \
        QLocale l(req_lc); \
        QVERIFY2(l.language() == QLocale::exp_lang \
            && l.country() == QLocale::exp_country, \
            QString("requested: \"" + QString(req_lc) + "\", got: " \
            + QLocale::languageToString(l.language()) \
            + "/" + QLocale::countryToString(l.country())).toLatin1().constData()); \
    }

    QLocale::setDefault(QLocale(QLocale::C));

    TEST_CTOR("C", C, AnyCountry)
    TEST_CTOR("bla", C, AnyCountry)
    TEST_CTOR("zz", C, AnyCountry)
    TEST_CTOR("zz_zz", C, AnyCountry)
    TEST_CTOR("zz...", C, AnyCountry)
    TEST_CTOR("", C, AnyCountry)
    TEST_CTOR("en/", C, AnyCountry)
    TEST_CTOR(QString::null, C, AnyCountry)
    TEST_CTOR("en", English, UnitedStates)
    TEST_CTOR("en", English, UnitedStates)
    TEST_CTOR("en.", English, UnitedStates)
    TEST_CTOR("en@", English, UnitedStates)
    TEST_CTOR("en.@", English, UnitedStates)
    TEST_CTOR("en_", English, UnitedStates)
    TEST_CTOR("en_U", English, UnitedStates)
    TEST_CTOR("en_.", English, UnitedStates)
    TEST_CTOR("en_.@", English, UnitedStates)
    TEST_CTOR("en.bla", English, UnitedStates)
    TEST_CTOR("en@bla", English, UnitedStates)
    TEST_CTOR("en_blaaa", English, UnitedStates)
    TEST_CTOR("en_zz", English, UnitedStates)
    TEST_CTOR("en_GB", English, UnitedKingdom)
    TEST_CTOR("en_GB.bla", English, UnitedKingdom)
    TEST_CTOR("en_GB@.bla", English, UnitedKingdom)
    TEST_CTOR("en_GB@bla", English, UnitedKingdom)
    TEST_CTOR("en-GB", English, UnitedKingdom)
    TEST_CTOR("en-GB@bla", English, UnitedKingdom)

    QVERIFY(QLocale::Norwegian == QLocale::NorwegianBokmal);
    TEST_CTOR("no", Norwegian, Norway)
    TEST_CTOR("nb", Norwegian, Norway)
    TEST_CTOR("nn", NorwegianNynorsk, Norway)
    TEST_CTOR("no_NO", Norwegian, Norway)
    TEST_CTOR("nb_NO", Norwegian, Norway)
    TEST_CTOR("nn_NO", NorwegianNynorsk, Norway)
    TEST_CTOR("es_ES", Spanish, Spain)
    TEST_CTOR("es_419", Spanish, LatinAmericaAndTheCaribbean)
    TEST_CTOR("es-419", Spanish, LatinAmericaAndTheCaribbean)
    TEST_CTOR("fr_MA", French, Morocco)

    // test default countries for languages
    TEST_CTOR("zh", Chinese, China)
    TEST_CTOR("zh-Hans", Chinese, China)
    TEST_CTOR("mn", Mongolian, Mongolia)
    TEST_CTOR("ne", Nepali, Nepal)

#undef TEST_CTOR
#define TEST_CTOR(req_lc, exp_lang, exp_script, exp_country) \
    { \
    QLocale l(req_lc); \
    QVERIFY2(l.language() == QLocale::exp_lang \
        && l.script() == QLocale::exp_script \
        && l.country() == QLocale::exp_country, \
        QString("requested: \"" + QString(req_lc) + "\", got: " \
        + QLocale::languageToString(l.language()) \
        + "/" + QLocale::scriptToString(l.script()) \
        + "/" + QLocale::countryToString(l.country())).toLatin1().constData()); \
    }

    TEST_CTOR("zh_CN", Chinese, SimplifiedHanScript, China)
    TEST_CTOR("zh_Hans_CN", Chinese, SimplifiedHanScript, China)
    TEST_CTOR("zh_Hans", Chinese, SimplifiedHanScript, China)
    TEST_CTOR("zh_Hant", Chinese, TraditionalHanScript, Taiwan)
    TEST_CTOR("zh_Hans_MO", Chinese, SimplifiedHanScript, Macau)
    TEST_CTOR("zh_Hant_MO", Chinese, TraditionalHanScript, Macau)
    TEST_CTOR("az_Latn_AZ", Azerbaijani, LatinScript, Azerbaijan)
    TEST_CTOR("ha_Latn_NG", Hausa, LatinScript, Nigeria)

    TEST_CTOR("ru", Russian, CyrillicScript, RussianFederation)
    TEST_CTOR("ru_Cyrl", Russian, CyrillicScript, RussianFederation)

#undef TEST_CTOR
}

void tst_QLocale::emptyCtor()
{
#if defined(Q_OS_WINCE)
    QSKIP("Uses unsupported Windows CE / Symbian QProcess functionality (std streams, env)", SkipAll);
#endif
#if defined(QT_NO_PROCESS)
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
#define TEST_CTOR(req_lc, exp_str) \
    { \
    /* Test constructor without arguments. Needs separate process */ \
    /* because of caching of the system locale. */ \
    QProcess process; \
    process.setEnvironment(QStringList(env) << QString("LANG=%1").arg(req_lc)); \
    process.start(m_sysLocaleApp); \
    QVERIFY(process.waitForStarted()); \
    process.waitForReadyRead(); \
    QString ret = QString(process.readAll()); \
    process.waitForFinished(); \
    QVERIFY2(!ret.isEmpty(), "Cannot launch external process"); \
    QVERIFY2(QString(exp_str) == ret, QString("Expected: " + QString(exp_str) + ", got: " \
            + ret + ". Requested: " + QString(req_lc)).toLatin1().constData()); \
    }

    // Get an environment free of any locale-related variables
    QStringList env;
    foreach (QString const& entry, QProcess::systemEnvironment()) {
        if (entry.startsWith("LANG=") || entry.startsWith("LC_"))
            continue;
        env << entry;
    }

    // Get default locale.
    QProcess p;
    p.setEnvironment(env);
    p.start("syslocaleapp/syslocaleapp");
    p.waitForReadyRead();
    QString defaultLoc = QString(p.readAll());
    p.waitForFinished();

    TEST_CTOR("C", "C")
    TEST_CTOR("bla", "C")
    TEST_CTOR("zz", "C")
    TEST_CTOR("zz_zz", "C")
    TEST_CTOR("zz...", "C")
    TEST_CTOR("en", "en_US")
    TEST_CTOR("en", "en_US")
    TEST_CTOR("en.", "en_US")
    TEST_CTOR("en@", "en_US")
    TEST_CTOR("en.@", "en_US")
    TEST_CTOR("en_", "en_US")
    TEST_CTOR("en_.", "en_US")
    TEST_CTOR("en_.@", "en_US")
    TEST_CTOR("en.bla", "en_US")
    TEST_CTOR("en@bla", "en_US")
    TEST_CTOR("en_blaaa", "en_US")
    TEST_CTOR("en_zz", "en_US")
    TEST_CTOR("en_GB", "en_GB")
    TEST_CTOR("en_GB.bla", "en_GB")
    TEST_CTOR("en_GB@.bla", "en_GB")
    TEST_CTOR("en_GB@bla", "en_GB")
    TEST_CTOR("de", "de_DE")

    QVERIFY(QLocale::Norwegian == QLocale::NorwegianBokmal);
    TEST_CTOR("no", "nb_NO")
    TEST_CTOR("nb", "nb_NO")
    TEST_CTOR("nn", "nn_NO")
    TEST_CTOR("no_NO", "nb_NO")
    TEST_CTOR("nb_NO", "nb_NO")
    TEST_CTOR("nn_NO", "nn_NO")

    TEST_CTOR("DE", "de_DE");
    TEST_CTOR("EN", "en_US");

    TEST_CTOR("en/", defaultLoc)
    TEST_CTOR("asdfghj", defaultLoc);
    TEST_CTOR("123456", defaultLoc);

#undef TEST_CTOR
#endif
}

void tst_QLocale::consistentC()
{
    const QLocale c(QLocale::C);
    QCOMPARE(c, QLocale::c());
    QCOMPARE(c, QLocale(QLocale::C, QLocale::AnyScript, QLocale::AnyCountry));
    QVERIFY(QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript,
                                     QLocale::AnyCountry).contains(c));
}

void tst_QLocale::unixLocaleName()
{
#define TEST_NAME(req_lang, req_country, exp_name) \
    { \
    	QLocale l(QLocale::req_lang, QLocale::req_country); \
	QCOMPARE(l.name(), QString(exp_name)); \
    }

    QLocale::setDefault(QLocale(QLocale::C));

    TEST_NAME(C, AnyCountry, "C")
    TEST_NAME(English, AnyCountry, "en_US")
    TEST_NAME(English, UnitedKingdom, "en_GB")
    TEST_NAME(Aymara, UnitedKingdom, "C")

#undef TEST_NAME
}

void tst_QLocale::toReal_data()
{
    QTest::addColumn<QString>("locale_name");
    QTest::addColumn<QString>("num_str");
    QTest::addColumn<bool>("good");
    QTest::addColumn<double>("num");

    QTest::newRow("C 1")         << QString("C") << QString("1")          << true  << 1.0;
    QTest::newRow("C 1.0")       << QString("C") << QString("1.0")        << true  << 1.0;
    QTest::newRow("C 1.234")     << QString("C") << QString("1.234")      << true  << 1.234;
    QTest::newRow("C 1.234e-10") << QString("C") << QString("1.234e-10")  << true  << 1.234e-10;
    QTest::newRow("C 1.234E10")  << QString("C") << QString("1.234E10")   << true  << 1.234e10;
    QTest::newRow("C 1e10")      << QString("C") << QString("1e10")       << true  << 1.0e10;
    QTest::newRow("C  1")        << QString("C") << QString(" 1")         << true  << 1.0;
    QTest::newRow("C   1")       << QString("C") << QString("  1")        << true  << 1.0;
    QTest::newRow("C 1 ")        << QString("C") << QString("1 ")         << true  << 1.0;
    QTest::newRow("C 1  ")       << QString("C") << QString("1  ")        << true  << 1.0;

    QTest::newRow("C 1,")	        << QString("C") << QString("1,")	 << false << 0.0;
    QTest::newRow("C 1,2")	        << QString("C") << QString("1,2")	 << false << 0.0;
    QTest::newRow("C 1,23")        << QString("C") << QString("1,23")	 << false << 0.0;
    QTest::newRow("C 1,234")       << QString("C") << QString("1,234")	 << true  << 1234.0;
    QTest::newRow("C 1,234,")      << QString("C") << QString("1,234,")	 << false << 0.0;
    QTest::newRow("C 1,234,5")     << QString("C") << QString("1,234,5")	 << false << 0.0;
    QTest::newRow("C 1,234,56")    << QString("C") << QString("1,234,56")   << false << 0.0;
    QTest::newRow("C 1,234,567")   << QString("C") << QString("1,234,567")  << true  << 1234567.0;
    QTest::newRow("C 1,234,567.")  << QString("C") << QString("1,234,567.") << true  << 1234567.0;
    QTest::newRow("C 1,234,567.8")	<< QString("C") << QString("1,234,567.8")<< true  << 1234567.8;
    QTest::newRow("C 1,234567.8")  << QString("C") << QString("1,234567.8") << false << 0.0;
    QTest::newRow("C 12,34567.8")  << QString("C") << QString("12,34567.8") << false << 0.0;
    QTest::newRow("C 1234,567.8")  << QString("C") << QString("1234,567.8") << false << 0.0;
    QTest::newRow("C 1234567.8")   << QString("C") << QString("1234567.8")  << true  << 1234567.8;
    QTest::newRow("C ,")	    	<< QString("C") << QString(",")	         << false << 0.0;
    QTest::newRow("C ,123")	<< QString("C") << QString(",123")       << false << 0.0;
    QTest::newRow("C ,3")	    	<< QString("C") << QString(",3")         << false << 0.0;
    QTest::newRow("C , 3")	    	<< QString("C") << QString(", 3")        << false << 0.0;
    QTest::newRow("C ,  3")	<< QString("C") << QString(",  3")       << false << 0.0;
    QTest::newRow("C ,  3.2")	<< QString("C") << QString(",  3.2")     << false << 0.0;
    QTest::newRow("C ,  3.2e2")	<< QString("C") << QString(",  3.2e2")   << false << 0.0;
    QTest::newRow("C ,  e2")	<< QString("C") << QString(",  e2")	 << false << 0.0;
    QTest::newRow("C 1,,234")	<< QString("C") << QString("1,,234")	 << false << 0.0;

    QTest::newRow("C empty")     << QString("C") << QString("")           << false << 0.0;
    QTest::newRow("C null")      << QString("C") << QString()         << false << 0.0;
    QTest::newRow("C .")         << QString("C") << QString(".")          << false << 0.0;
    QTest::newRow("C 1e")        << QString("C") << QString("1e")         << false << 0.0;
    QTest::newRow("C 1,0")       << QString("C") << QString("1,0")        << false << 0.0;
    QTest::newRow("C 1,000")     << QString("C") << QString("1,000")      << true  << 1000.0;
    QTest::newRow("C 1,000e-6")  << QString("C") << QString("1,000e-6")   << true  << 1000.0e-6;
    QTest::newRow("C 1e1.0")     << QString("C") << QString("1e1.0")      << false << 0.0;
    QTest::newRow("C 1e+")       << QString("C") << QString("1e+")        << false << 0.0;
    QTest::newRow("C 1e-")       << QString("C") << QString("1e-")        << false << 0.0;

    QTest::newRow("C .1")        << QString("C") << QString(".1")         << true  << 0.1;
    QTest::newRow("C -.1")       << QString("C") << QString("-.1")        << true  << -0.1;
    QTest::newRow("C 1.")        << QString("C") << QString("1.")         << true  << 1.0;
    QTest::newRow("C 1.E10")     << QString("C") << QString("1.E10")      << true  << 1.0e10;
    QTest::newRow("C 1e+10")     << QString("C") << QString("1e+10")      << true  << 1.0e+10;

    QTest::newRow("de_DE 1.")	    << QString("de_DE") << QString("1.")	 << false << 0.0;
    QTest::newRow("de_DE 1.2")	    << QString("de_DE") << QString("1.2")	 << false << 0.0;
    QTest::newRow("de_DE 1.23")        << QString("de_DE") << QString("1.23")       << false << 0.0;
    QTest::newRow("de_DE 1.234")       << QString("de_DE") << QString("1.234")	 << true  << 1234.0;
    QTest::newRow("de_DE 1.234,")      << QString("de_DE") << QString("1.234.")	 << false << 0.0;
    QTest::newRow("de_DE 1.234.5")     << QString("de_DE") << QString("1.234.5")	 << false << 0.0;
    QTest::newRow("de_DE 1.234.56")    << QString("de_DE") << QString("1.234.56")   << false << 0.0;
    QTest::newRow("de_DE 1.234.567")   << QString("de_DE") << QString("1.234.567")  << true  << 1234567.0;
    QTest::newRow("de_DE 1.234.567,")  << QString("de_DE") << QString("1.234.567,") << true  << 1234567.0;
    QTest::newRow("de_DE 1.234.567,8") << QString("de_DE") << QString("1.234.567,8")<< true  << 1234567.8;
    QTest::newRow("de_DE 1.234567,8")  << QString("de_DE") << QString("1.234567,8") << false << 0.0;
    QTest::newRow("de_DE 12.34567,8")  << QString("de_DE") << QString("12.34567,8") << false << 0.0;
    QTest::newRow("de_DE 1234.567,8")  << QString("de_DE") << QString("1234.567,8") << false << 0.0;
    QTest::newRow("de_DE 1234567,8")   << QString("de_DE") << QString("1234567,8")  << true  << 1234567.8;
    QTest::newRow("de_DE .123")	    << QString("de_DE") << QString(".123")       << false << 0.0;
    QTest::newRow("de_DE .3")	    << QString("de_DE") << QString(".3")         << false << 0.0;
    QTest::newRow("de_DE . 3")	    << QString("de_DE") << QString(". 3")        << false << 0.0;
    QTest::newRow("de_DE .  3")	    << QString("de_DE") << QString(".  3")       << false << 0.0;
    QTest::newRow("de_DE .  3,2")	    << QString("de_DE") << QString(".  3,2")     << false << 0.0;
    QTest::newRow("de_DE .  3,2e2")    << QString("de_DE") << QString(".  3,2e2")   << false << 0.0;
    QTest::newRow("de_DE .  e2")	    << QString("de_DE") << QString(".  e2")	 << false << 0.0;
    QTest::newRow("de_DE 1..234")	    << QString("de_DE") << QString("1..234")	 << false << 0.0;

    QTest::newRow("de_DE 1")         << QString("de_DE") << QString("1")          << true  << 1.0;
    QTest::newRow("de_DE 1.0")       << QString("de_DE") << QString("1.0")        << false << 0.0;
    QTest::newRow("de_DE 1.234e-10") << QString("de_DE") << QString("1.234e-10")  << true  << 1234.0e-10;
    QTest::newRow("de_DE 1.234E10")  << QString("de_DE") << QString("1.234E10")   << true  << 1234.0e10;
    QTest::newRow("de_DE 1e10")      << QString("de_DE") << QString("1e10")       << true  << 1.0e10;
    QTest::newRow("de_DE .1")        << QString("de_DE") << QString(".1")         << false << 0.0;
    QTest::newRow("de_DE -.1")       << QString("de_DE") << QString("-.1")        << false << 0.0;
    QTest::newRow("de_DE 1.E10")     << QString("de_DE") << QString("1.E10")      << false << 0.0;
    QTest::newRow("de_DE 1e+10")     << QString("de_DE") << QString("1e+10")      << true  << 1.0e+10;

    QTest::newRow("de_DE 1,0")       << QString("de_DE") << QString("1,0")        << true  << 1.0;
    QTest::newRow("de_DE 1,234")     << QString("de_DE") << QString("1,234")      << true  << 1.234;
    QTest::newRow("de_DE 1,234e-10") << QString("de_DE") << QString("1,234e-10")  << true  << 1.234e-10;
    QTest::newRow("de_DE 1,234E10")  << QString("de_DE") << QString("1,234E10")   << true  << 1.234e10;
    QTest::newRow("de_DE ,1")        << QString("de_DE") << QString(",1")         << true  << 0.1;
    QTest::newRow("de_DE -,1")       << QString("de_DE") << QString("-,1")        << true  << -0.1;
    QTest::newRow("de_DE 1,")        << QString("de_DE") << QString("1,")         << true  << 1.0;
    QTest::newRow("de_DE 1,E10")     << QString("de_DE") << QString("1,E10")      << true  << 1.0e10;

    QTest::newRow("de_DE empty")     << QString("de_DE") << QString("")           << false << 0.0;
    QTest::newRow("de_DE null")      << QString("de_DE") << QString()         << false << 0.0;
    QTest::newRow("de_DE .")         << QString("de_DE") << QString(".")          << false << 0.0;
    QTest::newRow("de_DE 1e")        << QString("de_DE") << QString("1e")         << false << 0.0;
    QTest::newRow("de_DE 1e1.0")     << QString("de_DE") << QString("1e1.0")      << false << 0.0;
    QTest::newRow("de_DE 1e+")       << QString("de_DE") << QString("1e+")        << false << 0.0;
    QTest::newRow("de_DE 1e-")       << QString("de_DE") << QString("1e-")        << false << 0.0;

    QTest::newRow("C 9,876543")      << QString("C") << QString("9,876543")        << false << 0.0;
    QTest::newRow("C 9,876543.2")    << QString("C") << QString("9,876543.2")      << false << 0.0;
    QTest::newRow("C 9,876543e-2")   << QString("C") << QString("9,876543e-2")     << false << 0.0;
    QTest::newRow("C 9,876543.0e-2") << QString("C") << QString("9,876543.0e-2")   << false << 0.0;

    QTest::newRow("de_DE 9.876543")      << QString("de_DE") << QString("9876.543")        << false << 0.0;
    QTest::newRow("de_DE 9.876543,2")    << QString("de_DE") << QString("9.876543,2")      << false << 0.0;
    QTest::newRow("de_DE 9.876543e-2")   << QString("de_DE") << QString("9.876543e-2")     << false << 0.0;
    QTest::newRow("de_DE 9.876543,0e-2") << QString("de_DE") << QString("9.876543,0e-2")   << false << 0.0;
    QTest::newRow("de_DE 9.876543e--2")   << QString("de_DE") << QString("9.876543e")+QChar(8722)+QString("2")     << false << 0.0;
    QTest::newRow("de_DE 9.876543,0e--2") << QString("de_DE") << QString("9.876543,0e")+QChar(8722)+QString("2")   << false << 0.0;
}

void tst_QLocale::stringToDouble_data()
{
    toReal_data();
    if (std::numeric_limits<double>::has_infinity) {
        double huge = std::numeric_limits<double>::infinity();
        QTest::newRow("C inf") << QString("C") << QString("inf") << true << huge;
        QTest::newRow("C +inf") << QString("C") << QString("+inf") << true << +huge;
        QTest::newRow("C -inf") << QString("C") << QString("-inf") << true << -huge;
        // Overflow:
        QTest::newRow("C huge") << QString("C") << QString("2e308") << false << huge;
        QTest::newRow("C -huge") << QString("C") << QString("-2e308") << false << -huge;
    }
    if (std::numeric_limits<double>::has_quiet_NaN)
        QTest::newRow("C qnan") << QString("C") << QString("NaN") << true << std::numeric_limits<double>::quiet_NaN();

    // In range (but outside float's range):
    QTest::newRow("C big") << QString("C") << QString("3.5e38") << true << 3.5e38;
    QTest::newRow("C -big") << QString("C") << QString("-3.5e38") << true << -3.5e38;
    QTest::newRow("C small") << QString("C") << QString("1e-45") << true << 1e-45;
    QTest::newRow("C -small") << QString("C") << QString("-1e-45") << true << -1e-45;

    // Underflow:
    QTest::newRow("C tiny") << QString("C") << QString("2e-324") << false << 0.;
    QTest::newRow("C -tiny") << QString("C") << QString("-2e-324") << false << 0.;
}

void tst_QLocale::stringToDouble()
{
#define MY_DOUBLE_EPSILON (2.22045e-16) // 1/2^{52}; double has a 53-bit mantissa

    QFETCH(QString, locale_name);
    QFETCH(QString, num_str);
    QFETCH(bool, good);
    QFETCH(double, num);
    QStringRef num_strRef = num_str.leftRef(-1);

    QLocale locale(locale_name);
    QCOMPARE(locale.name(), locale_name);

    bool ok;
    double d = locale.toDouble(num_str, &ok);
    QCOMPARE(ok, good);

    {
        // Make sure result is independent of locale:
        TransientLocale ignoreme(LC_ALL, "ar_SA");
        QCOMPARE(locale.toDouble(num_str, &ok), d);
        QCOMPARE(ok, good);
    }

    if (ok || std::isinf(num)) {
        // First use fuzzy-compare, then a more precise check:
        QCOMPARE(d, num);
        if (std::isfinite(num)) {
            double diff = d > num ? d - num : num - d;
            QVERIFY(diff <= MY_DOUBLE_EPSILON);
        }
    }

    d = locale.toDouble(num_str, &ok);
    QCOMPARE(ok, good);

    if (ok || std::isinf(num)) {
        QCOMPARE(d, num);
        if (std::isfinite(num)) {
            double diff = d > num ? d - num : num - d;
            QVERIFY(diff <= MY_DOUBLE_EPSILON);
        }
    }
#undef MY_DOUBLE_EPSILON
}

void tst_QLocale::stringToFloat_data()
{
    using Bounds = std::numeric_limits<float>;
    toReal_data();
    const QString C(QStringLiteral("C"));
    if (Bounds::has_infinity) {
        double huge = Bounds::infinity();
        QTest::newRow("C inf") << C << QString("inf") << true << huge;
        QTest::newRow("C +inf") << C << QString("+inf") << true << +huge;
        QTest::newRow("C -inf") << C << QString("-inf") << true << -huge;
        // Overflow float, but not double:
        QTest::newRow("C big") << C << QString("3.5e38") << false << huge;
        QTest::newRow("C -big") << C << QString("-3.5e38") << false << -huge;
        // Overflow double, too:
        QTest::newRow("C huge") << C << QString("2e308") << false << huge;
        QTest::newRow("C -huge") << C << QString("-2e308") << false << -huge;
    }
    if (Bounds::has_quiet_NaN)
        QTest::newRow("C qnan") << C << QString("NaN") << true << double(Bounds::quiet_NaN());

    // Minimal float: shouldn't underflow
    QTest::newRow("C float min")
        << C << QLocale::c().toString(Bounds::denorm_min()) << true << double(Bounds::denorm_min());
    QTest::newRow("C float -min")
        << C << QLocale::c().toString(-Bounds::denorm_min()) << true << -double(Bounds::denorm_min());

    // Underflow float, but not double:
    QTest::newRow("C small") << C << QString("7e-46") << false << 0.;
    QTest::newRow("C -small") << C << QString("-7e-46") << false << 0.;
    using Double = std::numeric_limits<double>;
    QTest::newRow("C double min")
        << C << QLocale::c().toString(Double::denorm_min()) << false << 0.0;
    QTest::newRow("C double -min")
        << C << QLocale::c().toString(-Double::denorm_min()) << false << 0.0;

    // Underflow double, too:
    QTest::newRow("C tiny") << C << QString("2e-324") << false << 0.;
    QTest::newRow("C -tiny") << C << QString("-2e-324") << false << 0.;
}

void tst_QLocale::stringToFloat()
{
#define MY_FLOAT_EPSILON (2.384e-7) // 1/2^{22}; float has a 23-bit mantissa

    QFETCH(QString, locale_name);
    QFETCH(QString, num_str);
    QFETCH(bool, good);
    QFETCH(double, num);
    QStringRef num_strRef = num_str.leftRef(-1);
    float fnum = num;

    QLocale locale(locale_name);
    QCOMPARE(locale.name(), locale_name);

    bool ok;
    float f = locale.toFloat(num_str, &ok);
    QCOMPARE(ok, good);

    {
        // Make sure result is independent of locale:
        TransientLocale ignoreme(LC_ALL, "ar_SA");
        QCOMPARE(locale.toFloat(num_str, &ok), f);
        QCOMPARE(ok, good);
    }

    if (ok || std::isinf(fnum)) {
        // First use fuzzy-compare, then a more precise check:
        QCOMPARE(f, fnum);
        if (std::isfinite(fnum)) {
            float diff = f > fnum ? f - fnum : fnum - f;
            QVERIFY(diff <= MY_FLOAT_EPSILON);
        }
    }

    f = locale.toFloat(num_strRef, &ok);
    QCOMPARE(ok, good);

    if (ok || std::isinf(fnum)) {
        QCOMPARE(f, fnum);
        if (std::isfinite(fnum)) {
            float diff = f > fnum ? f - fnum : fnum - f;
            QVERIFY(diff <= MY_FLOAT_EPSILON);
        }
    }
#undef MY_FLOAT_EPSILON
}

void tst_QLocale::doubleToString_data()
{
    QTest::addColumn<QString>("locale_name");
    QTest::addColumn<QString>("num_str");
    QTest::addColumn<double>("num");
    QTest::addColumn<char>("mode");
    QTest::addColumn<int>("precision");

    QTest::newRow("C 3.4 f 5")  << QString("C") << QString("3.40000")     << 3.4 << 'f' << 5;
    QTest::newRow("C 3.4 f 0")  << QString("C") << QString("3")           << 3.4 << 'f' << 0;
    QTest::newRow("C 3.4 e 5")  << QString("C") << QString("3.40000e+00") << 3.4 << 'e' << 5;
    QTest::newRow("C 3.4 e 0")  << QString("C") << QString("3e+00")       << 3.4 << 'e' << 0;
    QTest::newRow("C 3.4 g 5")  << QString("C") << QString("3.4")         << 3.4 << 'g' << 5;
    QTest::newRow("C 3.4 g 1")  << QString("C") << QString("3")           << 3.4 << 'g' << 1;

    QTest::newRow("C 3.4 f 1")  << QString("C") << QString("3.4")     << 3.4 << 'f' << 1;
    QTest::newRow("C 3.4 e 1")  << QString("C") << QString("3.4e+00") << 3.4 << 'e' << 1;
    QTest::newRow("C 3.4 g 2")  << QString("C") << QString("3.4")     << 3.4 << 'g' << 2;

    QTest::newRow("de_DE 3,4 f 1")  << QString("de_DE") << QString("3,4")     << 3.4 << 'f' << 1;
    QTest::newRow("de_DE 3,4 e 1")  << QString("de_DE") << QString("3,4e+00") << 3.4 << 'e' << 1;
    QTest::newRow("de_DE 3,4 g 2")  << QString("de_DE") << QString("3,4")     << 3.4 << 'g' << 2;

    QTest::newRow("C 0.035003945 f 12") << QString("C") << QString("0.035003945000")   << 0.035003945 << 'f' << 12;
    QTest::newRow("C 0.035003945 f 6")  << QString("C") << QString("0.035004")         << 0.035003945 << 'f' << 6;
    QTest::newRow("C 0.035003945 e 10") << QString("C") << QString("3.5003945000e-02") << 0.035003945 << 'e' << 10;
    QTest::newRow("C 0.035003945 e 4")  << QString("C") << QString("3.5004e-02")       << 0.035003945 << 'e' << 4;
    QTest::newRow("C 0.035003945 g 11") << QString("C") << QString("0.035003945")      << 0.035003945 << 'g' << 11;
    QTest::newRow("C 0.035003945 g 5")  << QString("C") << QString("0.035004")         << 0.035003945 << 'g' << 5;

    QTest::newRow("C 0.035003945 f 9")  << QString("C") << QString("0.035003945")   << 0.035003945 << 'f' << 9;
    QTest::newRow("C 0.035003945 e 7")  << QString("C") << QString("3.5003945e-02") << 0.035003945 << 'e' << 7;
    QTest::newRow("C 0.035003945 g 8")  << QString("C") << QString("0.035003945")   << 0.035003945 << 'g' << 8;

    QTest::newRow("de_DE 0,035003945 f 9")  << QString("de_DE") << QString("0,035003945")   << 0.035003945 << 'f' << 9;
    QTest::newRow("de_DE 0,035003945 e 7")  << QString("de_DE") << QString("3,5003945e-02") << 0.035003945 << 'e' << 7;
    QTest::newRow("de_DE 0,035003945 g 8")  << QString("de_DE") << QString("0,035003945")   << 0.035003945 << 'g' << 8;

    QTest::newRow("C 0.000003945 f 12") << QString("C") << QString("0.000003945000") << 0.000003945 << 'f' << 12;
    QTest::newRow("C 0.000003945 f 6")  << QString("C") << QString("0.000004")       << 0.000003945 << 'f' << 6;
    QTest::newRow("C 0.000003945 e 6")  << QString("C") << QString("3.945000e-06")   << 0.000003945 << 'e' << 6;
    QTest::newRow("C 0.000003945 e 0")  << QString("C") << QString("4e-06")          << 0.000003945 << 'e' << 0;
    QTest::newRow("C 0.000003945 g 7")  << QString("C") << QString("3.945e-06")      << 0.000003945 << 'g' << 7;
    QTest::newRow("C 0.000003945 g 1")  << QString("C") << QString("4e-06")          << 0.000003945 << 'g' << 1;

    QTest::newRow("C 0.000003945 f 9")  << QString("C") << QString("0.000003945") << 0.000003945 << 'f' << 9;
    QTest::newRow("C 0.000003945 e 3")  << QString("C") << QString("3.945e-06")   << 0.000003945 << 'e' << 3;
    QTest::newRow("C 0.000003945 g 4")  << QString("C") << QString("3.945e-06")   << 0.000003945 << 'g' << 4;

    QTest::newRow("de_DE 0,000003945 f 9")  << QString("de_DE") << QString("0,000003945") << 0.000003945 << 'f' << 9;
    QTest::newRow("de_DE 0,000003945 e 3")  << QString("de_DE") << QString("3,945e-06")   << 0.000003945 << 'e' << 3;
    QTest::newRow("de_DE 0,000003945 g 4")  << QString("de_DE") << QString("3,945e-06")   << 0.000003945 << 'g' << 4;

    QTest::newRow("C 12456789012 f 3")  << QString("C") << QString("12456789012.000")     << 12456789012.0 << 'f' << 3;
    QTest::newRow("C 12456789012 e 13") << QString("C") << QString("1.2456789012000e+10") << 12456789012.0 << 'e' << 13;
    QTest::newRow("C 12456789012 e 7")  << QString("C") << QString("1.2456789e+10")       << 12456789012.0 << 'e' << 7;
    QTest::newRow("C 12456789012 g 14") << QString("C") << QString("12456789012")         << 12456789012.0 << 'g' << 14;
    QTest::newRow("C 12456789012 g 8")  << QString("C") << QString("1.2456789e+10")       << 12456789012.0 << 'g' << 8;

    QTest::newRow("C 12456789012 f 0")  << QString("C") << QString("12456789012")      << 12456789012.0 << 'f' << 0;
    QTest::newRow("C 12456789012 e 10") << QString("C") << QString("1.2456789012e+10") << 12456789012.0 << 'e' << 10;
    QTest::newRow("C 12456789012 g 11") << QString("C") << QString("12456789012")      << 12456789012.0 << 'g' << 11;

    QTest::newRow("de_DE 12456789012 f 0")  << QString("de_DE") << QString("12.456.789.012")   << 12456789012.0 << 'f' << 0;
    QTest::newRow("de_DE 12456789012 e 10") << QString("de_DE") << QString("1,2456789012e+10") << 12456789012.0 << 'e' << 10;
    QTest::newRow("de_DE 12456789012 g 11") << QString("de_DE") << QString("12.456.789.012")   << 12456789012.0 << 'g' << 11;
}

void tst_QLocale::doubleToString()
{
    QFETCH(QString, locale_name);
    QFETCH(QString, num_str);
    QFETCH(double, num);
    QFETCH(char, mode);
    QFETCH(int, precision);

    const QLocale locale(locale_name);
    QCOMPARE(locale.toString(num, mode, precision), num_str);

    TransientLocale ignoreme(LC_ALL, "de_DE");
    QCOMPARE(locale.toString(num, mode, precision), num_str);
}

void tst_QLocale::long_long_conversion_data()
{
    QTest::addColumn<QString>("locale_name");
    QTest::addColumn<QString>("num_str");
    QTest::addColumn<bool>("good");
    QTest::addColumn<qlonglong>("num");

    QTest::newRow("C null")       << QString("C") << QString()	   << false  << (qlonglong) 0;
    QTest::newRow("C empty")      << QString("C") << QString("")	   << false  << (qlonglong) 0;
    QTest::newRow("C 1")          << QString("C") << "1"	   << true  << (qlonglong) 1;
    QTest::newRow("C 1,")         << QString("C") << "1,"     << false << (qlonglong) 0;
    QTest::newRow("C 1,2")        << QString("C") << "1,2"    << false << (qlonglong) 0;
    QTest::newRow("C 1,23")       << QString("C") << "1,23"   << false << (qlonglong) 0;
    QTest::newRow("C 1,234")      << QString("C") << "1,234"  << true  << (qlonglong) 1234;
    QTest::newRow("C 1234567")    << QString("C") << "1234567"<< true  << (qlonglong) 1234567;
    QTest::newRow("C 1,234567")    << QString("C") << "1,234567"<< false  << (qlonglong) 0;
    QTest::newRow("C 12,34567")    << QString("C") << "12,34567"<< false  << (qlonglong) 0;
    QTest::newRow("C 123,4567")    << QString("C") << "123,4567"<< false  << (qlonglong) 0;
    QTest::newRow("C 1234,567")    << QString("C") << "1234,567"<< false  << (qlonglong) 0;
    QTest::newRow("C 12345,67")    << QString("C") << "12345,67"<< false  << (qlonglong) 0;
    QTest::newRow("C 123456,7")    << QString("C") << "123456,7"<< false  << (qlonglong) 0;
    QTest::newRow("C 1,234,567")    << QString("C")<< "1,234,567"<< true  << (qlonglong) 1234567;

    QTest::newRow("de_DE 1")      << QString("de_DE") << "1"      << true  << (qlonglong) 1;
    QTest::newRow("de_DE 1.")     << QString("de_DE") << "1."     << false << (qlonglong) 0;
    QTest::newRow("de_DE 1.2")    << QString("de_DE") << "1.2"    << false << (qlonglong) 0;
    QTest::newRow("de_DE 1.23")   << QString("de_DE") << "1.23"   << false << (qlonglong) 0;
    QTest::newRow("de_DE 1.234")  << QString("de_DE") << "1.234"  << true  << (qlonglong) 1234;
    QTest::newRow("de_DE 1234567")     << QString("de_DE") << "1234567"<< true  << (qlonglong) 1234567;
    QTest::newRow("de_DE 1.234567")    << QString("de_DE") << "1.234567"<< false  << (qlonglong) 0;
    QTest::newRow("de_DE 12.34567")    << QString("de_DE") << "12.34567"<< false  << (qlonglong) 0;
    QTest::newRow("de_DE 123.4567")    << QString("de_DE") << "123.4567"<< false  << (qlonglong) 0;
    QTest::newRow("de_DE 1234.567")    << QString("de_DE") << "1234.567"<< false  << (qlonglong) 0;
    QTest::newRow("de_DE 12345.67")    << QString("de_DE") << "12345.67"<< false  << (qlonglong) 0;
    QTest::newRow("de_DE 123456.7")    << QString("de_DE") << "123456.7"<< false  << (qlonglong) 0;
    QTest::newRow("de_DE 1.234.567")   << QString("de_DE")<< "1.234.567"<< true  << (qlonglong) 1234567;
    QTest::newRow("de_DE 1.234.567 ldspcs")   << QString("de_DE")<< "  1.234.567" << true  << (qlonglong) 1234567;
    QTest::newRow("de_DE 1.234.567 trspcs")   << QString("de_DE")<< "1.234.567  "<< true  << (qlonglong) 1234567;
    QTest::newRow("de_DE 1.234.567 ldtrspcs")   << QString("de_DE")<< "  1.234.567  "<< true  << (qlonglong) 1234567;

    // test that space is also accepted whenever QLocale::groupSeparator() == 0xa0 (which looks like space).
    QTest::newRow("nb_NO 123 groupsep")   << QString("nb_NO")<< QString("1")+QChar(0xa0)+QString("234") << true  << (qlonglong) 1234;
    QTest::newRow("nb_NO 123 groupsep_space")   << QString("nb_NO")<< QString("1")+QChar(0x20)+QString("234") << true  << (qlonglong) 1234;

    QTest::newRow("nb_NO 123 ldspcs")   << QString("nb_NO")<< "  123" << true  << (qlonglong) 123;
    QTest::newRow("nb_NO 123 trspcs")   << QString("nb_NO")<< "123  "<< true  << (qlonglong) 123;
    QTest::newRow("nb_NO 123 ldtrspcs")   << QString("nb_NO")<< "  123  "<< true  << (qlonglong) 123;

    QTest::newRow("C   1234")       << QString("C") << "  1234"   << true  << (qlonglong) 1234;
    QTest::newRow("C 1234  ")       << QString("C") << "1234  "   << true  << (qlonglong) 1234;
    QTest::newRow("C   1234  ")     << QString("C") << "  1234  " << true  << (qlonglong) 1234;
}

void tst_QLocale::long_long_conversion()
{
    QFETCH(QString, locale_name);
    QFETCH(QString, num_str);
    QFETCH(bool, good);
    QFETCH(qlonglong, num);
    QStringRef num_strRef = num_str.leftRef(-1);

    QLocale locale(locale_name);
    QCOMPARE(locale.name(), locale_name);

    bool ok;
    qlonglong l = locale.toLongLong(num_str, &ok);
    QCOMPARE(ok, good);

    if (ok)
        QCOMPARE(l, num);

    l = locale.toLongLong(num_strRef, &ok);
    QCOMPARE(ok, good);

    if (ok)
        QCOMPARE(l, num);
}

void tst_QLocale::long_long_conversion_extra()
{
    QLocale l(QLocale::C);
    l.setNumberOptions(0);
    QCOMPARE(l.toString((qlonglong)1), QString("1"));
    QCOMPARE(l.toString((qlonglong)12), QString("12"));
    QCOMPARE(l.toString((qlonglong)123), QString("123"));
    QCOMPARE(l.toString((qlonglong)1234), QString("1,234"));
    QCOMPARE(l.toString((qlonglong)12345), QString("12,345"));
    QCOMPARE(l.toString((qlonglong)-1), QString("-1"));
    QCOMPARE(l.toString((qlonglong)-12), QString("-12"));
    QCOMPARE(l.toString((qlonglong)-123), QString("-123"));
    QCOMPARE(l.toString((qlonglong)-1234), QString("-1,234"));
    QCOMPARE(l.toString((qlonglong)-12345), QString("-12,345"));
    QCOMPARE(l.toString((qulonglong)1), QString("1"));
    QCOMPARE(l.toString((qulonglong)12), QString("12"));
    QCOMPARE(l.toString((qulonglong)123), QString("123"));
    QCOMPARE(l.toString((qulonglong)1234), QString("1,234"));
    QCOMPARE(l.toString((qulonglong)12345), QString("12,345"));
}

/*
void tst_QLocale::languageToString()
{
}

void tst_QLocale::setDefault()
{
}
*/

void tst_QLocale::testInfAndNan()
{
    double neginf = log(0.0);
    double nan = sqrt(-1.0);

#ifdef Q_OS_WIN
    // these cause INVALID floating point exception so we want to clear the status.
    _clear87();
#endif

    QVERIFY(qIsInf(-neginf));
    QVERIFY(!qIsNaN(-neginf));
    QVERIFY(!qIsFinite(-neginf));

    QVERIFY(!qIsInf(nan));
    QVERIFY(qIsNaN(nan));
    QVERIFY(!qIsFinite(nan));

    QVERIFY(!qIsInf(1.234));
    QVERIFY(!qIsNaN(1.234));
    QVERIFY(qIsFinite(1.234));
}

void tst_QLocale::fpExceptions()
{
#ifndef _MCW_EM
#define _MCW_EM 0x0008001F
#endif
#ifndef _EM_INEXACT
#define _EM_INEXACT 0x00000001
#endif

    // check that double-to-string conversion doesn't throw floating point exceptions when they are
    // enabled
#ifdef Q_OS_WIN
    unsigned int oldbits = _control87(0, 0);
    _control87( 0 | _EM_INEXACT, _MCW_EM );
#endif

#ifdef Q_OS_LINUX
    fenv_t envp;
    fegetenv(&envp);
    feclearexcept(FE_ALL_EXCEPT);
    feenableexcept(FE_DIVBYZERO | FE_OVERFLOW | FE_UNDERFLOW | FE_INVALID);
#endif

    QString::number(1000.1245);
    QString::number(1.1);
    QString::number(0.0);

    QVERIFY(true);

#ifdef Q_OS_WIN
    _clear87();
    _control87(oldbits, 0xFFFFF);
#endif

#ifdef Q_OS_LINUX
    fesetenv(&envp);
#endif
}

void tst_QLocale::negativeZero()
{
    double negativeZero( 0.0 ); // Initialise to zero.
    uchar *ptr = (uchar *)&negativeZero;
#ifdef QT_ARMFPA
    ptr[3] = 0x80;
#else
    ptr[QSysInfo::ByteOrder == QSysInfo::BigEndian ? 0 : 7] = 0x80;
#endif
    QString s = QString::number(negativeZero);
    QCOMPARE(s, QString("0"));
}

void tst_QLocale::dayOfWeek_data()
{
    QTest::addColumn<QDate>("date");
    QTest::addColumn<QString>("shortName");
    QTest::addColumn<QString>("longName");

    QTest::newRow("Sun") << QDate(2006, 1, 1) << "Sun" << "Sunday";
    QTest::newRow("Mon") << QDate(2006, 1, 2) << "Mon" << "Monday";
    QTest::newRow("Tue") << QDate(2006, 1, 3) << "Tue" << "Tuesday";
    QTest::newRow("Wed") << QDate(2006, 1, 4) << "Wed" << "Wednesday";
    QTest::newRow("Thu") << QDate(2006, 1, 5) << "Thu" << "Thursday";
    QTest::newRow("Fri") << QDate(2006, 1, 6) << "Fri" << "Friday";
    QTest::newRow("Sat") << QDate(2006, 1, 7) << "Sat" << "Saturday";
}

void tst_QLocale::dayOfWeek()
{
    QFETCH(QDate, date);
    QFETCH(QString, shortName);
    QFETCH(QString, longName);

    QCOMPARE(QLocale::c().toString(date, "ddd"), shortName);
    QCOMPARE(QLocale::c().toString(date, "dddd"), longName);
}

void tst_QLocale::formatDate_data()
{
    QTest::addColumn<QDate>("date");
    QTest::addColumn<QString>("format");
    QTest::addColumn<QString>("result");

    QTest::newRow("1") << QDate(1974, 12, 1) << "d/M/yyyy" << "1/12/1974";
    QTest::newRow("2") << QDate(1974, 12, 1) << "d/M/yyyyy" << "1/12/1974y";
    QTest::newRow("4") << QDate(1974, 1, 1) << "d/M/yyyy" << "1/1/1974";
    QTest::newRow("5") << QDate(1974, 1, 1) << "dd/MM/yyy" << "01/01/74y";
    QTest::newRow("6") << QDate(1974, 12, 1) << "ddd/MMM/yy" << "Sun/Dec/74";
    QTest::newRow("7") << QDate(1974, 12, 1) << "dddd/MMMM/y" << "Sunday/December/y";
    QTest::newRow("8") << QDate(1974, 12, 1) << "ddddd/MMMMM/yy" << "Sunday1/December12/74";
    QTest::newRow("9") << QDate(1974, 12, 1) << "'dddd'/MMMM/yy" << "dddd/December/74";
    QTest::newRow("10") << QDate(1974, 12, 1) << "d'dd'd/MMMM/yyy" << "1dd1/December/74y";
    QTest::newRow("11") << QDate(1974, 12, 1) << "d'dd'd/MMM'M'/yy" << "1dd1/DecM/74";
    QTest::newRow("12") << QDate(1974, 12, 1) << "d'd'dd/M/yy" << "1d01/12/74";

    QTest::newRow("20") << QDate(1974, 12, 1) << "foo" << "foo";
    QTest::newRow("21") << QDate(1974, 12, 1) << "'" << "";
    QTest::newRow("22") << QDate(1974, 12, 1) << "''" << "'";
    QTest::newRow("23") << QDate(1974, 12, 1) << "'''" << "'";
    QTest::newRow("24") << QDate(1974, 12, 1) << "\"" << "\"";
    QTest::newRow("25") << QDate(1974, 12, 1) << "\"\"" << "\"\"";
    QTest::newRow("26") << QDate(1974, 12, 1) << "\"yy\"" << "\"74\"";
    QTest::newRow("27") << QDate(1974, 12, 1) << "'\"yy\"'" << "\"yy\"";
    QTest::newRow("28") << QDate() << "'\"yy\"'" << "";
    QTest::newRow("29") << QDate(1974, 12, 1) << "hh:mm:ss.zzz ap d'd'dd/M/yy" << "hh:mm:ss.zzz ap 1d01/12/74";

    QTest::newRow("dd MMMM yyyy") << QDate(1, 1, 1) << "dd MMMM yyyy" << "01 January 0001";
}

void tst_QLocale::formatDate()
{
    QFETCH(QDate, date);
    QFETCH(QString, format);
    QFETCH(QString, result);

    QLocale l(QLocale::C);
    QCOMPARE(l.toString(date, format), result);
}

Q_DECLARE_METATYPE(QTime)

void tst_QLocale::formatTime_data()
{
    QTest::addColumn<QTime>("time");
    QTest::addColumn<QString>("format");
    QTest::addColumn<QString>("result");

    QTest::newRow("1") << QTime(1, 2, 3) << "h:m:s" << "1:2:3";
    QTest::newRow("3") << QTime(1, 2, 3) << "H:m:s" << "1:2:3";
    QTest::newRow("4") << QTime(1, 2, 3) << "hh:mm:ss" << "01:02:03";
    QTest::newRow("5") << QTime(1, 2, 3) << "HH:mm:ss" << "01:02:03";
    QTest::newRow("6") << QTime(1, 2, 3) << "hhh:mmm:sss" << "011:022:033";

    QTest::newRow("8") << QTime(14, 2, 3) << "h:m:s" << "14:2:3";
    QTest::newRow("9") << QTime(14, 2, 3) << "H:m:s" << "14:2:3";
    QTest::newRow("10") << QTime(14, 2, 3) << "hh:mm:ss" << "14:02:03";
    QTest::newRow("11") << QTime(14, 2, 3) << "HH:mm:ss" << "14:02:03";
    QTest::newRow("12") << QTime(14, 2, 3) << "hhh:mmm:sss" << "1414:022:033";

    QTest::newRow("14") << QTime(14, 2, 3) << "h:m:s ap" << "2:2:3 pm";
    QTest::newRow("15") << QTime(14, 2, 3) << "H:m:s AP" << "14:2:3 PM";
    QTest::newRow("16") << QTime(14, 2, 3) << "hh:mm:ss aap" << "02:02:03 pmpm";
    QTest::newRow("17") << QTime(14, 2, 3) << "HH:mm:ss AP aa" << "14:02:03 PM pmpm";

    QTest::newRow("18") << QTime(1, 2, 3) << "h:m:s ap" << "1:2:3 am";
    QTest::newRow("19") << QTime(1, 2, 3) << "H:m:s AP" << "1:2:3 AM";

    QTest::newRow("20") << QTime(1, 2, 3) << "foo" << "foo";
    QTest::newRow("21") << QTime(1, 2, 3) << "'" << "";
    QTest::newRow("22") << QTime(1, 2, 3) << "''" << "'";
    QTest::newRow("23") << QTime(1, 2, 3) << "'''" << "'";
    QTest::newRow("24") << QTime(1, 2, 3) << "\"" << "\"";
    QTest::newRow("25") << QTime(1, 2, 3) << "\"\"" << "\"\"";
    QTest::newRow("26") << QTime(1, 2, 3) << "\"H\"" << "\"1\"";
    QTest::newRow("27") << QTime(1, 2, 3) << "'\"H\"'" << "\"H\"";

    QTest::newRow("28") << QTime(1, 2, 3, 456) << "H:m:s.z" << "1:2:3.456";
    QTest::newRow("29") << QTime(1, 2, 3, 456) << "H:m:s.zz" << "1:2:3.456456";
    QTest::newRow("30") << QTime(1, 2, 3, 456) << "H:m:s.zzz" << "1:2:3.456";
    QTest::newRow("31") << QTime(1, 2, 3, 4) << "H:m:s.z" << "1:2:3.4";
    QTest::newRow("32") << QTime(1, 2, 3, 4) << "H:m:s.zzz" << "1:2:3.004";
    QTest::newRow("33") << QTime() << "H:m:s.zzz" << "";
    QTest::newRow("34") << QTime(1, 2, 3, 4) << "dd MM yyyy H:m:s.zzz" << "dd MM yyyy 1:2:3.004";
}

void tst_QLocale::formatTime()
{
    QFETCH(QTime, time);
    QFETCH(QString, format);
    QFETCH(QString, result);

    QLocale l(QLocale::C);
    QCOMPARE(l.toString(time, format), result);
}


void tst_QLocale::formatDateTime_data()
{
    QTest::addColumn<QString>("localeName");
    QTest::addColumn<QDateTime>("dateTime");
    QTest::addColumn<QString>("format");
    QTest::addColumn<QString>("result");

    QTest::newRow("1C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(5, 14, 13))
                        << "d/M/yyyy hh:h:mm" << "1/12/1974 05:5:14";
    QTest::newRow("2C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                        << "d/M/yyyyy h" << "1/12/1974y 15";
    QTest::newRow("4C") << "C" << QDateTime(QDate(1974, 1, 1), QTime(15, 14, 13))
                        << "d/M/yyyy zzz" << "1/1/1974 000";
    QTest::newRow("5C") << "C" << QDateTime(QDate(1974, 1, 1), QTime(15, 14, 13))
                        << "dd/MM/yyy z" << "01/01/74y 0";
    QTest::newRow("6C") << "C" << QDateTime(QDate(1974, 12, 2), QTime(15, 14, 13))
                        << "ddd/MMM/yy AP" << "Mon/Dec/74 PM";
    QTest::newRow("7C") << "C" << QDateTime(QDate(1974, 12, 2), QTime(15, 14, 13))
                        << "dddd/MMMM/y apa" << "Monday/December/y pmpm";
    QTest::newRow("8C") << "C" << QDateTime(QDate(1974, 12, 2), QTime(15, 14, 13))
                        << "ddddd/MMMMM/yy ss" << "Monday2/December12/74 13";
    QTest::newRow("9C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                        << "'dddd'/MMMM/yy s" << "dddd/December/74 13";
    QTest::newRow("10C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 4, 13))
                         << "d'dd'd/MMMM/yyy m'm'mm" << "1dd1/December/74y 4m04";
    QTest::newRow("11C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 3))
                         << "d'dd'd/MMM'M'/yysss" << "1dd1/DecM/74033";
    QTest::newRow("12C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                         << "d'd'dd/M/yyh" << "1d01/12/7415";

    QTest::newRow("dd MMMM yyyy, hh:mm:ss") << "C" << QDateTime(QDate(1, 1, 1), QTime(12, 00, 00))
                         << "dd MMMM yyyy, hh:mm:ss" << "01 January 0001, 12:00:00";

    QTest::newRow("20C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                         << "foo" << "foo";
    QTest::newRow("21C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                         << "'" << "";
    QTest::newRow("22C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                         << "''" << "'";
    QTest::newRow("23C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                         << "'''" << "'";
    QTest::newRow("24C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                         << "\"" << "\"";
    QTest::newRow("25C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                         << "\"\"" << "\"\"";
    QTest::newRow("26C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                         << "\"yymm\"" << "\"7414\"";
    QTest::newRow("27C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                         << "'\"yymm\"'" << "\"yymm\"";
    QTest::newRow("27C") << "C" << QDateTime()
                         << "'\"yymm\"'" << "";

    QTest::newRow("1no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(5, 14, 13))
                            << "d/M/yyyy hh:h:mm" << "1/12/1974 05:5:14";
    QTest::newRow("2no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                            << "d/M/yyyyy h" << "1/12/1974y 15";
    QTest::newRow("4no_NO") << "no_NO" << QDateTime(QDate(1974, 1, 1), QTime(15, 14, 13))
                            << "d/M/yyyy zzz" << "1/1/1974 000";
    QTest::newRow("5no_NO") << "no_NO" << QDateTime(QDate(1974, 1, 1), QTime(15, 14, 13))
                            << "dd/MM/yyy z" << "01/01/74y 0";
    QTest::newRow("6no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 2), QTime(15, 14, 13))
                            << "ddd/MMM/yy AP" << "man./des./74 P.M.";
    QTest::newRow("7no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 2), QTime(15, 14, 13))
                            << "dddd/MMMM/y apa" << "mandag/desember/y p.m.p.m.";
    QTest::newRow("8no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 2), QTime(15, 14, 13))
                            << "ddddd/MMMMM/yy ss" << "mandag2/desember12/74 13";
    QTest::newRow("9no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                            << "'dddd'/MMMM/yy s" << "dddd/desember/74 13";
    QTest::newRow("10no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 4, 13))
                             << "d'dd'd/MMMM/yyy m'm'mm" << "1dd1/desember/74y 4m04";
    QTest::newRow("11no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 3))
                             << "d'dd'd/MMM'M'/yysss" << "1dd1/des.M/74033";
    QTest::newRow("12no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                             << "d'd'dd/M/yyh" << "1d01/12/7415";

    QTest::newRow("20no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                             << "foo" << "foo";
    QTest::newRow("21no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                             << "'" << "";
    QTest::newRow("22no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                             << "''" << "'";
    QTest::newRow("23no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                             << "'''" << "'";
    QTest::newRow("24no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                             << "\"" << "\"";
    QTest::newRow("25no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                             << "\"\"" << "\"\"";
    QTest::newRow("26no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                             << "\"yymm\"" << "\"7414\"";
    QTest::newRow("27no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 14, 13))
                             << "'\"yymm\"'" << "\"yymm\"";
    QTest::newRow("27no_NO") << "no_NO" << QDateTime()
                             << "'\"yymm\"'" << "";
    QTest::newRow("28no_NO") << "no_NO" << QDateTime()
                             << "'\"yymm\"'" << "";

    QDateTime testLongHour(QDate(1999, 12, 31), QTime(23, 59, 59, 999));
    QDateTime testShortHour(QDate(1999, 12, 31), QTime(3, 59, 59, 999));
    QDateTime testZeroHour(QDate(1999, 12, 31), QTime(0, 59, 59, 999));

    QTest::newRow("datetime0")      << "en_US" << QDateTime()
                                    << QString("dd-MM-yyyy hh:mm:ss") << QString();
    QTest::newRow("datetime1")      << "en_US" << testLongHour
                                    << QString("dd-'mmddyy'MM-yyyy hh:mm:ss.zzz")
                                    << QString("31-mmddyy12-1999 23:59:59.999");
    QTest::newRow("datetime2")      << "en_US" << testLongHour
                                    << QString("dd-'apAP'MM-yyyy hh:mm:ss.zzz")
                                    << QString("31-apAP12-1999 23:59:59.999");
    QTest::newRow("datetime3")      << "en_US" << testLongHour
                                    << QString("Apdd-MM-yyyy hh:mm:ss.zzz")
                                    << QString("PMp31-12-1999 11:59:59.999");
    QTest::newRow("datetime4")      << "en_US" << testLongHour
                                    << QString("'ap'apdd-MM-yyyy 'AP'hh:mm:ss.zzz")
                                    << QString("appm31-12-1999 AP11:59:59.999");
    QTest::newRow("datetime5")      << "en_US" << testLongHour
                                    << QString("'''") << QString("'");
    QTest::newRow("datetime6")      << "en_US" << testLongHour
                                    << QString("'ap") << QString("ap");
    QTest::newRow("datetime7")      << "en_US" << testLongHour
                                    << QString("' ' 'hh' hh") << QString("  hh 23");
    QTest::newRow("datetime8")      << "en_US" << testLongHour
                                    << QString("d'foobar'") << QString("31foobar");
    QTest::newRow("datetime9")      << "en_US" << testShortHour
                                    << QString("hhhhh") << QString("03033");
    QTest::newRow("datetime11")     << "en_US" << testLongHour
                                    << QString("HHHhhhAaAPap") << QString("23231111PMpmPMpm");
    QTest::newRow("datetime12")     << "en_US" << testShortHour
                                    << QString("HHHhhhAaAPap") << QString("033033AMamAMam");
    QTest::newRow("datetime13")     << "en_US" << QDateTime(QDate(1974, 12, 1), QTime(14, 14, 20))
                                    << QString("hh''mm''ss dd''MM''yyyy")
                                    << QString("14'14'20 01'12'1974");
    QTest::newRow("AM no p")        << "en_US" << testZeroHour
                                    << QString("hhAX") << QString("12AMX");
    QTest::newRow("AM no p, x 2")   << "en_US" << testShortHour
                                    << QString("hhhhhaA") << QString("03033amAM");
    QTest::newRow("am 0 hour")      << "en_US" << testZeroHour
                                    << QString("hAP") << QString("12AM");
    QTest::newRow("AM zero hour")   << "en_US" << testZeroHour
                                    << QString("hhAP") << QString("12AM");
    QTest::newRow("dddd")           << "en_US" << testZeroHour
                                    << QString("dddd") << QString("Friday");
    QTest::newRow("ddd")            << "en_US" << testZeroHour
                                    << QString("ddd") << QString("Fri");
    QTest::newRow("MMMM")           << "en_US" << testZeroHour
                                    << QString("MMMM") << QString("December");
    QTest::newRow("MMM")            << "en_US" << testZeroHour
                                    << QString("MMM") << QString("Dec");
    QTest::newRow("empty")          << "en_US" << testZeroHour
                                    << QString("") << QString("");
}

void tst_QLocale::formatDateTime()
{
    QFETCH(QString, localeName);
    QFETCH(QDateTime, dateTime);
    QFETCH(QString, format);
    QFETCH(QString, result);

    QLocale l(localeName);
    QCOMPARE(l.toString(dateTime, format), result);
}

void tst_QLocale::formatTimeZone()
{
    QLocale enUS("en_US");

    QDateTime dt1(QDate(2013, 1, 1), QTime(1, 0, 0), Qt::OffsetFromUTC, 60 * 60);
    QCOMPARE(enUS.toString(dt1, "t"), QString("UTC+01:00"));

    QDateTime dt2(QDate(2013, 1, 1), QTime(1, 0, 0), Qt::OffsetFromUTC, -60 * 60);
    QCOMPARE(enUS.toString(dt2, "t"), QString("UTC-01:00"));

    QDateTime dt3(QDate(2013, 1, 1), QTime(0, 0, 0), Qt::UTC);
    QCOMPARE(enUS.toString(dt3, "t"), QString("UTC"));

    // LocalTime should vary
    if (europeanTimeZone) {
        // Time definitely in Standard Time
        QDateTime dt4(QDate(2013, 1, 1), QTime(0, 0, 0), Qt::LocalTime);
#ifdef Q_OS_WIN
        QEXPECT_FAIL("", "Windows only returns long name (QTBUG-32759)", Continue);
#endif // Q_OS_WIN
        QCOMPARE(enUS.toString(dt4, "t"), QString("CET"));

        // Time definitely in Daylight Time
        QDateTime dt5(QDate(2013, 6, 1), QTime(0, 0, 0), Qt::LocalTime);
#ifdef Q_OS_WIN
        QEXPECT_FAIL("", "Windows only returns long name (QTBUG-32759)", Continue);
#endif // Q_OS_WIN
        QCOMPARE(enUS.toString(dt5, "t"), QString("CEST"));
    } else {
        QSKIP("You must test using Central European (CET/CEST) time zone, e.g. TZ=Europe/Oslo", SkipAll);
    }

    // Current datetime should return current abbreviation
    QCOMPARE(enUS.toString(QDateTime::currentDateTime(), "t"),
             QDateTime::currentDateTime().timeZoneAbbreviation());

    // Time on its own will always be current local time zone
    QCOMPARE(enUS.toString(QTime(1, 2, 3), "t"), QDateTime::currentDateTime().timeZoneAbbreviation());
}

void tst_QLocale::toDateTime_data()
{
    QTest::addColumn<QString>("localeName");
    QTest::addColumn<QDateTime>("result");
    QTest::addColumn<QString>("format");
    QTest::addColumn<QString>("string");

    QTest::newRow("1C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(5, 14, 0))
                        << "d/M/yyyy hh:h:mm" << "1/12/1974 05:5:14";
    QTest::newRow("2C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 0, 0))
                        << "d/M/yyyyy h" << "1/12/1974y 15";
    QTest::newRow("4C") << "C" << QDateTime(QDate(1974, 1, 1), QTime(0, 0, 0))
                        << "d/M/yyyy zzz" << "1/1/1974 000";
    QTest::newRow("5C") << "C" << QDateTime(QDate(1974, 1, 1), QTime(0, 0, 0))
                        << "dd/MM/yyy z" << "01/01/74y 0";
    QTest::newRow("8C") << "C" << QDateTime(QDate(1974, 12, 2), QTime(0, 0, 13))
                        << "ddddd/MMMMM/yy ss" << "Monday2/December12/74 13";
    QTest::newRow("9C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(0, 0, 13))
                        << "'dddd'/MMMM/yy s" << "dddd/December/74 13";
    QTest::newRow("10C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(0, 4, 0))
                         << "d'dd'd/MMMM/yyy m'm'mm" << "1dd1/December/74y 4m04";
    QTest::newRow("11C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(0, 0, 3))
                         << "d'dd'd/MMM'M'/yysss" << "1dd1/DecM/74033";
    QTest::newRow("12C") << "C" << QDateTime(QDate(1974, 12, 1), QTime(15, 0, 0))
                         << "d'd'dd/M/yyh" << "1d01/12/7415";

    QTest::newRow("1no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(5, 14, 0))
                            << "d/M/yyyy hh:h:mm" << "1/12/1974 05:5:14";
    QTest::newRow("2no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 0, 0))
                            << "d/M/yyyyy h" << "1/12/1974y 15";
    QTest::newRow("4no_NO") << "no_NO" << QDateTime(QDate(1974, 1, 1), QTime(0, 0, 0))
                            << "d/M/yyyy zzz" << "1/1/1974 000";
    QTest::newRow("5no_NO") << "no_NO" << QDateTime(QDate(1974, 1, 1), QTime(0, 0, 0))
                            << "dd/MM/yyy z" << "01/01/74y 0";
    QTest::newRow("8no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 2), QTime(0, 0, 13))
                            << "ddddd/MMMMM/yy ss" << "mandag2/desember12/74 13";
    QTest::newRow("9no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(0, 0, 13))
                            << "'dddd'/MMMM/yy s" << "dddd/desember/74 13";
    QTest::newRow("10no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(0, 4, 0))
                             << "d'dd'd/MMMM/yyy m'm'mm" << "1dd1/desember/74y 4m04";
    QTest::newRow("11no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(0, 0, 3))
                             << "d'dd'd/MMM'M'/yysss" << "1dd1/des.M/74033";
    QTest::newRow("12no_NO") << "no_NO" << QDateTime(QDate(1974, 12, 1), QTime(15, 0, 0))
                             << "d'd'dd/M/yyh" << "1d01/12/7415";

    QTest::newRow("RFC-1123") << "C" << QDateTime(QDate(2007, 11, 1), QTime(18, 8, 30))
                              << "ddd, dd MMM yyyy hh:mm:ss 'GMT'" << "Thu, 01 Nov 2007 18:08:30 GMT";

    QTest::newRow("longFormat") << "en_US" << QDateTime(QDate(2009, 1, 5), QTime(11, 48, 32))
                      << "dddd, MMMM d, yyyy h:mm:ss AP " << "Monday, January 5, 2009 11:48:32 AM ";
}

void tst_QLocale::toDateTime()
{
    QFETCH(QString, localeName);
    QFETCH(QDateTime, result);
    QFETCH(QString, format);
    QFETCH(QString, string);

    QLocale l(localeName);
    QCOMPARE(l.toDateTime(string, format), result);
    if (l.dateTimeFormat(QLocale::LongFormat) == format)
        QCOMPARE(l.toDateTime(string, QLocale::LongFormat), result);
}

#ifdef Q_OS_MAC
void tst_QLocale::macDefaultLocale()
{
    QLocale locale = QLocale::system();
    if (locale.name() != QLatin1String("en_US")) {
        QSKIP("This test only tests for en_US", SkipAll);
    }

    QTime invalidTime;
    QDate invalidDate;
    QCOMPARE(locale.toString(invalidTime, QLocale::ShortFormat), QString());
    QCOMPARE(locale.toString(invalidDate, QLocale::ShortFormat), QString());
    QCOMPARE(locale.toString(invalidTime, QLocale::NarrowFormat), QString());
    QCOMPARE(locale.toString(invalidDate, QLocale::NarrowFormat), QString());
    QCOMPARE(locale.toString(invalidTime, QLocale::LongFormat), QString());
    QCOMPARE(locale.toString(invalidDate, QLocale::LongFormat), QString());
    QCOMPARE(locale.decimalPoint(), QChar('.'));
    QCOMPARE(locale.groupSeparator(), QChar(','));
    QCOMPARE(locale.dateFormat(QLocale::ShortFormat), QString("M/d/yy"));
    QCOMPARE(locale.dateFormat(QLocale::LongFormat), QString("MMMM d, yyyy"));
    QCOMPARE(locale.timeFormat(QLocale::ShortFormat), QString("h:mm AP"));
    QCOMPARE(locale.timeFormat(QLocale::LongFormat), QString("h:mm:ss AP t"));

    // make sure we are using the system to parse them
    QCOMPARE(locale.toString(1234.56), QString("1,234.56"));
    QCOMPARE(locale.toString(QDate(1974, 12, 1), QLocale::ShortFormat), QString("12/1/74"));
    QCOMPARE(locale.toString(QDate(1974, 12, 1), QLocale::NarrowFormat), locale.toString(QDate(1974, 12, 1), QLocale::ShortFormat));
    QCOMPARE(locale.toString(QDate(1974, 12, 1), QLocale::LongFormat), QString("December 1, 1974"));
    QCOMPARE(locale.toString(QTime(1,2,3), QLocale::ShortFormat), QString("1:02 AM"));
    QCOMPARE(locale.toString(QTime(1,2,3), QLocale::NarrowFormat), locale.toString(QTime(1,2,3), QLocale::ShortFormat));

    QTime currentTime = QTime::currentTime();
    QTime utcTime = QDateTime::currentDateTime().toUTC().time();

    int diff = currentTime.hour() - utcTime.hour();

     // Check if local time and utc time are on opposite sides of the 24-hour wrap-around.
	if (diff < -12)
	   diff += 24;
	if (diff > 12)
	   diff -= 24;

	const QString timeString = locale.toString(QTime(1,2,3), QLocale::LongFormat);
    QVERIFY(timeString.contains(QString("1:02:03")));

    // System Preferences->Language & Text, Region Tab, should choose "United States" for Region field
    QCOMPARE(locale.toCurrencyString(qulonglong(1234)), QString("$1,234.00"));
    QCOMPARE(locale.toCurrencyString(qlonglong(-1234)), QString("($1,234.00)"));
    QCOMPARE(locale.toCurrencyString(double(1234.56)), QString("$1,234.56"));
    QCOMPARE(locale.toCurrencyString(double(-1234.56)), QString("($1,234.56)"));

    // Depending on the configured time zone, the time string might not
    // contain a GMT specifier. (Sometimes it just names the zone, like "CEST")
    if (timeString.contains(QString("GMT"))) {
        QString expectedGMTSpecifier("GMT");
        if (diff >= 0)
            expectedGMTSpecifier.append("+");
        else
            expectedGMTSpecifier.append("-");
        if (qAbs(diff) < 10)
            expectedGMTSpecifier.append(QString("0%1").arg(qAbs(diff)));
        else
            expectedGMTSpecifier.append(QString("%1").arg(qAbs(diff)));
        QVERIFY2(timeString.contains(expectedGMTSpecifier), qPrintable(
            QString("timeString `%1', expectedGMTSpecifier `%2'")
            .arg(timeString)
            .arg(expectedGMTSpecifier)
        ));
    }
    QCOMPARE(locale.dayName(1), QString("Monday"));
    QCOMPARE(locale.dayName(7), QString("Sunday"));
    QCOMPARE(locale.monthName(1), QString("January"));
    QCOMPARE(locale.monthName(12), QString("December"));
    QCOMPARE(locale.firstDayOfWeek(), Qt::Sunday);
    QCOMPARE(locale.quoteString("string"), QString::fromUtf8("\xe2\x80\x9c" "string" "\xe2\x80\x9d"));
    QCOMPARE(locale.quoteString("string", QLocale::AlternateQuotation), QString::fromUtf8("\xe2\x80\x98" "string" "\xe2\x80\x99"));

    QList<Qt::DayOfWeek> days;
    days << Qt::Monday << Qt::Tuesday << Qt::Wednesday << Qt::Thursday << Qt::Friday;
    QCOMPARE(locale.weekdays(), days);

}
#endif

#ifdef Q_OS_WIN
#include <qt_windows.h>

static QString getWinLocaleInfo(LCTYPE type)
{
    LCID id = GetThreadLocale();
    int cnt = GetLocaleInfo(id, type, 0, 0) * 2;

    if (cnt == 0) {
        qWarning("QLocale: empty windows locale info (%d)", type);
        return QString();
    }

    QByteArray buff(cnt, 0);

    cnt = GetLocaleInfo(id, type, reinterpret_cast<wchar_t*>(buff.data()), buff.size() / 2);

    if (cnt == 0) {
        qWarning("QLocale: empty windows locale info (%d)", type);
        return QString();
    }

    return QString::fromWCharArray(reinterpret_cast<wchar_t*>(buff.data()));
}

static void setWinLocaleInfo(LCTYPE type, const QString &value)
{
    LCID id = GetThreadLocale();
    SetLocaleInfo(id, type, reinterpret_cast<const wchar_t*>(value.utf16()));
}

class RestoreLocaleHelper {
public:
    RestoreLocaleHelper() {
        m_decimal = getWinLocaleInfo(LOCALE_SDECIMAL);
        m_thousand = getWinLocaleInfo(LOCALE_STHOUSAND);
        m_sdate = getWinLocaleInfo(LOCALE_SSHORTDATE);
        m_ldate = getWinLocaleInfo(LOCALE_SLONGDATE);
        m_time = getWinLocaleInfo(LOCALE_STIMEFORMAT);
    }

    ~RestoreLocaleHelper() {
        // restore these, or the user will get a surprise
        setWinLocaleInfo(LOCALE_SDECIMAL, m_decimal);
        setWinLocaleInfo(LOCALE_STHOUSAND, m_thousand);
        setWinLocaleInfo(LOCALE_SSHORTDATE, m_sdate);
        setWinLocaleInfo(LOCALE_SLONGDATE, m_ldate);
        setWinLocaleInfo(LOCALE_STIMEFORMAT, m_time);
    }

    QString m_decimal, m_thousand, m_sdate, m_ldate, m_time;

};

#endif

void tst_QLocale::windowsDefaultLocale()
{
#ifndef Q_OS_WIN
    QSKIP("This is a Windows test", SkipAll);
#else
    RestoreLocaleHelper systemLocale;
    // set weird system defaults and make sure we're using them
    setWinLocaleInfo(LOCALE_SDECIMAL, QLatin1String("@"));
    setWinLocaleInfo(LOCALE_STHOUSAND, QLatin1String("?"));
    setWinLocaleInfo(LOCALE_SSHORTDATE, QLatin1String("d*M*yyyy"));
    setWinLocaleInfo(LOCALE_SLONGDATE, QLatin1String("d@M@yyyy"));
    setWinLocaleInfo(LOCALE_STIMEFORMAT, QLatin1String("h^m^s"));
    QLocale locale = QLocale::system();

    // make sure we are seeing the system's format strings
    QCOMPARE(locale.decimalPoint(), QChar('@'));
    QCOMPARE(locale.groupSeparator(), QChar('?'));
    QCOMPARE(locale.dateFormat(QLocale::ShortFormat), QString("d*M*yyyy"));
    QCOMPARE(locale.dateFormat(QLocale::LongFormat), QString("d@M@yyyy"));
    QCOMPARE(locale.timeFormat(QLocale::ShortFormat), QString("h^m^s"));
    QCOMPARE(locale.timeFormat(QLocale::LongFormat), QString("h^m^s"));
    QCOMPARE(locale.dateTimeFormat(QLocale::ShortFormat), QString("d*M*yyyy h^m^s"));
    QCOMPARE(locale.dateTimeFormat(QLocale::LongFormat), QString("d@M@yyyy h^m^s"));

    // make sure we are using the system to parse them
    QCOMPARE(locale.toString(1234.56), QString("1?234@56"));
    QCOMPARE(locale.toString(QDate(1974, 12, 1), QLocale::ShortFormat), QString("1*12*1974"));
    QCOMPARE(locale.toString(QDate(1974, 12, 1), QLocale::NarrowFormat), locale.toString(QDate(1974, 12, 1), QLocale::ShortFormat));
    QCOMPARE(locale.toString(QDate(1974, 12, 1), QLocale::LongFormat), QString("1@12@1974"));
    QCOMPARE(locale.toString(QTime(1,2,3), QLocale::ShortFormat), QString("1^2^3"));
    QCOMPARE(locale.toString(QTime(1,2,3), QLocale::NarrowFormat), locale.toString(QTime(1,2,3), QLocale::ShortFormat));
    QCOMPARE(locale.toString(QTime(1,2,3), QLocale::LongFormat), QString("1^2^3"));
    QCOMPARE(locale.toString(QDateTime(QDate(1974, 12, 1), QTime(1,2,3)), QLocale::ShortFormat),
             QString("1*12*1974 1^2^3"));
    QCOMPARE(locale.toString(QDateTime(QDate(1974, 12, 1), QTime(1,2,3)), QLocale::NarrowFormat),
             locale.toString(QDateTime(QDate(1974, 12, 1), QTime(1,2,3)), QLocale::ShortFormat));
    QCOMPARE(locale.toString(QDateTime(QDate(1974, 12, 1), QTime(1,2,3)), QLocale::LongFormat),
             QString("1@12@1974 1^2^3"));
    QCOMPARE(locale.toString(QTime(1,2,3), QLocale::LongFormat), QString("1^2^3"));
#endif
}

void tst_QLocale::numberOptions()
{
    bool ok;

    QLocale locale(QLocale::C);
    QCOMPARE(locale.numberOptions(), QLocale::OmitGroupSeparator);
    QCOMPARE(locale.toInt(QString("12345"), &ok), 12345);
    QVERIFY(ok);
    QCOMPARE(locale.toInt(QString("12345"), &ok), 12345);
    QVERIFY(ok);
    QCOMPARE(locale.toString(12345), QString("12345"));

    locale.setNumberOptions(0);
    QCOMPARE(locale.numberOptions(), 0);
    QCOMPARE(locale.toInt(QString("12,345"), &ok), 12345);
    QVERIFY(ok);
    QCOMPARE(locale.toInt(QString("12345"), &ok), 12345);
    QVERIFY(ok);
    QCOMPARE(locale.toString(12345), QString("12,345"));

    locale.setNumberOptions(QLocale::RejectGroupSeparator);
    QCOMPARE(locale.numberOptions(), QLocale::RejectGroupSeparator);
    locale.toInt(QString("12,345"), &ok);
    QVERIFY(!ok);
    QCOMPARE(locale.toInt(QString("12345"), &ok), 12345);
    QVERIFY(ok);
    QCOMPARE(locale.toString(12345), QString("12,345"));

    QLocale locale2 = locale;
    QCOMPARE(locale2.numberOptions(), QLocale::RejectGroupSeparator);
}

void tst_QLocale::negativeNumbers()
{
    QLocale locale(QLocale::C);

    bool ok;
    int i;

    i = locale.toInt(QLatin1String("-100"), &ok);
    QVERIFY(ok);
    QCOMPARE(i, -100);

    i = locale.toInt(QLatin1String("-1,000"), &ok);
    QVERIFY(ok);
    QCOMPARE(i, -1000);

    i = locale.toInt(QLatin1String("-1000"), &ok);
    QVERIFY(ok);
    QCOMPARE(i, -1000);

    i = locale.toInt(QLatin1String("-10,000"), &ok);
    QVERIFY(ok);
    QCOMPARE(i, -10000);

    i = locale.toInt(QLatin1String("-10000"), &ok);
    QVERIFY(ok);
    QCOMPARE(i, -10000);

    i = locale.toInt(QLatin1String("-100,000"), &ok);
    QVERIFY(ok);
    QCOMPARE(i, -100000);

    i = locale.toInt(QLatin1String("-100000"), &ok);
    QVERIFY(ok);
    QCOMPARE(i, -100000);

    i = locale.toInt(QLatin1String("-1,000,000"), &ok);
    QVERIFY(ok);
    QCOMPARE(i, -1000000);

    i = locale.toInt(QLatin1String("-1000000"), &ok);
    QVERIFY(ok);
    QCOMPARE(i, -1000000);
}

#include <private/qlocale_p.h>
#include <private/qlocale_data_p.h>

static const int locale_data_count = sizeof(locale_data)/sizeof(locale_data[0]);

void tst_QLocale::testNames_data()
{
    QTest::addColumn<int>("language");
    QTest::addColumn<int>("country");

    for (int i = 0; i < locale_data_count; ++i) {
        const QLocaleData &item = locale_data[i];

        const QString testName = QString::fromLatin1("data_%1 (%2/%3)").arg(i)
                .arg(QLocale::languageToString((QLocale::Language)item.m_language_id))
                .arg(QLocale::countryToString((QLocale::Country)item.m_country_id));
        QTest::newRow(testName.toLatin1().constData()) << (int)item.m_language_id << (int)item.m_country_id;
    }
}

void tst_QLocale::testNames()
{
    QFETCH(int, language);
    QFETCH(int, country);

    QLocale l1((QLocale::Language)language, (QLocale::Country)country);
    if (language == QLocale::AnyLanguage && country == QLocale::AnyCountry)
        language = QLocale::C;
    QCOMPARE((int)l1.language(), language);
    QCOMPARE((int)l1.country(), country);

    QString name = l1.name();

    QLocale l2(name);
    QCOMPARE((int)l2.language(), language);
    QCOMPARE((int)l2.country(), country);
    QCOMPARE(l2.name(), name);

    QLocale l3(name + QLatin1String("@foo"));
    QCOMPARE((int)l3.language(), language);
    QCOMPARE((int)l3.country(), country);
    QCOMPARE(l3.name(), name);

    QLocale l4(name + QLatin1String(".foo"));
    QCOMPARE((int)l4.language(), language);
    QCOMPARE((int)l4.country(), country);
    QCOMPARE(l4.name(), name);

    if (language != QLocale::C) {
        int idx = name.indexOf(QLatin1Char('_'));
        QVERIFY(idx != -1);
        QString lang = name.left(idx);

        QCOMPARE((int)QLocale(lang).language(), language);
        QCOMPARE((int)QLocale(lang + QLatin1String("@foo")).language(), language);
        QCOMPARE((int)QLocale(lang + QLatin1String(".foo")).language(), language);
    }
}

void tst_QLocale::dayName_data()
{
    QTest::addColumn<QString>("locale_name");
    QTest::addColumn<QString>("dayName");
    QTest::addColumn<int>("day");
    QTest::addColumn<QLocale::FormatType>("format");

    QTest::newRow("no_NO")  << QString("no_NO") << QString("tirsdag") << 2 << QLocale::LongFormat;
    QTest::newRow("nb_NO")  << QString("nb_NO") << QString("tirsdag") << 2 << QLocale::LongFormat;
    QTest::newRow("nn_NO")  << QString("nn_NO") << QString("tysdag") << 2 << QLocale::LongFormat;

    QTest::newRow("C long")  << QString("C") << QString("Sunday") << 7 << QLocale::LongFormat;
    QTest::newRow("C short")  << QString("C") << QString("Sun") << 7 << QLocale::ShortFormat;
    QTest::newRow("C narrow")  << QString("C") << QString("7") << 7 << QLocale::NarrowFormat;

    QTest::newRow("ru_RU long")
        << QString("ru_RU")
        << QString::fromUtf8("\320\262\320\276\321\201\320\272\321\200\320"
                             "\265\321\201\320\265\320\275\321\214\320\265")
        << 7 << QLocale::LongFormat;
    QTest::newRow("ru_RU short")
        << QString("ru_RU") << QString::fromUtf8("\320\262\321\201") << 7 << QLocale::ShortFormat;

    QTest::newRow("ru_RU narrow")
        << QString("ru_RU") << QString::fromUtf8("\320\262\321\201") << 7 << QLocale::NarrowFormat;
}

void tst_QLocale::dayName()
{
    QFETCH(QString, locale_name);
    QFETCH(QString, dayName);
    QFETCH(int, day);
    QFETCH(QLocale::FormatType, format);

    QLocale l(locale_name);
    QCOMPARE(l.dayName(day, format), dayName);

    QLocale ir("ga_IE");
    QCOMPARE(ir.dayName(1, QLocale::ShortFormat), QLatin1String("Luan"));
    QCOMPARE(ir.dayName(7, QLocale::ShortFormat), QLatin1String("Domh"));

    QLocale gr("el_GR");
    QCOMPARE(gr.dayName(2, QLocale::ShortFormat), QString::fromUtf8("\316\244\317\201\316\257"));
    QCOMPARE(gr.dayName(4, QLocale::ShortFormat), QString::fromUtf8("\316\240\316\255\316\274"));
    QCOMPARE(gr.dayName(6, QLocale::ShortFormat), QString::fromUtf8("\316\243\316\254\316\262"));
}

void tst_QLocale::standaloneDayName_data()
{
    QTest::addColumn<QString>("locale_name");
    QTest::addColumn<QString>("dayName");
    QTest::addColumn<int>("day");
    QTest::addColumn<QLocale::FormatType>("format");

    QTest::newRow("no_NO")  << QString("no_NO") << QString("tirsdag") << 2 << QLocale::LongFormat;
    QTest::newRow("nb_NO")  << QString("nb_NO") << QString("tirsdag") << 2 << QLocale::LongFormat;
    QTest::newRow("nn_NO")  << QString("nn_NO") << QString("tysdag") << 2 << QLocale::LongFormat;

    QTest::newRow("C invalid: 0 long")  << QString("C") << QString() << 0 << QLocale::LongFormat;
    QTest::newRow("C invalid: 0 short")  << QString("C") << QString() << 0 << QLocale::ShortFormat;
    QTest::newRow("C invalid: 0 narrow")  << QString("C") << QString() << 0 << QLocale::NarrowFormat;
    QTest::newRow("C invalid: 8 long")  << QString("C") << QString() << 8 << QLocale::LongFormat;
    QTest::newRow("C invalid: 8 short")  << QString("C") << QString() << 8 << QLocale::ShortFormat;
    QTest::newRow("C invalid: 8 narrow")  << QString("C") << QString() << 8 << QLocale::NarrowFormat;

    QTest::newRow("C long")  << QString("C") << QString("Sunday") << 7 << QLocale::LongFormat;
    QTest::newRow("C short")  << QString("C") << QString("Sun") << 7 << QLocale::ShortFormat;
    QTest::newRow("C narrow")  << QString("C") << QString("S") << 7 << QLocale::NarrowFormat;

    QTest::newRow("ru_RU long")
        << QString("ru_RU")
        << QString::fromUtf8("\320\262\320\276\321\201\320\272\321\200\320"
                             "\265\321\201\320\265\320\275\321\214\320\265")
        << 7 << QLocale::LongFormat;
    QTest::newRow("ru_RU short")
        << QString("ru_RU") << QString::fromUtf8("\320\262\321\201") << 7 << QLocale::ShortFormat;
    QTest::newRow("ru_RU narrow")
        << QString("ru_RU") << QString::fromUtf8("\320\222") << 7 << QLocale::NarrowFormat;
}

void tst_QLocale::standaloneDayName()
{
    QFETCH(QString, locale_name);
    QFETCH(QString, dayName);
    QFETCH(int, day);
    QFETCH(QLocale::FormatType, format);

    QLocale l(locale_name);
    QCOMPARE(l.standaloneDayName(day, format), dayName);
}

void tst_QLocale::underflowOverflow()
{
    QString a(QLatin1String("0.") + QString(546, QLatin1Char('0')) + QLatin1String("1e10"));
    bool ok = false;
    double d = a.toDouble(&ok);
    QVERIFY(!ok);
    QCOMPARE(d, 0.0);

    a = QLatin1String("1e600");
    ok = false;
    d = a.toDouble(&ok);
    QVERIFY(!ok); // detectable overflow
    QVERIFY(qIsInf(d));

    a = QLatin1String("-1e600");
    ok = false;
    d = a.toDouble(&ok);
    QVERIFY(!ok); // detectable underflow
    QVERIFY(qIsInf(-d));

    a = QLatin1String("1e-600");
    ok = false;
    d = a.toDouble(&ok);
    QVERIFY(!ok);
    QCOMPARE(d, 0.0);

    a = QLatin1String("-9223372036854775809");
    a.toLongLong(&ok);
    QVERIFY(!ok);
}

void tst_QLocale::defaultNumberingSystem_data()
{
    QTest::addColumn<QString>("expect");

    QTest::newRow("sk_SK") << QStringLiteral("123");
    QTest::newRow("ta_IN") << QStringLiteral("123");
    QTest::newRow("te_IN") << QStringLiteral("123");
    QTest::newRow("hi_IN") << QStringLiteral("123");
    QTest::newRow("gu_IN") << QStringLiteral("123");
    QTest::newRow("kn_IN") << QStringLiteral("123");
    QTest::newRow("pa_IN") << QStringLiteral("123");
    QTest::newRow("ne_IN") << QString::fromUtf8("१२३");
    QTest::newRow("mr_IN") << QString::fromUtf8("१२३");
    QTest::newRow("ml_IN") << QStringLiteral("123");
    QTest::newRow("kok_IN") << QStringLiteral("123");
}

void tst_QLocale::defaultNumberingSystem()
{
    QFETCH(QString, expect);
    QLatin1String name(QTest::currentDataTag());
    QLocale locale(name);
    QCOMPARE(locale.toString(123), expect);
}

void tst_QLocale::measurementSystems()
{
    QLocale locale(QLocale::English, QLocale::UnitedStates);
    QCOMPARE(locale.measurementSystem(), QLocale::ImperialUSSystem);

    locale = QLocale(QLocale::English, QLocale::UnitedKingdom);
    QCOMPARE(locale.measurementSystem(), QLocale::ImperialUKSystem);

    locale = QLocale(QLocale::English, QLocale::Australia);
    QCOMPARE(locale.measurementSystem(), QLocale::MetricSystem);

    locale = QLocale(QLocale::German);
    QCOMPARE(locale.measurementSystem(), QLocale::MetricSystem);
}

void tst_QLocale::systemMeasurementSystems_data()
{
    QTest::addColumn<QString>("lcAllLocale");
    QTest::addColumn<QString>("lcMeasurementLocale");
    QTest::addColumn<QString>("langLocale");
    QTest::addColumn<int>("mSystem");

    QTest::newRow("row0") << QString("no_NO") << QString("no_NO") << QString("no_NO")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row1") << QString("no_NO") << QString("no_NO") << QString("en_US")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row2") << QString("no_NO") << QString("en_US") << QString("no_NO")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row3") << QString("no_NO") << QString("en_US") << QString("en_US")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row4") << QString("en_US") << QString("no_NO") << QString("no_NO")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row5") << QString("en_US") << QString("no_NO") << QString("en_US")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row6") << QString("en_US") << QString("en_US") << QString("no_NO")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row7") << QString("en_US") << QString("en_US") << QString("en_US")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row8") << QString("") << QString("") << QString("")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row9") << QString("") << QString("") << QString("no_NO")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row10") << QString("") << QString("") << QString("en_US")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row11") << QString("") << QString("no_NO") << QString("")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row12") << QString("") << QString("en_US") << QString("")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row13") << QString("") << QString("no_NO") << QString("en_US")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row14") << QString("") << QString("en_US") << QString("no_NO")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row15") << QString("no_NO") << QString("") << QString("")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row16") << QString("en_US") << QString("") << QString("")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row17") << QString("no_NO") << QString("en_US") << QString("")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row18") << QString("en_US") << QString("no_NO") << QString("")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row19") << QString("no_NO") << QString("") << QString("en_US")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row20") << QString("en_US") << QString("") << QString("no_NO")
            << (int)QLocale::ImperialSystem;
}

void tst_QLocale::systemMeasurementSystems()
{
    QSKIP("Meh, skip the test as we do not reread the environment variables anymore", SkipAll);
    // Theoretically, we could include HPUX in this test, but its setenv implementation
    // stinks. It's called putenv, and it requires you to keep the variable you pass
    // to it around forever.
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
    QFETCH(QString, lcAllLocale);
    QFETCH(QString, lcMeasurementLocale);
    QFETCH(QString, langLocale);
    QFETCH(int, mSystem);

    // Save any old environment variables.
    QString oldLcAll = QString::fromLocal8Bit(getenv("LC_ALL"));
    QString oldLcMeasurement = QString::fromLocal8Bit(getenv("LC_MEASUREMENT"));
    QString oldLang = QString::fromLocal8Bit(getenv("LANG"));

    qputenv("LC_ALL", lcAllLocale.toLocal8Bit());
    qputenv("LC_MEASUREMENT", lcMeasurementLocale.toLocal8Bit());
    qputenv("LANG", langLocale.toLocal8Bit());

    // Method should be const.
    const QLocale locale(QLocale::system());
    QCOMPARE((int)locale.measurementSystem(), mSystem);

    // Restore environment.
    qputenv("LC_ALL", oldLcAll.toLocal8Bit());
    qputenv("LC_MEASUREMENT", oldLcMeasurement.toLocal8Bit());
    qputenv("LANG", oldLang.toLocal8Bit());
#else
    QSKIP("Test doesn't work on Mac, Windows or Symbian", SkipAll);
#endif
}

class SystemLocale : public QSystemLocale
{
public:
    virtual QVariant query(QueryType type, QVariant in) const
    {
        switch (type) {
        case DateTimeFormatLong: return QLatin1String("dddd ddd dd d MMMM MMM MM M yyyy hh:mm:ss.zzz");
        case DateTimeFormatShort: return QLatin1String("d M yy h:m");
        case DateTimeToStringLong:
        case DateTimeToStringShort:
            return in.toDateTime().toString(type == DateTimeToStringShort
                                            ? QLatin1String("dMyyhm")
                                            : QLatin1String("ddMMyyyyhhmmsszzz"));
        default:
            break;
        }
        return QSystemLocale::query(type, in);
    }
};



void tst_QLocale::queryDateTime()
{
    SystemLocale loc;
    QCOMPARE(QLocale::system().dateTimeFormat(QLocale::LongFormat),
             loc.query(QSystemLocale::DateTimeFormatLong, QVariant()).toString());
    QCOMPARE(QLocale::system().dateTimeFormat(QLocale::ShortFormat),
             loc.query(QSystemLocale::DateTimeFormatShort, QVariant()).toString());
    QCOMPARE(QLocale::system().toString(QDateTime(QDate(1974, 12, 1), QTime(1, 2, 3, 4)), QLocale::ShortFormat),
             QString("1127412"));
    QCOMPARE(QLocale::system().toString(QDateTime(QDate(1974, 12, 1), QTime(1, 2, 3, 4)), QLocale::NarrowFormat),
             QLocale::system().toString(QDateTime(QDate(1974, 12, 1), QTime(1, 2, 3, 4)), QLocale::ShortFormat));
    QCOMPARE(QLocale::system().toString(QDateTime(QDate(1974, 12, 1), QTime(1, 2, 3, 4)), QLocale::LongFormat),
             QString("01121974010203004"));
}

#ifndef QT_NO_SYSTEMLOCALE
void tst_QLocale::queryMeasureSystem_data()
{
    QTest::addColumn<QString>("lcAllLocale");
    QTest::addColumn<QString>("lcMeasurementLocale");
    QTest::addColumn<QString>("langLocale");
    QTest::addColumn<int>("mSystem");

    QTest::newRow("row0") << QString("no_NO") << QString("no_NO") << QString("no_NO")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row1") << QString("no_NO") << QString("no_NO") << QString("en_US")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row2") << QString("no_NO") << QString("en_US") << QString("no_NO")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row3") << QString("no_NO") << QString("en_US") << QString("en_US")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row4") << QString("en_US") << QString("no_NO") << QString("no_NO")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row5") << QString("en_US") << QString("no_NO") << QString("en_US")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row6") << QString("en_US") << QString("en_US") << QString("no_NO")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row7") << QString("en_US") << QString("en_US") << QString("en_US")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row8") << QString("") << QString("") << QString("")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row9") << QString("") << QString("") << QString("no_NO")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row10") << QString("") << QString("") << QString("en_US")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row11") << QString("") << QString("no_NO") << QString("")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row12") << QString("") << QString("en_US") << QString("")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row13") << QString("") << QString("no_NO") << QString("en_US")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row14") << QString("") << QString("en_US") << QString("no_NO")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row15") << QString("no_NO") << QString("") << QString("")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row16") << QString("en_US") << QString("") << QString("")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row17") << QString("no_NO") << QString("en_US") << QString("")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row18") << QString("en_US") << QString("no_NO") << QString("")
            << (int)QLocale::ImperialSystem;
    QTest::newRow("row19") << QString("no_NO") << QString("") << QString("en_US")
            << (int)QLocale::MetricSystem;
    QTest::newRow("row20") << QString("en_US") << QString("") << QString("no_NO")
            << (int)QLocale::ImperialSystem;
}

void tst_QLocale::queryMeasureSystem()
{
    QSKIP("Meh, skip the test as we do not reread the environment variables anymore", SkipAll);
    // Theoretically, we could include HPUX in this test, but its setenv implementation
    // stinks. It's called putenv, and it requires you to keep the variable you pass
    // to it around forever.
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
    QFETCH(QString, lcAllLocale);
    QFETCH(QString, lcMeasurementLocale);
    QFETCH(QString, langLocale);
    QFETCH(int, mSystem);

    // Save any old environment variables.
    QString oldLcAll = QString::fromLocal8Bit(getenv("LC_ALL"));
    QString oldLcMeasurement = QString::fromLocal8Bit(getenv("LC_MEASUREMENT"));
    QString oldLang = QString::fromLocal8Bit(getenv("LANG"));

    qputenv("LC_ALL", lcAllLocale.toLocal8Bit());
    qputenv("LC_MEASUREMENT", lcMeasurementLocale.toLocal8Bit());
    qputenv("LANG", langLocale.toLocal8Bit());

    // Method should be const.
    const QSystemLocale locale;
    QCOMPARE(locale.query(QSystemLocale::MeasurementSystem, QVariant()).toInt(), mSystem);

    // Restore environment.
    qputenv("LC_ALL", oldLcAll.toLocal8Bit());
    qputenv("LC_MEASUREMENT", oldLcMeasurement.toLocal8Bit());
    qputenv("LANG", oldLang.toLocal8Bit());
#else
    QSKIP("Test doesn't work on Mac, Windows or Symbian", SkipAll);
#endif
}
#endif // QT_NO_SYSTEMLOCALE

void tst_QLocale::ampm_data()
{
    QTest::addColumn<QString>("morn");
    QTest::addColumn<QString>("even");

    QTest::newRow("C") << QStringLiteral("AM") << QStringLiteral("PM");
    QTest::newRow("de_DE") << QStringLiteral("AM") << QStringLiteral("PM");
    QTest::newRow("sv_SE") << QStringLiteral("fm") << QStringLiteral("em");
    QTest::newRow("nl_NL") << QStringLiteral("a.m.") << QStringLiteral("p.m.");
    QTest::newRow("uk_UA") << QString::fromUtf8("\320\264\320\277")
                           << QString::fromUtf8("\320\277\320\277");
    QTest::newRow("tr_TR") << QString::fromUtf8("\303\226\303\226")
                           << QString::fromUtf8("\303\226\123");
    QTest::newRow("id_ID") << QStringLiteral("AM") << QStringLiteral("PM");
    QTest::newRow("ta_LK") << QString::fromUtf8("முற்பகல்") << QString::fromUtf8("பிற்பகல்");
}

void tst_QLocale::ampm()
{
    QFETCH(QString, morn);
    QFETCH(QString, even);
    QLatin1String name(QTest::currentDataTag());
    QLocale locale(name == QLatin1String("C") ? QLocale(QLocale::C) : QLocale(name));
    QCOMPARE(locale.amText(), morn);
    QCOMPARE(locale.pmText(), even);
}

void tst_QLocale::dateFormat()
{
    const QLocale c(QLocale::C);
    // check that the NarrowFormat is the same as ShortFormat.
    QCOMPARE(c.dateFormat(QLocale::NarrowFormat), c.dateFormat(QLocale::ShortFormat));

    const QLocale no("no_NO");
    QCOMPARE(no.dateFormat(QLocale::NarrowFormat), QLatin1String("dd.MM.yyyy"));
    QCOMPARE(no.dateFormat(QLocale::ShortFormat), QLatin1String("dd.MM.yyyy"));
    QCOMPARE(no.dateFormat(QLocale::LongFormat), QLatin1String("dddd d. MMMM yyyy"));

    const QLocale ca("en_CA");
    QCOMPARE(ca.dateFormat(QLocale::ShortFormat), QLatin1String("yyyy-MM-dd"));
    QCOMPARE(ca.dateFormat(QLocale::LongFormat), QLatin1String("dddd, MMMM d, yyyy"));

    const QLocale ja("ja_JP");
    QCOMPARE(ja.dateFormat(QLocale::ShortFormat), QLatin1String("yyyy/MM/dd"));

    const QLocale ir("ga_IE");
    QCOMPARE(ir.dateFormat(QLocale::ShortFormat), QLatin1String("dd/MM/yyyy"));
}

void tst_QLocale::timeFormat()
{
    const QLocale c(QLocale::C);
    // check that the NarrowFormat is the same as ShortFormat.
    QCOMPARE(c.timeFormat(QLocale::NarrowFormat), c.timeFormat(QLocale::ShortFormat));

    const QLocale no("no_NO");
    QCOMPARE(no.timeFormat(QLocale::NarrowFormat), QLatin1String("HH.mm"));
    QCOMPARE(no.timeFormat(QLocale::ShortFormat), QLatin1String("HH.mm"));
    QCOMPARE(no.timeFormat(QLocale::LongFormat), QLatin1String("HH.mm.ss t"));

    const QLocale id("id_ID");
    QCOMPARE(id.timeFormat(QLocale::ShortFormat), QLatin1String("HH.mm"));
    QCOMPARE(id.timeFormat(QLocale::LongFormat), QLatin1String("HH.mm.ss t"));

    const QLocale cat("ca_ES");
    QCOMPARE(cat.timeFormat(QLocale::ShortFormat), QLatin1String("H:mm"));
    QCOMPARE(cat.timeFormat(QLocale::LongFormat), QLatin1String("H:mm:ss t"));

    const QLocale bra("pt_BR");
    QCOMPARE(bra.timeFormat(QLocale::ShortFormat), QLatin1String("HH:mm"));
    QCOMPARE(bra.timeFormat(QLocale::LongFormat), QLatin1String("HH:mm:ss t"));
}

void tst_QLocale::dateTimeFormat()
{
    const QLocale c(QLocale::C);
    // check that the NarrowFormat is the same as ShortFormat.
    QCOMPARE(c.dateTimeFormat(QLocale::NarrowFormat), c.dateTimeFormat(QLocale::ShortFormat));

    const QLocale no("no_NO");
    QCOMPARE(no.dateTimeFormat(QLocale::NarrowFormat), QLatin1String("dd.MM.yyyy HH.mm"));
    QCOMPARE(no.dateTimeFormat(QLocale::ShortFormat), QLatin1String("dd.MM.yyyy HH.mm"));
    QCOMPARE(no.dateTimeFormat(QLocale::LongFormat), QLatin1String("dddd d. MMMM yyyy HH.mm.ss t"));
}

void tst_QLocale::monthName()
{
    const QLocale c(QLocale::C);
    QCOMPARE(c.monthName(0, QLocale::ShortFormat), QString());
    QCOMPARE(c.monthName(0, QLocale::LongFormat), QString());
    QCOMPARE(c.monthName(0, QLocale::NarrowFormat), QString());
    QCOMPARE(c.monthName(13, QLocale::ShortFormat), QString());
    QCOMPARE(c.monthName(13, QLocale::LongFormat), QString());
    QCOMPARE(c.monthName(13, QLocale::NarrowFormat), QString());

    QCOMPARE(c.monthName(1, QLocale::LongFormat), QLatin1String("January"));
    QCOMPARE(c.monthName(1, QLocale::ShortFormat), QLatin1String("Jan"));
    QCOMPARE(c.monthName(1, QLocale::NarrowFormat), QLatin1String("1"));

    const QLocale de("de_DE");
    QCOMPARE(de.monthName(12, QLocale::LongFormat), QLatin1String("Dezember"));
    QCOMPARE(de.monthName(12, QLocale::ShortFormat), QLatin1String("Dez."));
    // 'de' locale doesn't have narrow month name
    QCOMPARE(de.monthName(12, QLocale::NarrowFormat), QLatin1String("D"));

    QLocale ru("ru_RU");
    QCOMPARE(ru.monthName(1, QLocale::LongFormat),
             QString::fromUtf8("\321\217\320\275\320\262\320\260\321\200\321\217"));
    QCOMPARE(ru.monthName(1, QLocale::ShortFormat),
             QString::fromUtf8("\321\217\320\275\320\262\56"));
    QCOMPARE(ru.monthName(1, QLocale::NarrowFormat), QString::fromUtf8("\320\257"));

    QLocale ir("ga_IE");
    QCOMPARE(ir.monthName(1, QLocale::ShortFormat), QLatin1String("Ean"));
    QCOMPARE(ir.monthName(12, QLocale::ShortFormat), QLatin1String("Noll"));

    QLocale cz("cs_CZ");
    QCOMPARE(cz.monthName(1, QLocale::ShortFormat), QLatin1String("led"));
    QCOMPARE(cz.monthName(12, QLocale::ShortFormat), QLatin1String("pro"));
}

void tst_QLocale::standaloneMonthName()
{
    const QLocale c(QLocale::C);
    QCOMPARE(c.monthName(0, QLocale::ShortFormat), QString());
    QCOMPARE(c.monthName(0, QLocale::LongFormat), QString());
    QCOMPARE(c.monthName(0, QLocale::NarrowFormat), QString());
    QCOMPARE(c.monthName(13, QLocale::ShortFormat), QString());
    QCOMPARE(c.monthName(13, QLocale::LongFormat), QString());
    QCOMPARE(c.monthName(13, QLocale::NarrowFormat), QString());

    QCOMPARE(c.standaloneMonthName(1, QLocale::LongFormat), QLatin1String("January"));
    QCOMPARE(c.standaloneMonthName(1, QLocale::ShortFormat), QLatin1String("Jan"));

    const QLocale de("de_DE");
    // For de_DE locale Unicode CLDR database doesn't contain standalone long months
    // so just checking if the return value is the same as in monthName().
    QCOMPARE(de.standaloneMonthName(12, QLocale::LongFormat), QLatin1String("Dezember"));
    QCOMPARE(de.standaloneMonthName(12, QLocale::LongFormat), de.monthName(12, QLocale::LongFormat));
    QCOMPARE(de.standaloneMonthName(12, QLocale::ShortFormat), QLatin1String("Dez"));
    QCOMPARE(de.standaloneMonthName(12, QLocale::NarrowFormat), QLatin1String("D"));

    QLocale ru("ru_RU");
    QCOMPARE(ru.standaloneMonthName(1, QLocale::LongFormat),
             QString::fromUtf8("\xd1\x8f\xd0\xbd\xd0\xb2\xd0\xb0\xd1\x80\xd1\x8c"));
    QCOMPARE(ru.standaloneMonthName(1, QLocale::ShortFormat),
             QString::fromUtf8("\xd1\x8f\xd0\xbd\xd0\xb2."));
    QCOMPARE(ru.standaloneMonthName(1, QLocale::NarrowFormat), QString::fromUtf8("\xd0\xaf"));
}

void tst_QLocale::currency()
{
    const QLocale c(QLocale::C);
    QCOMPARE(c.toCurrencyString(qulonglong(1234)), QString("1234"));
    QCOMPARE(c.toCurrencyString(qlonglong(-1234)), QString("-1234"));
    QCOMPARE(c.toCurrencyString(double(1234.56)), QString("1234.56"));
    QCOMPARE(c.toCurrencyString(double(-1234.56)), QString("-1234.56"));

    const QLocale en_US("en_US");
    QCOMPARE(en_US.toCurrencyString(qulonglong(1234)), QString("$1,234"));
    QCOMPARE(en_US.toCurrencyString(qlonglong(-1234)), QString("$-1,234"));
    QCOMPARE(en_US.toCurrencyString(double(1234.56)), QString("$1,234.56"));
    QCOMPARE(en_US.toCurrencyString(double(-1234.56)), QString("$-1,234.56"));
    QCOMPARE(en_US.toCurrencyString(double(-1234.5678)), QString("$-1,234.57"));

    const QLocale ru_RU("ru_RU");
    QCOMPARE(ru_RU.toCurrencyString(qulonglong(1234)),
             QString::fromUtf8("1" "\xc2\xa0" "234\xc2\xa0\xe2\x82\xbd"));
    QCOMPARE(ru_RU.toCurrencyString(qlonglong(-1234)),
             QString::fromUtf8("-1" "\xc2\xa0" "234\xc2\xa0\xe2\x82\xbd"));
    QCOMPARE(ru_RU.toCurrencyString(double(1234.56)),
             QString::fromUtf8("1" "\xc2\xa0" "234,56\xc2\xa0\xe2\x82\xbd"));
    QCOMPARE(ru_RU.toCurrencyString(double(-1234.56)),
             QString::fromUtf8("-1" "\xc2\xa0" "234,56\xc2\xa0\xe2\x82\xbd"));

    const QLocale de_DE("de_DE");
    QCOMPARE(de_DE.toCurrencyString(qulonglong(1234)),
             QString::fromUtf8("1.234\xc2\xa0\xe2\x82\xac"));
    QCOMPARE(de_DE.toCurrencyString(qulonglong(1234), QLatin1String("BAZ")),
             QString::fromUtf8("1.234\xc2\xa0" "BAZ"));
    QCOMPARE(de_DE.toCurrencyString(qlonglong(-1234)),
             QString::fromUtf8("-1.234\xc2\xa0\xe2\x82\xac"));
    QCOMPARE(de_DE.toCurrencyString(qlonglong(-1234), QLatin1String("BAZ")),
             QString::fromUtf8("-1.234\xc2\xa0" "BAZ"));
    QCOMPARE(de_DE.toCurrencyString(double(1234.56)),
             QString::fromUtf8("1.234,56\xc2\xa0\xe2\x82\xac"));
    QCOMPARE(de_DE.toCurrencyString(double(-1234.56)),
             QString::fromUtf8("-1.234,56\xc2\xa0\xe2\x82\xac"));
    QCOMPARE(de_DE.toCurrencyString(double(-1234.56), QLatin1String("BAZ")),
             QString::fromUtf8("-1.234,56\xc2\xa0" "BAZ"));

    const QLocale es_CR(QLocale::Spanish, QLocale::CostaRica);
    QCOMPARE(es_CR.toCurrencyString(double(1565.25)),
             QString::fromUtf8("\xE2\x82\xA1" "1\xC2\xA0" "565,25"));

    const QLocale system = QLocale::system();
    QVERIFY(system.toCurrencyString(1, QLatin1String("FOO")).contains(QLatin1String("FOO")));
}

void tst_QLocale::quoteString()
{
    const QString someText("text");
    const QLocale c(QLocale::C);
    QCOMPARE(c.quoteString(someText), QString::fromUtf8("\x22" "text" "\x22"));
    QCOMPARE(c.quoteString(someText, QLocale::AlternateQuotation),
             QString::fromUtf8("\x27" "text" "\x27"));

    const QLocale de_CH("de_CH");
    QCOMPARE(de_CH.quoteString(someText), QString::fromUtf8("\xe2\x80\x9e" "text" "\xe2\x80\x9c"));
    QCOMPARE(de_CH.quoteString(someText, QLocale::AlternateQuotation),
             QString::fromUtf8("\xe2\x80\x9a" "text" "\xe2\x80\x98"));


}

void tst_QLocale::uiLanguages()
{
    const QLocale c(QLocale::C);
    QCOMPARE(c.uiLanguages().size(), 1);
    QCOMPARE(c.uiLanguages().at(0), QLatin1String("C"));

    const QLocale en_US("en_US");
    QCOMPARE(en_US.uiLanguages().size(), 3);
    QCOMPARE(en_US.uiLanguages().at(0), QLatin1String("en"));
    QCOMPARE(en_US.uiLanguages().at(1), QLatin1String("en-US"));
    QCOMPARE(en_US.uiLanguages().at(2), QLatin1String("en-Latn-US"));

    const QLocale en_Latn_US("en_Latn_US");
    QCOMPARE(en_Latn_US.uiLanguages().size(), 3);
    QCOMPARE(en_Latn_US.uiLanguages().at(0), QLatin1String("en"));
    QCOMPARE(en_Latn_US.uiLanguages().at(1), QLatin1String("en-US"));
    QCOMPARE(en_Latn_US.uiLanguages().at(2), QLatin1String("en-Latn-US"));

    const QLocale en_GB("en_GB");
    QCOMPARE(en_GB.uiLanguages().size(), 2);
    QCOMPARE(en_GB.uiLanguages().at(0), QLatin1String("en-GB"));
    QCOMPARE(en_GB.uiLanguages().at(1), QLatin1String("en-Latn-GB"));

    const QLocale en_Dsrt_US("en_Dsrt_US");
    QCOMPARE(en_Dsrt_US.uiLanguages().size(), 2);
    QCOMPARE(en_Dsrt_US.uiLanguages().at(0), QLatin1String("en-Dsrt"));
    QCOMPARE(en_Dsrt_US.uiLanguages().at(1), QLatin1String("en-Dsrt-US"));

    const QLocale ru_RU("ru_RU");
    QCOMPARE(ru_RU.uiLanguages().size(), 3);
    QCOMPARE(ru_RU.uiLanguages().at(0), QLatin1String("ru"));
    QCOMPARE(ru_RU.uiLanguages().at(1), QLatin1String("ru-RU"));
    QCOMPARE(ru_RU.uiLanguages().at(2), QLatin1String("ru-Cyrl-RU"));

    const QLocale zh_Hant("zh_Hant");
    QCOMPARE(zh_Hant.uiLanguages().size(), 2);
    QCOMPARE(zh_Hant.uiLanguages().at(0), QLatin1String("zh-TW"));
    QCOMPARE(zh_Hant.uiLanguages().at(1), QLatin1String("zh-Hant-TW"));
}

void tst_QLocale::weekendDays()
{
    const QLocale c(QLocale::C);
    QList<Qt::DayOfWeek> days;
    days << Qt::Monday << Qt::Tuesday << Qt::Wednesday << Qt::Thursday << Qt::Friday;
    QCOMPARE(c.weekdays(), days);
}

void tst_QLocale::listPatterns()
{
    QStringList sl1;
    QStringList sl2;
    sl2 << "aaa";
    QStringList sl3;
    sl3 << "aaa" << "bbb";
    QStringList sl4;
    sl4 << "aaa" << "bbb" << "ccc";
    QStringList sl5;
    sl5 << "aaa" << "bbb" << "ccc" << "ddd";

    const QLocale c(QLocale::C);
    QCOMPARE(c.createSeparatedList(sl1), QString(""));
    QCOMPARE(c.createSeparatedList(sl2), QString("aaa"));
    QCOMPARE(c.createSeparatedList(sl3), QString("aaa, bbb"));
    QCOMPARE(c.createSeparatedList(sl4), QString("aaa, bbb, ccc"));
    QCOMPARE(c.createSeparatedList(sl5), QString("aaa, bbb, ccc, ddd"));

    const QLocale en_US("en_US");
    QCOMPARE(en_US.createSeparatedList(sl1), QString(""));
    QCOMPARE(en_US.createSeparatedList(sl2), QString("aaa"));
    QCOMPARE(en_US.createSeparatedList(sl3), QString("aaa and bbb"));
    QCOMPARE(en_US.createSeparatedList(sl4), QString("aaa, bbb, and ccc"));
    QCOMPARE(en_US.createSeparatedList(sl5), QString("aaa, bbb, ccc, and ddd"));

    const QLocale zh_CN("zh_CN");
    QCOMPARE(zh_CN.createSeparatedList(sl1), QString(""));
    QCOMPARE(zh_CN.createSeparatedList(sl2), QString("aaa"));
    QCOMPARE(zh_CN.createSeparatedList(sl3), QString::fromUtf8("aaa" "\xe5\x92\x8c" "bbb"));
    QCOMPARE(zh_CN.createSeparatedList(sl4), QString::fromUtf8("aaa" "\xe3\x80\x81" "bbb" "\xe5\x92\x8c" "ccc"));
    QCOMPARE(zh_CN.createSeparatedList(sl5), QString::fromUtf8("aaa" "\xe3\x80\x81" "bbb" "\xe3\x80\x81" "ccc" "\xe5\x92\x8c" "ddd"));
}

void tst_QLocale::QTBUG_26035_positivesign()
{
    QLocale locale(QLocale::C);
    bool ok (false);
    QCOMPARE(locale.toInt(QString("+100,000"), &ok), 100000);
    QVERIFY(ok);
    ok = false;
    QCOMPARE(locale.toInt(QString("+100,000,000"), &ok), 100000000);
    QVERIFY(ok);
    ok = false;
    QCOMPARE(locale.toLongLong(QString("+100,000"), &ok), (qlonglong)100000);
    QVERIFY(ok);
    ok = false;
    QCOMPARE(locale.toLongLong(QString("+100,000,000"), &ok), (qlonglong)100000000);
    QVERIFY(ok);
}

void tst_QLocale::textDirection_data()
{
    QTest::addColumn<int>("language");
    QTest::addColumn<int>("script");
    QTest::addColumn<bool>("rightToLeft");

    for (int language = QLocale::C; language <= QLocale::LastLanguage; ++language) {
        bool rightToLeft = false;
        switch (language) {
        // based on likelySubtags for RTL scripts
        case QLocale::Arabic:
        //case QLocale::Divehi:
//        case QLocale::Fulah:
//        case QLocale::Hausa:
        case QLocale::Hebrew:
//        case QLocale::Hungarian:
        //case QLocale::Kashmiri:
//        case QLocale::Kurdish:
        //case QLocale::Pashto:
        case QLocale::Persian:
        //case QLocale::Sindhi:
        case QLocale::Syriac:
        case QLocale::Urdu:
        //case QLocale::Yiddish:
            // false if there is no locale data for language:
            rightToLeft = (QLocale(QLocale::Language(language)).language()
                           == QLocale::Language(language));
            break;
        default:
            break;
        }
        const QString testName = QLocalePrivate::languageToCode(QLocale::Language(language));
        QTest::newRow(qPrintable(testName)) << language << int(QLocale::AnyScript) << rightToLeft;
    }
    //QTest::newRow("pa_Arab") << int(QLocale::Punjabi) << int(QLocale::ArabicScript) << true;
    //QTest::newRow("uz_Arab") << int(QLocale::Uzbek) << int(QLocale::ArabicScript) << true;
}

void tst_QLocale::textDirection()
{
    QFETCH(int, language);
    QFETCH(int, script);
    QFETCH(bool, rightToLeft);

    QLocale locale(QLocale::Language(language), QLocale::Script(script), QLocale::AnyCountry);
    QCOMPARE(locale.textDirection() == Qt::RightToLeft, rightToLeft);
}

void tst_QLocale::bcp47Name_data()
{
    QTest::addColumn<QString>("expect");

    //QTest::newRow("C") << QStringLiteral("en");
    QTest::newRow("en") << QStringLiteral("en");
    QTest::newRow("en_US") << QStringLiteral("en");
    QTest::newRow("en_GB") << QStringLiteral("en-GB");
    QTest::newRow("en_DE") << QStringLiteral("en-DE");
    QTest::newRow("de_DE") << QStringLiteral("de");
    QTest::newRow("sr_RS") << QStringLiteral("sr");
    QTest::newRow("sr_Cyrl_RS") << QStringLiteral("sr");
    //QTest::newRow("sr_Latn_RS") << QStringLiteral("sr-Latn");
    QTest::newRow("sr_ME") << QStringLiteral("sr-ME");
    QTest::newRow("sr_Cyrl_ME") << QStringLiteral("sr-Cyrl-ME");
    QTest::newRow("sr_Latn_ME") << QStringLiteral("sr-ME");

    // Fall back to defaults when country isn't in CLDR for this language:
    QTest::newRow("sr_HR") << QStringLiteral("sr");
    QTest::newRow("sr_Cyrl_HR") << QStringLiteral("sr");
    //QTest::newRow("sr_Latn_HR") << QStringLiteral("sr-Latn");
}

void tst_QLocale::bcp47Name()
{
    QFETCH(QString, expect);
    QCOMPARE(QLocale(QLatin1String(QTest::currentDataTag())).bcp47Name(), expect);
}

QTEST_APPLESS_MAIN(tst_QLocale)
#include "tst_qlocale.moc"

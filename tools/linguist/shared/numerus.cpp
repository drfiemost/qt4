/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include "translator.h"

#include <QtCore/QByteArray>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMap>

#include <private/qtranslator_p.h>

QT_BEGIN_NAMESPACE

static const uchar englishStyleRules[] =
    { Q_EQ, 1 };
static const uchar frenchStyleRules[] =
    { Q_LEQ, 1 };
static const uchar latvianRules[] =
    { Q_MOD_10 | Q_EQ, 1, Q_AND, Q_MOD_100 | Q_NEQ, 11, Q_NEWRULE,
      Q_NEQ, 0 };
static const uchar icelandicRules[] =
    { Q_MOD_10 | Q_EQ, 1, Q_AND, Q_MOD_100 | Q_NEQ, 11 };
static const uchar irishStyleRules[] =
    { Q_EQ, 1, Q_NEWRULE,
      Q_EQ, 2 };
static const uchar slovakStyleRules[] =
    { Q_EQ, 1, Q_NEWRULE,
      Q_BETWEEN, 2, 4 };
static const uchar macedonianRules[] =
    { Q_MOD_10 | Q_EQ, 1, Q_NEWRULE,
      Q_MOD_10 | Q_EQ, 2 };
static const uchar lithuanianRules[] =
    { Q_MOD_10 | Q_EQ, 1, Q_AND, Q_MOD_100 | Q_NEQ, 11, Q_NEWRULE,
      Q_MOD_10 | Q_NEQ, 0, Q_AND, Q_MOD_100 | Q_NOT_BETWEEN, 10, 19 };
static const uchar russianStyleRules[] =
    { Q_MOD_10 | Q_EQ, 1, Q_AND, Q_MOD_100 | Q_NEQ, 11, Q_NEWRULE,
      Q_MOD_10 | Q_BETWEEN, 2, 4, Q_AND, Q_MOD_100 | Q_NOT_BETWEEN, 10, 19 };
static const uchar polishRules[] =
    { Q_EQ, 1, Q_NEWRULE,
      Q_MOD_10 | Q_BETWEEN, 2, 4, Q_AND, Q_MOD_100 | Q_NOT_BETWEEN, 10, 19 };
static const uchar romanianRules[] =
    { Q_EQ, 1, Q_NEWRULE,
      Q_EQ, 0, Q_OR, Q_MOD_100 | Q_BETWEEN, 1, 19 };
static const uchar slovenianRules[] =
    { Q_MOD_100 | Q_EQ, 1, Q_NEWRULE,
      Q_MOD_100 | Q_EQ, 2, Q_NEWRULE,
      Q_MOD_100 | Q_BETWEEN, 3, 4 };
static const uchar malteseRules[] =
    { Q_EQ, 1, Q_NEWRULE,
      Q_EQ, 0, Q_OR, Q_MOD_100 | Q_BETWEEN, 1, 10, Q_NEWRULE,
      Q_MOD_100 | Q_BETWEEN, 11, 19 };
static const uchar welshRules[] =
    { Q_EQ, 0, Q_NEWRULE,
      Q_EQ, 1, Q_NEWRULE,
      Q_BETWEEN, 2, 5, Q_NEWRULE,
      Q_EQ, 6 };
static const uchar arabicRules[] =
    { Q_EQ, 0, Q_NEWRULE,
      Q_EQ, 1, Q_NEWRULE,
      Q_EQ, 2, Q_NEWRULE,
      Q_MOD_100 | Q_BETWEEN, 3, 10, Q_NEWRULE,
      Q_MOD_100 | Q_GEQ, 11 };
static const uchar tagalogRules[] =
    { Q_LEQ, 1, Q_NEWRULE,
      Q_MOD_10 | Q_EQ, 4, Q_OR, Q_MOD_10 | Q_EQ, 6, Q_OR, Q_MOD_10 | Q_EQ, 9 };
static const uchar catalanRules[] =
    { Q_EQ, 1, Q_NEWRULE,
      Q_LEAD_1000 | Q_EQ, 11 };

static const char * const japaneseStyleForms[] = { "Universal Form", nullptr };
static const char * const englishStyleForms[] = { "Singular", "Plural", nullptr };
static const char * const frenchStyleForms[] = { "Singular", "Plural", nullptr };
static const char * const icelandicForms[] = { "Singular", "Plural", nullptr };
static const char * const latvianForms[] = { "Singular", "Plural", "Nullar", nullptr };
static const char * const irishStyleForms[] = { "Singular", "Dual", "Plural", nullptr };
static const char * const slovakStyleForms[] = { "Singular", "Paucal", "Plural", nullptr };
static const char * const macedonianForms[] = { "Singular", "Dual", "Plural", nullptr };
static const char * const lithuanianForms[] = { "Singular", "Paucal", "Plural", nullptr };
static const char * const russianStyleForms[] = { "Singular", "Dual", "Plural", nullptr };
static const char * const polishForms[] = { "Singular", "Paucal", "Plural", nullptr };
static const char * const romanianForms[] = { "Singular", "Paucal", "Plural", nullptr };
static const char * const slovenianForms[] = { "Singular", "Dual", "Trial", "Plural", nullptr };
static const char * const malteseForms[] =
    { "Singular", "Paucal", "Greater Paucal", "Plural", nullptr };
static const char * const welshForms[] =
    { "Nullar", "Singular", "Dual", "Sexal", "Plural", nullptr };
static const char * const arabicForms[] =
    { "Nullar", "Singular", "Dual", "Minority Plural", "Plural", "Plural (100-102, ...)", nullptr };
static const char * const tagalogForms[] =
    { "Singular", "Plural (consonant-ended)", "Plural (vowel-ended)", nullptr };
static const char * const catalanForms[] = { "Singular", "Undecal (11)", "Plural", nullptr };

#define EOL QLocale::C

static const QLocale::Language japaneseStyleLanguages[] = {
    QLocale::Afan,
    QLocale::Armenian,
    QLocale::Bhutani,
    QLocale::Bislama,
    QLocale::Burmese,
    QLocale::Chinese,
    QLocale::Fijian,
    QLocale::Guarani,
    QLocale::Hungarian,
    QLocale::Indonesian,
    QLocale::Japanese,
    QLocale::Javanese,
    QLocale::Korean,
    QLocale::Malay,
    QLocale::NauruLanguage,
    QLocale::Persian,
    QLocale::Sundanese,
    QLocale::Thai,
    QLocale::Tibetan,
    QLocale::Turkish,
    QLocale::Vietnamese,
    QLocale::Yoruba,
    QLocale::Zhuang,
    EOL
};

static const QLocale::Language englishStyleLanguages[] = {
    QLocale::Abkhazian,
    QLocale::Afar,
    QLocale::Afrikaans,
    QLocale::Albanian,
    QLocale::Amharic,
    QLocale::Assamese,
    QLocale::Aymara,
    QLocale::Azerbaijani,
    QLocale::Bashkir,
    QLocale::Basque,
    QLocale::Bengali,
    QLocale::Bihari,
    // Missing: Bokmal,
    QLocale::Bulgarian,
    QLocale::Cambodian,
    QLocale::Cornish,
    QLocale::Corsican,
    QLocale::Danish,
    QLocale::Dutch,
    QLocale::English,
    QLocale::Esperanto,
    QLocale::Estonian,
    QLocale::Faroese,
    QLocale::Finnish,
    // Missing: Friulian,
    QLocale::Frisian,
    QLocale::Galician,
    QLocale::Georgian,
    QLocale::German,
    QLocale::Greek,
    QLocale::Greenlandic,
    QLocale::Gujarati,
    QLocale::Hausa,
    QLocale::Hebrew,
    QLocale::Hindi,
    QLocale::Interlingua,
    QLocale::Interlingue,
    QLocale::Italian,
    QLocale::Kannada,
    QLocale::Kashmiri,
    QLocale::Kazakh,
    QLocale::Kinyarwanda,
    QLocale::Kirghiz,
    QLocale::Kurdish,
    QLocale::Kurundi,
    QLocale::Lao,
    QLocale::Latin,
    // Missing: Letzeburgesch,
    QLocale::Lingala,
    QLocale::Malagasy,
    QLocale::Malayalam,
    QLocale::Marathi,
    QLocale::Mongolian,
    // Missing: Nahuatl,
    QLocale::Nepali,
    // Missing: Northern Sotho,
    QLocale::Norwegian,
    QLocale::NorwegianNynorsk,
    QLocale::Occitan,
    QLocale::Oriya,
    QLocale::Pashto,
    QLocale::Portuguese,
    QLocale::Punjabi,
    QLocale::Quechua,
    QLocale::RhaetoRomance,
    QLocale::SouthernSotho,
    QLocale::Tswana,
    QLocale::Shona,
    QLocale::Sindhi,
    QLocale::Sinhala,
    QLocale::Swati,
    QLocale::Somali,
    QLocale::Spanish,
    QLocale::Swahili,
    QLocale::Swedish,
    QLocale::Tajik,
    QLocale::Tamil,
    QLocale::Tatar,
    QLocale::Telugu,
    QLocale::Tongan,
    QLocale::Tsonga,
    QLocale::Turkmen,
    QLocale::Twi,
    QLocale::Uigur,
    QLocale::Urdu,
    QLocale::Uzbek,
    QLocale::Volapuk,
    QLocale::Wolof,
    QLocale::Xhosa,
    QLocale::Yiddish,
    QLocale::Zulu,
    EOL
};
static const QLocale::Language frenchStyleLanguages[] = {
    // keep synchronized with frenchStyleCountries
    QLocale::Breton,
    QLocale::French,
    QLocale::Portuguese,
    // Missing: Filipino,
    QLocale::Tigrinya,
    // Missing: Walloon
    EOL
};
static const QLocale::Language latvianLanguage[] = { QLocale::Latvian, EOL };
static const QLocale::Language icelandicLanguage[] = { QLocale::Icelandic, EOL };
static const QLocale::Language irishStyleLanguages[] = {
    QLocale::Divehi,
    QLocale::Gaelic,
    QLocale::Inuktitut,
    QLocale::Inupiak,
    QLocale::Irish,
    QLocale::Manx,
    QLocale::Maori,
    // Missing: Sami,
    QLocale::Samoan,
    QLocale::Sanskrit,
    EOL
};
static const QLocale::Language slovakStyleLanguages[] = { QLocale::Slovak, QLocale::Czech, EOL };
static const QLocale::Language macedonianLanguage[] = { QLocale::Macedonian, EOL };
static const QLocale::Language lithuanianLanguage[] = { QLocale::Lithuanian, EOL };
static const QLocale::Language russianStyleLanguages[] = {
    QLocale::Bosnian,
    QLocale::Byelorussian,
    QLocale::Croatian,
    QLocale::Russian,
    QLocale::Serbian,
    QLocale::SerboCroatian,
    QLocale::Ukrainian,
    EOL
};
static const QLocale::Language polishLanguage[] = { QLocale::Polish, EOL };
static const QLocale::Language romanianLanguages[] = {
    QLocale::Moldavian,
    QLocale::Romanian,
    EOL
};
static const QLocale::Language slovenianLanguage[] = { QLocale::Slovenian, EOL };
static const QLocale::Language malteseLanguage[] = { QLocale::Maltese, EOL };
static const QLocale::Language welshLanguage[] = { QLocale::Welsh, EOL };
static const QLocale::Language arabicLanguage[] = { QLocale::Arabic, EOL };
static const QLocale::Language tagalogLanguage[] = { QLocale::Tagalog, EOL };
static const QLocale::Language catalanLanguage[] = { QLocale::Catalan, EOL };

static const QLocale::Country frenchStyleCountries[] = {
    // keep synchronized with frenchStyleLanguages
    QLocale::AnyCountry,
    QLocale::AnyCountry,
    QLocale::Brazil,
    QLocale::AnyCountry
};
struct NumerusTableEntry {
    const uchar *rules;
    int rulesSize;
    const char * const *forms;
    const QLocale::Language *languages;
    const QLocale::Country *countries;
    const char * const gettextRules;
};

static const NumerusTableEntry numerusTable[] = {
    { nullptr, 0, japaneseStyleForms, japaneseStyleLanguages, nullptr, "nplurals=1; plural=0;" },
    { englishStyleRules, sizeof(englishStyleRules), englishStyleForms, englishStyleLanguages, nullptr,
      "nplurals=2; plural=(n != 1);" },
    { frenchStyleRules, sizeof(frenchStyleRules), frenchStyleForms, frenchStyleLanguages,
      frenchStyleCountries, "nplurals=2; plural=(n > 1);" },
    { latvianRules, sizeof(latvianRules), latvianForms, latvianLanguage, nullptr,
      "nplurals=3; plural=(n%10==1 && n%100!=11 ? 0 : n != 0 ? 1 : 2);" },
    { icelandicRules, sizeof(icelandicRules), icelandicForms, icelandicLanguage, nullptr,
      "nplurals=2; plural=(n%10==1 && n%100!=11 ? 0 : 1);" },
    { irishStyleRules, sizeof(irishStyleRules), irishStyleForms, irishStyleLanguages, nullptr,
      "nplurals=3; plural=(n==1 ? 0 : n==2 ? 1 : 2);" },
    { slovakStyleRules, sizeof(slovakStyleRules), slovakStyleForms, slovakStyleLanguages, nullptr,
      "nplurals=3; plural=((n==1) ? 0 : (n>=2 && n<=4) ? 1 : 2);" },
    { macedonianRules, sizeof(macedonianRules), macedonianForms, macedonianLanguage, nullptr,
      "nplurals=3; plural=(n%100==1 ? 0 : n%100==2 ? 1 : 2);" },
    { lithuanianRules, sizeof(lithuanianRules), lithuanianForms, lithuanianLanguage, nullptr,
      "nplurals=3; plural=(n%10==1 && n%100!=11 ? 0 : n%10>=2 && (n%100<10 || n%100>=20) ? 1 : 2);" },
    { russianStyleRules, sizeof(russianStyleRules), russianStyleForms, russianStyleLanguages, nullptr,
      "nplurals=3; plural=(n%10==1 && n%100!=11 ? 0 : n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2);" },
    { polishRules, sizeof(polishRules), polishForms, polishLanguage, nullptr,
      "nplurals=3; plural=(n==1 ? 0 : n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2);" },
    { romanianRules, sizeof(romanianRules), romanianForms, romanianLanguages, nullptr,
      "nplurals=3; plural=(n==1 ? 0 : (n==0 || (n%100 > 0 && n%100 < 20)) ? 1 : 2);" },
    { slovenianRules, sizeof(slovenianRules), slovenianForms, slovenianLanguage, nullptr,
      "nplurals=4; plural=(n%100==1 ? 0 : n%100==2 ? 1 : n%100==3 || n%100==4 ? 2 : 3);" },
    { malteseRules, sizeof(malteseRules), malteseForms, malteseLanguage, nullptr,
      "nplurals=4; plural=(n==1 ? 0 : (n==0 || (n%100>=1 && n%100<=10)) ? 1 : (n%100>=11 && n%100<=19) ? 2 : 3);" },
    { welshRules, sizeof(welshRules), welshForms, welshLanguage, nullptr,
      "nplurals=5; plural=(n==0 ? 0 : n==1 ? 1 : (n>=2 && n<=5) ? 2 : n==6 ? 3 : 4);" },
    { arabicRules, sizeof(arabicRules), arabicForms, arabicLanguage, nullptr,
      "nplurals=6; plural=(n==0 ? 0 : n==1 ? 1 : n==2 ? 2 : (n%100>=3 && n%100<=10) ? 3 : n%100>=11 ? 4 : 5);" },
    { tagalogRules, sizeof(tagalogRules), tagalogForms, tagalogLanguage, nullptr,
      "nplurals=3; plural=(n==1 ? 0 : (n%10==4 || n%10==6 || n%10== 9) ? 1 : 2);" },
    { catalanRules, sizeof(catalanRules), catalanForms, catalanLanguage, nullptr,
      "nplurals=3; plural=(n==1 ? 0 : (n==11 || n/1000==11 || n/1000000==11 || n/1000000000==11) ? 1 : 2);" },
};

static const int NumerusTableSize = sizeof(numerusTable) / sizeof(numerusTable[0]);

bool getNumerusInfo(QLocale::Language language, QLocale::Country country,
                    QByteArray *rules, QStringList *forms, const char **gettextRules)
{
    while (true) {
        for (const auto & entry : numerusTable) {
            for (int j = 0; entry.languages[j] != EOL; ++j) {
                if (entry.languages[j] == language
                        && ((!entry.countries && country == QLocale::AnyCountry)
                            || (entry.countries && entry.countries[j] == country))) {
                    if (rules) {
                        *rules = QByteArray::fromRawData(reinterpret_cast<const char *>(entry.rules),
                                                    entry.rulesSize);
                    }
                    if (gettextRules)
                        *gettextRules = entry.gettextRules;
                    if (forms) {
                        forms->clear();
                        for (int k = 0; entry.forms[k]; ++k)
                            forms->append(QLatin1String(entry.forms[k]));
                    }
                    return true;
                }
            }
        }

        if (country == QLocale::AnyCountry)
            break;
        country = QLocale::AnyCountry;
    }
    return false;
}

QString getNumerusInfoString()
{
    QStringList langs;

    for (const auto & entry : numerusTable) {
        for (int j = 0; entry.languages[j] != EOL; ++j) {
            QLocale loc(entry.languages[j], entry.countries ? entry.countries[j] : QLocale::AnyCountry);
            QString lang = QLocale::languageToString(entry.languages[j]);
            if (loc.language() == QLocale::C)
                lang += QLatin1String(" (!!!)");
            else if (entry.countries && entry.countries[j] != QLocale::AnyCountry)
                lang += QLatin1String(" (") + QLocale::countryToString(loc.country()) + QLatin1Char(')');
            else
                lang += QLatin1String(" [") + QLocale::countryToString(loc.country()) + QLatin1Char(']');
            langs << QString::fromLatin1("%1 %2 %3\n").arg(lang, -40).arg(loc.name(), -8)
                                .arg(QString::fromLatin1(entry.gettextRules));
        }
    }
    langs.sort();
    return langs.join(QString());
}

QT_END_NAMESPACE

/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qunicodetools_p.h"

#include "qunicodetables_p.h"
#include "qvarlengtharray.h"

#include <harfbuzz-shaper.h>

#define FLAG(x) (1 << (x))

QT_BEGIN_NAMESPACE

Q_AUTOTEST_EXPORT int qt_initcharattributes_default_algorithm_only = 0;

namespace QUnicodeTools {

// -----------------------------------------------------------------------------------------------------
//
// The text boundaries determination algorithm. See http://www.unicode.org/reports/tr29/tr29-19.html
//
// -----------------------------------------------------------------------------------------------------
namespace GB {

static const uchar breakTable[QUnicodeTables::GraphemeBreak_LVT + 1][QUnicodeTables::GraphemeBreak_LVT + 1] = {
//    Other   CR     LF  Control Extend Prepend S-Mark   L     V      T      LV    LVT
    { true , true , true , true , false, true , false, true , true , true , true , true  }, // Other
    { true , true , false, true , true , true , true , true , true , true , true , true  }, // CR
    { true , true , true , true , true , true , true , true , true , true , true , true  }, // LF
    { true , true , true , true , true , true , true , true , true , true , true , true  }, // Control
    { true , true , true , true , false, true , false, true , true , true , true , true  }, // Extend
    { false, true , true , true , false, false, false, false, false, false, false, false }, // Prepend
    { true , true , true , true , false, true , false, true , true , true , true , true  }, // SpacingMark
    { true , true , true , true , false, true , false, false, false, true , false, false }, // L
    { true , true , true , true , false, true , false, true , false, false, true , true  }, // V
    { true , true , true , true , false, true , false, true , true , false, true , true  }, // T
    { true , true , true , true , false, true , false, true , false, false, true , true  }, // LV
    { true , true , true , true , false, true , false, true , true , false, true , true  }, // LVT
};

} // namespace GB


static void getGraphemeBreaks(const ushort *string, quint32 len, QCharAttributes *attributes)
{
    QUnicodeTables::GraphemeBreakClass lcls = QUnicodeTables::GraphemeBreak_LF; // to meet GB1
    for (quint32 i = 0; i != len; ++i) {
        quint32 pos = i;
        uint ucs4 = string[i];
        if (QChar::isHighSurrogate(ucs4) && i + 1 != len) {
            ushort low = string[i + 1];
            if (QChar::isLowSurrogate(low)) {
                ucs4 = QChar::surrogateToUcs4(ucs4, low);
                ++i;
            }
        }

        const QUnicodeTables::Properties *prop = QUnicodeTables::properties(ucs4);
        QUnicodeTables::GraphemeBreakClass cls = (QUnicodeTables::GraphemeBreakClass) prop->graphemeBreakClass;

        if (Q_LIKELY(GB::breakTable[lcls][cls]))
            attributes[pos].graphemeBoundary = true;

        lcls = cls;
    }
}


namespace WB {

enum Action {
    NoBreak = 0,
    Break = 1,
    Lookup = 2
};

static const uchar breakTable[QUnicodeTables::WordBreak_ExtendNumLet + 1][QUnicodeTables::WordBreak_ExtendNumLet + 1] = {
//    Other      CR       LF    Newline   Extend  Katakana ALetter MidNumLet MidLetter MidNum  Numeric  ExtendNumLet
    { Break  , Break  , Break  , Break  , NoBreak, Break  , Break  , Break  , Break  , Break  , Break  , Break   }, // Other
    { Break  , Break  , NoBreak, Break  , Break  , Break  , Break  , Break  , Break  , Break  , Break  , Break   }, // CR
    { Break  , Break  , Break  , Break  , Break  , Break  , Break  , Break  , Break  , Break  , Break  , Break   }, // LF
    { Break  , Break  , Break  , Break  , Break  , Break  , Break  , Break  , Break  , Break  , Break  , Break   }, // Newline
    { Break  , Break  , Break  , Break  , NoBreak, Break  , Break  , Break  , Break  , Break  , Break  , Break   }, // Extend
    { Break  , Break  , Break  , Break  , NoBreak, NoBreak, Break  , Break  , Break  , Break  , Break  , NoBreak }, // Katakana
    { Break  , Break  , Break  , Break  , NoBreak, Break  , NoBreak, Lookup , Lookup , Break  , NoBreak, NoBreak }, // ALetter
    { Break  , Break  , Break  , Break  , NoBreak, Break  , Break  , Break  , Break  , Break  , Break  , Break   }, // MidNumLet
    { Break  , Break  , Break  , Break  , NoBreak, Break  , Break  , Break  , Break  , Break  , Break  , Break   }, // MidLetter
    { Break  , Break  , Break  , Break  , NoBreak, Break  , Break  , Break  , Break  , Break  , Break  , Break   }, // MidNum
    { Break  , Break  , Break  , Break  , NoBreak, Break  , NoBreak, Lookup , Break  , Lookup , NoBreak, NoBreak }, // Numeric
    { Break  , Break  , Break  , Break  , NoBreak, NoBreak, NoBreak, Break  , Break  , Break  , NoBreak, NoBreak }, // ExtendNumLet
};

} // namespace WB

static void getWordBreaks(const ushort *string, quint32 len, QCharAttributes *attributes)
{
    QUnicodeTables::WordBreakClass cls = QUnicodeTables::WordBreak_LF; // to meet WB1
    for (quint32 i = 0; i != len; ++i) {
        quint32 pos = i;
        uint ucs4 = string[i];
        if (QChar::isHighSurrogate(ucs4) && i + 1 != len) {
            ushort low = string[i + 1];
            if (QChar::isLowSurrogate(low)) {
                ucs4 = QChar::surrogateToUcs4(ucs4, low);
                ++i;
            }
        }

        const QUnicodeTables::Properties *prop = QUnicodeTables::properties(ucs4);
        QUnicodeTables::WordBreakClass ncls = (QUnicodeTables::WordBreakClass) prop->wordBreakClass;
        uchar action = WB::breakTable[cls][ncls];
        if (Q_UNLIKELY(action == WB::Lookup)) {
            action = WB::Break;
            for (quint32 lookahead = i + 1; lookahead < len; ++lookahead) {
                ucs4 = string[lookahead];
                if (QChar::isHighSurrogate(ucs4) && lookahead + 1 != len) {
                    ushort low = string[lookahead + 1];
                    if (QChar::isLowSurrogate(low)) {
                        ucs4 = QChar::surrogateToUcs4(ucs4, low);
                        ++lookahead;
                    }
                }

                prop = QUnicodeTables::properties(ucs4);
                QUnicodeTables::WordBreakClass tcls = (QUnicodeTables::WordBreakClass) prop->wordBreakClass;
                if (Q_UNLIKELY(tcls == QUnicodeTables::WordBreak_Extend))
                    continue;
                if (Q_LIKELY(tcls == cls)) {
                    i = lookahead;
                    ncls = tcls;
                    action = WB::NoBreak;
                }
                break;
            }
        } else if (Q_UNLIKELY(ncls == QUnicodeTables::WordBreak_Extend)) {
            // WB4: X(Extend|Format)* -> X
            if (Q_LIKELY(action != WB::Break))
                continue;
        }
        cls = ncls;
        if (action == WB::Break)
            attributes[pos].wordBreak = true;
    }
}

namespace SB {

enum State {
    Initial,
    Upper,
    UpATerm,
    ATerm,
    ATermC,
    ACS,
    STerm,
    STermC,
    SCS,
    BAfterC,
    BAfter,
    Break,
    Lookup
};

static const uchar breakTable[BAfter + 1][QUnicodeTables::SentenceBreak_Close + 1] = {
//     Other     CR       LF      Sep     Extend     Sp      Lower   Upper    OLetter  Numeric  ATerm   SContinue STerm     Close
    { Initial, BAfterC, BAfter , BAfter , Initial, Initial, Initial, Upper  , Initial, Initial, ATerm  , Initial, STerm  , Initial }, // Initial
    { Initial, BAfterC, BAfter , BAfter , Upper  , Initial, Initial, Upper  , Initial, Initial, UpATerm, STerm  , STerm  , Initial }, // Upper

    { Lookup , BAfterC, BAfter , BAfter , UpATerm, ACS    , Initial, Upper  , Break  , Initial, ATerm  , STerm  , STerm  , ATermC  }, // UpATerm
    { Lookup , BAfterC, BAfter , BAfter , ATerm  , ACS    , Initial, Break  , Break  , Initial, ATerm  , STerm  , STerm  , ATermC  }, // ATerm
    { Lookup , BAfterC, BAfter , BAfter , ATermC , ACS    , Initial, Break  , Break  , Lookup , ATerm  , STerm  , STerm  , ATermC  }, // ATermC
    { Lookup , BAfterC, BAfter , BAfter , ACS    , ACS    , Initial, Break  , Break  , Lookup , ATerm  , STerm  , STerm  , Lookup  }, // ACS

    { Break  , BAfterC, BAfter , BAfter , STerm  , SCS    , Break  , Break  , Break  , Break  , ATerm  , STerm  , STerm  , STermC  }, // STerm,
    { Break  , BAfterC, BAfter , BAfter , STermC , SCS    , Break  , Break  , Break  , Break  , ATerm  , STerm  , STerm  , STermC  }, // STermC
    { Break  , BAfterC, BAfter , BAfter , SCS    , SCS    , Break  , Break  , Break  , Break  , ATerm  , STerm  , STerm  , Break   }, // SCS
    { Break  , Break  , BAfter , Break  , Break  , Break  , Break  , Break  , Break  , Break  , Break  , Break  , Break  , Break   }, // BAfterC
    { Break  , Break  , Break  , Break  , Break  , Break  , Break  , Break  , Break  , Break  , Break  , Break  , Break  , Break   }, // BAfter
};

} // namespace SB

static void getSentenceBreaks(const ushort *string, quint32 len, QCharAttributes *attributes)
{
    uchar state = SB::BAfter; // to meet SB1
    for (quint32 i = 0; i != len; ++i) {
        quint32 pos = i;
        uint ucs4 = string[i];
        if (QChar::isHighSurrogate(ucs4) && i + 1 != len) {
            ushort low = string[i + 1];
            if (QChar::isLowSurrogate(low)) {
                ucs4 = QChar::surrogateToUcs4(ucs4, low);
                ++i;
            }
        }

        const QUnicodeTables::Properties *prop = QUnicodeTables::properties(ucs4);
        QUnicodeTables::SentenceBreakClass ncls = (QUnicodeTables::SentenceBreakClass) prop->sentenceBreakClass;

        Q_ASSERT(state <= SB::BAfter);
        state = SB::breakTable[state][ncls];
        if (Q_UNLIKELY(state == SB::Lookup)) { // SB8
            state = SB::Break;
            for (quint32 lookahead = i + 1; lookahead < len; ++lookahead) {
                ucs4 = string[lookahead];
                if (QChar::isHighSurrogate(ucs4) && lookahead + 1 != len) {
                    ushort low = string[lookahead + 1];
                    if (QChar::isLowSurrogate(low)) {
                        ucs4 = QChar::surrogateToUcs4(ucs4, low);
                        ++lookahead;
                    }
                }

                prop = QUnicodeTables::properties(ucs4);
                QUnicodeTables::SentenceBreakClass tcls = (QUnicodeTables::SentenceBreakClass) prop->sentenceBreakClass;
                switch (tcls) {
                case QUnicodeTables::SentenceBreak_Other:
                case QUnicodeTables::SentenceBreak_Extend:
                case QUnicodeTables::SentenceBreak_Sp:
                case QUnicodeTables::SentenceBreak_Numeric:
                case QUnicodeTables::SentenceBreak_SContinue:
                case QUnicodeTables::SentenceBreak_Close:
                    continue;
                case QUnicodeTables::SentenceBreak_Lower:
                    i = lookahead;
                    state = SB::Initial;
                    break;
                default:
                    break;
                }
                break;
            }
        }
        if (Q_UNLIKELY(state == SB::Break)) {
            attributes[pos].sentenceBoundary = true;
            state = SB::breakTable[SB::Initial][ncls];
        }
    }
}

// -----------------------------------------------------------------------------------------------------
//
// The line breaking algorithm. See http://www.unicode.org/reports/tr14/tr14-28.html
//
// -----------------------------------------------------------------------------------------------------

namespace LB {

namespace NS { // Number Sequence

// LB25 recommends to not break lines inside numbers of the form
// described by the following regular expression:
//  (PR|PO)?(OP|HY)?NU(NU|SY|IS)*(CL|CP)?(PR|PO)?

enum Action {
    None,
    Start,
    Continue,
    Break
};

enum Class {
    XX,
    PRPO,
    OPHY,
    NU,
    SYIS,
    CLCP
};

static const uchar actionTable[CLCP + 1][CLCP + 1] = {
//     XX       PRPO      OPHY       NU       SYIS      CLCP
    { None    , Start   , Start   , Start   , None    , None     }, // XX
    { None    , Start   , Continue, Continue, None    , None     }, // PRPO
    { None    , Start   , Start   , Continue, None    , None     }, // OPHY
    { Break   , Break   , Break   , Continue, Continue, Continue }, // NU
    { Break   , Break   , Break   , Continue, Continue, Continue }, // SYIS
    { Break   , Continue, Break   , Break   , Break   , Break    }, // CLCP
};

inline Class toClass(QUnicodeTables::LineBreakClass lbc, QChar::Category category)
{
    switch (lbc) {
    case QUnicodeTables::LineBreak_AL:// case QUnicodeTables::LineBreak_AI:
        // resolve AI math symbols in numerical context to IS
        if (category == QChar::Symbol_Math)
            return SYIS;
        break;
    case QUnicodeTables::LineBreak_PR: case QUnicodeTables::LineBreak_PO:
        return PRPO;
    case QUnicodeTables::LineBreak_OP: case QUnicodeTables::LineBreak_HY:
        return OPHY;
    case QUnicodeTables::LineBreak_NU:
        return NU;
    case QUnicodeTables::LineBreak_SY: case QUnicodeTables::LineBreak_IS:
        return SYIS;
    case QUnicodeTables::LineBreak_CL: case QUnicodeTables::LineBreak_CP:
        return CLCP;
    default:
        break;
    }
    return XX;
}

} // namespace NS

/* In order to support the tailored implementation of LB25 properly
   the following changes were made in the pair table to allow breaks
   where the numeric expression doesn't match the template (i.e. [^NU](IS|SY)NU):
   CL->PO from IB to DB
   CP->PO from IB to DB
   CL->PR from IB to DB
   CP->PR from IB to DB
   PO->OP from IB to DB
   PR->OP from IB to DB
   IS->NU from IB to DB
   SY->NU from IB to DB
*/

// The following line break classes are not treated by the pair table
// and must be resolved outside:
//  AI, BK, CB, CJ, CR, LF, NL, SA, SG, SP, XX

enum Action {
    ProhibitedBreak, PB = ProhibitedBreak,
    DirectBreak, DB = DirectBreak,
    IndirectBreak, IB = IndirectBreak,
    CombiningIndirectBreak, CI = CombiningIndirectBreak,
    CombiningProhibitedBreak, CP = CombiningProhibitedBreak
};

static const uchar breakTable[QUnicodeTables::LineBreak_CB + 1][QUnicodeTables::LineBreak_CB + 1] = {
/*         OP  CL  CP  QU  GL  NS  EX  SY  IS  PR  PO  NU  AL  HL  ID  IN  HY  BA  BB  B2  ZW  CM  WJ  H2  H3  JL  JV  JT  CB */
/* OP */ { PB, PB, PB, PB, PB, PB, PB, PB, PB, PB, PB, PB, PB, PB, PB, PB, PB, PB, PB, PB, PB, CP, PB, PB, PB, PB, PB, PB, PB },
/* CL */ { DB, PB, PB, IB, IB, PB, PB, PB, PB, DB, DB, DB, DB, DB, DB, DB, IB, IB, DB, DB, PB, CI, PB, DB, DB, DB, DB, DB, DB },
/* CP */ { DB, PB, PB, IB, IB, PB, PB, PB, PB, DB, DB, IB, IB, IB, DB, DB, IB, IB, DB, DB, PB, CI, PB, DB, DB, DB, DB, DB, DB },
/* QU */ { PB, PB, PB, IB, IB, IB, PB, PB, PB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, PB, CI, PB, IB, IB, IB, IB, IB, IB },
/* GL */ { IB, PB, PB, IB, IB, IB, PB, PB, PB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, PB, CI, PB, IB, IB, IB, IB, IB, IB },
/* NS */ { DB, PB, PB, IB, IB, IB, PB, PB, PB, DB, DB, DB, DB, DB, DB, DB, IB, IB, DB, DB, PB, CI, PB, DB, DB, DB, DB, DB, DB },
/* EX */ { DB, PB, PB, IB, IB, IB, PB, PB, PB, DB, DB, DB, DB, DB, DB, DB, IB, IB, DB, DB, PB, CI, PB, DB, DB, DB, DB, DB, DB },
/* SY */ { DB, PB, PB, IB, IB, IB, PB, PB, PB, DB, DB, DB, DB, DB, DB, DB, IB, IB, DB, DB, PB, CI, PB, DB, DB, DB, DB, DB, DB },
/* IS */ { DB, PB, PB, IB, IB, IB, PB, PB, PB, DB, DB, DB, IB, IB, DB, DB, IB, IB, DB, DB, PB, CI, PB, DB, DB, DB, DB, DB, DB },
/* PR */ { DB, PB, PB, IB, IB, IB, PB, PB, PB, DB, DB, IB, IB, IB, IB, DB, IB, IB, DB, DB, PB, CI, PB, IB, IB, IB, IB, IB, DB },
/* PO */ { DB, PB, PB, IB, IB, IB, PB, PB, PB, DB, DB, IB, IB, IB, DB, DB, IB, IB, DB, DB, PB, CI, PB, DB, DB, DB, DB, DB, DB },
/* NU */ { IB, PB, PB, IB, IB, IB, PB, PB, PB, IB, IB, IB, IB, IB, DB, IB, IB, IB, DB, DB, PB, CI, PB, DB, DB, DB, DB, DB, DB },
/* AL */ { IB, PB, PB, IB, IB, IB, PB, PB, PB, DB, DB, IB, IB, IB, DB, IB, IB, IB, DB, DB, PB, CI, PB, DB, DB, DB, DB, DB, DB },
/* HL */ { IB, PB, PB, IB, IB, IB, PB, PB, PB, DB, DB, IB, IB, IB, DB, IB, IB, IB, DB, DB, PB, CI, PB, DB, DB, DB, DB, DB, DB },
/* ID */ { DB, PB, PB, IB, IB, IB, PB, PB, PB, DB, IB, DB, DB, DB, DB, IB, IB, IB, DB, DB, PB, CI, PB, DB, DB, DB, DB, DB, DB },
/* IN */ { DB, PB, PB, IB, IB, IB, PB, PB, PB, DB, DB, DB, DB, DB, DB, IB, IB, IB, DB, DB, PB, CI, PB, DB, DB, DB, DB, DB, DB },
/* HY */ { DB, PB, PB, IB, DB, IB, PB, PB, PB, DB, DB, IB, DB, DB, DB, DB, IB, IB, DB, DB, PB, CI, PB, DB, DB, DB, DB, DB, DB },
/* BA */ { DB, PB, PB, IB, DB, IB, PB, PB, PB, DB, DB, DB, DB, DB, DB, DB, IB, IB, DB, DB, PB, CI, PB, DB, DB, DB, DB, DB, DB },
/* BB */ { IB, PB, PB, IB, IB, IB, PB, PB, PB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, PB, CI, PB, IB, IB, IB, IB, IB, DB },
/* B2 */ { DB, PB, PB, IB, IB, IB, PB, PB, PB, DB, DB, DB, DB, DB, DB, DB, IB, IB, DB, PB, PB, CI, PB, DB, DB, DB, DB, DB, DB },
/* ZW */ { DB, DB, DB, DB, DB, DB, DB, DB, DB, DB, DB, DB, DB, DB, DB, DB, DB, DB, DB, DB, PB, DB, DB, DB, DB, DB, DB, DB, DB },
/* CM */ { IB, PB, PB, IB, IB, IB, PB, PB, PB, DB, DB, IB, IB, IB, DB, IB, IB, IB, DB, DB, PB, CI, PB, DB, DB, DB, DB, DB, DB },
/* WJ */ { IB, PB, PB, IB, IB, IB, PB, PB, PB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, PB, CI, PB, IB, IB, IB, IB, IB, IB },
/* H2 */ { DB, PB, PB, IB, IB, IB, PB, PB, PB, DB, IB, DB, DB, DB, DB, IB, IB, IB, DB, DB, PB, CI, PB, DB, DB, DB, IB, IB, DB },
/* H3 */ { DB, PB, PB, IB, IB, IB, PB, PB, PB, DB, IB, DB, DB, DB, DB, IB, IB, IB, DB, DB, PB, CI, PB, DB, DB, DB, DB, IB, DB },
/* JL */ { DB, PB, PB, IB, IB, IB, PB, PB, PB, DB, IB, DB, DB, DB, DB, IB, IB, IB, DB, DB, PB, CI, PB, IB, IB, IB, IB, DB, DB },
/* JV */ { DB, PB, PB, IB, IB, IB, PB, PB, PB, DB, IB, DB, DB, DB, DB, IB, IB, IB, DB, DB, PB, CI, PB, DB, DB, DB, IB, IB, DB },
/* JT */ { DB, PB, PB, IB, IB, IB, PB, PB, PB, DB, IB, DB, DB, DB, DB, IB, IB, IB, DB, DB, PB, CI, PB, DB, DB, DB, DB, IB, DB },
/* CB */ { DB, PB, PB, IB, IB, DB, PB, PB, PB, DB, DB, DB, DB, DB, DB, DB, DB, DB, DB, DB, PB, CI, PB, DB, DB, DB, DB, DB, DB }
};

} // namespace LB

static void getLineBreaks(const ushort *string, quint32 len, QCharAttributes *attributes)
{
    quint32 nestart = 0;
    LB::NS::Class nelast = LB::NS::XX;

    QUnicodeTables::LineBreakClass lcls = QUnicodeTables::LineBreak_LF; // to meet LB10
    QUnicodeTables::LineBreakClass cls = lcls;
    for (quint32 i = 0; i != len; ++i) {
        quint32 pos = i;
        uint ucs4 = string[i];
        if (QChar::isHighSurrogate(ucs4) && i + 1 != len) {
            ushort low = string[i + 1];
            if (QChar::isLowSurrogate(low)) {
                ucs4 = QChar::surrogateToUcs4(ucs4, low);
                ++i;
            }
        }

        const QUnicodeTables::Properties *prop = QUnicodeTables::properties(ucs4);
        QUnicodeTables::LineBreakClass ncls = (QUnicodeTables::LineBreakClass) prop->lineBreakClass;

        if (Q_UNLIKELY(ncls == QUnicodeTables::LineBreak_SA)) {
            // LB1: resolve SA to AL, except of those that have Category Mn or Mc be resolved to CM
            static const int test = FLAG(QChar::Mark_NonSpacing) | FLAG(QChar::Mark_SpacingCombining);
            if (FLAG(prop->category) & test)
                ncls = QUnicodeTables::LineBreak_CM;
        }
        if (Q_UNLIKELY(ncls == QUnicodeTables::LineBreak_CM)) {
            // LB10: treat CM that follows SP, BK, CR, LF, NL, or ZW as AL
            if (lcls == QUnicodeTables::LineBreak_ZW || lcls >= QUnicodeTables::LineBreak_SP)
                ncls = QUnicodeTables::LineBreak_AL;
        }

        if (Q_LIKELY(ncls != QUnicodeTables::LineBreak_CM)) {
            // LB25: do not break lines inside numbers
            LB::NS::Class necur = LB::NS::toClass(ncls, (QChar::Category)prop->category);
            switch (LB::NS::actionTable[nelast][necur]) {
            case LB::NS::Break:
                // do not change breaks before and after the expression
                for (quint32 j = nestart + 1; j < pos; ++j)
                    attributes[j].lineBreak = false;
                // fall through
            case LB::NS::None:
                nelast = LB::NS::XX; // reset state
                break;
            case LB::NS::Start:
                nestart = i;
                // fall through
            default:
                nelast = necur;
                break;
            }
        }

        if (Q_UNLIKELY(lcls >= QUnicodeTables::LineBreak_CR)) {
            // LB4: BK!, LB5: (CRxLF|CR|LF|NL)!
            if (lcls > QUnicodeTables::LineBreak_CR || ncls != QUnicodeTables::LineBreak_LF)
                attributes[pos].lineBreak = true;
            goto next;
        }

        if (Q_UNLIKELY(ncls >= QUnicodeTables::LineBreak_SP)) {
            if (ncls > QUnicodeTables::LineBreak_SP)
                goto next; // LB6: x(BK|CR|LF|NL)
            goto next_no_cls_update; // LB7: xSP
        }

        // for South East Asian chars that require a complex analysis, the Unicode
        // standard recommends to treat them as AL. tailoring that do dictionary analysis can override
        if (Q_UNLIKELY(cls >= QUnicodeTables::LineBreak_SA))
            cls = QUnicodeTables::LineBreak_AL;

        switch (LB::breakTable[cls][ncls < QUnicodeTables::LineBreak_SA ? ncls : QUnicodeTables::LineBreak_AL]) {
        case LB::DirectBreak:
            attributes[pos].lineBreak = true;
            break;
        case LB::IndirectBreak:
            if (lcls == QUnicodeTables::LineBreak_SP)
                attributes[pos].lineBreak = true;
            break;
        case LB::CombiningIndirectBreak:
            if (lcls != QUnicodeTables::LineBreak_SP)
                goto next_no_cls_update;
            attributes[pos].lineBreak = true;
            break;
        case LB::CombiningProhibitedBreak:
            if (lcls != QUnicodeTables::LineBreak_SP)
                goto next_no_cls_update;
            break;
        case LB::ProhibitedBreak:
            // nothing to do
        default:
            break;
        }

    next:
        cls = ncls;
    next_no_cls_update:
        lcls = ncls;
    }

    if (Q_UNLIKELY(LB::NS::actionTable[nelast][LB::NS::XX] == LB::NS::Break)) {
        // LB25: do not break lines inside numbers
        for (quint32 j = nestart + 1; j < len; ++j)
            attributes[j].lineBreak = false;
    }

    attributes[0].lineBreak = false; // LB2
}


static void getWhiteSpaces(const ushort *string, quint32 len, QCharAttributes *attributes)
{
    for (quint32 i = 0; i != len; ++i) {
        uint ucs4 = string[i];
        if (QChar::isHighSurrogate(ucs4) && i + 1 != len) {
            ushort low = string[i + 1];
            if (QChar::isLowSurrogate(low)) {
                ucs4 = QChar::surrogateToUcs4(ucs4, low);
                ++i;
            }
        }

        if (Q_UNLIKELY(QChar::isSpace(ucs4)))
            attributes[i].whiteSpace = true;
    }
}


Q_CORE_EXPORT void initCharAttributes(const ushort *string, int length,
                                      const ScriptItem *items, int numItems,
                                      QCharAttributes *attributes, CharAttributeOptions options)
{
    if (length <= 0)
        return;

    if (!(options & DontClearAttributes))
        ::memset(attributes, 0, length * sizeof(QCharAttributes));

    if (options & GraphemeBreaks)
        getGraphemeBreaks(string, length, attributes);
    if (options & WordBreaks)
        getWordBreaks(string, length, attributes);
    if (options & SentenceBreaks)
        getSentenceBreaks(string, length, attributes);
    if (options & LineBreaks)
        getLineBreaks(string, length, attributes);
    if (options & WhiteSpaces)
        getWhiteSpaces(string, length, attributes);

    if (!items || numItems <= 0)
        return;
    if (!qt_initcharattributes_default_algorithm_only) {
        QVarLengthArray<HB_ScriptItem, 64> scriptItems;
        scriptItems.reserve(numItems);
        int start = 0;
        for (int i = start + 1; i < numItems; ++i) {
            if (items[i].script == items[start].script)
                continue;
            HB_ScriptItem item;
            item.pos = items[start].position;
            item.length = items[i].position - items[start].position;
            item.script = (HB_Script)items[start].script;
            item.bidiLevel = 0; // unused
            scriptItems.append(item);
            start = i;
        }
        if (items[start].position + 1 < length) {
            HB_ScriptItem item;
            item.pos = items[start].position;
            item.length = length - items[start].position;
            item.script = (HB_Script)items[start].script;
            item.bidiLevel = 0; // unused
            scriptItems.append(item);
        }
        static_assert(sizeof(QCharAttributes) == sizeof(HB_CharAttributes));
        HB_GetTailoredCharAttributes(string, length,
                                     scriptItems.constData(), scriptItems.size(),
                                     reinterpret_cast<HB_CharAttributes *>(attributes));
    }
}

} // namespace QUnicodeTools

QT_END_NAMESPACE

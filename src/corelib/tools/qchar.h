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

#ifndef QCHAR_H
#define QCHAR_H

#include <QtCore/qglobal.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QString;

struct QLatin1Char
{
public:
    constexpr inline explicit QLatin1Char(char c) : ch(c) {}
    constexpr inline char toLatin1() const { return ch; }
    constexpr inline ushort unicode() const { return ushort(uchar(ch)); }

private:
    char ch;
};


class Q_CORE_EXPORT QChar {
public:
    enum SpecialCharacter {
        Null = 0x0000,
        Nbsp = 0x00a0,
        SoftHyphen = 0x00ad,
        ReplacementCharacter = 0xfffd,
        ObjectReplacementCharacter = 0xfffc,
        ByteOrderMark = 0xfeff,
        ByteOrderSwapped = 0xfffe,
        ParagraphSeparator = 0x2029,
        LineSeparator = 0x2028,
        LastValidCodePoint = 0x10ffff
    };

    constexpr QChar() : ucs(0) {}
    constexpr QChar(ushort rc) : ucs(rc){} // implicit
    constexpr QChar(uchar c, uchar r) : ucs(ushort((r << 8) | c)){}
    constexpr QChar(short rc) : ucs(ushort(rc)){} // implicit
    constexpr QChar(uint rc) : ucs(ushort(rc & 0xffff)){}
    constexpr QChar(int rc) : ucs(ushort(rc & 0xffff)){}
    constexpr QChar(SpecialCharacter s) : ucs(ushort(s)) {} // implicit
    constexpr QChar(QLatin1Char ch) : ucs(ch.unicode()) {} // implicit

#ifndef QT_NO_CAST_FROM_ASCII
    QT_ASCII_CAST_WARN_CONSTRUCTOR constexpr explicit QChar(char c) : ucs(uchar(c)) { }
    QT_ASCII_CAST_WARN_CONSTRUCTOR constexpr explicit QChar(uchar c) : ucs(c) { }
#endif
    // Unicode information

    enum Category
    {
        Mark_NonSpacing,          //   Mn
        Mark_SpacingCombining,    //   Mc
        Mark_Enclosing,           //   Me

        Number_DecimalDigit,      //   Nd
        Number_Letter,            //   Nl
        Number_Other,             //   No

        Separator_Space,          //   Zs
        Separator_Line,           //   Zl
        Separator_Paragraph,      //   Zp

        Other_Control,            //   Cc
        Other_Format,             //   Cf
        Other_Surrogate,          //   Cs
        Other_PrivateUse,         //   Co
        Other_NotAssigned,        //   Cn

        Letter_Uppercase,         //   Lu
        Letter_Lowercase,         //   Ll
        Letter_Titlecase,         //   Lt
        Letter_Modifier,          //   Lm
        Letter_Other,             //   Lo

        Punctuation_Connector,    //   Pc
        Punctuation_Dash,         //   Pd
        Punctuation_Open,         //   Ps
        Punctuation_Close,        //   Pe
        Punctuation_InitialQuote, //   Pi
        Punctuation_FinalQuote,   //   Pf
        Punctuation_Other,        //   Po

        Symbol_Math,              //   Sm
        Symbol_Currency,          //   Sc
        Symbol_Modifier,          //   Sk
        Symbol_Other,             //   So

        Punctuation_Dask = Punctuation_Dash // ### Qt 5: remove
    };

    enum Direction
    {
        DirL, DirR, DirEN, DirES, DirET, DirAN, DirCS, DirB, DirS, DirWS, DirON,
        DirLRE, DirLRO, DirAL, DirRLE, DirRLO, DirPDF, DirNSM, DirBN
    };

    enum Decomposition
    {
        NoDecomposition,
        Canonical,
        Font,
        NoBreak,
        Initial,
        Medial,
        Final,
        Isolated,
        Circle,
        Super,
        Sub,
        Vertical,
        Wide,
        Narrow,
        Small,
        Square,
        Compat,
        Fraction
    };

    enum Joining
    {
        OtherJoining, Dual, Right, Center
    };

    enum CombiningClass
    {
        Combining_BelowLeftAttached       = 200,
        Combining_BelowAttached           = 202,
        Combining_BelowRightAttached      = 204,
        Combining_LeftAttached            = 208,
        Combining_RightAttached           = 210,
        Combining_AboveLeftAttached       = 212,
        Combining_AboveAttached           = 214,
        Combining_AboveRightAttached      = 216,

        Combining_BelowLeft               = 218,
        Combining_Below                   = 220,
        Combining_BelowRight              = 222,
        Combining_Left                    = 224,
        Combining_Right                   = 226,
        Combining_AboveLeft               = 228,
        Combining_Above                   = 230,
        Combining_AboveRight              = 232,

        Combining_DoubleBelow             = 233,
        Combining_DoubleAbove             = 234,
        Combining_IotaSubscript           = 240
    };

    enum UnicodeVersion {
        Unicode_Unassigned,    // ### Qt 5: assign with some constantly big value
        Unicode_1_1,
        Unicode_2_0,
        Unicode_2_1_2,
        Unicode_3_0,
        Unicode_3_1,
        Unicode_3_2,
        Unicode_4_0,
        Unicode_4_1,
        Unicode_5_0,
        Unicode_5_1,
        Unicode_5_2,
        Unicode_6_0,
        Unicode_6_1
    };
    // ****** WHEN ADDING FUNCTIONS, CONSIDER ADDING TO QCharRef TOO

    inline Category category() const { return QChar::category(ucs); }
    inline Direction direction() const { return QChar::direction(ucs); }
    inline Joining joining() const { return QChar::joining(ucs); }
    inline unsigned char combiningClass() const { return QChar::combiningClass(ucs); }

    inline QChar mirroredChar() const { return QChar::mirroredChar(ucs); }
    inline bool hasMirrored() const { return QChar::hasMirrored(ucs); }

    QString decomposition() const;
    inline Decomposition decompositionTag() const { return QChar::decompositionTag(ucs); }

    inline int digitValue() const { return QChar::digitValue(ucs); }
    inline QChar toLower() const { return QChar::toLower(ucs); }
    inline QChar toUpper() const { return QChar::toUpper(ucs); }
    inline QChar toTitleCase() const { return QChar::toTitleCase(ucs); }
    inline QChar toCaseFolded() const { return QChar::toCaseFolded(ucs); }

    inline UnicodeVersion unicodeVersion() const { return QChar::unicodeVersion(ucs); }

    inline char toAscii() const;
    inline char toLatin1() const;
    inline ushort unicode() const { return ucs; }
#ifdef Q_NO_PACKED_REFERENCE
    inline ushort &unicode() { return const_cast<ushort&>(ucs); }
#else
    inline ushort &unicode() { return ucs; }
#endif

    static inline QChar fromAscii(char c);
    static inline QChar fromLatin1(char c);

    inline bool isNull() const { return ucs == 0; }

    inline bool isPrint() const { return QChar::isPrint(ucs); }
    inline bool isSpace() const { return QChar::isSpace(ucs); }
    inline bool isMark() const { return QChar::isMark(ucs); }
    inline bool isPunct() const { return QChar::isPunct(ucs); }
    inline bool isSymbol() const { return QChar::isSymbol(ucs); }
    inline bool isLetter() const { return QChar::isLetter(ucs); }
    inline bool isNumber() const { return QChar::isNumber(ucs); }
    inline bool isLetterOrNumber() const { return QChar::isLetterOrNumber(ucs); }
    inline bool isDigit() const { return QChar::isDigit(ucs); }
    inline bool isLower() const { return QChar::isLower(ucs); }
    inline bool isUpper() const { return QChar::isUpper(ucs); }
    inline bool isTitleCase() const { return QChar::isTitleCase(ucs); }

    inline bool isNonCharacter() const { return QChar::isNonCharacter(ucs); }
    inline bool isHighSurrogate() const { return QChar::isHighSurrogate(ucs); }
    inline bool isLowSurrogate() const { return QChar::isLowSurrogate(ucs); }
    inline bool isSurrogate() const { return QChar::isSurrogate(ucs); }

    inline uchar cell() const { return uchar(ucs & 0xff); }
    inline uchar row() const { return uchar((ucs>>8)&0xff); }
    inline void setCell(uchar cell);
    inline void setRow(uchar row);

    static inline bool isNonCharacter(uint ucs4) {
        return ucs4 >= 0xfdd0 && (ucs4 <= 0xfdef || (ucs4 & 0xfffe) == 0xfffe);
    }
    static inline bool isHighSurrogate(uint ucs4) {
        return ((ucs4 & 0xfffffc00) == 0xd800);
    }
    static inline bool isLowSurrogate(uint ucs4) {
        return ((ucs4 & 0xfffffc00) == 0xdc00);
    }
    static inline bool isSurrogate(uint ucs4) {
        return (ucs4 - 0xd800u < 2048u);
    }
    static inline bool requiresSurrogates(uint ucs4) {
        return (ucs4 >= 0x10000);
    }
    static inline uint surrogateToUcs4(ushort high, ushort low) {
        return (uint(high)<<10) + low - 0x35fdc00;
    }
    static inline uint surrogateToUcs4(QChar high, QChar low) {
        return surrogateToUcs4(high.unicode(), low.unicode());
    }
    static inline ushort highSurrogate(uint ucs4) {
        return ushort((ucs4>>10) + 0xd7c0);
    }
    static inline ushort lowSurrogate(uint ucs4) {
        return ushort(ucs4%0x400 + 0xdc00);
    }

    static Category QT_FASTCALL category(uint ucs4);
    static Direction QT_FASTCALL direction(uint ucs4);
    static Joining QT_FASTCALL joining(uint ucs4);
    static unsigned char QT_FASTCALL combiningClass(uint ucs4);

    static uint QT_FASTCALL mirroredChar(uint ucs4);
    static bool QT_FASTCALL hasMirrored(uint ucs4);

    static QString QT_FASTCALL decomposition(uint ucs4);
    static Decomposition QT_FASTCALL decompositionTag(uint ucs4);

    static int QT_FASTCALL digitValue(uint ucs4);
    static uint QT_FASTCALL toLower(uint ucs4);
    static uint QT_FASTCALL toUpper(uint ucs4);
    static uint QT_FASTCALL toTitleCase(uint ucs4);
    static uint QT_FASTCALL toCaseFolded(uint ucs4);

    static UnicodeVersion QT_FASTCALL unicodeVersion(uint ucs4);

    static UnicodeVersion QT_FASTCALL currentUnicodeVersion();

    static bool QT_FASTCALL isPrint(uint ucs4);
    static inline bool isSpace(uint ucs4);
    static bool QT_FASTCALL isMark(uint ucs4);
    static bool QT_FASTCALL isPunct(uint ucs4);
    static bool QT_FASTCALL isSymbol(uint ucs4);
    static inline bool isLetter(uint ucs4);
    static inline bool isNumber(uint ucs4);
    static inline bool isLetterOrNumber(uint ucs4);
    static inline bool isDigit(uint ucs4);
    static inline bool isLower(uint ucs4);
    static inline bool isUpper(uint ucs4);
    static inline bool isTitleCase(uint ucs4);

private:
    static bool QT_FASTCALL isSpace_helper(uint ucs4);
    static bool QT_FASTCALL isLetter_helper(uint ucs4);
    static bool QT_FASTCALL isNumber_helper(uint ucs4);
    static bool QT_FASTCALL isLetterOrNumber_helper(uint ucs4);

#ifdef QT_NO_CAST_FROM_ASCII
    QChar(char c);
    QChar(uchar c);
#endif
    ushort ucs;
}
#if (defined(__arm__) && defined(QT_NO_ARM_EABI))
    Q_PACKED
#endif
;

Q_DECLARE_TYPEINFO(QChar, Q_MOVABLE_TYPE);

inline char QChar::toAscii() const { return ucs > 0xff ? 0 : char(ucs); }
inline char QChar::toLatin1() const { return ucs > 0xff ? '\0' : char(ucs); }
inline QChar QChar::fromLatin1(char c) { return QChar(ushort(uchar(c))); }
inline QChar QChar::fromAscii(char c) { return QChar(ushort(uchar(c))); }

inline void QChar::setCell(uchar acell)
{ ucs = ushort((ucs & 0xff00) + acell); }
inline void QChar::setRow(uchar arow)
{ ucs = ushort((ushort(arow)<<8) + (ucs&0xff)); }

inline bool QChar::isSpace(uint ucs4)
{
    // note that [0x09..0x0d] + 0x85 are exceptional Cc-s and must be handled explicitly
    return ucs4 == 0x20 || (ucs4 <= 0x0d && ucs4 >= 0x09)
            || (ucs4 > 127 && (ucs4 == 0x85 || ucs4 == 0xa0 || QChar::isSpace_helper(ucs4)));
}
inline bool QChar::isLetter(uint ucs4)
{
    return (ucs4 >= 'A' && ucs4 <= 'z' && (ucs4 >= 'a' || ucs4 <= 'Z'))
            || (ucs4 > 127 && QChar::isLetter_helper(ucs4));
}
inline bool QChar::isNumber(uint ucs4)
{ return (ucs4 <= '9' && ucs4 >= '0') || (ucs4 > 127 && QChar::isNumber_helper(ucs4)); }
inline bool QChar::isLetterOrNumber(uint ucs4)
{
    return (ucs4 >= 'A' && ucs4 <= 'z' && (ucs4 >= 'a' || ucs4 <= 'Z'))
            || (ucs4 >= '0' && ucs4 <= '9')
            || (ucs4 > 127 && QChar::isLetterOrNumber_helper(ucs4));
}
inline bool QChar::isDigit(uint ucs4)
{ return (ucs4 <= '9' && ucs4 >= '0') || (ucs4 > 127 && QChar::category(ucs4) == Number_DecimalDigit); }
inline bool QChar::isLower(uint ucs4)
{ return (ucs4 <= 'z' && ucs4 >= 'a') || (ucs4 > 127 && QChar::category(ucs4) == Letter_Lowercase); }
inline bool QChar::isUpper(uint ucs4)
{ return (ucs4 <= 'Z' && ucs4 >= 'A') || (ucs4 > 127 && QChar::category(ucs4) == Letter_Uppercase); }
inline bool QChar::isTitleCase(uint ucs4)
{ return ucs4 > 127 && QChar::category(ucs4) == Letter_Titlecase; }

inline bool operator==(QChar c1, QChar c2) { return c1.unicode() == c2.unicode(); }
inline bool operator!=(QChar c1, QChar c2) { return c1.unicode() != c2.unicode(); }
inline bool operator<=(QChar c1, QChar c2) { return c1.unicode() <= c2.unicode(); }
inline bool operator>=(QChar c1, QChar c2) { return c1.unicode() >= c2.unicode(); }
inline bool operator<(QChar c1, QChar c2) { return c1.unicode() < c2.unicode(); }
inline bool operator>(QChar c1, QChar c2) { return c1.unicode() > c2.unicode(); }

#ifndef QT_NO_DATASTREAM
Q_CORE_EXPORT QDataStream &operator<<(QDataStream &, QChar);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &, QChar &);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QCHAR_H

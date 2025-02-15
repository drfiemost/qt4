/*
 * Copyright (C) 2009 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "RegexCompiler.h"

#include "RegexInterpreter.h"
#include "RegexPattern.h"
#include <wtf/Vector.h>

#if ENABLE(YARR)

using namespace WTF;

namespace JSC { namespace Yarr {

class CharacterClassConstructor {
public:
    CharacterClassConstructor(bool isCaseInsensitive = false)
        : m_isCaseInsensitive(isCaseInsensitive)
    {
    }
    
    void reset()
    {
        m_matches.clear();
        m_ranges.clear();
        m_matchesUnicode.clear();
        m_rangesUnicode.clear();
    }

    void append(const CharacterClass* other)
    {
        for (unsigned short m_matche : other->m_matches)
            addSorted(m_matches, m_matche);
        for (auto m_range : other->m_ranges)
            addSortedRange(m_ranges, m_range.begin, m_range.end);
        for (unsigned short i : other->m_matchesUnicode)
            addSorted(m_matchesUnicode, i);
        for (auto i : other->m_rangesUnicode)
            addSortedRange(m_rangesUnicode, i.begin, i.end);
    }

    void putChar(UChar ch)
    {
        if (ch <= 0x7f) {
            if (m_isCaseInsensitive && isASCIIAlpha(ch)) {
                addSorted(m_matches, toASCIIUpper(ch));
                addSorted(m_matches, toASCIILower(ch));
            } else
                addSorted(m_matches, ch);
        } else {
            UChar upper, lower;
            if (m_isCaseInsensitive && ((upper = Unicode::toUpper(ch)) != (lower = Unicode::toLower(ch)))) {
                addSorted(m_matchesUnicode, upper);
                addSorted(m_matchesUnicode, lower);
            } else
                addSorted(m_matchesUnicode, ch);
        }
    }

    // returns true if this character has another case, and 'ch' is the upper case form.
    static inline bool isUnicodeUpper(UChar ch)
    {
        return ch != Unicode::toLower(ch);
    }

    // returns true if this character has another case, and 'ch' is the lower case form.
    static inline bool isUnicodeLower(UChar ch)
    {
        return ch != Unicode::toUpper(ch);
    }

    void putRange(UChar lo, UChar hi)
    {
        if (lo <= 0x7f) {
            char asciiLo = lo;
            char asciiHi = std::min(hi, (UChar)0x7f);
            addSortedRange(m_ranges, lo, asciiHi);
            
            if (m_isCaseInsensitive) {
                if ((asciiLo <= 'Z') && (asciiHi >= 'A'))
                    addSortedRange(m_ranges, std::max(asciiLo, 'A')+('a'-'A'), std::min(asciiHi, 'Z')+('a'-'A'));
                if ((asciiLo <= 'z') && (asciiHi >= 'a'))
                    addSortedRange(m_ranges, std::max(asciiLo, 'a')+('A'-'a'), std::min(asciiHi, 'z')+('A'-'a'));
            }
        }
        if (hi >= 0x80) {
            uint32_t unicodeCurr = std::max(lo, (UChar)0x80);
            addSortedRange(m_rangesUnicode, unicodeCurr, hi);
            
            if (m_isCaseInsensitive) {
                while (unicodeCurr <= hi) {
                    // If the upper bound of the range (hi) is 0xffff, the increments to
                    // unicodeCurr in this loop may take it to 0x10000.  This is fine
                    // (if so we won't re-enter the loop, since the loop condition above
                    // will definitely fail) - but this does mean we cannot use a UChar
                    // to represent unicodeCurr, we must use a 32-bit value instead.
                    ASSERT(unicodeCurr <= 0xffff);

                    if (isUnicodeUpper(unicodeCurr)) {
                        UChar lowerCaseRangeBegin = Unicode::toLower(unicodeCurr);
                        UChar lowerCaseRangeEnd = lowerCaseRangeBegin;
                        while ((++unicodeCurr <= hi) && isUnicodeUpper(unicodeCurr) && (Unicode::toLower(unicodeCurr) == (lowerCaseRangeEnd + 1)))
                            lowerCaseRangeEnd++;
                        addSortedRange(m_rangesUnicode, lowerCaseRangeBegin, lowerCaseRangeEnd);
                    } else if (isUnicodeLower(unicodeCurr)) {
                        UChar upperCaseRangeBegin = Unicode::toUpper(unicodeCurr);
                        UChar upperCaseRangeEnd = upperCaseRangeBegin;
                        while ((++unicodeCurr <= hi) && isUnicodeLower(unicodeCurr) && (Unicode::toUpper(unicodeCurr) == (upperCaseRangeEnd + 1)))
                            upperCaseRangeEnd++;
                        addSortedRange(m_rangesUnicode, upperCaseRangeBegin, upperCaseRangeEnd);
                    } else
                        ++unicodeCurr;
                }
            }
        }
    }

    CharacterClass* charClass()
    {
        CharacterClass* characterClass = new CharacterClass();

        characterClass->m_matches.append(m_matches);
        characterClass->m_ranges.append(m_ranges);
        characterClass->m_matchesUnicode.append(m_matchesUnicode);
        characterClass->m_rangesUnicode.append(m_rangesUnicode);

        reset();

        return characterClass;
    }

private:
    void addSorted(Vector<UChar>& matches, UChar ch)
    {
        unsigned pos = 0;
        unsigned range = matches.size();

        // binary chop, find position to insert char.
        while (range) {
            unsigned index = range >> 1;

            int val = matches[pos+index] - ch;
            if (!val)
                return;
            else if (val > 0)
                range = index;
            else {
                pos += (index+1);
                range -= (index+1);
            }
        }
        
        if (pos == matches.size())
            matches.append(ch);
        else
            matches.insert(pos, ch);
    }

    void addSortedRange(Vector<CharacterRange>& ranges, UChar lo, UChar hi)
    {
        unsigned end = ranges.size();
        
        // Simple linear scan - I doubt there are that many ranges anyway...
        // feel free to fix this with something faster (eg binary chop).
        for (unsigned i = 0; i < end; ++i) {
            // does the new range fall before the current position in the array
            if (hi < ranges[i].begin) {
                // optional optimization: concatenate appending ranges? - may not be worthwhile.
                if (hi == (ranges[i].begin - 1)) {
                    ranges[i].begin = lo;
                    return;
                }
                ranges.insert(i, CharacterRange(lo, hi));
                return;
            }
            // Okay, since we didn't hit the last case, the end of the new range is definitely at or after the begining
            // If the new range start at or before the end of the last range, then the overlap (if it starts one after the
            // end of the last range they concatenate, which is just as good.
            if (lo <= (ranges[i].end + 1)) {
                // found an intersect! we'll replace this entry in the array.
                ranges[i].begin = std::min(ranges[i].begin, lo);
                ranges[i].end = std::max(ranges[i].end, hi);

                // now check if the new range can subsume any subsequent ranges.
                unsigned next = i+1;
                // each iteration of the loop we will either remove something from the list, or break the loop.
                while (next < ranges.size()) {
                    if (ranges[next].begin <= (ranges[i].end + 1)) {
                        // the next entry now overlaps / concatenates this one.
                        ranges[i].end = std::max(ranges[i].end, ranges[next].end);
                        ranges.remove(next);
                    } else
                        break;
                }
                
                return;
            }
        }

        // CharacterRange comes after all existing ranges.
        ranges.append(CharacterRange(lo, hi));
    }

    bool m_isCaseInsensitive;

    Vector<UChar> m_matches;
    Vector<CharacterRange> m_ranges;
    Vector<UChar> m_matchesUnicode;
    Vector<CharacterRange> m_rangesUnicode;
};


CharacterClass* newlineCreate()
{
    CharacterClass* characterClass = new CharacterClass();

    characterClass->m_matches.append('\n');
    characterClass->m_matches.append('\r');
    characterClass->m_matchesUnicode.append(0x2028);
    characterClass->m_matchesUnicode.append(0x2029);
    
    return characterClass;
}

CharacterClass* digitsCreate()
{
    CharacterClass* characterClass = new CharacterClass();

    characterClass->m_ranges.append(CharacterRange('0', '9'));
    
    return characterClass;
}

CharacterClass* spacesCreate()
{
    CharacterClass* characterClass = new CharacterClass();

    characterClass->m_matches.append(' ');
    characterClass->m_ranges.append(CharacterRange('\t', '\r'));
    characterClass->m_matchesUnicode.append(0x00a0);
    characterClass->m_matchesUnicode.append(0x1680);
    characterClass->m_matchesUnicode.append(0x180e);
    characterClass->m_matchesUnicode.append(0x2028);
    characterClass->m_matchesUnicode.append(0x2029);
    characterClass->m_matchesUnicode.append(0x202f);
    characterClass->m_matchesUnicode.append(0x205f);
    characterClass->m_matchesUnicode.append(0x3000);
    characterClass->m_rangesUnicode.append(CharacterRange(0x2000, 0x200a));
    
    return characterClass;
}

CharacterClass* wordcharCreate()
{
    CharacterClass* characterClass = new CharacterClass();

    characterClass->m_matches.append('_');
    characterClass->m_ranges.append(CharacterRange('0', '9'));
    characterClass->m_ranges.append(CharacterRange('A', 'Z'));
    characterClass->m_ranges.append(CharacterRange('a', 'z'));
    
    return characterClass;
}

CharacterClass* nondigitsCreate()
{
    CharacterClass* characterClass = new CharacterClass();

    characterClass->m_ranges.append(CharacterRange(0, '0' - 1));
    characterClass->m_ranges.append(CharacterRange('9' + 1, 0x7f));
    characterClass->m_rangesUnicode.append(CharacterRange(0x80, 0xffff));
    
    return characterClass;
}

CharacterClass* nonspacesCreate()
{
    CharacterClass* characterClass = new CharacterClass();

    characterClass->m_ranges.append(CharacterRange(0, '\t' - 1));
    characterClass->m_ranges.append(CharacterRange('\r' + 1, ' ' - 1));
    characterClass->m_ranges.append(CharacterRange(' ' + 1, 0x7f));
    characterClass->m_rangesUnicode.append(CharacterRange(0x0080, 0x009f));
    characterClass->m_rangesUnicode.append(CharacterRange(0x00a1, 0x167f));
    characterClass->m_rangesUnicode.append(CharacterRange(0x1681, 0x180d));
    characterClass->m_rangesUnicode.append(CharacterRange(0x180f, 0x1fff));
    characterClass->m_rangesUnicode.append(CharacterRange(0x200b, 0x2027));
    characterClass->m_rangesUnicode.append(CharacterRange(0x202a, 0x202e));
    characterClass->m_rangesUnicode.append(CharacterRange(0x2030, 0x205e));
    characterClass->m_rangesUnicode.append(CharacterRange(0x2060, 0x2fff));
    characterClass->m_rangesUnicode.append(CharacterRange(0x3001, 0xffff));
    
    return characterClass;
}

CharacterClass* nonwordcharCreate()
{
    CharacterClass* characterClass = new CharacterClass();

    characterClass->m_matches.append('`');
    characterClass->m_ranges.append(CharacterRange(0, '0' - 1));
    characterClass->m_ranges.append(CharacterRange('9' + 1, 'A' - 1));
    characterClass->m_ranges.append(CharacterRange('Z' + 1, '_' - 1));
    characterClass->m_ranges.append(CharacterRange('z' + 1, 0x7f));
    characterClass->m_rangesUnicode.append(CharacterRange(0x80, 0xffff));

    return characterClass;
}


class RegexPatternConstructor {
public:
    RegexPatternConstructor(RegexPattern& pattern)
        : m_pattern(pattern)
        , m_characterClassConstructor(pattern.m_ignoreCase)
    {
    }

    ~RegexPatternConstructor()
    = default;

    void reset()
    {
        m_pattern.reset();
        m_characterClassConstructor.reset();
    }
    
    void assertionBOL()
    {
        m_alternative->m_terms.append(PatternTerm::BOL());
    }
    void assertionEOL()
    {
        m_alternative->m_terms.append(PatternTerm::EOL());
    }
    void assertionWordBoundary(bool invert)
    {
        m_alternative->m_terms.append(PatternTerm::WordBoundary(invert));
    }

    void atomPatternCharacter(UChar ch)
    {
        // We handle case-insensitive checking of unicode characters which do have both
        // cases by handling them as if they were defined using a CharacterClass.
        if (m_pattern.m_ignoreCase && !isASCII(ch) && (Unicode::toUpper(ch) != Unicode::toLower(ch))) {
            atomCharacterClassBegin();
            atomCharacterClassAtom(ch);
            atomCharacterClassEnd();
        } else
            m_alternative->m_terms.append(PatternTerm(ch));
    }

    void atomBuiltInCharacterClass(BuiltInCharacterClassID classID, bool invert)
    {
        switch (classID) {
        case DigitClassID:
            m_alternative->m_terms.append(PatternTerm(m_pattern.digitsCharacterClass(), invert));
            break;
        case SpaceClassID:
            m_alternative->m_terms.append(PatternTerm(m_pattern.spacesCharacterClass(), invert));
            break;
        case WordClassID:
            m_alternative->m_terms.append(PatternTerm(m_pattern.wordcharCharacterClass(), invert));
            break;
        case NewlineClassID:
            m_alternative->m_terms.append(PatternTerm(m_pattern.newlineCharacterClass(), invert));
            break;
        }
    }

    void atomCharacterClassBegin(bool invert = false)
    {
        m_invertCharacterClass = invert;
    }

    void atomCharacterClassAtom(UChar ch)
    {
        m_characterClassConstructor.putChar(ch);
    }

    void atomCharacterClassRange(UChar begin, UChar end)
    {
        m_characterClassConstructor.putRange(begin, end);
    }

    void atomCharacterClassBuiltIn(BuiltInCharacterClassID classID, bool invert)
    {
        ASSERT(classID != NewlineClassID);

        switch (classID) {
        case DigitClassID:
            m_characterClassConstructor.append(invert ? m_pattern.nondigitsCharacterClass() : m_pattern.digitsCharacterClass());
            break;
        
        case SpaceClassID:
            m_characterClassConstructor.append(invert ? m_pattern.nonspacesCharacterClass() : m_pattern.spacesCharacterClass());
            break;
        
        case WordClassID:
            m_characterClassConstructor.append(invert ? m_pattern.nonwordcharCharacterClass() : m_pattern.wordcharCharacterClass());
            break;
        
        default:
            ASSERT_NOT_REACHED();
        }
    }

    void atomCharacterClassEnd()
    {
        CharacterClass* newCharacterClass = m_characterClassConstructor.charClass();
        m_pattern.m_userCharacterClasses.append(newCharacterClass);
        m_alternative->m_terms.append(PatternTerm(newCharacterClass, m_invertCharacterClass));
    }

    void atomParenthesesSubpatternBegin(bool capture = true)
    {
        unsigned subpatternId = m_pattern.m_numSubpatterns + 1;
        if (capture)
            m_pattern.m_numSubpatterns++;

        PatternDisjunction* parenthesesDisjunction = new PatternDisjunction(m_alternative);
        m_pattern.m_disjunctions.append(parenthesesDisjunction);
        m_alternative->m_terms.append(PatternTerm(PatternTerm::TypeParenthesesSubpattern, subpatternId, parenthesesDisjunction, capture));
        m_alternative = parenthesesDisjunction->addNewAlternative();
    }

    void atomParentheticalAssertionBegin(bool invert = false)
    {
        PatternDisjunction* parenthesesDisjunction = new PatternDisjunction(m_alternative);
        m_pattern.m_disjunctions.append(parenthesesDisjunction);
        m_alternative->m_terms.append(PatternTerm(PatternTerm::TypeParentheticalAssertion, m_pattern.m_numSubpatterns + 1, parenthesesDisjunction, invert));
        m_alternative = parenthesesDisjunction->addNewAlternative();
    }

    void atomParenthesesEnd()
    {
        ASSERT(m_alternative->m_parent);
        ASSERT(m_alternative->m_parent->m_parent);
        m_alternative = m_alternative->m_parent->m_parent;
        
        m_alternative->lastTerm().parentheses.lastSubpatternId = m_pattern.m_numSubpatterns;
    }

    void atomBackReference(unsigned subpatternId)
    {
        ASSERT(subpatternId);
        m_pattern.m_maxBackReference = std::max(m_pattern.m_maxBackReference, subpatternId);

        if (subpatternId > m_pattern.m_numSubpatterns) {
            m_alternative->m_terms.append(PatternTerm::ForwardReference());
            return;
        }

        PatternAlternative* currentAlternative = m_alternative;
        ASSERT(currentAlternative);

        // Note to self: if we waited until the AST was baked, we could also remove forwards refs 
        while ((currentAlternative = currentAlternative->m_parent->m_parent)) {
            PatternTerm& term = currentAlternative->lastTerm();
            ASSERT((term.type == PatternTerm::TypeParenthesesSubpattern) || (term.type == PatternTerm::TypeParentheticalAssertion));

            if ((term.type == PatternTerm::TypeParenthesesSubpattern) && term.invertOrCapture && (subpatternId == term.subpatternId)) {
                m_alternative->m_terms.append(PatternTerm::ForwardReference());
                return;
            }
        }

        m_alternative->m_terms.append(PatternTerm(subpatternId));
    }

    PatternDisjunction* copyDisjunction(PatternDisjunction* disjunction)
    {
        PatternDisjunction* newDisjunction = new PatternDisjunction();

        newDisjunction->m_parent = disjunction->m_parent;
        for (auto alternative : disjunction->m_alternatives) {
            PatternAlternative* newAlternative = newDisjunction->addNewAlternative();
            for (auto & m_term : alternative->m_terms)
                newAlternative->m_terms.append(copyTerm(m_term));
        }

        m_pattern.m_disjunctions.append(newDisjunction);
        return newDisjunction;
    }

    PatternTerm copyTerm(PatternTerm& term)
    {
        if ((term.type != PatternTerm::TypeParenthesesSubpattern) && (term.type != PatternTerm::TypeParentheticalAssertion))
            return PatternTerm(term);

        PatternTerm termCopy = term;
        termCopy.parentheses.disjunction = copyDisjunction(termCopy.parentheses.disjunction);
        return termCopy;
    }

    void quantifyAtom(unsigned min, unsigned max, bool greedy)
    {
        ASSERT(min <= max);
        ASSERT(m_alternative->m_terms.size());

        if (!max) {
            m_alternative->removeLastTerm();
            return;
        }

        PatternTerm& term = m_alternative->lastTerm();
        ASSERT(term.type > PatternTerm::TypeAssertionWordBoundary);
        ASSERT((term.quantityCount == 1) && (term.quantityType == QuantifierFixedCount));

        // For any assertion with a zero minimum, not matching is valid and has no effect,
        // remove it.  Otherwise, we need to match as least once, but there is no point
        // matching more than once, so remove the quantifier.  It is not entirely clear
        // from the spec whether or not this behavior is correct, but I believe this
        // matches Firefox. :-/
        if (term.type == PatternTerm::TypeParentheticalAssertion) {
            if (!min)
                m_alternative->removeLastTerm();
            return;
        }

        if (min == 0)
            term.quantify(max, greedy   ? QuantifierGreedy : QuantifierNonGreedy);
        else if (min == max)
            term.quantify(min, QuantifierFixedCount);
        else {
            term.quantify(min, QuantifierFixedCount);
            m_alternative->m_terms.append(copyTerm(term));
            // NOTE: this term is interesting from an analysis perspective, in that it can be ignored.....
            m_alternative->lastTerm().quantify((max == UINT_MAX) ? max : max - min, greedy ? QuantifierGreedy : QuantifierNonGreedy);
            if (m_alternative->lastTerm().type == PatternTerm::TypeParenthesesSubpattern)
                m_alternative->lastTerm().parentheses.isCopy = true;
        }
    }

    void disjunction()
    {
        m_alternative = m_alternative->m_parent->addNewAlternative();
    }

    void regexBegin()
    {
        m_pattern.m_body = new PatternDisjunction();
        m_alternative = m_pattern.m_body->addNewAlternative();
        m_pattern.m_disjunctions.append(m_pattern.m_body);
    }
    void regexEnd()
    {
    }
    void regexError()
    {
    }

    unsigned setupAlternativeOffsets(PatternAlternative* alternative, unsigned currentCallFrameSize, unsigned initialInputPosition)
    {
        alternative->m_hasFixedSize = true;
        unsigned currentInputPosition = initialInputPosition;

        for (unsigned i = 0; i < alternative->m_terms.size(); ++i) {
            PatternTerm& term = alternative->m_terms[i];

            switch (term.type) {
            case PatternTerm::TypeAssertionBOL:
            case PatternTerm::TypeAssertionEOL:
            case PatternTerm::TypeAssertionWordBoundary:
                term.inputPosition = currentInputPosition;
                break;

            case PatternTerm::TypeBackReference:
                term.inputPosition = currentInputPosition;
                term.frameLocation = currentCallFrameSize;
                currentCallFrameSize += RegexStackSpaceForBackTrackInfoBackReference;
                alternative->m_hasFixedSize = false;
                break;

            case PatternTerm::TypeForwardReference:
                break;

            case PatternTerm::TypePatternCharacter:
                term.inputPosition = currentInputPosition;
                if (term.quantityType != QuantifierFixedCount) {
                    term.frameLocation = currentCallFrameSize;
                    currentCallFrameSize += RegexStackSpaceForBackTrackInfoPatternCharacter;
                    alternative->m_hasFixedSize = false;
                } else
                    currentInputPosition += term.quantityCount;
                break;

            case PatternTerm::TypeCharacterClass:
                term.inputPosition = currentInputPosition;
                if (term.quantityType != QuantifierFixedCount) {
                    term.frameLocation = currentCallFrameSize;
                    currentCallFrameSize += RegexStackSpaceForBackTrackInfoCharacterClass;
                    alternative->m_hasFixedSize = false;
                } else
                    currentInputPosition += term.quantityCount;
                break;

            case PatternTerm::TypeParenthesesSubpattern:
                // Note: for fixed once parentheses we will ensure at least the minimum is available; others are on their own.
                term.frameLocation = currentCallFrameSize;
                if ((term.quantityCount == 1) && !term.parentheses.isCopy) {
                    if (term.quantityType == QuantifierFixedCount) {
                        currentCallFrameSize = setupDisjunctionOffsets(term.parentheses.disjunction, currentCallFrameSize, currentInputPosition);
                        currentInputPosition += term.parentheses.disjunction->m_minimumSize;
                    } else {
                        currentCallFrameSize += RegexStackSpaceForBackTrackInfoParenthesesOnce;
                        currentCallFrameSize = setupDisjunctionOffsets(term.parentheses.disjunction, currentCallFrameSize, currentInputPosition);
                    }
                    term.inputPosition = currentInputPosition;
                } else {
                    term.inputPosition = currentInputPosition;
                    setupDisjunctionOffsets(term.parentheses.disjunction, 0, currentInputPosition);
                    currentCallFrameSize += RegexStackSpaceForBackTrackInfoParentheses;
                }
                // Fixed count of 1 could be accepted, if they have a fixed size *AND* if all alternatives are of the same length.
                alternative->m_hasFixedSize = false;
                break;

            case PatternTerm::TypeParentheticalAssertion:
                term.inputPosition = currentInputPosition;
                term.frameLocation = currentCallFrameSize;
                currentCallFrameSize = setupDisjunctionOffsets(term.parentheses.disjunction, currentCallFrameSize + RegexStackSpaceForBackTrackInfoParentheticalAssertion, currentInputPosition);
                break;
            }
        }

        alternative->m_minimumSize = currentInputPosition - initialInputPosition;
        return currentCallFrameSize;
    }

    unsigned setupDisjunctionOffsets(PatternDisjunction* disjunction, unsigned initialCallFrameSize, unsigned initialInputPosition)
    {
        if ((disjunction != m_pattern.m_body) && (disjunction->m_alternatives.size() > 1))
            initialCallFrameSize += RegexStackSpaceForBackTrackInfoAlternative;

        unsigned minimumInputSize = UINT_MAX;
        unsigned maximumCallFrameSize = 0;
        bool hasFixedSize = true;

        for (auto alternative : disjunction->m_alternatives) {
            unsigned currentAlternativeCallFrameSize = setupAlternativeOffsets(alternative, initialCallFrameSize, initialInputPosition);
            minimumInputSize = min(minimumInputSize, alternative->m_minimumSize);
            maximumCallFrameSize = max(maximumCallFrameSize, currentAlternativeCallFrameSize);
            hasFixedSize &= alternative->m_hasFixedSize;
        }
        
        ASSERT(minimumInputSize != UINT_MAX);
        ASSERT(maximumCallFrameSize >= initialCallFrameSize);

        disjunction->m_hasFixedSize = hasFixedSize;
        disjunction->m_minimumSize = minimumInputSize;
        disjunction->m_callFrameSize = maximumCallFrameSize;
        return maximumCallFrameSize;
    }

    void setupOffsets()
    {
        setupDisjunctionOffsets(m_pattern.m_body, 0, 0);
    }

private:
    RegexPattern& m_pattern;
    PatternAlternative* m_alternative;
    CharacterClassConstructor m_characterClassConstructor;
    bool m_invertCharacterClass;
};


const char* compileRegex(const UString& patternString, RegexPattern& pattern)
{
    RegexPatternConstructor constructor(pattern);

    if (const char* error = parse(constructor, patternString))
        return error;
    
    // If the pattern contains illegal backreferences reset & reparse.
    // Quoting Netscape's "What's new in JavaScript 1.2",
    //      "Note: if the number of left parentheses is less than the number specified
    //       in \#, the \# is taken as an octal escape as described in the next row."
    if (pattern.containsIllegalBackReference()) {
        unsigned numSubpatterns = pattern.m_numSubpatterns;

        constructor.reset();
#if !ASSERT_DISABLED
        const char* error =
#endif
            parse(constructor, patternString, numSubpatterns);

        ASSERT(!error);
        ASSERT(numSubpatterns == pattern.m_numSubpatterns);
    }

    constructor.setupOffsets();

    return nullptr;
};


} }

#endif

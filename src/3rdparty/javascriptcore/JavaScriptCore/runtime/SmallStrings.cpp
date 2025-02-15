/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
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
#include "SmallStrings.h"

#include "JSGlobalObject.h"
#include "JSString.h"

#include <wtf/Noncopyable.h>

namespace JSC {
static const unsigned numCharactersToStore = 0x100;

class SmallStringsStorage : public Noncopyable {
public:
    SmallStringsStorage();

    UString::Rep* rep(unsigned char character) { return &m_reps[character]; }

private:
    UString::Rep m_reps[numCharactersToStore];
};

SmallStringsStorage::SmallStringsStorage()
{
    UChar* characterBuffer = nullptr;
    RefPtr<UStringImpl> baseString = UStringImpl::createUninitialized(numCharactersToStore, characterBuffer);
    for (unsigned i = 0; i < numCharactersToStore; ++i) {
        characterBuffer[i] = i;
        new (&m_reps[i]) UString::Rep(&characterBuffer[i], 1, PassRefPtr<UStringImpl>(baseString));
    }
}

SmallStrings::SmallStrings()
    : m_emptyString(nullptr)
    , m_storage(nullptr)
{
    COMPILE_ASSERT(numCharactersToStore == sizeof(m_singleCharacterStrings) / sizeof(m_singleCharacterStrings[0]), IsNumCharactersConstInSyncWithClassUsage);

    for (auto & m_singleCharacterString : m_singleCharacterStrings)
        m_singleCharacterString = nullptr;
}

SmallStrings::~SmallStrings()
= default;

void SmallStrings::markChildren(MarkStack& markStack)
{
    if (m_emptyString)
        markStack.append(m_emptyString);
    for (auto & m_singleCharacterString : m_singleCharacterStrings) {
        if (m_singleCharacterString)
            markStack.append(m_singleCharacterString);
    }
}

unsigned SmallStrings::count() const
{
    unsigned count = 0;
    if (m_emptyString)
        ++count;
    for (auto m_singleCharacterString : m_singleCharacterStrings) {
        if (m_singleCharacterString)
            ++count;
    }
    return count;
}

void SmallStrings::createEmptyString(JSGlobalData* globalData)
{
    ASSERT(!m_emptyString);
    m_emptyString = new (globalData) JSString(globalData, "", JSString::HasOtherOwner);
}

void SmallStrings::createSingleCharacterString(JSGlobalData* globalData, unsigned char character)
{
    if (!m_storage)
        m_storage.set(new SmallStringsStorage);
    ASSERT(!m_singleCharacterStrings[character]);
    m_singleCharacterStrings[character] = new (globalData) JSString(globalData, m_storage->rep(character), JSString::HasOtherOwner);
}

UString::Rep* SmallStrings::singleCharacterStringRep(unsigned char character)
{
    if (!m_storage)
        m_storage.set(new SmallStringsStorage);
    return m_storage->rep(character);
}

} // namespace JSC

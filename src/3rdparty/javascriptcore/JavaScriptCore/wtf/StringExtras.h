/*
 * Copyright (C) 2006 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef WTF_StringExtras_h
#define WTF_StringExtras_h

#include <stdarg.h>
#include <stdio.h>

#if HAVE(STRINGS_H) 
#include <strings.h> 
#endif 

#if COMPILER(MSVC)
// FIXME: why a COMPILER check instead of OS? also, these should be HAVE checks

#  if _MSC_VER < 1900
inline int snprintf(char* buffer, size_t count, const char* format, ...) 
{
    int result;
    va_list args;
    va_start(args, format);
    result = _vsnprintf(buffer, count, format, args);
    va_end(args);
    return result;
}
#  endif // _MSC_VER < 1900

#if COMPILER(MSVC7) || OS(WINCE)

inline int vsnprintf(char* buffer, size_t count, const char* format, va_list args)
{
    return _vsnprintf(buffer, count, format, args);
}

#endif

#if OS(WINCE)

inline int strnicmp(const char* string1, const char* string2, size_t count)
{
    return _strnicmp(string1, string2, count);
}

inline int stricmp(const char* string1, const char* string2)
{
    return _stricmp(string1, string2);
}

inline char* strdup(const char* strSource)
{
    return _strdup(strSource);
}

#endif

inline int strncasecmp(const char* s1, const char* s2, size_t len)
{
    return _strnicmp(s1, s2, len);
}

inline int strcasecmp(const char* s1, const char* s2)
{
    return _stricmp(s1, s2);
}

#endif

#if OS(WINDOWS) || OS(LINUX) || OS(SOLARIS)
// FIXME: should check HAVE_STRNSTR

inline char* strnstr(const char* buffer, const char* target, size_t bufferLength)
{
    size_t targetLength = strlen(target);
    if (targetLength == 0)
        return const_cast<char*>(buffer);
    for (const char* start = buffer; *start && start + targetLength <= buffer + bufferLength; start++) {
        if (*start == *target && strncmp(start + 1, target + 1, targetLength - 1) == 0)
            return const_cast<char*>(start);
    }
    return nullptr;
}

#endif

#if COMPILER(RVCT) && __ARMCC_VERSION < 400000

int strcasecmp(const char* s1, const char* s2);
int strncasecmp(const char* s1, const char* s2, size_t len);

#endif

#endif // WTF_StringExtras_h

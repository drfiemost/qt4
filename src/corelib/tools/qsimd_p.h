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

#ifndef QSIMD_P_H
#define QSIMD_P_H

#include <qglobal.h>
#include <qatomic.h>


/*
 * qt_module_config.prf defines the QT_COMPILER_SUPPORTS_XXX macros.
 * They mean the compiler supports the necessary flags and the headers
 * for the x86 and ARM intrinsics:
 *  - GCC: the -mXXX or march=YYY flag is necessary before #include
 *  - Intel CC: #include can happen unconditionally
 *  - MSVC: #include can happen unconditionally
 *  - RVCT: ???
 *
 * We will try to include all headers possible under this configuration.
 *
 * Supported XXX are:
 *   Flag  | Arch |  GCC  | Intel CC |  MSVC  |
 *  NEON   | ARM  | I & C | None     |   ?    |
 *  SSE2   | x86  | I & C | I & C    | I & C  |
 *  SSE3   | x86  | I & C | I & C    | I only |
 *  SSSE3  | x86  | I & C | I & C    | I only |
 *  SSE4_1 | x86  | I & C | I & C    | I only |
 *  SSE4_2 | x86  | I & C | I & C    | I only |
 *  AVX    | x86  | I & C | I & C    | I & C  |
 *  AVX2   | x86  | I & C | I & C    | I only |
 * I = intrinsics; C = code generation
 */

// SSE intrinsics
#if defined(__SSE2__) || (defined(QT_COMPILER_SUPPORTS_SSE2) && defined(Q_CC_MSVC))
#if defined(QT_LINUXBASE)
/// this is an evil hack - the posix_memalign declaration in LSB
/// is wrong - see http://bugs.linuxbase.org/show_bug.cgi?id=2431
#  define posix_memalign _lsb_hack_posix_memalign
#  include <emmintrin.h>
#  undef posix_memalign
#else
#  ifdef Q_CC_MINGW
#    include <windows.h>
#  endif
#  include <emmintrin.h>
#endif
#endif

// SSE3 intrinsics
#if defined(__SSE3__) || (defined(QT_COMPILER_SUPPORTS_SSE3) && defined(Q_CC_MSVC))
#include <pmmintrin.h>
#endif

// SSSE3 intrinsics
#if defined(__SSSE3__) || (defined(QT_COMPILER_SUPPORTS_SSSE3) && defined(Q_CC_MSVC))
#include <tmmintrin.h>
#endif

// SSE4.1 intrinsics
#if defined(__SSE4_1__) || (defined(QT_COMPILER_SUPPORTS_SSE4_1) && defined(Q_CC_MSVC))
#include <smmintrin.h>
#endif

// SSE4.2 intrinsics
#if defined(__SSE4_2__) || (defined(QT_COMPILER_SUPPORTS_SSE4_2) && defined(Q_CC_MSVC))
#include <nmmintrin.h>

// Add missing intrisics in some compilers (e.g. llvm-gcc)
#ifndef _SIDD_UBYTE_OPS
#define _SIDD_UBYTE_OPS                 0x00
#endif

#ifndef _SIDD_UWORD_OPS
#define _SIDD_UWORD_OPS                 0x01
#endif

#ifndef _SIDD_SBYTE_OPS
#define _SIDD_SBYTE_OPS                 0x02
#endif

#ifndef _SIDD_SWORD_OPS
#define _SIDD_SWORD_OPS                 0x03
#endif

#ifndef _SIDD_CMP_EQUAL_ANY
#define _SIDD_CMP_EQUAL_ANY             0x00
#endif

#ifndef _SIDD_CMP_RANGES
#define _SIDD_CMP_RANGES                0x04
#endif

#ifndef _SIDD_CMP_EQUAL_EACH
#define _SIDD_CMP_EQUAL_EACH            0x08
#endif

#ifndef _SIDD_CMP_EQUAL_ORDERED
#define _SIDD_CMP_EQUAL_ORDERED         0x0c
#endif

#ifndef _SIDD_POSITIVE_POLARITY
#define _SIDD_POSITIVE_POLARITY         0x00
#endif

#ifndef _SIDD_NEGATIVE_POLARITY
#define _SIDD_NEGATIVE_POLARITY         0x10
#endif

#ifndef _SIDD_MASKED_POSITIVE_POLARITY
#define _SIDD_MASKED_POSITIVE_POLARITY  0x20
#endif

#ifndef _SIDD_MASKED_NEGATIVE_POLARITY
#define _SIDD_MASKED_NEGATIVE_POLARITY  0x30
#endif

#ifndef _SIDD_LEAST_SIGNIFICANT
#define _SIDD_LEAST_SIGNIFICANT         0x00
#endif

#ifndef _SIDD_MOST_SIGNIFICANT
#define _SIDD_MOST_SIGNIFICANT          0x40
#endif

#ifndef _SIDD_BIT_MASK
#define _SIDD_BIT_MASK                  0x00
#endif

#ifndef _SIDD_UNIT_MASK
#define _SIDD_UNIT_MASK                 0x40
#endif

#endif

// AVX intrinsics
#if defined(__AVX__) || (defined(QT_COMPILER_SUPPORTS_AVX) && defined(Q_CC_MSVC))
#include <immintrin.h>
#endif

// NEON intrinsics
#if defined __ARM_NEON__
#define QT_ALWAYS_HAVE_NEON
#include <arm_neon.h>
#endif

QT_BEGIN_NAMESPACE


enum CPUFeatures {
    NEON        = 0x1,
    SSE2        = 0x2,
    SSE3        = 0x4,
    SSSE3       = 0x8,
    SSE4_1      = 0x10,
    SSE4_2      = 0x20,
    AVX         = 0x40,
    AVX2        = 0x80,
    HLE         = 0x100,
    RTM         = 0x200,

    // used only to indicate that the CPU detection was initialised
    QSimdInitialized = 0x80000000
};

static const uint qCompilerCpuFeatures = 0
#if defined __RTM__
        | RTM
#endif
#if defined __HLE__
        | HLE
#endif
#if defined __AVX2__
        | AVX2
#endif
#if defined __AVX__
        | AVX
#endif
#if defined __SSE4_2__
        | SSE4_2
#endif
#if defined __SSE4_1__
        | SSE4_1
#endif
#if defined __SSSE3__
        | SSSE3
#endif
#if defined __SSE3__
        | SSE3
#endif
#if defined __SSE2__
        | SSE2
#endif
#if defined __ARM_NEON__
        | NEON
#endif
        ;


extern Q_CORE_EXPORT QBasicAtomicInt qt_cpu_features;
Q_CORE_EXPORT void qDetectCpuFeatures();

inline uint qCpuFeatures()
{
    int features = qt_cpu_features.loadRelaxed();
    if (Q_UNLIKELY(features == 0)) {
        qDetectCpuFeatures();
        features = qt_cpu_features.loadRelaxed();
        Q_ASSUME(features != 0);
    }
    return uint(features);
}

inline uint qCpuHasFeature(CPUFeatures feature)
{
    return qCompilerCpuFeatures & feature || qCpuFeatures() & feature;
}


#define ALIGNMENT_PROLOGUE_16BYTES(ptr, i, length) \
    for (; i < static_cast<int>(std::min(static_cast<quintptr>(length), ((4 - ((reinterpret_cast<quintptr>(ptr) >> 2) & 0x3)) & 0x3))); ++i)

inline short operator ""_s(unsigned long long value) { return static_cast<short>(value); }
inline char  operator ""_c(unsigned long long value) { return static_cast<char>(value); }

QT_END_NAMESPACE


#endif // QSIMD_P_H

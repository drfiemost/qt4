/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Copyright (C) 2012 Intel Corporation
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

#ifndef QGLOBAL_H
# include <QtCore/qglobal.h>
#endif

#ifndef QCOMPILERDETECTION_H
#define QCOMPILERDETECTION_H


#ifdef __has_cpp_attribute
#  if __has_cpp_attribute(deprecated)
#    define Q_DECL_DEPRECATED [[deprecated]]
#  endif
#  if __has_cpp_attribute(noreturn)
#    define Q_NORETURN [[noreturn]]
#  endif
#endif

/*
   The compiler, must be one of: (Q_CC_x)

     MSVC     - Microsoft Visual C/C++, Intel C++ for Windows
     GNU      - GNU C++
     INTEL    - Intel C++ for Linux, Intel C++ for Windows
     RVCT     - ARM Realview Compiler Suite
     CLANG    - C++ front-end for the LLVM compiler


   Should be sorted most to least authoritative.
*/

#if defined(_MSC_VER)
#  define Q_CC_MSVC
#  define Q_CC_MSVC_NET
#  define Q_OUTOFLINE_TEMPLATE inline
#  define Q_NO_TEMPLATE_FRIENDS
#  define Q_COMPILER_MANGLES_RETURN_TYPE
#  define Q_ALIGNOF(type) __alignof(type)
#  define Q_ASSUME(expr) __assume(expr)
#  define Q_UNREACHABLE() __assume(0)
#  ifndef Q_NORETURN
#    define Q_NORETURN __declspec(noreturn)
#  endif
#  ifndef Q_DECL_DEPRECATED
#    define Q_DECL_DEPRECATED __declspec(deprecated)
#  endif
#  define Q_DECL_EXPORT __declspec(dllexport)
#  define Q_DECL_IMPORT __declspec(dllimport)
/* Intel C++ disguising as Visual C++: the `using' keyword avoids warnings */
#  if defined(__INTEL_COMPILER)
#    define Q_DECL_VARIABLE_DEPRECATED
#    define Q_CC_INTEL
#  endif
/* MSVC does not support SSE/MMX on x64 */
#  if (defined(Q_CC_MSVC) && defined(_M_X64))
#    undef QT_HAVE_SSE
#    undef QT_HAVE_MMX
#    undef QT_HAVE_3DNOW
#  endif

/* ARM Realview Compiler Suite
   RVCT compiler also defines __EDG__ and __GNUC__ (if --gnu flag is given),
   so check for it before that */
#elif defined(__ARMCC__) || defined(__CC_ARM)
#  define Q_CC_RVCT
#  if __TARGET_ARCH_ARM >= 6
#    define QT_HAVE_ARMV6
#  endif
/* work-around for missing compiler intrinsics */
#  define __is_empty(X) false
#  define __is_pod(X) false
#  ifndef Q_DECL_DEPRECATED
#    define Q_DECL_DEPRECATED __attribute__ ((__deprecated__))
#  endif
#  ifdef Q_OS_LINUX
#    define Q_DECL_EXPORT     __attribute__((visibility("default")))
#    define Q_DECL_IMPORT     __attribute__((visibility("default")))
#    define Q_DECL_HIDDEN     __attribute__((visibility("hidden")))
#  else
#    define Q_DECL_EXPORT     __declspec(dllexport)
#    define Q_DECL_IMPORT     __declspec(dllimport)
#  endif

#elif defined(__GNUC__)
#  define Q_CC_GNU          (__GNUC__ * 100 + __GNUC_MINOR__)
#  define Q_C_CALLBACKS
#  if defined(__MINGW32__)
#    define Q_CC_MINGW
#  endif
#  if defined(__INTEL_COMPILER)
/* Intel C++ also masquerades as GCC */
#    define Q_CC_INTEL
#    define Q_ASSUME(expr)  __assume(expr)
#    define Q_UNREACHABLE() __assume(0)
#  elif defined(__clang__)
/* Clang also masquerades as GCC */
#    define Q_CC_CLANG
#    define Q_ASSUME(expr)  if (expr){} else __builtin_unreachable()
#    define Q_UNREACHABLE() __builtin_unreachable()
#    if !defined(__has_extension)
/* Compatibility with older Clang versions */
#      define __has_extension __has_feature
#    endif
#  else
/* Plain GCC */
#    define Q_ASSUME(expr)  if (expr){} else __builtin_unreachable()
#    define Q_UNREACHABLE() __builtin_unreachable()
#  endif
#  define Q_ALIGNOF(type)   __alignof__(type)
#  define Q_TYPEOF(expr)    __typeof__(expr)
#  ifndef Q_DECL_DEPRECATED
#    define Q_DECL_DEPRECATED __attribute__ ((__deprecated__))
#  endif
#  define Q_DECL_ALIGN(n)   __attribute__((__aligned__(n)))
#  define Q_LIKELY(expr)    __builtin_expect(!!(expr), true)
#  define Q_UNLIKELY(expr)  __builtin_expect(!!(expr), false)
#  ifndef Q_NORETURN
#    define Q_NORETURN        __attribute__((__noreturn__))
#  endif
#  define Q_REQUIRED_RESULT __attribute__ ((__warn_unused_result__))
#  define Q_DECL_PURE_FUNCTION __attribute__((pure))
#  define Q_DECL_CONST_FUNCTION __attribute__((const))
#  if !defined(QT_MOC_CPP)
#    define Q_PACKED __attribute__ ((__packed__))
#    define Q_NO_PACKED_REFERENCE
#    ifndef __ARM_EABI__
#      define QT_NO_ARM_EABI
#    endif
#  endif

#  ifdef Q_OS_WIN
#    define Q_DECL_EXPORT     __declspec(dllexport)
#    define Q_DECL_IMPORT     __declspec(dllimport)
#  elif defined(QT_VISIBILITY_AVAILABLE)
#    define Q_DECL_EXPORT     __attribute__((visibility("default")))
#    define Q_DECL_IMPORT     __attribute__((visibility("default")))
#    define Q_DECL_HIDDEN     __attribute__((visibility("hidden")))
#  endif

#else
#  error "Qt has not been tested with this compiler"
#endif

#ifndef Q_NORETURN
# define Q_NORETURN
#endif

/*
 * C++11 support
 *
 *  Paper           Macro                               SD-6 macro
 *  N2341           Q_COMPILER_ALIGNAS
 *  N2341           Q_COMPILER_ALIGNOF
 *  N2427           Q_COMPILER_ATOMICS
 *  N2761           Q_COMPILER_ATTRIBUTES
 *  N2541           Q_COMPILER_AUTO_FUNCTION
 *  N1984 N2546     Q_COMPILER_AUTO_TYPE
 *  N2437           Q_COMPILER_CLASS_ENUM
 *  N2235           Q_COMPILER_CONSTEXPR                __cpp_constexpr = 200704
 *  N2343 N3276     Q_COMPILER_DECLTYPE                 __cpp_decltype = 200707
 *  N2346           Q_COMPILER_DEFAULT_DELETE_MEMBERS
 *  N1986           Q_COMPILER_DELEGATING_CONSTRUCTORS
 *  N3206 N3272     Q_COMPILER_EXPLICIT_OVERRIDES   (v0.9 and above only)
 *  N1987           Q_COMPILER_EXTERN_TEMPLATES
 *  N2540           Q_COMPILER_INHERITING_CONSTRUCTORS
 *  N2672           Q_COMPILER_INITIALIZER_LISTS
 *  N2658 N2927     Q_COMPILER_LAMBDA                   __cpp_lambdas = 200907
 *  N2756           Q_COMPILER_NONSTATIC_MEMBER_INIT
 *  N2431           Q_COMPILER_NULLPTR
 *  N2930           Q_COMPILER_RANGE_FOR
 *  N2442           Q_COMPILER_RAW_STRINGS
 *  N2439           Q_COMPILER_REF_QUALIFIERS
 *  N2118 N2844 N3053 Q_COMPILER_RVALUE_REFS            __cpp_rvalue_references = 200610
 *  N1720           Q_COMPILER_STATIC_ASSERT
 *  N2258           Q_COMPILER_TEMPLATE_ALIAS
 *  N2659           Q_COMPILER_THREAD_LOCAL
 *  N2756           Q_COMPILER_UDL
 *  N2442           Q_COMPILER_UNICODE_STRINGS          __cpp_unicode_literals = 200710
 *  N2544           Q_COMPILER_UNRESTRICTED_UNIONS
 *  N1653           Q_COMPILER_VARIADIC_MACROS
 *  N2242 N2555     Q_COMPILER_VARIADIC_TEMPLATES       __cpp_variadic_templates = 200704
 *
 * For any future version of the C++ standard, we use only the SD-6 macro.
 * For full listing, see
 *  http://isocpp.org/std/standing-documents/sd-6-sg10-feature-test-recommendations
 */

#ifdef Q_CC_INTEL
#  if __INTEL_COMPILER < 1200
#    define Q_NO_TEMPLATE_FRIENDS
#  endif
#  if __cplusplus >= 201103L
#    if __INTEL_COMPILER >= 1100
#      define Q_COMPILER_RVALUE_REFS
#      define Q_COMPILER_EXTERN_TEMPLATES
#      define Q_COMPILER_DECLTYPE
#    elif __INTEL_COMPILER >= 1200
#      define Q_COMPILER_VARIADIC_TEMPLATES
#      define Q_COMPILER_AUTO_TYPE
#      define Q_COMPILER_DEFAULT_DELETE_MEMBERS
#      define Q_COMPILER_CLASS_ENUM
#      define Q_COMPILER_LAMBDA
#    endif
#  endif
#endif

#if defined(Q_CC_CLANG) && !defined(Q_CC_INTEL)
#  if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
    /* Detect C++ features using __has_feature(), see http://clang.llvm.org/docs/LanguageExtensions.html#cxx11 */
#    if __has_feature(cxx_auto_type)
#      define Q_COMPILER_AUTO_FUNCTION
#      define Q_COMPILER_AUTO_TYPE
#    endif
#    if __has_feature(cxx_constexpr)
#      define Q_COMPILER_CONSTEXPR
#    endif
#    if __has_feature(cxx_decltype) /* && __has_feature(cxx_decltype_incomplete_return_types) */
#      define Q_COMPILER_DECLTYPE
#    endif
#    if __has_feature(cxx_defaulted_functions) && __has_feature(cxx_deleted_functions)
#      define Q_COMPILER_DEFAULT_DELETE_MEMBERS
#    endif
#    if __has_feature(cxx_generalized_initializers)
#      define Q_COMPILER_INITIALIZER_LISTS
#    endif
#    if __has_feature(cxx_lambdas)
#      define Q_COMPILER_LAMBDA
#    endif
#    if __has_feature(cxx_rvalue_references)
#      define Q_COMPILER_RVALUE_REFS
#    endif
#    if __has_feature(cxx_strong_enums)
#      define Q_COMPILER_CLASS_ENUM
#    endif
#    if __has_feature(cxx_unicode_literals)
#      define Q_COMPILER_UNICODE_STRINGS
#    endif
#    if __has_feature(cxx_variadic_templates)
#      define Q_COMPILER_VARIADIC_TEMPLATES
#    endif
    /* Features that have no __has_feature() check */
#    if ((__clang_major__ * 100) + __clang_minor__) >= 209 /* since clang 2.9 */
#      define Q_COMPILER_EXTERN_TEMPLATES
#    endif
#  endif
#endif

#if defined(Q_CC_GNU) && !defined(Q_CC_INTEL) && !defined(Q_CC_CLANG)
#  if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 403
       /* C++0x features supported in GCC 4.3: */
#      define Q_COMPILER_RVALUE_REFS
#      define Q_COMPILER_DECLTYPE
#      define Q_COMPILER_STATIC_ASSERT
#      define Q_COMPILER_VARIADIC_MACROS
#    endif
#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 404
       /* C++0x features supported in GCC 4.4: */
#      define Q_COMPILER_ATOMICS
#      define Q_COMPILER_AUTO_FUNCTION
#      define Q_COMPILER_AUTO_TYPE
#      define Q_COMPILER_CLASS_ENUM
#      define Q_COMPILER_DEFAULT_DELETE_MEMBERS
#      define Q_COMPILER_EXTERN_TEMPLATES
#      define Q_COMPILER_INITIALIZER_LISTS
#      define Q_COMPILER_UNICODE_STRINGS
#      define Q_COMPILER_VARIADIC_TEMPLATES
#    endif
#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 405
       /* C++0x features supported in GCC 4.5: */
#      define Q_COMPILER_LAMBDA
#      define Q_COMPILER_RAW_STRINGS
#    endif
#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 406
       /* C++0x features supported in GCC 4.6: */
#      define Q_COMPILER_CONSTEXPR
#      define Q_COMPILER_NULLPTR
#      define Q_COMPILER_UNRESTRICTED_UNIONS
#      define Q_COMPILER_RANGE_FOR
#    endif
#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 407
       /* C++11 features supported in GCC 4.7: */
#      define Q_COMPILER_NONSTATIC_MEMBER_INIT
#      define Q_COMPILER_DELEGATING_CONSTRUCTORS
#      define Q_COMPILER_EXPLICIT_OVERRIDES
#      define Q_COMPILER_TEMPLATE_ALIAS
#      define Q_COMPILER_UDL
#    endif
#  endif
#endif

#if defined(Q_CC_MSVC) && !defined(Q_CC_INTEL)
#  if defined(__cplusplus)
#    if _MSC_VER >= 1600
       /* C++11 features supported in VC10 = VC2010: */
#      define Q_COMPILER_AUTO_FUNCTION
#      define Q_COMPILER_AUTO_TYPE
#      define Q_COMPILER_DECLTYPE
#      define Q_COMPILER_LAMBDA
#      define Q_COMPILER_RVALUE_REFS
//  MSVC's library has std::initializer_list, but the compiler does not support the braces initialization
//#      define Q_COMPILER_INITIALIZER_LISTS
#    endif
#  endif
#endif

#ifndef Q_REQUIRED_RESULT
#  define Q_REQUIRED_RESULT
#endif
#ifndef Q_DECL_DEPRECATED
#  define Q_DECL_DEPRECATED
#endif
#ifndef Q_DECL_VARIABLE_DEPRECATED
#  define Q_DECL_VARIABLE_DEPRECATED Q_DECL_DEPRECATED
#endif
#ifndef Q_DECL_EXPORT
#  define Q_DECL_EXPORT
#endif
#ifndef Q_DECL_IMPORT
#  define Q_DECL_IMPORT
#endif
#ifndef Q_DECL_HIDDEN
#  define Q_DECL_HIDDEN
#endif
#ifndef Q_DECL_PURE_FUNCTION
#  define Q_DECL_PURE_FUNCTION
#endif
#ifndef Q_DECL_CONST_FUNCTION
#  define Q_DECL_CONST_FUNCTION Q_DECL_PURE_FUNCTION
#endif

#endif // QCOMPILERDETECTION_H

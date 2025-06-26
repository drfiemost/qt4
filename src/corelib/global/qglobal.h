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

#ifndef QGLOBAL_H
#define QGLOBAL_H

#include <stddef.h>

#define QT_VERSION_STR   "4.8.7"
/*
   QT_VERSION is (major << 16) + (minor << 8) + patch.
*/
#define QT_VERSION 0x040807
/*
   can be used like #if (QT_VERSION >= QT_VERSION_CHECK(4, 4, 0))
*/
#define QT_VERSION_CHECK(qt_version_check_major, qt_version_check_minor, qt_version_check_patch) ((qt_version_check_major<<16)|(qt_version_check_minor<<8)|(qt_version_check_patch))

#define QT_PACKAGEDATE_STR "YYYY-MM-DD"

#define QT_PACKAGE_TAG ""

#if !defined(QT_BUILD_MOC)
#include <QtCore/qconfig.h>
#endif

/* These two macros makes it possible to turn the builtin line expander into a
 * string literal. */
#define QT_STRINGIFY2(x) #x
#define QT_STRINGIFY(x) QT_STRINGIFY2(x)

#ifdef __cplusplus

#include <algorithm>

#ifndef QT_NAMESPACE /* user namespace */

# define QT_PREPEND_NAMESPACE(name) ::name
# define QT_USE_NAMESPACE
# define QT_BEGIN_NAMESPACE
# define QT_END_NAMESPACE
# define QT_BEGIN_INCLUDE_NAMESPACE
# define QT_END_INCLUDE_NAMESPACE
# define QT_BEGIN_MOC_NAMESPACE
# define QT_END_MOC_NAMESPACE
# define QT_FORWARD_DECLARE_CLASS(name) class name;
# define QT_FORWARD_DECLARE_STRUCT(name) struct name;
# define QT_MANGLE_NAMESPACE(name) name

#else /* user namespace */

# define QT_PREPEND_NAMESPACE(name) ::QT_NAMESPACE::name
# define QT_USE_NAMESPACE using namespace ::QT_NAMESPACE;
# define QT_BEGIN_NAMESPACE namespace QT_NAMESPACE {
# define QT_END_NAMESPACE }
# define QT_BEGIN_INCLUDE_NAMESPACE }
# define QT_END_INCLUDE_NAMESPACE namespace QT_NAMESPACE {
# define QT_BEGIN_MOC_NAMESPACE QT_USE_NAMESPACE
# define QT_END_MOC_NAMESPACE
# define QT_FORWARD_DECLARE_CLASS(name) \
    QT_BEGIN_NAMESPACE class name; QT_END_NAMESPACE \
    using QT_PREPEND_NAMESPACE(name);

# define QT_FORWARD_DECLARE_STRUCT(name) \
    QT_BEGIN_NAMESPACE struct name; QT_END_NAMESPACE \
    using QT_PREPEND_NAMESPACE(name);

# define QT_MANGLE_NAMESPACE0(x) x
# define QT_MANGLE_NAMESPACE1(a, b) a##_##b
# define QT_MANGLE_NAMESPACE2(a, b) QT_MANGLE_NAMESPACE1(a,b)
# define QT_MANGLE_NAMESPACE(name) QT_MANGLE_NAMESPACE2( \
        QT_MANGLE_NAMESPACE0(name), QT_MANGLE_NAMESPACE0(QT_NAMESPACE))

namespace QT_NAMESPACE {}

# ifndef QT_BOOTSTRAPPED
# ifndef QT_NO_USING_NAMESPACE
   /*
    This expands to a "using QT_NAMESPACE" also in _header files_.
    It is the only way the feature can be used without too much
    pain, but if people _really_ do not want it they can add
    DEFINES += QT_NO_USING_NAMESPACE to their .pro files.
    */
   QT_USE_NAMESPACE
# endif
# endif

#endif /* user namespace */

#else /* __cplusplus */

# define QT_BEGIN_NAMESPACE
# define QT_END_NAMESPACE
# define QT_USE_NAMESPACE
# define QT_BEGIN_INCLUDE_NAMESPACE
# define QT_END_INCLUDE_NAMESPACE

#endif /* __cplusplus */

#if defined(Q_OS_MAC) && !defined(Q_CC_INTEL)
#define QT_BEGIN_HEADER extern "C++" {
#define QT_END_HEADER }
#else
#define QT_BEGIN_HEADER
#define QT_END_HEADER
#endif

/*
   The operating system, must be one of: (Q_OS_x)

     DARWIN   - Darwin OS (synonym for Q_OS_MAC)
     MSDOS    - MS-DOS and Windows
     OS2      - OS/2
     OS2EMX   - XFree86 on OS/2 (not PM)
     WIN32    - Win32 (Windows 2000/XP/Vista/7 and Windows Server 2003/2008)
     CYGWIN   - Cygwin
     LINUX    - Linux
     FREEBSD  - FreeBSD
     NETBSD   - NetBSD
     OPENBSD  - OpenBSD
     BSDI     - BSD/OS
     HURD     - GNU Hurd
     BSD4     - Any BSD 4.4 system
*/

#if defined(__APPLE__) && (defined(__GNUC__) || defined(__xlC__) || defined(__xlc__))
#  define Q_OS_DARWIN
#  define Q_OS_BSD4
#  ifdef __LP64__
#    define Q_OS_DARWIN64
#  else
#    define Q_OS_DARWIN32
#  endif
#elif defined(__CYGWIN__)
#  define Q_OS_CYGWIN
#elif !defined(SAG_COM) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#  define Q_OS_WIN32
#  define Q_OS_WIN64
#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#    define Q_OS_WIN32
#elif defined(__MWERKS__) && defined(__INTEL__)
#  define Q_OS_WIN32
#elif defined(__linux__) || defined(__linux)
#  define Q_OS_LINUX
#elif defined(__FreeBSD__) || defined(__DragonFly__)
#  define Q_OS_FREEBSD
#  define Q_OS_BSD4
#elif defined(__NetBSD__)
#  define Q_OS_NETBSD
#  define Q_OS_BSD4
#elif defined(__OpenBSD__)
#  define Q_OS_OPENBSD
#  define Q_OS_BSD4
#elif defined(__bsdi__)
#  define Q_OS_BSDI
#  define Q_OS_BSD4
#elif defined(__GNU__)
#  define Q_OS_HURD
#elif defined(__MAKEDEPEND__)
#else
#  error "Qt has not been ported to this OS - talk to qt-bugs@trolltech.com"
#endif

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
#  define Q_OS_WIN
#endif

#if defined(Q_OS_DARWIN)
#  define Q_OS_MAC /* Q_OS_MAC is mostly for compatibility, but also more clear */
#  define Q_OS_MACX /* Q_OS_MACX is only for compatibility.*/
#  if defined(Q_OS_DARWIN64)
#     define Q_OS_MAC64
#  elif defined(Q_OS_DARWIN32)
#     define Q_OS_MAC32
#  endif
#endif

#ifdef QT_AUTODETECT_COCOA
#  ifdef Q_OS_MAC64
#    define QT_MAC_USE_COCOA 1
#  endif
#endif

#if defined(Q_WS_MAC64) && !defined(QT_MAC_USE_COCOA) && !defined(QT_BUILD_QMAKE) && !defined(QT_BOOTSTRAPPED)
#error "You are building a 64-bit application, but using a 32-bit version of Qt. Check your build configuration."
#endif

#if defined(Q_OS_WIN)
#  undef Q_OS_UNIX
#elif !defined(Q_OS_UNIX)
#  define Q_OS_UNIX
#endif

#if defined(Q_OS_DARWIN) && !defined(QT_LARGEFILE_SUPPORT)
#  define QT_LARGEFILE_SUPPORT 64
#endif

#ifdef Q_OS_DARWIN
#  include <AvailabilityMacros.h>
#
#  // Availability.h was introduced with the OS X 10.6 SDK
#  if (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && __MAC_OS_X_VERSION_MAX_ALLOWED >= 1060) || \
      (defined(MAC_OS_X_VERSION_MAX_ALLOWED) && MAC_OS_X_VERSION_MAX_ALLOWED >= 1060)
#    include <Availability.h>
#  endif
#
#  ifdef Q_OS_MACX
#    if !defined(__MAC_OS_X_VERSION_MIN_REQUIRED) || __MAC_OS_X_VERSION_MIN_REQUIRED < 1040
#       undef __MAC_OS_X_VERSION_MIN_REQUIRED
#       define __MAC_OS_X_VERSION_MIN_REQUIRED 1040
#    endif
#    if !defined(MAC_OS_X_VERSION_MIN_REQUIRED) || MAC_OS_X_VERSION_MIN_REQUIRED < 1040
#       undef MAC_OS_X_VERSION_MIN_REQUIRED
#       define MAC_OS_X_VERSION_MIN_REQUIRED 1040
#    endif
#  endif
#
#  // Numerical checks are preferred to named checks, but to be safe
#  // we define the missing version names in case Qt uses them.
#
#  if !defined(__MAC_10_4)
#       define __MAC_10_4 1040
#  endif
#  if !defined(__MAC_10_5)
#       define __MAC_10_5 1050
#  endif
#  if !defined(__MAC_10_6)
#       define __MAC_10_6 1060
#  endif
#  if !defined(__MAC_10_7)
#       define __MAC_10_7 1070
#  endif
#  if !defined(__MAC_10_8)
#       define __MAC_10_8 1080
#  endif
#  if !defined(__MAC_10_9)
#       define __MAC_10_9 1090
#  endif
#  if !defined(__MAC_10_10)
#       define __MAC_10_10 101000
#  endif
#  if !defined(MAC_OS_X_VERSION_10_4)
#       define MAC_OS_X_VERSION_10_4 1040
#  endif
#  if !defined(MAC_OS_X_VERSION_10_5)
#       define MAC_OS_X_VERSION_10_5 1050
#  endif
#  if !defined(MAC_OS_X_VERSION_10_6)
#       define MAC_OS_X_VERSION_10_6 1060
#  endif
#  if !defined(MAC_OS_X_VERSION_10_7)
#       define MAC_OS_X_VERSION_10_7 1070
#  endif
#  if !defined(MAC_OS_X_VERSION_10_8)
#       define MAC_OS_X_VERSION_10_8 1080
#  endif
#  if !defined(MAC_OS_X_VERSION_10_9)
#       define MAC_OS_X_VERSION_10_9 1090
#  endif
#  if !defined(MAC_OS_X_VERSION_10_10)
#       define MAC_OS_X_VERSION_10_10 101000
#  endif
#endif

#ifdef __LSB_VERSION__
#  if __LSB_VERSION__ < 40
#    error "This version of the Linux Standard Base is unsupported"
#  endif
#ifndef QT_LINUXBASE
#  define QT_LINUXBASE
#endif
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
#  define Q_CANNOT_DELETE_CONSTANT
#  define Q_OUTOFLINE_TEMPLATE inline
#  define Q_NO_TEMPLATE_FRIENDS
#  define Q_ALIGNOF(type) __alignof(type)
#  define Q_ASSUME(expr) __assume(expr)
#  define Q_UNREACHABLE() __assume(0)
/* Intel C++ disguising as Visual C++: the `using' keyword avoids warnings */
#  if defined(__INTEL_COMPILER)
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
#  ifdef __APPLE__
#    define Q_NO_DEPRECATED_CONSTRUCTORS
#  endif
#  if __GNUC__ == 2 && __GNUC_MINOR__ <= 7
#    define Q_FULL_TEMPLATE_INSTANTIATION
#  endif
/* GCC 2.95 knows "using" but does not support it correctly */
#  if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#    define Q_ALIGNOF(type)   __alignof__(type)
#  endif
#  if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 96)
#    define Q_LIKELY(expr)    __builtin_expect(!!(expr), true)
#    define Q_UNLIKELY(expr)  __builtin_expect(!!(expr), false)
#  endif
/* GCC <= 3.3 cannot handle template friends */
#  if __GNUC__ < 3 || (__GNUC__ == 3 && __GNUC_MINOR__ <= 3)
#    define Q_NO_TEMPLATE_FRIENDS
#  endif
/* Apple's GCC 3.1 chokes on our streaming qDebug() */
#  if defined(Q_OS_DARWIN) && __GNUC__ == 3 && (__GNUC_MINOR__ >= 1 && __GNUC_MINOR__ < 3)
#    define Q_BROKEN_DEBUG_STREAM
#  endif
#  if (defined(Q_CC_GNU) || defined(Q_CC_INTEL)) && !defined(QT_MOC_CPP)
#    define Q_PACKED __attribute__ ((__packed__))
#    define Q_NO_PACKED_REFERENCE
#    ifndef __ARM_EABI__
#      define QT_NO_ARM_EABI
#    endif
#  endif

#else
#  error "Qt has not been tested with this compiler"
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

#ifndef Q_PACKED
#  define Q_PACKED
#  undef Q_NO_PACKED_REFERENCE
#endif

#ifndef Q_LIKELY
#  define Q_LIKELY(x) (x)
#endif
#ifndef Q_UNLIKELY
#  define Q_UNLIKELY(x) (x)
#endif
#ifndef Q_ASSUME
#  define Q_ASSUME(expr)
#endif
#ifndef Q_UNREACHABLE
#  define Q_UNREACHABLE()
#endif


#ifndef Q_CONSTRUCTOR_FUNCTION
# define Q_CONSTRUCTOR_FUNCTION0(AFUNC) \
   static const int AFUNC ## __init_variable__ = AFUNC();
# define Q_CONSTRUCTOR_FUNCTION(AFUNC) Q_CONSTRUCTOR_FUNCTION0(AFUNC)
#endif

#ifndef Q_DESTRUCTOR_FUNCTION
# define Q_DESTRUCTOR_FUNCTION0(AFUNC) \
    class AFUNC ## __dest_class__ { \
    public: \
       inline AFUNC ## __dest_class__() { } \
       inline ~ AFUNC ## __dest_class__() { AFUNC(); } \
    } AFUNC ## __dest_instance__;
# define Q_DESTRUCTOR_FUNCTION(AFUNC) Q_DESTRUCTOR_FUNCTION0(AFUNC)
#endif

#ifndef Q_REQUIRED_RESULT
#  if __cplusplus >= 201703L
#    define Q_REQUIRED_RESULT [[nodiscard]]
#  elif defined(Q_CC_GNU) && !defined(Q_CC_INTEL) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1))
#    define Q_REQUIRED_RESULT __attribute__ ((warn_unused_result))
#  else
#    define Q_REQUIRED_RESULT
#  endif
#endif

#ifndef Q_COMPILER_MANGLES_RETURN_TYPE
#  if defined(Q_CC_MSVC)
#    define Q_COMPILER_MANGLES_RETURN_TYPE
#  endif
#endif

/*
   The window system, must be one of: (Q_WS_x)

     MACX     - Mac OS X
     MAC9     - Mac OS 9
     QWS      - Qt for Embedded Linux
     WIN32    - Windows
     X11      - X Window System
     PM       - unsupported
     WIN16    - unsupported
*/

#if defined(_WIN32_X11_)
#  define Q_WS_X11
#elif defined(Q_OS_WIN32)
#  define Q_WS_WIN32
#  if defined(Q_OS_WIN64)
#    define Q_WS_WIN64
#  endif
#elif defined(Q_OS_UNIX)
#  if defined(Q_OS_MAC) && !defined(__USE_WS_X11__) && !defined(Q_WS_QWS) && !defined(Q_WS_QPA)
#    define Q_WS_MAC
#    define Q_WS_MACX
#    if defined(Q_OS_MAC64)
#      define Q_WS_MAC64
#    elif defined(Q_OS_MAC32)
#      define Q_WS_MAC32
#    endif
#  elif !defined(Q_WS_QWS) && !defined(Q_WS_QPA)
#    define Q_WS_X11
#  endif
#endif

#if defined(Q_WS_WIN16) || defined(Q_WS_WIN32)
#  define Q_WS_WIN
#endif

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

/*
   Size-dependent types (architechture-dependent byte order)

   Make sure to update QMetaType when changing these typedefs
*/

typedef signed char qint8;         /* 8 bit signed */
typedef unsigned char quint8;      /* 8 bit unsigned */
typedef short qint16;              /* 16 bit signed */
typedef unsigned short quint16;    /* 16 bit unsigned */
typedef int qint32;                /* 32 bit signed */
typedef unsigned int quint32;      /* 32 bit unsigned */
#if defined(Q_OS_WIN) && !defined(Q_CC_GNU)
#  define Q_INT64_C(c) c ## i64    /* signed 64 bit constant */
#  define Q_UINT64_C(c) c ## ui64   /* unsigned 64 bit constant */
typedef __int64 qint64;            /* 64 bit signed */
typedef unsigned __int64 quint64;  /* 64 bit unsigned */
#else
#  define Q_INT64_C(c) static_cast<long long>(c ## LL)     /* signed 64 bit constant */
#  define Q_UINT64_C(c) static_cast<unsigned long long>(c ## ULL) /* unsigned 64 bit constant */
typedef long long qint64;           /* 64 bit signed */
typedef unsigned long long quint64; /* 64 bit unsigned */
#endif

typedef qint64 qlonglong;
typedef quint64 qulonglong;

#ifndef QT_POINTER_SIZE
#  if defined(Q_OS_WIN64)
#   define QT_POINTER_SIZE 8
#  elif defined(Q_OS_WIN32)
#   define QT_POINTER_SIZE 4
#  endif
#endif

#define Q_INIT_RESOURCE_EXTERN(name) \
    extern int QT_MANGLE_NAMESPACE(qInitResources_ ## name) ();

#define Q_INIT_RESOURCE(name) \
    do { extern int QT_MANGLE_NAMESPACE(qInitResources_ ## name) ();       \
        QT_MANGLE_NAMESPACE(qInitResources_ ## name) (); } while (0)
#define Q_CLEANUP_RESOURCE(name) \
    do { extern int QT_MANGLE_NAMESPACE(qCleanupResources_ ## name) ();    \
        QT_MANGLE_NAMESPACE(qCleanupResources_ ## name) (); } while (0)

#if defined(__cplusplus)

namespace QtPrivate {
    template <class T>
    struct AlignOfHelper
    {
        char c;
        T type;

        AlignOfHelper();
        ~AlignOfHelper();
    };

    template <class T>
    struct AlignOf_Default
    {
        enum { Value = sizeof(AlignOfHelper<T>) - sizeof(T) };
    };

    template <class T> struct AlignOf : AlignOf_Default<T> { };
    template <class T> struct AlignOf<T &> : AlignOf<T> {};
    template <size_t N, class T> struct AlignOf<T[N]> : AlignOf<T> {};

#ifdef Q_COMPILER_RVALUE_REFS
    template <class T> struct AlignOf<T &&> : AlignOf<T> {};
#endif

#if defined(Q_PROCESSOR_X86_32) && !defined(Q_OS_WIN)
    template <class T> struct AlignOf_WorkaroundForI386Abi { enum { Value = sizeof(T) }; };

    // x86 ABI weirdness
    // Alignment of naked type is 8, but inside struct has alignment 4.
    template <> struct AlignOf<double>  : AlignOf_WorkaroundForI386Abi<double> {};
    template <> struct AlignOf<qint64>  : AlignOf_WorkaroundForI386Abi<qint64> {};
    template <> struct AlignOf<quint64> : AlignOf_WorkaroundForI386Abi<quint64> {};
#ifdef Q_CC_CLANG
    // GCC and Clang seem to disagree wrt to alignment of arrays
    template <size_t N> struct AlignOf<double[N]>   : AlignOf_Default<double> {};
    template <size_t N> struct AlignOf<qint64[N]>   : AlignOf_Default<qint64> {};
    template <size_t N> struct AlignOf<quint64[N]>  : AlignOf_Default<quint64> {};
#endif
#endif
} // namespace QtPrivate

#define QT_EMULATED_ALIGNOF(T) \
    (size_t(QT_PREPEND_NAMESPACE(QtPrivate)::AlignOf<T>::Value))

#ifndef Q_ALIGNOF
#define Q_ALIGNOF(T) QT_EMULATED_ALIGNOF(T)
#endif


/*
  quintptr and qptrdiff is guaranteed to be the same size as a pointer, i.e.

      sizeof(void *) == sizeof(quintptr)
      && sizeof(void *) == sizeof(qptrdiff)

  size_t and qsizetype are not guaranteed to be the same size as a pointer, but
  they usually are.
*/
template <int> struct QIntegerForSize;
template <>    struct QIntegerForSize<1> { typedef quint8  Unsigned; typedef qint8  Signed; };
template <>    struct QIntegerForSize<2> { typedef quint16 Unsigned; typedef qint16 Signed; };
template <>    struct QIntegerForSize<4> { typedef quint32 Unsigned; typedef qint32 Signed; };
template <>    struct QIntegerForSize<8> { typedef quint64 Unsigned; typedef qint64 Signed; };
template <class T> struct QIntegerForSizeof: QIntegerForSize<sizeof(T)> { };
using quintptr = QIntegerForSizeof<void*>::Unsigned;
using qptrdiff = QIntegerForSizeof<void*>::Signed;
using qintptr = qptrdiff;
using qsizetype = QIntegerForSizeof<std::size_t>::Signed;

/*
   Useful type definitions for Qt
*/

QT_BEGIN_INCLUDE_NAMESPACE
using uchar = unsigned char;
using ushort = unsigned short;
using uint = unsigned int;
using ulong = unsigned long;
QT_END_INCLUDE_NAMESPACE

#if defined(Q_NO_BOOL_TYPE)
#error "Compiler doesn't support the bool type"
#endif

/*
 * Warning/diagnostic handling
 */

#define QT_DO_PRAGMA(text)                      _Pragma(#text)
#if defined(Q_CC_INTEL)
#  define QT_WARNING_PUSH                       QT_DO_PRAGMA(warning(push))
#  define QT_WARNING_POP                        QT_DO_PRAGMA(warning(pop))
#  define QT_WARNING_DISABLE_INTEL(number)      QT_DO_PRAGMA(warning(disable: number))
#  define QT_WARNING_DISABLE_MSVC(number)
#  define QT_WARNING_DISABLE_CLANG(text)
#  define QT_WARNING_DISABLE_GCC(text)
#elif defined(Q_CC_MSVC) && _MSC_VER >= 1500
#  undef QT_DO_PRAGMA                           /* not needed */
#  define QT_WARNING_PUSH                       __pragma(warning(push))
#  define QT_WARNING_POP                        __pragma(warning(pop))
#  define QT_WARNING_DISABLE_MSVC(number)       __pragma(warning(disable: number))
#  define QT_WARNING_DISABLE_INTEL(number)
#  define QT_WARNING_DISABLE_CLANG(text)
#  define QT_WARNING_DISABLE_GCC(text)
#elif defined(Q_CC_CLANG)
#  define QT_WARNING_PUSH                       QT_DO_PRAGMA(clang diagnostic push)
#  define QT_WARNING_POP                        QT_DO_PRAGMA(clang diagnostic pop)
#  define QT_WARNING_DISABLE_CLANG(text)        QT_DO_PRAGMA(clang diagnostic ignored text)
#  define QT_WARNING_DISABLE_GCC(text)          QT_DO_PRAGMA(GCC diagnostic ignored text)   // GCC directives work in Clang too
#  define QT_WARNING_DISABLE_INTEL(number)
#  define QT_WARNING_DISABLE_MSVC(number)
#elif defined(Q_CC_GNU) && (__GNUC__ * 100 + __GNUC_MINOR__ >= 405)
#  define QT_WARNING_PUSH                       QT_DO_PRAGMA(GCC diagnostic push)
#  define QT_WARNING_POP                        QT_DO_PRAGMA(GCC diagnostic pop)
#  define QT_WARNING_DISABLE_GCC(text)          QT_DO_PRAGMA(GCC diagnostic ignored text)
#  define QT_WARNING_DISABLE_CLANG(text)
#  define QT_WARNING_DISABLE_INTEL(number)
#  define QT_WARNING_DISABLE_MSVC(number)
#else       // All other compilers, GCC < 4.6 and MSVC < 2008
#  define QT_WARNING_DISABLE_GCC(text)
#  define QT_WARNING_PUSH
#  define QT_WARNING_POP
#  define QT_WARNING_DISABLE_INTEL(number)
#  define QT_WARNING_DISABLE_MSVC(number)
#  define QT_WARNING_DISABLE_CLANG(text)
#  define QT_WARNING_DISABLE_GCC(text)
#endif

/*
   Workaround for static const members on MSVC++.
*/

#if defined(Q_CC_MSVC)
#  define QT_STATIC_CONST static
#  define QT_STATIC_CONST_IMPL
#else
#  define QT_STATIC_CONST static const
#  define QT_STATIC_CONST_IMPL const
#endif

/*
   Warnings and errors when using deprecated methods
*/
#if defined(Q_MOC_RUN)
#  define Q_DECL_DEPRECATED Q_DECL_DEPRECATED
#elif __cplusplus >= 201402L
#  define Q_DECL_DEPRECATED [[deprecated]]
#elif (defined(Q_CC_GNU) && !defined(Q_CC_INTEL) && (__GNUC__ - 0 > 3 || (__GNUC__ - 0 == 3 && __GNUC_MINOR__ - 0 >= 2))) || defined(Q_CC_RVCT)
#  define Q_DECL_DEPRECATED __attribute__ ((__deprecated__))
#elif defined(Q_CC_MSVC)
#  define Q_DECL_DEPRECATED __declspec(deprecated)
#  if defined (Q_CC_INTEL)
#    define Q_DECL_VARIABLE_DEPRECATED
#  else
#  endif
#else
#  define Q_DECL_DEPRECATED
#endif
#ifndef Q_DECL_VARIABLE_DEPRECATED
#  define Q_DECL_VARIABLE_DEPRECATED Q_DECL_DEPRECATED
#endif
#ifndef Q_DECL_CONSTRUCTOR_DEPRECATED
#  if defined(Q_MOC_RUN)
#    define Q_DECL_CONSTRUCTOR_DEPRECATED Q_DECL_CONSTRUCTOR_DEPRECATED
#  elif defined(Q_NO_DEPRECATED_CONSTRUCTORS)
#    define Q_DECL_CONSTRUCTOR_DEPRECATED
#  else
#    define Q_DECL_CONSTRUCTOR_DEPRECATED Q_DECL_DEPRECATED
#  endif
#endif

#if defined(QT_NO_DEPRECATED)
/* disable Qt3 support as well */
#  undef QT3_SUPPORT_WARNINGS
#  undef QT3_SUPPORT
#  undef QT_DEPRECATED
#  undef QT_DEPRECATED_VARIABLE
#  undef QT_DEPRECATED_CONSTRUCTOR
#elif defined(QT_DEPRECATED_WARNINGS)
#  ifdef QT3_SUPPORT
/* enable Qt3 support warnings as well */
#    undef QT3_SUPPORT_WARNINGS
#    define QT3_SUPPORT_WARNINGS
#  endif
#  undef QT_DEPRECATED
#  define QT_DEPRECATED Q_DECL_DEPRECATED
#  undef QT_DEPRECATED_VARIABLE
#  define QT_DEPRECATED_VARIABLE Q_DECL_VARIABLE_DEPRECATED
#  undef QT_DEPRECATED_CONSTRUCTOR
#  define QT_DEPRECATED_CONSTRUCTOR explicit Q_DECL_CONSTRUCTOR_DEPRECATED
#else
#  undef QT_DEPRECATED
#  define QT_DEPRECATED
#  undef QT_DEPRECATED_VARIABLE
#  define QT_DEPRECATED_VARIABLE
#  undef QT_DEPRECATED_CONSTRUCTOR
#  define QT_DEPRECATED_CONSTRUCTOR
#endif

#if defined(QT3_SUPPORT_WARNINGS)
#  if !defined(QT_COMPAT_WARNINGS) /* also enable compat */
#    define QT_COMPAT_WARNINGS
#  endif
#  undef QT3_SUPPORT
#  define QT3_SUPPORT Q_DECL_DEPRECATED
#  undef QT3_SUPPORT_VARIABLE
#  define QT3_SUPPORT_VARIABLE Q_DECL_VARIABLE_DEPRECATED
#  undef QT3_SUPPORT_CONSTRUCTOR
#  define QT3_SUPPORT_CONSTRUCTOR explicit Q_DECL_CONSTRUCTOR_DEPRECATED
#elif defined(QT3_SUPPORT) /* define back to nothing */
#  if !defined(QT_COMPAT) /* also enable qt3 support */
#    define QT_COMPAT
#  endif
#  undef QT3_SUPPORT
#  define QT3_SUPPORT
#  undef QT3_SUPPORT_VARIABLE
#  define QT3_SUPPORT_VARIABLE
#  undef QT3_SUPPORT_CONSTRUCTOR
#  define QT3_SUPPORT_CONSTRUCTOR explicit
#endif

/* moc compats (signals/slots) */
#ifndef QT_MOC_COMPAT
#  if defined(QT3_SUPPORT)
#    define QT_MOC_COMPAT QT3_SUPPORT
#  else
#    define QT_MOC_COMPAT
#  endif
#else
#  undef QT_MOC_COMPAT
#  define QT_MOC_COMPAT
#endif

#ifdef QT_ASCII_CAST_WARNINGS
#  define QT_ASCII_CAST_WARN Q_DECL_DEPRECATED
#  if defined(Q_CC_GNU) && __GNUC__ < 4
     /* gcc < 4 doesn't like Q_DECL_DEPRECATED in front of constructors */
#    define QT_ASCII_CAST_WARN_CONSTRUCTOR
#  else
#    define QT_ASCII_CAST_WARN_CONSTRUCTOR Q_DECL_CONSTRUCTOR_DEPRECATED
#  endif
#else
#  define QT_ASCII_CAST_WARN
#  define QT_ASCII_CAST_WARN_CONSTRUCTOR
#endif

#if defined(__i386__) || defined(_WIN32) || defined(_WIN32_WCE)
#  if defined(Q_CC_GNU)
#if !defined(Q_CC_INTEL) && ((100*(__GNUC__ - 0) + 10*(__GNUC_MINOR__ - 0) + __GNUC_PATCHLEVEL__) >= 332)
#    define QT_FASTCALL __attribute__((regparm(3)))
#else
#    define QT_FASTCALL
#endif
#  elif defined(Q_CC_MSVC)
#    define QT_FASTCALL __fastcall
#  else
#     define QT_FASTCALL
#  endif
#else
#  define QT_FASTCALL
#endif

#ifdef Q_COMPILER_CONSTEXPR
# define Q_DECL_CONSTEXPR constexpr
#else
# define Q_DECL_CONSTEXPR
#endif

#ifdef Q_COMPILER_EXPLICIT_OVERRIDES
# define Q_DECL_OVERRIDE override
# define Q_DECL_FINAL final
#else
# define Q_DECL_OVERRIDE
# define Q_DECL_FINAL
#endif

// enable gcc warnings for printf-style functions
#if defined(Q_CC_GNU) && !defined(__INSURE__)
#  if defined(Q_CC_MINGW) && !defined(Q_CC_CLANG)
#    define Q_ATTRIBUTE_FORMAT_PRINTF(A, B) \
         __attribute__((format(gnu_printf, (A), (B))))
#  else
#    define Q_ATTRIBUTE_FORMAT_PRINTF(A, B) \
         __attribute__((format(printf, (A), (B))))
#  endif
#else
#  define Q_ATTRIBUTE_FORMAT_PRINTF(A, B)
#endif

//defines the type for the WNDPROC on windows
//the alignment needs to be forced for sse2 to not crash with mingw
#if defined(Q_WS_WIN)
#  if defined(Q_CC_MINGW)
#    define QT_ENSURE_STACK_ALIGNED_FOR_SSE __attribute__ ((force_align_arg_pointer))
#  else
#    define QT_ENSURE_STACK_ALIGNED_FOR_SSE
#  endif
#  define QT_WIN_CALLBACK CALLBACK QT_ENSURE_STACK_ALIGNED_FOR_SSE
#endif

using QNoImplicitBoolCast = int;

#if defined(QT_ARCH_ARM) || defined(QT_ARCH_ARMV6) || defined(QT_ARCH_AVR32) || (defined(QT_ARCH_MIPS) && (defined(Q_WS_QWS) || defined(Q_WS_QPA))) || defined(QT_ARCH_SH) || defined(QT_ARCH_SH4A)
#define QT_NO_FPU
#endif

// This logic must match the one in qmetatype.h
#if defined(QT_COORD_TYPE)
using qreal = QT_COORD_TYPE;
#elif defined(QT_NO_FPU) || defined(QT_ARCH_ARM)
using qreal = float;
#else
using qreal = double;
#endif

/*
   Utility macros and inline functions
*/

Q_DECL_CONSTEXPR inline int qRound(qreal d)
{ return d >= qreal(0.0) ? int(d + qreal(0.5)) : int(d - int(d-1) + qreal(0.5)) + int(d-1); }

#if defined(QT_NO_FPU) || defined(QT_ARCH_ARM)
Q_DECL_CONSTEXPR inline qint64 qRound64(double d)
{ return d >= 0.0 ? qint64(d + 0.5) : qint64(d - qreal(qint64(d-1)) + 0.5) + qint64(d-1); }
#else
Q_DECL_CONSTEXPR inline qint64 qRound64(qreal d)
{ return d >= qreal(0.0) ? qint64(d + qreal(0.5)) : qint64(d - qreal(qint64(d-1)) + qreal(0.5)) + qint64(d-1); }
#endif

/*
   Data stream functions are provided by many classes (defined in qdatastream.h)
*/

class QDataStream;

#if defined(Q_WS_MAC)
#  ifndef QMAC_QMENUBAR_NO_EVENT
#    define QMAC_QMENUBAR_NO_EVENT
#  endif
#endif

#if !defined(Q_WS_QWS) && !defined(QT_NO_COP)
#  define QT_NO_COP
#endif

# include <QtCore/qfeatures.h>

#define QT_SUPPORTS(FEATURE) (!defined(QT_NO_##FEATURE))

#if defined(Q_OS_LINUX) && defined(Q_CC_RVCT)
#  define Q_DECL_EXPORT     __attribute__((visibility("default")))
#  define Q_DECL_IMPORT     __attribute__((visibility("default")))
#  define Q_DECL_HIDDEN     __attribute__((visibility("hidden")))
#endif

#ifndef Q_DECL_EXPORT
#  if defined(Q_OS_WIN) || defined(Q_CC_RVCT)
#    define Q_DECL_EXPORT __declspec(dllexport)
#  elif defined(QT_VISIBILITY_AVAILABLE)
#    define Q_DECL_EXPORT __attribute__((visibility("default")))
#    define Q_DECL_HIDDEN __attribute__((visibility("hidden")))
#  endif
#  ifndef Q_DECL_EXPORT
#    define Q_DECL_EXPORT
#  endif
#endif
#ifndef Q_DECL_IMPORT
#  if defined(Q_OS_WIN) || defined(Q_CC_RVCT)
#    define Q_DECL_IMPORT __declspec(dllimport)
#  else
#    define Q_DECL_IMPORT
#  endif
#endif
#ifndef Q_DECL_HIDDEN
#  define Q_DECL_HIDDEN
#endif


/*
   Create Qt DLL if QT_DLL is defined (Windows only)
*/

#if defined(Q_OS_WIN)
#  if defined(QT_NODLL)
#    undef QT_MAKEDLL
#    undef QT_DLL
#  elif defined(QT_MAKEDLL)        /* create a Qt DLL library */
#    if defined(QT_DLL)
#      undef QT_DLL
#    endif
#    if defined(QT_BUILD_CORE_LIB)
#      define Q_CORE_EXPORT Q_DECL_EXPORT
#    else
#      define Q_CORE_EXPORT Q_DECL_IMPORT
#    endif
#    if defined(QT_BUILD_GUI_LIB)
#      define Q_GUI_EXPORT Q_DECL_EXPORT
#    else
#      define Q_GUI_EXPORT Q_DECL_IMPORT
#    endif
#    if defined(QT_BUILD_SQL_LIB)
#      define Q_SQL_EXPORT Q_DECL_EXPORT
#    else
#      define Q_SQL_EXPORT Q_DECL_IMPORT
#    endif
#    if defined(QT_BUILD_NETWORK_LIB)
#      define Q_NETWORK_EXPORT Q_DECL_EXPORT
#    else
#      define Q_NETWORK_EXPORT Q_DECL_IMPORT
#    endif
#    if defined(QT_BUILD_SVG_LIB)
#      define Q_SVG_EXPORT Q_DECL_EXPORT
#    else
#      define Q_SVG_EXPORT Q_DECL_IMPORT
#    endif
#    if defined(QT_BUILD_DECLARATIVE_LIB)
#      define Q_DECLARATIVE_EXPORT Q_DECL_EXPORT
#    else
#      define Q_DECLARATIVE_EXPORT Q_DECL_IMPORT
#    endif
#    if defined(QT_BUILD_OPENGL_LIB)
#      define Q_OPENGL_EXPORT Q_DECL_EXPORT
#    else
#      define Q_OPENGL_EXPORT Q_DECL_IMPORT
#    endif
#    if defined(QT_BUILD_XML_LIB)
#      define Q_XML_EXPORT Q_DECL_EXPORT
#    else
#      define Q_XML_EXPORT Q_DECL_IMPORT
#    endif
#    if defined(QT_BUILD_XMLPATTERNS_LIB)
#      define Q_XMLPATTERNS_EXPORT Q_DECL_EXPORT
#    else
#      define Q_XMLPATTERNS_EXPORT Q_DECL_IMPORT
#    endif
#    if defined(QT_BUILD_SCRIPT_LIB)
#      define Q_SCRIPT_EXPORT Q_DECL_EXPORT
#    else
#      define Q_SCRIPT_EXPORT Q_DECL_IMPORT
#    endif
#    if defined(QT_BUILD_SCRIPTTOOLS_LIB)
#      define Q_SCRIPTTOOLS_EXPORT Q_DECL_EXPORT
#    else
#      define Q_SCRIPTTOOLS_EXPORT Q_DECL_IMPORT
#    endif
#    if defined(QT_BUILD_CANVAS_LIB)
#      define Q_CANVAS_EXPORT Q_DECL_EXPORT
#    else
#      define Q_CANVAS_EXPORT Q_DECL_IMPORT
#    endif
#    if defined(QT_BUILD_COMPAT_LIB)
#      define Q_COMPAT_EXPORT Q_DECL_EXPORT
#    else
#      define Q_COMPAT_EXPORT Q_DECL_IMPORT
#    endif
#    if defined(QT_BUILD_DBUS_LIB)
#      define Q_DBUS_EXPORT Q_DECL_EXPORT
#    else
#      define Q_DBUS_EXPORT Q_DECL_IMPORT
#    endif
#    define Q_TEMPLATEDLL
#  elif defined(QT_DLL) /* use a Qt DLL library */
#    define Q_CORE_EXPORT Q_DECL_IMPORT
#    define Q_GUI_EXPORT Q_DECL_IMPORT
#    define Q_SQL_EXPORT Q_DECL_IMPORT
#    define Q_NETWORK_EXPORT Q_DECL_IMPORT
#    define Q_SVG_EXPORT Q_DECL_IMPORT
#    define Q_DECLARATIVE_EXPORT Q_DECL_IMPORT
#    define Q_CANVAS_EXPORT Q_DECL_IMPORT
#    define Q_OPENGL_EXPORT Q_DECL_IMPORT
#    define Q_XML_EXPORT Q_DECL_IMPORT
#    define Q_XMLPATTERNS_EXPORT Q_DECL_IMPORT
#    define Q_SCRIPT_EXPORT Q_DECL_IMPORT
#    define Q_SCRIPTTOOLS_EXPORT Q_DECL_IMPORT
#    define Q_COMPAT_EXPORT Q_DECL_IMPORT
#    define Q_DBUS_EXPORT Q_DECL_IMPORT
#    define Q_TEMPLATEDLL
#  endif
#  define Q_NO_DECLARED_NOT_DEFINED
#else
#  undef QT_MAKEDLL /* ignore these for other platforms */
#  undef QT_DLL
#endif

#if !defined(Q_CORE_EXPORT)
#  if defined(QT_SHARED)
#    define Q_CORE_EXPORT Q_DECL_EXPORT
#    define Q_GUI_EXPORT Q_DECL_EXPORT
#    define Q_SQL_EXPORT Q_DECL_EXPORT
#    define Q_NETWORK_EXPORT Q_DECL_EXPORT
#    define Q_SVG_EXPORT Q_DECL_EXPORT
#    define Q_DECLARATIVE_EXPORT Q_DECL_EXPORT
#    define Q_OPENGL_EXPORT Q_DECL_EXPORT
#    define Q_XML_EXPORT Q_DECL_EXPORT
#    define Q_XMLPATTERNS_EXPORT Q_DECL_EXPORT
#    define Q_SCRIPT_EXPORT Q_DECL_EXPORT
#    define Q_SCRIPTTOOLS_EXPORT Q_DECL_EXPORT
#    define Q_COMPAT_EXPORT Q_DECL_EXPORT
#    define Q_DBUS_EXPORT Q_DECL_EXPORT
#  else
#    define Q_CORE_EXPORT
#    define Q_GUI_EXPORT
#    define Q_SQL_EXPORT
#    define Q_NETWORK_EXPORT
#    define Q_SVG_EXPORT
#    define Q_DECLARATIVE_EXPORT
#    define Q_OPENGL_EXPORT
#    define Q_XML_EXPORT
#    define Q_XMLPATTERNS_EXPORT
#    define Q_SCRIPT_EXPORT
#    define Q_SCRIPTTOOLS_EXPORT
#    define Q_COMPAT_EXPORT
#    define Q_DBUS_EXPORT
#  endif
#endif

// Functions marked as Q_GUI_EXPORT_INLINE were exported and inlined by mistake.
// Compilers like MinGW complain that the import attribute is ignored.
#if defined(Q_CC_MINGW)
#    if defined(QT_BUILD_CORE_LIB)
#      define Q_CORE_EXPORT_INLINE Q_CORE_EXPORT inline
#    else
#      define Q_CORE_EXPORT_INLINE inline
#    endif
#    if defined(QT_BUILD_GUI_LIB)
#      define Q_GUI_EXPORT_INLINE Q_GUI_EXPORT inline
#    else
#      define Q_GUI_EXPORT_INLINE inline
#    endif
#    if defined(QT_BUILD_COMPAT_LIB)
#      define Q_COMPAT_EXPORT_INLINE Q_COMPAT_EXPORT inline
#    else
#      define Q_COMPAT_EXPORT_INLINE inline
#    endif
#elif defined(Q_CC_RVCT)
// we force RVCT not to export inlines by passing --visibility_inlines_hidden
// so we need to just inline it, rather than exporting and inlining
// note: this affects the contents of the DEF files (ie. these functions do not appear)
#    define Q_CORE_EXPORT_INLINE inline
#    define Q_GUI_EXPORT_INLINE inline
#    define Q_COMPAT_EXPORT_INLINE inline
#else
#    define Q_CORE_EXPORT_INLINE Q_CORE_EXPORT inline
#    define Q_GUI_EXPORT_INLINE Q_GUI_EXPORT inline
#    define Q_COMPAT_EXPORT_INLINE Q_COMPAT_EXPORT inline
#endif

/*
   No, this is not an evil backdoor. QT_BUILD_INTERNAL just exports more symbols
   for Qt's internal unit tests. If you want slower loading times and more
   symbols that can vanish from version to version, feel free to define QT_BUILD_INTERNAL.
*/
#if defined(QT_BUILD_INTERNAL) && (defined(Q_OS_WIN)) && defined(QT_MAKEDLL)
#    define Q_AUTOTEST_EXPORT Q_DECL_EXPORT
#elif defined(QT_BUILD_INTERNAL) && (defined(Q_OS_WIN)) && defined(QT_DLL)
#    define Q_AUTOTEST_EXPORT Q_DECL_IMPORT
#elif defined(QT_BUILD_INTERNAL) && !(defined(Q_OS_WIN)) && defined(QT_SHARED)
#    define Q_AUTOTEST_EXPORT Q_DECL_EXPORT
#else
#    define Q_AUTOTEST_EXPORT
#endif

inline void qt_noop(void) {}

/* These wrap try/catch so we can switch off exceptions later.

   Beware - do not use more than one QT_CATCH per QT_TRY, and do not use
   the exception instance in the catch block.
   If you can't live with those constraints, don't use these macros.
   Use the QT_NO_EXCEPTIONS macro to protect your code instead.
*/

#if !defined(QT_NO_EXCEPTIONS)
#  if defined(QT_BOOTSTRAPPED) || (defined(Q_CC_GNU) && !defined (__EXCEPTIONS) && !defined(Q_MOC_RUN))
#    define QT_NO_EXCEPTIONS
#  endif
#endif

#ifdef QT_NO_EXCEPTIONS
#  define QT_TRY if (true)
#  define QT_CATCH(A) else
#  define QT_THROW(A) qt_noop()
#  define QT_RETHROW qt_noop()
#else
#  define QT_TRY try
#  define QT_CATCH(A) catch (A)
#  define QT_THROW(A) throw A
#  define QT_RETHROW throw
#endif

Q_CORE_EXPORT const char *qVersion();
Q_CORE_EXPORT bool qSharedBuild();

#if defined(Q_OS_MAC)
inline int qMacVersion() { return QSysInfo::MacintoshVersion; }
#endif

#if defined(Q_WS_WIN) || defined(Q_OS_CYGWIN)

// ### Qt 5: remove Win9x support macros QT_WA and QT_WA_INLINE.
#define QT_WA(unicode, ansi) unicode
#define QT_WA_INLINE(unicode, ansi) (unicode)

#endif /* Q_WS_WIN */

#ifndef Q_OUTOFLINE_TEMPLATE
#  define Q_OUTOFLINE_TEMPLATE
#endif
#ifndef Q_INLINE_TEMPLATE
#  define Q_INLINE_TEMPLATE inline
#endif

/*
   Avoid "unused parameter" warnings
*/
#define Q_UNUSED(x) (void)x;

/*
   Debugging and error handling
*/

#if !defined(QT_NO_DEBUG) && !defined(QT_DEBUG)
#  define QT_DEBUG
#endif

#ifndef qPrintable
#  define qPrintable(string) QtPrivate::asString(string).toLocal8Bit().constData()
#endif

#ifndef qUtf8Printable
#  define qUtf8Printable(string) QtPrivate::asString(string).toUtf8().constData()
#endif

Q_CORE_EXPORT void qDebug(const char *, ...) /* print debug message */
#if defined(Q_CC_GNU) && !defined(__INSURE__)
    __attribute__ ((format (printf, 1, 2)))
#endif
;

Q_CORE_EXPORT void qWarning(const char *, ...) /* print warning message */
#if defined(Q_CC_GNU) && !defined(__INSURE__)
    __attribute__ ((format (printf, 1, 2)))
#endif
;

class QString;
Q_CORE_EXPORT QString qt_error_string(int errorCode = -1);
Q_CORE_EXPORT void qCritical(const char *, ...) /* print critical message */
#if defined(Q_CC_GNU) && !defined(__INSURE__)
    __attribute__ ((format (printf, 1, 2)))
#endif
;
Q_CORE_EXPORT void qFatal(const char *, ...) /* print fatal message and exit */
#if defined(Q_CC_GNU) && !defined(__INSURE__)
    __attribute__ ((format (printf, 1, 2)))
#endif
;

Q_CORE_EXPORT void qErrnoWarning(int code, const char *msg, ...);
Q_CORE_EXPORT void qErrnoWarning(const char *msg, ...);

#if (defined(QT_NO_DEBUG_OUTPUT) || defined(QT_NO_TEXTSTREAM)) && !defined(QT_NO_DEBUG_STREAM)
#define QT_NO_DEBUG_STREAM
#endif

/*
  Forward declarations only.

  In order to use the qDebug() stream, you must #include<QDebug>
*/
class QDebug;
class QNoDebug;
#ifndef QT_NO_DEBUG_STREAM
Q_CORE_EXPORT_INLINE QDebug qDebug();
#ifndef QT_NO_WARNING_OUTPUT
Q_CORE_EXPORT_INLINE QDebug qWarning();
#endif
Q_CORE_EXPORT_INLINE QDebug qCritical();
#else
inline QNoDebug qDebug();
#endif

#ifdef QT_NO_WARNING_OUTPUT
inline QNoDebug qWarning();
#endif

#define QT_NO_QDEBUG_MACRO while (false) qDebug
#ifdef QT_NO_DEBUG_OUTPUT
#  define qDebug QT_NO_QDEBUG_MACRO
#endif
#define QT_NO_QWARNING_MACRO while (false) qWarning
#ifdef QT_NO_WARNING_OUTPUT
#  define qWarning QT_NO_QWARNING_MACRO
#endif


Q_CORE_EXPORT void qt_assert(const char *assertion, const char *file, int line);

#if !defined(Q_ASSERT)
#  ifndef QT_NO_DEBUG
#    define Q_ASSERT(cond) ((!(cond)) ? qt_assert(#cond,__FILE__,__LINE__) : qt_noop())
#  else
#    define Q_ASSERT(cond) qt_noop()
#  endif
#endif

#if defined(QT_NO_DEBUG) && !defined(QT_PAINT_DEBUG)
#define QT_NO_PAINT_DEBUG
#endif

Q_CORE_EXPORT void qt_assert_x(const char *where, const char *what, const char *file, int line);

#if !defined(Q_ASSERT_X)
#  ifndef QT_NO_DEBUG
#    define Q_ASSERT_X(cond, where, what) ((!(cond)) ? qt_assert_x(where, what,__FILE__,__LINE__) : qt_noop())
#  else
#    define Q_ASSERT_X(cond, where, what) qt_noop()
#  endif
#endif

Q_CORE_EXPORT void qt_check_pointer(const char *, int);
Q_CORE_EXPORT void qBadAlloc();

#ifdef QT_NO_EXCEPTIONS
#  if defined(QT_NO_DEBUG)
#    define Q_CHECK_PTR(p) qt_noop()
#  else
#    define Q_CHECK_PTR(p) do {if(!(p))qt_check_pointer(__FILE__,__LINE__);} while (0)
#  endif
#else
#  define Q_CHECK_PTR(p) do { if (!(p)) qBadAlloc(); } while (0)
#endif

template <typename T>
inline T *q_check_ptr(T *p) { Q_CHECK_PTR(p); return p; }

#ifdef __GNUC__
#  define Q_UNREACHABLE() __builtin_unreachable()
#else
#  define Q_UNREACHABLE() Q_ASSERT(false);
#endif

#if defined(Q_CC_GNU)
#  define Q_FUNC_INFO __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#  define Q_FUNC_INFO __FUNCSIG__
#else
#  define Q_FUNC_INFO __FILE__ ":" QT_STRINGIFY(__LINE__)
#endif

enum QtMsgType { QtDebugMsg, QtWarningMsg, QtCriticalMsg, QtFatalMsg, QtSystemMsg = QtCriticalMsg };

Q_CORE_EXPORT void qt_message_output(QtMsgType, const char *buf);

using QtMsgHandler = void (*)(QtMsgType, const char*);
Q_CORE_EXPORT QtMsgHandler qInstallMsgHandler(QtMsgHandler);

#if defined(QT_NO_THREAD)

template <typename T>
class QGlobalStatic
{
public:
    T *pointer;
    inline QGlobalStatic(T *p) : pointer(p) { }
    inline ~QGlobalStatic() { pointer = nullptr; }
};

#define Q_GLOBAL_STATIC(TYPE, NAME)                                  \
    static TYPE *NAME()                                              \
    {                                                                \
        static TYPE thisVariable;                                    \
        static QGlobalStatic<TYPE > thisGlobalStatic(&thisVariable); \
        return thisGlobalStatic.pointer;                             \
    }

#define Q_GLOBAL_STATIC_WITH_ARGS(TYPE, NAME, ARGS)                  \
    static TYPE *NAME()                                              \
    {                                                                \
        static TYPE thisVariable ARGS;                               \
        static QGlobalStatic<TYPE > thisGlobalStatic(&thisVariable); \
        return thisGlobalStatic.pointer;                             \
    }

#define Q_GLOBAL_STATIC_WITH_INITIALIZER(TYPE, NAME, INITIALIZER)    \
    static TYPE *NAME()                                              \
    {                                                                \
        static TYPE thisVariable;                                    \
        static QGlobalStatic<TYPE > thisGlobalStatic(0);             \
        if (!thisGlobalStatic.pointer) {                             \
            TYPE *x = thisGlobalStatic.pointer = &thisVariable;      \
            INITIALIZER;                                             \
        }                                                            \
        return thisGlobalStatic.pointer;                             \
    }

#else

// forward declaration, since qatomic.h needs qglobal.h
template <typename T> class QBasicAtomicPointer;

// POD for Q_GLOBAL_STATIC
template <typename T>
class QGlobalStatic
{
public:
    QBasicAtomicPointer<T> pointer;
    bool destroyed;
};

// Created as a function-local static to delete a QGlobalStatic<T>
template <typename T>
class QGlobalStaticDeleter
{
public:
    QGlobalStatic<T> &globalStatic;
    QGlobalStaticDeleter(QGlobalStatic<T> &_globalStatic)
        : globalStatic(_globalStatic)
    { }

    inline ~QGlobalStaticDeleter()
    {
        delete globalStatic.pointer.loadRelaxed();
        globalStatic.pointer.storeRelaxed(nullptr);
        globalStatic.destroyed = true;
    }
};

#define Q_GLOBAL_STATIC_INIT(TYPE, NAME)                                      \
        static QGlobalStatic<TYPE > this_ ## NAME                             \
                            = { Q_BASIC_ATOMIC_INITIALIZER(nullptr), false }

#define Q_GLOBAL_STATIC(TYPE, NAME)                                           \
    static TYPE *NAME()                                                       \
    {                                                                         \
        Q_GLOBAL_STATIC_INIT(TYPE, _StaticVar_);                              \
        if (!this__StaticVar_.pointer.loadRelaxed() && !this__StaticVar_.destroyed) { \
            TYPE *x = new TYPE;                                               \
            if (!this__StaticVar_.pointer.testAndSetOrdered(nullptr, x))            \
                delete x;                                                     \
            else                                                              \
                static QGlobalStaticDeleter<TYPE > cleanup(this__StaticVar_); \
        }                                                                     \
        return this__StaticVar_.pointer.loadRelaxed();                               \
    }

#define Q_GLOBAL_STATIC_WITH_ARGS(TYPE, NAME, ARGS)                           \
    static TYPE *NAME()                                                       \
    {                                                                         \
        Q_GLOBAL_STATIC_INIT(TYPE, _StaticVar_);                              \
        if (!this__StaticVar_.pointer.loadRelaxed() && !this__StaticVar_.destroyed) { \
            TYPE *x = new TYPE ARGS;                                          \
            if (!this__StaticVar_.pointer.testAndSetOrdered(0, x))            \
                delete x;                                                     \
            else                                                              \
                static QGlobalStaticDeleter<TYPE > cleanup(this__StaticVar_); \
        }                                                                     \
        return this__StaticVar_.pointer.loadRelaxed();                               \
    }

#define Q_GLOBAL_STATIC_WITH_INITIALIZER(TYPE, NAME, INITIALIZER)             \
    static TYPE *NAME()                                                       \
    {                                                                         \
        Q_GLOBAL_STATIC_INIT(TYPE, _StaticVar_);                              \
        if (!this__StaticVar_.pointer.loadRelaxed() && !this__StaticVar_.destroyed) { \
            QScopedPointer<TYPE > x(new TYPE);                                \
            INITIALIZER;                                                      \
            if (this__StaticVar_.pointer.testAndSetOrdered(0, x.data())) {    \
                static QGlobalStaticDeleter<TYPE > cleanup(this__StaticVar_); \
                x.take();                                                     \
            }                                                                 \
        }                                                                     \
        return this__StaticVar_.pointer.loadRelaxed();                               \
    }

#endif

[[nodiscard]] constexpr static inline bool qFuzzyCompare(double p1, double p2)
{
    return (std::abs(p1 - p2) <= 0.000000000001 * std::min(std::abs(p1), std::abs(p2)));
}

[[nodiscard]] constexpr static inline bool qFuzzyCompare(float p1, float p2)
{
    return (std::abs(p1 - p2) <= 0.00001f * std::min(std::abs(p1), std::abs(p2)));
}

/*!
  \internal
*/
[[nodiscard]] constexpr static inline bool qFuzzyIsNull(double d)
{
    return std::abs(d) <= 0.000000000001;
}

/*!
  \internal
*/
[[nodiscard]] constexpr static inline bool qFuzzyIsNull(float f)
{
    return std::abs(f) <= 0.00001f;
}

/*
   This function tests a double for a null value. It doesn't
   check whether the actual value is 0 or close to 0, but whether
   it is binary 0.
*/
[[nodiscard]] static inline bool qIsNull(double d)
{
    union U {
        double d;
        quint64 u;
    };
    U val;
    val.d = d;
    return val.u == quint64(0);
}

/*
   This function tests a float for a null value. It doesn't
   check whether the actual value is 0 or close to 0, but whether
   it is binary 0.
*/
[[nodiscard]] static inline bool qIsNull(float f)
{
    union U {
        float f;
        quint32 u;
    };
    U val;
    val.f = f;
    return val.u == 0u;
}

/*
   Compilers which follow outdated template instantiation rules
   require a class to have a comparison operator to exist when
   a QList of this type is instantiated. It's not actually
   used in the list, though. Hence the dummy implementation.
   Just in case other code relies on it we better trigger a warning
   mandating a real implementation.
*/

#ifdef Q_FULL_TEMPLATE_INSTANTIATION
#  define Q_DUMMY_COMPARISON_OPERATOR(C) \
    bool operator==(const C&) const { \
        qWarning(#C"::operator==(const "#C"&) was called"); \
        return false; \
    }
#else
#  define Q_DUMMY_COMPARISON_OPERATOR(C)
#endif


template <typename T>
//[[deprecated("Use std::swap")]]
inline void qSwap(T &value1, T &value2)
{
    using std::swap;
    swap(value1, value2);
}

Q_CORE_EXPORT void *qMallocAligned(size_t size, size_t alignment);
Q_CORE_EXPORT void *qReallocAligned(void *ptr, size_t size, size_t oldsize, size_t alignment);
Q_CORE_EXPORT void qFreeAligned(void *ptr);


/*
   Avoid some particularly useless warnings from some stupid compilers.
   To get ALL C++ compiler warnings, define QT_CC_WARNINGS or comment out
   the line "#define QT_NO_WARNINGS".
*/
#if !defined(QT_CC_WARNINGS)
#  define QT_NO_WARNINGS
#endif
#if defined(QT_NO_WARNINGS)
#  if defined(Q_CC_MSVC)
#    pragma warning(disable: 4251) /* class 'A' needs to have dll interface for to be used by clients of class 'B'. */
#    pragma warning(disable: 4244) /* 'conversion' conversion from 'type1' to 'type2', possible loss of data */
#    pragma warning(disable: 4275) /* non - DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier' */
#    pragma warning(disable: 4514) /* unreferenced inline/local function has been removed */
#    pragma warning(disable: 4800) /* 'type' : forcing value to bool 'true' or 'false' (performance warning) */
#    pragma warning(disable: 4097) /* typedef-name 'identifier1' used as synonym for class-name 'identifier2' */
#    pragma warning(disable: 4706) /* assignment within conditional expression */
#    pragma warning(disable: 4786) /* truncating debug info after 255 characters */
#    pragma warning(disable: 4660) /* template-class specialization 'identifier' is already instantiated */
#    pragma warning(disable: 4355) /* 'this' : used in base member initializer list */
#    pragma warning(disable: 4231) /* nonstandard extension used : 'extern' before template explicit instantiation */
#    pragma warning(disable: 4710) /* function not inlined */
#    pragma warning(disable: 4530) /* C++ exception handler used, but unwind semantics are not enabled. Specify -GX */
#  endif
#endif

#if defined(Q_CC_GNU)
/* make use of typeof-extension */
template <typename T>
class QForeachContainer {
public:
    inline QForeachContainer(const T& t) : c(t), i(c.begin()), e(c.end()), control(1) { }
    const T c;
    typename T::const_iterator i, e;
    int control;
};

// Explanation of the control word:
//  - it's initialized to 1
//  - that means both the inner and outer loops start
//  - if there were no breaks, at the end of the inner loop, it's set to 0, which
//    causes it to exit (the inner loop is run exactly once)
//  - at the end of the outer loop, it's inverted, so it becomes 1 again, allowing
//    the outer loop to continue executing
//  - if there was a break inside the inner loop, it will exit with control still
//    set to 1; in that case, the outer loop will invert it to 0 and will exit too
#define Q_FOREACH(variable, container)                                  \
for (QForeachContainer<__typeof__(container)> _container_(container);   \
     _container_.control && _container_.i != _container_.e;             \
     ++_container_.i, _container_.control ^= 1)                         \
for (variable = *_container_.i; _container_.control; _container_.control = 0)

#else

struct QForeachContainerBase {};

template <typename T>
class QForeachContainer : public QForeachContainerBase {
public:
    inline QForeachContainer(const T& t): c(t), brk(0), i(c.begin()), e(c.end()){};
    const T c;
    mutable int brk;
    mutable typename T::const_iterator i, e;
    inline bool condition() const { return (!brk++ && i != e); }
};

template <typename T> inline T *qForeachPointer(const T &) { return 0; }

template <typename T> inline QForeachContainer<T> qForeachContainerNew(const T& t)
{ return QForeachContainer<T>(t); }

template <typename T>
inline const QForeachContainer<T> *qForeachContainer(const QForeachContainerBase *base, const T *)
{ return static_cast<const QForeachContainer<T> *>(base); }

#  define Q_FOREACH(variable, container) \
    for (const QForeachContainerBase &_container_ = qForeachContainerNew(container); \
         qForeachContainer(&_container_, true ? 0 : qForeachPointer(container))->condition();       \
         ++qForeachContainer(&_container_, true ? 0 : qForeachPointer(container))->i)               \
        for (variable = *qForeachContainer(&_container_, true ? 0 : qForeachPointer(container))->i; \
             qForeachContainer(&_container_, true ? 0 : qForeachPointer(container))->brk;           \
             --qForeachContainer(&_container_, true ? 0 : qForeachPointer(container))->brk)

#endif

#define Q_FOREVER for(;;)
#ifndef QT_NO_KEYWORDS
#  ifndef foreach
#    define foreach Q_FOREACH
#  endif
#  ifndef forever
#    define forever Q_FOREVER
#  endif
#endif

template <typename T> static inline T *qGetPtrHelper(T *ptr) { return ptr; }
template <typename Wrapper> static inline typename Wrapper::pointer qGetPtrHelper(const Wrapper &p) { return p.data(); }

#define Q_DECLARE_PRIVATE(Class) \
    inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(qGetPtrHelper(d_ptr)); } \
    inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(qGetPtrHelper(d_ptr)); } \
    friend class Class##Private;

#define Q_DECLARE_PRIVATE_D(Dptr, Class) \
    inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(Dptr); } \
    inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(Dptr); } \
    friend class Class##Private;

#define Q_DECLARE_PUBLIC(Class)                                    \
    inline Class* q_func() { return static_cast<Class *>(q_ptr); } \
    inline const Class* q_func() const { return static_cast<const Class *>(q_ptr); } \
    friend class Class;

#define Q_D(Class) Class##Private * const d = d_func()
#define Q_Q(Class) Class * const q = q_func()

#define QT_TR_NOOP(x) x
#define QT_TR_NOOP_UTF8(x) x
#define QT_TRANSLATE_NOOP(scope, x) x
#define QT_TRANSLATE_NOOP_UTF8(scope, x) x
#define QT_TRANSLATE_NOOP3(scope, x, comment) {x, comment}
#define QT_TRANSLATE_NOOP3_UTF8(scope, x, comment) {x, comment}

#ifndef QT_NO_TRANSLATION // ### This should enclose the NOOPs above

// Defined in qcoreapplication.cpp
// The better name qTrId() is reserved for an upcoming function which would
// return a much more powerful QStringFormatter instead of a QString.
Q_CORE_EXPORT QString qtTrId(const char *id, int n = -1);

#define QT_TRID_NOOP(id) id

#endif // QT_NO_TRANSLATION

#define QDOC_PROPERTY(text)

/*
   When RTTI is not available, define this macro to force any uses of
   dynamic_cast to cause a compile failure.
*/

#ifdef QT_NO_DYNAMIC_CAST
#  define dynamic_cast QT_PREPEND_NAMESPACE(qt_dynamic_cast_check)

  template<typename T, typename X>
  T qt_dynamic_cast_check(X, T* = 0)
  { return T::dynamic_cast_will_always_fail_because_rtti_is_disabled; }
#endif

/*
   Some classes do not permit copies to be made of an object. These
   classes contains a private copy constructor and assignment
   operator to disable copying (the compiler gives an error message).
*/
#define Q_DISABLE_COPY(Class) \
    Class(const Class &) = delete; \
    Class &operator=(const Class &) = delete;

class QByteArray;
Q_CORE_EXPORT QByteArray qgetenv(const char *varName);
Q_CORE_EXPORT bool qputenv(const char *varName, const QByteArray& value);
Q_CORE_EXPORT bool qunsetenv(const char *varName);

Q_CORE_EXPORT bool qEnvironmentVariableIsEmpty(const char *varName) noexcept;
Q_CORE_EXPORT bool qEnvironmentVariableIsSet(const char *varName) noexcept;

inline int qIntCast(double f) { return int(f); }
inline int qIntCast(float f) { return int(f); }

/*
  Reentrant versions of basic rand() functions for random number generation
*/
Q_CORE_EXPORT void qsrand(uint seed);
Q_CORE_EXPORT int qrand();

#define QT_MODULE(x)

#ifdef QT_NO_CONCURRENT
#  define QT_NO_QFUTURE
#endif

// gcc 3 version has problems with some of the
// map/filter overloads.
#if defined(Q_CC_GNU) && (__GNUC__ < 4)
#  define QT_NO_CONCURRENT_MAP
#  define QT_NO_CONCURRENT_FILTER
#endif

#if defined (__ELF__)
#  if defined (Q_OS_LINUX) || defined (Q_OS_FREEBSD) || defined (Q_OS_OPENBSD)
#    define Q_OF_ELF
#  endif
#endif

#if !(defined(Q_WS_WIN)) \
    && !(defined(Q_WS_MAC) && defined(QT_MAC_USE_COCOA)) \
    && !(defined(Q_WS_X11) && !defined(QT_NO_FREETYPE)) \
    && !(defined(Q_WS_QPA))
#  define QT_NO_RAWFONT
#endif

#if defined(QT_REDUCE_RELOCATIONS) && defined(__ELF__) && !defined(__PIC__)
#  error "You must build your code with position independent code if Qt was built with -reduce-relocations. "\
         "Compile your code with -fPIC or -fPIE."
#endif

namespace QtPrivate {
//like std::enable_if
template <bool B, typename T = void> struct QEnableIf;
template <typename T> struct QEnableIf<true, T> { typedef T Type; };

template <bool B, typename T, typename F> struct QConditional { typedef T Type; };
template <typename T, typename F> struct QConditional<false, T, F> { typedef F Type; };
}

QT_END_NAMESPACE
QT_END_HEADER

#include <QtCore/qflags.h>
#include <QtCore/qsysinfo.h>
#include <QtCore/qtypeinfo.h>

#endif /* __cplusplus */

#endif /* QGLOBAL_H */

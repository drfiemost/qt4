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

#ifndef QENDIAN_H
#define QENDIAN_H

#include <QtCore/qglobal.h>

// include stdlib.h and hope that it defines __GLIBC__ for glibc-based systems
#include <stdlib.h>

#ifdef __GLIBC__
#include <byteswap.h>
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


/*
 * ENDIAN FUNCTIONS
*/
inline void qbswap_helper(const uchar *src, uchar *dest, int size)
{
    for (int i = 0; i < size ; ++i) dest[i] = src[size - 1 - i];
}

/*
 * qbswap(const T src, const uchar *dest);
 * Changes the byte order of \a src from big endian to little endian or vice versa
 * and stores the result in \a dest.
 * There is no alignment requirements for \a dest.
*/
template <typename T> inline void qbswap(const T src, uchar *dest)
{
    qbswap_helper(reinterpret_cast<const uchar *>(&src), dest, sizeof(T));
}

// Used to implement a type-safe and alignment-safe copy operation
// If you want to avoid the memcopy, you must write specializations for this function
template <typename T> inline void qToUnaligned(const T src, uchar *dest)
{
    memcpy(dest, &src, sizeof(T));
}

/* T qFromLittleEndian(const uchar *src)
 * This function will read a little-endian encoded value from \a src
 * and return the value in host-endian encoding.
 * There is no requirement that \a src must be aligned.
*/
template <typename T> inline T qFromLittleEndian(const uchar *src);
template <> inline quint64 qFromLittleEndian<quint64>(const uchar *src)
{
    return 0
        | src[0]
        | src[1] * Q_UINT64_C(0x0000000000000100)
        | src[2] * Q_UINT64_C(0x0000000000010000)
        | src[3] * Q_UINT64_C(0x0000000001000000)
        | src[4] * Q_UINT64_C(0x0000000100000000)
        | src[5] * Q_UINT64_C(0x0000010000000000)
        | src[6] * Q_UINT64_C(0x0001000000000000)
        | src[7] * Q_UINT64_C(0x0100000000000000);
}

template <> inline quint32 qFromLittleEndian<quint32>(const uchar *src)
{
    return 0
        | src[0]
        | src[1] * quint32(0x00000100)
        | src[2] * quint32(0x00010000)
        | src[3] * quint32(0x01000000);
}

template <> inline quint16 qFromLittleEndian<quint16>(const uchar *src)
{
    return quint16(0
                   | src[0]
                   | src[1] * 0x0100);
}

// signed specializations
template <> inline qint64 qFromLittleEndian<qint64>(const uchar *src)
{ return static_cast<qint64>(qFromLittleEndian<quint64>(src)); }

template <> inline qint32 qFromLittleEndian<qint32>(const uchar *src)
{ return static_cast<qint32>(qFromLittleEndian<quint32>(src)); }

template <> inline qint16 qFromLittleEndian<qint16>(const uchar *src)
{ return static_cast<qint16>(qFromLittleEndian<quint16>(src)); }


/* This function will read a big-endian (also known as network order) encoded value from \a src
 * and return the value in host-endian encoding.
 * There is no requirement that \a src must be aligned.
*/
template <class T> inline T qFromBigEndian(const uchar *src);
template<>
inline quint64 qFromBigEndian<quint64>(const uchar *src)
{
    return 0
        | src[7]
        | src[6] * Q_UINT64_C(0x0000000000000100)
        | src[5] * Q_UINT64_C(0x0000000000010000)
        | src[4] * Q_UINT64_C(0x0000000001000000)
        | src[3] * Q_UINT64_C(0x0000000100000000)
        | src[2] * Q_UINT64_C(0x0000010000000000)
        | src[1] * Q_UINT64_C(0x0001000000000000)
        | src[0] * Q_UINT64_C(0x0100000000000000);
}

template<>
inline quint32 qFromBigEndian<quint32>(const uchar *src)
{
    return 0
        | src[3]
        | src[2] * quint32(0x00000100)
        | src[1] * quint32(0x00010000)
        | src[0] * quint32(0x01000000);
}

template<>
inline quint16 qFromBigEndian<quint16>(const uchar *src)
{
    return quint16( 0
                    | src[1]
                    | src[0] * quint16(0x0100));
}


// signed specializations
template <> inline qint64 qFromBigEndian<qint64>(const uchar *src)
{ return static_cast<qint64>(qFromBigEndian<quint64>(src)); }

template <> inline qint32 qFromBigEndian<qint32>(const uchar *src)
{ return static_cast<qint32>(qFromBigEndian<quint32>(src)); }

template <> inline qint16 qFromBigEndian<qint16>(const uchar *src)
{ return static_cast<qint16>(qFromBigEndian<quint16>(src)); }

/*
 * T qbswap(T source).
 * Changes the byte order of a value from big endian to little endian or vice versa.
 * This function can be used if you are not concerned about alignment issues,
 * and it is therefore a bit more convenient and in most cases more efficient.
*/
template <typename T> T qbswap(T source);

#ifdef __GLIBC__
template <> inline quint64 qbswap<quint64>(quint64 source)
{
    return bswap_64(source);
}
template <> inline quint32 qbswap<quint32>(quint32 source)
{
    return bswap_32(source);
}
template <> inline quint16 qbswap<quint16>(quint16 source)
{
    return bswap_16(source);
}
#else
template <> inline quint64 qbswap<quint64>(quint64 source)
{
    return 0
        | ((source & Q_UINT64_C(0x00000000000000ff)) << 56)
        | ((source & Q_UINT64_C(0x000000000000ff00)) << 40)
        | ((source & Q_UINT64_C(0x0000000000ff0000)) << 24)
        | ((source & Q_UINT64_C(0x00000000ff000000)) << 8)
        | ((source & Q_UINT64_C(0x000000ff00000000)) >> 8)
        | ((source & Q_UINT64_C(0x0000ff0000000000)) >> 24)
        | ((source & Q_UINT64_C(0x00ff000000000000)) >> 40)
        | ((source & Q_UINT64_C(0xff00000000000000)) >> 56);
}

template <> inline quint32 qbswap<quint32>(quint32 source)
{
    return 0
        | ((source & 0x000000ff) << 24)
        | ((source & 0x0000ff00) << 8)
        | ((source & 0x00ff0000) >> 8)
        | ((source & 0xff000000) >> 24);
}

template <> inline quint16 qbswap<quint16>(quint16 source)
{
    return quint16( 0
                    | ((source & 0x00ff) << 8)
                    | ((source & 0xff00) >> 8) );
}
#endif // __GLIBC__

// signed specializations
template <> inline qint64 qbswap<qint64>(qint64 source)
{
    return qbswap<quint64>(quint64(source));
}

template <> inline qint32 qbswap<qint32>(qint32 source)
{
    return qbswap<quint32>(quint32(source));
}

template <> inline qint16 qbswap<qint16>(qint16 source)
{
    return qbswap<quint16>(quint16(source));
}

#if Q_BYTE_ORDER == Q_BIG_ENDIAN

template <typename T> inline T qToBigEndian(T source)
{ return source; }
template <typename T> inline T qFromBigEndian(T source)
{ return source; }
template <typename T> inline T qToLittleEndian(T source)
{ return qbswap<T>(source); }
template <typename T> inline T qFromLittleEndian(T source)
{ return qbswap<T>(source); }
template <typename T> inline void qToBigEndian(T src, uchar *dest)
{ qToUnaligned<T>(src, dest); }
template <typename T> inline void qToLittleEndian(T src, uchar *dest)
{ qbswap<T>(src, dest); }
#else // Q_LITTLE_ENDIAN

template <typename T> inline T qToBigEndian(T source)
{ return qbswap<T>(source); }
template <typename T> inline T qFromBigEndian(T source)
{ return qbswap<T>(source); }
template <typename T> inline T qToLittleEndian(T source)
{ return source; }
template <typename T> inline T qFromLittleEndian(T source)
{ return source; }
template <typename T> inline void qToBigEndian(T src, uchar *dest)
{ qbswap<T>(src, dest); }
template <typename T> inline void qToLittleEndian(T src, uchar *dest)
{ qToUnaligned<T>(src, dest); }

#endif // Q_BYTE_ORDER == Q_BIG_ENDIAN

template <> inline quint8 qbswap<quint8>(quint8 source)
{
    return source;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QENDIAN_H

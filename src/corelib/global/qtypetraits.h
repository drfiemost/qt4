/****************************************************************************
**
** Copyright (C) 2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author Marc Mutz <marc.mutz@kdab.com>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "QtCore/qglobal.h"

#ifndef QTYPETRAITS_H
#define QTYPETRAITS_H

QT_BEGIN_NAMESPACE

namespace QtPrivate {

//
// define custom is_signed, is_unsigned that also works with enum's
//

// a metafunction to invert an integral_constant:
template <typename T>
struct not_
    : std::integral_constant<bool, !T::value> {};

// Checks whether a type is unsigned (T must be convertible to unsigned int):
template <typename T>
struct is_unsigned
    : std::integral_constant<bool, (T(0) < T(-1))> {};

// Checks whether a type is signed (T must be convertible to int):
template <typename T>
struct is_signed
    : not_< is_unsigned<T> > {};

static_assert(( is_unsigned<quint8>::value));
static_assert((!is_unsigned<qint8>::value));

static_assert((!is_signed<quint8>::value));
static_assert(( is_signed<qint8>::value));

static_assert(( is_unsigned<quint16>::value));
static_assert((!is_unsigned<qint16>::value));

static_assert((!is_signed<quint16>::value));
static_assert(( is_signed<qint16>::value));

static_assert(( is_unsigned<quint32>::value));
static_assert((!is_unsigned<qint32>::value));

static_assert((!is_signed<quint32>::value));
static_assert(( is_signed<qint32>::value));

static_assert(( is_unsigned<quint64>::value));
static_assert((!is_unsigned<qint64>::value));

static_assert((!is_signed<quint64>::value));
static_assert(( is_signed<qint64>::value));

} // namespace QtPrivate

QT_END_NAMESPACE
#endif  // QTYPETRAITS_H

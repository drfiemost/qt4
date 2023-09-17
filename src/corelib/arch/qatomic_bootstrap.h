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

#ifndef QATOMIC_BOOTSTRAP_H
#define QATOMIC_BOOTSTRAP_H

#include <QtCore/qgenericatomic.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

#if 0
// silence syncqt warnings
QT_END_NAMESPACE
QT_END_HEADER

#pragma qt_sync_skip_header_check
#pragma qt_sync_stop_processing
#endif

template <typename T> struct QAtomicOps: QGenericAtomicOps<QAtomicOps<T> >
{
    typedef T Type;

    static bool ref(T &_q_value) noexcept
    {
        return ++_q_value != 0;
    }
    static bool deref(T &_q_value) noexcept
    {
        return --_q_value != 0;
    }

    static bool testAndSetRelaxed(T &_q_value, T expectedValue, T newValue) noexcept
    {
        if (_q_value == expectedValue) {
            _q_value = newValue;
            return true;
        }
        return false;
    }

    static T fetchAndStoreRelaxed(T &_q_value, T newValue) noexcept
    {
        T tmp = _q_value;
        _q_value = newValue;
        return tmp;
    }

    static
    T fetchAndAddRelaxed(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd) noexcept
    {
        T returnValue = _q_value;
        _q_value += valueToAdd;
        return returnValue;
    }
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QATOMIC_BOOTSTRAP_H

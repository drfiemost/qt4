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

#ifndef QNUMERIC_P_H
#define QNUMERIC_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "QtCore/qglobal.h"

#include <cmath>
#include <limits>

QT_BEGIN_NAMESPACE

constexpr static inline double qt_inf()
{
    static_assert(std::numeric_limits<double>::has_infinity,
                  "platform has no definition for infinity for type double");
    return std::numeric_limits<double>::infinity();
}

// Signaling NAN
constexpr static inline double qt_snan()
{
    static_assert(std::numeric_limits<double>::has_signaling_NaN,
                  "platform has no definition for signaling NaN for type double");
    return std::numeric_limits<double>::signaling_NaN();
}

// Quiet NAN
constexpr static inline double qt_qnan()
{
    static_assert(std::numeric_limits<double>::has_quiet_NaN,
                  "platform has no definition for quiet NaN for type double");
    return std::numeric_limits<double>::quiet_NaN();
}

static inline bool qt_is_inf(double d)
{
    return std::isinf(d);
}

static inline bool qt_is_nan(double d)
{
    return std::isnan(d);
}

static inline bool qt_is_finite(double d)
{
    return std::isfinite(d);
}

static inline bool qt_is_inf(float d)
{
    return std::isinf(d);
}

static inline bool qt_is_nan(float d)
{
    return std::isnan(d);
}

static inline bool qt_is_finite(float d)
{
    return std::isfinite(d);
}

QT_END_NAMESPACE

#endif // QNUMERIC_P_H

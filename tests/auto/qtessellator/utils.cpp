/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
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
#include "utils.h"

#include <assert.h>
#include <qglobal.h>

#include "qnum.h"

#define FloatToXFixed(i) (int)((i) * 65536)
#define IntToXFixed(i) ((i) << 16)

static double compute_x_at(XFixed y, XPointFixed p1, XPointFixed p2)
{
    double d = XFixedToDouble(p2.x - p1.x);
    return
        XFixedToDouble(p1.x) + d*XFixedToDouble(y - p1.y)/XFixedToDouble(p2.y - p1.y);
}

double compute_area(XTrapezoid *trap)
{
    double x1 = compute_x_at(trap->top, trap->left.p1, trap->left.p2);
    double x2 = compute_x_at(trap->top, trap->right.p1, trap->right.p2);
    double x3 = compute_x_at(trap->bottom, trap->left.p1, trap->left.p2);
    double x4 = compute_x_at(trap->bottom, trap->right.p1, trap->right.p2);

    double top = XFixedToDouble(trap->top);
    double bottom = XFixedToDouble(trap->bottom);
    double h = bottom - top;

    double top_base = x2 - x1;
    double bottom_base = x4 - x3;

    if ((top_base < 0 && bottom_base > 0)
        || (top_base > 0 && bottom_base < 0)) {
        double y0 = top_base*h/(top_base - bottom_base) + top;
        double area = std::abs(top_base * (y0 - top) / 2.);
        area += std::abs(bottom_base * (bottom - y0) /2.);
        return area;
    }


    return 0.5 * h * std::abs(top_base + bottom_base);
}

double compute_area_for_x(const QVector<XTrapezoid> &traps)
{
    double area = 0;

    for (int i = 0; i < traps.size(); ++i) {
        XTrapezoid trap = traps[i];
        area += compute_area(&trap);
    }
    return area;
}

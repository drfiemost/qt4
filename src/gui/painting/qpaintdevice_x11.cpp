/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qpaintdevice.h"
#include "qpainter.h"
#include "qwidget.h"
#include "qbitmap.h"
#include "qapplication.h"
#include <private/qt_x11_p.h>
#include "qx11info_x11.h"

QT_BEGIN_NAMESPACE

/*! \internal

    Returns the X11 Drawable of the paint device. 0 is returned if it
    can't be obtained.
*/

Drawable Q_GUI_EXPORT qt_x11Handle(const QPaintDevice *pd)
{
    if (!pd) return 0;
    if (pd->devType() == QInternal::Widget)
        return static_cast<const QWidget *>(pd)->handle();
    else if (pd->devType() == QInternal::Pixmap)
        return static_cast<const QPixmap *>(pd)->handle();
    return 0;
}

/*!
    \relates QPaintDevice

    Returns the QX11Info structure for the \a pd paint device. 0 is
    returned if it can't be obtained.
*/
const Q_GUI_EXPORT QX11Info *qt_x11Info(const QPaintDevice *pd)
{
    if (!pd) return nullptr;
    if (pd->devType() == QInternal::Widget)
        return &static_cast<const QWidget *>(pd)->x11Info();
    else if (pd->devType() == QInternal::Pixmap)
        return &static_cast<const QPixmap *>(pd)->x11Info();
    return nullptr;
}


QT_END_NAMESPACE

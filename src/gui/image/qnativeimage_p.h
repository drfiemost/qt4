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

#ifndef QNATIVEIMAGE_P_H
#define QNATIVEIMAGE_P_H

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

#include "qimage.h"

#ifdef Q_WS_WIN
#include "qt_windows.h"

#elif defined(Q_WS_X11)
#include <private/qt_x11_p.h>

#elif defined(Q_WS_MAC)
#include <private/qt_mac_p.h>

#endif

QT_BEGIN_NAMESPACE

class QWidget;

class QNativeImage
{
public:
    QNativeImage(int width, int height, QImage::Format format, bool isTextBuffer = false, QWidget *widget = nullptr);
    ~QNativeImage();

    inline int width() const;
    inline int height() const;

    QImage image;

    static QImage::Format systemFormat();

#ifdef Q_WS_WIN
    HDC hdc;
    HBITMAP bitmap;
    HBITMAP null_bitmap;

#elif defined(Q_WS_X11) && !defined(QT_NO_MITSHM)
    XImage *xshmimg;
    Pixmap xshmpm;
    XShmSegmentInfo xshminfo;

#elif defined(Q_WS_MAC)
    CGContextRef cg;
#endif

private:
    Q_DISABLE_COPY(QNativeImage)
};

inline int QNativeImage::width() const { return image.width(); }
inline int QNativeImage::height() const { return image.height(); }

QT_END_NAMESPACE

#endif // QNATIVEIMAGE_P_H

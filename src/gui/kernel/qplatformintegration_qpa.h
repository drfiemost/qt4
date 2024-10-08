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

#ifndef QPLATFORMINTEGRATION_H
#define QPLATFORMINTEGRATION_H

#include <QtGui/qwindowdefs.h>
#include <QtGui/private/qwindowsurface_p.h>
#include <QtGui/private/qpixmapdata_p.h>
#include <QtGui/qplatformscreen_qpa.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QPlatformWindow;
class QWindow;
class QBlittable;
class QWidget;
class QPlatformEventLoopIntegration;
class QPlatformFontDatabase;
class QPlatformClipboard;
class QPlatformNativeInterface;


class Q_GUI_EXPORT QPlatformIntegration
{
public:
    enum Capability {
        ThreadedPixmaps = 1,
        OpenGL = 2
    };

    virtual ~QPlatformIntegration() { }

    virtual bool hasCapability(Capability cap) const;

// GraphicsSystem functions
    virtual QPixmapData *createPixmapData(QPixmapData::PixelType type) const = 0;
    virtual QPlatformWindow *createPlatformWindow(QWindow *window) const = 0;
    virtual QWindowSurface *createWindowSurface(QWindow *window, WId winId) const = 0;

// Window System functions
    virtual QList<QPlatformScreen *> screens() const = 0;
    virtual void moveToScreen(QWidget *window, int screen) {Q_UNUSED(window); Q_UNUSED(screen);}
    virtual bool isVirtualDesktop() { return false; }
    virtual QPixmap grabWindow(WId window, int x, int y, int width, int height) const;

//Deeper window system integrations
    virtual QPlatformFontDatabase *fontDatabase() const;
#ifndef QT_NO_CLIPBOARD
    virtual QPlatformClipboard *clipboard() const;
#endif

// Experimental in mainthread eventloop integration
// This should only be used if it is only possible to do window system event processing in
// the gui thread. All of the functions in QWindowSystemInterface are thread safe.
    virtual QPlatformEventLoopIntegration *createEventLoopIntegration() const;

// Access native handles. The window handle is already available from Wid;
    virtual QPlatformNativeInterface *nativeInterface() const;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QPLATFORMINTEGRATION_H

/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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

#ifndef QGLPIXELBUFFER_P_H
#define QGLPIXELBUFFER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QLibrary class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

QT_BEGIN_NAMESPACE

QT_BEGIN_INCLUDE_NAMESPACE
#include "QtOpenGL/qglpixelbuffer.h"
#include <private/qgl_p.h>
#include <private/qglpaintdevice_p.h>

#if defined(Q_WS_X11) && defined(QT_NO_EGL)
#include <GL/glx.h>

#elif defined(Q_WS_WIN)
DECLARE_HANDLE(HPBUFFERARB);
#elif !defined(QT_NO_EGL)
#include <QtGui/private/qegl_p.h>
#endif
QT_END_INCLUDE_NAMESPACE

class QEglContext;


class QGLPBufferGLPaintDevice : public QGLPaintDevice
{
public:
    QPaintEngine* paintEngine() const override {return pbuf->paintEngine();}
    QSize size() const override {return pbuf->size();}
    QGLContext* context() const override;
    void endPaint() override;
    void setPBuffer(QGLPixelBuffer* pb);
private:
    QGLPixelBuffer* pbuf;
};

class QGLPixelBufferPrivate {
    Q_DECLARE_PUBLIC(QGLPixelBuffer)
public:
    QGLPixelBufferPrivate(QGLPixelBuffer *q) : q_ptr(q), invalid(true), qctx(nullptr), pbuf(0), ctx(nullptr)
    {
#ifdef Q_WS_WIN
        dc = 0;
#elif defined(Q_WS_MACX)
        share_ctx = 0;
#endif
    }
    bool init(const QSize &size, const QGLFormat &f, QGLWidget *shareWidget);
    void common_init(const QSize &size, const QGLFormat &f, QGLWidget *shareWidget);
    bool cleanup();

    QGLPixelBuffer *q_ptr;
    bool invalid;
    QGLContext *qctx;
    QGLPBufferGLPaintDevice glDevice;
    QGLFormat format;

    QGLFormat req_format;
    QPointer<QGLWidget> req_shareWidget;
    QSize req_size;

#if defined(Q_WS_X11) && defined(QT_NO_EGL)
    GLXPbuffer pbuf;
    GLXContext ctx;
#elif defined(Q_WS_WIN)
    HDC dc;
    bool has_render_texture :1;
#if !defined(QT_OPENGL_ES)
    HPBUFFERARB pbuf;
    HGLRC ctx;
#endif
#elif defined(Q_WS_MACX)
#  ifdef QT_MAC_USE_COCOA
    void *pbuf;
    void *ctx;
    void *share_ctx;
#  else
    AGLPbuffer pbuf;
    AGLContext ctx;
    AGLContext share_ctx;
#  endif
#endif
#ifndef QT_NO_EGL
    EGLSurface pbuf;
    QEglContext *ctx;
    int textureFormat;
#elif defined(Q_WS_QPA)
    //stubs
    void *pbuf;
    void *ctx;
#endif
};

QT_END_NAMESPACE

#endif // QGLPIXELBUFFER_P_H

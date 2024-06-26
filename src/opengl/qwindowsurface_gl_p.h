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

#ifndef QWINDOWSURFACE_GL_P_H
#define QWINDOWSURFACE_GL_P_H

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

#include <qglobal.h>
#include <qgl.h>
#include <private/qwindowsurface_p.h>
#include <private/qglpaintdevice_p.h>

QT_BEGIN_NAMESPACE

class QPaintDevice;
class QPoint;
class QRegion;
class QWidget;
struct QGLWindowSurfacePrivate;

Q_OPENGL_EXPORT QGLWidget* qt_gl_share_widget();
Q_OPENGL_EXPORT void qt_destroy_gl_share_widget();
bool qt_initializing_gl_share_widget();

class QGLWindowSurfaceGLPaintDevice : public QGLPaintDevice
{
public:
    QPaintEngine* paintEngine() const override;
    QSize size() const override;
    int metric(PaintDeviceMetric m) const override;
    QGLContext* context() const override;
    QGLWindowSurfacePrivate* d;
};

class Q_OPENGL_EXPORT QGLWindowSurface : public QObject, public QWindowSurface // , public QPaintDevice
{
    Q_OBJECT
public:
    QGLWindowSurface(QWidget *window);
    ~QGLWindowSurface() override;

    QPaintDevice *paintDevice() override;
    void flush(QWidget *widget, const QRegion &region, const QPoint &offset) override;

#if !defined(Q_WS_QPA)
    void setGeometry(const QRect &rect) override;
#else
    virtual void resize(const QSize &size);
#endif

    void updateGeometry();
    bool scroll(const QRegion &area, int dx, int dy) override;

    void beginPaint(const QRegion &region) override;
    void endPaint(const QRegion &region) override;

    QImage *buffer(const QWidget *widget) override;

    WindowSurfaceFeatures features() const override;

    QGLContext *context() const;

    static QGLFormat surfaceFormat;

    enum SwapMode { AutomaticSwap, AlwaysFullSwap, AlwaysPartialSwap, KillSwap };
    static SwapMode swapBehavior;

private slots:
    void deleted(QObject *object);

private:
    void hijackWindow(QWidget *widget);
    bool initializeOffscreenTexture(const QSize &size);

    QGLWindowSurfacePrivate *d_ptr;
};

QT_END_NAMESPACE

#endif // QWINDOWSURFACE_GL_P_H


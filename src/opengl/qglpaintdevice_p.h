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

#ifndef QGLPAINTDEVICE_P_H
#define QGLPAINTDEVICE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QtOpenGL module.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//


#include <qpaintdevice.h>
#include <qgl.h>


QT_BEGIN_NAMESPACE

class Q_OPENGL_EXPORT QGLPaintDevice : public QPaintDevice
{
public:
    QGLPaintDevice();
    ~QGLPaintDevice() override;

    int devType() const override {return QInternal::OpenGL;}

    virtual void beginPaint();
    virtual void ensureActiveTarget();
    virtual void endPaint();

    virtual QGLContext* context() const = 0;
    virtual QGLFormat format() const;
    virtual QSize size() const = 0;
    virtual bool alphaRequested() const;
    virtual bool isFlipped() const;

    // returns the QGLPaintDevice for the given QPaintDevice
    static QGLPaintDevice* getDevice(QPaintDevice*);

protected:
    int metric(QPaintDevice::PaintDeviceMetric metric) const override;
    GLuint m_previousFBO;
    GLuint m_thisFBO;
};


// Wraps a QGLWidget
class QGLWidget;
class QGLWidgetGLPaintDevice : public QGLPaintDevice
{
public:
    QGLWidgetGLPaintDevice();

    QPaintEngine* paintEngine() const override;

    // QGLWidgets need to do swapBufers in endPaint:
    void beginPaint() override;
    void endPaint() override;
    QSize size() const override;
    QGLContext* context() const override;

    void setWidget(QGLWidget*);

private:
    friend class QGLWidget;
    QGLWidget *glWidget;
};

QT_END_NAMESPACE

#endif // QGLPAINTDEVICE_P_H

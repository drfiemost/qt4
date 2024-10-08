/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef QGRAPHICSSYSTEM_RUNTIME_P_H
#define QGRAPHICSSYSTEM_RUNTIME_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qgraphicssystem_p.h"

#include <private/qpixmapdata_p.h>

QT_BEGIN_NAMESPACE

class QRuntimeGraphicsSystem;

class Q_GUI_EXPORT QRuntimePixmapData : public QPixmapData {
public:
    QRuntimePixmapData(const QRuntimeGraphicsSystem *gs, PixelType type);
    ~QRuntimePixmapData() override;

    QPixmapData *createCompatiblePixmapData() const override;
    void resize(int width, int height) override;
    void fromImage(const QImage &image,
                           Qt::ImageConversionFlags flags) override;

    bool fromFile(const QString &filename, const char *format,
                          Qt::ImageConversionFlags flags) override;
    bool fromData(const uchar *buffer, uint len, const char *format,
                          Qt::ImageConversionFlags flags) override;

    void copy(const QPixmapData *data, const QRect &rect) override;
    bool scroll(int dx, int dy, const QRect &rect) override;

    int metric(QPaintDevice::PaintDeviceMetric metric) const override;
    void fill(const QColor &color) override;
    QBitmap mask() const override;
    void setMask(const QBitmap &mask) override;
    bool hasAlphaChannel() const override;
    QPixmap transformed(const QTransform &matrix,
                                Qt::TransformationMode mode) const override;
    void setAlphaChannel(const QPixmap &alphaChannel) override;
    QPixmap alphaChannel() const override;
    QImage toImage() const override;
    QPaintEngine *paintEngine() const override;

    QImage *buffer() override;

    void readBackInfo();

    QPixmapData *m_data;

    QPixmapData *runtimeData() const override;

private:
    const QRuntimeGraphicsSystem *m_graphicsSystem;

};

class QRuntimeWindowSurface : public QWindowSurface {
public:
    QRuntimeWindowSurface(const QRuntimeGraphicsSystem *gs, QWidget *window);
    ~QRuntimeWindowSurface() override;

    QPaintDevice *paintDevice() override;
    void flush(QWidget *widget, const QRegion &region,
                       const QPoint &offset) override;
    void setGeometry(const QRect &rect) override;

    bool scroll(const QRegion &area, int dx, int dy) override;

    void beginPaint(const QRegion &) override;
    void endPaint(const QRegion &) override;

    QImage* buffer(const QWidget *widget) override;
    QPixmap grabWidget(const QWidget *widget, const QRect& rectangle = QRect()) const override;

    QPoint offset(const QWidget *widget) const override;

    WindowSurfaceFeatures features() const override;

    QScopedPointer<QWindowSurface> m_windowSurface;
    QScopedPointer<QWindowSurface> m_pendingWindowSurface;

private:
    const QRuntimeGraphicsSystem *m_graphicsSystem;
};

class QRuntimeGraphicsSystem : public QGraphicsSystem
{
public:

    enum WindowSurfaceDestroyPolicy
    {
        DestroyImmediately,
        DestroyAfterFirstFlush
    };

public:
    QRuntimeGraphicsSystem();

    QPixmapData *createPixmapData(QPixmapData::PixelType type) const override;
    QWindowSurface *createWindowSurface(QWidget *widget) const override;

    void removePixmapData(QRuntimePixmapData *pixmapData) const;
    void removeWindowSurface(QRuntimeWindowSurface *windowSurface) const;

    void setGraphicsSystem(const QString &name);
    QString graphicsSystemName() const { return m_graphicsSystemName; }

    void setWindowSurfaceDestroyPolicy(WindowSurfaceDestroyPolicy policy)
    {
        m_windowSurfaceDestroyPolicy = policy;
    }

    int windowSurfaceDestroyPolicy() const { return m_windowSurfaceDestroyPolicy; }


private:
    int m_windowSurfaceDestroyPolicy;
    QGraphicsSystem *m_graphicsSystem;
    mutable QList<QRuntimePixmapData *> m_pixmapDatas;
    mutable QList<QRuntimeWindowSurface *> m_windowSurfaces;
    QString m_graphicsSystemName;

    QString m_pendingGraphicsSystemName;

    friend class QRuntimePixmapData;
    friend class QRuntimeWindowSurface;
    friend class QMeeGoGraphicsSystem;
};

QT_END_NAMESPACE

#endif

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

#ifndef QPAINTENGINE_OPENGL_P_H
#define QPAINTENGINE_OPENGL_P_H

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

#include <private/qpaintengineex_p.h>

QT_BEGIN_NAMESPACE

class QOpenGLPaintEnginePrivate;
class QGLTexture;

class QOpenGLPaintEngineState : public QPainterState
{
public:
    QOpenGLPaintEngineState(QOpenGLPaintEngineState &other);
    QOpenGLPaintEngineState();
    ~QOpenGLPaintEngineState() override;

    QRegion clipRegion;
    bool hasClipping;
    QRect fastClip;
    uint depthClipId;
};

class QOpenGLPaintEngine : public QPaintEngineEx
{
    Q_DECLARE_PRIVATE(QOpenGLPaintEngine)
public:
    QOpenGLPaintEngine();
    ~QOpenGLPaintEngine() override;

    bool begin(QPaintDevice *pdev) override;
    bool end() override;

    // new stuff
    void clipEnabledChanged() override;
    void penChanged() override;
    void brushChanged() override;
    void brushOriginChanged() override;
    void opacityChanged() override;
    void compositionModeChanged() override;
    void renderHintsChanged() override;
    void transformChanged() override;

    void fill(const QVectorPath &path, const QBrush &brush) override;
    void clip(const QVectorPath &path, Qt::ClipOperation op) override;

    void setState(QPainterState *s) override;
    QPainterState *createState(QPainterState *orig) const override;
    inline QOpenGLPaintEngineState *state() {
        return static_cast<QOpenGLPaintEngineState *>(QPaintEngineEx::state());
    }
    inline const QOpenGLPaintEngineState *state() const {
        return static_cast<const QOpenGLPaintEngineState *>(QPaintEngineEx::state());
    }


    // old stuff
    void updateState(const QPaintEngineState &state) override;

    void updatePen(const QPen &pen);
    void updateBrush(const QBrush &brush, const QPointF &pt);
    void updateFont(const QFont &font);
    void updateMatrix(const QTransform &matrix);
    void updateClipRegion(const QRegion &region, Qt::ClipOperation op);
    void updateRenderHints(QPainter::RenderHints hints);
    void updateCompositionMode(QPainter::CompositionMode composition_mode);

    void drawRects(const QRectF *r, int rectCount) override;
    void drawLines(const QLineF *lines, int lineCount) override;
    void drawPoints(const QPointF *p, int pointCount) override;
    void drawRects(const QRect *r, int rectCount) override;
    void drawLines(const QLine *lines, int lineCount) override;
    void drawPoints(const QPoint *p, int pointCount) override;

    void drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr) override;

    void drawPath(const QPainterPath &path) override;
    void drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode) override;
    void drawPolygon(const QPoint *points, int pointCount, PolygonDrawMode mode) override;
    void drawTiledPixmap(const QRectF &r, const QPixmap &pixmap, const QPointF &s) override;
    void drawImage(const QRectF &r, const QImage &image, const QRectF &sr,
                   Qt::ImageConversionFlags conversionFlags) override;
    void drawTextItem(const QPointF &p, const QTextItem &ti) override;
    void drawStaticTextItem(QStaticTextItem *staticTextItem) override;

    void drawEllipse(const QRectF &rect) override;

#ifdef Q_WS_WIN
    HDC handle() const;
#else
    Qt::HANDLE handle() const;
#endif
    inline Type type() const override { return QPaintEngine::OpenGL; }
    bool supportsTransformations(qreal, const QTransform &) const override { return true; }

private:
    void drawPolyInternal(const QPolygonF &pa, bool close = true);
    void drawTextureRect(int tx_width, int tx_height, const QRectF &r, const QRectF &sr,
                         GLenum target, QGLTexture *tex);
    Q_DISABLE_COPY(QOpenGLPaintEngine)
};


QT_END_NAMESPACE

#endif // QPAINTENGINE_OPENGL_P_H

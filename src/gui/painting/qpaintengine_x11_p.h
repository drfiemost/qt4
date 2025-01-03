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

#ifndef QPAINTENGINE_X11_P_H
#define QPAINTENGINE_X11_P_H

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

#include "QtGui/qpaintengine.h"
#include "QtGui/qregion.h"
#include "QtGui/qpen.h"
#include "QtCore/qpoint.h"
#include "private/qpaintengine_p.h"
#include "private/qpainter_p.h"
#include "private/qpolygonclipper_p.h"

typedef unsigned long Picture;

QT_BEGIN_NAMESPACE

class QX11PaintEnginePrivate;
class QFontEngineFT;
class QXRenderTessellator;

struct qt_float_point
{
    qreal x, y;
};

class QX11PaintEngine : public QPaintEngine
{
    Q_DECLARE_PRIVATE(QX11PaintEngine)
public:
    QX11PaintEngine();
    ~QX11PaintEngine() override;

    bool begin(QPaintDevice *pdev) override;
    bool end() override;

    void updateState(const QPaintEngineState &state) override;

    void updatePen(const QPen &pen);
    void updateBrush(const QBrush &brush, const QPointF &pt);
    void updateRenderHints(QPainter::RenderHints hints);
    void updateFont(const QFont &font);
    void updateMatrix(const QTransform &matrix);
    void updateClipRegion_dev(const QRegion &region, Qt::ClipOperation op);

    void drawLines(const QLine *lines, int lineCount) override;
    void drawLines(const QLineF *lines, int lineCount) override;

    void drawRects(const QRect *rects, int rectCount) override;
    void drawRects(const QRectF *rects, int rectCount) override;

    void drawPoints(const QPoint *points, int pointCount) override;
    void drawPoints(const QPointF *points, int pointCount) override;

    void drawEllipse(const QRect &r) override;
    void drawEllipse(const QRectF &r) override;

    void drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode) override;
    inline void drawPolygon(const QPoint *points, int pointCount, PolygonDrawMode mode) override
        { QPaintEngine::drawPolygon(points, pointCount, mode); }

    void drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr) override;
    void drawTiledPixmap(const QRectF &r, const QPixmap &pixmap, const QPointF &s) override;
    void drawPath(const QPainterPath &path) override;
    void drawTextItem(const QPointF &p, const QTextItem &textItem) override;
    void drawImage(const QRectF &r, const QImage &img, const QRectF &sr,
                   Qt::ImageConversionFlags flags = Qt::AutoColor) override;

    virtual Qt::HANDLE handle() const;
    inline Type type() const override { return QPaintEngine::X11; }

    QPainter::RenderHints supportedRenderHints() const;

protected:
    QX11PaintEngine(QX11PaintEnginePrivate &dptr);

    void drawXLFD(const QPointF &p, const QTextItemInt &si);
#ifndef QT_NO_FONTCONFIG
    void drawFreetype(const QPointF &p, const QTextItemInt &si);
#endif

    friend class QPixmap;
    friend class QFontEngineBox;
    friend Q_GUI_EXPORT GC qt_x11_get_pen_gc(QPainter *);
    friend Q_GUI_EXPORT GC qt_x11_get_brush_gc(QPainter *);

private:
    Q_DISABLE_COPY(QX11PaintEngine)
};

class QX11PaintEnginePrivate : public QPaintEnginePrivate
{
    Q_DECLARE_PUBLIC(QX11PaintEngine)
public:
    QX11PaintEnginePrivate()
    {
        scrn = -1;
        hd = 0;
        picture = 0;
        gc = gc_brush = nullptr;
        dpy  = nullptr;
        xinfo = nullptr;
        txop = QTransform::TxNone;
        has_clipping = false;
        render_hints = 0;
        xform_scale = 1;
#ifndef QT_NO_XRENDER
        tessellator = nullptr;
#endif
    }
    enum GCMode {
        PenGC,
        BrushGC
    };

    void init();
    void fillPolygon_translated(const QPointF *points, int pointCount, GCMode gcMode,
                                QPaintEngine::PolygonDrawMode mode);
    void fillPolygon_dev(const QPointF *points, int pointCount, GCMode gcMode,
                         QPaintEngine::PolygonDrawMode mode);
    void fillPath(const QPainterPath &path, GCMode gcmode, bool transform);
    void strokePolygon_dev(const QPointF *points, int pointCount, bool close);
    void strokePolygon_translated(const QPointF *points, int pointCount, bool close);
    void setupAdaptedOrigin(const QPoint &p);
    void resetAdaptedOrigin();
    void decidePathFallback() {
        use_path_fallback = has_alpha_brush
                            || has_alpha_pen
                            || has_custom_pen
                            || has_complex_xform
                            || (render_hints & QPainter::Antialiasing);
    }
    void decideCoordAdjust() {
        adjust_coords = !(render_hints & QPainter::Antialiasing)
                        && (has_alpha_pen
                            || (has_alpha_brush && has_pen && !has_alpha_pen)
                            || (cpen.style() > Qt::SolidLine));
    }
    void clipPolygon_dev(const QPolygonF &poly, QPolygonF *clipped_poly);
    void systemStateChanged() override;

    Display *dpy;
    int scrn;
    int pdev_depth;
    Qt::HANDLE hd;
    QPixmap brush_pm;
#if !defined (QT_NO_XRENDER)
    Qt::HANDLE picture;
    Qt::HANDLE current_brush;
    QPixmap bitmap_texture;
    int composition_mode;
#else
    Qt::HANDLE picture;
#endif
    GC gc;
    GC gc_brush;

    QPen cpen;
    QBrush cbrush;
    QRegion crgn;
    QTransform matrix;
    qreal opacity;

    uint has_complex_xform : 1;
    uint has_scaling_xform : 1;
    uint has_non_scaling_xform : 1;
    uint has_custom_pen : 1;
    uint use_path_fallback : 1;
    uint adjust_coords : 1;
    uint has_clipping : 1;
    uint adapted_brush_origin : 1;
    uint adapted_pen_origin : 1;
    uint has_pen : 1;
    uint has_brush : 1;
    uint has_texture : 1;
    uint has_alpha_texture : 1;
    uint has_pattern : 1;
    uint has_alpha_pen : 1;
    uint has_alpha_brush : 1;
    uint render_hints;

    const QX11Info *xinfo;
    QPointF bg_origin;
    QTransform::TransformationType txop;
    qreal xform_scale;
    QPolygonClipper<qt_float_point, qt_float_point, float> polygonClipper;

    int xlibMaxLinePoints;
#ifndef QT_NO_XRENDER
    QXRenderTessellator *tessellator;
#endif
};

QT_END_NAMESPACE

#endif // QPAINTENGINE_X11_P_H

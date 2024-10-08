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

#ifndef QEMULATIONPAINTENGINE_P_H
#define QEMULATIONPAINTENGINE_P_H

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

#include <private/qpaintengineex_p.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QEmulationPaintEngine : public QPaintEngineEx
{
public:
    QEmulationPaintEngine(QPaintEngineEx *engine);
    
    bool begin(QPaintDevice *pdev) override;
    bool end() override;
    
    Type type() const override;
    QPainterState *createState(QPainterState *orig) const override;

    void fill(const QVectorPath &path, const QBrush &brush) override;
    void stroke(const QVectorPath &path, const QPen &pen) override;
    void clip(const QVectorPath &path, Qt::ClipOperation op) override;

    void drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr) override;
    void drawTextItem(const QPointF &p, const QTextItem &textItem) override;
    void drawStaticTextItem(QStaticTextItem *item) override;
    void drawTiledPixmap(const QRectF &r, const QPixmap &pixmap, const QPointF &s) override;
    void drawImage(const QRectF &r, const QImage &pm, const QRectF &sr, Qt::ImageConversionFlags flags) override;

    void drawPixmapFragments(const QPainter::PixmapFragment *fragments, int fragmentCount, const QPixmap &pixmap,
                                     QPainter::PixmapFragmentHints hints) override;
    void drawPixmapFragments(const QRectF *targetRects, const QRectF *sourceRects, int fragmentCount, const QPixmap &pixmap,
                                     QPainter::PixmapFragmentHints hints) override;
    
    void clipEnabledChanged() override;
    void penChanged() override;
    void brushChanged() override;
    void brushOriginChanged() override;
    void opacityChanged() override;
    void compositionModeChanged() override;
    void renderHintsChanged() override;
    void transformChanged() override;

    void setState(QPainterState *s) override;

    void beginNativePainting() override;
    void endNativePainting() override;

    uint flags() const override {return QPaintEngineEx::IsEmulationEngine | QPaintEngineEx::DoNotEmulate;}

    inline QPainterState *state() { return (QPainterState *)QPaintEngine::state; }
    inline const QPainterState *state() const { return (const QPainterState *)QPaintEngine::state; }

    QPaintEngineEx *real_engine;
private:
    void fillBGRect(const QRectF &r);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif

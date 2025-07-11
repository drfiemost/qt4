/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtSvg module of the Qt Toolkit.
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

#include "qsvggraphics_p.h"

#ifndef QT_NO_SVG

#include "qsvgfont_p.h"

#include <qabstracttextdocumentlayout.h>
#include <qdebug.h>
#include <qpainter.h>
#include <qscopedvaluerollback.h>
#include <qtextcursor.h>
#include <qtextdocument.h>
#include <private/qfixed_p.h>

#include <climits>
#include <cmath>

#include <algorithm>
#include <utility>


QT_BEGIN_NAMESPACE

#define QT_SVG_DRAW_SHAPE(command)                          \
    qreal oldOpacity = p->opacity();                        \
    QBrush oldBrush = p->brush();                           \
    QPen oldPen = p->pen();                                 \
    p->setPen(Qt::NoPen);                                   \
    p->setOpacity(oldOpacity * states.fillOpacity);         \
    command;                                                \
    p->setPen(oldPen);                                      \
    if (oldPen != Qt::NoPen && oldPen.brush() != Qt::NoBrush && oldPen.widthF() != 0) { \
        p->setOpacity(oldOpacity * states.strokeOpacity);   \
        p->setBrush(Qt::NoBrush);                           \
        command;                                            \
        p->setBrush(oldBrush);                              \
    }                                                       \
    p->setOpacity(oldOpacity);


#ifndef QT_SVG_MAX_LAYOUT_SIZE
#define QT_SVG_MAX_LAYOUT_SIZE (qint64(QFIXED_MAX / 2))
#endif

void QSvgAnimation::draw(QPainter *, QSvgExtraStates &)
{
    qWarning("<animation> no implemented");
}

static inline QRectF boundsOnStroke(QPainter *p, const QPainterPath &path, qreal width)
{
    QPainterPathStroker stroker;
    stroker.setWidth(width);
    QPainterPath stroke = stroker.createStroke(path);
    return p->transform().map(stroke).boundingRect();
}

QSvgEllipse::QSvgEllipse(QSvgNode *parent, const QRectF &rect)
    : QSvgNode(parent), m_bounds(rect)
{
}


QRectF QSvgEllipse::bounds(QPainter *p, QSvgExtraStates &) const
{
    QPainterPath path;
    path.addEllipse(m_bounds);
    qreal sw = strokeWidth(p);
    return qFuzzyIsNull(sw) ? p->transform().map(path).boundingRect() : boundsOnStroke(p, path, sw);
}

void QSvgEllipse::draw(QPainter *p, QSvgExtraStates &states)
{
    applyStyle(p, states);
    QT_SVG_DRAW_SHAPE(p->drawEllipse(m_bounds));
    revertStyle(p, states);
}

QSvgArc::QSvgArc(QSvgNode *parent, const QPainterPath &path)
    : QSvgNode(parent), m_path(path)
{
}

void QSvgArc::draw(QPainter *p, QSvgExtraStates &states)
{
    applyStyle(p, states);
    if (p->pen().widthF() != 0) {
        qreal oldOpacity = p->opacity();
        p->setOpacity(oldOpacity * states.strokeOpacity);
        p->drawPath(m_path);
        p->setOpacity(oldOpacity);
    }
    revertStyle(p, states);
}

QSvgImage::QSvgImage(QSvgNode *parent, const QImage &image,
                     const QRectF &bounds)
    : QSvgNode(parent), m_image(image),
      m_bounds(bounds)
{
    if (m_bounds.width() == 0.0)
        m_bounds.setWidth(static_cast<qreal>(m_image.width()));
    if (m_bounds.height() == 0.0)
        m_bounds.setHeight(static_cast<qreal>(m_image.height()));
}

void QSvgImage::draw(QPainter *p, QSvgExtraStates &states)
{
    applyStyle(p, states);
    p->drawImage(m_bounds, m_image);
    revertStyle(p, states);
}


QSvgLine::QSvgLine(QSvgNode *parent, const QLineF &line)
    : QSvgNode(parent), m_line(line)
{
}


void QSvgLine::draw(QPainter *p, QSvgExtraStates &states)
{
    applyStyle(p, states);
    if (p->pen().widthF() != 0) {
        qreal oldOpacity = p->opacity();
        p->setOpacity(oldOpacity * states.strokeOpacity);
        p->drawLine(m_line);
        p->setOpacity(oldOpacity);
    }
    revertStyle(p, states);
}

QSvgPath::QSvgPath(QSvgNode *parent, const QPainterPath &qpath)
    : QSvgNode(parent), m_path(qpath)
{
}

void QSvgPath::draw(QPainter *p, QSvgExtraStates &states)
{
    applyStyle(p, states);
    m_path.setFillRule(states.fillRule);
    QT_SVG_DRAW_SHAPE(p->drawPath(m_path));
    revertStyle(p, states);
}

QRectF QSvgPath::bounds(QPainter *p, QSvgExtraStates &) const
{
    qreal sw = strokeWidth(p);
    return qFuzzyIsNull(sw) ? p->transform().map(m_path).boundingRect()
        : boundsOnStroke(p, m_path, sw);
}

QSvgPolygon::QSvgPolygon(QSvgNode *parent, const QPolygonF &poly)
    : QSvgNode(parent), m_poly(poly)
{
}

QRectF QSvgPolygon::bounds(QPainter *p, QSvgExtraStates &) const
{
    qreal sw = strokeWidth(p);
    if (qFuzzyIsNull(sw)) {
        return p->transform().map(m_poly).boundingRect();
    } else {
        QPainterPath path;
        path.addPolygon(m_poly);
        return boundsOnStroke(p, path, sw);
    }
}

void QSvgPolygon::draw(QPainter *p, QSvgExtraStates &states)
{
    applyStyle(p, states);
    QT_SVG_DRAW_SHAPE(p->drawPolygon(m_poly, states.fillRule));
    revertStyle(p, states);
}


QSvgPolyline::QSvgPolyline(QSvgNode *parent, const QPolygonF &poly)
    : QSvgNode(parent), m_poly(poly)
{

}

void QSvgPolyline::draw(QPainter *p, QSvgExtraStates &states)
{
    applyStyle(p, states);
    qreal oldOpacity = p->opacity();
    if (p->brush().style() != Qt::NoBrush) {
        QPen save = p->pen();
        p->setPen(QPen(Qt::NoPen));
        p->setOpacity(oldOpacity * states.fillOpacity);
        p->drawPolygon(m_poly, states.fillRule);
        p->setPen(save);
    }
    if (p->pen().widthF() != 0) {
        p->setOpacity(oldOpacity * states.strokeOpacity);
        p->drawPolyline(m_poly);
    }
    p->setOpacity(oldOpacity);
    revertStyle(p, states);
}

QSvgRect::QSvgRect(QSvgNode *node, const QRectF &rect, int rx, int ry)
    : QSvgNode(node),
      m_rect(rect), m_rx(rx), m_ry(ry)
{
}

QRectF QSvgRect::bounds(QPainter *p, QSvgExtraStates &) const
{
    qreal sw = strokeWidth(p);
    if (qFuzzyIsNull(sw)) {
        return p->transform().mapRect(m_rect);
    } else {
        QPainterPath path;
        path.addRect(m_rect);
        return boundsOnStroke(p, path, sw);
    }
}

void QSvgRect::draw(QPainter *p, QSvgExtraStates &states)
{
    applyStyle(p, states);
    if (m_rx || m_ry) {
        QT_SVG_DRAW_SHAPE(p->drawRoundedRect(m_rect, m_rx, m_ry, Qt::RelativeSize));
    } else {
        QT_SVG_DRAW_SHAPE(p->drawRect(m_rect));
    }
    revertStyle(p, states);
}

QSvgTspan * const QSvgText::LINEBREAK = nullptr;

QSvgText::QSvgText(QSvgNode *parent, const QPointF &coord)
    : QSvgNode(parent)
    , m_coord(coord)
    , m_type(TEXT)
    , m_size(0, 0)
    , m_mode(Default)
{
}

QSvgText::~QSvgText()
{
    for (auto & m_tspan : m_tspans) {
        if (m_tspan != LINEBREAK)
            delete m_tspan;
    }
}

void QSvgText::setTextArea(const QSizeF &size)
{
    m_size = size;
    m_type = TEXTAREA;
}

QRectF QSvgText::bounds(QPainter *p, QSvgExtraStates &states) const
{
    QRectF boundingRect;
    if (precheck(p))
        draw_helper(p, states, &boundingRect);
    return p->transform().mapRect(boundingRect);
}

bool QSvgText::precheck(QPainter *p) const
{
    qsizetype numChars = 0;
    qreal originalFontSize = p->font().pointSizeF();
    qreal maxFontSize = originalFontSize;
    for (const QSvgTspan *span : std::as_const(m_tspans)) {
        numChars += span->text().size();

        QSvgFontStyle *style = static_cast<QSvgFontStyle *>(span->styleProperty(QSvgStyleProperty::FONT));
        if (style != nullptr && style->qfont().pointSizeF() > maxFontSize)
            maxFontSize = style->qfont().pointSizeF();
    }

    QFont font = p->font();
    font.setPixelSize((100.0 / originalFontSize) * maxFontSize);
    QFontMetricsF fm(font);
    if (m_tspans.size() * fm.height() >= QT_SVG_MAX_LAYOUT_SIZE) {
        qWarning("Text element too high to lay out, ignoring");
        return false;
    }

    if (numChars * fm.maxWidth() >= QT_SVG_MAX_LAYOUT_SIZE) {
        qWarning("Text element too wide to lay out, ignoring");
        return false;
    }

    return true;
}

void QSvgText::draw(QPainter *p, QSvgExtraStates &states)
{
    if (precheck(p))
        draw_helper(p, states);
}

void QSvgText::draw_helper(QPainter *p, QSvgExtraStates &states, QRectF *boundingRect) const
{
    const bool isPainting = (boundingRect == nullptr);
    if (isPainting)
        applyStyle(p, states);
    qreal oldOpacity = p->opacity();
    p->setOpacity(oldOpacity * states.fillOpacity);

    // Force the font to have a size of 100 pixels to avoid truncation problems
    // when the font is very small.
    qreal scale = 100.0 / p->font().pointSizeF();
    Qt::Alignment alignment = states.textAnchor;

    QTransform oldTransform = p->worldTransform();
    p->scale(1 / scale, 1 / scale);

    qreal y = 0;
    bool initial = true;
    qreal px = m_coord.x() * scale;
    qreal py = m_coord.y() * scale;
    QSizeF scaledSize = m_size * scale;

    if (m_type == TEXTAREA) {
        if (alignment == Qt::AlignHCenter)
            px += scaledSize.width() / 2;
        else if (alignment == Qt::AlignRight)
            px += scaledSize.width();
    }

    QRectF bounds;
    if (m_size.height() != 0)
        bounds = QRectF(0, py, 1, scaledSize.height()); // x and width are not used.

    bool appendSpace = false;
    QVector<QString> paragraphs;
    QVector<QList<QTextLayout::FormatRange> > formatRanges;
    paragraphs.push_back(QString());
    formatRanges.push_back(QList<QTextLayout::FormatRange>());

    for (auto m_tspan : m_tspans) {
        if (m_tspan == LINEBREAK) {
            if (m_type == TEXTAREA) {
                if (paragraphs.back().isEmpty()) {
                    QFont font = p->font();
                    font.setPixelSize(font.pointSizeF() * scale);

                    QTextLayout::FormatRange range;
                    range.start = 0;
                    range.length = 1;
                    range.format.setFont(font);
                    formatRanges.back().append(range);

                    paragraphs.back().append(QLatin1Char(' '));;
                }
                appendSpace = false;
                paragraphs.push_back(QString());
                formatRanges.push_back(QList<QTextLayout::FormatRange>());
            }
        } else {
            WhitespaceMode mode = m_tspan->whitespaceMode();
            m_tspan->applyStyle(p, states);

            QFont font = p->font();
            font.setPixelSize(font.pointSizeF() * scale);

            QString newText(m_tspan->text());
            newText.replace(QLatin1Char('\t'), QLatin1Char(' '));
            newText.replace(QLatin1Char('\n'), QLatin1Char(' '));

            bool prependSpace = !appendSpace && !m_tspan->isTspan() && (mode == Default) && !paragraphs.back().isEmpty() && newText.startsWith(QLatin1Char(' '));
            if (appendSpace || prependSpace)
                paragraphs.back().append(QLatin1Char(' '));

            bool appendSpaceNext = (!m_tspan->isTspan() && (mode == Default) && newText.endsWith(QLatin1Char(' ')));

            if (mode == Default) {
                newText = newText.simplified();
                if (newText.isEmpty())
                    appendSpaceNext = false;
            }

            QTextLayout::FormatRange range;
            range.start = paragraphs.back().length();
            range.length = newText.length();
            range.format.setFont(font);
            if (p->pen().style() != Qt::NoPen && p->pen().brush() != Qt::NoBrush)
                range.format.setTextOutline(p->pen());
            range.format.setForeground(p->brush());

            if (appendSpace) {
                Q_ASSERT(!formatRanges.back().isEmpty());
                ++formatRanges.back().back().length;
            } else if (prependSpace) {
                --range.start;
                ++range.length;
            }
            formatRanges.back().append(range);

            appendSpace = appendSpaceNext;
            paragraphs.back() += newText;

            m_tspan->revertStyle(p, states);
        }
    }

    if (states.svgFont) {
        // SVG fonts not fully supported...
        QString text = paragraphs.front();
        for (int i = 1; i < paragraphs.size(); ++i) {
            text.append(QLatin1Char('\n'));
            text.append(paragraphs[i]);
        }
        states.svgFont->draw(p, m_coord * scale, text, p->font().pointSizeF() * scale, states.textAnchor);
    } else {
        QRectF brect;
        for (int i = 0; i < paragraphs.size(); ++i) {
            QTextLayout tl(paragraphs[i]);
            QTextOption op = tl.textOption();
            op.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
            tl.setTextOption(op);
            tl.setAdditionalFormats(formatRanges[i]);
            tl.beginLayout();

            forever {
                QTextLine line = tl.createLine();
                if (!line.isValid())
                    break;
                if (m_size.width() != 0)
                    line.setLineWidth(scaledSize.width());
            }
            tl.endLayout();

            bool endOfBoundsReached = false;
            for (int i = 0; i < tl.lineCount(); ++i) {
                QTextLine line = tl.lineAt(i);

                qreal x = 0;
                if (alignment == Qt::AlignHCenter)
                    x -= 0.5 * line.naturalTextWidth();
                else if (alignment == Qt::AlignRight)
                    x -= line.naturalTextWidth();

                if (initial && m_type == TEXT)
                    y -= line.ascent();
                initial = false;

                line.setPosition(QPointF(x, y));
                brect |= line.naturalTextRect();

                // Check if the current line fits into the bounding rectangle.
                if ((m_size.width() != 0 && line.naturalTextWidth() > scaledSize.width())
                    || (m_size.height() != 0 && y + line.height() > scaledSize.height())) {
                    // I need to set the bounds height to 'y-epsilon' to avoid drawing the current
                    // line. Since the font is scaled to 100 units, 1 should be a safe epsilon.
                    bounds.setHeight(y - 1);
                    endOfBoundsReached = true;
                    break;
                }

                y += 1.1 * line.height();
            }
            if (isPainting)
                tl.draw(p, QPointF(px, py), QVector<QTextLayout::FormatRange>(), bounds);

            if (endOfBoundsReached)
                break;
        }
        if (boundingRect) {
            brect.translate(m_coord * scale);
            if (bounds.height() > 0)
                brect.setBottom(std::min(brect.bottom(), bounds.bottom()));
            *boundingRect = QTransform::fromScale(1 / scale, 1 / scale).mapRect(brect);
        }
    }

    p->setWorldTransform(oldTransform, false);
    p->setOpacity(oldOpacity);
    if (isPainting)
        revertStyle(p, states);
}

void QSvgText::addText(const QString &text)
{
    m_tspans.append(new QSvgTspan(this, false));
    m_tspans.back()->setWhitespaceMode(m_mode);
    m_tspans.back()->addText(text);
}

QSvgUse::QSvgUse(const QPointF &start, QSvgNode *parent, QSvgNode *node)
    : QSvgNode(parent), m_link(node), m_start(start), m_recursing(false)
{

}

void QSvgUse::draw(QPainter *p, QSvgExtraStates &states)
{
    if (Q_UNLIKELY(!m_link || isDescendantOf(m_link) || m_recursing))
        return;

    Q_ASSERT(states.nestedUseCount == 0 || states.nestedUseLevel > 0);
    if (states.nestedUseLevel > 3 && states.nestedUseCount > (256 + states.nestedUseLevel * 2)) {
        qDebug("Too many nested use nodes at #%s!", qPrintable(m_linkId));
        return;
    }

    applyStyle(p, states);

    if (!m_start.isNull()) {
        p->translate(m_start);
    }
    if (states.nestedUseLevel > 0)
        ++states.nestedUseCount;
    {
        QScopedValueRollback<int> useLevelGuard(states.nestedUseLevel, states.nestedUseLevel + 1);
        QScopedValueRollback<bool> recursingGuard(m_recursing, true);
        m_link->draw(p, states);
    }
    if (states.nestedUseLevel == 0)
        states.nestedUseCount = 0;

    if (!m_start.isNull()) {
        p->translate(-m_start);
    }

    revertStyle(p, states);
}

void QSvgVideo::draw(QPainter *p, QSvgExtraStates &states)
{
    applyStyle(p, states);

    revertStyle(p, states);
}

QSvgNode::Type QSvgAnimation::type() const
{
    return ANIMATION;
}

QSvgNode::Type QSvgArc::type() const
{
    return ARC;
}

QSvgNode::Type QSvgCircle::type() const
{
    return CIRCLE;
}

QSvgNode::Type QSvgEllipse::type() const
{
    return ELLIPSE;
}

QSvgNode::Type QSvgImage::type() const
{
    return IMAGE;
}

QSvgNode::Type QSvgLine::type() const
{
    return LINE;
}

QSvgNode::Type QSvgPath::type() const
{
    return PATH;
}

QSvgNode::Type QSvgPolygon::type() const
{
    return POLYGON;
}

QSvgNode::Type QSvgPolyline::type() const
{
    return POLYLINE;
}

QSvgNode::Type QSvgRect::type() const
{
    return RECT;
}

QSvgNode::Type QSvgText::type() const
{
    return m_type;
}

QSvgNode::Type QSvgUse::type() const
{
    return USE;
}

QSvgNode::Type QSvgVideo::type() const
{
    return VIDEO;
}

QRectF QSvgUse::bounds(QPainter *p, QSvgExtraStates &states) const
{
    QRectF bounds;
    if (Q_LIKELY(m_link && !isDescendantOf(m_link) && !m_recursing)) {
        QScopedValueRollback<bool> guard(m_recursing);
        m_recursing = true;
        p->translate(m_start);
        bounds = m_link->transformedBounds(p, states);
        p->translate(-m_start);
    }
    return bounds;
}

QRectF QSvgPolyline::bounds(QPainter *p, QSvgExtraStates &) const
{
    qreal sw = strokeWidth(p);
    if (qFuzzyIsNull(sw)) {
        return p->transform().map(m_poly).boundingRect();
    } else {
        QPainterPath path;
        path.addPolygon(m_poly);
        return boundsOnStroke(p, path, sw);
    }
}

QRectF QSvgArc::bounds(QPainter *p, QSvgExtraStates &) const
{
    qreal sw = strokeWidth(p);
    return qFuzzyIsNull(sw) ? p->transform().map(m_path).boundingRect()
        : boundsOnStroke(p, m_path, sw);
}

QRectF QSvgImage::bounds(QPainter *p, QSvgExtraStates &) const
{
    return p->transform().mapRect(m_bounds);
}

QRectF QSvgLine::bounds(QPainter *p, QSvgExtraStates &) const
{
    qreal sw = strokeWidth(p);
    if (qFuzzyIsNull(sw)) {
        QPointF p1 = p->transform().map(m_line.p1());
        QPointF p2 = p->transform().map(m_line.p2());
        qreal minX = std::min(p1.x(), p2.x());
        qreal minY = std::min(p1.y(), p2.y());
        qreal maxX = std::max(p1.x(), p2.x());
        qreal maxY = std::max(p1.y(), p2.y());
        return QRectF(minX, minY, maxX - minX, maxY - minY);
    } else {
        QPainterPath path;
        path.moveTo(m_line.p1());
        path.lineTo(m_line.p2());
        return boundsOnStroke(p, path, sw);
    }
}

QT_END_NAMESPACE

#endif // QT_NO_SVG

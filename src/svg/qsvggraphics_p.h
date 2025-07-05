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

#ifndef QSVGGRAPHICS_P_H
#define QSVGGRAPHICS_P_H

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

#include "qsvgnode_p.h"

#ifndef QT_NO_SVG

#include "QtGui/qpainterpath.h"
#include "QtGui/qimage.h"
#include "QtGui/qtextlayout.h"
#include "QtGui/qtextoption.h"
#include "QtCore/qstack.h"

QT_BEGIN_NAMESPACE

class QTextCharFormat;

class QSvgAnimation : public QSvgNode
{
public:
    void draw(QPainter *p, QSvgExtraStates &states) override;
    Type type() const override;
};

class QSvgArc : public QSvgNode
{
public:
    QSvgArc(QSvgNode *parent, const QPainterPath &path);
    void draw(QPainter *p, QSvgExtraStates &states) override;
    Type type() const override;
    QRectF bounds(QPainter *p, QSvgExtraStates &states) const override;
private:
    QPainterPath m_path;
};

class QSvgEllipse : public QSvgNode
{
public:
    QSvgEllipse(QSvgNode *parent, const QRectF &rect);
    void draw(QPainter *p, QSvgExtraStates &states) override;
    Type type() const override;
    QRectF bounds(QPainter *p, QSvgExtraStates &states) const override;
private:
    QRectF m_bounds;
};

class QSvgCircle : public QSvgEllipse
{
public:
    QSvgCircle(QSvgNode *parent, const QRectF &rect) : QSvgEllipse(parent, rect) { }
    Type type() const override;
};

class QSvgImage : public QSvgNode
{
public:
    QSvgImage(QSvgNode *parent, const QImage &image,
              const QRectF &bounds);
    void draw(QPainter *p, QSvgExtraStates &states) override;
    Type type() const override;
    QRectF bounds(QPainter *p, QSvgExtraStates &states) const override;
private:
    QImage m_image;
    QRectF m_bounds;
};

class QSvgLine : public QSvgNode
{
public:
    QSvgLine(QSvgNode *parent, const QLineF &line);
    void draw(QPainter *p, QSvgExtraStates &states) override;
    Type type() const override;
    QRectF bounds(QPainter *p, QSvgExtraStates &states) const override;
private:
    QLineF m_line;
};

class QSvgPath : public QSvgNode
{
public:
    QSvgPath(QSvgNode *parent, const QPainterPath &qpath);
    void draw(QPainter *p, QSvgExtraStates &states) override;
    Type type() const override;
    QRectF bounds(QPainter *p, QSvgExtraStates &states) const override;

    QPainterPath *qpath() {
        return &m_path;
    }
private:
    QPainterPath m_path;
};

class QSvgPolygon : public QSvgNode
{
public:
    QSvgPolygon(QSvgNode *parent, const QPolygonF &poly);
    void draw(QPainter *p, QSvgExtraStates &states) override;
    Type type() const override;
    QRectF bounds(QPainter *p, QSvgExtraStates &states) const override;
private:
    QPolygonF m_poly;
};

class QSvgPolyline : public QSvgNode
{
public:
    QSvgPolyline(QSvgNode *parent, const QPolygonF &poly);
    void draw(QPainter *p, QSvgExtraStates &states) override;
    Type type() const override;
    QRectF bounds(QPainter *p, QSvgExtraStates &states) const override;
private:
    QPolygonF m_poly;
};

class QSvgRect : public QSvgNode
{
public:
    QSvgRect(QSvgNode *paren, const QRectF &rect, int rx=0, int ry=0);
    Type type() const override;
    void draw(QPainter *p, QSvgExtraStates &states) override;
    QRectF bounds(QPainter *p, QSvgExtraStates &states) const override;
private:
    QRectF m_rect;
    qreal m_rx, m_ry;
};

class  QSvgTspan;

class  QSvgText : public QSvgNode
{
public:
    enum WhitespaceMode
    {
        Default,
        Preserve
    };

    QSvgText(QSvgNode *parent, const QPointF &coord);
    ~QSvgText() override;
    void setTextArea(const QSizeF &size);

    void draw(QPainter *p, QSvgExtraStates &states) override;
    Type type() const override;

    void addTspan(QSvgTspan *tspan) {m_tspans.append(tspan);}
    void addText(const QString &text);
    void addLineBreak() {m_tspans.append(LINEBREAK);}
    void setWhitespaceMode(WhitespaceMode mode) {m_mode = mode;}

    QRectF bounds(QPainter *p, QSvgExtraStates &states) const override;
private:
    bool precheck(QPainter *p) const;
    void draw_helper(QPainter *p, QSvgExtraStates &states, QRectF *boundingRect = nullptr) const;

    static QSvgTspan * const LINEBREAK;

    QPointF m_coord;

    // 'm_tspans' is also used to store characters outside tspans and line breaks.
    // If a 'm_tspan' item is null, it indicates a line break.
    QVector<QSvgTspan *> m_tspans;

    Type m_type;
    QSizeF m_size;
    WhitespaceMode m_mode;
};

class  QSvgTspan : public QSvgNode
{
public:
    // tspans are also used to store normal text, so the 'isProperTspan' is used to separate text from tspan.
    QSvgTspan(QSvgNode *parent, bool isProperTspan = true)
        : QSvgNode(parent), m_mode(QSvgText::Default), m_isTspan(isProperTspan)
    {
    }
    ~QSvgTspan() override = default;
    Type type() const override {return TSPAN;}
    void draw(QPainter *, QSvgExtraStates &) override {Q_ASSERT(!"Tspans should be drawn through QSvgText::draw().");}
    void addText(const QString &text) {m_text += text;}
    const QString &text() const {return m_text;}
    bool isTspan() const {return m_isTspan;}
    void setWhitespaceMode(QSvgText::WhitespaceMode mode) {m_mode = mode;}
    QSvgText::WhitespaceMode whitespaceMode() const {return m_mode;}
private:
    QString m_text;
    QSvgText::WhitespaceMode m_mode;
    bool m_isTspan;
};

class QSvgUse : public QSvgNode
{
public:
    QSvgUse(const QPointF &start, QSvgNode *parent, QSvgNode *link);
    QSvgUse(const QPointF &start, QSvgNode *parent, const QString &linkId)
        : QSvgUse(start, parent, nullptr)
    { m_linkId = linkId; }
    void draw(QPainter *p, QSvgExtraStates &states) override;
    Type type() const override;
    QRectF bounds(QPainter *p, QSvgExtraStates &states) const override;
    bool isResolved() const { return m_link != nullptr; }
    QString linkId() const { return m_linkId; }
    void setLink(QSvgNode *link) { m_link = link; }
    QSvgNode *link() const { return m_link; }

private:
    QSvgNode *m_link;
    QPointF   m_start;
    QString   m_linkId;
    mutable bool m_recursing;
};

class QSvgVideo : public QSvgNode
{
public:
    void draw(QPainter *p, QSvgExtraStates &states) override;
    Type type() const override;
};

QT_END_NAMESPACE

#endif // QT_NO_SVG
#endif // QSVGGRAPHICS_P_H

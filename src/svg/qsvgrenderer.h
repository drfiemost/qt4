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

#ifndef QSVGRENDERER_H
#define QSVGRENDERER_H

#include <QtGui/qmatrix.h>

#ifndef QT_NO_SVGRENDERER

#include <QtCore/qobject.h>
#include <QtCore/qsize.h>
#include <QtCore/qrect.h>
#include <QtCore/qxmlstream.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QSvgRendererPrivate;
class QPainter;
class QByteArray;
class QTransform;

class Q_SVG_EXPORT QSvgRenderer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QRectF viewBox READ viewBoxF WRITE setViewBox)
    Q_PROPERTY(int framesPerSecond READ framesPerSecond WRITE setFramesPerSecond)
    Q_PROPERTY(int currentFrame READ currentFrame WRITE setCurrentFrame)
    Q_PROPERTY(bool animationEnabled READ isAnimationEnabled WRITE setAnimationEnabled)
public:
    QSvgRenderer(QObject *parent=nullptr);
    QSvgRenderer(const QString &filename, QObject *parent=nullptr);
    QSvgRenderer(const QByteArray &contents, QObject *parent=nullptr);
    QSvgRenderer(QXmlStreamReader *contents, QObject *parent=nullptr);
    ~QSvgRenderer() override;

    bool isValid() const;

    QSize defaultSize() const;

    QRect viewBox() const;
    QRectF viewBoxF() const;
    void setViewBox(const QRect &viewbox);
    void setViewBox(const QRectF &viewbox);

    bool animated() const;
    int framesPerSecond() const;
    void setFramesPerSecond(int num);
    int currentFrame() const;
    void setCurrentFrame(int);
    int animationDuration() const;//in seconds
    bool isAnimationEnabled() const;
    void setAnimationEnabled(bool enable);

    QRectF boundsOnElement(const QString &id) const;
    [[deprecated("Use transformForElement()")]]
    bool elementExists(const QString &id) const;
    QMatrix matrixForElement(const QString &id) const;
    QTransform transformForElement(const QString &id) const;

public Q_SLOTS:
    bool load(const QString &filename);
    bool load(const QByteArray &contents);
    bool load(QXmlStreamReader *contents);
    void render(QPainter *p);
    void render(QPainter *p, const QRectF &bounds);

    void render(QPainter *p, const QString &elementId,
                const QRectF &bounds=QRectF());

Q_SIGNALS:
    void repaintNeeded();

private:
    Q_DECLARE_PRIVATE(QSvgRenderer)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_SVGRENDERER
#endif // QSVGRENDERER_H

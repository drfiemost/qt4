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

#include "qsvgiohandler.h"

#ifndef QT_NO_SVGRENDERER

#include "qsvgrenderer.h"
#include "qimage.h"
#include "qpixmap.h"
#include "qpainter.h"
#include "qvariant.h"
#include "qbuffer.h"
#include "qdebug.h"

QT_BEGIN_NAMESPACE

class QSvgIOHandlerPrivate
{
public:
    QSvgIOHandlerPrivate(QSvgIOHandler *qq)
        : q(qq), loadAttempted(false), loadStatus(false), readDone(false), backColor(Qt::transparent)
    {}

    bool load(QIODevice *device);

    QSvgIOHandler   *q;
    QSvgRenderer     r;
    QXmlStreamReader xmlReader;
    QSize            defaultSize;
    QRect            clipRect;
    QSize            scaledSize;
    QRect            scaledClipRect;
    bool             loadAttempted;
    bool             loadStatus;
    bool             readDone;
    QColor           backColor;
};


bool QSvgIOHandlerPrivate::load(QIODevice *device)
{
    if (loadAttempted)
        return loadStatus;
    loadAttempted = true;
    if (q->format().isEmpty())
        q->canRead();

    // # The SVG renderer doesn't handle trailing, unrelated data, so we must
    // assume that all available data in the device is to be read.
    bool res = false;
    QBuffer *buf = qobject_cast<QBuffer *>(device);
    if (buf) {
        const QByteArray &ba = buf->data();
        res = r.load(QByteArray::fromRawData(ba.constData() + buf->pos(), ba.size() - buf->pos()));
        buf->seek(ba.size());
    } else if (q->format() == "svgz") {
        res = r.load(device->readAll());
    } else {
        xmlReader.setDevice(device);
        res = r.load(&xmlReader);
    }

    if (res) {
        defaultSize = r.defaultSize();
        loadStatus = true;
    }

    return loadStatus;
}


QSvgIOHandler::QSvgIOHandler()
    : d(new QSvgIOHandlerPrivate(this))
{

}

static bool isPossiblySvg(QIODevice *device, bool *isCompressed = nullptr)
{
    constexpr int bufSize = 64;
    char buf[bufSize];
    const qint64 readLen = device->peek(buf, bufSize);
    if (readLen < 8)
        return false;
#    ifndef QT_NO_COMPRESS
    if (quint8(buf[0]) == 0x1f && quint8(buf[1]) == 0x8b) {
        if (isCompressed)
            *isCompressed = true;
        return true;
    }
#    endif
    QTextStream str(QByteArray::fromRawData(buf, readLen));
    QByteArray ba = str.read(16).trimmed().toLatin1();
    return ba.startsWith("<?xml") || ba.startsWith("<svg") || ba.startsWith("<!--") || ba.startsWith("<!DOCTYPE svg");
}

QSvgIOHandler::~QSvgIOHandler()
{
    delete d;
}


bool QSvgIOHandler::canRead() const
{
    if (!device())
        return false;
    if (d->loadStatus && !d->readDone)
        return true;        // Will happen if we have been asked for the size

    bool isCompressed = false;
    if (isPossiblySvg(device(), &isCompressed)) {
        setFormat(isCompressed ? "svgz" : "svg");
        return true;
    }
    return false;
}


QByteArray QSvgIOHandler::name() const
{
    return "svg";
}


bool QSvgIOHandler::read(QImage *image)
{
    if (!d->readDone && d->load(device())) {
        bool xform = (d->clipRect.isValid() || d->scaledSize.isValid() || d->scaledClipRect.isValid());
        QSize finalSize = d->defaultSize;
        QRectF bounds;
        if (xform && !d->defaultSize.isEmpty()) {
            bounds = QRectF(QPointF(0,0), QSizeF(d->defaultSize));
            QPoint tr1, tr2;
            QSizeF sc(1, 1);
            if (d->clipRect.isValid()) {
                tr1 = -d->clipRect.topLeft();
                finalSize = d->clipRect.size();
            }
            if (d->scaledSize.isValid()) {
                sc = QSizeF(qreal(d->scaledSize.width()) / finalSize.width(),
                            qreal(d->scaledSize.height()) / finalSize.height());
                finalSize = d->scaledSize;
            }
            if (d->scaledClipRect.isValid()) {
                tr2 = -d->scaledClipRect.topLeft();
                finalSize = d->scaledClipRect.size();
            }
            QTransform t;
            t.translate(tr2.x(), tr2.y());
            t.scale(sc.width(), sc.height());
            t.translate(tr1.x(), tr1.y());
            bounds = t.mapRect(bounds);
        }
        if (finalSize.isEmpty()) {
            *image = QImage();
        } else {
            if (std::max(finalSize.width(), finalSize.height()) > 0xffff)
                return false; // Assume corrupted file
            if (image->size() != finalSize || !image->reinterpretAsFormat(QImage::Format_ARGB32_Premultiplied))
                *image = QImage(finalSize, QImage::Format_ARGB32_Premultiplied);
            if (image->isNull()) {
                qWarning("QSvgIOHandler: QImage allocation failed (size %i x %i)", finalSize.width(), finalSize.height());
                return false;
            }
            image->fill(d->backColor.rgba());
            QPainter p(image);
            d->r.render(&p, bounds);
            p.end();
        }
        d->readDone = true;
        return true;
    }

    return false;
}


QVariant QSvgIOHandler::option(ImageOption option) const
{
    switch(option) {
    case ImageFormat:
        return QImage::Format_ARGB32_Premultiplied;
        break;
    case Size:
        d->load(device());
        return d->defaultSize;
        break;
    case ClipRect:
        return d->clipRect;
        break;
    case ScaledSize:
        return d->scaledSize;
        break;
    case ScaledClipRect:
        return d->scaledClipRect;
        break;
    case BackgroundColor:
        return d->backColor;
        break;
    default:
        break;
    }
    return QVariant();
}


void QSvgIOHandler::setOption(ImageOption option, const QVariant & value)
{
    switch(option) {
    case ClipRect:
        d->clipRect = value.toRect();
        break;
    case ScaledSize:
        d->scaledSize = value.toSize();
        break;
    case ScaledClipRect:
        d->scaledClipRect = value.toRect();
        break;
    case BackgroundColor:
        d->backColor = value.value<QColor>();
        break;
    default:
        break;
    }
}


bool QSvgIOHandler::supportsOption(ImageOption option) const
{
    switch(option)
    {
    case ImageFormat:
    case Size:
    case ClipRect:
    case ScaledSize:
    case ScaledClipRect:
    case BackgroundColor:
        return true;
    default:
        break;
    }
    return false;
}


bool QSvgIOHandler::canRead(QIODevice *device)
{
    return isPossiblySvg(device);
}

QT_END_NAMESPACE

#endif // QT_NO_SVGRENDERER

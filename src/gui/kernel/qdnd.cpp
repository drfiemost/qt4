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

#include "qplatformdefs.h"

#include "qapplication.h"
#include "qbitmap.h"
#include "qbuffer.h"
#include "qdebug.h"
#include "qdir.h"
#include "qdnd_p.h"
#include "qdrag.h"
#include "qevent.h"
#include "qfile.h"
#include "qimage.h"
#include "qimagereader.h"
#include "qimagewriter.h"
#include "qpixmap.h"
#include "qpoint.h"
#include "qregexp.h"
#include "qtextcodec.h"
#include "qwidget.h"
#include <cctype>

#include <private/qapplication_p.h>

#ifndef QT_NO_DRAGANDDROP

QT_BEGIN_NAMESPACE

#ifndef QT_NO_DRAGANDDROP

//#define QDND_DEBUG

#ifdef QDND_DEBUG
QString dragActionsToString(Qt::DropActions actions)
{
    QString str;
    if (actions == Qt::IgnoreAction) {
        if (!str.isEmpty())
            str += " | ";
        str += "IgnoreAction";
    }
    if (actions & Qt::LinkAction) {
        if (!str.isEmpty())
            str += " | ";
        str += "LinkAction";
    }
    if (actions & Qt::CopyAction) {
        if (!str.isEmpty())
            str += " | ";
        str += "CopyAction";
    }
    if (actions & Qt::MoveAction) {
        if (!str.isEmpty())
            str += " | ";
        str += "MoveAction";
    }
    if ((actions & Qt::TargetMoveAction) == Qt::TargetMoveAction ) {
        if (!str.isEmpty())
            str += " | ";
        str += "TargetMoveAction";
    }
    return str;
}

QString KeyboardModifiersToString(Qt::KeyboardModifiers moderfies)
{
    QString str;
    if (moderfies & Qt::ControlModifier) {
        if (!str.isEmpty())
            str += " | ";
        str += Qt::ControlModifier;
    }
    if (moderfies & Qt::AltModifier) {
        if (!str.isEmpty())
            str += " | ";
        str += Qt::AltModifier;
    }
    if (moderfies & Qt::ShiftModifier) {
        if (!str.isEmpty())
            str += " | ";
        str += Qt::ShiftModifier;
    }
    return str;
}
#endif


// the universe's only drag manager
QDragManager *QDragManager::instance = nullptr;


QDragManager::QDragManager()
    : QObject(qApp)
{
    Q_ASSERT(!instance);

#ifdef Q_WS_QWS
    currentActionForOverrideCursor = Qt::IgnoreAction;
#endif
    object = nullptr;
    beingCancelled = false;
    restoreCursor = false;
    willDrop = false;
    eventLoop = nullptr;
    dropData = new QDropData();
    currentDropTarget = nullptr;
#ifdef Q_WS_X11
    xdndMimeTransferedPixmapIndex = 0;
#endif
}


QDragManager::~QDragManager()
{
#ifndef QT_NO_CURSOR
    if (restoreCursor)
        QApplication::restoreOverrideCursor();
#endif
    instance = nullptr;
    delete dropData;
}

QDragManager *QDragManager::self()
{
    if (!instance && !QApplication::closingDown())
        instance = new QDragManager;
    return instance;
}

QPixmap QDragManager::dragCursor(Qt::DropAction action) const
{
    QDragPrivate * d = dragPrivate();
    if (d && d->customCursors.contains(action))
        return d->customCursors[action];
    else if (action == Qt::MoveAction)
        return QApplicationPrivate::instance()->getPixmapCursor(Qt::DragMoveCursor);
    else if (action == Qt::CopyAction)
        return QApplicationPrivate::instance()->getPixmapCursor(Qt::DragCopyCursor);
    else if (action == Qt::LinkAction)
        return QApplicationPrivate::instance()->getPixmapCursor(Qt::DragLinkCursor);
#ifdef Q_WS_WIN
    else if (action == Qt::IgnoreAction)
        return QApplicationPrivate::instance()->getPixmapCursor(Qt::ForbiddenCursor);
#endif
    return QPixmap();
}

bool QDragManager::hasCustomDragCursors() const
{
    QDragPrivate * d = dragPrivate();
    return d && !d->customCursors.isEmpty();
}

Qt::DropAction QDragManager::defaultAction(Qt::DropActions possibleActions,
                                           Qt::KeyboardModifiers modifiers) const
{
#ifdef QDND_DEBUG
    qDebug("QDragManager::defaultAction(Qt::DropActions possibleActions)");
    qDebug("keyboard modifiers : %s", qPrintable(KeyboardModifiersToString(modifiers)));
#endif

    QDragPrivate *d = dragPrivate();
    Qt::DropAction defaultAction = d ? d->defaultDropAction : Qt::IgnoreAction;

    if (defaultAction == Qt::IgnoreAction) {
        //This means that the drag was initiated by QDrag::start and we need to
        //preserve the old behavior
#ifdef Q_WS_MAC
        defaultAction = Qt::MoveAction;
#else
        defaultAction = Qt::CopyAction;
#endif
    }

#ifdef Q_WS_MAC
    if (modifiers & Qt::ControlModifier && modifiers & Qt::AltModifier)
        defaultAction = Qt::LinkAction;
    else if (modifiers & Qt::AltModifier)
        defaultAction = Qt::CopyAction;
    else if (modifiers & Qt::ControlModifier)
        defaultAction = Qt::MoveAction;
#else
    if (modifiers & Qt::ControlModifier && modifiers & Qt::ShiftModifier)
        defaultAction = Qt::LinkAction;
    else if (modifiers & Qt::ControlModifier)
        defaultAction = Qt::CopyAction;
    else if (modifiers & Qt::ShiftModifier)
        defaultAction = Qt::MoveAction;
    else if (modifiers & Qt::AltModifier)
        defaultAction = Qt::LinkAction;
#endif

    // if the object is set take the list of possibles from it
    if (object)
        possibleActions = object->d_func()->possible_actions;

#ifdef QDND_DEBUG
    qDebug("possible actions : %s", qPrintable(dragActionsToString(possibleActions)));
#endif

    // Check if the action determined is allowed
    if (!(possibleActions & defaultAction)) {
        if (possibleActions & Qt::CopyAction)
            defaultAction = Qt::CopyAction;
        else if (possibleActions & Qt::MoveAction)
            defaultAction = Qt::MoveAction;
        else if (possibleActions & Qt::LinkAction)
            defaultAction = Qt::LinkAction;
        else
            defaultAction = Qt::IgnoreAction;
    }

#ifdef QDND_DEBUG
    qDebug("default action : %s", qPrintable(dragActionsToString(defaultAction)));
#endif

    return defaultAction;
}

void QDragManager::setCurrentTarget(QWidget *target, bool dropped)
{
    if (currentDropTarget == target)
        return;

    currentDropTarget = target;
    if (!dropped && object) {
        object->d_func()->target = target;
        emit object->targetChanged(target);
    }

}

QWidget *QDragManager::currentTarget()
{
    return currentDropTarget;
}

#endif

QDropData::QDropData()
    : QInternalMimeData()
{
}

QDropData::~QDropData()
{
}
#endif // QT_NO_DRAGANDDROP

#if !(defined(QT_NO_DRAGANDDROP) && defined(QT_NO_CLIPBOARD))

static QStringList imageReadMimeFormats()
{
    QStringList formats;
    QList<QByteArray> imageFormats = QImageReader::supportedImageFormats();
    for (const auto & imageFormat : imageFormats) {
        QString format = QLatin1String("image/");
        format += QString::fromLatin1(imageFormat.toLower());
        formats.append(format);
    }

    //put png at the front because it is best
    int pngIndex = formats.indexOf(QLatin1String("image/png"));
    if (pngIndex != -1 && pngIndex != 0)
        formats.move(pngIndex, 0);

    return formats;
}


static QStringList imageWriteMimeFormats()
{
    QStringList formats;
    QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();
    for (const auto & imageFormat : imageFormats) {
        QString format = QLatin1String("image/");
        format += QString::fromLatin1(imageFormat.toLower());
        formats.append(format);
    }

    //put png at the front because it is best
    int pngIndex = formats.indexOf(QLatin1String("image/png"));
    if (pngIndex != -1 && pngIndex != 0)
        formats.move(pngIndex, 0);

    return formats;
}

QInternalMimeData::QInternalMimeData()
    : QMimeData()
{
}

QInternalMimeData::~QInternalMimeData()
= default;

bool QInternalMimeData::hasFormat(const QString &mimeType) const
{
    bool foundFormat = hasFormat_sys(mimeType);
    if (!foundFormat && mimeType == QLatin1String("application/x-qt-image")) {
        QStringList imageFormats = imageReadMimeFormats();
        for (const auto & imageFormat : imageFormats) {
            if ((foundFormat = hasFormat_sys(imageFormat)))
                break;
        }
    }
    return foundFormat;
}

QStringList QInternalMimeData::formats() const
{
    QStringList realFormats = formats_sys();
    if (!realFormats.contains(QLatin1String("application/x-qt-image"))) {
        QStringList imageFormats = imageReadMimeFormats();
        for (const auto & imageFormat : imageFormats) {
            if (realFormats.contains(imageFormat)) {
                realFormats += QLatin1String("application/x-qt-image");
                break;
            }
        }
    }
    return realFormats;
}

QVariant QInternalMimeData::retrieveData(const QString &mimeType, QVariant::Type type) const
{
    QVariant data = retrieveData_sys(mimeType, type);
    if (mimeType == QLatin1String("application/x-qt-image")) {
        if (data.isNull() || (data.type() == QVariant::ByteArray && data.toByteArray().isEmpty())) {
            // try to find an image
            QStringList imageFormats = imageReadMimeFormats();
            for (const auto & imageFormat : imageFormats) {
                data = retrieveData_sys(imageFormat, type);
                if (data.isNull() || (data.type() == QVariant::ByteArray && data.toByteArray().isEmpty()))
                    continue;
                break;
            }
        }
        // we wanted some image type, but all we got was a byte array. Convert it to an image.
        if (data.type() == QVariant::ByteArray
            && (type == QVariant::Image || type == QVariant::Pixmap || type == QVariant::Bitmap))
            data = QImage::fromData(data.toByteArray());

    } else if (mimeType == QLatin1String("application/x-color") && data.type() == QVariant::ByteArray) {
        QColor c;
        QByteArray ba = data.toByteArray();
        if (ba.size() == 8) {
            ushort * colBuf = (ushort *)ba.data();
            c.setRgbF(qreal(colBuf[0]) / qreal(0xFFFF),
                      qreal(colBuf[1]) / qreal(0xFFFF),
                      qreal(colBuf[2]) / qreal(0xFFFF),
                      qreal(colBuf[3]) / qreal(0xFFFF));
            data = c;
        } else {
            qWarning("Qt: Invalid color format");
        }
    } else if (data.type() != type && data.type() == QVariant::ByteArray) {
        // try to use mime data's internal conversion stuf.
        QInternalMimeData *that = const_cast<QInternalMimeData *>(this);
        that->setData(mimeType, data.toByteArray());
        data = QMimeData::retrieveData(mimeType, type);
        that->clear();
    }
    return data;
}

bool QInternalMimeData::canReadData(const QString &mimeType)
{
    return imageReadMimeFormats().contains(mimeType);
}

// helper functions for rendering mimedata to the system, this is needed because QMimeData is in core.
QStringList QInternalMimeData::formatsHelper(const QMimeData *data)
{
    QStringList realFormats = data->formats();
    if (realFormats.contains(QLatin1String("application/x-qt-image"))) {
        // add all supported image formats
        QStringList imageFormats = imageWriteMimeFormats();
        for (const auto & imageFormat : imageFormats) {
            if (!realFormats.contains(imageFormat))
                realFormats.append(imageFormat);
        }
    }
    return realFormats;
}

bool QInternalMimeData::hasFormatHelper(const QString &mimeType, const QMimeData *data)
{

    bool foundFormat = data->hasFormat(mimeType);
    if (!foundFormat) {
        if (mimeType == QLatin1String("application/x-qt-image")) {
            // check all supported image formats
            QStringList imageFormats = imageWriteMimeFormats();
            for (const auto & imageFormat : imageFormats) {
                if ((foundFormat = data->hasFormat(imageFormat)))
                    break;
            }
        } else if (mimeType.startsWith(QLatin1String("image/"))) {
            return data->hasImage() && imageWriteMimeFormats().contains(mimeType);
        }
    }
    return foundFormat;
}

QByteArray QInternalMimeData::renderDataHelper(const QString &mimeType, const QMimeData *data)
{
    QByteArray ba;
    if (mimeType == QLatin1String("application/x-color")) {
        /* QMimeData can only provide colors as QColor or the name
           of a color as a QByteArray or a QString. So we need to do
           the conversion to application/x-color here.
           The application/x-color format is :
           type: application/x-color
           format: 16
           data[0]: red
           data[1]: green
           data[2]: blue
           data[3]: opacity
        */
        ba.resize(8);
        ushort * colBuf = (ushort *)ba.data();
        QColor c = qvariant_cast<QColor>(data->colorData());
        colBuf[0] = ushort(c.redF() * 0xFFFF);
        colBuf[1] = ushort(c.greenF() * 0xFFFF);
        colBuf[2] = ushort(c.blueF() * 0xFFFF);
        colBuf[3] = ushort(c.alphaF() * 0xFFFF);
    } else {
        ba = data->data(mimeType);
        if (ba.isEmpty()) {
            if (mimeType == QLatin1String("application/x-qt-image") && data->hasImage()) {
                QImage image = qvariant_cast<QImage>(data->imageData());
                QBuffer buf(&ba);
                buf.open(QBuffer::WriteOnly);
                // would there not be PNG ??
                image.save(&buf, "PNG");
            } else if (mimeType.startsWith(QLatin1String("image/")) && data->hasImage()) {
                QImage image = qvariant_cast<QImage>(data->imageData());
                QBuffer buf(&ba);
                buf.open(QBuffer::WriteOnly);
                image.save(&buf, mimeType.mid(mimeType.indexOf(QLatin1Char('/')) + 1).toLatin1().toUpper());
            }
        }
    }
    return ba;
}

#endif // QT_NO_DRAGANDDROP && QT_NO_CLIPBOARD

QT_END_NAMESPACE

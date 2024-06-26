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

#ifndef QPRINTENGINE_PDF_P_H
#define QPRINTENGINE_PDF_P_H

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

#include "QtGui/qprintengine.h"

#ifndef QT_NO_PRINTER
#include "QtCore/qmap.h"
#include "QtGui/qmatrix.h"
#include "QtCore/qstring.h"
#include "QtCore/qvector.h"
#include "QtGui/qpaintengine.h"
#include "QtGui/qpainterpath.h"
#include "QtCore/qdatastream.h"

#include "private/qfontengine_p.h"
#include "private/qpdf_p.h"
#include "private/qpaintengine_p.h"

QT_BEGIN_NAMESPACE

// #define USE_NATIVE_GRADIENTS

class QImage;
class QDataStream;
class QPen;
class QPointF;
class QRegion;
class QFile;
class QPdfEngine;

class QPdfEnginePrivate;

class QPdfEngine : public QPdfBaseEngine
{
    Q_DECLARE_PRIVATE(QPdfEngine)
public:
    QPdfEngine(QPrinter::PrinterMode m);
    ~QPdfEngine() override;

    // reimplementations QPaintEngine
    bool begin(QPaintDevice *pdev) override;
    bool end() override;
    void drawPixmap (const QRectF & rectangle, const QPixmap & pixmap, const QRectF & sr) override;
    void drawImage(const QRectF &r, const QImage &pm, const QRectF &sr,
                   Qt::ImageConversionFlags flags = Qt::AutoColor) override;
    void drawTiledPixmap (const QRectF & rectangle, const QPixmap & pixmap, const QPointF & point) override;

    Type type() const override;
    // end reimplementations QPaintEngine

    // reimplementations QPrintEngine
    bool abort() override {return false;}
    bool newPage() override;
    QPrinter::PrinterState printerState() const override {return state;}
    // end reimplementations QPrintEngine

    void setBrush() override;

    // ### unused, should have something for this in QPrintEngine
    void setAuthor(const QString &author);
    QString author() const;

    void setDevice(QIODevice* dev);

private:
    Q_DISABLE_COPY(QPdfEngine)

    QPrinter::PrinterState state;
};

class QPdfEnginePrivate : public QPdfBaseEnginePrivate
{
    Q_DECLARE_PUBLIC(QPdfEngine)
public:
    QPdfEnginePrivate(QPrinter::PrinterMode m);
    ~QPdfEnginePrivate() override;

    void newPage();

    int width() const {
        QRect r = paperRect();
        return qRound(r.width()*72./resolution);
    }
    int height() const {
        QRect r = paperRect();
        return qRound(r.height()*72./resolution);
    }

    void writeHeader();
    void writeTail();

    int addImage(const QImage &image, bool *bitmap, qint64 serial_no);
    int addConstantAlphaObject(int brushAlpha, int penAlpha = 255);
    int addBrushPattern(const QTransform &matrix, bool *specifyColor, int *gStateObject);

    void drawTextItem(const QPointF &p, const QTextItemInt &ti) override;

    QTransform pageMatrix() const;

private:
    Q_DISABLE_COPY(QPdfEnginePrivate)

#ifdef USE_NATIVE_GRADIENTS
    int gradientBrush(const QBrush &b, const QMatrix &matrix, int *gStateObject);
#endif

    void writeInfo();
    void writePageRoot();
    void writeFonts();
    void embedFont(QFontSubset *font);

    QVector<int> xrefPositions;
    QDataStream* stream;
    int streampos;

    int writeImage(const QByteArray &data, int width, int height, int depth,
                   int maskObject, int softMaskObject, bool dct = false);
    void writePage();

    int addXrefEntry(int object, bool printostr = true);
    void printString(const QString &string);
    void xprintf(const char* fmt, ...);
    inline void write(const QByteArray &data) {
        stream->writeRawData(data.constData(), data.size());
        streampos += data.size();
    }

    int writeCompressed(const char *src, int len);
    inline int writeCompressed(const QByteArray &data) { return writeCompressed(data.constData(), data.length()); }
    int writeCompressed(QIODevice *dev);

    // various PDF objects
    int pageRoot, catalog, info, graphicsState, patternColorSpace;
    QVector<uint> pages;
    QHash<qint64, uint> imageCache;
    QHash<QPair<uint, uint>, uint > alphaCache;
};

QT_END_NAMESPACE

#endif // QT_NO_PRINTER

#endif // QPRINTENGINE_PDF_P_H

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

#ifndef QPIXMAP_H
#define QPIXMAP_H

#include <QtGui/qpaintdevice.h>
#include <QtGui/qcolor.h>
#include <QtCore/qnamespace.h>
#include <QtCore/qstring.h> // char*->QString conversion
#include <QtCore/qsharedpointer.h>
#include <QtGui/qimage.h>
#include <QtGui/qtransform.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QImageWriter;
class QImageReader;
class QColor;
class QVariant;
class QX11Info;
class QPixmapData;

class Q_GUI_EXPORT QPixmap : public QPaintDevice
{
public:
    QPixmap();
    explicit QPixmap(QPixmapData *data);
    QPixmap(int w, int h);
    QPixmap(const QSize &);
    QPixmap(const QString& fileName, const char *format = nullptr, Qt::ImageConversionFlags flags = Qt::AutoColor);
#ifndef QT_NO_IMAGEFORMAT_XPM
    QPixmap(const char * const xpm[]);
#endif
    QPixmap(const QPixmap &);
    ~QPixmap() override;

    QPixmap &operator=(const QPixmap &);
#ifdef Q_COMPILER_RVALUE_REFS
    inline QPixmap &operator=(QPixmap &&other)
    { qSwap(data, other.data); return *this; }
#endif
    inline void swap(QPixmap &other) { qSwap(data, other.data); }

    operator QVariant() const;

    bool isNull() const; // ### Qt 5: make inline
    int devType() const override;

    int width() const; // ### Qt 5: make inline
    int height() const; // ### Qt 5: make inline
    QSize size() const;
    QRect rect() const;
    int depth() const;

    static int defaultDepth();

    void fill(const QColor &fillColor = Qt::white);
    void fill(const QWidget *widget, const QPoint &ofs);
    inline void fill(const QWidget *widget, int xofs, int yofs) { fill(widget, QPoint(xofs, yofs)); }

    QBitmap mask() const;
    void setMask(const QBitmap &);

#ifdef QT_DEPRECATED
    QT_DEPRECATED QPixmap alphaChannel() const;
    QT_DEPRECATED void setAlphaChannel(const QPixmap &);
#endif

    bool hasAlpha() const;
    bool hasAlphaChannel() const;

#ifndef QT_NO_IMAGE_HEURISTIC_MASK
    QBitmap createHeuristicMask(bool clipTight = true) const;
#endif
    QBitmap createMaskFromColor(const QColor &maskColor) const; // ### Qt 5: remove
    QBitmap createMaskFromColor(const QColor &maskColor, Qt::MaskMode mode) const;

    static QPixmap grabWindow(WId, int x=0, int y=0, int w=-1, int h=-1);
    static QPixmap grabWidget(QWidget *widget, const QRect &rect);
    static inline QPixmap grabWidget(QWidget *widget, int x=0, int y=0, int w=-1, int h=-1)
    { return grabWidget(widget, QRect(x, y, w, h)); }


    inline QPixmap scaled(int w, int h, Qt::AspectRatioMode aspectMode = Qt::IgnoreAspectRatio,
                          Qt::TransformationMode mode = Qt::FastTransformation) const
        { return scaled(QSize(w, h), aspectMode, mode); }
    QPixmap scaled(const QSize &s, Qt::AspectRatioMode aspectMode = Qt::IgnoreAspectRatio,
                   Qt::TransformationMode mode = Qt::FastTransformation) const;
    QPixmap scaledToWidth(int w, Qt::TransformationMode mode = Qt::FastTransformation) const;
    QPixmap scaledToHeight(int h, Qt::TransformationMode mode = Qt::FastTransformation) const;
    QPixmap transformed(const QMatrix &, Qt::TransformationMode mode = Qt::FastTransformation) const;
    static QMatrix trueMatrix(const QMatrix &m, int w, int h);
    QPixmap transformed(const QTransform &, Qt::TransformationMode mode = Qt::FastTransformation) const;
    static QTransform trueMatrix(const QTransform &m, int w, int h);

    QImage toImage() const;
    static QPixmap fromImage(const QImage &image, Qt::ImageConversionFlags flags = Qt::AutoColor);
    static QPixmap fromImageReader(QImageReader *imageReader, Qt::ImageConversionFlags flags = Qt::AutoColor);

    bool load(const QString& fileName, const char *format = nullptr, Qt::ImageConversionFlags flags = Qt::AutoColor);
    bool loadFromData(const uchar *buf, uint len, const char* format = nullptr, Qt::ImageConversionFlags flags = Qt::AutoColor);
    inline bool loadFromData(const QByteArray &data, const char* format = nullptr, Qt::ImageConversionFlags flags = Qt::AutoColor);
    bool save(const QString& fileName, const char* format = nullptr, int quality = -1) const;
    bool save(QIODevice* device, const char* format = nullptr, int quality = -1) const;

    bool convertFromImage(const QImage &img, Qt::ImageConversionFlags flags = Qt::AutoColor);

#if defined(Q_WS_WIN)
    enum HBitmapFormat {
        NoAlpha,
        PremultipliedAlpha,
        Alpha
    };

    HBITMAP toWinHBITMAP(HBitmapFormat format = NoAlpha) const;
    HICON toWinHICON() const;

    static QPixmap fromWinHBITMAP(HBITMAP hbitmap, HBitmapFormat format = NoAlpha);
    static QPixmap fromWinHICON(HICON hicon);
#endif

#if defined(Q_WS_MAC)
    CGImageRef toMacCGImageRef() const;
    static QPixmap fromMacCGImageRef(CGImageRef image);
#endif

    inline QPixmap copy(int x, int y, int width, int height) const;
    QPixmap copy(const QRect &rect = QRect()) const;

    inline void scroll(int dx, int dy, int x, int y, int width, int height, QRegion *exposed = nullptr);
    void scroll(int dx, int dy, const QRect &rect, QRegion *exposed = nullptr);

#ifdef QT_DEPRECATED
    QT_DEPRECATED int serialNumber() const;
#endif
    qint64 cacheKey() const;

    bool isDetached() const;
    void detach();

    bool isQBitmap() const;

#if defined(Q_WS_QWS)
    const uchar *qwsBits() const;
    int qwsBytesPerLine() const;
    QRgb *clut() const;
#ifdef QT_DEPRECATED
    QT_DEPRECATED int numCols() const;
#endif
    int colorCount() const;
#elif defined(Q_WS_MAC)
    Qt::HANDLE macQDHandle() const;
    Qt::HANDLE macQDAlphaHandle() const;
    Qt::HANDLE macCGHandle() const;
#elif defined(Q_WS_X11)
    enum ShareMode { ImplicitlyShared, ExplicitlyShared };

    static QPixmap fromX11Pixmap(Qt::HANDLE pixmap, ShareMode mode = ImplicitlyShared);
    static int x11SetDefaultScreen(int screen);
    void x11SetScreen(int screen);
    const QX11Info &x11Info() const;
    Qt::HANDLE x11PictureHandle() const;
#endif

#if defined(Q_WS_X11) || defined(Q_WS_QWS)
    Qt::HANDLE handle() const;
#endif

    QPaintEngine *paintEngine() const override;

    inline bool operator!() const { return isNull(); }

protected:
    int metric(PaintDeviceMetric) const override;

private:
    QExplicitlySharedDataPointer<QPixmapData> data;

    bool doImageIO(QImageWriter *io, int quality) const;

    // ### Qt5: remove the following three lines
    enum Type { PixmapType, BitmapType }; // must match QPixmapData::PixelType
    QPixmap(const QSize &s, Type);
    void init(int, int, Type = PixmapType);

    QPixmap(const QSize &s, int type);
    void init(int, int, int);
    void deref();
#if defined(Q_WS_WIN)
    void initAlphaPixmap(uchar *bytes, int length, struct tagBITMAPINFO *bmi);
#endif
    Q_DUMMY_COMPARISON_OPERATOR(QPixmap)
#ifdef Q_WS_MAC
    friend CGContextRef qt_mac_cg_context(const QPaintDevice*);
    friend CGImageRef qt_mac_create_imagemask(const QPixmap&, const QRectF&);
    friend IconRef qt_mac_create_iconref(const QPixmap&);
    friend quint32 *qt_mac_pixmap_get_base(const QPixmap*);
    friend int qt_mac_pixmap_get_bytes_per_line(const QPixmap*);
#endif
    friend class QPixmapData;
    friend class QX11PixmapData;
    friend class QMacPixmapData;
    friend class QSymbianRasterPixmapData;
    friend class QBitmap;
    friend class QPaintDevice;
    friend class QPainter;
    friend class QGLWidget;
    friend class QX11PaintEngine;
    friend class QCoreGraphicsPaintEngine;
    friend class QWidgetPrivate;
    friend class QRasterBuffer;
#if !defined(QT_NO_DATASTREAM)
    friend Q_GUI_EXPORT QDataStream &operator>>(QDataStream &, QPixmap &);
#endif
    friend Q_GUI_EXPORT qint64 qt_pixmap_id(const QPixmap &pixmap);

public:
    QPixmapData* pixmapData() const;

public:
    typedef QExplicitlySharedDataPointer<QPixmapData> DataPtr;
    inline DataPtr &data_ptr() { return data; }
};

Q_DECLARE_SHARED(QPixmap)

inline QPixmap QPixmap::copy(int ax, int ay, int awidth, int aheight) const
{
    return copy(QRect(ax, ay, awidth, aheight));
}

inline void QPixmap::scroll(int dx, int dy, int ax, int ay, int awidth, int aheight, QRegion *exposed)
{
    scroll(dx, dy, QRect(ax, ay, awidth, aheight), exposed);
}

inline bool QPixmap::loadFromData(const QByteArray &buf, const char *format,
                                  Qt::ImageConversionFlags flags)
{
    return loadFromData(reinterpret_cast<const uchar *>(buf.constData()), buf.size(), format, flags);
}

/*****************************************************************************
 QPixmap stream functions
*****************************************************************************/

#if !defined(QT_NO_DATASTREAM)
Q_GUI_EXPORT QDataStream &operator<<(QDataStream &, const QPixmap &);
Q_GUI_EXPORT QDataStream &operator>>(QDataStream &, QPixmap &);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QPIXMAP_H

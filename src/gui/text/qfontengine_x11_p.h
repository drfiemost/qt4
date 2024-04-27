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

#ifndef QFONTENGINE_X11_P_H
#define QFONTENGINE_X11_P_H

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
#include <private/qt_x11_p.h>

#include <private/qfontengine_ft_p.h>

QT_BEGIN_NAMESPACE

class QFreetypeFace;

// --------------------------------------------------------------------------

class QFontEngineMultiXLFD : public QFontEngineMulti
{
public:
    QFontEngineMultiXLFD(const QFontDef &r, const QList<int> &l, int s);
    ~QFontEngineMultiXLFD() override;

    void loadEngine(int at) override;

private:
    QList<int> encodings;
    int screen;
    QFontDef request;
};

/**
 * \internal The font engine for X Logical Font Description (XLFD) fonts, which is for X11 systems without freetype.
 */
class QFontEngineXLFD : public QFontEngine
{
public:
    QFontEngineXLFD(XFontStruct *f, const QByteArray &name, int mib);
    ~QFontEngineXLFD() override;

    QFontEngine::FaceId faceId() const override;
    QFontEngine::Properties properties() const override;
    void getUnscaledGlyph(glyph_t glyph, QPainterPath *path, glyph_metrics_t *metrics) override;
    bool getSfntTableData(uint tag, uchar *buffer, uint *length) const override;
    int synthesized() const override;

    bool stringToCMap(const QChar *str, int len, QGlyphLayout *glyphs, int *nglyphs,
                      ShaperFlags flags) const override;
    void recalcAdvances(QGlyphLayout *, ShaperFlags) const override;

    glyph_metrics_t boundingBox(const QGlyphLayout &glyphs) override;
    glyph_metrics_t boundingBox(glyph_t glyph) override;

    void addOutlineToPath(qreal x, qreal y, const QGlyphLayout &glyphs, QPainterPath *path, QTextItem::RenderFlags) override;
    QFixed ascent() const override;
    QFixed descent() const override;
    QFixed leading() const override;
    qreal maxCharWidth() const override;
    qreal minLeftBearing() const override;
    qreal minRightBearing() const override;
    QImage alphaMapForGlyph(glyph_t) override;

    inline Type type() const override
    { return QFontEngine::XLFD; }

    bool canRender(const QChar *string, int len) override;
    const char *name() const override;

    inline XFontStruct *fontStruct() const
    { return _fs; }

#ifndef QT_NO_FREETYPE
    FT_Face non_locked_face() const;
    glyph_t glyphIndexToFreetypeGlyphIndex(glyph_t g) const;
#endif
    uint toUnicode(glyph_t g) const;

private:
    QBitmap bitmapForGlyphs(const QGlyphLayout &glyphs, const glyph_metrics_t &metrics, QTextItem::RenderFlags flags = nullptr);

    XFontStruct *_fs;
    QByteArray _name;
    QTextCodec *_codec;
    int _cmap;
    int lbearing, rbearing;
    mutable QFontEngine::FaceId face_id;
    mutable QFreetypeFace *freetype;
    mutable int synth;
};

#ifndef QT_NO_FONTCONFIG

class Q_GUI_EXPORT QFontEngineMultiFT : public QFontEngineMulti
{
public:
    QFontEngineMultiFT(QFontEngine *fe, FcPattern *firstEnginePattern, FcPattern *p, int s, const QFontDef &request);
    ~QFontEngineMultiFT() override;

    void loadEngine(int at) override;

private:
    QFontDef request;
    FcPattern *pattern;
    FcPattern *firstEnginePattern;
    FcFontSet *fontSet;
    int screen;
    int firstFontIndex; // first font in fontset
};

class Q_GUI_EXPORT QFontEngineX11FT : public QFontEngineFT
{
public:
    explicit QFontEngineX11FT(const QFontDef &fontDef) : QFontEngineFT(fontDef) {}
    explicit QFontEngineX11FT(FcPattern *pattern, const QFontDef &fd, int screen);
    ~QFontEngineX11FT() override;

    QFontEngine *cloneWithSize(qreal pixelSize) const override;

#ifndef QT_NO_XRENDER
    int xglyph_format;
#endif

protected:
    bool uploadGlyphToServer(QGlyphSet *set, uint glyphid, Glyph *g, GlyphInfo *info, int glyphDataSize) const override;
    unsigned long allocateServerGlyphSet() override;
    void freeServerGlyphSet(unsigned long id) override;
};

#endif // QT_NO_FONTCONFIG

QT_END_NAMESPACE

#endif // QFONTENGINE_X11_P_H

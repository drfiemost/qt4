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

#ifndef QSVGHANDLER_P_H
#define QSVGHANDLER_P_H

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

#include "QtXml/qxmlstream.h"

#ifndef QT_NO_SVG

#include "QtCore/qhash.h"
#include "QtCore/qstack.h"
#include "qsvgstyle_p.h"
#include "private/qcssparser_p.h"
#include "private/qsvggraphics_p.h"

QT_BEGIN_NAMESPACE

class QSvgNode;
class QSvgTinyDocument;
class QSvgHandler;
class QColor;
class QSvgStyleSelector;

struct QSvgCssAttribute
{
    QXmlStreamStringRef name;
    QXmlStreamStringRef value;
};

class QSvgHandler
{
public:
    enum LengthType {
        LT_PERCENT,
        LT_PX,
        LT_PC,
        LT_PT,
        LT_MM,
        LT_CM,
        LT_IN,
        LT_OTHER
    };

public:
    QSvgHandler(QIODevice *device);
    QSvgHandler(const QByteArray &data);
    QSvgHandler(QXmlStreamReader *const data);
    ~QSvgHandler();

    QIODevice *device() const;
    QSvgTinyDocument *document() const;

    inline bool ok() const {
        return document() != nullptr && !xml->hasError();
    }

    inline QString errorString() const { return xml->errorString(); }
    inline int lineNumber() const { return xml->lineNumber(); }

    void setDefaultCoordinateSystem(LengthType type);
    LengthType defaultCoordinateSystem() const;

    void pushColor(const QColor &color);
    void pushColorCopy();
    void popColor();
    QColor currentColor() const;

    void setInStyle(bool b);
    bool inStyle() const;

    QSvgStyleSelector *selector() const;

    void setAnimPeriod(int start, int end);
    int animationDuration() const;

    void parseCSStoXMLAttrs(const QString &css, QVector<QSvgCssAttribute> *attributes);

    inline QPen defaultPen() const
    { return m_defaultPen; }

public:
    bool startElement(const QString &localName, const QXmlStreamAttributes &attributes);
    bool endElement(const QStringRef &localName);
    bool characters(const QStringRef &str);
    bool processingInstruction(const QString &target, const QString &data);

private:
    void init();

    QSvgTinyDocument *m_doc;
    QStack<QSvgNode *> m_nodes;
    // TODO: This is only needed during parsing, so it unnecessarily takes up space after that.
    // Temporary container for <use> nodes which haven't been resolved yet.
    QList<QSvgUse *> m_toBeResolved;

    enum CurrentNode
    {
        Unknown,
        Graphics,
        Style
    };
    QStack<CurrentNode> m_skipNodes;

    /*!
        Follows the depths of elements. The top is current xml:space
        value that applies for a given element.
     */
    QStack<QSvgText::WhitespaceMode> m_whitespaceMode;

    QSvgRefCounter<QSvgStyleProperty> m_style;

    LengthType m_defaultCoords;

    QStack<QColor> m_colorStack;
    QStack<int>    m_colorTagCount;

    bool m_inStyle;

    QSvgStyleSelector *m_selector;

    int m_animEnd;

    QXmlStreamReader *const xml;
    QCss::Parser m_cssParser;
    void parse();
    void resolveGradients(QSvgNode *node, int nestedDepth = 0);
    void resolveNodes();

    QPen m_defaultPen;
    /**
     * Whether we own the variable xml, and hence whether
     * we need to delete it.
     */
    const bool m_ownsReader;
};

QT_END_NAMESPACE

#endif // QT_NO_SVG
#endif // QSVGHANDLER_P_H

/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtXmlPatterns module of the Qt Toolkit.
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

#ifndef QXMLSERIALIZER_H
#define QXMLSERIALIZER_H

#include <QtXmlPatterns/QAbstractXmlReceiver>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QIODevice;
class QTextCodec;
class QXmlQuery;
class QXmlSerializerPrivate;

class Q_XMLPATTERNS_EXPORT QXmlSerializer : public QAbstractXmlReceiver
{
public:
    QXmlSerializer(const QXmlQuery &query,
                   QIODevice *outputDevice);

    void namespaceBinding(const QXmlName &nb) override;

    void characters(const QStringRef &value) override;
    void comment(const QString &value) override;

    void startElement(const QXmlName &name) override;

    void endElement() override;

    void attribute(const QXmlName &name,
                           const QStringRef &value) override;

    void processingInstruction(const QXmlName &name,
                                       const QString &value) override;

    void atomicValue(const QVariant &value) override;

    void startDocument() override;
    void endDocument() override;
    void startOfSequence() override;
    void endOfSequence() override;

    QIODevice *outputDevice() const;

#ifndef QT_NO_TEXTCODEC
    void setCodec(const QTextCodec *codec);
    const QTextCodec *codec() const;
#endif

    /* The members below are internal, not part of the public API, and
     * unsupported. Using them leads to undefined behavior. */
    void item(const QPatternist::Item &item) override;
protected:
    QXmlSerializer(QAbstractXmlReceiverPrivate *d);

private:
    inline bool isBindingInScope(const QXmlName nb) const;

    /**
     * Where in the document the QXmlSerializer is currently working.
     */
    enum State
    {
        /**
         * Before the document element. This is the XML prolog where the
         * XML declaration, and possibly comments and processing
         * instructions are found.
         */
        BeforeDocumentElement,

        /**
         * This is inside the document element, at any level.
         */
        InsideDocumentElement
    };

    /**
     * If the current state is neither BeforeDocumentElement or
     * AfterDocumentElement.
     */
    inline bool atDocumentRoot() const;

    /**
     * Closes any open element start tag. Must be called before outputting
     * any element content.
     */
    inline void startContent();

    /**
     * Escapes content intended as text nodes for elements.
     */
    void writeEscaped(const QString &toEscape);

    /**
     * Identical to writeEscaped(), but also escapes quotes.
     */
    inline void writeEscapedAttribute(const QString &toEscape);

    /**
     * Writes out @p name.
     */
    inline void write(const QXmlName &name);

    inline void write(const char *const chars);
    /**
     * Encodes and writes out @p content.
     */
    inline void write(const QString &content);

    Q_DECLARE_PRIVATE(QXmlSerializer)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif

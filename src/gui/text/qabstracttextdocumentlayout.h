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

#ifndef QABSTRACTTEXTDOCUMENTLAYOUT_H
#define QABSTRACTTEXTDOCUMENTLAYOUT_H

#include <QtCore/qobject.h>
#include <QtGui/qtextlayout.h>
#include <QtGui/qtextdocument.h>
#include <QtGui/qtextcursor.h>
#include <QtGui/qpalette.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QAbstractTextDocumentLayoutPrivate;
class QTextBlock;
class QTextObjectInterface;
class QTextFrame;

class Q_GUI_EXPORT QAbstractTextDocumentLayout : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QAbstractTextDocumentLayout)

public:
    explicit QAbstractTextDocumentLayout(QTextDocument *doc);
    ~QAbstractTextDocumentLayout() override;

    struct Selection
    {
        QTextCursor cursor;
        QTextCharFormat format;
    };

    struct PaintContext
    {
        PaintContext()
            : cursorPosition(-1)
            {}
        int cursorPosition;
        QPalette palette;
        QRectF clip;
        QVector<Selection> selections;
    };

    virtual void draw(QPainter *painter, const PaintContext &context) = 0;
    virtual int hitTest(const QPointF &point, Qt::HitTestAccuracy accuracy) const = 0;
    QString anchorAt(const QPointF& pos) const;

    virtual int pageCount() const = 0;
    virtual QSizeF documentSize() const = 0;

    virtual QRectF frameBoundingRect(QTextFrame *frame) const = 0;
    virtual QRectF blockBoundingRect(const QTextBlock &block) const = 0;

    void setPaintDevice(QPaintDevice *device);
    QPaintDevice *paintDevice() const;

    QTextDocument *document() const;

    void registerHandler(int objectType, QObject *component);
    QTextObjectInterface *handlerForObject(int objectType) const;

Q_SIGNALS:
    void update(const QRectF & = QRectF(0., 0., 1000000000., 1000000000.));
    void updateBlock(const QTextBlock &block);
    void documentSizeChanged(const QSizeF &newSize);
    void pageCountChanged(int newPages);

protected:
    QAbstractTextDocumentLayout(QAbstractTextDocumentLayoutPrivate &, QTextDocument *);

    virtual void documentChanged(int from, int charsRemoved, int charsAdded) = 0;

    virtual void resizeInlineObject(QTextInlineObject item, int posInDocument, const QTextFormat &format);
    virtual void positionInlineObject(QTextInlineObject item, int posInDocument, const QTextFormat &format);
    virtual void drawInlineObject(QPainter *painter, const QRectF &rect, QTextInlineObject object, int posInDocument, const QTextFormat &format);

    int formatIndex(int pos);
    QTextCharFormat format(int pos);

private:
    friend class QTextControl;
    friend class QTextDocument;
    friend class QTextDocumentPrivate;
    friend class QTextEngine;
    friend class QTextLayout;
    friend class QTextLine;
    Q_PRIVATE_SLOT(d_func(), void _q_handlerDestroyed(QObject *obj))
    Q_PRIVATE_SLOT(d_func(), int _q_dynamicPageCountSlot())
    Q_PRIVATE_SLOT(d_func(), QSizeF _q_dynamicDocumentSizeSlot())
};

class Q_GUI_EXPORT QTextObjectInterface
{
public:
    virtual ~QTextObjectInterface() = default;
    virtual QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format) = 0;
    virtual void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format) = 0;
};

Q_DECLARE_INTERFACE(QTextObjectInterface, "com.trolltech.Qt.QTextObjectInterface")

QT_END_NAMESPACE

QT_END_HEADER

#endif // QABSTRACTTEXTDOCUMENTLAYOUT_H

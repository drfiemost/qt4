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

#ifndef QLAYOUT_H
#define QLAYOUT_H

#include <QtCore/qobject.h>
#include <QtGui/qlayoutitem.h>
#include <QtGui/qsizepolicy.h>
#include <QtCore/qrect.h>
#include <QtCore/qmargins.h>

#include <limits.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QLayout;
class QSize;


class QLayoutPrivate;

class Q_GUI_EXPORT QLayout : public QObject, public QLayoutItem
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QLayout)

    Q_ENUMS(SizeConstraint)
    Q_PROPERTY(int margin READ margin WRITE setMargin)
    Q_PROPERTY(int spacing READ spacing WRITE setSpacing)
    Q_PROPERTY(SizeConstraint sizeConstraint READ sizeConstraint WRITE setSizeConstraint)
public:
    enum SizeConstraint {
        SetDefaultConstraint,
        SetNoConstraint,
        SetMinimumSize,
        SetFixedSize,
        SetMaximumSize,
        SetMinAndMaxSize
    };

    QLayout(QWidget *parent);
    QLayout();
    ~QLayout() override;

    int margin() const;
    int spacing() const;

    void setMargin(int);
    void setSpacing(int);

    void setContentsMargins(int left, int top, int right, int bottom);
    void setContentsMargins(const QMargins &margins);
    void getContentsMargins(int *left, int *top, int *right, int *bottom) const;
    QMargins contentsMargins() const;
    QRect contentsRect() const;

    bool setAlignment(QWidget *w, Qt::Alignment alignment);
    bool setAlignment(QLayout *l, Qt::Alignment alignment);

    using QLayoutItem::setAlignment;

    void setSizeConstraint(SizeConstraint);
    SizeConstraint sizeConstraint() const;
    void setMenuBar(QWidget *w);
    QWidget *menuBar() const;

    QWidget *parentWidget() const;

    void invalidate() override;
    QRect geometry() const override;
    bool activate();
    void update();

    void addWidget(QWidget *w);
    virtual void addItem(QLayoutItem *) = 0;

    void removeWidget(QWidget *w);
    void removeItem(QLayoutItem *);

    Qt::Orientations expandingDirections() const override;
    QSize minimumSize() const override;
    QSize maximumSize() const override;
    void setGeometry(const QRect&) override;
    virtual QLayoutItem *itemAt(int index) const = 0;
    virtual QLayoutItem *takeAt(int index) = 0;
    virtual int indexOf(QWidget *) const;
    virtual int count() const = 0;
    bool isEmpty() const override;

    int totalHeightForWidth(int w) const;
    QSize totalMinimumSize() const;
    QSize totalMaximumSize() const;
    QSize totalSizeHint() const;
    QLayout *layout() override;

    void setEnabled(bool);
    bool isEnabled() const;

    static QSize closestAcceptableSize(const QWidget *w, const QSize &s);

protected:
    void widgetEvent(QEvent *);
    void childEvent(QChildEvent *e) override;
    void addChildLayout(QLayout *l);
    void addChildWidget(QWidget *w);
    bool adoptLayout(QLayout *layout);

    QRect alignmentRect(const QRect&) const;
protected:
    QLayout(QLayoutPrivate &d, QLayout*, QWidget*);

private:
    Q_DISABLE_COPY(QLayout)

    static void activateRecursiveHelper(QLayoutItem *item);

    friend class QApplicationPrivate;
    friend class QWidget;
};

//### support old includes
#if 1 //def QT3_SUPPORT
QT_BEGIN_INCLUDE_NAMESPACE
#include <QtGui/qboxlayout.h>
#include <QtGui/qgridlayout.h>
QT_END_INCLUDE_NAMESPACE
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QLAYOUT_H

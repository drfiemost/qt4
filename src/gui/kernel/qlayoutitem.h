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

#ifndef QLAYOUTITEM_H
#define QLAYOUTITEM_H

#include <QtGui/qsizepolicy.h>
#include <QtCore/qrect.h>

#include <limits.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


static const int QLAYOUTSIZE_MAX = INT_MAX/256/16;

class QLayout;
class QLayoutItem;
class QSpacerItem;
class QWidget;
class QSize;

class Q_GUI_EXPORT QLayoutItem
{
public:
    inline explicit QLayoutItem(Qt::Alignment alignment = nullptr);
    virtual ~QLayoutItem();
    virtual QSize sizeHint() const = 0;
    virtual QSize minimumSize() const = 0;
    virtual QSize maximumSize() const = 0;
    virtual Qt::Orientations expandingDirections() const = 0;
    virtual void setGeometry(const QRect&) = 0;
    virtual QRect geometry() const = 0;
    virtual bool isEmpty() const = 0;
    virtual bool hasHeightForWidth() const;
    virtual int heightForWidth(int) const;
    virtual int minimumHeightForWidth(int) const;
    virtual void invalidate();

    virtual QWidget *widget();
    virtual QLayout *layout();
    virtual QSpacerItem *spacerItem();

    Qt::Alignment alignment() const { return align; }
    void setAlignment(Qt::Alignment a);
    QSizePolicy::ControlTypes controlTypes() const;

protected:
    Qt::Alignment align;
};

inline QLayoutItem::QLayoutItem(Qt::Alignment aalignment)
    : align(aalignment) { }

class Q_GUI_EXPORT QSpacerItem : public QLayoutItem
{
public:
    QSpacerItem(int w, int h,
                 QSizePolicy::Policy hData = QSizePolicy::Minimum,
                 QSizePolicy::Policy vData = QSizePolicy::Minimum)
        : width(w), height(h), sizeP(hData, vData) { }
    ~QSpacerItem() override;

    void changeSize(int w, int h,
                     QSizePolicy::Policy hData = QSizePolicy::Minimum,
                     QSizePolicy::Policy vData = QSizePolicy::Minimum);
    QSize sizeHint() const override;
    QSize minimumSize() const override;
    QSize maximumSize() const override;
    Qt::Orientations expandingDirections() const override;
    bool isEmpty() const override;
    void setGeometry(const QRect&) override;
    QRect geometry() const override;
    QSpacerItem *spacerItem() override;

private:
    int width;
    int height;
    QSizePolicy sizeP;
    QRect rect;
};

class Q_GUI_EXPORT QWidgetItem : public QLayoutItem
{
    Q_DISABLE_COPY(QWidgetItem)

public:
    explicit QWidgetItem(QWidget *w) : wid(w) { }
    ~QWidgetItem() override;

    QSize sizeHint() const override;
    QSize minimumSize() const override;
    QSize maximumSize() const override;
    Qt::Orientations expandingDirections() const override;
    bool isEmpty() const override;
    void setGeometry(const QRect&) override;
    QRect geometry() const override;
    QWidget *widget() override;

    bool hasHeightForWidth() const override;
    int heightForWidth(int) const override;

protected:
    QWidget *wid;
};

class Q_GUI_EXPORT QWidgetItemV2 : public QWidgetItem
{
public:
    explicit QWidgetItemV2(QWidget *widget);
    ~QWidgetItemV2() override;

    QSize sizeHint() const override;
    QSize minimumSize() const override;
    QSize maximumSize() const override;
    int heightForWidth(int width) const override;

private:
    enum { Dirty = -123, HfwCacheMaxSize = 3 };

    inline bool useSizeCache() const;
    void updateCacheIfNecessary() const;
    inline void invalidateSizeCache() {
        q_cachedMinimumSize.setWidth(Dirty);
        q_hfwCacheSize = 0;
    }

    mutable QSize q_cachedMinimumSize;
    mutable QSize q_cachedSizeHint;
    mutable QSize q_cachedMaximumSize;
    mutable QSize q_cachedHfws[HfwCacheMaxSize];
    mutable short q_firstCachedHfw;
    mutable short q_hfwCacheSize;
    void *d;

    friend class QWidgetPrivate;

    Q_DISABLE_COPY(QWidgetItemV2)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QLAYOUTITEM_H

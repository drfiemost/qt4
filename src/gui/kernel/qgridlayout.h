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

#ifndef QGRIDLAYOUT_H
#define QGRIDLAYOUT_H

#include <QtGui/qlayout.h>
#ifdef QT_INCLUDE_COMPAT
#include <QtGui/qwidget.h>
#endif

#include <limits.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QGridLayoutPrivate;

class Q_GUI_EXPORT QGridLayout : public QLayout
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGridLayout)
    QDOC_PROPERTY(int horizontalSpacing READ horizontalSpacing WRITE setHorizontalSpacing)
    QDOC_PROPERTY(int verticalSpacing READ verticalSpacing WRITE setVerticalSpacing)

public:
    explicit QGridLayout(QWidget *parent);
    QGridLayout();

    ~QGridLayout() override;

    QSize sizeHint() const override;
    QSize minimumSize() const override;
    QSize maximumSize() const override;

    void setHorizontalSpacing(int spacing);
    int horizontalSpacing() const;
    void setVerticalSpacing(int spacing);
    int verticalSpacing() const;
    void setSpacing(int spacing);
    int spacing() const;

    void setRowStretch(int row, int stretch);
    void setColumnStretch(int column, int stretch);
    int rowStretch(int row) const;
    int columnStretch(int column) const;

    void setRowMinimumHeight(int row, int minSize);
    void setColumnMinimumWidth(int column, int minSize);
    int rowMinimumHeight(int row) const;
    int columnMinimumWidth(int column) const;

    int columnCount() const;
    int rowCount() const;

    QRect cellRect(int row, int column) const;

    bool hasHeightForWidth() const override;
    int heightForWidth(int) const override;
    int minimumHeightForWidth(int) const override;

    Qt::Orientations expandingDirections() const override;
    void invalidate() override;

    inline void addWidget(QWidget *w) { QLayout::addWidget(w); }
    void addWidget(QWidget *, int row, int column, Qt::Alignment = nullptr);
    void addWidget(QWidget *, int row, int column, int rowSpan, int columnSpan, Qt::Alignment = nullptr);
    void addLayout(QLayout *, int row, int column, Qt::Alignment = nullptr);
    void addLayout(QLayout *, int row, int column, int rowSpan, int columnSpan, Qt::Alignment = nullptr);

    void setOriginCorner(Qt::Corner);
    Qt::Corner originCorner() const;

    QLayoutItem *itemAt(int index) const override;
    QLayoutItem *itemAtPosition(int row, int column) const;
    QLayoutItem *takeAt(int index) override;
    int count() const override;
    void setGeometry(const QRect&) override;

    void addItem(QLayoutItem *item, int row, int column, int rowSpan = 1, int columnSpan = 1, Qt::Alignment = nullptr);

    void setDefaultPositioning(int n, Qt::Orientation orient);
    void getItemPosition(int idx, int *row, int *column, int *rowSpan, int *columnSpan);

protected:
    void addItem(QLayoutItem *) override;

private:
    Q_DISABLE_COPY(QGridLayout)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGRIDLAYOUT_H

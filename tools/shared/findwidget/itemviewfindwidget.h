/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the tools applications of the Qt Toolkit.
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

#ifndef ITEMVIEWFINDWIDGET_H
#define ITEMVIEWFINDWIDGET_H

#include "abstractfindwidget.h"

#include <QModelIndex>

QT_BEGIN_NAMESPACE

class QAbstractItemView;

class ItemViewFindWidget : public AbstractFindWidget
{
    Q_OBJECT

public:
    explicit ItemViewFindWidget(FindFlags flags = FindFlags(), QWidget *parent = nullptr);

    QAbstractItemView *itemView() const
    { return m_itemView; }

    void setItemView(QAbstractItemView *itemView);

protected:
    void deactivate() override;
    void find(const QString &textToFind, bool skipCurrent, bool backward, bool *found, bool *wrapped) override;

private:
    QModelIndex findHelper(const QString &textToFind, bool skipCurrent, bool backward,
        QModelIndex parent, int row, int column);

    QAbstractItemView *m_itemView;
};

QT_END_NAMESPACE

#endif  // ITEMVIEWFINDWIDGET_H

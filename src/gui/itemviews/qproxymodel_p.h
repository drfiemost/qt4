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

#ifndef QPROXYMODEL_P_H
#define QPROXYMODEL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of QAbstractItemModel*.  This header file may change from version
// to version without notice, or even be removed.
//
// We mean it.
//
//

#include "QtCore/qabstractitemmodel.h"
#include "private/qabstractitemmodel_p.h"

#ifndef QT_NO_PROXYMODEL

QT_BEGIN_NAMESPACE

class QEmptyModel : public QAbstractItemModel
{
public:
    explicit QEmptyModel(QObject *parent = nullptr) : QAbstractItemModel(parent) {}
    QModelIndex index(int, int, const QModelIndex &) const override { return QModelIndex(); }
    QModelIndex parent(const QModelIndex &) const override { return QModelIndex(); }
    int rowCount(const QModelIndex &) const override { return 0; }
    int columnCount(const QModelIndex &) const override { return 0; }
    bool hasChildren(const QModelIndex &) const override { return false; }
    QVariant data(const QModelIndex &, int) const override { return QVariant(); }
};

class QProxyModelPrivate : private QAbstractItemModelPrivate
{
    Q_DECLARE_PUBLIC(QProxyModel)

public:
    void _q_sourceDataChanged(const QModelIndex &tl,const QModelIndex &br);
    void _q_sourceRowsAboutToBeInserted(const QModelIndex &parent, int first ,int last);
    void _q_sourceRowsInserted(const QModelIndex &parent, int first ,int last);
    void _q_sourceRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void _q_sourceRowsRemoved(const QModelIndex &parent, int first, int last);
    void _q_sourceColumnsAboutToBeInserted(const QModelIndex &parent, int first, int last);
    void _q_sourceColumnsInserted(const QModelIndex &parent, int first, int last);
    void _q_sourceColumnsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void _q_sourceColumnsRemoved(const QModelIndex &parent, int first, int last);

    QProxyModelPrivate() : QAbstractItemModelPrivate(), model(nullptr) {}
    QAbstractItemModel *model;
    QEmptyModel empty;
};

QT_END_NAMESPACE

#endif // QT_NO_PROXYMODEL

#endif // QPROXYMODEL_P_H

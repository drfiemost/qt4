/****************************************************************************
**
** Copyright (C) 2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author Stephen Kelly <stephen.kelly@kdab.com>
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


#ifndef QIDENTITYPROXYMODEL_H
#define QIDENTITYPROXYMODEL_H

#include <QtGui/qabstractproxymodel.h>

#ifndef QT_NO_IDENTITYPROXYMODEL

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QIdentityProxyModelPrivate;

class Q_GUI_EXPORT QIdentityProxyModel : public QAbstractProxyModel
{
    Q_OBJECT
public:
    explicit QIdentityProxyModel(QObject* parent = nullptr);
    ~QIdentityProxyModel() override;

    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;

    QItemSelection mapSelectionFromSource(const QItemSelection& selection) const override;
    QItemSelection mapSelectionToSource(const QItemSelection& selection) const override;
    QModelIndexList match(const QModelIndex& start, int role, const QVariant& value, int hits = 1, Qt::MatchFlags flags = Qt::MatchFlags(Qt::MatchStartsWith|Qt::MatchWrap)) const override;
    void setSourceModel(QAbstractItemModel* sourceModel) override;

    bool insertColumns(int column, int count, const QModelIndex& parent = QModelIndex()) override;
    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

protected:
    QIdentityProxyModel(QIdentityProxyModelPrivate &dd, QObject* parent);

private:
    Q_DECLARE_PRIVATE(QIdentityProxyModel)
    Q_DISABLE_COPY(QIdentityProxyModel)

    Q_PRIVATE_SLOT(d_func(), void _q_sourceRowsAboutToBeInserted(QModelIndex,int,int))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceRowsInserted(QModelIndex,int,int))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceRowsAboutToBeRemoved(QModelIndex,int,int))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceRowsRemoved(QModelIndex,int,int))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceRowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceRowsMoved(QModelIndex,int,int,QModelIndex,int))

    Q_PRIVATE_SLOT(d_func(), void _q_sourceColumnsAboutToBeInserted(QModelIndex,int,int))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceColumnsInserted(QModelIndex,int,int))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceColumnsAboutToBeRemoved(QModelIndex,int,int))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceColumnsRemoved(QModelIndex,int,int))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceColumnsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceColumnsMoved(QModelIndex,int,int,QModelIndex,int))

    Q_PRIVATE_SLOT(d_func(), void _q_sourceDataChanged(QModelIndex,QModelIndex))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceHeaderDataChanged(Qt::Orientation orientation, int first, int last))

    Q_PRIVATE_SLOT(d_func(), void _q_sourceLayoutAboutToBeChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_sourceLayoutChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_sourceModelAboutToBeReset())
    Q_PRIVATE_SLOT(d_func(), void _q_sourceModelReset())
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_IDENTITYPROXYMODEL

#endif // QIDENTITYPROXYMODEL_H


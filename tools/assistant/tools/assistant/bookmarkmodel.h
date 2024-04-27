/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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
#ifndef BOOKMARKMODEL_H
#define BOOKMARKMODEL_H

#include <QtCore/QAbstractItemModel>

#include <QtGui/QIcon>

QT_BEGIN_NAMESPACE

class BookmarkItem;
class QMimeData;
class QTreeView;

typedef QMap<BookmarkItem*, QPersistentModelIndex> ItemModelIndexCache;

class BookmarkModel : public QAbstractItemModel
{
     Q_OBJECT
public:
    BookmarkModel();
    ~BookmarkModel() override;

    QByteArray bookmarks() const;
    void setBookmarks(const QByteArray &bookmarks);

    void setItemsEditable(bool editable);
    void expandFoldersIfNeeeded(QTreeView *treeView);

    QModelIndex addItem(const QModelIndex &parent, bool isFolder = false);
    bool removeItem(const QModelIndex &index);

    int rowCount(const QModelIndex &index = QModelIndex()) const override;
    int columnCount(const QModelIndex &index = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &index) const override;

    Qt::DropActions supportedDropActions () const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant data(const QModelIndex &index, int role) const override;
    void setData(const QModelIndex &index, const QVector<QVariant> &data);
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QModelIndex indexFromItem(BookmarkItem *item) const;
    BookmarkItem *itemFromIndex(const QModelIndex &index) const;
    QList<QPersistentModelIndex> indexListFor(const QString &label) const;

    bool insertRows(int position, int rows, const QModelIndex &parent) override;
    bool removeRows(int position, int rows, const QModelIndex &parent) override;

    QStringList mimeTypes() const override;
    QMimeData* mimeData(const QModelIndexList &indexes) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row,
        int column, const QModelIndex &parent) override;

private:
    void setupCache(const QModelIndex &parent);
    QModelIndexList collectItems(const QModelIndex &parent) const;
    void collectItems(const QModelIndex &parent, qint32 depth,
        QDataStream *stream) const;

private:
    int columns;
    bool m_folder;
    bool m_editable;
    QIcon folderIcon;
    QIcon bookmarkIcon;
    QTreeView *treeView;
    BookmarkItem *rootItem;
    ItemModelIndexCache cache;
};

QT_END_NAMESPACE

#endif  // BOOKMARKMODEL_H

/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtNetwork module of the Qt Toolkit.
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

#ifndef QNETWORKDISKCACHE_H
#define QNETWORKDISKCACHE_H

#include <QtNetwork/qabstractnetworkcache.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


#ifndef QT_NO_NETWORKDISKCACHE

class QNetworkDiskCachePrivate;
class Q_NETWORK_EXPORT QNetworkDiskCache : public QAbstractNetworkCache
{
    Q_OBJECT

public:
    explicit QNetworkDiskCache(QObject *parent = nullptr);
    ~QNetworkDiskCache() override;

    QString cacheDirectory() const;
    void setCacheDirectory(const QString &cacheDir);

    qint64 maximumCacheSize() const;
    void setMaximumCacheSize(qint64 size);

    qint64 cacheSize() const override;
    QNetworkCacheMetaData metaData(const QUrl &url) override;
    void updateMetaData(const QNetworkCacheMetaData &metaData) override;
    QIODevice *data(const QUrl &url) override;
    bool remove(const QUrl &url) override;
    QIODevice *prepare(const QNetworkCacheMetaData &metaData) override;
    void insert(QIODevice *device) override;

    QNetworkCacheMetaData fileMetaData(const QString &fileName) const;

public Q_SLOTS:
    void clear() override;

protected:
    virtual qint64 expire();

private:
    Q_DECLARE_PRIVATE(QNetworkDiskCache)
    Q_DISABLE_COPY(QNetworkDiskCache)
};

#endif // QT_NO_NETWORKDISKCACHE

QT_END_NAMESPACE

QT_END_HEADER

#endif // QNETWORKDISKCACHE_H

/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#ifndef QDECLARATIVEPIXMAPCACHE_H
#define QDECLARATIVEPIXMAPCACHE_H

#include <QtCore/qcoreapplication.h>
#include <QtCore/qstring.h>
#include <QtGui/qpixmap.h>
#include <QtCore/qurl.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QDeclarativeEngine;
class QDeclarativePixmapData;
class Q_DECLARATIVE_EXPORT QDeclarativePixmap
{
    Q_DECLARE_TR_FUNCTIONS(QDeclarativePixmap)
public:
    QDeclarativePixmap();
    QDeclarativePixmap(QDeclarativeEngine *, const QUrl &);
    QDeclarativePixmap(QDeclarativeEngine *, const QUrl &, const QSize &);
    ~QDeclarativePixmap();

    enum Status { Null, Ready, Error, Loading };

    enum Option {
        Asynchronous = 0x00000001,
        Cache        = 0x00000002
    };
    Q_DECLARE_FLAGS(Options, Option)

    bool isNull() const;
    bool isReady() const;
    bool isError() const;
    bool isLoading() const;

    Status status() const;
    QString error() const;
    const QUrl &url() const;
    const QSize &implicitSize() const;
    const QSize &requestSize() const;
    const QPixmap &pixmap() const;
    void setPixmap(const QPixmap &);

    QRect rect() const;
    int width() const;
    int height() const;
    inline operator const QPixmap &() const;

    void load(QDeclarativeEngine *, const QUrl &);
    void load(QDeclarativeEngine *, const QUrl &, QDeclarativePixmap::Options options);
    void load(QDeclarativeEngine *, const QUrl &, const QSize &);
    void load(QDeclarativeEngine *, const QUrl &, const QSize &, QDeclarativePixmap::Options options);

    void clear();
    void clear(QObject *);

    bool connectFinished(QObject *, const char *);
    bool connectFinished(QObject *, int);
    bool connectDownloadProgress(QObject *, const char *);
    bool connectDownloadProgress(QObject *, int);

    static void flushCache();

private:
    Q_DISABLE_COPY(QDeclarativePixmap)
    QDeclarativePixmapData *d;
};

inline QDeclarativePixmap::operator const QPixmap &() const
{
    return pixmap();
}

Q_DECLARE_OPERATORS_FOR_FLAGS(QDeclarativePixmap::Options)

QT_END_NAMESPACE

QT_END_HEADER

#endif // QDECLARATIVEPIXMAPCACHE_H

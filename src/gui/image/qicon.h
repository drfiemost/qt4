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

#ifndef QICON_H
#define QICON_H

#include <QtCore/qglobal.h>
#include <QtCore/qsize.h>
#include <QtCore/qlist.h>
#include <QtGui/qpixmap.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QIconPrivate;
class QIconEngine;

class Q_GUI_EXPORT QIcon
{
public:
    enum Mode { Normal, Disabled, Active, Selected };
    enum State { On, Off };

    QIcon();
    QIcon(const QPixmap &pixmap);
    QIcon(const QIcon &other);
    explicit QIcon(const QString &fileName); // file or resource name
    explicit QIcon(QIconEngine *engine);
    ~QIcon();
    QIcon &operator=(const QIcon &other);
#ifdef Q_COMPILER_RVALUE_REFS
    inline QIcon &operator=(QIcon &&other)
    { qSwap(d, other.d); return *this; }
#endif
    inline void swap(QIcon &other) { qSwap(d, other.d); }

    operator QVariant() const;

    QPixmap pixmap(const QSize &size, Mode mode = Normal, State state = Off) const;
    inline QPixmap pixmap(int w, int h, Mode mode = Normal, State state = Off) const
        { return pixmap(QSize(w, h), mode, state); }
    inline QPixmap pixmap(int extent, Mode mode = Normal, State state = Off) const
        { return pixmap(QSize(extent, extent), mode, state); }

    QSize actualSize(const QSize &size, Mode mode = Normal, State state = Off) const;

    QString name() const;

    void paint(QPainter *painter, const QRect &rect, Qt::Alignment alignment = Qt::AlignCenter, Mode mode = Normal, State state = Off) const;
    inline void paint(QPainter *painter, int x, int y, int w, int h, Qt::Alignment alignment = Qt::AlignCenter, Mode mode = Normal, State state = Off) const
        { paint(painter, QRect(x, y, w, h), alignment, mode, state); }

    bool isNull() const;
    bool isDetached() const;
    void detach();

    int serialNumber() const;
    qint64 cacheKey() const;

    void addPixmap(const QPixmap &pixmap, Mode mode = Normal, State state = Off);
    void addFile(const QString &fileName, const QSize &size = QSize(), Mode mode = Normal, State state = Off);

    QList<QSize> availableSizes(Mode mode = Normal, State state = Off) const;

    static QIcon fromTheme(const QString &name, const QIcon &fallback = QIcon());
    static bool hasThemeIcon(const QString &name);

    static QStringList themeSearchPaths();
    static void setThemeSearchPaths(const QStringList &searchpath);

    static QString themeName();
    static void setThemeName(const QString &path);

    Q_DUMMY_COMPARISON_OPERATOR(QIcon)

private:
    QIconPrivate *d;
#if !defined(QT_NO_DATASTREAM)
    friend Q_GUI_EXPORT QDataStream &operator<<(QDataStream &, const QIcon &);
    friend Q_GUI_EXPORT QDataStream &operator>>(QDataStream &, QIcon &);
#endif

public:
    typedef QIconPrivate * DataPtr;
    inline DataPtr &data_ptr() { return d; }
};

Q_DECLARE_SHARED(QIcon)

#if !defined(QT_NO_DATASTREAM)
Q_GUI_EXPORT QDataStream &operator<<(QDataStream &, const QIcon &);
Q_GUI_EXPORT QDataStream &operator>>(QDataStream &, QIcon &);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QICON_H

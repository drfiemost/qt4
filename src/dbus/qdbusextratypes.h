/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtDBus module of the Qt Toolkit.
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

#ifndef QDBUSEXTRATYPES_H
#define QDBUSEXTRATYPES_H

// define some useful types for D-BUS

#include <QtCore/qvariant.h>
#include <QtCore/qstring.h>
#include <QtDBus/qdbusmacros.h>
#include <QtCore/qhash.h>

#ifndef QT_NO_DBUS

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class Q_DBUS_EXPORT QDBusObjectPath : private QString
{
public:
    inline QDBusObjectPath() = default;

    inline explicit QDBusObjectPath(const char *path);
    inline explicit QDBusObjectPath(QLatin1String path);
    inline explicit QDBusObjectPath(const QString &path);
    inline QDBusObjectPath(const QDBusObjectPath &path);
    inline QDBusObjectPath &operator=(const QDBusObjectPath &path) = default;

    inline void setPath(const QString &path);

    inline QString path() const
    { return *this; }

private:
    void check();
};

inline QDBusObjectPath::QDBusObjectPath(const char *objectPath)
    : QString(QString::fromLatin1(objectPath))
{ check(); }

inline QDBusObjectPath::QDBusObjectPath(QLatin1String objectPath)
    : QString(objectPath)
{ check(); }

inline QDBusObjectPath::QDBusObjectPath(const QString &objectPath)
    : QString(objectPath)
{ check(); }

inline QDBusObjectPath::QDBusObjectPath(const QDBusObjectPath &path)
    : QString(path)
{ check(); }

inline void QDBusObjectPath::setPath(const QString &objectPath)
{ QString::operator=(objectPath); check(); }

inline bool operator==(const QDBusObjectPath &lhs, const QDBusObjectPath &rhs)
{ return lhs.path() == rhs.path(); }

inline bool operator!=(const QDBusObjectPath &lhs, const QDBusObjectPath &rhs)
{ return lhs.path() != rhs.path(); }

inline bool operator<(const QDBusObjectPath &lhs, const QDBusObjectPath &rhs)
{ return lhs.path() < rhs.path(); }

inline uint qHash(const QDBusObjectPath &objectPath, uint seed)
{ return qHash(objectPath.path(), seed); }


class Q_DBUS_EXPORT QDBusSignature : private QString
{
public:
    inline QDBusSignature() = default;

    inline explicit QDBusSignature(const char *signature);
    inline explicit QDBusSignature(QLatin1String signature);
    inline explicit QDBusSignature(const QString &signature);
    inline QDBusSignature(const QDBusSignature &dbusSignature);
    inline QDBusSignature &operator=(const QDBusSignature &signature) = default;

    inline void setSignature(const QString &signature);

    inline QString signature() const
    { return *this; }

private:
    void check();
};

inline QDBusSignature::QDBusSignature(const char *dBusSignature)
    : QString(QString::fromLatin1(dBusSignature))
{ check(); }

inline QDBusSignature::QDBusSignature(QLatin1String dBusSignature)
    : QString(dBusSignature)
{ check(); }

inline QDBusSignature::QDBusSignature(const QString &dBusSignature)
    : QString(dBusSignature)
{ check(); }

inline QDBusSignature::QDBusSignature(const QDBusSignature &dbusSignature)
    : QString(dbusSignature)
{ check(); }

inline void QDBusSignature::setSignature(const QString &dBusSignature)
{ QString::operator=(dBusSignature); check(); }

inline bool operator==(const QDBusSignature &lhs, const QDBusSignature &rhs)
{ return lhs.signature() == rhs.signature(); }

inline bool operator!=(const QDBusSignature &lhs, const QDBusSignature &rhs)
{ return lhs.signature() != rhs.signature(); }

inline bool operator<(const QDBusSignature &lhs, const QDBusSignature &rhs)
{ return lhs.signature() < rhs.signature(); }

inline uint qHash(const QDBusSignature &signature, uint seed)
{ return qHash(signature.signature(), seed); }

class QDBusVariant : private QVariant
{
public:
    inline QDBusVariant() = default;
    inline explicit QDBusVariant(const QVariant &variant);

    inline void setVariant(const QVariant &variant);

    inline QVariant variant() const
    { return *this; }
};

inline  QDBusVariant::QDBusVariant(const QVariant &dBusVariant)
    : QVariant(dBusVariant) { }

inline void QDBusVariant::setVariant(const QVariant &dBusVariant)
{ QVariant::operator=(dBusVariant); }

inline bool operator==(const QDBusVariant &v1, const QDBusVariant &v2)
{ return v1.variant() == v2.variant(); }

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QDBusVariant)
Q_DECLARE_METATYPE(QDBusObjectPath)
Q_DECLARE_METATYPE(QList<QDBusObjectPath>)
Q_DECLARE_METATYPE(QDBusSignature)
Q_DECLARE_METATYPE(QList<QDBusSignature>)

QT_END_HEADER

#endif // QT_NO_DBUS
#endif

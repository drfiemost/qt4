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

#ifndef QHOSTADDRESS_H
#define QHOSTADDRESS_H

#include <QtCore/qpair.h>
#include <QtCore/qstring.h>
#include <QtCore/qscopedpointer.h>
#include <QtNetwork/qabstractsocket.h>

struct sockaddr;

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QHostAddressPrivate;

class Q_NETWORK_EXPORT QIPv6Address
{
public:
    inline quint8 &operator [](int index) { return c[index]; }
    inline quint8 operator [](int index) const { return c[index]; }
    quint8 c[16];
};

typedef QIPv6Address Q_IPV6ADDR;

class Q_NETWORK_EXPORT QHostAddress
{
public:
    enum SpecialAddress {
        Null,
        Broadcast,
        LocalHost,
        LocalHostIPv6,
        Any,
        AnyIPv6
    };

    QHostAddress();
    explicit QHostAddress(quint32 ip4Addr);
    explicit QHostAddress(quint8 *ip6Addr);
    explicit QHostAddress(const Q_IPV6ADDR &ip6Addr);
    explicit QHostAddress(const sockaddr *sockaddr);
    explicit QHostAddress(const QString &address);
    QHostAddress(const QHostAddress &copy);
    QHostAddress(SpecialAddress address);
    ~QHostAddress();

    QHostAddress &operator=(const QHostAddress &other);
    QHostAddress &operator=(const QString &address);

    void setAddress(quint32 ip4Addr);
    void setAddress(quint8 *ip6Addr);
    void setAddress(const Q_IPV6ADDR &ip6Addr);
    void setAddress(const sockaddr *sockaddr);
    bool setAddress(const QString &address);

    QAbstractSocket::NetworkLayerProtocol protocol() const;
    quint32 toIPv4Address() const;
    Q_IPV6ADDR toIPv6Address() const;

    QString toString() const;

    QString scopeId() const;
    void setScopeId(const QString &id);

    bool operator ==(const QHostAddress &address) const;
    bool operator ==(SpecialAddress address) const;
    inline bool operator !=(const QHostAddress &address) const
    { return !operator==(address); }
    inline bool operator !=(SpecialAddress address) const
    { return !operator==(address); }
    bool isNull() const;
    void clear();

    bool isInSubnet(const QHostAddress &subnet, int netmask) const;
    bool isInSubnet(const QPair<QHostAddress, int> &subnet) const;

    static QPair<QHostAddress, int> parseSubnet(const QString &subnet);

protected:
    QScopedPointer<QHostAddressPrivate> d;
};

inline bool operator ==(QHostAddress::SpecialAddress address1, const QHostAddress &address2)
{ return address2 == address1; }

#ifndef QT_NO_DEBUG_STREAM
Q_NETWORK_EXPORT QDebug operator<<(QDebug, const QHostAddress &);
#endif


Q_NETWORK_EXPORT uint qHash(const QHostAddress &key, uint seed = 0);

#ifndef QT_NO_DATASTREAM
Q_NETWORK_EXPORT QDataStream &operator<<(QDataStream &, const QHostAddress &);
Q_NETWORK_EXPORT QDataStream &operator>>(QDataStream &, QHostAddress &);
#endif

QT_END_NAMESPACE

QT_END_HEADER
#endif // QHOSTADDRESS_H

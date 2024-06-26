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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the public API.  This header file may
// change from version to version without notice, or even be
// removed.
//
// We mean it.
//
//

#ifndef QDBUSINTEGRATOR_P_H
#define QDBUSINTEGRATOR_P_H

#include "qdbus_symbols_p.h"

#include "qcoreevent.h"
#include "qeventloop.h"
#include "qhash.h"
#include "qobject.h"
#include "private/qobject_p.h"
#include "qlist.h"
#include "qpointer.h"
#include "qsemaphore.h"

#include "qdbusconnection.h"
#include "qdbusmessage.h"
#include "qdbusconnection_p.h"

#ifndef QT_NO_DBUS

QT_BEGIN_NAMESPACE

class QDBusConnectionPrivate;

// Really private structs used by qdbusintegrator.cpp
// Things that aren't used by any other file

struct QDBusSlotCache
{
    struct Data
    {
        int flags;
        int slotIdx;
        QList<int> metaTypes;
    };
    typedef QMultiHash<QString, Data> Hash;
    Hash hash;
};

class QDBusCallDeliveryEvent: public QMetaCallEvent
{
public:
    QDBusCallDeliveryEvent(const QDBusConnection &c, int id, QObject *sender,
                           const QDBusMessage &msg, const QList<int> &types, int f = 0)
        : QMetaCallEvent(0, id, nullptr, sender, -1), connection(c), message(msg), metaTypes(types), flags(f)
        { }

    void placeMetaCall(QObject *object) override
    {
        QDBusConnectionPrivate::d(connection)->deliverCall(object, flags, message, metaTypes, id());
    }

private:
    QDBusConnection connection; // just for refcounting
    QDBusMessage message;
    QList<int> metaTypes;
    int flags;
};

class QDBusActivateObjectEvent: public QMetaCallEvent
{
public:
    QDBusActivateObjectEvent(const QDBusConnection &c, QObject *sender,
                             const QDBusConnectionPrivate::ObjectTreeNode &n,
                             int p, const QDBusMessage &m, QSemaphore *s = nullptr)
        : QMetaCallEvent(0, -1, nullptr, sender, -1, 0, nullptr, nullptr, s), connection(c), node(n),
          pathStartPos(p), message(m), handled(false)
        { }
    ~QDBusActivateObjectEvent() override;

    void placeMetaCall(QObject *) override;

private:
    QDBusConnection connection; // just for refcounting
    QDBusConnectionPrivate::ObjectTreeNode node;
    int pathStartPos;
    QDBusMessage message;
    bool handled;
};

class QDBusConnectionCallbackEvent : public QEvent
{
public:
    QDBusConnectionCallbackEvent()
        : QEvent(User), subtype(Subtype(0))
    { }

    DBusWatch *watch;
    union {
        int timerId;
        int fd;
    };
    int extra;

    enum Subtype {
        AddTimeout = 0,
        KillTimer,
        AddWatch,
        //RemoveWatch,
        ToggleWatch
    } subtype;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QDBusSlotCache)

#endif // QT_NO_DBUS
#endif

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

#ifndef QLOCALSERVER_P_H
#define QLOCALSERVER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QLocalServer class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef QT_NO_LOCALSERVER

#include "qlocalserver.h"
#include "private/qobject_p.h"
#include <qqueue.h>

#if defined(QT_LOCALSOCKET_TCP)
#   include <qtcpserver.h>
#elif defined(Q_OS_WIN)
#   include <qt_windows.h>
#   include <private/qwineventnotifier_p.h>
#else
#   include <private/qabstractsocketengine_p.h>
#   include <qsocketnotifier.h>
#endif

QT_BEGIN_NAMESPACE

class QLocalServerPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QLocalServer)

public:
    QLocalServerPrivate() :
#if !defined(QT_LOCALSOCKET_TCP) && !defined(Q_OS_WIN)
            listenSocket(-1), socketNotifier(nullptr),
#endif
            maxPendingConnections(30), error(QAbstractSocket::UnknownSocketError)
    {
    }

    void init();
    bool listen(const QString &name);
    static bool removeServer(const QString &name);
    void closeServer();
    void waitForNewConnection(int msec, bool *timedOut);
    void _q_onNewConnection();

#if defined(QT_LOCALSOCKET_TCP)

    QTcpServer tcpServer;
    QMap<quintptr, QTcpSocket*> socketMap;
#elif defined(Q_OS_WIN)
    struct Listener {
        HANDLE handle;
        OVERLAPPED overlapped;
        bool connected;
    };

    void setError(const QString &function);
    bool addListener();

    QList<Listener> listeners;
    HANDLE eventHandle;
    QWinEventNotifier *connectionEventNotifier;
#else
    void setError(const QString &function);

    int listenSocket;
    QSocketNotifier *socketNotifier;
#endif

    QString serverName;
    QString fullServerName;
    int maxPendingConnections;
    QQueue<QLocalSocket*> pendingConnections;
    QString errorString;
    QAbstractSocket::SocketError error;
};

QT_END_NAMESPACE

#endif // QT_NO_LOCALSERVER

#endif // QLOCALSERVER_P_H


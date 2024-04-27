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

#ifndef QHTTPSOCKETENGINE_P_H
#define QHTTPSOCKETENGINE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "private/qabstractsocketengine_p.h"
#include "qabstractsocket.h"
#include "qnetworkproxy.h"
#include "private/qauthenticator_p.h"

QT_BEGIN_NAMESPACE

#if !defined(QT_NO_NETWORKPROXY) && !defined(QT_NO_HTTP)

class QTcpSocket;
class QHttpSocketEnginePrivate;

class Q_AUTOTEST_EXPORT QHttpSocketEngine : public QAbstractSocketEngine
{
    Q_OBJECT
public:
    enum HttpState {
        None,
        ConnectSent,
        Connected,
        SendAuthentication,
        ReadResponseContent
    };
    QHttpSocketEngine(QObject *parent = nullptr);
    ~QHttpSocketEngine() override;

    bool initialize(QAbstractSocket::SocketType type, QAbstractSocket::NetworkLayerProtocol protocol = QAbstractSocket::IPv4Protocol) override;
    bool initialize(qintptr socketDescriptor, QAbstractSocket::SocketState socketState = QAbstractSocket::ConnectedState) override;

    void setProxy(const QNetworkProxy &networkProxy);

    qintptr socketDescriptor() const override;

    bool isValid() const override;

    bool connectInternal();
    bool connectToHost(const QHostAddress &address, quint16 port) override;
    bool connectToHostByName(const QString &name, quint16 port) override;
    bool bind(const QHostAddress &address, quint16 port) override;
    bool listen() override;
    int accept() override;
    void close() override;

    qint64 bytesAvailable() const override;

    qint64 read(char *data, qint64 maxlen) override;
    qint64 write(const char *data, qint64 len) override;

#ifndef QT_NO_UDPSOCKET
#ifndef QT_NO_NETWORKINTERFACE
    bool joinMulticastGroup(const QHostAddress &groupAddress,
                            const QNetworkInterface &interface) override;
    bool leaveMulticastGroup(const QHostAddress &groupAddress,
                             const QNetworkInterface &interface) override;
    QNetworkInterface multicastInterface() const override;
    bool setMulticastInterface(const QNetworkInterface &iface) override;
#endif // QT_NO_NETWORKINTERFACE

    qint64 readDatagram(char *data, qint64 maxlen, QHostAddress *addr = nullptr,
        quint16 *port = nullptr) override;
    qint64 writeDatagram(const char *data, qint64 len, const QHostAddress &addr,
        quint16 port) override;
    bool hasPendingDatagrams() const override;
    qint64 pendingDatagramSize() const override;
#endif // QT_NO_UDPSOCKET

    qint64 bytesToWrite() const override;

    int option(SocketOption option) const override;
    bool setOption(SocketOption option, int value) override;

    bool waitForRead(int msecs = 30000, bool *timedOut = nullptr) override;
    bool waitForWrite(int msecs = 30000, bool *timedOut = nullptr) override;
    bool waitForReadOrWrite(bool *readyToRead, bool *readyToWrite,
                            bool checkRead, bool checkWrite,
                            int msecs = 30000, bool *timedOut = nullptr) override;

    bool isReadNotificationEnabled() const override;
    void setReadNotificationEnabled(bool enable) override;
    bool isWriteNotificationEnabled() const override;
    void setWriteNotificationEnabled(bool enable) override;
    bool isExceptionNotificationEnabled() const override;
    void setExceptionNotificationEnabled(bool enable) override;

public slots:
    void slotSocketConnected();
    void slotSocketDisconnected();
    void slotSocketReadNotification();
    void slotSocketBytesWritten();
    void slotSocketError(QAbstractSocket::SocketError error);
    void slotSocketStateChanged(QAbstractSocket::SocketState state);

private slots:
    void emitPendingReadNotification();
    void emitPendingWriteNotification();
    void emitPendingConnectionNotification();

private:
    void emitReadNotification();
    void emitWriteNotification();
    void emitConnectionNotification();

    Q_DECLARE_PRIVATE(QHttpSocketEngine)
    Q_DISABLE_COPY(QHttpSocketEngine)

};


class QHttpSocketEnginePrivate : public QAbstractSocketEnginePrivate
{
    Q_DECLARE_PUBLIC(QHttpSocketEngine)
public:
    QHttpSocketEnginePrivate();
    ~QHttpSocketEnginePrivate() override;

    QNetworkProxy proxy;
    QString peerName;
    QTcpSocket *socket;
    QByteArray readBuffer; // only used for parsing the proxy response
    QHttpSocketEngine::HttpState state;
    QAuthenticator authenticator;
    bool readNotificationEnabled;
    bool writeNotificationEnabled;
    bool exceptNotificationEnabled;
    bool readNotificationActivated;
    bool writeNotificationActivated;
    bool readNotificationPending;
    bool writeNotificationPending;
    bool connectionNotificationPending;
    bool credentialsSent;
    uint pendingResponseData;
};

class Q_AUTOTEST_EXPORT QHttpSocketEngineHandler : public QSocketEngineHandler
{
public:
    QAbstractSocketEngine *createSocketEngine(QAbstractSocket::SocketType socketType,
                                                      const QNetworkProxy &, QObject *parent) override;
    QAbstractSocketEngine *createSocketEngine(qintptr socketDescripter, QObject *parent) override;
};
#endif

QT_END_NAMESPACE

#endif // QHTTPSOCKETENGINE_H

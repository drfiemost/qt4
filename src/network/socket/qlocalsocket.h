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

#ifndef QLOCALSOCKET_H
#define QLOCALSOCKET_H

#include <QtCore/qiodevice.h>
#include <QtNetwork/qabstractsocket.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


#ifndef QT_NO_LOCALSOCKET

class QLocalSocketPrivate;

class Q_NETWORK_EXPORT QLocalSocket : public QIODevice
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QLocalSocket)

public:
    enum LocalSocketError
    {
        ConnectionRefusedError = QAbstractSocket::ConnectionRefusedError,
        PeerClosedError = QAbstractSocket::RemoteHostClosedError,
        ServerNotFoundError = QAbstractSocket::HostNotFoundError,
        SocketAccessError = QAbstractSocket::SocketAccessError,
        SocketResourceError = QAbstractSocket::SocketResourceError,
        SocketTimeoutError = QAbstractSocket::SocketTimeoutError,
        DatagramTooLargeError = QAbstractSocket::DatagramTooLargeError,
        ConnectionError = QAbstractSocket::NetworkError,
        UnsupportedSocketOperationError = QAbstractSocket::UnsupportedSocketOperationError,
        UnknownSocketError = QAbstractSocket::UnknownSocketError,
        OperationError = QAbstractSocket::OperationError
    };

    enum LocalSocketState
    {
        UnconnectedState = QAbstractSocket::UnconnectedState,
        ConnectingState = QAbstractSocket::ConnectingState,
        ConnectedState = QAbstractSocket::ConnectedState,
        ClosingState = QAbstractSocket::ClosingState
    };

    QLocalSocket(QObject *parent = nullptr);
    ~QLocalSocket() override;

    void connectToServer(const QString &name, OpenMode openMode = ReadWrite);
    void disconnectFromServer();

    QString serverName() const;
    QString fullServerName() const;

    void abort();
    bool isSequential() const override;
    qint64 bytesAvailable() const override;
    qint64 bytesToWrite() const override;
    bool canReadLine() const override;
    void close() override;
    LocalSocketError error() const;
    bool flush();
    bool isValid() const;
    qint64 readBufferSize() const;
    void setReadBufferSize(qint64 size);

    bool setSocketDescriptor(quintptr socketDescriptor,
                             LocalSocketState socketState = ConnectedState,
                             OpenMode openMode = ReadWrite);
    quintptr socketDescriptor() const;

    LocalSocketState state() const;
    bool waitForBytesWritten(int msecs = 30000) override;
    bool waitForConnected(int msecs = 30000);
    bool waitForDisconnected(int msecs = 30000);
    bool waitForReadyRead(int msecs = 30000) override;

Q_SIGNALS:
    void connected();
    void disconnected();
    void error(QLocalSocket::LocalSocketError socketError);
    void stateChanged(QLocalSocket::LocalSocketState socketState);

protected:
    qint64 readData(char*, qint64) override;
    qint64 writeData(const char*, qint64) override;

private:
    Q_DISABLE_COPY(QLocalSocket)
#if defined(QT_LOCALSOCKET_TCP)
    Q_PRIVATE_SLOT(d_func(), void _q_stateChanged(QAbstractSocket::SocketState))
    Q_PRIVATE_SLOT(d_func(), void _q_error(QAbstractSocket::SocketError))
#elif defined(Q_OS_WIN)
    Q_PRIVATE_SLOT(d_func(), void _q_notified())
    Q_PRIVATE_SLOT(d_func(), void _q_canWrite())
    Q_PRIVATE_SLOT(d_func(), void _q_pipeClosed())
    Q_PRIVATE_SLOT(d_func(), void _q_emitReadyRead())
#else
    Q_PRIVATE_SLOT(d_func(), void _q_stateChanged(QAbstractSocket::SocketState))
    Q_PRIVATE_SLOT(d_func(), void _q_error(QAbstractSocket::SocketError))
    Q_PRIVATE_SLOT(d_func(), void _q_connectToSocket())
    Q_PRIVATE_SLOT(d_func(), void _q_abortConnectionAttempt())
#endif
};

#ifndef QT_NO_DEBUG_STREAM
#include <QtCore/qdebug.h>
Q_NETWORK_EXPORT QDebug operator<<(QDebug, QLocalSocket::LocalSocketError);
Q_NETWORK_EXPORT QDebug operator<<(QDebug, QLocalSocket::LocalSocketState);
#endif

#endif // QT_NO_LOCALSOCKET

QT_END_NAMESPACE

QT_END_HEADER

#endif // QLOCALSOCKET_H

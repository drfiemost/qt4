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

#ifndef QLOCALSOCKET_P_H
#define QLOCALSOCKET_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QLocalSocket class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef QT_NO_LOCALSOCKET

#include "qlocalsocket.h"
#include "private/qiodevice_p.h"

#include <qtimer.h>

#if defined(QT_LOCALSOCKET_TCP)
#   include "qtcpsocket.h"
#elif defined(Q_OS_WIN)
#   include "private/qwindowspipewriter_p.h"
#   include "private/qringbuffer_p.h"
#   include <private/qwineventnotifier_p.h>
#else
#   include "private/qabstractsocketengine_p.h"
#   include <qtcpsocket.h>
#   include <qsocketnotifier.h>
#   include <errno.h>
#endif

QT_BEGIN_NAMESPACE

#if !defined(Q_OS_WIN) || defined(QT_LOCALSOCKET_TCP)
class QLocalUnixSocket : public QTcpSocket
{

public:
    QLocalUnixSocket() : QTcpSocket()
    {
    };

    inline void setSocketState(QAbstractSocket::SocketState state)
    {
        QTcpSocket::setSocketState(state);
    };

    inline void setErrorString(const QString &string)
    {
        QTcpSocket::setErrorString(string);
    }

    inline void setSocketError(QAbstractSocket::SocketError error)
    {
        QTcpSocket::setSocketError(error);
    }

    inline qint64 readData(char *data, qint64 maxSize) override
    {
        return QTcpSocket::readData(data, maxSize);
    }

    inline qint64 writeData(const char *data, qint64 maxSize) override
    {
        return QTcpSocket::writeData(data, maxSize);
    }
};
#endif //#if !defined(Q_OS_WIN) || defined(QT_LOCALSOCKET_TCP)

class QLocalSocketPrivate : public QIODevicePrivate
{
    Q_DECLARE_PUBLIC(QLocalSocket)

public:
    QLocalSocketPrivate();
    void init();

#if defined(QT_LOCALSOCKET_TCP)
    QLocalUnixSocket* tcpSocket;
    bool ownsTcpSocket;
    void setSocket(QLocalUnixSocket*);
    QString generateErrorString(QLocalSocket::LocalSocketError, const QString &function) const;
    void errorOccurred(QLocalSocket::LocalSocketError, const QString &function);
    void _q_stateChanged(QAbstractSocket::SocketState newState);
    void _q_error(QAbstractSocket::SocketError newError);
#elif defined(Q_OS_WIN)
    ~QLocalSocketPrivate();
    void destroyPipeHandles();
    void setErrorString(const QString &function);
    void _q_notified();
    void _q_canWrite();
    void _q_pipeClosed();
    void _q_emitReadyRead();
    DWORD checkPipeState();
    void startAsyncRead();
    bool completeAsyncRead();
    void checkReadyRead();
    HANDLE handle;
    OVERLAPPED overlapped;
    QWindowsPipeWriter *pipeWriter;
    qint64 readBufferMaxSize;
    QRingBuffer readBuffer;
    int actualReadBufferSize;
    QWinEventNotifier *dataReadNotifier;
    QLocalSocket::LocalSocketError error;
    bool readSequenceStarted;
    bool pendingReadyRead;
    bool pipeClosed;
    static const qint64 initialReadBufferSize = 4096;
#else
    QLocalUnixSocket unixSocket;
    QString generateErrorString(QLocalSocket::LocalSocketError, const QString &function) const;
    void errorOccurred(QLocalSocket::LocalSocketError, const QString &function);
    void _q_stateChanged(QAbstractSocket::SocketState newState);
    void _q_error(QAbstractSocket::SocketError newError);
    void _q_connectToSocket();
    void _q_abortConnectionAttempt();
    void cancelDelayedConnect();
    QSocketNotifier *delayConnect;
    QTimer *connectTimer;
    int connectingSocket;
    QString connectingName;
    QIODevice::OpenMode connectingOpenMode;
#endif

    QString serverName;
    QString fullServerName;
    QLocalSocket::LocalSocketState state;
};

QT_END_NAMESPACE

#endif // QT_NO_LOCALSOCKET

#endif // QLOCALSOCKET_P_H


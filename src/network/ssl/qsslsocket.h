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


#ifndef QSSLSOCKET_H
#define QSSLSOCKET_H

#include <QtCore/qlist.h>
#include <QtCore/qregexp.h>
#ifndef QT_NO_OPENSSL
#   include <QtNetwork/qtcpsocket.h>
#   include <QtNetwork/qsslerror.h>
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


#ifndef QT_NO_OPENSSL

class QDir;
class QSslCipher;
class QSslCertificate;
class QSslConfiguration;

class QSslSocketPrivate;
class Q_NETWORK_EXPORT QSslSocket : public QTcpSocket
{
    Q_OBJECT
public:
    enum SslMode {
        UnencryptedMode,
        SslClientMode,
        SslServerMode
    };

    enum PeerVerifyMode {
        VerifyNone,
        QueryPeer,
        VerifyPeer,
        AutoVerifyPeer
    };

    QSslSocket(QObject *parent = nullptr);
    ~QSslSocket() override;

    // Autostarting the SSL client handshake.
    void connectToHostEncrypted(const QString &hostName, quint16 port, OpenMode mode = ReadWrite);
    void connectToHostEncrypted(const QString &hostName, quint16 port, const QString &sslPeerName, OpenMode mode = ReadWrite);
    bool setSocketDescriptor(qintptr socketDescriptor, SocketState state = ConnectedState,
                             OpenMode openMode = ReadWrite);

    // ### Qt 5: Make virtual
    void setSocketOption(QAbstractSocket::SocketOption option, const QVariant &value);
    QVariant socketOption(QAbstractSocket::SocketOption option);

    SslMode mode() const;
    bool isEncrypted() const;

    QSsl::SslProtocol protocol() const;
    void setProtocol(QSsl::SslProtocol protocol);

    QSslSocket::PeerVerifyMode peerVerifyMode() const;
    void setPeerVerifyMode(QSslSocket::PeerVerifyMode mode);

    int peerVerifyDepth() const;
    void setPeerVerifyDepth(int depth);

    QString peerVerifyName() const;
    void setPeerVerifyName(const QString &hostName);

    // From QIODevice
    qint64 bytesAvailable() const override;
    qint64 bytesToWrite() const override;
    bool canReadLine() const override;
    void close() override;
    bool atEnd() const override;
    bool flush();
    void abort();

    // From QAbstractSocket:
    void setReadBufferSize(qint64 size);

    // Similar to QIODevice's:
    qint64 encryptedBytesAvailable() const;
    qint64 encryptedBytesToWrite() const;

    // SSL configuration
    QSslConfiguration sslConfiguration() const;
    void setSslConfiguration(const QSslConfiguration &config);

    // Certificate & cipher accessors.
    void setLocalCertificate(const QSslCertificate &certificate);
    void setLocalCertificate(const QString &fileName, QSsl::EncodingFormat format = QSsl::Pem);
    QSslCertificate localCertificate() const;
    QSslCertificate peerCertificate() const;
    QList<QSslCertificate> peerCertificateChain() const;
    QSslCipher sessionCipher() const;

    // Private keys, for server sockets.
    void setPrivateKey(const QSslKey &key);
    void setPrivateKey(const QString &fileName, QSsl::KeyAlgorithm algorithm = QSsl::Rsa,
                       QSsl::EncodingFormat format = QSsl::Pem,
                       const QByteArray &passPhrase = QByteArray());
    QSslKey privateKey() const;

    // Cipher settings.
    QList<QSslCipher> ciphers() const;
    void setCiphers(const QList<QSslCipher> &ciphers);
    void setCiphers(const QString &ciphers);
    static void setDefaultCiphers(const QList<QSslCipher> &ciphers);
    static QList<QSslCipher> defaultCiphers();
    static QList<QSslCipher> supportedCiphers();

    // CA settings.
    bool addCaCertificates(const QString &path, QSsl::EncodingFormat format = QSsl::Pem,
                           QRegExp::PatternSyntax syntax = QRegExp::FixedString);
    void addCaCertificate(const QSslCertificate &certificate);
    void addCaCertificates(const QList<QSslCertificate> &certificates);
    void setCaCertificates(const QList<QSslCertificate> &certificates);
    QList<QSslCertificate> caCertificates() const;
    static bool addDefaultCaCertificates(const QString &path, QSsl::EncodingFormat format = QSsl::Pem,
                                         QRegExp::PatternSyntax syntax = QRegExp::FixedString);
    static void addDefaultCaCertificate(const QSslCertificate &certificate);
    static void addDefaultCaCertificates(const QList<QSslCertificate> &certificates);
    static void setDefaultCaCertificates(const QList<QSslCertificate> &certificates);
    static QList<QSslCertificate> defaultCaCertificates();
    static QList<QSslCertificate> systemCaCertificates();

    bool waitForConnected(int msecs = 30000);
    bool waitForEncrypted(int msecs = 30000);
    bool waitForReadyRead(int msecs = 30000) override;
    bool waitForBytesWritten(int msecs = 30000) override;
    bool waitForDisconnected(int msecs = 30000);

    QList<QSslError> sslErrors() const;

    static bool supportsSsl();
    void ignoreSslErrors(const QList<QSslError> &errors);

public Q_SLOTS:
    void startClientEncryption();
    void startServerEncryption();
    void ignoreSslErrors();

Q_SIGNALS:
    void encrypted();
    void peerVerifyError(const QSslError &error);
    void sslErrors(const QList<QSslError> &errors);
    void modeChanged(QSslSocket::SslMode newMode);
    void encryptedBytesWritten(qint64 totalBytes);

protected Q_SLOTS:
    void connectToHostImplementation(const QString &hostName, quint16 port,
                                     OpenMode openMode);
    void disconnectFromHostImplementation();

protected:
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

private:
    Q_DECLARE_PRIVATE(QSslSocket)
    Q_DISABLE_COPY(QSslSocket)
    Q_PRIVATE_SLOT(d_func(), void _q_connectedSlot())
    Q_PRIVATE_SLOT(d_func(), void _q_hostFoundSlot())
    Q_PRIVATE_SLOT(d_func(), void _q_disconnectedSlot())
    Q_PRIVATE_SLOT(d_func(), void _q_stateChangedSlot(QAbstractSocket::SocketState))
    Q_PRIVATE_SLOT(d_func(), void _q_errorSlot(QAbstractSocket::SocketError))
    Q_PRIVATE_SLOT(d_func(), void _q_readyReadSlot())
    Q_PRIVATE_SLOT(d_func(), void _q_bytesWrittenSlot(qint64))
    Q_PRIVATE_SLOT(d_func(), void _q_flushWriteBuffer())
    Q_PRIVATE_SLOT(d_func(), void _q_flushReadBuffer())
    friend class QSslSocketBackendPrivate;
};

#endif // QT_NO_OPENSSL

QT_END_NAMESPACE

#ifndef QT_NO_OPENSSL
Q_DECLARE_METATYPE(QList<QSslError>)
#endif

QT_END_HEADER

#endif

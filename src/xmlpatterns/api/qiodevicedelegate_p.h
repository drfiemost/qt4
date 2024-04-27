/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtXmlPatterns module of the Qt Toolkit.
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
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef QIODEVICEDELEGATE_P_H
#define QIODEVICEDELEGATE_P_H

#include <QtCore/QTimer>
#include <QtNetwork/QNetworkReply>

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * This is read-only currently.
     */
    class QIODeviceDelegate : public QNetworkReply
    {
        Q_OBJECT
    public:
        QIODeviceDelegate(QIODevice *const source);

        void abort() override;

        bool atEnd() const override;
        qint64 bytesAvailable() const override;
        qint64 bytesToWrite() const override;
        bool canReadLine() const override;
        void close() override;
        bool isSequential() const override;
        bool open(OpenMode mode) override;
        qint64 pos() const override;
        bool reset() override;
        bool seek(qint64 pos) override;
        qint64 size() const override;
        bool waitForBytesWritten(int msecs) override;
        bool waitForReadyRead(int msecs) override;

    protected:
        qint64 readData(char *data, qint64 maxSize) override;

    private Q_SLOTS:
        void networkTimeout();
    private:
        enum
        {
            /**
             * 20 seconds expressed in milliseconds.
             */
            Timeout = 20000
        };

        QIODevice *const m_source;
        QTimer m_timeout;
    };
}

QT_END_NAMESPACE

#endif

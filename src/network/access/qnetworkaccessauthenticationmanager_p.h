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

#ifndef QNETWORKACCESSAUTHENTICATIONMANAGER_P_H
#define QNETWORKACCESSAUTHENTICATIONMANAGER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the Network Access API.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include "qnetworkaccessmanager.h"
#include "qnetworkaccesscache_p.h"
#include "qnetworkaccessbackend_p.h"
#include "QtNetwork/qnetworkproxy.h"
#include "QtCore/QMutex"

QT_BEGIN_NAMESPACE

class QAuthenticator;
class QAbstractNetworkCache;
class QNetworkAuthenticationCredential;
class QNetworkCookieJar;

class QNetworkAuthenticationCredential
{
public:
    QString domain;
    QString user;
    QString password;
    bool isNull() {
        return domain.isNull() && user.isNull() && password.isNull();
    }
};
Q_DECLARE_TYPEINFO(QNetworkAuthenticationCredential, Q_MOVABLE_TYPE);
inline bool operator<(const QNetworkAuthenticationCredential &t1, const QString &t2)
{ return t1.domain < t2; }

class QNetworkAccessAuthenticationManager
{
public:
    QNetworkAccessAuthenticationManager() = default;

    void cacheCredentials(const QUrl &url, const QAuthenticator *auth);
    QNetworkAuthenticationCredential fetchCachedCredentials(const QUrl &url,
                                                             const QAuthenticator *auth = nullptr);

#ifndef QT_NO_NETWORKPROXY
    void cacheProxyCredentials(const QNetworkProxy &proxy, const QAuthenticator *auth);
    QNetworkAuthenticationCredential fetchCachedProxyCredentials(const QNetworkProxy &proxy,
                                                             const QAuthenticator *auth = nullptr);
#endif

    void clearCache();

protected:
    QNetworkAccessCache authenticationCache;
    QMutex mutex;
};

QT_END_NAMESPACE

#endif

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

#ifndef QNETWORKCONFIGURATIONMANAGER_H
#define QNETWORKCONFIGURATIONMANAGER_H

#ifdef QT_MOBILITY_BEARER
# include "qmobilityglobal.h"
#endif

#include <QtCore/qobject.h>
#include <QtNetwork/qnetworkconfiguration.h>

#ifndef QT_NO_BEARERMANAGEMENT

QT_BEGIN_HEADER

#ifndef QT_MOBILITY_BEARER
QT_BEGIN_NAMESPACE
#define QNetworkConfigurationManagerExport Q_NETWORK_EXPORT
#else
QTM_BEGIN_NAMESPACE
#define QNetworkConfigurationManagerExport Q_BEARER_EXPORT
#endif

class QNetworkConfigurationManagerPrivate;
class QNetworkConfigurationManagerExport QNetworkConfigurationManager : public QObject
{
    Q_OBJECT

public:
    enum Capability {
         CanStartAndStopInterfaces  = 0x00000001,
         DirectConnectionRouting = 0x00000002,
         SystemSessionSupport = 0x00000004,
         ApplicationLevelRoaming = 0x00000008,
         ForcedRoaming = 0x00000010,
         DataStatistics = 0x00000020,
         NetworkSessionRequired = 0x00000040
    };

    Q_DECLARE_FLAGS(Capabilities, Capability)

    explicit QNetworkConfigurationManager(QObject *parent = nullptr);
    ~QNetworkConfigurationManager() override;

    QNetworkConfigurationManager::Capabilities capabilities() const;

    QNetworkConfiguration defaultConfiguration() const;
    QList<QNetworkConfiguration> allConfigurations(QNetworkConfiguration::StateFlags flags = nullptr) const;
    QNetworkConfiguration configurationFromIdentifier(const QString &identifier) const;

    bool isOnline() const;

public Q_SLOTS:
    void updateConfigurations();

Q_SIGNALS:
    void configurationAdded(const QNetworkConfiguration &config);
    void configurationRemoved(const QNetworkConfiguration &config);
    void configurationChanged(const QNetworkConfiguration &config);
    void onlineStateChanged(bool isOnline);
    void updateCompleted();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QNetworkConfigurationManager::Capabilities)

#ifndef QT_MOBILITY_BEARER
QT_END_NAMESPACE
#else
QTM_END_NAMESPACE
#endif

QT_END_HEADER

#endif // QT_NO_BEARERMANAGEMENT

#endif // QNETWORKCONFIGURATIONMANAGER_H

/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#ifndef QDECLARATIVEINCLUDE_P_H
#define QDECLARATIVEINCLUDE_P_H

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

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtScript/qscriptvalue.h>

#include <private/qdeclarativecontext_p.h>
#include <private/qdeclarativeguard_p.h>

QT_BEGIN_NAMESPACE

class QDeclarativeEngine;
class QScriptContext;
class QScriptEngine;
class QNetworkAccessManager;
class QNetworkReply;
class QDeclarativeInclude : public QObject
{
    Q_OBJECT
public:
    enum Status {
        Ok = 0,
        Loading = 1,
        NetworkError = 2,
        Exception = 3
    };

    QDeclarativeInclude(const QUrl &, QDeclarativeEngine *, QScriptContext *ctxt);
    ~QDeclarativeInclude() override;

    void setCallback(const QScriptValue &);
    QScriptValue callback() const;

    QScriptValue result() const;

    static QScriptValue resultValue(QScriptEngine *, Status status = Loading);
    static void callback(QScriptEngine *, QScriptValue &callback, QScriptValue &status);

    static QScriptValue include(QScriptContext *ctxt, QScriptEngine *engine);
    static QScriptValue worker_include(QScriptContext *ctxt, QScriptEngine *engine);

public slots:
    void finished();

private:
    QDeclarativeEngine *m_engine;
    QScriptEngine *m_scriptEngine;
    QNetworkAccessManager *m_network;
    QDeclarativeGuard<QNetworkReply> m_reply;

    QUrl m_url;
    int m_redirectCount;
    QScriptValue m_callback;
    QScriptValue m_result;
    QDeclarativeGuardedContextData m_context;
    QScriptValue m_scope[2];
};

QT_END_NAMESPACE

#endif // QDECLARATIVEINCLUDE_P_H


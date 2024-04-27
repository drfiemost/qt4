/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL-ONLY$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** us via http://www.qt.io/contact-us/.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSCRIPTDECLARATIVEOBJECT_P_H
#define QSCRIPTDECLARATIVEOBJECT_P_H

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

#include <QtCore/qobjectdefs.h>

#include "config.h"
#include "qscriptobject_p.h"
#include "qscriptdeclarativeclass_p.h"

QT_BEGIN_NAMESPACE

class QScriptClass;

class QScriptDeclarativeClassPrivate
{
public:
    QScriptDeclarativeClassPrivate() : engine(nullptr), q_ptr(nullptr), context(nullptr), supportsCall(false) {}

    QScriptEngine *engine;
    QScriptDeclarativeClass *q_ptr;
    QScriptContext *context;
    bool supportsCall:1;

    static QScriptDeclarativeClassPrivate *get(QScriptDeclarativeClass *c) {
        return c->d_ptr.data();
    }
};

namespace QScript
{

class DeclarativeObjectDelegate : public QScriptObjectDelegate
{
public:
    DeclarativeObjectDelegate(QScriptDeclarativeClass *c, QScriptDeclarativeClass::Object *o);
    ~DeclarativeObjectDelegate() override;

    Type type() const override;

    QScriptDeclarativeClass *scriptClass() const { return m_class; }
    QScriptDeclarativeClass::Object *object() const { return m_object; }

    bool getOwnPropertySlot(QScriptObject*, JSC::ExecState*,
                                    const JSC::Identifier& propertyName,
                                    JSC::PropertySlot&) override;
    void put(QScriptObject*, JSC::ExecState* exec,
                     const JSC::Identifier& propertyName,
                     JSC::JSValue, JSC::PutPropertySlot&) override;
    bool deleteProperty(QScriptObject*, JSC::ExecState*,
                                const JSC::Identifier& propertyName) override;
    void getOwnPropertyNames(QScriptObject*, JSC::ExecState*,
                                     JSC::PropertyNameArray&, 
                                     JSC::EnumerationMode mode = JSC::ExcludeDontEnumProperties) override;

    JSC::CallType getCallData(QScriptObject*, JSC::CallData&) override;
    static JSC::JSValue JSC_HOST_CALL call(JSC::ExecState*, JSC::JSObject*,
                                           JSC::JSValue, const JSC::ArgList&);

    JSC::ConstructType getConstructData(QScriptObject*, JSC::ConstructData&) override;

    bool hasInstance(QScriptObject*, JSC::ExecState*,
                             JSC::JSValue value, JSC::JSValue proto) override;

    bool compareToObject(QScriptObject *, JSC::ExecState *, JSC::JSObject *) override;

private:
    QScriptDeclarativeClass *m_class;
    QScriptDeclarativeClass::Object *m_object;
};

} // namespace QScript

QT_END_NAMESPACE

#endif

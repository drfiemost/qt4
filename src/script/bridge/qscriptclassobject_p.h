/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtScript module of the Qt Toolkit.
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

#ifndef QSCRIPTCLASSOBJECT_P_H
#define QSCRIPTCLASSOBJECT_P_H

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

#include "qscriptobject_p.h"

QT_BEGIN_NAMESPACE

class QScriptClass;

namespace QScript
{

class ClassObjectDelegate : public QScriptObjectDelegate
{
public:
    ClassObjectDelegate(QScriptClass *scriptClass);
    ~ClassObjectDelegate() override;

    inline QScriptClass *scriptClass() const;
    inline void setScriptClass(QScriptClass *scriptClass);

    Type type() const override;

    bool getOwnPropertySlot(QScriptObject*, JSC::ExecState*,
                                    const JSC::Identifier& propertyName,
                                    JSC::PropertySlot&) override;
    bool getOwnPropertyDescriptor(QScriptObject*, JSC::ExecState*,
                                          const JSC::Identifier& propertyName,
                                          JSC::PropertyDescriptor&) override;
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
    static JSC::JSObject* construct(JSC::ExecState*, JSC::JSObject*,
                                    const JSC::ArgList&);

    bool hasInstance(QScriptObject*, JSC::ExecState*,
                             JSC::JSValue value, JSC::JSValue proto) override;

private:
    QScriptClass *m_scriptClass;
};

inline QScriptClass *ClassObjectDelegate::scriptClass() const
{
    return m_scriptClass;
}

inline void ClassObjectDelegate::setScriptClass(QScriptClass *scriptClass)
{
    Q_ASSERT(scriptClass != 0);
    m_scriptClass = scriptClass;
}

} // namespace QScript

QT_END_NAMESPACE

#endif

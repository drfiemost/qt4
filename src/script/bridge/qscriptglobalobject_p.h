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

#ifndef QSCRIPTGLOBALOBJECT_P_H
#define QSCRIPTGLOBALOBJECT_P_H

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

#include "JSGlobalObject.h"

QT_BEGIN_NAMESPACE

namespace QScript
{

class GlobalObject : public JSC::JSGlobalObject
{
public:
    GlobalObject();
    ~GlobalObject() override;
    JSC::UString className() const override { return "global"; }
    void markChildren(JSC::MarkStack&) override;
    bool getOwnPropertySlot(JSC::ExecState*,
                                    const JSC::Identifier& propertyName,
                                    JSC::PropertySlot&) override;
    bool getOwnPropertyDescriptor(JSC::ExecState*,
                                          const JSC::Identifier& propertyName,
                                          JSC::PropertyDescriptor&) override;
    void put(JSC::ExecState* exec, const JSC::Identifier& propertyName,
                     JSC::JSValue, JSC::PutPropertySlot&) override;
    void putWithAttributes(JSC::ExecState* exec, const JSC::Identifier& propertyName,
                                   JSC::JSValue value, unsigned attributes) override;
    bool deleteProperty(JSC::ExecState*,
                                const JSC::Identifier& propertyName) override;
    void getOwnPropertyNames(JSC::ExecState*, JSC::PropertyNameArray&,
                                     JSC::EnumerationMode mode = JSC::ExcludeDontEnumProperties) override;
    void defineGetter(JSC::ExecState*, const JSC::Identifier& propertyName, JSC::JSObject* getterFunction, unsigned attributes = 0) override;
    void defineSetter(JSC::ExecState*, const JSC::Identifier& propertyName, JSC::JSObject* setterFunction, unsigned attributes = 0) override;
    JSC::JSValue lookupGetter(JSC::ExecState*, const JSC::Identifier& propertyName) override;
    JSC::JSValue lookupSetter(JSC::ExecState*, const JSC::Identifier& propertyName) override;

public:
    JSC::JSObject *customGlobalObject;
};

class OriginalGlobalObjectProxy : public JSC::JSObject
{
public:
    explicit OriginalGlobalObjectProxy(WTF::PassRefPtr<JSC::Structure> sid,
                                       JSC::JSGlobalObject *object)
        : JSC::JSObject(sid), originalGlobalObject(object)
    {}
    ~OriginalGlobalObjectProxy() override
    = default;
    JSC::UString className() const override
    { return originalGlobalObject->className(); }
    void markChildren(JSC::MarkStack& markStack) override
    {
        markStack.append(originalGlobalObject);
        JSC::JSObject::markChildren(markStack);
    }
    bool getOwnPropertySlot(JSC::ExecState* exec,
                                    const JSC::Identifier& propertyName,
                                    JSC::PropertySlot& slot) override
    { return originalGlobalObject->JSC::JSGlobalObject::getOwnPropertySlot(exec, propertyName, slot); }
    bool getOwnPropertyDescriptor(JSC::ExecState* exec,
                                          const JSC::Identifier& propertyName,
                                          JSC::PropertyDescriptor& descriptor) override
    { return originalGlobalObject->JSC::JSGlobalObject::getOwnPropertyDescriptor(exec, propertyName, descriptor); }
    void put(JSC::ExecState* exec, const JSC::Identifier& propertyName,
                     JSC::JSValue value, JSC::PutPropertySlot& slot) override
    { originalGlobalObject->JSC::JSGlobalObject::put(exec, propertyName, value, slot); }
    void putWithAttributes(JSC::ExecState* exec, const JSC::Identifier& propertyName, JSC::JSValue value, unsigned attributes) override
    { originalGlobalObject->JSC::JSGlobalObject::putWithAttributes(exec, propertyName, value, attributes); }
    bool deleteProperty(JSC::ExecState* exec,
                                const JSC::Identifier& propertyName) override
    { return originalGlobalObject->JSC::JSGlobalObject::deleteProperty(exec, propertyName); }
    void getOwnPropertyNames(JSC::ExecState* exec, JSC::PropertyNameArray& propertyNames, JSC::EnumerationMode mode = JSC::ExcludeDontEnumProperties) override
    { originalGlobalObject->JSC::JSGlobalObject::getOwnPropertyNames(exec, propertyNames, mode); }
    void defineGetter(JSC::ExecState* exec, const JSC::Identifier& propertyName, JSC::JSObject* getterFunction, unsigned attributes) override
    { originalGlobalObject->JSC::JSGlobalObject::defineGetter(exec, propertyName, getterFunction, attributes); }
    void defineSetter(JSC::ExecState* exec, const JSC::Identifier& propertyName, JSC::JSObject* setterFunction, unsigned attributes) override
    { originalGlobalObject->JSC::JSGlobalObject::defineSetter(exec, propertyName, setterFunction, attributes); }
    JSC::JSValue lookupGetter(JSC::ExecState* exec, const JSC::Identifier& propertyName) override
    { return originalGlobalObject->JSC::JSGlobalObject::lookupGetter(exec, propertyName); }
    JSC::JSValue lookupSetter(JSC::ExecState* exec, const JSC::Identifier& propertyName) override
    { return originalGlobalObject->JSC::JSGlobalObject::lookupSetter(exec, propertyName); }
private:
    JSC::JSGlobalObject *originalGlobalObject;
};

} // namespace QScript

QT_END_NAMESPACE

#endif

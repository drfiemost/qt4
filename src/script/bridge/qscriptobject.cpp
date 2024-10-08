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

#include "config.h"
#include "qscriptobject_p.h"
#include "private/qobject_p.h"

namespace JSC
{
//QT_USE_NAMESPACE
ASSERT_CLASS_FITS_IN_CELL(QT_PREPEND_NAMESPACE(QScriptObject));
ASSERT_CLASS_FITS_IN_CELL(QT_PREPEND_NAMESPACE(QScriptObjectPrototype));
}

QT_BEGIN_NAMESPACE

// masquerading as JSC::JSObject
const JSC::ClassInfo QScriptObject::info = { "Object", nullptr, nullptr, nullptr };

QScriptObject::Data::~Data()
{
    delete delegate;
}

QScriptObject::QScriptObject(WTF::PassRefPtr<JSC::Structure> sid)
    : JSC::JSObject(sid), d(nullptr)
{
}

QScriptObject::~QScriptObject()
{
    delete d;
}

bool QScriptObject::getOwnPropertySlot(JSC::ExecState* exec,
                                       const JSC::Identifier& propertyName,
                                       JSC::PropertySlot& slot)
{
    if (!d || !d->delegate)
        return JSC::JSObject::getOwnPropertySlot(exec, propertyName, slot);
    return d->delegate->getOwnPropertySlot(this, exec, propertyName, slot);
}

bool QScriptObject::getOwnPropertyDescriptor(JSC::ExecState* exec,
                                             const JSC::Identifier& propertyName,
                                             JSC::PropertyDescriptor& descriptor)
{
    if (!d || !d->delegate)
        return JSC::JSObject::getOwnPropertyDescriptor(exec, propertyName, descriptor);
    return d->delegate->getOwnPropertyDescriptor(this, exec, propertyName, descriptor);
}

void QScriptObject::put(JSC::ExecState* exec, const JSC::Identifier& propertyName,
                        JSC::JSValue value, JSC::PutPropertySlot& slot)
{
    if (!d || !d->delegate) {
        JSC::JSObject::put(exec, propertyName, value, slot);
        return;
    }
    d->delegate->put(this, exec, propertyName, value, slot);
}

bool QScriptObject::deleteProperty(JSC::ExecState* exec,
                                   const JSC::Identifier& propertyName)
{
    if (!d || !d->delegate)
        return JSC::JSObject::deleteProperty(exec, propertyName);
    return d->delegate->deleteProperty(this, exec, propertyName);
}

void QScriptObject::getOwnPropertyNames(JSC::ExecState* exec, JSC::PropertyNameArray& propertyNames,
                                        JSC::EnumerationMode mode)
{
    if (!d || !d->delegate) {
        JSC::JSObject::getOwnPropertyNames(exec, propertyNames, mode);
        return;
    }
    d->delegate->getOwnPropertyNames(this, exec, propertyNames, mode);
}

bool QScriptObject::compareToObject(JSC::ExecState* exec, JSC::JSObject *other)
{
    if (!d || !d->delegate) {
        return JSC::JSObject::compareToObject(exec, other);
    }
    return d->delegate->compareToObject(this, exec, other);
}

void QScriptObject::markChildren(JSC::MarkStack& markStack)
{
    if (!d)
        d = new Data();
    if (d->isMarking)
        return;
    QBoolBlocker markBlocker(d->isMarking, true);
    if (d && d->data)
        markStack.append(d->data);
    if (!d || !d->delegate) {
        JSC::JSObject::markChildren(markStack);
        return;
    }
    d->delegate->markChildren(this, markStack);
}

JSC::CallType QScriptObject::getCallData(JSC::CallData &data)
{
    if (!d || !d->delegate)
        return JSC::JSObject::getCallData(data);
    return d->delegate->getCallData(this, data);
}

JSC::ConstructType QScriptObject::getConstructData(JSC::ConstructData &data)
{
    if (!d || !d->delegate)
        return JSC::JSObject::getConstructData(data);
    return d->delegate->getConstructData(this, data);
}

bool QScriptObject::hasInstance(JSC::ExecState* exec, JSC::JSValue value, JSC::JSValue proto)
{
    if (!d || !d->delegate)
        return JSC::JSObject::hasInstance(exec, value, proto);
    return d->delegate->hasInstance(this, exec, value, proto);
}

QScriptObjectPrototype::QScriptObjectPrototype(JSC::ExecState*, WTF::PassRefPtr<JSC::Structure> structure,
                                               JSC::Structure* /*prototypeFunctionStructure*/)
    : QScriptObject(structure)
{
}

QScriptObjectDelegate::QScriptObjectDelegate()
= default;

QScriptObjectDelegate::~QScriptObjectDelegate()
= default;

bool QScriptObjectDelegate::getOwnPropertySlot(QScriptObject* object, JSC::ExecState* exec,
                                               const JSC::Identifier& propertyName,
                                               JSC::PropertySlot& slot)
{
    return object->JSC::JSObject::getOwnPropertySlot(exec, propertyName, slot);
}

bool QScriptObjectDelegate::getOwnPropertyDescriptor(QScriptObject* object, JSC::ExecState* exec,
                                               const JSC::Identifier& propertyName,
                                               JSC::PropertyDescriptor& descriptor)
{
    return object->JSC::JSObject::getOwnPropertyDescriptor(exec, propertyName, descriptor);
}


void QScriptObjectDelegate::put(QScriptObject* object, JSC::ExecState* exec,
                                const JSC::Identifier& propertyName,
                                JSC::JSValue value, JSC::PutPropertySlot& slot)
{
    object->JSC::JSObject::put(exec, propertyName, value, slot);
}

bool QScriptObjectDelegate::deleteProperty(QScriptObject* object, JSC::ExecState* exec,
                                           const JSC::Identifier& propertyName)
{
    return object->JSC::JSObject::deleteProperty(exec, propertyName);
}

void QScriptObjectDelegate::getOwnPropertyNames(QScriptObject* object, JSC::ExecState* exec,
                                                JSC::PropertyNameArray& propertyNames,
                                                JSC::EnumerationMode mode)
{
    object->JSC::JSObject::getOwnPropertyNames(exec, propertyNames, mode);
}

void QScriptObjectDelegate::markChildren(QScriptObject* object, JSC::MarkStack& markStack)
{
    // ### should this call the virtual function instead??
    object->JSC::JSObject::markChildren(markStack);
}

JSC::CallType QScriptObjectDelegate::getCallData(QScriptObject* object, JSC::CallData& data)
{
    return object->JSC::JSObject::getCallData(data);
}

JSC::ConstructType QScriptObjectDelegate::getConstructData(QScriptObject* object, JSC::ConstructData& data)
{
    return object->JSC::JSObject::getConstructData(data);
}

bool QScriptObjectDelegate::hasInstance(QScriptObject* object, JSC::ExecState* exec,
                                        JSC::JSValue value, JSC::JSValue proto)
{
    return object->JSC::JSObject::hasInstance(exec, value, proto);
}

bool QScriptObjectDelegate::compareToObject(QScriptObject* object, JSC::ExecState* exec, JSC::JSObject* o)
{
    return object->JSC::JSObject::compareToObject(exec, o);
}

QT_END_NAMESPACE

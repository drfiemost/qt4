/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtTest module of the Qt Toolkit.
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

#ifndef QTESTCOREELEMENT_H
#define QTESTCOREELEMENT_H

#include <QtTest/qtestcorelist.h>
#include <QtTest/qtestelementattribute.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

template <class ElementType>
class QTestCoreElement: public QTestCoreList<ElementType>
{
    public:
        QTestCoreElement( int type = -1 );
        ~QTestCoreElement() override;

        void addAttribute(const QTest::AttributeIndex index, const char *value);
        QTestElementAttribute *attributes() const;
        const char *attributeValue(QTest::AttributeIndex index) const;
        const char *attributeName(QTest::AttributeIndex index) const;
        const QTestElementAttribute *attribute(QTest::AttributeIndex index) const;

        const char *elementName() const;
        QTest::LogElementType elementType() const;

    private:
        QTestElementAttribute *listOfAttributes;
        QTest::LogElementType type;
};

template<class ElementType>
QTestCoreElement<ElementType>::QTestCoreElement(int t)
    :listOfAttributes(nullptr), type(QTest::LogElementType(t))
{
}

template<class ElementType>
QTestCoreElement<ElementType>::~QTestCoreElement()
{
    delete listOfAttributes;
}

template <class ElementType>
void QTestCoreElement<ElementType>::addAttribute(const QTest::AttributeIndex attributeIndex, const char *value)
{
    if(attributeIndex == -1)
        return;

    if (attribute(attributeIndex))
        return;

    QTestElementAttribute *testAttribute = new QTestElementAttribute;
    testAttribute->setPair(attributeIndex, value);
    testAttribute->addToList(&listOfAttributes);
}

template <class ElementType>
QTestElementAttribute *QTestCoreElement<ElementType>::attributes() const
{
    return listOfAttributes;
}

template <class ElementType>
const char *QTestCoreElement<ElementType>::attributeValue(QTest::AttributeIndex index) const
{
    const QTestElementAttribute *attrb = attribute(index);
    if(attrb)
        return attrb->value();

    return nullptr;
}

template <class ElementType>
const char *QTestCoreElement<ElementType>::attributeName(QTest::AttributeIndex index) const
{
    const QTestElementAttribute *attrb = attribute(index);
    if(attrb)
        return attrb->name();

    return nullptr;
}

template <class ElementType>
const char *QTestCoreElement<ElementType>::elementName() const
{
    const char *xmlElementNames[] =
    {
        "property",
        "properties",
        "failure",
        "error",
        "testcase",
        "testsuite",
        "benchmark",
        "system-err"
    };

    if(type != QTest::LET_Undefined)
        return xmlElementNames[type];

    return nullptr;
}

template <class ElementType>
QTest::LogElementType QTestCoreElement<ElementType>::elementType() const
{
    return type;
}

template <class ElementType>
const QTestElementAttribute *QTestCoreElement<ElementType>::attribute(QTest::AttributeIndex index) const
{
    QTestElementAttribute *iterator = listOfAttributes;
    while(iterator){
        if(iterator->index() == index)
            return iterator;

        iterator = iterator->nextElement();
    }

    return nullptr;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

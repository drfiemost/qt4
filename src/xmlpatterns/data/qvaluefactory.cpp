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

#include "qatomiccaster_p.h"
#include "qatomicstring_p.h"
#include "qcastingplatform_p.h"
#include "qvaluefactory_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

/**
 * @short Helper class for ValueFactory::fromLexical() which exposes
 * CastingPlatform appropriately.
 *
 * @relates ValueFactory
 */
class PerformValueConstruction : public CastingPlatform<PerformValueConstruction, false>
                               , public SourceLocationReflection
{
public:
    PerformValueConstruction(const SourceLocationReflection *const sourceLocationReflection,
                             const SchemaType::Ptr &toType) : m_sourceReflection(sourceLocationReflection)
                                                            , m_targetType(AtomicType::Ptr(toType))
    {
        Q_ASSERT(m_sourceReflection);
    }

    AtomicValue::Ptr operator()(const AtomicValue::Ptr &lexicalValue,
                                const SchemaType::Ptr & /*type*/,
                                const ReportContext::Ptr &context)
    {
        prepareCasting(context, BuiltinTypes::xsString);
        return AtomicValue::Ptr(const_cast<AtomicValue *>(cast(lexicalValue, context).asAtomicValue()));
    }

    const SourceLocationReflection *actualReflection() const override
    {
        return m_sourceReflection;
    }

    ItemType::Ptr targetType() const
    {
        return m_targetType;
    }

private:
    const SourceLocationReflection *const m_sourceReflection;
    const ItemType::Ptr                   m_targetType;
};

AtomicValue::Ptr ValueFactory::fromLexical(const QString &lexicalValue,
                                           const SchemaType::Ptr &type,
                                           const ReportContext::Ptr &context,
                                           const SourceLocationReflection *const sourceLocationReflection)
{
    Q_ASSERT(context);
    Q_ASSERT(type);
    Q_ASSERT_X(type->category() == SchemaType::SimpleTypeAtomic, Q_FUNC_INFO,
               "We can only construct for atomic values.");

    return PerformValueConstruction(sourceLocationReflection, type)(AtomicString::fromValue(lexicalValue),
                                                                    type,
                                                                    context);
}

QT_END_NAMESPACE

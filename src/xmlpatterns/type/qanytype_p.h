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

#ifndef Patternist_AnyType_H
#define Patternist_AnyType_H

#include "qschematype_p.h"


QT_BEGIN_NAMESPACE

namespace QPatternist
{
    class AtomicType;

    /**
     * @short Represents the @c xs:anyType item type.
     *
     * @ingroup Patternist_types
     * @author Frans Englich <frans.englich@nokia.com>
     */
    class AnyType : public SchemaType
    {
    public:

        typedef QExplicitlySharedDataPointer<AnyType> Ptr;
        friend class BuiltinTypes;

        ~AnyType() override;

        QXmlName name(const NamePool::Ptr &np) const override;

        /**
         * @returns always "xs:anyType"
         */
        QString displayName(const NamePool::Ptr &np) const override;

        /**
         * @returns always @c false
         */
        bool isAbstract() const override;

        /**
         * @returns @c null, since <tt>xs:anyType</tt> has no base type, it is the ur-type.
         *
         * @returns always @c null
         */
        SchemaType::Ptr wxsSuperType() const override;

        /**
         * @returns @c true only if @p other is xsAnyType.
         */
        bool wxsTypeMatches(const SchemaType::Ptr &other) const override;

        /**
         * <tt>xs:anyType</tt> is the "ur-type" and special. Therefore, this function
         * returns SchemaType::None.
         *
         * @returns SchemaType::None
         */
        TypeCategory category() const override;

        /**
         * @returns always NoDerivation.
         */
        DerivationMethod derivationMethod() const override;

        /**
         * @returns an empty set of derivation constraint flags.
         */
        DerivationConstraints derivationConstraints() const override;

        /**
         * Always returns @c true.
         */
        bool isComplexType() const override;

    protected:
        /**
         * @short This constructor is protected, because this
         * class must be sub-classed.
         */
        inline AnyType()
        {
        }
    };
}

QT_END_NAMESPACE


#endif

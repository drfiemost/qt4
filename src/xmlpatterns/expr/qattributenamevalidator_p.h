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

#ifndef Patternist_AttributeNameValidator_H
#define Patternist_AttributeNameValidator_H

#include "qsinglecontainer_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Ensures the lexical space of the string value of the Item returned
     * from its child Expression is an NCName. Also possibly changes the name
     * by generating a prefix if one is needed.
     *
     * @note It doesn't actually construct an @c xs:NCName. It only ensures the lexical
     * space is an @c NCName. The atomic value can be of any string type, such as @c xs:untypedAtomic
     * of @c xs:string.
     *
     * @author Frans Englich <frans.englich@nokia.com>
     * @ingroup Patternist_expressions
     */
    class AttributeNameValidator : public SingleContainer
    {
    public:
        AttributeNameValidator(const Expression::Ptr &source);

        Item evaluateSingleton(const DynamicContext::Ptr &) const override;

        SequenceType::List expectedOperandTypes() const override;

        /**
         * @returns the static type of its operand. This is returned as opposed
         * CommonSequenceTypes::ExactlyOneQName, since the operand might return a subtype
         * of @c xs:QName.
         */
        SequenceType::Ptr staticType() const override;

        ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const override;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

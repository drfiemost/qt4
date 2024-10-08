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

#ifndef Patternist_ExpressionVariableReference_H
#define Patternist_ExpressionVariableReference_H

#include "qvariabledeclaration_p.h"
#include "qvariablereference_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short A reference to a variable declared with <tt>declare variable</tt> or @c let.
     *
     * It's also used by variable bindings in @c case branches of the @c typeswitch
     * expression.
     *
     * This AST node is only used up until the typeCheck() stage. Therefore it
     * has no functions for evaluation, such as evaluateSequence().
     *
     * @author Frans Englich <frans.englich@nokia.com>
     * @ingroup Patternist_expressions
     */
    class ExpressionVariableReference : public VariableReference
    {
    public:
        ExpressionVariableReference(const VariableSlotID slot,
                                    const VariableDeclaration *varDecl);

        bool evaluateEBV(const DynamicContext::Ptr &context) const override;
        Item evaluateSingleton(const DynamicContext::Ptr &context) const override;
        Item::Iterator::Ptr evaluateSequence(const DynamicContext::Ptr &context) const override;

        Expression::Ptr typeCheck(const StaticContext::Ptr &context,
                                          const SequenceType::Ptr &reqType) override;
        SequenceType::Ptr staticType() const override;
        ID id() const override;
        ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const override;

        inline const Expression::Ptr &sourceExpression() const;
        inline const VariableDeclaration *variableDeclaration() const;
    private:
        const VariableDeclaration *m_varDecl;
    };

    inline const Expression::Ptr &ExpressionVariableReference::sourceExpression() const
    {
        return m_varDecl->expression();
    }

    inline const VariableDeclaration *ExpressionVariableReference::variableDeclaration() const
    {
        return m_varDecl;
    }

}

QT_END_NAMESPACE

QT_END_HEADER

#endif

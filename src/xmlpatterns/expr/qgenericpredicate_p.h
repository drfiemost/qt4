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

#ifndef Patternist_GenericPredicate_H
#define Patternist_GenericPredicate_H

#include "qpaircontainer_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short A predicate that can handle all kinds of predicates and
     * is therefore not very efficient, but can cope with all the tricky scenarios.
     *
     * @see FirstItemPredicate
     * @see TruthPredicate
     * @author Frans Englich <frans.englich@nokia.com>
     * @ingroup Patternist_expressions
     */
    class GenericPredicate : public PairContainer
    {
    public:

        /**
         * Creates a predicate expression that filters the items gained
         * from evaluating @p sourceExpression through the filter @p predicateExpression.
         *
         * This function performs type analyzis on the passed expressions, and may
         * return more specialized expressions depending on the analyzis.
         *
         * If @p predicateExpression is an invalid predicate, an error is issued
         * via the @p context.
         */
        static Expression::Ptr create(const Expression::Ptr &sourceExpression,
                                      const Expression::Ptr &predicateExpression,
                                      const StaticContext::Ptr &context,
                                      const QSourceLocation &location);

        static Expression::Ptr createFirstItem(const Expression::Ptr &sourceExpression);

        /**
         * Creates a source iterator which is passed to the ItemMappingIterator
         * and the Focus. The ItemMappingIterator modifies it with
         * its QAbstractXmlForwardIterator::next() calls, and since the Focus references the same QAbstractXmlForwardIterator,
         * the focus is automatically moved.
         */
        Item::Iterator::Ptr evaluateSequence(const DynamicContext::Ptr &context) const override;

        /**
         * Doesn't return the first item from calling evaluateSequence(), but does the mapping
         * manually. This avoid allocating an ItemMappingIterator.
         */
        Item evaluateSingleton(const DynamicContext::Ptr &context) const override;

        inline Item mapToItem(const Item &subject,
                                   const DynamicContext::Ptr &) const;

        SequenceType::List expectedOperandTypes() const override;
        SequenceType::Ptr staticType() const override;
        ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const override;
        ID id() const override;

        /**
         * @returns always CreatesFocusForLast.
         */
        Properties properties() const override;

        QString description() const override;

    protected:

        /**
         * Creates a GenericPredicate which filters the items from the @p sourceExpression
         * through @p predicate.
         *
         * This constructor is protected. The proper way to create predicates is via the static
         * create() function.
         */
        GenericPredicate(const Expression::Ptr &sourceExpression,
                         const Expression::Ptr &predicate);

        /**
         * @returns the ItemType of the first operand's staticType().
         */
        ItemType::Ptr newFocusType() const override;

    private:
        typedef QExplicitlySharedDataPointer<const GenericPredicate> ConstPtr;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

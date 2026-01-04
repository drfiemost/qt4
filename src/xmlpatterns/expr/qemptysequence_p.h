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

#ifndef Patternist_EmptySequence_H
#define Patternist_EmptySequence_H

#include "qemptycontainer_p.h"


QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Implements the value instance of empty sequence: <tt>()</tt>.
     *
     * @author Frans Englich <frans.englich@nokia.com>
     * @ingroup Patternist_xdm
     */
    class EmptySequence : public EmptyContainer
    {
    public:
        /**
         * @short Creates an EmptySequence that is a replacement for @p
         * replacementFor.
         *
         * @see EmptySequence()
         */
        static Expression::Ptr create(const Expression *const replacementFor,
                                      const StaticContext::Ptr &context);


        /**
         * @short Creates an instance of EmptySequence.
         *
         * @note In most cases create() should be used, since it takes care of
         * adjusting source location annotations.
         *
         * @see create()
         */
        inline EmptySequence()
        = default;

        virtual QString stringValue() const;

        /**
         * @returns always an empty iterator, an instance of EmptyIterator.
         */
        Item::Iterator::Ptr evaluateSequence(const DynamicContext::Ptr &) const override;

        /**
         * @returns always @c null.
         */
        Item evaluateSingleton(const DynamicContext::Ptr &) const override;

        /**
         * Does nothing.
         */
        void evaluateToSequenceReceiver(const DynamicContext::Ptr &) const override;

        /**
         * @returns always @c false.
         */
        bool evaluateEBV(const DynamicContext::Ptr &context) const override;

        /**
         * @returns always CommonSequenceTypes::Empty
         */
        virtual ItemType::Ptr type() const;

        /**
         * @returns always CommonSequenceTypes::Empty
         */
        SequenceType::Ptr staticType() const override;

        ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const override;
        ID id() const override;
        Properties properties() const override;
    };
}

QT_END_NAMESPACE


#endif

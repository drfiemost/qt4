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

#ifndef Patternist_CurrentItemStore_H
#define Patternist_CurrentItemStore_H

#include "qsinglecontainer_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Creates a DynamicContext which provides the focus item for the
     * function @c fn:current().
     *
     * @author Frans Englich <frans.englich@nokia.com>
     * @ingroup Patternist_expressions
     * @since 4.5
     */
    class CurrentItemStore : public SingleContainer
    {
    public:
        CurrentItemStore(const Expression::Ptr &operand);

        bool evaluateEBV(const DynamicContext::Ptr &) const override;
        Item::Iterator::Ptr evaluateSequence(const DynamicContext::Ptr &) const override;
        Item evaluateSingleton(const DynamicContext::Ptr &) const override;

        SequenceType::List expectedOperandTypes() const override;
        Expression::Ptr compress(const StaticContext::Ptr &context) override;
        Expression::Ptr typeCheck(const StaticContext::Ptr &context,
                                          const SequenceType::Ptr &reqType) override;

        Properties properties() const override;

        /**
         * @returns the staticType() of its operand.
         */
        SequenceType::Ptr staticType() const override;
        ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const override;
        const SourceLocationReflection *actualReflection() const override;

    private:
        static inline StaticContext::Ptr newStaticContext(const StaticContext::Ptr &context);
        inline DynamicContext::Ptr createContext(const DynamicContext::Ptr &old) const;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

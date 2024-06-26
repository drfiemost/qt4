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

#ifndef Patternist_SequenceGeneratingFNs_H
#define Patternist_SequenceGeneratingFNs_H

#include "qanyuri_p.h"
#include "qcontextnodechecker_p.h"
#include "qstaticbaseuricontainer_p.h"

/**
 * @file
 * @short Contains classes implementing the functions found in
 * <a href="http://www.w3.org/TR/xpath-functions/#fns-that-generate-sequences">XQuery 1.0 and
 * XPath 2.0 Functions and Operators, 15.5 Functions and Operators that Generate Sequences</a>.
 *
 * @ingroup Patternist_functions
 */

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Implements the function <tt>fn:id()</tt>.
     *
     * @ingroup Patternist_functions
     * @author Frans Englich <frans.englich@nokia.com>
     */
    class IdFN : public ContextNodeChecker
    {
    public:
        IdFN();
        typedef QPair<DynamicContext::Ptr, const QAbstractXmlNodeModel *> IDContext;

        Item::Iterator::Ptr evaluateSequence(const DynamicContext::Ptr &context) const override;

        inline Item mapToItem(const QString &id,
                              const IDContext &context) const;

        Expression::Ptr typeCheck(const StaticContext::Ptr &context,
                                          const SequenceType::Ptr &reqType) override;

    private:
        typedef QExplicitlySharedDataPointer<const IdFN> ConstPtr;
        bool m_hasCreatedSorter;
    };

    /**
     * @short Implements the function <tt>fn:idref()</tt>.
     *
     * @ingroup Patternist_functions
     * @author Frans Englich <frans.englich@nokia.com>
     */
    class IdrefFN : public ContextNodeChecker
    {
    public:
        Item::Iterator::Ptr evaluateSequence(const DynamicContext::Ptr &context) const override;
    };

    /**
     * @short Implements the function <tt>fn:doc()</tt>.
     *
     * @ingroup Patternist_functions
     * @author Frans Englich <frans.englich@nokia.com>
     */
    class DocFN : public StaticBaseUriContainer
    {
    public:
        Item evaluateSingleton(const DynamicContext::Ptr &context) const override;

        /**
         * The implementation of this function is placed in a different compilation unit,
         * namely qsequencefns.cpp, to workaround a compiler bug on
         * solaris-cc-64, suspected to be related to the instantiation of QUrl::toQUrl().
         *
         * @see <a
         * href="http://onesearch.sun.com/search/onesearch/index.jsp?qt=6532605&site=sunsolve&otf=ss&col=support-sunsolve&otf=sunsolve&site=ss&col=search-sunsolve">Sun,
         * multiply-defined label for template instance, bug 6532605</a>
         */
        Expression::Ptr typeCheck(const StaticContext::Ptr &context,
                                          const SequenceType::Ptr &reqType) override;
        SequenceType::Ptr staticType() const override;

    private:
        SequenceType::Ptr m_type;
    };

    /**
     * @short Implements the function <tt>fn:doc-available()</tt>.
     *
     * @ingroup Patternist_functions
     * @author Frans Englich <frans.englich@nokia.com>
     */
    class DocAvailableFN : public StaticBaseUriContainer
    {
    public:
        bool evaluateEBV(const DynamicContext::Ptr &context) const override;
    };

    /**
     * @short Implements the function <tt>fn:collection()</tt>.
     *
     * @ingroup Patternist_functions
     * @author Frans Englich <frans.englich@nokia.com>
     */
    class CollectionFN : public FunctionCall
    {
    public:
        Item::Iterator::Ptr evaluateSequence(const DynamicContext::Ptr &context) const override;
    };
}

QT_END_NAMESPACE
QT_END_HEADER

#endif

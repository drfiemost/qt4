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

#ifndef Patternist_TypeAvailableFN_H
#define Patternist_TypeAvailableFN_H

#include "qschematypefactory_p.h"
#include "qstaticnamespacescontainer_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Implements XSL-T 2.0's XPath function <tt>fn:type-available()</tt>.
     *
     * @see <a href="http://www.w3.org/TR/xslt20/#function-function-available">XSL Transformations
     * (XSLT) Version 2.0, 18.1.1 Testing Availability of Functions</a>
     * @author Frans Englich <frans.englich@nokia.com>
     * @ingroup Patternist_functions
     */
    class TypeAvailableFN : public StaticNamespacesContainer
    {
    public:
        Item evaluateSingleton(const DynamicContext::Ptr &context) const override;

        /**
         * Reimplemented to store data from the @p context which is needed at runtime.
         */
        Expression::Ptr typeCheck(const StaticContext::Ptr &context,
                                          const SequenceType::Ptr &reqType) override;

    private:
        SchemaTypeFactory::Ptr m_schemaTypeFactory;
    };

}

QT_END_NAMESPACE
QT_END_HEADER

#endif

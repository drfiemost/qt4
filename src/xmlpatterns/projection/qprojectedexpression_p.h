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

#ifndef Patternist_ProjectedExpression_H
#define Patternist_ProjectedExpression_H

#include "qitem_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    class ProjectedExpression
    {
    public:
        typedef ProjectedExpression * Ptr;
        typedef QVector<ProjectedExpression::Ptr> Vector;
        virtual ~ProjectedExpression()
        = default;

        enum Action
        {
            Move = 0,
            Skip = 1,
            Keep = 2,
            KeepSubtree = 4 | Keep
        };

        virtual Action actionForElement(const QXmlName name,
                                        ProjectedExpression::Ptr &next) const
        {
            Q_UNUSED(name);
            Q_UNUSED(next);
            return Skip;
        }

    };

    class ProjectedNodeTest
    {
    public:
        typedef ProjectedNodeTest * Ptr;
        virtual ~ProjectedNodeTest()
        = default;

        virtual bool isMatch(const QXmlNodeModelIndex::NodeKind kind) const
        {
            Q_UNUSED(kind);
            return false;
        }
    };

    class ProjectedStep : public ProjectedExpression
    {
    public:
        ProjectedStep(const ProjectedNodeTest::Ptr test,
                      const QXmlNodeModelIndex::Axis axis) : m_test(test),
                                               m_axis(axis)
        {
            Q_ASSERT(m_test);
        }

        Action actionForElement(const QXmlName name,
                                        ProjectedExpression::Ptr &next) const override
        {
            Q_UNUSED(name);
            Q_UNUSED(next);
            // TODO
            return Skip;
        }

    private:
        const ProjectedNodeTest::Ptr    m_test;
        const QXmlNodeModelIndex::Axis                m_axis;
    };

    class ProjectedPath : public ProjectedExpression
    {
    public:
        ProjectedPath(const ProjectedExpression::Ptr left,
                      const ProjectedExpression::Ptr right) : m_left(left),
                                                              m_right(right)
        {
            Q_ASSERT(m_left);
            Q_ASSERT(m_right);
        }

        Action actionForElement(const QXmlName name,
                                        ProjectedExpression::Ptr &next) const override
        {
            ProjectedExpression::Ptr &candidateNext = next;
            const Action a = m_left->actionForElement(name, candidateNext);

            if(a != Skip)
            {
                /* The test accepted it, so let's replace us with the new step. */
                next = candidateNext;
            }

            return a;
        }

    private:
        const ProjectedExpression::Ptr  m_left;
        const ProjectedExpression::Ptr  m_right;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

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

#ifndef Patternist_DocumentProjector_H
#define Patternist_DocumentProjector_H

#include "qprojectedexpression_p.h"
#include "qabstractxmlreceiver.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short
     *
     * @author Frans Englich <frans.englich@nokia.com>
     */
    class DocumentProjector : public QAbstractXmlReceiver
    {
    public:
        DocumentProjector(const ProjectedExpression::Vector &paths,
                          QAbstractXmlReceiver *const receiver);

        void namespaceBinding(const QXmlName &nb) override;

        void characters(const QStringRef &value) override;
        void comment(const QString &value) override;

        void startElement(const QXmlName &name) override;

        void endElement() override;

        void attribute(const QXmlName &name,
                       const QStringRef &value) override;

        void processingInstruction(const QXmlName &target,
                                   const QString &value) override;

        void item(const Item &item) override;

        void startDocument() override;
        void endDocument() override;

        ProjectedExpression::Vector m_paths;
        const int                   m_pathCount;
        ProjectedExpression::Action m_action;
        int                         m_nodesInProcess;
        QAbstractXmlReceiver *const m_receiver;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

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

#ifndef Patternist_DelegatingDynamicContext_H
#define Patternist_DelegatingDynamicContext_H

#include "qdynamiccontext_p.h"
#include "qexpression_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Base class for dynamic contexts that are
     * created from an existing one.
     *
     * In some cases multiple DynamicContext instances must be used in
     * order to maintain somekind of scope. This class delegates
     * the DynamicContext interface onto another DynamicContext instance,
     * allowing the sub-class to only implement what it needs to.
     *
     * @author Frans Englich <frans.englich@nokia.com>
     */
    class DelegatingDynamicContext : public DynamicContext
    {
    public:
        xsInteger contextPosition() const override;
        Item contextItem() const override;
        xsInteger contextSize() override;

        ItemCacheCell &itemCacheCell(const VariableSlotID slot) override;
        ItemSequenceCacheCell::Vector &itemSequenceCacheCells(const VariableSlotID slot) override;

        void setRangeVariable(const VariableSlotID slotNumber,
                                      const Item &newValue) override;
        Item rangeVariable(const VariableSlotID slotNumber) const override;

        void setExpressionVariable(const VariableSlotID slotNumber,
                                           const Expression::Ptr &newValue) override;
        Expression::Ptr expressionVariable(const VariableSlotID slotNumber) const override;

        void setFocusIterator(const Item::Iterator::Ptr &it) override;
        Item::Iterator::Ptr focusIterator() const override;

        Item::Iterator::Ptr positionIterator(const VariableSlotID slot) const override;
        void setPositionIterator(const VariableSlotID slot,
                                         const Item::Iterator::Ptr &newValue) override;

        QAbstractMessageHandler * messageHandler() const override;
        QExplicitlySharedDataPointer<DayTimeDuration> implicitTimezone() const override;
        QDateTime currentDateTime() const override;
        QAbstractXmlReceiver *outputReceiver() const override;
        NodeBuilder::Ptr nodeBuilder(const QUrl &baseURI) const override;
        ResourceLoader::Ptr resourceLoader() const override;
        ExternalVariableLoader::Ptr externalVariableLoader() const override;
        NamePool::Ptr namePool() const override;
        QSourceLocation locationFor(const SourceLocationReflection *const reflection) const override;
        void addNodeModel(const QAbstractXmlNodeModel::Ptr &nm) override;
        const QAbstractUriResolver *uriResolver() const override;
        ItemCacheCell &globalItemCacheCell(const VariableSlotID slot) override;
        ItemSequenceCacheCell::Vector &globalItemSequenceCacheCells(const VariableSlotID slot) override;
        Item currentItem() const override;
        TemplateParameterHash &templateParameterStore() override;

        DynamicContext::Ptr previousContext() const override;
        QExplicitlySharedDataPointer<TemplateMode> currentTemplateMode() const override;

    protected:
        DelegatingDynamicContext(const DynamicContext::Ptr &prevContext);

        const DynamicContext::Ptr m_prevContext;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

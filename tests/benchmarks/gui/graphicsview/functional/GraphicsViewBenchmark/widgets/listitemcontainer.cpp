/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
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

#include <qmath.h>
#include <QGraphicsLinearLayout>
#include <QGraphicsScene>

#include "listitemcontainer.h"
#include "abstractviewitem.h"

#include "recycledlistitem.h"

#if (QT_VERSION >= 0x040600)
#include "listitemcache.h"
#include "itemrecyclinglist.h"
#endif

ListItemContainer::ListItemContainer(int bufferSize, ItemRecyclingList *view, QGraphicsWidget *parent)
    : AbstractItemContainer(bufferSize, parent)
    , m_view(view)
    , m_layout(new QGraphicsLinearLayout(Qt::Vertical))
#if (QT_VERSION >= 0x040600)
    , m_listItemCaching(false)
#endif
{
    setContentsMargins(0,0,0,0);    
    m_layout->setContentsMargins(0,0,0,0);
    m_layout->setSpacing(0);
    setLayout(m_layout);
}

/*virtual*/
ListItemContainer::~ListItemContainer()
{
#if (QT_VERSION >= 0x040600)
    setListItemCaching(false);
#endif
    for (int i = 0; i < m_items.count(); ++i) {
        m_layout->removeItem(m_items.at(i));
        m_items.at(i)->setParentItem(0);
    }
    qDeleteAll(m_items);
    m_items.clear();
}

#if (QT_VERSION >= 0x040600)
bool ListItemContainer::listItemCaching() const
{
    return m_listItemCaching;
}

void ListItemContainer::setListItemCaching(const bool enabled)
{    
    if (m_listItemCaching == enabled)
        return;

    m_listItemCaching = enabled;

    const int itemCount = m_layout->count(); 

    for (int i = 0; i < itemCount; ++i)
        setListItemCaching(enabled, i);
}
#endif

/*virtual*/
void ListItemContainer::adjustVisibleContainerSize(const QSizeF &size)
{
    m_layout->setPreferredWidth(size.width());
}

/*virtual*/
void ListItemContainer::addItemToVisibleLayout(int index, AbstractViewItem *item)
{
    m_layout->insertItem(index,item);

#if (QT_VERSION >= 0x040600)    
    setListItemCaching(m_listItemCaching, index);
#endif
}

/*virtual*/
void ListItemContainer::removeItemFromVisibleLayout(AbstractViewItem *item)
{
    m_layout->removeItem(item);

#if (QT_VERSION >= 0x040600)
    RecycledListItem *recycledItem = static_cast<RecycledListItem*>(item);

    if (!recycledItem)
        return;

    ListItem *listItem = recycledItem->item();

    setListItemCaching(false, listItem);
#endif
}

/*virtual*/
int ListItemContainer::maxItemCountInItemBuffer() const
{
    int count = AbstractItemContainer::maxItemCountInItemBuffer();

    if (count > 0) {
        int currentItemCount = m_items.count();
        qreal heightOfOneItem = 0;
        if (currentItemCount > 0)
        {
            heightOfOneItem = m_layout->effectiveSizeHint(Qt::PreferredSize).height() / currentItemCount;
        }
        int guess = 0;
        if( heightOfOneItem <= 0 ) {
            if (m_prototype) {
                heightOfOneItem = m_prototype->effectiveSizeHint(Qt::PreferredSize).height();
            }
            else
                heightOfOneItem = 50; // TODO magic number, do we have better guess if prototype is not set?
        }
        if (heightOfOneItem > 0) {
            guess = qCeil(m_itemView->boundingRect().height() / heightOfOneItem) + m_bufferSize;

            if (guess < currentItemCount) {
                if( guess > currentItemCount-2) { // TODO magic number here, Can we use buffer size?
                    guess = currentItemCount;
                }
            }
        }
        count = std::min(guess, count);
    }
    return count;
}

#if (QT_VERSION >= 0x040600)
void ListItemContainer::setListItemCaching(const bool enabled, const int index)
{
    RecycledListItem *recycledItem = static_cast<RecycledListItem*>(m_layout->itemAt(index));

    if (!recycledItem)
        return;

    ListItem *listItem = recycledItem->item();

    if (!listItem)
        return;

    setListItemCaching(enabled, listItem);
}

void ListItemContainer::setListItemCaching(const bool enabled, ListItem *listItem)
{
    if (!listItem)
        return;

    // Deletes the effect.
    listItem->setGraphicsEffect(0);

    if (enabled) {
        ListItemCache* cache = new ListItemCache;
        Q_ASSERT(!listItem->graphicsEffect());
        listItem->setGraphicsEffect(cache);
    }
}
#endif


void ListItemContainer::setTwoColumns(const bool twoColumns)
{
    AbstractItemContainer::setTwoColumns(twoColumns);

    if (!m_layout->isActivated())
        m_layout->activate();
}


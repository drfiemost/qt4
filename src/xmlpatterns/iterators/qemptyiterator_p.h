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

#ifndef Patternist_EmptyIterator_H
#define Patternist_EmptyIterator_H

#include "qabstractxmlforwarditerator_p.h"
#include "qprimitives_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short An QAbstractXmlForwardIterator which always is empty.
     *
     * EmptyIterator is an QAbstractXmlForwardIterator over the type @c T, which always is empty. Other
     * iterators can also be empty(or, at least behave as they are empty), but this
     * class is special designed for this purpose and is therefore fast.
     *
     * EmptyIterator's constructor is protected, instances is retrieved from CommonValues.
     *
     * @author Frans Englich <frans.englich@nokia.com>
     * @ingroup Patternist_iterators
     */
    template<typename T> class EmptyIterator : public QAbstractXmlForwardIterator<T>
    {
    public:
        /**
         * @returns always a default constructed value, T().
         */
        T next() override
        {
            return T();
        }

        /**
         * @returns always a default constructed value, T().
         */
        T current() const override
        {
            return T();
        }

        /**
         * @returns always 0.
         */
        xsInteger position() const override
        {
            return 0;
        }

        /**
         * @returns always @c this, the reverse of <tt>()</tt> is <tt>()</tt>.
         */
        typename QAbstractXmlForwardIterator<T>::Ptr toReversed() override
        {
            return typename QAbstractXmlForwardIterator<T>::Ptr(const_cast<EmptyIterator<T> *>(this));
        }

        /**
         * @returns always 0
         */
        xsInteger count() override
        {
            return 0;
        }

        /**
         * @returns @c this
         */
        typename QAbstractXmlForwardIterator<T>::Ptr copy() const override
        {
            return typename QAbstractXmlForwardIterator<T>::Ptr(const_cast<EmptyIterator *>(this));
        }

    protected:
        friend class CommonValues;
    };

    template<typename T>
    static inline
    typename QAbstractXmlForwardIterator<T>::Ptr
    makeEmptyIterator()
    {
        return typename QAbstractXmlForwardIterator<T>::Ptr(new EmptyIterator<T>());
    }

}

QT_END_NAMESPACE

QT_END_HEADER

#endif

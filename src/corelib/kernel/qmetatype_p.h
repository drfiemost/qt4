/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMETATYPE_P_H
#define QMETATYPE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qmetatype.h"

QT_BEGIN_NAMESPACE

class QMetaTypeInterface
{
private:
    template<typename T>
    struct Impl {
        static void *constr(const T *t)
        {
            if (t)
                return new T(*t);
            return new T();
        }

        static void destr(T *t) { delete t; }
    #ifndef QT_NO_DATASTREAM
        static void saver(QDataStream &stream, const T *t) { stream << *t; }
        static void loader(QDataStream &stream, T *t) { stream >> *t; }
    #endif // QT_NO_DATASTREAM
    };
public:
    template<typename T>
    explicit QMetaTypeInterface(T *)
        : constr(reinterpret_cast<QMetaType::Constructor>(Impl<T>::constr))
              , destr(reinterpret_cast<QMetaType::Destructor>(Impl<T>::destr))
          #ifndef QT_NO_DATASTREAM
              , saveOp(reinterpret_cast<QMetaType::SaveOperator>(Impl<T>::saver))
              , loadOp(reinterpret_cast<QMetaType::LoadOperator>(Impl<T>::loader))
          #endif
    {}

    QMetaTypeInterface()
        : constr(nullptr)
        , destr(nullptr)
    #ifndef QT_NO_DATASTREAM
        , saveOp(nullptr)
        , loadOp(nullptr)
    #endif
    {}

    QMetaType::Constructor constr;
    QMetaType::Destructor destr;
#ifndef QT_NO_DATASTREAM
    QMetaType::SaveOperator saveOp;
    QMetaType::LoadOperator loadOp;
#endif
};

QT_END_NAMESPACE

#endif // QMETATYPE_P_H
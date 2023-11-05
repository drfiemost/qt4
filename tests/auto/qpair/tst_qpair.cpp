/****************************************************************************
**
** Copyright (C) 2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author Marc Mutz <marc.mutz@kdab.com>
** Contact: http://www.qt-project.org/
**
** This file is part of the test suite of the Qt Toolkit.
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>

#include <QPair>

class tst_QPair : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void dummy() {}
};

class C { char _[4]; };
class M { char _[4]; };
class P { char _[4]; };

QT_BEGIN_NAMESPACE
Q_DECLARE_TYPEINFO(M, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(P, Q_PRIMITIVE_TYPE);
QT_END_NAMESPACE

// avoid the comma:
typedef QPair<C,C> QPairCC;
typedef QPair<C,M> QPairCM;
typedef QPair<C,P> QPairCP;
typedef QPair<M,C> QPairMC;
typedef QPair<M,M> QPairMM;
typedef QPair<M,P> QPairMP;
typedef QPair<P,C> QPairPC;
typedef QPair<P,M> QPairPM;
typedef QPair<P,P> QPairPP;

static_assert( QTypeInfo<QPairCC>::isComplex);
static_assert( QTypeInfo<QPairCC>::isStatic );

static_assert( QTypeInfo<QPairCM>::isComplex);
static_assert( QTypeInfo<QPairCM>::isStatic );

static_assert( QTypeInfo<QPairCP>::isComplex);
static_assert( QTypeInfo<QPairCP>::isStatic );

static_assert( QTypeInfo<QPairMC>::isComplex);
static_assert( QTypeInfo<QPairMC>::isStatic );

static_assert( QTypeInfo<QPairMM>::isComplex);
static_assert(!QTypeInfo<QPairMM>::isStatic );

static_assert( QTypeInfo<QPairMP>::isComplex);
static_assert(!QTypeInfo<QPairMP>::isStatic );

static_assert( QTypeInfo<QPairPC>::isComplex);
static_assert( QTypeInfo<QPairPC>::isStatic );

static_assert( QTypeInfo<QPairPM>::isComplex);
static_assert(!QTypeInfo<QPairPM>::isStatic );

static_assert(!QTypeInfo<QPairPP>::isComplex);
static_assert(!QTypeInfo<QPairPP>::isStatic );

static_assert(!QTypeInfo<QPairPP>::isDummy  );
static_assert(!QTypeInfo<QPairPP>::isPointer);


QTEST_APPLESS_MAIN(tst_QPair)
#include "tst_qpair.moc"

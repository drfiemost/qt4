/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
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
#include <QtTest/QtTest>

class tst_QFlags: public QObject
{
    Q_OBJECT
private slots:
    void boolCasts() const;
    void testFlag() const;
    void testFlagZeroFlag() const;
    void testFlagMultiBits() const;
    //void testFlags();
    void testAnyFlag();
    void constExpr();
    void signedness();
    void classEnum();
    void initializerLists();
    void testSetFlags();
    void adl();
};

void tst_QFlags::boolCasts() const
{
    // This tests that the operator overloading is sufficient so that common
    // idioms involving flags -> bool casts work as expected:

    const Qt::Alignment nonNull = Qt::AlignCenter;
    const Qt::Alignment null = {};

    // basic premiss:
    QVERIFY(bool(nonNull));
    QVERIFY(!bool(null));

    // The rest is just checking that stuff compiles:

    // QVERIFY should compile:
    QVERIFY(nonNull);
    QVERIFY(!null);

    // ifs should compile:
    if (null) QFAIL("Can't contextually convert QFlags to bool!");
    if (!nonNull) QFAIL("Missing operator! on QFlags (shouldn't be necessary).");

    // ternary should compile:
    QVERIFY(nonNull ? true : false);
    QVERIFY(!null ? true : false);

    // logical operators should compile:
    QVERIFY(nonNull && true);
    QVERIFY(nonNull || false);
    QVERIFY(!null && true);
    QVERIFY(!null || false);

    // ... in both directions:
    QVERIFY(true && nonNull);
    QVERIFY(false || nonNull);
    QVERIFY(true && !null);
    QVERIFY(false || !null);

    // ... and mixed:
    QVERIFY(null || nonNull);
    QVERIFY(!(null && nonNull));
}

void tst_QFlags::testFlag() const
{
    Qt::MouseButtons btn = Qt::LeftButton | Qt::RightButton;

    QVERIFY(btn.testFlag(Qt::LeftButton));
    QVERIFY(!btn.testFlag(Qt::MidButton));

    btn = 0;
    QVERIFY(!btn.testFlag(Qt::LeftButton));
}

void tst_QFlags::testFlagZeroFlag() const
{
    {
        Qt::MouseButtons btn = Qt::LeftButton | Qt::RightButton;
        /* Qt::NoButton has the value 0. */

        QVERIFY(!btn.testFlag(Qt::NoButton));
    }

    {
        /* A zero enum set should test true with zero. */
        QVERIFY(Qt::MouseButtons().testFlag(Qt::NoButton));
    }

    {
        Qt::MouseButtons btn = Qt::NoButton;
        QVERIFY(btn.testFlag(Qt::NoButton));
    }
}

void tst_QFlags::testFlagMultiBits() const
{
    /* Qt::Window is 0x00000001
     * Qt::Dialog is 0x00000002 | Window
     */
    {
        const Qt::WindowFlags onlyWindow(Qt::Window);
        QVERIFY(!onlyWindow.testFlag(Qt::Dialog));
    }

    {
        const Qt::WindowFlags hasDialog(Qt::Dialog);
        QVERIFY(hasDialog.testFlag(Qt::Dialog));
    }
}
/*
void tst_QFlags::testFlags()
{
    using Int = Qt::TextInteractionFlags::Int;
    constexpr Int Zero(0);

    Qt::TextInteractionFlags flags;
    QCOMPARE(flags.toInt(), Zero);
    QVERIFY(flags.testFlags(flags));
    QVERIFY(Qt::TextInteractionFlags::fromInt(Zero).testFlags(flags));
    QVERIFY(!flags.testFlags(Qt::TextSelectableByMouse));
    QVERIFY(!flags.testFlags(Qt::TextSelectableByKeyboard));
    QVERIFY(!flags.testFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard));
    QVERIFY(flags.testFlags(Qt::TextInteractionFlags::fromInt(Zero)));
    QVERIFY(flags.testFlags(Qt::TextInteractionFlags(Qt::TextSelectableByMouse) & ~Qt::TextSelectableByMouse));

    flags = Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard;
    QVERIFY(flags.toInt() != Zero);
    QVERIFY(flags.testFlags(flags));
    QVERIFY(flags.testFlags(Qt::TextSelectableByMouse));
    QVERIFY(flags.testFlags(Qt::TextSelectableByKeyboard));
    QVERIFY(flags.testFlags(Qt::TextSelectableByKeyboard | Qt::TextSelectableByMouse));
    QVERIFY(!flags.testFlags(Qt::TextSelectableByKeyboard | Qt::TextSelectableByMouse | Qt::TextEditable));
    QVERIFY(!flags.testFlags(Qt::TextInteractionFlags()));
    QVERIFY(!flags.testFlags(Qt::TextInteractionFlags::fromInt(Zero)));
    QVERIFY(!flags.testFlags(Qt::TextEditable));
    QVERIFY(!flags.testFlags(Qt::TextSelectableByMouse | Qt::TextEditable));
}
*/
void tst_QFlags::testAnyFlag()
{
    Qt::TextInteractionFlags flags;
    QVERIFY(!flags.testAnyFlags(Qt::NoTextInteraction));
    QVERIFY(!flags.testAnyFlags(Qt::TextSelectableByMouse));
    QVERIFY(!flags.testAnyFlags(Qt::TextSelectableByKeyboard));
    QVERIFY(!flags.testAnyFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard));
    QVERIFY(!flags.testAnyFlag(Qt::TextEditorInteraction));
    QVERIFY(!flags.testAnyFlag(Qt::TextBrowserInteraction));

    flags = Qt::TextSelectableByMouse;
    QVERIFY(!flags.testAnyFlags(Qt::NoTextInteraction));
    QVERIFY(flags.testAnyFlags(Qt::TextSelectableByMouse));
    QVERIFY(!flags.testAnyFlags(Qt::TextSelectableByKeyboard));
    QVERIFY(flags.testAnyFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard));
    QVERIFY(flags.testAnyFlag(Qt::TextEditorInteraction));
    QVERIFY(flags.testAnyFlag(Qt::TextBrowserInteraction));

    flags = Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard;
    QVERIFY(!flags.testAnyFlags(Qt::NoTextInteraction));
    QVERIFY(flags.testAnyFlags(Qt::TextSelectableByMouse));
    QVERIFY(flags.testAnyFlags(Qt::TextSelectableByKeyboard));
    QVERIFY(flags.testAnyFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard));
    QVERIFY(flags.testAnyFlag(Qt::TextEditorInteraction));
    QVERIFY(flags.testAnyFlag(Qt::TextEditorInteraction));
    QVERIFY(flags.testAnyFlag(Qt::TextBrowserInteraction));
}

template <unsigned int N, typename T> bool verifyConstExpr(T n) { return n == N; }

void tst_QFlags::constExpr()
{
#ifdef Q_COMPILER_CONSTEXPR
    Qt::MouseButtons btn = Qt::LeftButton | Qt::RightButton;
    switch (btn) {
        case Qt::LeftButton: QVERIFY(false); break;
        case Qt::RightButton: QVERIFY(false); break;
        case Qt::LeftButton | Qt::RightButton: QVERIFY(true); break;
        default: QVERIFY(false);
    }

    QVERIFY(verifyConstExpr<(Qt::LeftButton | Qt::RightButton) & Qt::LeftButton>(Qt::LeftButton));
    QVERIFY(verifyConstExpr<(Qt::LeftButton | Qt::RightButton) & Qt::MiddleButton>(0));
    QVERIFY(verifyConstExpr<(Qt::LeftButton | Qt::RightButton) | Qt::MiddleButton>(Qt::LeftButton | Qt::RightButton | Qt::MiddleButton));
    QVERIFY(verifyConstExpr<~(Qt::LeftButton | Qt::RightButton)>(~(Qt::LeftButton | Qt::RightButton)));
    QVERIFY(verifyConstExpr<Qt::MouseButtons(Qt::LeftButton) ^ Qt::RightButton>(Qt::LeftButton ^ Qt::RightButton));
    QVERIFY(verifyConstExpr<Qt::MouseButtons(0)>(0));
    QVERIFY(verifyConstExpr<Qt::MouseButtons(Qt::RightButton) & 0xff>(Qt::RightButton));
    QVERIFY(verifyConstExpr<Qt::MouseButtons(Qt::RightButton) | 0xff>(0xff));

    QVERIFY(!verifyConstExpr<Qt::RightButton>(~Qt::MouseButtons(Qt::LeftButton)));
#endif
}

void tst_QFlags::signedness()
{
    // these are all 'true' on GCC, but since the std says the
    // underlying type is implementation-defined, we need to allow for
    // a different signedness, so we only check that the relative
    // signedness of the types matches:
    static_assert((std::is_unsigned<typename std::underlying_type<Qt::MouseButton>::type>::value ==
                     std::is_unsigned<Qt::MouseButtons::Int>::value));

    static_assert((std::is_signed<typename std::underlying_type<Qt::AlignmentFlag>::type>::value ==
                     std::is_signed<Qt::Alignment::Int>::value));
}

#if defined(Q_COMPILER_CLASS_ENUM)
enum class MyStrictEnum { StrictZero, StrictOne, StrictTwo, StrictFour=4 };
Q_DECLARE_FLAGS( MyStrictFlags, MyStrictEnum )
Q_DECLARE_OPERATORS_FOR_FLAGS( MyStrictFlags )

enum class MyStrictNoOpEnum { StrictZero, StrictOne, StrictTwo, StrictFour=4 };
Q_DECLARE_FLAGS( MyStrictNoOpFlags, MyStrictNoOpEnum )

static_assert( !QTypeInfo<MyStrictFlags>::isComplex );
static_assert( !QTypeInfo<MyStrictFlags>::isStatic );
static_assert( !QTypeInfo<MyStrictFlags>::isLarge );
static_assert( !QTypeInfo<MyStrictFlags>::isPointer );
#endif

void tst_QFlags::classEnum()
{
#if defined(Q_COMPILER_CLASS_ENUM)
    // The main aim of the test is making sure it compiles
    // The QCOMPARE are there as an extra
    MyStrictEnum e1 = MyStrictEnum::StrictOne;
    MyStrictEnum e2 = MyStrictEnum::StrictTwo;

    MyStrictFlags f1(MyStrictEnum::StrictOne);
    QCOMPARE(f1, 1);

    MyStrictFlags f2(e2);
    QCOMPARE(f2, 2);

    MyStrictFlags f0;
    QCOMPARE(f0, 0);

    MyStrictFlags f3(e2 | e1);
    QCOMPARE(f3, 3);

    QVERIFY(f3.testFlag(MyStrictEnum::StrictOne));
    QVERIFY(!f1.testFlag(MyStrictEnum::StrictTwo));

    QVERIFY(!f0);

    QCOMPARE(f3 & int(1), 1);
    QCOMPARE(f3 & uint(1), 1);
    QCOMPARE(f3 & MyStrictEnum::StrictOne, 1);

    MyStrictFlags aux;
    aux = f3;
    aux &= int(1);
    QCOMPARE(aux, 1);

    aux = f3;
    aux &= uint(1);
    QCOMPARE(aux, 1);

    aux = f3;
    aux &= MyStrictEnum::StrictOne;
    QCOMPARE(aux, 1);

    aux = f3;
    aux &= f1;
    QCOMPARE(aux, 1);

    aux = f3 ^ f3;
    QCOMPARE(aux, 0);

    aux = f3 ^ f1;
    QCOMPARE(aux, 2);

    aux = f3 ^ f0;
    QCOMPARE(aux, 3);

    aux = f3 ^ MyStrictEnum::StrictOne;
    QCOMPARE(aux, 2);

    aux = f3 ^ MyStrictEnum::StrictZero;
    QCOMPARE(aux, 3);

    aux = f3;
    aux ^= f3;
    QCOMPARE(aux, 0);

    aux = f3;
    aux ^= f1;
    QCOMPARE(aux, 2);

    aux = f3;
    aux ^= f0;
    QCOMPARE(aux, 3);

    aux = f3;
    aux ^= MyStrictEnum::StrictOne;
    QCOMPARE(aux, 2);

    aux = f3;
    aux ^= MyStrictEnum::StrictZero;
    QCOMPARE(aux, 3);

    aux = f1 | f2;
    QCOMPARE(aux, 3);

    aux = MyStrictEnum::StrictOne | MyStrictEnum::StrictTwo;
    QCOMPARE(aux, 3);

    aux = f1;
    aux |= f2;
    QCOMPARE(aux, 3);

    aux = MyStrictEnum::StrictOne;
    aux |= MyStrictEnum::StrictTwo;
    QCOMPARE(aux, 3);

    aux = ~f1;
    QCOMPARE(aux, -2);

    // Just to make sure it compiles
    if (false)
        qDebug() << f3;
#endif
}

void tst_QFlags::initializerLists()
{
#if defined(Q_COMPILER_INITIALIZER_LISTS)
    Qt::MouseButtons bts = { Qt::LeftButton, Qt::RightButton };
    QVERIFY(bts.testFlag(Qt::LeftButton));
    QVERIFY(bts.testFlag(Qt::RightButton));
    QVERIFY(!bts.testFlag(Qt::MiddleButton));

#if defined(Q_COMPILER_CLASS_ENUM)
    MyStrictNoOpFlags flags = { MyStrictNoOpEnum::StrictOne, MyStrictNoOpEnum::StrictFour };
    QVERIFY(flags.testFlag(MyStrictNoOpEnum::StrictOne));
    QVERIFY(flags.testFlag(MyStrictNoOpEnum::StrictFour));
    QVERIFY(!flags.testFlag(MyStrictNoOpEnum::StrictTwo));
#endif // Q_COMPILER_CLASS_ENUM

#else
    QSKIP("This test requires C++11 initializer_list support.");
#endif // Q_COMPILER_INITIALIZER_LISTS
}

void tst_QFlags::testSetFlags()
{
    Qt::MouseButtons btn = Qt::NoButton;

    btn.setFlag(Qt::LeftButton);
    QVERIFY(btn.testFlag(Qt::LeftButton));
    QVERIFY(!btn.testFlag(Qt::MidButton));

    btn.setFlag(Qt::LeftButton, false);
    QVERIFY(!btn.testFlag(Qt::LeftButton));
    QVERIFY(!btn.testFlag(Qt::MidButton));
}

namespace SomeNS {
enum Foo { Foo_A = 1 << 0, Foo_B = 1 << 1, Foo_C = 1 << 2 };

Q_DECLARE_FLAGS(Foos, Foo)
Q_DECLARE_OPERATORS_FOR_FLAGS(Foos);

Qt::Alignment alignment()
{
    // Checks that the operator| works, despite there is another operator| in this namespace.
    return Qt::AlignLeft | Qt::AlignTop;
}
}

void tst_QFlags::adl()
{
    SomeNS::Foos fl = SomeNS::Foo_B | SomeNS::Foo_C;
    QVERIFY(fl & SomeNS::Foo_B);
    QVERIFY(!(fl & SomeNS::Foo_A));
    QCOMPARE(SomeNS::alignment(), Qt::AlignLeft | Qt::AlignTop);
}

// (statically) check QTypeInfo for QFlags instantiations:
enum MyEnum { Zero, One, Two, Four=4 };
Q_DECLARE_FLAGS( MyFlags, MyEnum )
Q_DECLARE_OPERATORS_FOR_FLAGS( MyFlags )

static_assert( !QTypeInfo<MyFlags>::isComplex );
static_assert( !QTypeInfo<MyFlags>::isStatic );
static_assert( !QTypeInfo<MyFlags>::isLarge );
static_assert( !QTypeInfo<MyFlags>::isPointer );


QTEST_MAIN(tst_QFlags)
#include "tst_qflags.moc"

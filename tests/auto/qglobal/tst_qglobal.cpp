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

#include <QPair>
#include <QTextCodec>

class tst_QGlobal: public QObject
{
    Q_OBJECT
private slots:
    void qIsNull();
    void qInternalCallbacks();
    void for_each();
    void qassert();
    void qtry();
    void checkptr();
    void qAlignOf();
    void integerForSize();
    void qprintable();
    void qprintable_data();
};

void tst_QGlobal::qIsNull()
{
    double d = 0.0;
    float f = 0.0f;

    QVERIFY(::qIsNull(d));
    QVERIFY(::qIsNull(f));

    d += 0.000000001;
    f += 0.0000001f;

    QVERIFY(!::qIsNull(d));
    QVERIFY(!::qIsNull(f));
}

struct ConnectInfo {
    QObject *sender;
    QObject *receiver;
    QString signal, slot;
    int type;
    void reset() {
        sender = receiver = 0;
        signal = slot = QString();
        type = -1;
    }
} connect_info;

bool disconnect_callback(void **data)
{
    connect_info.sender = (QObject *)(data[0]);
    connect_info.receiver = (QObject *)(data[2]);
    connect_info.signal = QString::fromLatin1((const char *) data[1]);
    connect_info.slot = QString::fromLatin1((const char *) data[3]);
    return true;
}

bool connect_callback(void **data)
{
    disconnect_callback(data);
    connect_info.type = *(int *) data[4];
    return true;
}

void tst_QGlobal::qInternalCallbacks()
{
    QInternal::registerCallback(QInternal::ConnectCallback, connect_callback);
    QInternal::registerCallback(QInternal::DisconnectCallback, disconnect_callback);

    QObject a, b;
    QString signal = QLatin1String("2mysignal(x)");
    QString slot = QLatin1String("1myslot(x)");

    // Test that connect works as expected...
    connect_info.reset();
    bool ok = QObject::connect(&a, signal.toLatin1(), &b, slot.toLatin1(), Qt::AutoConnection);
    QVERIFY(ok);
    QCOMPARE(&a, connect_info.sender);
    QCOMPARE(&b, connect_info.receiver);
    QCOMPARE(signal, connect_info.signal);
    QCOMPARE(slot, connect_info.slot);
    QCOMPARE((int) Qt::AutoConnection, connect_info.type);

    // Test that disconnect works as expected
    connect_info.reset();
    ok = QObject::disconnect(&a, signal.toLatin1(), &b, slot.toLatin1());
    QVERIFY(ok);
    QCOMPARE(&a, connect_info.sender);
    QCOMPARE(&b, connect_info.receiver);
    QCOMPARE(signal, connect_info.signal);
    QCOMPARE(slot, connect_info.slot);

    // Unregister callbacks and verify that they are not triggered...
    QInternal::unregisterCallback(QInternal::ConnectCallback, connect_callback);
    QInternal::unregisterCallback(QInternal::DisconnectCallback, disconnect_callback);

    connect_info.reset();
    ok = QObject::connect(&a, signal.toLatin1(), &b, slot.toLatin1(), Qt::AutoConnection);
    QVERIFY(!ok);
    QCOMPARE(connect_info.sender, (QObject *) 0);

    ok = QObject::disconnect(&a, signal.toLatin1(), &b, slot.toLatin1());
    QVERIFY(!ok);
    QCOMPARE(connect_info.sender, (QObject *) 0);
}

void tst_QGlobal::for_each()
{
    QList<int> list;
    list << 0 << 1 << 2 << 3 << 4 << 5;

    int counter = 0;
    foreach(int i, list) {
        QCOMPARE(i, counter++);
    }
    QCOMPARE(counter, list.count());

    // do it again, to make sure we don't have any for-scoping
    // problems with older compilers
    counter = 0;
    foreach(int i, list) {
        QCOMPARE(i, counter++);
    }
    QCOMPARE(counter, list.count());
}

void tst_QGlobal::qassert()
{
    bool passed = false;
    if (false) {
        Q_ASSERT(false);
    } else {
        passed = true;
    }
    QVERIFY(passed);

    passed = false;
    if (false) {
        Q_ASSERT_X(false, "tst_QGlobal", "qassert");
    } else {
        passed = true;
    }
    QVERIFY(passed);

    passed = false;
    if (false)
        Q_ASSERT(false);
    else
        passed = true;
    QVERIFY(passed);

    passed = false;
    if (false)
        Q_ASSERT_X(false, "tst_QGlobal", "qassert");
    else
        passed = true;
    QVERIFY(passed);
}

void tst_QGlobal::qtry()
{
    int i = 0;
    QT_TRY {
        i = 1;
        QT_THROW(42);
        i = 2;
    } QT_CATCH(int) {
        QCOMPARE(i, 1);
        i = 7;
    }
#ifdef QT_NO_EXCEPTIONS
    QCOMPARE(i, 2);
#else
    QCOMPARE(i, 7);
#endif

    // check propper if/else scoping
    i = 0;
    if (true)
        QT_TRY {
            i = 2;
            QT_THROW(42);
            i = 4;
        } QT_CATCH(int) {
            QCOMPARE(i, 2);
            i = 4;
        }
    else
        QCOMPARE(i, 0);
    QCOMPARE(i, 4);

    i = 0;
    if (false)
        QT_TRY {
            i = 2;
            QT_THROW(42);
            i = 4;
        } QT_CATCH(int) {
            QCOMPARE(i, 2);
            i = 2;
        }
    else
        i = 8;
    QCOMPARE(i, 8);

    i = 0;
    if (false)
        i = 42;
    else
        QT_TRY {
            i = 2;
            QT_THROW(42);
            i = 4;
        } QT_CATCH(int) {
            QCOMPARE(i, 2);
            i = 4;
        }
    QCOMPARE(i, 4);
}

void tst_QGlobal::checkptr()
{
    int i;
    QCOMPARE(q_check_ptr(&i), &i);

    const char *c = "hello";
    QCOMPARE(q_check_ptr(c), c);
}

struct Empty {};
template <class T> struct AlignmentInStruct { T dummy; };

typedef int (*fun) ();
typedef int (Empty::*memFun) ();

#define TEST_AlignOf(type, alignment)                                       \
    do {                                                                    \
        TEST_AlignOf_impl(type, alignment);                                 \
                                                                            \
        TEST_AlignOf_impl(type &, alignment);                               \
        TEST_AlignOf_RValueRef(type &&, alignment);                         \
                                                                            \
        TEST_AlignOf_impl(type [5], alignment);                             \
        TEST_AlignOf_impl(type (&) [5], alignment);                         \
                                                                            \
        TEST_AlignOf_impl(AlignmentInStruct<type>, alignment);              \
                                                                            \
        /* Some internal sanity validation, just for fun */                 \
        TEST_AlignOf_impl(AlignmentInStruct<type [5]>, alignment);          \
        TEST_AlignOf_impl(AlignmentInStruct<type &>, Q_ALIGNOF(void *));    \
        TEST_AlignOf_impl(AlignmentInStruct<type (&) [5]>,                  \
                Q_ALIGNOF(void *));                                         \
        TEST_AlignOf_RValueRef(AlignmentInStruct<type &&>,                  \
                Q_ALIGNOF(void *));                                         \
    } while (false)                                                         \
    /**/

#ifdef Q_COMPILER_RVALUE_REFS
#define TEST_AlignOf_RValueRef(type, alignment) \
        TEST_AlignOf_impl(type, alignment)
#else
#define TEST_AlignOf_RValueRef(type, alignment) do {} while (false)
#endif

#define TEST_AlignOf_impl(type, alignment) \
    do { \
        QCOMPARE(Q_ALIGNOF(type), size_t(alignment)); \
        /* Compare to native operator for compilers that support it,
           otherwise...  erm... check consistency! :-) */ \
        QCOMPARE(QT_EMULATED_ALIGNOF(type), Q_ALIGNOF(type)); \
    } while (false)
    /**/

void tst_QGlobal::qAlignOf()
{
    // Built-in types, except 64-bit integers and double
    TEST_AlignOf(char, 1);
    TEST_AlignOf(signed char, 1);
    TEST_AlignOf(unsigned char, 1);
    TEST_AlignOf(qint8, 1);
    TEST_AlignOf(quint8, 1);
    TEST_AlignOf(qint16, 2);
    TEST_AlignOf(quint16, 2);
    TEST_AlignOf(qint32, 4);
    TEST_AlignOf(quint32, 4);
    TEST_AlignOf(void *, sizeof(void *));

    // Depends on platform and compiler, disabling test for now
    // TEST_AlignOf(long double, 16);

    // Empty struct
    TEST_AlignOf(Empty, 1);

    // Function pointers
    TEST_AlignOf(fun, Q_ALIGNOF(void *));
    TEST_AlignOf(memFun, Q_ALIGNOF(void *));


    // 64-bit integers and double
    TEST_AlignOf_impl(qint64, 8);
    TEST_AlignOf_impl(quint64, 8);
    TEST_AlignOf_impl(double, 8);

    TEST_AlignOf_impl(qint64 &, 8);
    TEST_AlignOf_impl(quint64 &, 8);
    TEST_AlignOf_impl(double &, 8);

    TEST_AlignOf_RValueRef(qint64 &&, 8);
    TEST_AlignOf_RValueRef(quint64 &&, 8);
    TEST_AlignOf_RValueRef(double &&, 8);

    // 32-bit x86 ABI idiosyncrasies
#if defined(Q_PROCESSOR_X86_32) && !defined(Q_OS_WIN)
    TEST_AlignOf_impl(AlignmentInStruct<qint64>, 4);
#else
    TEST_AlignOf_impl(AlignmentInStruct<qint64>, 8);
#endif

    TEST_AlignOf_impl(AlignmentInStruct<quint64>, Q_ALIGNOF(AlignmentInStruct<qint64>));
    TEST_AlignOf_impl(AlignmentInStruct<double>, Q_ALIGNOF(AlignmentInStruct<qint64>));

    // 32-bit x86 ABI, Clang disagrees with gcc
#if !defined(Q_PROCESSOR_X86_32) || !defined(Q_CC_CLANG)
    TEST_AlignOf_impl(qint64 [5],       Q_ALIGNOF(qint64));
#else
    TEST_AlignOf_impl(qint64 [5],       Q_ALIGNOF(AlignmentInStruct<qint64>));
#endif

    TEST_AlignOf_impl(qint64 (&) [5],   Q_ALIGNOF(qint64 [5]));
    TEST_AlignOf_impl(quint64 [5],      Q_ALIGNOF(quint64 [5]));
    TEST_AlignOf_impl(quint64 (&) [5],  Q_ALIGNOF(quint64 [5]));
    TEST_AlignOf_impl(double [5],       Q_ALIGNOF(double [5]));
    TEST_AlignOf_impl(double (&) [5],   Q_ALIGNOF(double [5]));
}

#undef TEST_AlignOf
#undef TEST_AlignOf_RValueRef
#undef TEST_AlignOf_impl

void tst_QGlobal::integerForSize()
{
    // compile-only test:
    static_assert(sizeof(QIntegerForSize<1>::Signed) == 1);
    static_assert(sizeof(QIntegerForSize<2>::Signed) == 2);
    static_assert(sizeof(QIntegerForSize<4>::Signed) == 4);
    static_assert(sizeof(QIntegerForSize<8>::Signed) == 8);

    static_assert(sizeof(QIntegerForSize<1>::Unsigned) == 1);
    static_assert(sizeof(QIntegerForSize<2>::Unsigned) == 2);
    static_assert(sizeof(QIntegerForSize<4>::Unsigned) == 4);
    static_assert(sizeof(QIntegerForSize<8>::Unsigned) == 8);
}

typedef QPair<const char *, const char *> stringpair;
Q_DECLARE_METATYPE(stringpair)

void tst_QGlobal::qprintable()
{
    QFETCH(QList<stringpair>, localestrings);
    QFETCH(int, utf8index);

    QVERIFY(utf8index >= 0 && utf8index < localestrings.count());
    if (utf8index < 0 || utf8index >= localestrings.count())
        return;

    const char *const utf8string = localestrings.at(utf8index).second;

    QString string = QString::fromUtf8(utf8string);

    foreach (const stringpair &pair, localestrings) {
        QTextCodec *codec = QTextCodec::codecForName(pair.first);
        if (!codec)
            continue;
        QTextCodec::setCodecForLocale(codec);
        // test qPrintable()
        QVERIFY(qstrcmp(qPrintable(string), pair.second) == 0);
        foreach (const stringpair &pair2, localestrings) {
            if (pair2.second == pair.second)
                continue;
            QVERIFY(qstrcmp(qPrintable(string), pair2.second) != 0);
        }
        // test qUtf8Printable()
        QVERIFY(qstrcmp(qUtf8Printable(string), utf8string) == 0);
        foreach (const stringpair &pair2, localestrings) {
            if (qstrcmp(pair2.second, utf8string) == 0)
                continue;
            QVERIFY(qstrcmp(qUtf8Printable(string), pair2.second) != 0);
        }
    }

    QTextCodec::setCodecForLocale(0);
}

void tst_QGlobal::qprintable_data()
{
    QTest::addColumn<QList<stringpair> >("localestrings");
    QTest::addColumn<int>("utf8index"); // index of utf8 string

    // Unicode: HIRAGANA LETTER A, I, U, E, O (U+3442, U+3444, U+3446, U+3448, U+344a)
    static const char *const utf8string = "\xe3\x81\x82\xe3\x81\x84\xe3\x81\x86\xe3\x81\x88\xe3\x81\x8a";
    static const char *const eucjpstring = "\xa4\xa2\xa4\xa4\xa4\xa6\xa4\xa8\xa4\xaa";
    static const char *const sjisstring = "\x82\xa0\x82\xa2\x82\xa4\x82\xa6\x82\xa8";

    QList<stringpair> japanesestrings;
    japanesestrings << stringpair("UTF-8", utf8string)
                    << stringpair("EUC-JP", eucjpstring)
                    << stringpair("Shift_JIS", sjisstring);

    QTest::newRow("Japanese") << japanesestrings << 0;

}

QTEST_MAIN(tst_QGlobal)
#include "tst_qglobal.moc"

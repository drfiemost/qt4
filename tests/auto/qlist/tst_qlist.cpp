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
#include <QList>

//TESTED_CLASS=QList
//TESTED_FILES=corelib/tools/qlist.h corelib/tools/qlist.cpp

/*!
  \class tst_QVector
  \internal
  \since 4.5
  \brief Test Qt's class QList.
 */
class tst_QList : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void length() const;
    void lengthSignature() const;
    void append() const;
    void prepend() const;
    void mid() const;
    void at() const;
    void first() const;
    void last() const;
    void begin() const;
    void end() const;
    void contains() const;
    void count() const;
    void empty() const;
    void endsWith() const;
    void lastIndexOf() const;
    void move() const;
    void removeAll() const;
    void removeAt() const;
    void removeOne() const;
    void replace() const;
    void startsWith() const;
    void swap() const;
    void takeAt() const;
    void takeFirst() const;
    void takeLast() const;
    void toSet() const;
    void toStdList() const;
    void toVector() const;
    void value() const;

    void testSTLIterators() const;
    void testOperators() const;

    void initializeList() const;

    void const_shared_null() const;
    void setSharable1_data() const;
    void setSharable1() const;
    void setSharable2_data() const;
    void setSharable2() const;
    void eraseValidIteratorsOnSharedList() const;
    void insertWithValidIteratorsOnSharedList() const;

    void reserve() const;

private:
    int dummyForGuard;
};

struct Complex
{
    Complex(int val)
        : value(val)
        , checkSum(this)
    {
        ++liveCount;
    }

    Complex(Complex const &other)
        : value(other.value)
        , checkSum(this)
    {
        ++liveCount;
    }

    Complex &operator=(Complex const &other)
    {
        check(); other.check();

        value = other.value;
        return *this;
    }

    ~Complex()
    {
        --liveCount;
        check();
    }

    operator int() const { return value; }

    bool operator==(Complex const &other) const
    {
        check(); other.check();
        return value == other.value;
    }

    bool check() const
    {
        if (this != checkSum) {
            ++errorCount;
            return false;
        }
        return true;
    }

    struct Guard
    {
        Guard() : initialLiveCount(liveCount) {}
        ~Guard() { if (liveCount != initialLiveCount) ++errorCount; }

    private:
        Q_DISABLE_COPY(Guard)
        int initialLiveCount;
    };

    static void resetErrors() { errorCount = 0; }
    static int errors() { return errorCount; }

private:
    static int errorCount;
    static int liveCount;

    int value;
    void *checkSum;
};

int Complex::errorCount = 0;
int Complex::liveCount = 0;

void tst_QList::init()
{
    Complex::resetErrors();
    new (&dummyForGuard) Complex::Guard();
}

void tst_QList::cleanup()
{
    QCOMPARE(Complex::errors(), 0);

    reinterpret_cast<Complex::Guard *>(&dummyForGuard)->~Guard();
    QCOMPARE(Complex::errors(), 0);
}

void tst_QList::length() const
{
    /* Empty list. */
    {
        const QList<int> list;
        QCOMPARE(list.length(), 0);
    }

    /* One entry. */
    {
        QList<int> list;
        list.append(0);
        QCOMPARE(list.length(), 1);
    }

    /* Two entries. */
    {
        QList<int> list;
        list.append(0);
        list.append(1);
        QCOMPARE(list.length(), 2);
    }

    /* Three entries. */
    {
        QList<int> list;
        list.append(0);
        list.append(0);
        list.append(0);
        QCOMPARE(list.length(), 3);
    }
}

void tst_QList::lengthSignature() const
{
    /* Constness. */
    {
        const QList<int> list;
        /* The function should be const. */
        list.length();
    }
}

void tst_QList::append() const
{
    /* test append(const QList<T> &) function */
    QString one("one");
    QString two("two");
    QString three("three");
    QString four("four");
    QList<QString> list1;
    QList<QString> list2;
    QList<QString> listTotal;
    list1.append(one);
    list1.append(two);
    list2.append(three);
    list2.append(four);
    list1.append(list2);
    qDebug() << list1;
    listTotal.append(one);
    listTotal.append(two);
    listTotal.append(three);
    listTotal.append(four);
    QCOMPARE(list1, listTotal);

}

void tst_QList::prepend() const
{
    QList<QString *> list;
    QString *str1 = new QString;
    list.prepend(str1);
    QVERIFY(list.size() == 1);
    QVERIFY(list.at(0) == str1);
    QString *str2 = new QString;
    list.prepend(str2);
    QVERIFY(list.size() == 2);
    QVERIFY(list.at(0) == str2);
    QVERIFY(list.at(1) == str1);
    QString *str3 = new QString;
    list.prepend(str3);
    QVERIFY(list.size() == 3);
    QVERIFY(list.at(0) == str3);
    QVERIFY(list.at(1) == str2);
    QVERIFY(list.at(2) == str1);
    list.removeAll(str2);
    delete str2;
    QVERIFY(list.size() == 2);
    QVERIFY(list.at(0) == str3);
    QVERIFY(list.at(1) == str1);
    QString *str4 = new QString;
    list.prepend(str4);
    QVERIFY(list.size() == 3);
    QVERIFY(list.at(0) == str4);
    QVERIFY(list.at(1) == str3);
    QVERIFY(list.at(2) == str1);
    qDeleteAll(list);
    list.clear();
}

void tst_QList::mid() const
{
    QList<QString> list;
    list << "foo" << "bar" << "baz" << "bak" << "buck" << "hello" << "kitty";

    QCOMPARE(list.mid(3, 3),
             QList<QString>() << "bak" << "buck" << "hello");

    QList<int> list1;
    QCOMPARE(list1.mid(1, 1).length(), 0);
}

void tst_QList::at() const
{
    // test at() and make sure it functions correctly with some simple list manipulation.
    QList<QString> list;

    // create a list
    list << "foo" << "bar" << "baz";
    QVERIFY(list.size() == 3);
    QCOMPARE(list.at(0), QLatin1String("foo"));
    QCOMPARE(list.at(1), QLatin1String("bar"));
    QCOMPARE(list.at(2), QLatin1String("baz"));

    // append an item
    list << "hello";
    QVERIFY(list.size() == 4);
    QCOMPARE(list.at(0), QLatin1String("foo"));
    QCOMPARE(list.at(1), QLatin1String("bar"));
    QCOMPARE(list.at(2), QLatin1String("baz"));
    QCOMPARE(list.at(3), QLatin1String("hello"));

    // remove an item
    list.removeAt(1);
    QVERIFY(list.size() == 3);
    QCOMPARE(list.at(0), QLatin1String("foo"));
    QCOMPARE(list.at(1), QLatin1String("baz"));
    QCOMPARE(list.at(2), QLatin1String("hello"));
}

void tst_QList::first() const
{
    QList<QString> list;
    list << "foo" << "bar";

    QCOMPARE(list.first(), QLatin1String("foo"));

    // remove an item, make sure it still works
    list.pop_front();
    QVERIFY(list.size() == 1);
    QCOMPARE(list.first(), QLatin1String("bar"));
}

void tst_QList::last() const
{
    QList<QString> list;
    list << "foo" << "bar";

    QCOMPARE(list.last(), QLatin1String("bar"));

    // remove an item, make sure it still works
    list.pop_back();
    QVERIFY(list.size() == 1);
    QCOMPARE(list.last(), QLatin1String("foo"));
}

void tst_QList::begin() const
{
    QList<QString> list;
    list << "foo" << "bar";

    QCOMPARE(*list.begin(), QLatin1String("foo"));

    // remove an item, make sure it still works
    list.pop_front();
    QVERIFY(list.size() == 1);
    QCOMPARE(*list.begin(), QLatin1String("bar"));
}

void tst_QList::end() const
{
    QList<QString> list;
    list << "foo" << "bar";

    QCOMPARE(*--list.end(), QLatin1String("bar"));

    // remove an item, make sure it still works
    list.pop_back();
    QVERIFY(list.size() == 1);
    QCOMPARE(*--list.end(), QLatin1String("foo"));
}

void tst_QList::contains() const
{
    QList<QString> list;
    list << "foo" << "bar" << "baz";

    QVERIFY(list.contains(QLatin1String("foo")) == true);
    QVERIFY(list.contains(QLatin1String("pirates")) != true);

    // add it and make sure it matches
    list.append(QLatin1String("ninjas"));
    QVERIFY(list.contains(QLatin1String("ninjas")) == true);
}

void tst_QList::count() const
{
    QList<QString> list;

    // starts empty
    QVERIFY(list.count() == 0);

    // goes up
    list.append(QLatin1String("foo"));
    QVERIFY(list.count() == 1);

    // and up
    list.append(QLatin1String("bar"));
    QVERIFY(list.count() == 2);

    // and down
    list.pop_back();
    QVERIFY(list.count() == 1);

    // and empty. :)
    list.pop_back();
    QVERIFY(list.count() == 0);
}

void tst_QList::empty() const
{
    QList<QString> list;

    // make sure it starts empty
    QVERIFY(list.empty());

    // and doesn't stay empty
    list.append(QLatin1String("foo"));
    QVERIFY(!list.empty());

    // and goes back to being empty
    list.pop_back();
    QVERIFY(list.empty());
}

void tst_QList::endsWith() const
{
    QList<QString> list;
    list << "foo" << "bar" << "baz";

    // test it returns correctly in both cases
    QVERIFY(list.endsWith(QLatin1String("baz")));
    QVERIFY(!list.endsWith(QLatin1String("bar")));

    // remove an item and make sure the end item changes
    list.pop_back();
    QVERIFY(list.endsWith(QLatin1String("bar")));
}

void tst_QList::lastIndexOf() const
{
    QList<QString> list;
    list << "foo" << "bar" << "baz";

    // one instance of the target item
    QVERIFY(list.lastIndexOf(QLatin1String("baz")) == 2);

    // shouldn't find this
    QVERIFY(list.lastIndexOf(QLatin1String("shouldntfindme")) == -1);

    // multiple instances
    list.append("baz");
    list.append("baz");
    QVERIFY(list.lastIndexOf(QLatin1String("baz")) == 4);

    // search from the middle to find the last one
    QVERIFY(list.lastIndexOf(QLatin1String("baz"), 3) == 3);

    // try find none
    QVERIFY(list.lastIndexOf(QLatin1String("baz"), 1) == -1);
}

void tst_QList::move() const
{
    QList<QString> list;
    list << "foo" << "bar" << "baz";

    // move an item
    list.move(0, list.count() - 1);
    QCOMPARE(list, QList<QString>() << "bar" << "baz" << "foo");

    // move it back
    list.move(list.count() - 1, 0);
    QCOMPARE(list, QList<QString>() << "foo" << "bar" << "baz");

    // move an item in the middle
    list.move(1, 0);
    QCOMPARE(list, QList<QString>() << "bar" << "foo" << "baz");
}

void tst_QList::removeAll() const
{
    QList<QString> list;
    list << "foo" << "bar" << "baz";

    // remove one instance
    list.removeAll(QLatin1String("bar"));
    QCOMPARE(list, QList<QString>() << "foo" << "baz");

    // many instances
    list << "foo" << "bar" << "baz";
    list << "foo" << "bar" << "baz";
    list << "foo" << "bar" << "baz";
    list.removeAll(QLatin1String("bar"));
    QCOMPARE(list, QList<QString>() << "foo" << "baz" << "foo" << "baz" << "foo" << "baz" << "foo" << "baz");

    // try remove something that doesn't exist
    list.removeAll(QLatin1String("you won't remove anything I hope"));
    QCOMPARE(list, QList<QString>() << "foo" << "baz" << "foo" << "baz" << "foo" << "baz" << "foo" << "baz");
}

void tst_QList::removeAt() const
{
    QList<QString> list;
    list << "foo" << "bar" << "baz";

    // middle
    list.removeAt(1);
    QCOMPARE(list, QList<QString>() << "foo" << "baz");

    // start
    list.removeAt(0);
    QCOMPARE(list, QList<QString>() << "baz");

    // final
    list.removeAt(0);
    QCOMPARE(list, QList<QString>());
}

void tst_QList::removeOne() const
{
    QList<QString> list;
    list << "foo" << "bar" << "baz";

    // middle
    list.removeOne(QLatin1String("bar"));
    QCOMPARE(list, QList<QString>() << "foo" << "baz");

    // start
    list.removeOne(QLatin1String("foo"));
    QCOMPARE(list, QList<QString>() << "baz");

    // last
    list.removeOne(QLatin1String("baz"));
    QCOMPARE(list, QList<QString>());

    // make sure it really only removes one :)
    list << "foo" << "foo";
    list.removeOne("foo");
    QCOMPARE(list, QList<QString>() << "foo");

    // try remove something that doesn't exist
    list.removeOne(QLatin1String("you won't remove anything I hope"));
    QCOMPARE(list, QList<QString>() << "foo");
}

void tst_QList::replace() const
{
    QList<QString> list;
    list << "foo" << "bar" << "baz";

    // start
    list.replace(0, "moo");
    QCOMPARE(list, QList<QString>() << "moo" << "bar" << "baz");

    // middle
    list.replace(1, "cow");
    QCOMPARE(list, QList<QString>() << "moo" << "cow" << "baz");

    // end
    list.replace(2, "milk");
    QCOMPARE(list, QList<QString>() << "moo" << "cow" << "milk");
}

void tst_QList::startsWith() const
{
    QList<QString> list;
    list << "foo" << "bar" << "baz";

    // make sure it starts ok
    QVERIFY(list.startsWith(QLatin1String("foo")));

    // remove an item
    list.removeFirst();
    QVERIFY(list.startsWith(QLatin1String("bar")));
}

void tst_QList::swap() const
{
    QList<QString> list;
    list << "foo" << "bar" << "baz";

    // swap
    list.swap(0, 2);
    QCOMPARE(list, QList<QString>() << "baz" << "bar" << "foo");

    // swap again
    list.swap(1, 2);
    QCOMPARE(list, QList<QString>() << "baz" << "foo" << "bar");

    QList<QString> list2;
    list2 << "alpha" << "beta";

    list.swap(list2);
    QCOMPARE(list,  QList<QString>() << "alpha" << "beta");
    QCOMPARE(list2, QList<QString>() << "baz" << "foo" << "bar");
}

void tst_QList::takeAt() const
{
    QList<QString> list;
    list << "foo" << "bar" << "baz";

    QCOMPARE(list.takeAt(0), QLatin1String("foo"));
    QVERIFY(list.size() == 2);
    QCOMPARE(list.takeAt(1), QLatin1String("baz"));
    QVERIFY(list.size() == 1);
    QCOMPARE(list.takeAt(0), QLatin1String("bar"));
    QVERIFY(list.size() == 0);
}

void tst_QList::takeFirst() const
{
    QList<QString> list;
    list << "foo" << "bar" << "baz";

    QCOMPARE(list.takeFirst(), QLatin1String("foo"));
    QVERIFY(list.size() == 2);
    QCOMPARE(list.takeFirst(), QLatin1String("bar"));
    QVERIFY(list.size() == 1);
    QCOMPARE(list.takeFirst(), QLatin1String("baz"));
    QVERIFY(list.size() == 0);
}

void tst_QList::takeLast() const
{
    QList<QString> list;
    list << "foo" << "bar" << "baz";

    QCOMPARE(list.takeLast(), QLatin1String("baz"));
    QCOMPARE(list.takeLast(), QLatin1String("bar"));
    QCOMPARE(list.takeLast(), QLatin1String("foo"));
}

void tst_QList::toSet() const
{
    QList<QString> list;
    list << "foo" << "bar" << "baz";

    // no duplicates
    QCOMPARE(list.toSet(), QSet<QString>() << "foo" << "bar" << "baz");
    QCOMPARE(list, QList<QString>() << "foo" << "bar" << "baz");

    // duplicates (is this more of a QSet test?)
    list << "foo" << "bar" << "baz";
    QCOMPARE(list.toSet(), QSet<QString>() << "foo" << "bar" << "baz");
    QCOMPARE(list, QList<QString>() << "foo" << "bar" << "baz" << "foo" << "bar" << "baz");
}

void tst_QList::toStdList() const
{
    QList<QString> list;
    list << "foo" << "bar" << "baz";

    // yuck.
    std::list<QString> slist;
    slist.push_back(QLatin1String("foo"));
    slist.push_back(QLatin1String("bar"));
    slist.push_back(QLatin1String("baz"));

    QCOMPARE(list.toStdList(), slist);
    QCOMPARE(list, QList<QString>() << "foo" << "bar" << "baz");
}

void tst_QList::toVector() const
{
    QList<QString> list;
    list << "foo" << "bar" << "baz";

    QCOMPARE(list.toVector(), QVector<QString>() << "foo" << "bar" << "baz");
}

void tst_QList::value() const
{
    QList<QString> list;
    list << "foo" << "bar" << "baz";

    // test real values
    QCOMPARE(list.value(0), QLatin1String("foo"));
    QCOMPARE(list.value(2), QLatin1String("baz"));

    // test empty default
    QCOMPARE(list.value(3), QString());
    QCOMPARE(list.value(-1), QString());

    // test defaults
    QLatin1String defaultstr("default");
    QCOMPARE(list.value(-1, defaultstr), defaultstr);
    QCOMPARE(list.value(3, defaultstr), defaultstr);
}

void tst_QList::testOperators() const
{
    QList<QString> list;
    list << "foo" << "bar" << "baz";

    QList<QString> listtwo;
    listtwo << "foo" << "bar" << "baz";

    // test equal
    QVERIFY(list == listtwo);

    // not equal
    listtwo.append("not equal");
    QVERIFY(list != listtwo);

    // +=
    list += listtwo;
    QVERIFY(list.size() == 7);
    QVERIFY(listtwo.size() == 4);
    QCOMPARE(list, QList<QString>() << "foo" << "bar" << "baz" << "foo" << "bar" << "baz" << "not equal");

    // =
    list = listtwo;
    QCOMPARE(list, listtwo);
    QCOMPARE(list, QList<QString>() << "foo" << "bar" << "baz" << "not equal");

    // []
    QCOMPARE(list[0], QLatin1String("foo"));
    QCOMPARE(list[list.size() - 1], QLatin1String("not equal"));
}

void tst_QList::testSTLIterators() const
{
    QList<QString> list;

    // create a list
    list << "foo" << "bar" << "baz";
    QList<QString>::iterator it = list.begin();
    QCOMPARE(*it, QLatin1String("foo")); it++;
    QCOMPARE(*it, QLatin1String("bar")); it++;
    QCOMPARE(*it, QLatin1String("baz")); it++;
    QCOMPARE(it, list.end()); it--;

    // walk backwards
    QCOMPARE(*it, QLatin1String("baz")); it--;
    QCOMPARE(*it, QLatin1String("bar")); it--;
    QCOMPARE(*it, QLatin1String("foo"));

    // test erase
    it = list.erase(it);
    QVERIFY(list.size() == 2);
    QCOMPARE(*it, QLatin1String("bar"));

    // test multiple erase
    it = list.erase(it, it + 2);
    QVERIFY(list.size() == 0);
    QCOMPARE(it, list.end());

    // insert again
    it = list.insert(it, QLatin1String("foo"));
    QVERIFY(list.size() == 1);
    QCOMPARE(*it, QLatin1String("foo"));

    // insert again
    it = list.insert(it, QLatin1String("bar"));
    QVERIFY(list.size() == 2);
    QCOMPARE(*it++, QLatin1String("bar"));
    QCOMPARE(*it, QLatin1String("foo"));
}

void tst_QList::initializeList() const
{
#ifdef Q_COMPILER_INITIALIZER_LISTS
    QList<int> v1{2,3,4};
    QCOMPARE(v1, QList<int>() << 2 << 3 << 4);
    QCOMPARE(v1, (QList<int>{2,3,4}));

    QList<QList<int>> v2{ v1, {1}, QList<int>(), {2,3,4}  };
    QList<QList<int>> v3;
    v3 << v1 << (QList<int>() << 1) << QList<int>() << v1;
    QCOMPARE(v3, v2);
#endif
}

void tst_QList::const_shared_null() const
{
    QList<int> list1;
    list1.setSharable(false);
    QVERIFY(list1.isDetached());

    QList<int> list2;
    list2.setSharable(true);
    QVERIFY(!list2.isDetached());
}

Q_DECLARE_METATYPE(QList<int>);
Q_DECLARE_METATYPE(QList<Complex>);

template <class T>
void generateSetSharableData()
{
    QTest::addColumn<QList<T> >("list");
    QTest::addColumn<int>("size");

    QTest::newRow("null") << QList<T>() << 0;
    QTest::newRow("non-empty") << (QList<T>() << T(0) << T(1) << T(2) << T(3) << T(4)) << 5;
}

template <class T>
void runSetSharableTest()
{
    QFETCH(QList<T>, list);
    QFETCH(int, size);

    QVERIFY(!list.isDetached()); // Shared with QTest

    list.setSharable(true);

    QCOMPARE(list.size(), size);

    {
        QList<T> copy(list);
        QVERIFY(!copy.isDetached());
        QVERIFY(copy.isSharedWith(list));
    }

    list.setSharable(false);
    QVERIFY(list.isDetached() || list.isSharedWith(QList<T>()));

    {
        QList<T> copy(list);

        QVERIFY(copy.isDetached() || copy.isSharedWith(QList<T>()));
        QCOMPARE(copy.size(), size);
        QCOMPARE(copy, list);
    }

    list.setSharable(true);

    {
        QList<T> copy(list);

        QVERIFY(!copy.isDetached());
        QVERIFY(copy.isSharedWith(list));
    }

    for (int i = 0; i < list.size(); ++i)
        QCOMPARE(int(list[i]), i);

    QCOMPARE(list.size(), size);
}

void tst_QList::setSharable1_data() const
{
    generateSetSharableData<int>();
}

void tst_QList::setSharable2_data() const
{
    generateSetSharableData<Complex>();
}

void tst_QList::setSharable1() const
{
    runSetSharableTest<int>();
}

void tst_QList::setSharable2() const
{
    runSetSharableTest<Complex>();
}

void tst_QList::eraseValidIteratorsOnSharedList() const
{
    QList<int> a, b;
    a.push_back(10);
    a.push_back(20);
    a.push_back(30);
    QList<int>::iterator i = a.begin();
    ++i;
    b = a;
    a.erase(i);
    QCOMPARE(b.size(), 3);
    QCOMPARE(a.size(), 2);
    QCOMPARE(a.at(0), 10);
    QCOMPARE(a.at(1), 30);

    a.push_back(40);
    a.push_back(50);
    a.push_back(60);
    QCOMPARE(a.size(), 5);
    i = a.begin();
    b = a;
    ++i;
    QList<int>::iterator j = i;
    ++j;
    ++j;
    a.erase(i, j); // remove 3 elements
    QCOMPARE(b.size(), 5);
    QCOMPARE(a.size(), 3);
    QCOMPARE(a.at(0), 10);
    QCOMPARE(a.at(1), 50);
}

void tst_QList::insertWithValidIteratorsOnSharedList() const
{
    QList<int> a, b;
    a.push_back(10);
    a.push_back(20);
    a.push_back(30);
    QList<int>::iterator i = a.begin();
    ++i;
    b = a;
    a.insert(i, 15);
    QCOMPARE(a.size(), b.size() + 1);
    QCOMPARE(b.at(1), 20);
    QCOMPARE(a.at(1), 15);
}

void tst_QList::reserve() const
{
    // Note:
    // This test depends on QList's current behavior that ints are stored in the array itself.
    // This test would not work for QList<Complex>.
    int capacity = 100;
    QList<int> list;
    list.reserve(capacity);
    list << 0;
    int *data = &list[0];

    for (int i = 1; i < capacity; i++) {
        list << i;
        QCOMPARE(&list.at(0), data);
    }

    QList<int> copy = list;
    list.reserve(capacity / 2);
    QCOMPARE(list.size(), capacity); // we didn't shrink the size!

    copy = list;
    list.reserve(capacity * 2);
    QCOMPARE(list.size(), capacity);
    QVERIFY(&list.at(0) != data);
}

QTEST_APPLESS_MAIN(tst_QList)
#include "tst_qlist.moc"

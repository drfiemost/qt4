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


#define private public
#include <qtextdocument.h>
#include <qdebug.h>
#ifndef Q_WS_WIN
#include <private/qtextdocument_p.h>
#endif



#include <qtextobject.h>
#include <qtextcursor.h>


//TESTED_FILES=

QT_FORWARD_DECLARE_CLASS(QTextDocument)

class tst_QTextBlock : public QObject
{
    Q_OBJECT

public:
    tst_QTextBlock();


public slots:
    void init();
    void cleanup();
private slots:
    void fragmentOverBlockBoundaries();
    void excludeParagraphSeparatorFragment();
    void backwardsBlockIterator();
    void previousBlock_qtbug18026();

private:
    QTextDocument *doc;
    QTextCursor cursor;
};

tst_QTextBlock::tst_QTextBlock()
{}

void tst_QTextBlock::init()
{
    doc = new QTextDocument;
    cursor = QTextCursor(doc);
}

void tst_QTextBlock::cleanup()
{
    cursor = QTextCursor();
    delete doc;
    doc = 0;
}

void tst_QTextBlock::fragmentOverBlockBoundaries()
{
    /* this creates two fragments in the piecetable:
     * 1) 'hello<parag separator here>world'
     * 2) '<parag separator>'
     * (they are not united because the former was interested after the latter,
     * hence their position in the pt buffer is the other way around)
     */
    cursor.insertText("Hello");
    cursor.insertBlock();
    cursor.insertText("World");

    cursor.movePosition(QTextCursor::Start);

    const QTextDocument *doc = cursor.block().document();
    QVERIFY(doc);
    // Block separators are always a fragment of their self. Thus:
    // |Hello|\b|World|\b|
#if !defined(Q_WS_WIN)
    QVERIFY(doc->d_func()->fragmentMap().numNodes() == 4);
#endif

    QCOMPARE(cursor.block().text(), QString("Hello"));
    cursor.movePosition(QTextCursor::NextBlock);
    QCOMPARE(cursor.block().text(), QString("World"));
}

void tst_QTextBlock::excludeParagraphSeparatorFragment()
{
    QTextCharFormat fmt;
    fmt.setForeground(Qt::blue);
    cursor.insertText("Hello", fmt);

    QTextBlock block = doc->begin();
    QVERIFY(block.isValid());

    QTextBlock::Iterator it = block.begin();

    QTextFragment fragment = it.fragment();
    QVERIFY(fragment.isValid());
    QCOMPARE(fragment.text(), QString("Hello"));

    ++it;
    QVERIFY(it.atEnd());
    QVERIFY(it == block.end());
}

void tst_QTextBlock::backwardsBlockIterator()
{
    QTextCharFormat fmt;

    fmt.setForeground(Qt::magenta);
    cursor.insertText("A", fmt);

    fmt.setForeground(Qt::red);
    cursor.insertText("A", fmt);

    fmt.setForeground(Qt::magenta);
    cursor.insertText("A", fmt);

    QTextBlock block = doc->begin();
    QVERIFY(block.isValid());

    QTextBlock::Iterator it = block.begin();
    QCOMPARE(it.fragment().position(), 0);
    ++it;
    QCOMPARE(it.fragment().position(), 1);
    ++it;

    QCOMPARE(it.fragment().position(), 2);

    --it;
    QCOMPARE(it.fragment().position(), 1);
    --it;
    QCOMPARE(it.fragment().position(), 0);
}

void tst_QTextBlock::previousBlock_qtbug18026()
{
    QTextBlock last = doc->end().previous();
    QVERIFY(last.isValid());
}

QTEST_MAIN(tst_QTextBlock)
#include "tst_qtextblock.moc"

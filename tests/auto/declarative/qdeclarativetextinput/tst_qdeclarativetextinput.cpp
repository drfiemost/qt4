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
#include <qtest.h>
#include <QtTest/QSignalSpy>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QFile>
#include <QtDeclarative/qdeclarativeview.h>
#include <private/qdeclarativetextinput_p.h>
#include <private/qdeclarativetextinput_p_p.h>
#include <QDebug>
#include <QDir>
#include <QStyle>
#include <QInputContext>
#include <private/qapplication_p.h>

#include "qplatformdefs.h"

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

Q_DECLARE_METATYPE(QDeclarativeTextInput::SelectionMode)

QString createExpectedFileIfNotFound(const QString& filebasename, const QImage& actual)
{
    // XXX This will be replaced by some clever persistent platform image store.
    QString persistent_dir = SRCDIR "/data";
    QString arch = "unknown-architecture"; // QTest needs to help with this.

    QString expectfile = persistent_dir + QDir::separator() + filebasename + "-" + arch + ".png";

    if (!QFile::exists(expectfile)) {
        actual.save(expectfile);
        qWarning() << "created" << expectfile;
    }

    return expectfile;
}

class tst_qdeclarativetextinput : public QObject

{
    Q_OBJECT
public:
    tst_qdeclarativetextinput();

private slots:

    void text();
    void width();
    void font();
    void color();
    void selection();
    void isRightToLeft_data();
    void isRightToLeft();
    void moveCursorSelection_data();
    void moveCursorSelection();
    void moveCursorSelectionSequence_data();
    void moveCursorSelectionSequence();
    void mouseSelection_data();
    void mouseSelection();
    void deferEnableSelectByMouse_data();
    void deferEnableSelectByMouse();
    void deferDisableSelectByMouse_data();
    void deferDisableSelectByMouse();
    void dragMouseSelection();
    void mouseSelectionMode_data();
    void mouseSelectionMode();

    void horizontalAlignment_data();
    void horizontalAlignment();
    void horizontalAlignment_RightToLeft();

    void positionAt();

    void maxLength();
    void masks();
    void validators();
#ifndef QT_NO_IM
    void inputMethods();
#endif
    void passwordCharacter();
    void cursorDelegate();
    void cursorVisible();
    void cursorRectangle();
    void navigation();
    void navigation_RTL();
    void copyAndPaste();
    void canPasteEmpty();
    void canPaste();
    void readOnly();

#ifndef QT_NO_IM
    void openInputPanelOnClick();
    void openInputPanelOnFocus();
#endif
    void setHAlignClearCache();
    void focusOutClearSelection();

    void echoMode();
#ifdef QT_GUI_PASSWORD_ECHO_DELAY
    void passwordEchoDelay();
#endif
    void geometrySignals();
    void testQtQuick11Attributes();
    void testQtQuick11Attributes_data();

#ifndef QT_NO_IM
    void preeditAutoScroll();
    void preeditMicroFocus();
    void inputContextMouseHandler();
    void inputMethodComposing();
#endif
    void cursorRectangleSize();
    void deselect();

private:
    void simulateKey(QDeclarativeView *, int key);
    QDeclarativeView *createView(const QString &filename);

    QDeclarativeEngine engine;
    QStringList standard;
    QStringList colorStrings;
};

tst_qdeclarativetextinput::tst_qdeclarativetextinput()
{
    standard << "the quick brown fox jumped over the lazy dog"
        << "It's supercalifragisiticexpialidocious!"
        << "Hello, world!"
        << "!dlrow ,olleH"
        << " spacey   text ";

    colorStrings << "aliceblue"
                 << "antiquewhite"
                 << "aqua"
                 << "darkkhaki"
                 << "darkolivegreen"
                 << "dimgray"
                 << "palevioletred"
                 << "lightsteelblue"
                 << "#000000"
                 << "#AAAAAA"
                 << "#FFFFFF"
                 << "#2AC05F";
}

void tst_qdeclarativetextinput::text()
{
    {
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData("import QtQuick 1.0\nTextInput {  text: \"\"  }", QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->text(), QString(""));

        delete textinputObject;
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QString componentStr = "import QtQuick 1.0\nTextInput { text: \"" + standard.at(i) + "\" }";
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->text(), standard.at(i));

        delete textinputObject;
    }

}

void tst_qdeclarativetextinput::width()
{
    // uses Font metrics to find the width for standard
    {
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData("import QtQuick 1.0\nTextInput {  text: \"\" }", QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->width(), 0.0);

        delete textinputObject;
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QFont f;
        QFontMetricsF fm(f);
        qreal metricWidth = fm.width(standard.at(i));

        QString componentStr = "import QtQuick 1.0\nTextInput { text: \"" + standard.at(i) + "\" }";
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        int delta = abs(int(int(textinputObject->width()) - metricWidth));
        QVERIFY(delta <= 3.0); // As best as we can hope for cross-platform.

        delete textinputObject;
    }
}

void tst_qdeclarativetextinput::font()
{
    //test size, then bold, then italic, then family
    { 
        QString componentStr = "import QtQuick 1.0\nTextInput {  font.pointSize: 40; text: \"Hello World\" }";
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->font().pointSize(), 40);
        QCOMPARE(textinputObject->font().bold(), false);
        QCOMPARE(textinputObject->font().italic(), false);

        delete textinputObject;
    }

    { 
        QString componentStr = "import QtQuick 1.0\nTextInput {  font.bold: true; text: \"Hello World\" }";
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->font().bold(), true);
        QCOMPARE(textinputObject->font().italic(), false);

        delete textinputObject;
    }

    { 
        QString componentStr = "import QtQuick 1.0\nTextInput {  font.italic: true; text: \"Hello World\" }";
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->font().italic(), true);
        QCOMPARE(textinputObject->font().bold(), false);

        delete textinputObject;
    }
 
    { 
        QString componentStr = "import QtQuick 1.0\nTextInput {  font.family: \"Helvetica\"; text: \"Hello World\" }";
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->font().family(), QString("Helvetica"));
        QCOMPARE(textinputObject->font().bold(), false);
        QCOMPARE(textinputObject->font().italic(), false);

        delete textinputObject;
    }

    { 
        QString componentStr = "import QtQuick 1.0\nTextInput {  font.family: \"\"; text: \"Hello World\" }";
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->font().family(), QString(""));

        delete textinputObject;
    }
}

void tst_qdeclarativetextinput::color()
{
    //test color
    for (int i = 0; i < colorStrings.size(); i++)
    { 
        QString componentStr = "import QtQuick 1.0\nTextInput {  color: \"" + colorStrings.at(i) + "\"; text: \"Hello World\" }";
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());
        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->color(), QColor(colorStrings.at(i)));

        delete textinputObject;
    }

    //test selection color
    for (int i = 0; i < colorStrings.size(); i++)
    {
        QString componentStr = "import QtQuick 1.0\nTextInput {  selectionColor: \"" + colorStrings.at(i) + "\"; text: \"Hello World\" }";
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());
        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->selectionColor(), QColor(colorStrings.at(i)));

        delete textinputObject;
    }

    //test selected text color
    for (int i = 0; i < colorStrings.size(); i++)
    { 
        QString componentStr = "import QtQuick 1.0\nTextInput {  selectedTextColor: \"" + colorStrings.at(i) + "\"; text: \"Hello World\" }";
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());
        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->selectedTextColor(), QColor(colorStrings.at(i)));

        delete textinputObject;
    }

    {
        QString colorStr = "#AA001234";
        QColor testColor("#001234");
        testColor.setAlpha(170);

        QString componentStr = "import QtQuick 1.0\nTextInput {  color: \"" + colorStr + "\"; text: \"Hello World\" }";
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->color(), testColor);

        delete textinputObject;
    }
}

void tst_qdeclarativetextinput::selection()
{
    QString testStr = standard[0];
    QString componentStr = "import QtQuick 1.0\nTextInput {  text: \""+ testStr +"\"; }";
    QDeclarativeComponent textinputComponent(&engine);
    textinputComponent.setData(componentStr.toLatin1(), QUrl());
    QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());
    QVERIFY(textinputObject != 0);


    //Test selection follows cursor
    for(int i=0; i<= testStr.size(); i++) {
        textinputObject->setCursorPosition(i);
        QCOMPARE(textinputObject->cursorPosition(), i);
        QCOMPARE(textinputObject->selectionStart(), i);
        QCOMPARE(textinputObject->selectionEnd(), i);
        QVERIFY(textinputObject->selectedText().isNull());
    }
    //Test cursor follows selection
    for(int i=0; i<= testStr.size(); i++) {
        textinputObject->select(i,i);
        QCOMPARE(textinputObject->cursorPosition(), i);
        QCOMPARE(textinputObject->selectionStart(), i);
        QCOMPARE(textinputObject->selectionEnd(), i);
    }

    textinputObject->setCursorPosition(0);
    QVERIFY(textinputObject->cursorPosition() == 0);
    QVERIFY(textinputObject->selectionStart() == 0);
    QVERIFY(textinputObject->selectionEnd() == 0);
    QVERIFY(textinputObject->selectedText().isNull());

    // Verify invalid positions are ignored.
    textinputObject->setCursorPosition(-1);
    QVERIFY(textinputObject->cursorPosition() == 0);
    QVERIFY(textinputObject->selectionStart() == 0);
    QVERIFY(textinputObject->selectionEnd() == 0);
    QVERIFY(textinputObject->selectedText().isNull());

    textinputObject->setCursorPosition(textinputObject->text().count()+1);
    QVERIFY(textinputObject->cursorPosition() == 0);
    QVERIFY(textinputObject->selectionStart() == 0);
    QVERIFY(textinputObject->selectionEnd() == 0);
    QVERIFY(textinputObject->selectedText().isNull());

    //Test selection
    for(int i=0; i<= testStr.size(); i++) {
        textinputObject->select(0,i);
        QCOMPARE(testStr.mid(0,i), textinputObject->selectedText());
        QCOMPARE(textinputObject->cursorPosition(), i);
    }
    for(int i=0; i<= testStr.size(); i++) {
        textinputObject->select(i,testStr.size());
        QCOMPARE(testStr.mid(i,testStr.size()-i), textinputObject->selectedText());
        QCOMPARE(textinputObject->cursorPosition(), testStr.size());
    }

    textinputObject->setCursorPosition(0);
    QVERIFY(textinputObject->cursorPosition() == 0);
    QVERIFY(textinputObject->selectionStart() == 0);
    QVERIFY(textinputObject->selectionEnd() == 0);
    QVERIFY(textinputObject->selectedText().isNull());

    //Test Error Ignoring behaviour
    textinputObject->setCursorPosition(0);
    QVERIFY(textinputObject->selectedText().isNull());
    textinputObject->select(-10,0);
    QVERIFY(textinputObject->selectedText().isNull());
    textinputObject->select(100,110);
    QVERIFY(textinputObject->selectedText().isNull());
    textinputObject->select(0,-10);
    QVERIFY(textinputObject->selectedText().isNull());
    textinputObject->select(0,100);
    QVERIFY(textinputObject->selectedText().isNull());
    textinputObject->select(0,10);
    QVERIFY(textinputObject->selectedText().size() == 10);
    textinputObject->select(-10,10);
    QVERIFY(textinputObject->selectedText().size() == 10);
    textinputObject->select(100,101);
    QVERIFY(textinputObject->selectedText().size() == 10);
    textinputObject->select(0,-10);
    QVERIFY(textinputObject->selectedText().size() == 10);
    textinputObject->select(0,100);
    QVERIFY(textinputObject->selectedText().size() == 10);

    textinputObject->deselect();
    QVERIFY(textinputObject->selectedText().isNull());
    textinputObject->select(0,10);
    QVERIFY(textinputObject->selectedText().size() == 10);
    textinputObject->deselect();
    QVERIFY(textinputObject->selectedText().isNull());

    delete textinputObject;
}

void tst_qdeclarativetextinput::isRightToLeft_data()
{
    QTest::addColumn<QString>("text");
    QTest::addColumn<bool>("emptyString");
    QTest::addColumn<bool>("firstCharacter");
    QTest::addColumn<bool>("lastCharacter");
    QTest::addColumn<bool>("middleCharacter");
    QTest::addColumn<bool>("startString");
    QTest::addColumn<bool>("midString");
    QTest::addColumn<bool>("endString");

    const quint16 arabic_str[] = { 0x0638, 0x0643, 0x00646, 0x0647, 0x0633, 0x0638, 0x0643, 0x00646, 0x0647, 0x0633, 0x0647};
    QTest::newRow("Empty") << "" << false << false << false << false << false << false << false;
    QTest::newRow("Neutral") << "23244242" << false << false << false << false << false << false << false;
    QTest::newRow("LTR") << "Hello world" << false << false << false << false << false << false << false;
    QTest::newRow("RTL") << QString::fromUtf16(arabic_str, 11) << false << true << true << true << true << true << true;
    QTest::newRow("Bidi RTL + LTR + RTL") << QString::fromUtf16(arabic_str, 11) + QString("Hello world") + QString::fromUtf16(arabic_str, 11) << false << true << true << false << true << true << true;
    QTest::newRow("Bidi LTR + RTL + LTR") << QString("Hello world") + QString::fromUtf16(arabic_str, 11) + QString("Hello world") << false << false << false << true << false << false << false;
}

void tst_qdeclarativetextinput::isRightToLeft()
{
    QFETCH(QString, text);
    QFETCH(bool, emptyString);
    QFETCH(bool, firstCharacter);
    QFETCH(bool, lastCharacter);
    QFETCH(bool, middleCharacter);
    QFETCH(bool, startString);
    QFETCH(bool, midString);
    QFETCH(bool, endString);

    QDeclarativeTextInput textInput;
    textInput.setText(text);

    // first test that the right string is delivered to the QString::isRightToLeft()
    QCOMPARE(textInput.isRightToLeft(0,0), text.mid(0,0).isRightToLeft());
    QCOMPARE(textInput.isRightToLeft(0,1), text.mid(0,1).isRightToLeft());
    QCOMPARE(textInput.isRightToLeft(text.count()-2, text.count()-1), text.mid(text.count()-2, text.count()-1).isRightToLeft());
    QCOMPARE(textInput.isRightToLeft(text.count()/2, text.count()/2 + 1), text.mid(text.count()/2, text.count()/2 + 1).isRightToLeft());
    QCOMPARE(textInput.isRightToLeft(0,text.count()/4), text.mid(0,text.count()/4).isRightToLeft());
    QCOMPARE(textInput.isRightToLeft(text.count()/4,3*text.count()/4), text.mid(text.count()/4,3*text.count()/4).isRightToLeft());
    if (text.isEmpty())
        QTest::ignoreMessage(QtWarningMsg, "<Unknown File>: QML TextInput: isRightToLeft(start, end) called with the end property being smaller than the start.");
    QCOMPARE(textInput.isRightToLeft(3*text.count()/4,text.count()-1), text.mid(3*text.count()/4,text.count()-1).isRightToLeft());

    // then test that the feature actually works
    QCOMPARE(textInput.isRightToLeft(0,0), emptyString);
    QCOMPARE(textInput.isRightToLeft(0,1), firstCharacter);
    QCOMPARE(textInput.isRightToLeft(text.count()-2, text.count()-1), lastCharacter);
    QCOMPARE(textInput.isRightToLeft(text.count()/2, text.count()/2 + 1), middleCharacter);
    QCOMPARE(textInput.isRightToLeft(0,text.count()/4), startString);
    QCOMPARE(textInput.isRightToLeft(text.count()/4,3*text.count()/4), midString);
    if (text.isEmpty())
        QTest::ignoreMessage(QtWarningMsg, "<Unknown File>: QML TextInput: isRightToLeft(start, end) called with the end property being smaller than the start.");
    QCOMPARE(textInput.isRightToLeft(3*text.count()/4,text.count()-1), endString);
}

void tst_qdeclarativetextinput::moveCursorSelection_data()
{
    QTest::addColumn<QString>("testStr");
    QTest::addColumn<int>("cursorPosition");
    QTest::addColumn<int>("movePosition");
    QTest::addColumn<QDeclarativeTextInput::SelectionMode>("mode");
    QTest::addColumn<int>("selectionStart");
    QTest::addColumn<int>("selectionEnd");
    QTest::addColumn<bool>("reversible");

    // () contains the text selected by the cursor.
    // <> contains the actual selection.

    QTest::newRow("(t)he|characters")
            << standard[0] << 0 << 1 << QDeclarativeTextInput::SelectCharacters << 0 << 1 << true;
    QTest::newRow("do(g)|characters")
            << standard[0] << 43 << 44 << QDeclarativeTextInput::SelectCharacters << 43 << 44 << true;
    QTest::newRow("jum(p)ed|characters")
            << standard[0] << 23 << 24 << QDeclarativeTextInput::SelectCharacters << 23 << 24 << true;
    QTest::newRow("jumped( )over|characters")
            << standard[0] << 26 << 27 << QDeclarativeTextInput::SelectCharacters << 26 << 27 << true;
    QTest::newRow("(the )|characters")
            << standard[0] << 0 << 4 << QDeclarativeTextInput::SelectCharacters << 0 << 4 << true;
    QTest::newRow("( dog)|characters")
            << standard[0] << 40 << 44 << QDeclarativeTextInput::SelectCharacters << 40 << 44 << true;
    QTest::newRow("( jumped )|characters")
            << standard[0] << 19 << 27 << QDeclarativeTextInput::SelectCharacters << 19 << 27 << true;
    QTest::newRow("th(e qu)ick|characters")
            << standard[0] << 2 << 6 << QDeclarativeTextInput::SelectCharacters << 2 << 6 << true;
    QTest::newRow("la(zy d)og|characters")
            << standard[0] << 38 << 42 << QDeclarativeTextInput::SelectCharacters << 38 << 42 << true;
    QTest::newRow("jum(ped ov)er|characters")
            << standard[0] << 23 << 29 << QDeclarativeTextInput::SelectCharacters << 23 << 29 << true;
    QTest::newRow("()the|characters")
            << standard[0] << 0 << 0 << QDeclarativeTextInput::SelectCharacters << 0 << 0 << true;
    QTest::newRow("dog()|characters")
            << standard[0] << 44 << 44 << QDeclarativeTextInput::SelectCharacters << 44 << 44 << true;
    QTest::newRow("jum()ped|characters")
            << standard[0] << 23 << 23 << QDeclarativeTextInput::SelectCharacters << 23 << 23 << true;

    QTest::newRow("<(t)he>|words")
            << standard[0] << 0 << 1 << QDeclarativeTextInput::SelectWords << 0 << 3 << true;
    QTest::newRow("<do(g)>|words")
            << standard[0] << 43 << 44 << QDeclarativeTextInput::SelectWords << 41 << 44 << true;
    QTest::newRow("<jum(p)ed>|words")
            << standard[0] << 23 << 24 << QDeclarativeTextInput::SelectWords << 20 << 26 << true;
    QTest::newRow("<jumped( )>over|words,ltr")
            << standard[0] << 26 << 27 << QDeclarativeTextInput::SelectWords << 20 << 27 << false;
    QTest::newRow("jumped<( )over>|words,rtl")
            << standard[0] << 27 << 26 << QDeclarativeTextInput::SelectWords << 26 << 31 << false;
    QTest::newRow("<(the )>quick|words,ltr")
            << standard[0] << 0 << 4 << QDeclarativeTextInput::SelectWords << 0 << 4 << false;
    QTest::newRow("<(the )quick>|words,rtl")
            << standard[0] << 4 << 0 << QDeclarativeTextInput::SelectWords << 0 << 9 << false;
    QTest::newRow("<lazy( dog)>|words,ltr")
            << standard[0] << 40 << 44 << QDeclarativeTextInput::SelectWords << 36 << 44 << false;
    QTest::newRow("lazy<( dog)>|words,rtl")
            << standard[0] << 44 << 40 << QDeclarativeTextInput::SelectWords << 40 << 44 << false;
    QTest::newRow("<fox( jumped )>over|words,ltr")
            << standard[0] << 19 << 27 << QDeclarativeTextInput::SelectWords << 16 << 27 << false;
    QTest::newRow("fox<( jumped )over>|words,rtl")
            << standard[0] << 27 << 19 << QDeclarativeTextInput::SelectWords << 19 << 31 << false;
    QTest::newRow("<th(e qu)ick>|words")
            << standard[0] << 2 << 6 << QDeclarativeTextInput::SelectWords << 0 << 9 << true;
    QTest::newRow("<la(zy d)og|words>")
            << standard[0] << 38 << 42 << QDeclarativeTextInput::SelectWords << 36 << 44 << true;
    QTest::newRow("<jum(ped ov)er>|words")
            << standard[0] << 23 << 29 << QDeclarativeTextInput::SelectWords << 20 << 31 << true;
    QTest::newRow("<()>the|words")
            << standard[0] << 0 << 0 << QDeclarativeTextInput::SelectWords << 0 << 0 << true;
    QTest::newRow("dog<()>|words")
            << standard[0] << 44 << 44 << QDeclarativeTextInput::SelectWords << 44 << 44 << true;
    QTest::newRow("jum<()>ped|words")
            << standard[0] << 23 << 23 << QDeclarativeTextInput::SelectWords << 23 << 23 << true;

    QTest::newRow("Hello<(,)> |words")
            << standard[2] << 5 << 6 << QDeclarativeTextInput::SelectWords << 5 << 6 << true;
    QTest::newRow("Hello<(, )>world|words,ltr")
            << standard[2] << 5 << 7 << QDeclarativeTextInput::SelectWords << 5 << 7 << false;
    QTest::newRow("Hello<(, )world>|words,rtl")
            << standard[2] << 7 << 5 << QDeclarativeTextInput::SelectWords << 5 << 12 << false;
    QTest::newRow("<Hel(lo, )>world|words,ltr")
            << standard[2] << 3 << 7 << QDeclarativeTextInput::SelectWords << 0 << 7 << false;
    QTest::newRow("<Hel(lo, )world>|words,rtl")
            << standard[2] << 7 << 3 << QDeclarativeTextInput::SelectWords << 0 << 12 << false;
    QTest::newRow("<Hel(lo)>,|words")
            << standard[2] << 3 << 5 << QDeclarativeTextInput::SelectWords << 0 << 5 << true;
    QTest::newRow("Hello<()>,|words")
            << standard[2] << 5 << 5 << QDeclarativeTextInput::SelectWords << 5 << 5 << true;
    QTest::newRow("Hello,<()>|words")
            << standard[2] << 6 << 6 << QDeclarativeTextInput::SelectWords << 6 << 6 << true;
    QTest::newRow("Hello<,( )>world|words,ltr")
            << standard[2] << 6 << 7 << QDeclarativeTextInput::SelectWords << 5 << 7 << false;
    QTest::newRow("Hello,<( )world>|words,rtl")
            << standard[2] << 7 << 6 << QDeclarativeTextInput::SelectWords << 6 << 12 << false;
    QTest::newRow("Hello<,( world)>|words,ltr")
            << standard[2] << 6 << 12 << QDeclarativeTextInput::SelectWords << 5 << 12 << false;
    QTest::newRow("Hello,<( world)>|words,rtl")
            << standard[2] << 12 << 6 << QDeclarativeTextInput::SelectWords << 6 << 12 << false;
    QTest::newRow("Hello<,( world!)>|words,ltr")
            << standard[2] << 6 << 13 << QDeclarativeTextInput::SelectWords << 5 << 13 << false;
    QTest::newRow("Hello,<( world!)>|words,rtl")
            << standard[2] << 13 << 6 << QDeclarativeTextInput::SelectWords << 6 << 13 << false;
    QTest::newRow("Hello<(, world!)>|words")
            << standard[2] << 5 << 13 << QDeclarativeTextInput::SelectWords << 5 << 13 << true;
     QTest::newRow("world<(!)>|words")
             << standard[2] << 12 << 13 << QDeclarativeTextInput::SelectWords << 12 << 13 << true;
    QTest::newRow("world!<()>)|words")
            << standard[2] << 13 << 13 << QDeclarativeTextInput::SelectWords << 13 << 13 << true;
    QTest::newRow("world<()>!)|words")
            << standard[2] << 12 << 12 << QDeclarativeTextInput::SelectWords << 12 << 12 << true;

    QTest::newRow("<(,)>olleH |words")
            << standard[3] << 7 << 8 << QDeclarativeTextInput::SelectWords << 7 << 8 << true;
    QTest::newRow("<dlrow( ,)>olleH|words,ltr")
            << standard[3] << 6 << 8 << QDeclarativeTextInput::SelectWords << 1 << 8 << false;
    QTest::newRow("dlrow<( ,)>olleH|words,rtl")
            << standard[3] << 8 << 6 << QDeclarativeTextInput::SelectWords << 6 << 8 << false;
    QTest::newRow("<dlrow( ,ol)leH>|words,ltr")
            << standard[3] << 6 << 10 << QDeclarativeTextInput::SelectWords << 1 << 13 << false;
    QTest::newRow("dlrow<( ,ol)leH>|words,rtl")
            << standard[3] << 10 << 6 << QDeclarativeTextInput::SelectWords << 6 << 13 << false;
    QTest::newRow(",<(ol)leH>,|words")
            << standard[3] << 8 << 10 << QDeclarativeTextInput::SelectWords << 8 << 13 << true;
    QTest::newRow(",<()>olleH|words")
            << standard[3] << 8 << 8 << QDeclarativeTextInput::SelectWords << 8 << 8 << true;
    QTest::newRow("<()>,olleH|words")
            << standard[3] << 7 << 7 << QDeclarativeTextInput::SelectWords << 7 << 7 << true;
    QTest::newRow("<dlrow( )>,olleH|words,ltr")
            << standard[3] << 6 << 7 << QDeclarativeTextInput::SelectWords << 1 << 7 << false;
    QTest::newRow("dlrow<( ),>olleH|words,rtl")
            << standard[3] << 7 << 6 << QDeclarativeTextInput::SelectWords << 6 << 8 << false;
    QTest::newRow("<(dlrow )>,olleH|words,ltr")
            << standard[3] << 1 << 7 << QDeclarativeTextInput::SelectWords << 1 << 7 << false;
    QTest::newRow("<(dlrow ),>olleH|words,rtl")
            << standard[3] << 7 << 1 << QDeclarativeTextInput::SelectWords << 1 << 8 << false;
    QTest::newRow("<(!dlrow )>,olleH|words,ltr")
            << standard[3] << 0 << 7 << QDeclarativeTextInput::SelectWords << 0 << 7 << false;
    QTest::newRow("<(!dlrow ),>olleH|words,rtl")
            << standard[3] << 7 << 0 << QDeclarativeTextInput::SelectWords << 0 << 8 << false;
    QTest::newRow("(!dlrow ,)olleH|words")
            << standard[3] << 0 << 8 << QDeclarativeTextInput::SelectWords << 0 << 8 << true;
    QTest::newRow("<(!)>dlrow|words")
            << standard[3] << 0 << 1 << QDeclarativeTextInput::SelectWords << 0 << 1 << true;
    QTest::newRow("<()>!dlrow|words")
            << standard[3] << 0 << 0 << QDeclarativeTextInput::SelectWords << 0 << 0 << true;
    QTest::newRow("!<()>dlrow|words")
            << standard[3] << 1 << 1 << QDeclarativeTextInput::SelectWords << 1 << 1 << true;

    QTest::newRow(" <s(pac)ey>   text |words")
            << standard[4] << 1 << 4 << QDeclarativeTextInput::SelectWords << 1 << 7 << true;
    QTest::newRow(" spacey   <t(ex)t> |words")
            << standard[4] << 11 << 13 << QDeclarativeTextInput::SelectWords << 10 << 14 << true;
    QTest::newRow("<( )>spacey   text |words|ltr")
            << standard[4] << 0 << 1 << QDeclarativeTextInput::SelectWords << 0 << 1 << false;
    QTest::newRow("<( )spacey>   text |words|rtl")
            << standard[4] << 1 << 0 << QDeclarativeTextInput::SelectWords << 0 << 7 << false;
    QTest::newRow("spacey   <text( )>|words|ltr")
            << standard[4] << 14 << 15 << QDeclarativeTextInput::SelectWords << 10 << 15 << false;
    QTest::newRow("spacey   text<( )>|words|rtl")
            << standard[4] << 15 << 14 << QDeclarativeTextInput::SelectWords << 14 << 15 << false;
    QTest::newRow("<()> spacey   text |words")
            << standard[4] << 0 << 0 << QDeclarativeTextInput::SelectWords << 0 << 0 << false;
    QTest::newRow(" spacey   text <()>|words")
            << standard[4] << 15 << 15 << QDeclarativeTextInput::SelectWords << 15 << 15 << false;
}

void tst_qdeclarativetextinput::moveCursorSelection()
{
    QFETCH(QString, testStr);
    QFETCH(int, cursorPosition);
    QFETCH(int, movePosition);
    QFETCH(QDeclarativeTextInput::SelectionMode, mode);
    QFETCH(int, selectionStart);
    QFETCH(int, selectionEnd);
    QFETCH(bool, reversible);

    QString componentStr = "import QtQuick 1.1\nTextInput {  text: \""+ testStr +"\"; }";
    QDeclarativeComponent textinputComponent(&engine);
    textinputComponent.setData(componentStr.toLatin1(), QUrl());
    QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());
    QVERIFY(textinputObject != 0);

    textinputObject->setCursorPosition(cursorPosition);
    textinputObject->moveCursorSelection(movePosition, mode);

    QCOMPARE(textinputObject->selectedText(), testStr.mid(selectionStart, selectionEnd - selectionStart));
    QCOMPARE(textinputObject->selectionStart(), selectionStart);
    QCOMPARE(textinputObject->selectionEnd(), selectionEnd);

    if (reversible) {
        textinputObject->setCursorPosition(movePosition);
        textinputObject->moveCursorSelection(cursorPosition, mode);

        QCOMPARE(textinputObject->selectedText(), testStr.mid(selectionStart, selectionEnd - selectionStart));
        QCOMPARE(textinputObject->selectionStart(), selectionStart);
        QCOMPARE(textinputObject->selectionEnd(), selectionEnd);
    }
}

void tst_qdeclarativetextinput::moveCursorSelectionSequence_data()
{
    QTest::addColumn<QString>("testStr");
    QTest::addColumn<int>("cursorPosition");
    QTest::addColumn<int>("movePosition1");
    QTest::addColumn<int>("movePosition2");
    QTest::addColumn<int>("selection1Start");
    QTest::addColumn<int>("selection1End");
    QTest::addColumn<int>("selection2Start");
    QTest::addColumn<int>("selection2End");

    // () contains the text selected by the cursor.
    // <> contains the actual selection.
    // ^ is the revised cursor position.
    // {} contains the revised selection.

    QTest::newRow("the {<quick( bro)wn> f^ox} jumped|ltr")
            << standard[0]
            << 9 << 13 << 17
            << 4 << 15
            << 4 << 19;
    QTest::newRow("the quick<( {bro)wn> f^ox} jumped|rtl")
            << standard[0]
            << 13 << 9 << 17
            << 9 << 15
            << 10 << 19;
    QTest::newRow("the {<quick( bro)wn> ^}fox jumped|ltr")
            << standard[0]
            << 9 << 13 << 16
            << 4 << 15
            << 4 << 16;
    QTest::newRow("the quick<( {bro)wn> ^}fox jumped|rtl")
            << standard[0]
            << 13 << 9 << 16
            << 9 << 15
            << 10 << 16;
    QTest::newRow("the {<quick( bro)wn^>} fox jumped|ltr")
            << standard[0]
            << 9 << 13 << 15
            << 4 << 15
            << 4 << 15;
    QTest::newRow("the quick<( {bro)wn^>} f^ox jumped|rtl")
            << standard[0]
            << 13 << 9 << 15
            << 9 << 15
            << 10 << 15;
    QTest::newRow("the {<quick() ^}bro)wn> fox|ltr")
            << standard[0]
            << 9 << 13 << 10
            << 4 << 15
            << 4 << 10;
    QTest::newRow("the quick<( {^bro)wn>} fox|rtl")
            << standard[0]
            << 13 << 9 << 10
            << 9 << 15
            << 10 << 15;
    QTest::newRow("the {<quick^}( bro)wn> fox|ltr")
            << standard[0]
            << 9 << 13 << 9
            << 4 << 15
            << 4 << 9;
    QTest::newRow("the quick{<(^ bro)wn>} fox|rtl")
            << standard[0]
            << 13 << 9 << 9
            << 9 << 15
            << 9 << 15;
    QTest::newRow("the {<qui^ck}( bro)wn> fox|ltr")
            << standard[0]
            << 9 << 13 << 7
            << 4 << 15
            << 4 << 9;
    QTest::newRow("the {<qui^ck}( bro)wn> fox|rtl")
            << standard[0]
            << 13 << 9 << 7
            << 9 << 15
            << 4 << 15;
    QTest::newRow("the {<^quick}( bro)wn> fox|ltr")
            << standard[0]
            << 9 << 13 << 4
            << 4 << 15
            << 4 << 9;
    QTest::newRow("the {<^quick}( bro)wn> fox|rtl")
            << standard[0]
            << 13 << 9 << 4
            << 9 << 15
            << 4 << 15;
    QTest::newRow("the{^ <quick}( bro)wn> fox|ltr")
            << standard[0]
            << 9 << 13 << 3
            << 4 << 15
            << 3 << 9;
    QTest::newRow("the{^ <quick}( bro)wn> fox|rtl")
            << standard[0]
            << 13 << 9 << 3
            << 9 << 15
            << 3 << 15;
    QTest::newRow("{t^he <quick}( bro)wn> fox|ltr")
            << standard[0]
            << 9 << 13 << 1
            << 4 << 15
            << 0 << 9;
    QTest::newRow("{t^he <quick}( bro)wn> fox|rtl")
            << standard[0]
            << 13 << 9 << 1
            << 9 << 15
            << 0 << 15;

    QTest::newRow("{<He(ll)o>, w^orld}!|ltr")
            << standard[2]
            << 2 << 4 << 8
            << 0 << 5
            << 0 << 12;
    QTest::newRow("{<He(ll)o>, w^orld}!|rtl")
            << standard[2]
            << 4 << 2 << 8
            << 0 << 5
            << 0 << 12;

    QTest::newRow("!{dlro^w ,<o(ll)eH>}|ltr")
            << standard[3]
            << 9 << 11 << 5
            << 8 << 13
            << 1 << 13;
    QTest::newRow("!{dlro^w ,<o(ll)eH>}|rtl")
            << standard[3]
            << 11 << 9 << 5
            << 8 << 13
            << 1 << 13;

    QTest::newRow("{<(^} sp)acey>   text |ltr")
            << standard[4]
            << 0 << 3 << 0
            << 0 << 7
            << 0 << 0;
    QTest::newRow("{<( ^}sp)acey>   text |ltr")
            << standard[4]
            << 0 << 3 << 1
            << 0 << 7
            << 0 << 1;
    QTest::newRow("<( {s^p)acey>}   text |rtl")
            << standard[4]
            << 3 << 0 << 2
            << 0 << 7
            << 1 << 7;
    QTest::newRow("<( {^sp)acey>}   text |rtl")
            << standard[4]
            << 3 << 0 << 1
            << 0 << 7
            << 1 << 7;

    QTest::newRow(" spacey   <te(xt {^)>}|rtl")
            << standard[4]
            << 15 << 12 << 15
            << 10 << 15
            << 15 << 15;
    QTest::newRow(" spacey   <te(xt{^ )>}|rtl")
            << standard[4]
            << 15 << 12 << 14
            << 10 << 15
            << 14 << 15;
    QTest::newRow(" spacey   {<te(x^t} )>|ltr")
            << standard[4]
            << 12 << 15 << 13
            << 10 << 15
            << 10 << 14;
    QTest::newRow(" spacey   {<te(xt^} )>|ltr")
            << standard[4]
            << 12 << 15 << 14
            << 10 << 15
            << 10 << 14;
}

void tst_qdeclarativetextinput::moveCursorSelectionSequence()
{
    QFETCH(QString, testStr);
    QFETCH(int, cursorPosition);
    QFETCH(int, movePosition1);
    QFETCH(int, movePosition2);
    QFETCH(int, selection1Start);
    QFETCH(int, selection1End);
    QFETCH(int, selection2Start);
    QFETCH(int, selection2End);

    QString componentStr = "import QtQuick 1.1\nTextInput {  text: \""+ testStr +"\"; }";
    QDeclarativeComponent textinputComponent(&engine);
    textinputComponent.setData(componentStr.toLatin1(), QUrl());
    QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());
    QVERIFY(textinputObject != 0);

    textinputObject->setCursorPosition(cursorPosition);

    textinputObject->moveCursorSelection(movePosition1, QDeclarativeTextInput::SelectWords);
    QCOMPARE(textinputObject->selectedText(), testStr.mid(selection1Start, selection1End - selection1Start));
    QCOMPARE(textinputObject->selectionStart(), selection1Start);
    QCOMPARE(textinputObject->selectionEnd(), selection1End);

    textinputObject->moveCursorSelection(movePosition2, QDeclarativeTextInput::SelectWords);
    QCOMPARE(textinputObject->selectedText(), testStr.mid(selection2Start, selection2End - selection2Start));
    QCOMPARE(textinputObject->selectionStart(), selection2Start);
    QCOMPARE(textinputObject->selectionEnd(), selection2End);
}

void tst_qdeclarativetextinput::mouseSelection_data()
{
    QTest::addColumn<QString>("qmlfile");
    QTest::addColumn<bool>("expectSelection");

    // import installed
    QTest::newRow("on") << SRCDIR "/data/mouseselection_true.qml" << true;
    QTest::newRow("off") << SRCDIR "/data/mouseselection_false.qml" << false;
    QTest::newRow("default") << SRCDIR "/data/mouseselection_default.qml" << false;
    QTest::newRow("on word selection") << SRCDIR "/data/mouseselection_true_words.qml" << true;
    QTest::newRow("off word selection") << SRCDIR "/data/mouseselection_false_words.qml" << false;
    QTest::newRow("on read only") << SRCDIR "/data/mouseselection_true_readonly.qml" << true;
    QTest::newRow("off read only") << SRCDIR "/data/mouseselection_false_readonly.qml" << false;
}

void tst_qdeclarativetextinput::mouseSelection()
{
    QFETCH(QString, qmlfile);
    QFETCH(bool, expectSelection);

    QDeclarativeView *canvas = createView(qmlfile);

    canvas->show();
    QApplication::setActiveWindow(canvas);
    QTest::qWaitForWindowShown(canvas);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(canvas));

    QVERIFY(canvas->rootObject() != 0);
    QDeclarativeTextInput *textInputObject = qobject_cast<QDeclarativeTextInput *>(canvas->rootObject());
    QVERIFY(textInputObject != 0);

    // press-and-drag-and-release from x1 to x2
    int x1 = 10;
    int x2 = 70;
    int y = textInputObject->height()/2;
    QTest::mousePress(canvas->viewport(), Qt::LeftButton, 0, canvas->mapFromScene(QPoint(x1,y)));
    //QTest::mouseMove(canvas->viewport(), canvas->mapFromScene(QPoint(x2,y))); // doesn't work
    QMouseEvent mv(QEvent::MouseMove, canvas->mapFromScene(QPoint(x2,y)), Qt::LeftButton, Qt::LeftButton,Qt::NoModifier);
    QApplication::sendEvent(canvas->viewport(), &mv);
    QTest::mouseRelease(canvas->viewport(), Qt::LeftButton, 0, canvas->mapFromScene(QPoint(x2,y)));
    QString str = textInputObject->selectedText();
    if (expectSelection)
        QVERIFY(str.length() > 3); // don't reallly care *what* was selected (and it's too sensitive to platform)
    else
        QVERIFY(str.isEmpty());

    // Clicking and shift to clicking between the same points should select the same text.
    textInputObject->setCursorPosition(0);
    QTest::mouseClick(canvas->viewport(), Qt::LeftButton, Qt::NoModifier, canvas->mapFromScene(QPoint(x1,y)));
    QTest::mouseClick(canvas->viewport(), Qt::LeftButton, Qt::ShiftModifier, canvas->mapFromScene(QPoint(x2,y)));
    QCOMPARE(textInputObject->selectedText(), str);

    delete canvas;
}

void tst_qdeclarativetextinput::deferEnableSelectByMouse_data()
{
    QTest::addColumn<QString>("qmlfile");

    QTest::newRow("writable") << SRCDIR "/data/mouseselection_false.qml";
    QTest::newRow("read only") << SRCDIR "/data/mouseselection_false_readonly.qml";
}

void tst_qdeclarativetextinput::deferEnableSelectByMouse()
{
    // Verify text isn't selected if selectByMouse is enabled after the mouse button has been pressed.
    QFETCH(QString, qmlfile);

    QDeclarativeView *canvas = createView(qmlfile);

    canvas->show();
    QApplication::setActiveWindow(canvas);
    QTest::qWaitForWindowShown(canvas);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(canvas));

    QVERIFY(canvas->rootObject() != 0);
    QDeclarativeTextInput *textInputObject = qobject_cast<QDeclarativeTextInput *>(canvas->rootObject());
    QVERIFY(textInputObject != 0);

    // press-and-drag-and-release from x1 to x2
    int x1 = 10;
    int x2 = 70;
    int y = textInputObject->height()/2;

    QTest::mousePress(canvas->viewport(), Qt::LeftButton, 0, canvas->mapFromScene(QPoint(x1,y)));
    textInputObject->setSelectByMouse(true);
    //QTest::mouseMove(canvas->viewport(), canvas->mapFromScene(QPoint(x2,y))); // doesn't work
    QMouseEvent mv(QEvent::MouseMove, canvas->mapFromScene(QPoint(x2,y)), Qt::LeftButton, Qt::LeftButton,Qt::NoModifier);
    QApplication::sendEvent(canvas->viewport(), &mv);
    QTest::mouseRelease(canvas->viewport(), Qt::LeftButton, 0, canvas->mapFromScene(QPoint(x2,y)));
    QVERIFY(textInputObject->selectedText().isEmpty());

    delete canvas;
}

void tst_qdeclarativetextinput::deferDisableSelectByMouse_data()
{
    QTest::addColumn<QString>("qmlfile");

    QTest::newRow("writable") << SRCDIR "/data/mouseselection_true.qml";
    QTest::newRow("read only") << SRCDIR "/data/mouseselection_true_readonly.qml";
}

void tst_qdeclarativetextinput::deferDisableSelectByMouse()
{
    // Verify text isn't selected if selectByMouse is enabled after the mouse button has been pressed.
    QFETCH(QString, qmlfile);

    QDeclarativeView *canvas = createView(qmlfile);

    canvas->show();
    QApplication::setActiveWindow(canvas);
    QTest::qWaitForWindowShown(canvas);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(canvas));

    QVERIFY(canvas->rootObject() != 0);
    QDeclarativeTextInput *textInputObject = qobject_cast<QDeclarativeTextInput *>(canvas->rootObject());
    QVERIFY(textInputObject != 0);

    // press-and-drag-and-release from x1 to x2
    int x1 = 10;
    int x2 = 70;
    int y = textInputObject->height()/2;

    QTest::mousePress(canvas->viewport(), Qt::LeftButton, 0, canvas->mapFromScene(QPoint(x1,y)));
    textInputObject->setSelectByMouse(false);
    //QTest::mouseMove(canvas->viewport(), canvas->mapFromScene(QPoint(x2,y))); // doesn't work
    QMouseEvent mv(QEvent::MouseMove, canvas->mapFromScene(QPoint(x2,y)), Qt::LeftButton, Qt::LeftButton,Qt::NoModifier);
    QApplication::sendEvent(canvas->viewport(), &mv);
    QTest::mouseRelease(canvas->viewport(), Qt::LeftButton, 0, canvas->mapFromScene(QPoint(x2,y)));
    QVERIFY(textInputObject->selectedText().length() > 3);

    delete canvas;
}

void tst_qdeclarativetextinput::dragMouseSelection()
{
    QString qmlfile = SRCDIR "/data/mouseselection_true.qml";

    QDeclarativeView *canvas = createView(qmlfile);

    canvas->show();
    QApplication::setActiveWindow(canvas);
    QTest::qWaitForWindowShown(canvas);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(canvas));

    QVERIFY(canvas->rootObject() != 0);
    QDeclarativeTextInput *textInputObject = qobject_cast<QDeclarativeTextInput *>(canvas->rootObject());
    QVERIFY(textInputObject != 0);

    // press-and-drag-and-release from x1 to x2
    int x1 = 10;
    int x2 = 70;
    int y = textInputObject->height()/2;
    QTest::mousePress(canvas->viewport(), Qt::LeftButton, 0, canvas->mapFromScene(QPoint(x1,y)));
    {
        QMouseEvent mv(QEvent::MouseMove, canvas->mapFromScene(QPoint(x2,y)), Qt::LeftButton, Qt::LeftButton,Qt::NoModifier);
        QApplication::sendEvent(canvas->viewport(), &mv);
    }
    QTest::mouseRelease(canvas->viewport(), Qt::LeftButton, 0, canvas->mapFromScene(QPoint(x2,y)));

    QString str1 = textInputObject->selectedText();
    QVERIFY(str1.length() > 3);

    // press and drag the current selection.
    x1 = 40;
    x2 = 100;
    QTest::mousePress(canvas->viewport(), Qt::LeftButton, 0, canvas->mapFromScene(QPoint(x1,y)));
    {
        QMouseEvent mv(QEvent::MouseMove, canvas->mapFromScene(QPoint(x2,y)), Qt::LeftButton, Qt::LeftButton,Qt::NoModifier);
        QApplication::sendEvent(canvas->viewport(), &mv);
    }
        QTest::mouseRelease(canvas->viewport(), Qt::LeftButton, 0, canvas->mapFromScene(QPoint(x2,y)));
    QString str2 = textInputObject->selectedText();
    QVERIFY(str2.length() > 3);

    QVERIFY(str1 != str2); // Verify the second press and drag is a new selection and doesn't not the first moved.
    delete canvas;
}

void tst_qdeclarativetextinput::mouseSelectionMode_data()
{
    QTest::addColumn<QString>("qmlfile");
    QTest::addColumn<bool>("selectWords");

    // import installed
    QTest::newRow("SelectWords") << SRCDIR "/data/mouseselectionmode_words.qml" << true;
    QTest::newRow("SelectCharacters") << SRCDIR "/data/mouseselectionmode_characters.qml" << false;
    QTest::newRow("default") << SRCDIR "/data/mouseselectionmode_default.qml" << false;
}

void tst_qdeclarativetextinput::mouseSelectionMode()
{
    QFETCH(QString, qmlfile);
    QFETCH(bool, selectWords);

    QString text = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    QDeclarativeView *canvas = createView(qmlfile);

    canvas->show();
    QApplication::setActiveWindow(canvas);
    QTest::qWaitForWindowShown(canvas);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(canvas));

    QVERIFY(canvas->rootObject() != 0);
    QDeclarativeTextInput *textInputObject = qobject_cast<QDeclarativeTextInput *>(canvas->rootObject());
    QVERIFY(textInputObject != 0);

    // press-and-drag-and-release from x1 to x2
    int x1 = 10;
    int x2 = 70;
    int y = textInputObject->height()/2;
    QTest::mousePress(canvas->viewport(), Qt::LeftButton, 0, canvas->mapFromScene(QPoint(x1,y)));
    //QTest::mouseMove(canvas->viewport(), canvas->mapFromScene(QPoint(x2,y))); // doesn't work
    QMouseEvent mv(QEvent::MouseMove, canvas->mapFromScene(QPoint(x2,y)), Qt::LeftButton, Qt::LeftButton,Qt::NoModifier);
    QApplication::sendEvent(canvas->viewport(), &mv);
    QTest::mouseRelease(canvas->viewport(), Qt::LeftButton, 0, canvas->mapFromScene(QPoint(x2,y)));
    QString str = textInputObject->selectedText();
    if (selectWords) {
        QCOMPARE(str, text);
    } else {
        QVERIFY(str.length() > 3);
        QVERIFY(str != text);
    }

    delete canvas;
}

void tst_qdeclarativetextinput::horizontalAlignment_data()
{
    QTest::addColumn<int>("hAlign");
    QTest::addColumn<QString>("expectfile");

    QTest::newRow("L") << int(Qt::AlignLeft) << "halign_left";
    QTest::newRow("R") << int(Qt::AlignRight) << "halign_right";
    QTest::newRow("C") << int(Qt::AlignHCenter) << "halign_center";
}

void tst_qdeclarativetextinput::horizontalAlignment()
{
    QFETCH(int, hAlign);
    QFETCH(QString, expectfile);

    QDeclarativeView *canvas = createView(SRCDIR "/data/horizontalAlignment.qml");

    canvas->show();
    QApplication::setActiveWindow(canvas);
    QTest::qWaitForWindowShown(canvas);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(canvas));
    QObject *ob = canvas->rootObject();
    QVERIFY(ob != 0);
    ob->setProperty("horizontalAlignment",hAlign);
    QImage actual(canvas->width(), canvas->height(), QImage::Format_RGB32);
    actual.fill(qRgb(255,255,255));
    {
        QPainter p(&actual);
        canvas->render(&p);
    }

    expectfile = createExpectedFileIfNotFound(expectfile, actual);

    QImage expect(expectfile);

    QCOMPARE(actual,expect);

    delete canvas;
}

void tst_qdeclarativetextinput::horizontalAlignment_RightToLeft()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/horizontalAlignment_RightToLeft.qml");
    QDeclarativeTextInput *textInput = canvas->rootObject()->findChild<QDeclarativeTextInput*>("text");
    QVERIFY(textInput != 0);
    canvas->show();

    const QString rtlText = textInput->text();

    QDeclarativeTextInputPrivate *textInputPrivate = QDeclarativeTextInputPrivate::get(textInput);
    QVERIFY(textInputPrivate != 0);
    QVERIFY(-textInputPrivate->hscroll > canvas->width()/2);

    // implicit alignment should follow the reading direction of RTL text
    QCOMPARE(textInput->hAlign(), QDeclarativeTextInput::AlignRight);
    QCOMPARE(textInput->effectiveHAlign(), textInput->hAlign());
    QVERIFY(-textInputPrivate->hscroll > canvas->width()/2);

    // explicitly left aligned
    textInput->setHAlign(QDeclarativeTextInput::AlignLeft);
    QCOMPARE(textInput->hAlign(), QDeclarativeTextInput::AlignLeft);
    QCOMPARE(textInput->effectiveHAlign(), textInput->hAlign());
    QVERIFY(-textInputPrivate->hscroll < canvas->width()/2);

    // explicitly right aligned
    textInput->setHAlign(QDeclarativeTextInput::AlignRight);
    QCOMPARE(textInput->effectiveHAlign(), textInput->hAlign());
    QCOMPARE(textInput->hAlign(), QDeclarativeTextInput::AlignRight);
    QVERIFY(-textInputPrivate->hscroll > canvas->width()/2);

    // explicitly center aligned
    textInput->setHAlign(QDeclarativeTextInput::AlignHCenter);
    QCOMPARE(textInput->effectiveHAlign(), textInput->hAlign());
    QCOMPARE(textInput->hAlign(), QDeclarativeTextInput::AlignHCenter);
    QVERIFY(-textInputPrivate->hscroll < canvas->width()/2);
    QVERIFY(-textInputPrivate->hscroll + textInputPrivate->width() > canvas->width()/2);

    // reseted alignment should go back to following the text reading direction
    textInput->resetHAlign();
    QCOMPARE(textInput->hAlign(), QDeclarativeTextInput::AlignRight);
    QCOMPARE(textInput->effectiveHAlign(), textInput->hAlign());
    QVERIFY(-textInputPrivate->hscroll > canvas->width()/2);

    // mirror the text item
    QDeclarativeItemPrivate::get(textInput)->setLayoutMirror(true);

    // mirrored implicit alignment should continue to follow the reading direction of the text
    QCOMPARE(textInput->hAlign(), QDeclarativeTextInput::AlignRight);
    QCOMPARE(textInput->effectiveHAlign(), textInput->hAlign());
    QVERIFY(-textInputPrivate->hscroll > canvas->width()/2);

    // explicitly right aligned behaves as left aligned
    textInput->setHAlign(QDeclarativeTextInput::AlignRight);
    QCOMPARE(textInput->hAlign(), QDeclarativeTextInput::AlignRight);
    QCOMPARE(textInput->effectiveHAlign(), QDeclarativeTextInput::AlignLeft);
    QVERIFY(-textInputPrivate->hscroll < canvas->width()/2);

    // mirrored explicitly left aligned behaves as right aligned
    textInput->setHAlign(QDeclarativeTextInput::AlignLeft);
    QCOMPARE(textInput->hAlign(), QDeclarativeTextInput::AlignLeft);
    QCOMPARE(textInput->effectiveHAlign(), QDeclarativeTextInput::AlignRight);
    QVERIFY(-textInputPrivate->hscroll > canvas->width()/2);

    // disable mirroring
    QDeclarativeItemPrivate::get(textInput)->setLayoutMirror(false);
    QCOMPARE(textInput->effectiveHAlign(), textInput->hAlign());
    textInput->resetHAlign();

    // English text should be implicitly left aligned
    textInput->setText("Hello world!");
    QCOMPARE(textInput->hAlign(), QDeclarativeTextInput::AlignLeft);
    QVERIFY(-textInputPrivate->hscroll < canvas->width()/2);

    QApplication::setActiveWindow(canvas);
    QTest::qWaitForWindowShown(canvas);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(canvas));

    // If there is no committed text, the preedit text should determine the alignment.
    textInput->setText(QString());
    { QInputMethodEvent ev(rtlText, QList<QInputMethodEvent::Attribute>()); QApplication::sendEvent(canvas, &ev); }
    QCOMPARE(textInput->hAlign(), QDeclarativeTextInput::AlignRight);
    { QInputMethodEvent ev("Hello world!", QList<QInputMethodEvent::Attribute>()); QApplication::sendEvent(canvas, &ev); }
    QCOMPARE(textInput->hAlign(), QDeclarativeTextInput::AlignLeft);

#ifndef Q_OS_MAC    // QTBUG-18040
    // empty text with implicit alignment follows the system locale-based
    // keyboard input direction from QApplication::keyboardInputDirection
    textInput->setText("");
    QCOMPARE(textInput->hAlign(), QApplication::keyboardInputDirection() == Qt::LeftToRight ?
                                  QDeclarativeTextInput::AlignLeft : QDeclarativeTextInput::AlignRight);
    if (QApplication::keyboardInputDirection() == Qt::LeftToRight)
        QVERIFY(-textInputPrivate->hscroll < canvas->width()/2);
    else
        QVERIFY(-textInputPrivate->hscroll > canvas->width()/2);
    textInput->setHAlign(QDeclarativeTextInput::AlignRight);
    QCOMPARE(textInput->hAlign(), QDeclarativeTextInput::AlignRight);
    QVERIFY(-textInputPrivate->hscroll > canvas->width()/2);
#endif

    delete canvas;

#ifndef Q_OS_MAC    // QTBUG-18040
    // alignment of TextInput with no text set to it
    QString componentStr = "import QtQuick 1.0\nTextInput {}";
    QDeclarativeComponent textComponent(&engine);
    textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeTextInput *textObject = qobject_cast<QDeclarativeTextInput*>(textComponent.create());
    QCOMPARE(textObject->hAlign(), QApplication::keyboardInputDirection() == Qt::LeftToRight ?
                                  QDeclarativeTextInput::AlignLeft : QDeclarativeTextInput::AlignRight);
    delete textObject;
#endif
}

void tst_qdeclarativetextinput::positionAt()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/positionAt.qml");
    QVERIFY(canvas->rootObject() != 0);
    canvas->show();
    canvas->setFocus();
    QApplication::setActiveWindow(canvas);
    QTest::qWaitForWindowShown(canvas);

    QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput *>(canvas->rootObject());
    QVERIFY(textinputObject != 0);

    // Check autoscrolled...
    QFontMetrics fm(textinputObject->font());

    int pos = textinputObject->positionAt(textinputObject->width()/2);
    int diff = abs(int(fm.width(textinputObject->text()) - (fm.width(textinputObject->text().left(pos))+textinputObject->width()/2)));

    // some tollerance for different fonts.
#ifdef Q_OS_LINUX
    QVERIFY(diff < 2);
#else
    QVERIFY(diff < 5);
#endif

    int x = textinputObject->positionToRectangle(pos + 1).x() - 1;
    QCOMPARE(textinputObject->positionAt(x, QDeclarativeTextInput::CursorBetweenCharacters), pos + 1);
    QCOMPARE(textinputObject->positionAt(x, QDeclarativeTextInput::CursorOnCharacter), pos);

    // Check without autoscroll...
    textinputObject->setAutoScroll(false);
    pos = textinputObject->positionAt(textinputObject->width()/2);
    diff = abs(int(fm.width(textinputObject->text().left(pos))-textinputObject->width()/2));

    // some tollerance for different fonts.
#ifdef Q_OS_LINUX
    QVERIFY(diff < 2);
#else
    QVERIFY(diff < 5);
#endif

    x = textinputObject->positionToRectangle(pos + 1).x() - 1;
    QCOMPARE(textinputObject->positionAt(x, QDeclarativeTextInput::CursorBetweenCharacters), pos + 1);
    QCOMPARE(textinputObject->positionAt(x, QDeclarativeTextInput::CursorOnCharacter), pos);

    const qreal x0 = textinputObject->positionToRectangle(pos).x();
    const qreal x1 = textinputObject->positionToRectangle(pos + 1).x();

    QString preeditText = textinputObject->text().mid(0, pos);
    textinputObject->setText(textinputObject->text().mid(pos));
    textinputObject->setCursorPosition(0);

    QInputMethodEvent inputEvent(preeditText, QList<QInputMethodEvent::Attribute>());
    QApplication::sendEvent(canvas, &inputEvent);

    // Check all points within the preedit text return the same position.
    QCOMPARE(textinputObject->positionAt(0), 0);
    QCOMPARE(textinputObject->positionAt(x0 / 2), 0);
    QCOMPARE(textinputObject->positionAt(x0), 0);

    // Verify positioning returns to normal after the preedit text.
    QCOMPARE(textinputObject->positionAt(x1), 1);
    QCOMPARE(textinputObject->positionToRectangle(1).x(), x1);

    delete canvas;
}

void tst_qdeclarativetextinput::maxLength()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/maxLength.qml");
    QVERIFY(canvas->rootObject() != 0);
    canvas->show();
    canvas->setFocus();
    QApplication::setActiveWindow(canvas);
    QTest::qWaitForWindowShown(canvas);

    QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput *>(canvas->rootObject());
    QVERIFY(textinputObject != 0);
    QVERIFY(textinputObject->text().isEmpty());
    QVERIFY(textinputObject->maxLength() == 10);
    foreach(const QString &str, standard){
        QVERIFY(textinputObject->text().length() <= 10);
        textinputObject->setText(str);
        QVERIFY(textinputObject->text().length() <= 10);
    }

    textinputObject->setText("");
    QTRY_VERIFY(textinputObject->hasActiveFocus() == true);
    for(int i=0; i<20; i++){
        QCOMPARE(textinputObject->text().length(), std::min(i,10));
        //simulateKey(canvas, Qt::Key_A);
        QTest::keyPress(canvas, Qt::Key_A);
        QTest::keyRelease(canvas, Qt::Key_A, Qt::NoModifier ,10);
    }

    delete canvas;
}

void tst_qdeclarativetextinput::masks()
{
    //Not a comprehensive test of the possible masks, that's done elsewhere (QLineEdit)
    //QString componentStr = "import QtQuick 1.0\nTextInput {  inputMask: 'HHHHhhhh'; }";
    QDeclarativeView *canvas = createView(SRCDIR "/data/masks.qml");
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);
    QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput *>(canvas->rootObject());
    QVERIFY(textinputObject != 0);
    QTRY_VERIFY(textinputObject->hasActiveFocus() == true);
    QVERIFY(textinputObject->text().length() == 0);
    QCOMPARE(textinputObject->inputMask(), QString("HHHHhhhh; "));
    for(int i=0; i<10; i++){
        QCOMPARE(std::min(i,8), textinputObject->text().length());
        QCOMPARE(i>=4, textinputObject->hasAcceptableInput());
        //simulateKey(canvas, Qt::Key_A);
        QTest::keyPress(canvas, Qt::Key_A);
        QTest::keyRelease(canvas, Qt::Key_A, Qt::NoModifier ,10);
    }

    delete canvas;
}

void tst_qdeclarativetextinput::validators()
{
    // Note that this test assumes that the validators are working properly
    // so you may need to run their tests first. All validators are checked
    // here to ensure that their exposure to QML is working.

    QDeclarativeView *canvas = createView(SRCDIR "/data/validators.qml");
    canvas->show();
    canvas->setFocus();

    QVERIFY(canvas->rootObject() != 0);

    QDeclarativeTextInput *intInput = qobject_cast<QDeclarativeTextInput *>(qvariant_cast<QObject *>(canvas->rootObject()->property("intInput")));
    QVERIFY(intInput);
    intInput->setFocus(true);
    QTRY_VERIFY(intInput->hasActiveFocus());
    QTest::keyPress(canvas, Qt::Key_1);
    QTest::keyRelease(canvas, Qt::Key_1, Qt::NoModifier ,10);
    QCOMPARE(intInput->text(), QLatin1String("1"));
    QCOMPARE(intInput->hasAcceptableInput(), false);
    QTest::keyPress(canvas, Qt::Key_2);
    QTest::keyRelease(canvas, Qt::Key_2, Qt::NoModifier ,10);
    QCOMPARE(intInput->text(), QLatin1String("1"));
    QCOMPARE(intInput->hasAcceptableInput(), false);
    QTest::keyPress(canvas, Qt::Key_1);
    QTest::keyRelease(canvas, Qt::Key_1, Qt::NoModifier ,10);
    QCOMPARE(intInput->text(), QLatin1String("11"));
    QCOMPARE(intInput->hasAcceptableInput(), true);
    QTest::keyPress(canvas, Qt::Key_0);
    QTest::keyRelease(canvas, Qt::Key_0, Qt::NoModifier ,10);
    QCOMPARE(intInput->text(), QLatin1String("11"));
    QCOMPARE(intInput->hasAcceptableInput(), true);

    QDeclarativeTextInput *dblInput = qobject_cast<QDeclarativeTextInput *>(qvariant_cast<QObject *>(canvas->rootObject()->property("dblInput")));
    QTRY_VERIFY(dblInput);
    dblInput->setFocus(true);
    QVERIFY(dblInput->hasActiveFocus() == true);
    QTest::keyPress(canvas, Qt::Key_1);
    QTest::keyRelease(canvas, Qt::Key_1, Qt::NoModifier ,10);
    QCOMPARE(dblInput->text(), QLatin1String("1"));
    QCOMPARE(dblInput->hasAcceptableInput(), false);
    QTest::keyPress(canvas, Qt::Key_2);
    QTest::keyRelease(canvas, Qt::Key_2, Qt::NoModifier ,10);
    QCOMPARE(dblInput->text(), QLatin1String("12"));
    QCOMPARE(dblInput->hasAcceptableInput(), true);
    QTest::keyPress(canvas, Qt::Key_Period);
    QTest::keyRelease(canvas, Qt::Key_Period, Qt::NoModifier ,10);
    QCOMPARE(dblInput->text(), QLatin1String("12."));
    QCOMPARE(dblInput->hasAcceptableInput(), true);
    QTest::keyPress(canvas, Qt::Key_1);
    QTest::keyRelease(canvas, Qt::Key_1, Qt::NoModifier ,10);
    QCOMPARE(dblInput->text(), QLatin1String("12.1"));
    QCOMPARE(dblInput->hasAcceptableInput(), true);
    QTest::keyPress(canvas, Qt::Key_1);
    QTest::keyRelease(canvas, Qt::Key_1, Qt::NoModifier ,10);
    QCOMPARE(dblInput->text(), QLatin1String("12.11"));
    QCOMPARE(dblInput->hasAcceptableInput(), true);
    QTest::keyPress(canvas, Qt::Key_1);
    QTest::keyRelease(canvas, Qt::Key_1, Qt::NoModifier ,10);
    QCOMPARE(dblInput->text(), QLatin1String("12.11"));
    QCOMPARE(dblInput->hasAcceptableInput(), true);

    QDeclarativeTextInput *strInput = qobject_cast<QDeclarativeTextInput *>(qvariant_cast<QObject *>(canvas->rootObject()->property("strInput")));
    QTRY_VERIFY(strInput);
    strInput->setFocus(true);
    QVERIFY(strInput->hasActiveFocus() == true);
    QTest::keyPress(canvas, Qt::Key_1);
    QTest::keyRelease(canvas, Qt::Key_1, Qt::NoModifier ,10);
    QCOMPARE(strInput->text(), QLatin1String(""));
    QCOMPARE(strInput->hasAcceptableInput(), false);
    QTest::keyPress(canvas, Qt::Key_A);
    QTest::keyRelease(canvas, Qt::Key_A, Qt::NoModifier ,10);
    QCOMPARE(strInput->text(), QLatin1String("a"));
    QCOMPARE(strInput->hasAcceptableInput(), false);
    QTest::keyPress(canvas, Qt::Key_A);
    QTest::keyRelease(canvas, Qt::Key_A, Qt::NoModifier ,10);
    QCOMPARE(strInput->text(), QLatin1String("aa"));
    QCOMPARE(strInput->hasAcceptableInput(), true);
    QTest::keyPress(canvas, Qt::Key_A);
    QTest::keyRelease(canvas, Qt::Key_A, Qt::NoModifier ,10);
    QCOMPARE(strInput->text(), QLatin1String("aaa"));
    QCOMPARE(strInput->hasAcceptableInput(), true);
    QTest::keyPress(canvas, Qt::Key_A);
    QTest::keyRelease(canvas, Qt::Key_A, Qt::NoModifier ,10);
    QCOMPARE(strInput->text(), QLatin1String("aaaa"));
    QCOMPARE(strInput->hasAcceptableInput(), true);
    QTest::keyPress(canvas, Qt::Key_A);
    QTest::keyRelease(canvas, Qt::Key_A, Qt::NoModifier ,10);
    QCOMPARE(strInput->text(), QLatin1String("aaaa"));
    QCOMPARE(strInput->hasAcceptableInput(), true);

    delete canvas;
}

#ifndef QT_NO_IM
void tst_qdeclarativetextinput::inputMethods()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/inputmethods.qml");
    canvas->show();
    canvas->setFocus();
    QApplication::setActiveWindow(canvas);
    QTest::qWaitForWindowShown(canvas);

    // test input method hints
    QVERIFY(canvas->rootObject() != 0);
    QDeclarativeTextInput *input = qobject_cast<QDeclarativeTextInput *>(canvas->rootObject());
    QVERIFY(input != 0);
    QVERIFY(input->imHints() & Qt::ImhNoPredictiveText);
    QVERIFY(input->inputMethodHints() & Qt::ImhNoPredictiveText);
    input->setIMHints(Qt::ImhUppercaseOnly);
    QVERIFY(input->imHints() & Qt::ImhUppercaseOnly);
    QVERIFY(input->inputMethodHints() & Qt::ImhUppercaseOnly);

    QVERIFY(canvas->rootObject() != 0);

    input->setFocus(true);
    QVERIFY(input->hasActiveFocus() == true);
    // test that input method event is committed
    QInputMethodEvent event;
    event.setCommitString( "My ", -12, 0);
    QApplication::sendEvent(canvas, &event);
    QCOMPARE(input->text(), QString("My Hello world!"));

    input->setCursorPosition(2);
    event.setCommitString("Your", -2, 2);
    QApplication::sendEvent(canvas, &event);
    QCOMPARE(input->text(), QString("Your Hello world!"));
    QCOMPARE(input->cursorPosition(), 4);

    input->setCursorPosition(7);
    event.setCommitString("Goodbye", -2, 5);
    QApplication::sendEvent(canvas, &event);
    QCOMPARE(input->text(), QString("Your Goodbye world!"));
    QCOMPARE(input->cursorPosition(), 12);

    input->setCursorPosition(8);
    event.setCommitString("Our", -8, 4);
    QApplication::sendEvent(canvas, &event);
    QCOMPARE(input->text(), QString("Our Goodbye world!"));
    QCOMPARE(input->cursorPosition(), 7);

    delete canvas;
}
#endif // QT_NO_IM

/*
TextInput element should only handle left/right keys until the cursor reaches
the extent of the text, then they should ignore the keys.

*/
void tst_qdeclarativetextinput::navigation()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/navigation.qml");
    canvas->show();
    canvas->setFocus();

    QVERIFY(canvas->rootObject() != 0);

    QDeclarativeTextInput *input = qobject_cast<QDeclarativeTextInput *>(qvariant_cast<QObject *>(canvas->rootObject()->property("myInput")));

    QVERIFY(input != 0);
    input->setCursorPosition(0);
    QTRY_VERIFY(input->hasActiveFocus() == true);
    simulateKey(canvas, Qt::Key_Left);
    QVERIFY(input->hasActiveFocus() == false);
    simulateKey(canvas, Qt::Key_Right);
    QVERIFY(input->hasActiveFocus() == true);
    //QT-2944: If text is selected, ensure we deselect upon cursor motion
    input->setCursorPosition(input->text().length());
    input->select(0,input->text().length());
    QVERIFY(input->selectionStart() != input->selectionEnd());
    simulateKey(canvas, Qt::Key_Right);
    QVERIFY(input->selectionStart() == input->selectionEnd());
    QVERIFY(input->selectionStart() == input->text().length());
    QVERIFY(input->hasActiveFocus() == true);
    simulateKey(canvas, Qt::Key_Right);
    QVERIFY(input->hasActiveFocus() == false);
    simulateKey(canvas, Qt::Key_Left);
    QVERIFY(input->hasActiveFocus() == true);

    // Up and Down should NOT do Home/End, even on Mac OS X (QTBUG-10438).
    input->setCursorPosition(2);
    QCOMPARE(input->cursorPosition(),2);
    simulateKey(canvas, Qt::Key_Up);
    QCOMPARE(input->cursorPosition(),2);
    simulateKey(canvas, Qt::Key_Down);
    QCOMPARE(input->cursorPosition(),2);

    // Test left and right navigation works if the TextInput is empty (QTBUG-25447).
    input->setText(QString());
    QCOMPARE(input->cursorPosition(), 0);
    simulateKey(canvas, Qt::Key_Left);
    QCOMPARE(input->hasActiveFocus(), false);
    simulateKey(canvas, Qt::Key_Right);
    QCOMPARE(input->hasActiveFocus(), true);
    simulateKey(canvas, Qt::Key_Right);
    QCOMPARE(input->hasActiveFocus(), false);
    simulateKey(canvas, Qt::Key_Left);
    QCOMPARE(input->hasActiveFocus(), true);

    delete canvas;
}

void tst_qdeclarativetextinput::navigation_RTL()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/navigation.qml");
    canvas->show();
    canvas->setFocus();

    QVERIFY(canvas->rootObject() != 0);

    QDeclarativeTextInput *input = qobject_cast<QDeclarativeTextInput *>(qvariant_cast<QObject *>(canvas->rootObject()->property("myInput")));

    QVERIFY(input != 0);
    const quint16 arabic_str[] = { 0x0638, 0x0643, 0x00646, 0x0647, 0x0633, 0x0638, 0x0643, 0x00646, 0x0647, 0x0633, 0x0647};
    input->setText(QString::fromUtf16(arabic_str, 11));

    input->setCursorPosition(0);
    QTRY_VERIFY(input->hasActiveFocus() == true);

    // move off
    simulateKey(canvas, Qt::Key_Right);
    QVERIFY(input->hasActiveFocus() == false);

    // move back
    simulateKey(canvas, Qt::Key_Left);
    QVERIFY(input->hasActiveFocus() == true);

    input->setCursorPosition(input->text().length());
    QVERIFY(input->hasActiveFocus() == true);

    // move off
    simulateKey(canvas, Qt::Key_Left);
    QVERIFY(input->hasActiveFocus() == false);

    // move back
    simulateKey(canvas, Qt::Key_Right);
    QVERIFY(input->hasActiveFocus() == true);

    delete canvas;
}

void tst_qdeclarativetextinput::copyAndPaste() {
#ifndef QT_NO_CLIPBOARD

#ifdef Q_WS_MAC
    {
        PasteboardRef pasteboard;
        OSStatus status = PasteboardCreate(0, &pasteboard);
        if (status == noErr)
            CFRelease(pasteboard);
        else
            QSKIP("This machine doesn't support the clipboard", SkipAll);
    }
#endif

    QString componentStr = "import QtQuick 1.0\nTextInput { text: \"Hello world!\" }";
    QDeclarativeComponent textInputComponent(&engine);
    textInputComponent.setData(componentStr.toLatin1(), QUrl());
    QDeclarativeTextInput *textInput = qobject_cast<QDeclarativeTextInput*>(textInputComponent.create());
    QVERIFY(textInput != 0);

    // copy and paste
    QCOMPARE(textInput->text().length(), 12);
    textInput->select(0, textInput->text().length());;
    textInput->copy();
    QCOMPARE(textInput->selectedText(), QString("Hello world!"));
    QCOMPARE(textInput->selectedText().length(), 12);
    textInput->setCursorPosition(0);
    QVERIFY(textInput->canPaste());
    textInput->paste();
    QCOMPARE(textInput->text(), QString("Hello world!Hello world!"));
    QCOMPARE(textInput->text().length(), 24);

    // can paste
    QVERIFY(textInput->canPaste());
    textInput->setReadOnly(true);
    QVERIFY(!textInput->canPaste());
    textInput->setReadOnly(false);
    QVERIFY(textInput->canPaste());

    // select word
    textInput->setCursorPosition(0);
    textInput->selectWord();
    QCOMPARE(textInput->selectedText(), QString("Hello"));

    // select all and cut
    textInput->selectAll();
    textInput->cut();
    QCOMPARE(textInput->text().length(), 0);
    textInput->paste();
    QCOMPARE(textInput->text(), QString("Hello world!Hello world!"));
    QCOMPARE(textInput->text().length(), 24);

    // clear copy buffer
    QClipboard *clipboard = QApplication::clipboard();
    QVERIFY(clipboard);
    clipboard->clear();
    QVERIFY(!textInput->canPaste());

    // test that copy functionality is disabled
    // when echo mode is set to hide text/password mode
    int index = 0;
    while (index < 4) {
        QDeclarativeTextInput::EchoMode echoMode = QDeclarativeTextInput::EchoMode(index);
        textInput->setEchoMode(echoMode);
        textInput->setText("My password");
        textInput->select(0, textInput->text().length());;
        textInput->copy();
        if (echoMode == QDeclarativeTextInput::Normal) {
            QVERIFY(!clipboard->text().isEmpty());
            QCOMPARE(clipboard->text(), QString("My password"));
            clipboard->clear();
        } else {
            QVERIFY(clipboard->text().isEmpty());
        }
        index++;
    }
#endif
}

void tst_qdeclarativetextinput::canPasteEmpty() {
#ifndef QT_NO_CLIPBOARD

    QApplication::clipboard()->clear();

    QString componentStr = "import QtQuick 1.0\nTextInput { text: \"Hello world!\" }";
    QDeclarativeComponent textInputComponent(&engine);
    textInputComponent.setData(componentStr.toLatin1(), QUrl());
    QDeclarativeTextInput *textInput = qobject_cast<QDeclarativeTextInput*>(textInputComponent.create());
    QVERIFY(textInput != 0);

    QLineControl lc;
    bool cp = !lc.isReadOnly() && QApplication::clipboard()->text().length() != 0;
    QCOMPARE(textInput->canPaste(), cp);

#endif
}

void tst_qdeclarativetextinput::canPaste() {
#ifndef QT_NO_CLIPBOARD

    QApplication::clipboard()->setText("Some text");

    QString componentStr = "import QtQuick 1.0\nTextInput { text: \"Hello world!\" }";
    QDeclarativeComponent textInputComponent(&engine);
    textInputComponent.setData(componentStr.toLatin1(), QUrl());
    QDeclarativeTextInput *textInput = qobject_cast<QDeclarativeTextInput*>(textInputComponent.create());
    QVERIFY(textInput != 0);

    QLineControl lc;
    bool cp = !lc.isReadOnly() && QApplication::clipboard()->text().length() != 0;
    QCOMPARE(textInput->canPaste(), cp);

#endif
}

void tst_qdeclarativetextinput::passwordCharacter()
{
    QString componentStr = "import QtQuick 1.0\nTextInput { text: \"Hello world!\"; font.family: \"Helvetica\"; echoMode: TextInput.Password }";
    QDeclarativeComponent textInputComponent(&engine);
    textInputComponent.setData(componentStr.toLatin1(), QUrl());
    QDeclarativeTextInput *textInput = qobject_cast<QDeclarativeTextInput*>(textInputComponent.create());
    QVERIFY(textInput != 0);

    textInput->setPasswordCharacter("X");
    QSize contentsSize = textInput->contentsSize();
    textInput->setPasswordCharacter(".");
    // QTBUG-12383 content is updated and redrawn
    QVERIFY(contentsSize != textInput->contentsSize());
}

void tst_qdeclarativetextinput::cursorDelegate()
{
    QDeclarativeView* view = createView(SRCDIR "/data/cursorTest.qml");
    view->show();
    view->setFocus();
    QDeclarativeTextInput *textInputObject = view->rootObject()->findChild<QDeclarativeTextInput*>("textInputObject");
    QVERIFY(textInputObject != 0);
    QVERIFY(textInputObject->findChild<QDeclarativeItem*>("cursorInstance"));
    //Test Delegate gets created
    textInputObject->setFocus(true);
    QDeclarativeItem* delegateObject = textInputObject->findChild<QDeclarativeItem*>("cursorInstance");
    QVERIFY(delegateObject);
    //Test Delegate gets moved
    for(int i=0; i<= textInputObject->text().length(); i++){
        textInputObject->setCursorPosition(i);
        QCOMPARE(textInputObject->cursorRectangle().x(), qRound(delegateObject->x()));
        QCOMPARE(textInputObject->cursorRectangle().y(), qRound(delegateObject->y()));
    }
    const QString preedit = "preedit";
    for (int i = 0; i <= preedit.length(); i++) {
        QInputMethodEvent event(preedit, QList<QInputMethodEvent::Attribute>()
                << QInputMethodEvent::Attribute(QInputMethodEvent::Cursor, i, 1, QVariant()));
        QApplication::sendEvent(view, &event);

        QCOMPARE(textInputObject->cursorRectangle().x(), qRound(delegateObject->x()));
        QCOMPARE(textInputObject->cursorRectangle().y(), qRound(delegateObject->y()));
    }
    textInputObject->setCursorPosition(0);
    QCOMPARE(textInputObject->cursorRectangle().x(), qRound(delegateObject->x()));
    QCOMPARE(textInputObject->cursorRectangle().y(), qRound(delegateObject->y()));
    //Test Delegate gets deleted
    textInputObject->setCursorDelegate(0);
    QVERIFY(!textInputObject->findChild<QDeclarativeItem*>("cursorInstance"));

    delete view;
}

void tst_qdeclarativetextinput::cursorVisible()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    view.show();
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&view));

    QDeclarativeTextInput input;
    QSignalSpy spy(&input, SIGNAL(cursorVisibleChanged(bool)));

    QCOMPARE(input.isCursorVisible(), false);

    input.setCursorVisible(true);
    QCOMPARE(input.isCursorVisible(), true);
    QCOMPARE(spy.count(), 1);

    input.setCursorVisible(false);
    QCOMPARE(input.isCursorVisible(), false);
    QCOMPARE(spy.count(), 2);

    input.setFocus(true);
    QCOMPARE(input.isCursorVisible(), false);
    QCOMPARE(spy.count(), 2);

    scene.addItem(&input);
    QCOMPARE(input.isCursorVisible(), true);
    QCOMPARE(spy.count(), 3);

    input.setFocus(false);
    QCOMPARE(input.isCursorVisible(), false);
    QCOMPARE(spy.count(), 4);

    input.setFocus(true);
    QCOMPARE(input.isCursorVisible(), true);
    QCOMPARE(spy.count(), 5);

    scene.clearFocus();
    QCOMPARE(input.isCursorVisible(), false);
    QCOMPARE(spy.count(), 6);

    scene.setFocus();
    QCOMPARE(input.isCursorVisible(), true);
    QCOMPARE(spy.count(), 7);

    view.clearFocus();
    QCOMPARE(input.isCursorVisible(), false);
    QCOMPARE(spy.count(), 8);

    view.setFocus();
    QCOMPARE(input.isCursorVisible(), true);
    QCOMPARE(spy.count(), 9);

    // on mac, setActiveWindow(0) on mac does not deactivate the current application
    // (you have to switch to a different app or hide the current app to trigger this)
#if !defined(Q_WS_MAC)
    QApplication::setActiveWindow(0);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(0));
    QCOMPARE(input.isCursorVisible(), false);
    QCOMPARE(spy.count(), 10);

    QApplication::setActiveWindow(&view);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&view));
    QCOMPARE(input.isCursorVisible(), true);
    QCOMPARE(spy.count(), 11);
#endif
}

void tst_qdeclarativetextinput::cursorRectangle()
{
    QString text = "Hello World!";

    QDeclarativeTextInput input;
    input.setText(text);
    QFontMetricsF fm(input.font());
    input.setWidth(fm.width(text.mid(0, 5)));

    QRect r;

    // some tolerance for different fonts.
#ifdef Q_OS_LINUX
    const int error = 2;
#else
    const int error = 5;
#endif

    for (int i = 0; i <= 5; ++i) {
        input.setCursorPosition(i);
        r = input.cursorRectangle();
        int textWidth = fm.width(text.mid(0, i));

        QVERIFY(r.left() < textWidth + error);
        QVERIFY(r.right() > textWidth - error);
        QCOMPARE(input.inputMethodQuery(Qt::ImMicroFocus).toRect(), r);
    }

    // Check the cursor rectangle remains within the input bounding rect when auto scrolling.
    QVERIFY(r.left() < input.boundingRect().width());
    QVERIFY(r.right() >= input.width() - error);

    for (int i = 6; i < text.length(); ++i) {
        input.setCursorPosition(i);
        QCOMPARE(r, input.cursorRectangle());
        QCOMPARE(input.inputMethodQuery(Qt::ImMicroFocus).toRect(), r);
    }

    for (int i = text.length() - 2; i >= 0; --i) {
        input.setCursorPosition(i);
        r = input.cursorRectangle();
        QVERIFY(r.right() >= 0);
        QCOMPARE(input.inputMethodQuery(Qt::ImMicroFocus).toRect(), r);
    }
}

void tst_qdeclarativetextinput::readOnly()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/readOnly.qml");
    canvas->show();
    canvas->setFocus();

    QVERIFY(canvas->rootObject() != 0);

    QDeclarativeTextInput *input = qobject_cast<QDeclarativeTextInput *>(qvariant_cast<QObject *>(canvas->rootObject()->property("myInput")));

    QVERIFY(input != 0);
    QTRY_VERIFY(input->hasActiveFocus() == true);
    QVERIFY(input->isReadOnly() == true);
    QString initial = input->text();
    for(int k=Qt::Key_0; k<=Qt::Key_Z; k++)
        simulateKey(canvas, k);
    simulateKey(canvas, Qt::Key_Return);
    simulateKey(canvas, Qt::Key_Space);
    simulateKey(canvas, Qt::Key_Escape);
    QCOMPARE(input->text(), initial);

    delete canvas;
}

void tst_qdeclarativetextinput::echoMode()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/echoMode.qml");
    canvas->show();
    canvas->setFocus();
    QApplication::setActiveWindow(canvas);
    QTest::qWaitForWindowShown(canvas);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(canvas));

    QVERIFY(canvas->rootObject() != 0);

    QDeclarativeTextInput *input = qobject_cast<QDeclarativeTextInput *>(qvariant_cast<QObject *>(canvas->rootObject()->property("myInput")));

    QVERIFY(input != 0);
    QTRY_VERIFY(input->hasActiveFocus() == true);
    QString initial = input->text();
    Qt::InputMethodHints ref;
    QCOMPARE(initial, QLatin1String("ABCDefgh"));
    QCOMPARE(input->echoMode(), QDeclarativeTextInput::Normal);
    QCOMPARE(input->displayText(), input->text());
    //Normal
    ref &= ~Qt::ImhHiddenText;
    ref &= ~(Qt::ImhNoAutoUppercase | Qt::ImhNoPredictiveText);
    QCOMPARE(input->inputMethodHints(), ref);
    QCOMPARE(input->imHints(), Qt::ImhNone);
    input->setEchoMode(QDeclarativeTextInput::NoEcho);
    QCOMPARE(input->text(), initial);
    QCOMPARE(input->displayText(), QLatin1String(""));
    QCOMPARE(input->passwordCharacter(), QLatin1String("*"));
    //NoEcho
    ref |= Qt::ImhHiddenText;
    ref |= (Qt::ImhNoAutoUppercase | Qt::ImhNoPredictiveText);
    QCOMPARE(input->inputMethodHints(), ref);
    QCOMPARE(input->imHints(), Qt::ImhNone);
    input->setEchoMode(QDeclarativeTextInput::Password);
    //Password
    ref |= Qt::ImhHiddenText;
    ref |= (Qt::ImhNoAutoUppercase | Qt::ImhNoPredictiveText);
    QCOMPARE(input->text(), initial);
    QCOMPARE(input->displayText(), QLatin1String("********"));
    QCOMPARE(input->inputMethodHints(), ref);
    QCOMPARE(input->imHints(), Qt::ImhNone);
    input->setPasswordCharacter(QChar('Q'));
    QCOMPARE(input->passwordCharacter(), QLatin1String("Q"));
    QCOMPARE(input->text(), initial);
    QCOMPARE(input->displayText(), QLatin1String("QQQQQQQQ"));
    input->setEchoMode(QDeclarativeTextInput::PasswordEchoOnEdit);
    //PasswordEchoOnEdit
    ref &= ~Qt::ImhHiddenText;
    ref |= (Qt::ImhNoAutoUppercase | Qt::ImhNoPredictiveText);
    QCOMPARE(input->inputMethodHints(), ref);
    QCOMPARE(input->imHints(), Qt::ImhNone);
    QCOMPARE(input->text(), initial);
    QCOMPARE(input->displayText(), QLatin1String("QQQQQQQQ"));
    QCOMPARE(input->inputMethodQuery(Qt::ImSurroundingText).toString(), QLatin1String("QQQQQQQQ"));
    QTest::keyPress(canvas, Qt::Key_A);//Clearing previous entry is part of PasswordEchoOnEdit
    QTest::keyRelease(canvas, Qt::Key_A, Qt::NoModifier ,10);
    QCOMPARE(input->text(), QLatin1String("a"));
    QCOMPARE(input->displayText(), QLatin1String("a"));
    QCOMPARE(input->inputMethodQuery(Qt::ImSurroundingText).toString(), QLatin1String("a"));
    input->setFocus(false);
    QVERIFY(input->hasActiveFocus() == false);
    QCOMPARE(input->displayText(), QLatin1String("Q"));
    QCOMPARE(input->inputMethodQuery(Qt::ImSurroundingText).toString(), QLatin1String("Q"));
    input->setFocus(true);
    QInputMethodEvent inputEvent;
    inputEvent.setCommitString(initial);
    QApplication::sendEvent(canvas, &inputEvent);
    QCOMPARE(input->text(), initial);
    QCOMPARE(input->displayText(), initial);
    QCOMPARE(input->inputMethodQuery(Qt::ImSurroundingText).toString(), initial);

    // Test echo mode doesn't override imHints.
    input->setIMHints(Qt::ImhHiddenText | Qt::ImhDialableCharactersOnly);
    ref |=  Qt::ImhDialableCharactersOnly;
    //Normal
    input->setEchoMode(QDeclarativeTextInput::Normal);
    ref |= Qt::ImhHiddenText;
    ref &= ~(Qt::ImhNoAutoUppercase | Qt::ImhNoPredictiveText);
    QCOMPARE(input->inputMethodHints(), ref);
    QCOMPARE(input->imHints(), Qt::ImhHiddenText | Qt::ImhDialableCharactersOnly);
    //NoEcho
    input->setEchoMode(QDeclarativeTextInput::NoEcho);
    ref |= Qt::ImhHiddenText;
    ref |= (Qt::ImhNoAutoUppercase | Qt::ImhNoPredictiveText);
    QCOMPARE(input->inputMethodHints(), ref);
    QCOMPARE(input->imHints(), Qt::ImhHiddenText | Qt::ImhDialableCharactersOnly);
    //Password
    input->setEchoMode(QDeclarativeTextInput::Password);
    ref |= Qt::ImhHiddenText;
    ref |= (Qt::ImhNoAutoUppercase | Qt::ImhNoPredictiveText);
    QCOMPARE(input->inputMethodHints(), ref);
    QCOMPARE(input->imHints(), Qt::ImhHiddenText | Qt::ImhDialableCharactersOnly);
    //PasswordEchoOnEdit
    input->setEchoMode(QDeclarativeTextInput::PasswordEchoOnEdit);
    ref &= ~Qt::ImhHiddenText;
    ref |= (Qt::ImhNoAutoUppercase | Qt::ImhNoPredictiveText);
    QCOMPARE(input->inputMethodHints(), ref);
    QCOMPARE(input->imHints(), Qt::ImhHiddenText | Qt::ImhDialableCharactersOnly);
    //Normal
    input->setEchoMode(QDeclarativeTextInput::Normal);
    ref |= Qt::ImhHiddenText;
    ref &= ~(Qt::ImhNoAutoUppercase | Qt::ImhNoPredictiveText);
    QCOMPARE(input->inputMethodHints(), ref);
    QCOMPARE(input->imHints(), Qt::ImhHiddenText | Qt::ImhDialableCharactersOnly);

    delete canvas;
}


#ifdef QT_GUI_PASSWORD_ECHO_DELAY
void tst_qdeclarativetextinput::passwordEchoDelay()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/echoMode.qml");
    canvas->show();
    canvas->setFocus();
    QApplication::setActiveWindow(canvas);
    QTest::qWaitForWindowShown(canvas);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(canvas));

    QVERIFY(canvas->rootObject() != 0);

    QDeclarativeTextInput *input = qobject_cast<QDeclarativeTextInput *>(qvariant_cast<QObject *>(canvas->rootObject()->property("myInput")));

    QChar fillChar = QLatin1Char('*');

    input->setEchoMode(QDeclarativeTextInput::Password);
    QCOMPARE(input->displayText(), QString(8, fillChar));
    input->setText(QString());
    QCOMPARE(input->displayText(), QString());

    QTest::keyPress(canvas, '0');
    QTest::keyPress(canvas, '1');
    QTest::keyPress(canvas, '2');
    QCOMPARE(input->displayText(), QString(2, fillChar) + QLatin1Char('2'));
    QTest::keyPress(canvas, '3');
    QTest::keyPress(canvas, '4');
    QCOMPARE(input->displayText(), QString(4, fillChar) + QLatin1Char('4'));
    QTest::keyPress(canvas, Qt::Key_Backspace);
    QCOMPARE(input->displayText(), QString(4, fillChar));
    QTest::keyPress(canvas, '4');
    QCOMPARE(input->displayText(), QString(4, fillChar) + QLatin1Char('4'));
    QTest::qWait(QT_GUI_PASSWORD_ECHO_DELAY);
    QTRY_COMPARE(input->displayText(), QString(5, fillChar));
    QTest::keyPress(canvas, '5');
    QCOMPARE(input->displayText(), QString(5, fillChar) + QLatin1Char('5'));
    input->setFocus(false);
    QVERIFY(!input->hasFocus());
    QCOMPARE(input->displayText(), QString(6, fillChar));
    input->setFocus(true);
    QTRY_VERIFY(input->hasFocus());
    QCOMPARE(input->displayText(), QString(6, fillChar));
    QTest::keyPress(canvas, '6');
    QCOMPARE(input->displayText(), QString(6, fillChar) + QLatin1Char('6'));

    QInputMethodEvent ev;
    ev.setCommitString(QLatin1String("7"));
    QApplication::sendEvent(canvas, &ev);
    QCOMPARE(input->displayText(), QString(7, fillChar) + QLatin1Char('7'));

    delete canvas;
}
#endif


void tst_qdeclarativetextinput::simulateKey(QDeclarativeView *view, int key)
{
    QKeyEvent press(QKeyEvent::KeyPress, key, 0);
    QKeyEvent release(QKeyEvent::KeyRelease, key, 0);

    QApplication::sendEvent(view, &press);
    QApplication::sendEvent(view, &release);
}

QDeclarativeView *tst_qdeclarativetextinput::createView(const QString &filename)
{
    QDeclarativeView *canvas = new QDeclarativeView(0);

    canvas->setSource(QUrl::fromLocalFile(filename));

    return canvas;
}

#ifndef QT_NO_IM
class MyInputContext : public QInputContext
{
public:
    MyInputContext() : openInputPanelReceived(false), closeInputPanelReceived(false), updateReceived(false), eventType(QEvent::None) {}
    ~MyInputContext() {}

    QString identifierName() { return QString(); }
    QString language() { return QString(); }

    void reset() {}

    bool isComposing() const { return false; }

    bool filterEvent( const QEvent *event )
    {
        if (event->type() == QEvent::RequestSoftwareInputPanel)
            openInputPanelReceived = true;
        if (event->type() == QEvent::CloseSoftwareInputPanel)
            closeInputPanelReceived = true;
        return QInputContext::filterEvent(event);
    }

    void update() { updateReceived = true; }

    void mouseHandler(int x, QMouseEvent *event)
    {
        cursor = x;
        eventType = event->type();
        eventPosition = event->pos();
        eventGlobalPosition = event->globalPos();
        eventButton = event->button();
        eventButtons = event->buttons();
        eventModifiers = event->modifiers();
    }

    void sendPreeditText(const QString &text, int cursor)
    {
        QList<QInputMethodEvent::Attribute> attributes;
        attributes.append(QInputMethodEvent::Attribute(
                QInputMethodEvent::Cursor, cursor, text.length(), QVariant()));

        QInputMethodEvent event(text, attributes);
        sendEvent(event);
    }

    bool openInputPanelReceived;
    bool closeInputPanelReceived;
    bool updateReceived;
    int cursor;
    QEvent::Type eventType;
    QPoint eventPosition;
    QPoint eventGlobalPosition;
    Qt::MouseButton eventButton;
    Qt::MouseButtons eventButtons;
    Qt::KeyboardModifiers eventModifiers;
};

void tst_qdeclarativetextinput::openInputPanelOnClick()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    MyInputContext ic;
    view.setInputContext(&ic);
    QDeclarativeTextInput input;
    QSignalSpy focusOnPressSpy(&input, SIGNAL(activeFocusOnPressChanged(bool)));
    input.setText("Hello world");
    input.setPos(0, 0);
    scene.addItem(&input);
    view.show();
    qApp->setAutoSipEnabled(true);
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&view));

    QDeclarativeItemPrivate* pri = QDeclarativeItemPrivate::get(&input);
    QDeclarativeTextInputPrivate *inputPrivate = static_cast<QDeclarativeTextInputPrivate*>(pri);

    // input panel on click
    inputPrivate->showInputPanelOnFocus = false;

    QStyle::RequestSoftwareInputPanel behavior = QStyle::RequestSoftwareInputPanel(
            view.style()->styleHint(QStyle::SH_RequestSoftwareInputPanel));
    QTest::mouseClick(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(input.scenePos()));
    QApplication::processEvents();
    if (behavior == QStyle::RSIP_OnMouseClickAndAlreadyFocused) {
        QCOMPARE(ic.openInputPanelReceived, false);
        QTest::mouseClick(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(input.scenePos()));
        QApplication::processEvents();
        QCOMPARE(ic.openInputPanelReceived, true);
    } else if (behavior == QStyle::RSIP_OnMouseClick) {
        QCOMPARE(ic.openInputPanelReceived, true);
    }
    ic.openInputPanelReceived = false;

    // focus should not cause input panels to open or close
    input.setFocus(false);
    input.setFocus(true);
    input.setFocus(false);
    input.setFocus(true);
    input.setFocus(false);
    QCOMPARE(ic.openInputPanelReceived, false);
    QCOMPARE(ic.closeInputPanelReceived, false);
}

void tst_qdeclarativetextinput::openInputPanelOnFocus()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    MyInputContext ic;
    view.setInputContext(&ic);
    QDeclarativeTextInput input;
    QSignalSpy focusOnPressSpy(&input, SIGNAL(activeFocusOnPressChanged(bool)));
    input.setText("Hello world");
    input.setPos(0, 0);
    scene.addItem(&input);
    view.show();
    qApp->setAutoSipEnabled(true);
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&view));

    QDeclarativeItemPrivate* pri = QDeclarativeItemPrivate::get(&input);
    QDeclarativeTextInputPrivate *inputPrivate = static_cast<QDeclarativeTextInputPrivate*>(pri);
    inputPrivate->showInputPanelOnFocus = true;

    // test default values
    QVERIFY(input.focusOnPress());
    QCOMPARE(ic.openInputPanelReceived, false);
    QCOMPARE(ic.closeInputPanelReceived, false);

    // focus on press, input panel on focus
    QTest::mousePress(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(input.scenePos()));
    QApplication::processEvents();
    QVERIFY(input.hasActiveFocus());
    QCOMPARE(ic.openInputPanelReceived, true);
    ic.openInputPanelReceived = false;

    // no events on release
    QTest::mouseRelease(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(input.scenePos()));
    QCOMPARE(ic.openInputPanelReceived, false);
    ic.openInputPanelReceived = false;

    // if already focused, input panel can be opened on press
    QVERIFY(input.hasActiveFocus());
    QTest::mousePress(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(input.scenePos()));
    QApplication::processEvents();
    QCOMPARE(ic.openInputPanelReceived, true);
    ic.openInputPanelReceived = false;

    // input method should stay enabled if focus
    // is lost to an item that also accepts inputs
    QDeclarativeTextInput anotherInput;
    scene.addItem(&anotherInput);
    anotherInput.setFocus(true);
    QApplication::processEvents();
    QCOMPARE(ic.openInputPanelReceived, true);
    ic.openInputPanelReceived = false;
    QCOMPARE(view.inputContext(), (QInputContext*)&ic);
    QVERIFY(view.testAttribute(Qt::WA_InputMethodEnabled));

    // input method should be disabled if focus
    // is lost to an item that doesn't accept inputs
    QDeclarativeItem item;
    scene.addItem(&item);
    item.setFocus(true);
    QApplication::processEvents();
    QCOMPARE(ic.openInputPanelReceived, false);
    QVERIFY(view.inputContext() == 0);
    QVERIFY(!view.testAttribute(Qt::WA_InputMethodEnabled));

    // no automatic input panel events should
    // be sent if activeFocusOnPress is false
    input.setFocusOnPress(false);
    QCOMPARE(focusOnPressSpy.count(),1);
    input.setFocusOnPress(false);
    QCOMPARE(focusOnPressSpy.count(),1);
    input.setFocus(false);
    input.setFocus(true);
    QTest::mousePress(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(input.scenePos()));
    QTest::mouseRelease(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(input.scenePos()));
    QApplication::processEvents();
    QCOMPARE(ic.openInputPanelReceived, false);
    QCOMPARE(ic.closeInputPanelReceived, false);

    // one show input panel event should
    // be set when openSoftwareInputPanel is called
    input.openSoftwareInputPanel();
    QCOMPARE(ic.openInputPanelReceived, true);
    QCOMPARE(ic.closeInputPanelReceived, false);
    ic.openInputPanelReceived = false;

    // one close input panel event should
    // be sent when closeSoftwareInputPanel is called
    input.closeSoftwareInputPanel();
    QCOMPARE(ic.openInputPanelReceived, false);
    QCOMPARE(ic.closeInputPanelReceived, true);
    ic.closeInputPanelReceived = false;

    // set activeFocusOnPress back to true
    input.setFocusOnPress(true);
    QCOMPARE(focusOnPressSpy.count(),2);
    input.setFocusOnPress(true);
    QCOMPARE(focusOnPressSpy.count(),2);
    input.setFocus(false);
    QApplication::processEvents();
    QCOMPARE(ic.openInputPanelReceived, false);
    QCOMPARE(ic.closeInputPanelReceived, false);
    ic.closeInputPanelReceived = false;

    // input panel should not re-open
    // if focus has already been set
    input.setFocus(true);
    QCOMPARE(ic.openInputPanelReceived, true);
    ic.openInputPanelReceived = false;
    input.setFocus(true);
    QCOMPARE(ic.openInputPanelReceived, false);

    // input method should be disabled
    // if TextInput loses focus
    input.setFocus(false);
    QApplication::processEvents();
    QVERIFY(view.inputContext() == 0);
    QVERIFY(!view.testAttribute(Qt::WA_InputMethodEnabled));

    // input method should not be enabled
    // if TextInput is read only.
    input.setReadOnly(true);
    ic.openInputPanelReceived = false;
    input.setFocus(true);
    QApplication::processEvents();
    QCOMPARE(ic.openInputPanelReceived, false);
    QVERIFY(view.inputContext() == 0);
    QVERIFY(!view.testAttribute(Qt::WA_InputMethodEnabled));
}
#endif // QT_NO_IM

class MyTextInput : public QDeclarativeTextInput
{
public:
    MyTextInput(QDeclarativeItem *parent = 0) : QDeclarativeTextInput(parent)
    {
        nbPaint = 0;
    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
       nbPaint++;
       QDeclarativeTextInput::paint(painter, option, widget);
    }
    int nbPaint;
};

void tst_qdeclarativetextinput::setHAlignClearCache()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    MyTextInput input;
    input.setText("Hello world");
    scene.addItem(&input);
    view.show();
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(input.nbPaint, 1);
    input.setHAlign(QDeclarativeTextInput::AlignRight);
    QApplication::processEvents();
    //Changing the alignment should trigger a repaint
    QCOMPARE(input.nbPaint, 2);
}

void tst_qdeclarativetextinput::focusOutClearSelection()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QDeclarativeTextInput input;
    QDeclarativeTextInput input2;
    input.setText(QLatin1String("Hello world"));
    input.setFocus(true);
    scene.addItem(&input2);
    scene.addItem(&input);
    view.show();
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    input.select(2,5);
    //The selection should work
    QTRY_COMPARE(input.selectedText(), QLatin1String("llo"));
    input2.setFocus(true);
    QApplication::processEvents();
    //The input lost the focus selection should be cleared
    QTRY_COMPARE(input.selectedText(), QLatin1String(""));
}

void tst_qdeclarativetextinput::geometrySignals()
{
    QDeclarativeComponent component(&engine, SRCDIR "/data/geometrySignals.qml");
    QObject *o = component.create();
    QVERIFY(o);
    QCOMPARE(o->property("bindingWidth").toInt(), 400);
    QCOMPARE(o->property("bindingHeight").toInt(), 500);
    delete o;
}

void tst_qdeclarativetextinput::testQtQuick11Attributes()
{
    QFETCH(QString, code);
    QFETCH(QString, warning);
    QFETCH(QString, error);

    QDeclarativeEngine engine;
    QObject *obj;

    QDeclarativeComponent valid(&engine);
    valid.setData("import QtQuick 1.1; TextInput { " + code.toUtf8() + " }", QUrl(""));
    obj = valid.create();
    QVERIFY(obj);
    QVERIFY(valid.errorString().isEmpty());
    delete obj;

    QDeclarativeComponent invalid(&engine);
    invalid.setData("import QtQuick 1.0; TextInput { " + code.toUtf8() + " }", QUrl(""));
    QTest::ignoreMessage(QtWarningMsg, warning.toUtf8());
    obj = invalid.create();
    QCOMPARE(invalid.errorString(), error);
    delete obj;
}

void tst_qdeclarativetextinput::testQtQuick11Attributes_data()
{
    QTest::addColumn<QString>("code");
    QTest::addColumn<QString>("warning");
    QTest::addColumn<QString>("error");

    QTest::newRow("canPaste") << "property bool foo: canPaste"
        << "<Unknown File>:1: ReferenceError: Can't find variable: canPaste"
        << "";

    QTest::newRow("moveCursorSelection") << "Component.onCompleted: moveCursorSelection(0, TextEdit.SelectCharacters)"
        << "<Unknown File>:1: ReferenceError: Can't find variable: moveCursorSelection"
        << "";

    QTest::newRow("deselect") << "Component.onCompleted: deselect()"
        << "<Unknown File>:1: ReferenceError: Can't find variable: deselect"
        << "";
}

#ifndef QT_NO_IM
void tst_qdeclarativetextinput::preeditAutoScroll()
{
    QString committedText = "super";
    QString preeditText = "califragisiticexpialidocious!";

    QGraphicsScene scene;
    QGraphicsView view(&scene);
    MyInputContext ic;
    view.setInputContext(&ic);
    QDeclarativeTextInput input;
    QFontMetricsF fm(input.font());
    input.setWidth(fm.width(committedText));
    input.setText(committedText);
    input.setPos(0, 0);
    input.setFocus(true);
    scene.addItem(&input);
    view.show();
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&view));

    QSignalSpy cursorRectangleSpy(&input, SIGNAL(cursorRectangleChanged()));
    int cursorRectangleChanges = 0;

    // test the text is scrolled so the preedit is visible.
    ic.sendPreeditText(preeditText.mid(0, 3), 1);
    QVERIFY(input.positionAt(0) != 0);
    QVERIFY(input.cursorRectangle().left() < input.boundingRect().width());
    QVERIFY(cursorRectangleSpy.count() > cursorRectangleChanges);
    cursorRectangleChanges = cursorRectangleSpy.count();

    // test the text is scrolled back when the preedit is removed.
    ic.sendEvent(QInputMethodEvent());
    QCOMPARE(input.positionAt(0), 0);
    QCOMPARE(input.positionAt(input.width()), 5);
    QVERIFY(cursorRectangleSpy.count() > cursorRectangleChanges);
    cursorRectangleChanges = cursorRectangleSpy.count();

    // some tolerance for different fonts.
#ifdef Q_OS_LINUX
    const int error = 2;
#else
    const int error = 5;
#endif

    // test if the preedit is larger than the text input that the
    // character preceding the cursor is still visible.
    qreal x = input.positionToRectangle(0).x();
    for (int i = 0; i < 3; ++i) {
        ic.sendPreeditText(preeditText, i + 1);
        QVERIFY(input.cursorRectangle().right() >= fm.width(preeditText.at(i)) - error);
        QVERIFY(input.positionToRectangle(0).x() < x);
        QVERIFY(cursorRectangleSpy.count() > cursorRectangleChanges);
        cursorRectangleChanges = cursorRectangleSpy.count();
        x = input.positionToRectangle(0).x();
    }
    for (int i = 1; i >= 0; --i) {
        ic.sendPreeditText(preeditText, i + 1);
        QVERIFY(input.cursorRectangle().right() >= fm.width(preeditText.at(i)) - error);
        QVERIFY(input.positionToRectangle(0).x() > x);
        QVERIFY(cursorRectangleSpy.count() > cursorRectangleChanges);
        cursorRectangleChanges = cursorRectangleSpy.count();
        x = input.positionToRectangle(0).x();
    }

    // Test incrementing the preedit cursor doesn't cause further
    // scrolling when right most text is visible.
    ic.sendPreeditText(preeditText, preeditText.length() - 3);
    QCOMPARE(cursorRectangleSpy.count(), ++cursorRectangleChanges);
    x = input.positionToRectangle(0).x();
    for (int i = 2; i >= 0; --i) {
        ic.sendPreeditText(preeditText, preeditText.length() - i);
        QCOMPARE(input.positionToRectangle(0).x(), x);
        QVERIFY(cursorRectangleSpy.count() > cursorRectangleChanges);
        cursorRectangleChanges = cursorRectangleSpy.count();
    }
    for (int i = 1; i <  3; ++i) {
        ic.sendPreeditText(preeditText, preeditText.length() - i);
        QCOMPARE(input.positionToRectangle(0).x(), x);
        QVERIFY(cursorRectangleSpy.count() > cursorRectangleChanges);
        cursorRectangleChanges = cursorRectangleSpy.count();
    }

    // Test disabling auto scroll.
    ic.sendEvent(QInputMethodEvent());

    input.setAutoScroll(false);
    ic.sendPreeditText(preeditText.mid(0, 3), 1);
    QCOMPARE(input.positionAt(0), 0);
    QCOMPARE(input.positionAt(input.width()), 5);

    ic.sendEvent(QInputMethodEvent());
    input.setAutoScroll(true);
    // Test committing pre-edit text at the start of the string. QTBUG-18789
    input.setCursorPosition(0);
    ic.sendPreeditText(input.text(), 5);
    QCOMPARE(input.positionAt(0), 0);

    QInputMethodEvent event;
    event.setCommitString(input.text());
    ic.sendEvent(event);

    QCOMPARE(input.positionAt(0), 0);
    QCOMPARE(input.positionAt(input.width()), 5);
}

void tst_qdeclarativetextinput::preeditMicroFocus()
{
    QString preeditText = "super";

    QGraphicsScene scene;
    QGraphicsView view(&scene);
    MyInputContext ic;
    view.setInputContext(&ic);
    QDeclarativeTextInput input;
    input.setPos(0, 0);
    input.setAutoScroll(false);
    input.setFocus(true);
    scene.addItem(&input);
    view.show();
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&view));

    QRect currentRect;
    QRect previousRect = input.inputMethodQuery(Qt::ImMicroFocus).toRect();

    // Verify that the micro focus rect is positioned the same for position 0 as
    // it would be if there was no preedit text.
    ic.updateReceived = false;
    ic.sendPreeditText(preeditText, 0);
    currentRect = input.inputMethodQuery(Qt::ImMicroFocus).toRect();
    QCOMPARE(currentRect, previousRect);
#if defined(Q_WS_X11) || defined(Q_WS_QWS) || defined(Q_OS_SYMBIAN)
    QCOMPARE(ic.updateReceived, true);
#endif

    // Verify that the micro focus rect moves to the left as the cursor position
    // is incremented.
    for (int i = 1; i <= 5; ++i) {
        ic.updateReceived = false;
        ic.sendPreeditText(preeditText, i);
        currentRect = input.inputMethodQuery(Qt::ImMicroFocus).toRect();
        QVERIFY(previousRect.left() < currentRect.left());
#if defined(Q_WS_X11) || defined(Q_WS_QWS) || defined(Q_OS_SYMBIAN)
        QCOMPARE(ic.updateReceived, true);
#endif
        previousRect = currentRect;
    }

    // Verify that if there is no preedit cursor then the micro focus rect is the
    // same as it would be if it were positioned at the end of the preedit text.
    ic.sendPreeditText(preeditText, 0);
    ic.updateReceived = false;
    ic.sendEvent(QInputMethodEvent(preeditText, QList<QInputMethodEvent::Attribute>()));
    currentRect = input.inputMethodQuery(Qt::ImMicroFocus).toRect();
    QCOMPARE(currentRect, previousRect);
#if defined(Q_WS_X11) || defined(Q_WS_QWS) || defined(Q_OS_SYMBIAN)
    QCOMPARE(ic.updateReceived, true);
#endif
}

void tst_qdeclarativetextinput::inputContextMouseHandler()
{
    QString text = "supercalifragisiticexpialidocious!";

    QGraphicsScene scene;
    QGraphicsView view(&scene);
    MyInputContext ic;
    view.setInputContext(&ic);
    QDeclarativeTextInput input;
    input.setWidth(200);
    input.setText(text.mid(0, 12));
    input.setCursorPosition(12);
    input.setPos(0, 0);
    input.setFocus(true);
    scene.addItem(&input);
    view.show();
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&view));

    QFontMetricsF fm(input.font());
    const qreal y = fm.height() / 2;

    QPoint position2 = view.mapFromScene(input.mapToScene(QPointF(fm.width(text.mid(0, 2)), y)));
    QPoint position8 = view.mapFromScene(input.mapToScene(QPointF(fm.width(text.mid(0, 8)), y)));
    QPoint position20 = view.mapFromScene(input.mapToScene(QPointF(fm.width(text.mid(0, 20)), y)));
    QPoint position27 = view.mapFromScene(input.mapToScene(QPointF(fm.width(text.mid(0, 27)), y)));
    QPoint globalPosition2 = view.viewport()->mapToGlobal(position2);
    QPoint globalposition8 = view.viewport()->mapToGlobal(position8);
    QPoint globalposition20 = view.viewport()->mapToGlobal(position20);
    QPoint globalposition27 = view.viewport()->mapToGlobal(position27);

    ic.sendEvent(QInputMethodEvent(text.mid(12), QList<QInputMethodEvent::Attribute>()));

    QTest::mouseDClick(view.viewport(), Qt::LeftButton, Qt::NoModifier, position2);
    QCOMPARE(ic.eventType, QEvent::MouseButtonDblClick);
    QCOMPARE(ic.eventPosition, position2);
    QCOMPARE(ic.eventGlobalPosition, globalPosition2);
    QCOMPARE(ic.eventButton, Qt::LeftButton);
    QCOMPARE(ic.eventModifiers, Qt::NoModifier);
    QVERIFY(ic.cursor < 0);
    ic.eventType = QEvent::None;

    QTest::mousePress(view.viewport(), Qt::LeftButton, Qt::NoModifier, position2);
    QCOMPARE(ic.eventType, QEvent::MouseButtonPress);
    QCOMPARE(ic.eventPosition, position2);
    QCOMPARE(ic.eventGlobalPosition, globalPosition2);
    QCOMPARE(ic.eventButton, Qt::LeftButton);
    QCOMPARE(ic.eventModifiers, Qt::NoModifier);
    QVERIFY(ic.cursor < 0);
    ic.eventType = QEvent::None;

    {   QMouseEvent mv(QEvent::MouseMove, position8, globalposition8, Qt::LeftButton, Qt::LeftButton,Qt::NoModifier);
        QApplication::sendEvent(view.viewport(), &mv); }
    QCOMPARE(ic.eventType, QEvent::None);

    {   QMouseEvent mv(QEvent::MouseMove, position27, globalposition27, Qt::LeftButton, Qt::LeftButton,Qt::NoModifier);
        QApplication::sendEvent(view.viewport(), &mv); }
    QCOMPARE(ic.eventType, QEvent::MouseMove);
    QCOMPARE(ic.eventPosition, position27);
    QCOMPARE(ic.eventGlobalPosition, globalposition27);
    QCOMPARE(ic.eventButton, Qt::LeftButton);
    QCOMPARE(ic.eventModifiers, Qt::NoModifier);
    QVERIFY(ic.cursor >= 14 && ic.cursor <= 16);    // 15 is expected but some platforms may be off by one.
    ic.eventType = QEvent::None;

    QTest::mouseRelease(view.viewport(), Qt::LeftButton, Qt::NoModifier, position27);
    QCOMPARE(ic.eventType, QEvent::MouseButtonRelease);
    QCOMPARE(ic.eventPosition, position27);
    QCOMPARE(ic.eventGlobalPosition, globalposition27);
    QCOMPARE(ic.eventButton, Qt::LeftButton);
    QCOMPARE(ic.eventModifiers, Qt::NoModifier);
    QVERIFY(ic.cursor >= 14 && ic.cursor <= 16);
    ic.eventType = QEvent::None;

    // And in the other direction.
    QTest::mouseDClick(view.viewport(), Qt::LeftButton, Qt::ControlModifier, position27);
    QCOMPARE(ic.eventType, QEvent::MouseButtonDblClick);
    QCOMPARE(ic.eventPosition, position27);
    QCOMPARE(ic.eventGlobalPosition, globalposition27);
    QCOMPARE(ic.eventButton, Qt::LeftButton);
    QCOMPARE(ic.eventModifiers, Qt::ControlModifier);
    QVERIFY(ic.cursor >= 14 && ic.cursor <= 16);
    ic.eventType = QEvent::None;

    QTest::mousePress(view.viewport(), Qt::RightButton, Qt::ControlModifier, position27);
    QCOMPARE(ic.eventType, QEvent::MouseButtonPress);
    QCOMPARE(ic.eventPosition, position27);
    QCOMPARE(ic.eventGlobalPosition, globalposition27);
    QCOMPARE(ic.eventButton, Qt::RightButton);
    QCOMPARE(ic.eventModifiers, Qt::ControlModifier);
    QVERIFY(ic.cursor >= 14 && ic.cursor <= 16);
    ic.eventType = QEvent::None;

    {   QMouseEvent mv(QEvent::MouseMove, position20, globalposition20, Qt::RightButton, Qt::RightButton,Qt::ControlModifier);
        QApplication::sendEvent(view.viewport(), &mv); }
    QCOMPARE(ic.eventType, QEvent::MouseMove);
    QCOMPARE(ic.eventPosition, position20);
    QCOMPARE(ic.eventGlobalPosition, globalposition20);
    QCOMPARE(ic.eventButton, Qt::RightButton);
    QCOMPARE(ic.eventModifiers, Qt::ControlModifier);
    QVERIFY(ic.cursor >= 7 && ic.cursor <= 9);
    ic.eventType = QEvent::None;

    {   QMouseEvent mv(QEvent::MouseMove, position2, globalPosition2, Qt::RightButton, Qt::RightButton,Qt::ControlModifier);
        QApplication::sendEvent(view.viewport(), &mv); }
    QCOMPARE(ic.eventType, QEvent::None);

    QTest::mouseRelease(view.viewport(), Qt::RightButton, Qt::ControlModifier, position2);
    QCOMPARE(ic.eventType, QEvent::MouseButtonRelease);
    QCOMPARE(ic.eventPosition, position2);
    QCOMPARE(ic.eventGlobalPosition, globalPosition2);
    QCOMPARE(ic.eventButton, Qt::RightButton);
    QCOMPARE(ic.eventModifiers, Qt::ControlModifier);
    QVERIFY(ic.cursor < 0);
    ic.eventType = QEvent::None;
}

void tst_qdeclarativetextinput::inputMethodComposing()
{
    QString text = "supercalifragisiticexpialidocious!";

    QGraphicsScene scene;
    QGraphicsView view(&scene);
    MyInputContext ic;
    view.setInputContext(&ic);
    QDeclarativeTextInput input;
    input.setWidth(200);
    input.setText(text.mid(0, 12));
    input.setCursorPosition(12);
    input.setPos(0, 0);
    input.setFocus(true);
    scene.addItem(&input);
    view.show();
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&view));

    QSignalSpy spy(&input, SIGNAL(inputMethodComposingChanged()));

    QCOMPARE(input.isInputMethodComposing(), false);

    ic.sendEvent(QInputMethodEvent(text.mid(3), QList<QInputMethodEvent::Attribute>()));
    QCOMPARE(input.isInputMethodComposing(), true);
    QCOMPARE(spy.count(), 1);

    ic.sendEvent(QInputMethodEvent(text.mid(12), QList<QInputMethodEvent::Attribute>()));
    QCOMPARE(input.isInputMethodComposing(), true);
    QCOMPARE(spy.count(), 1);

    ic.sendEvent(QInputMethodEvent());
    QCOMPARE(input.isInputMethodComposing(), false);
    QCOMPARE(spy.count(), 2);
}
#endif // QT_NO_IM

void tst_qdeclarativetextinput::cursorRectangleSize()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/positionAt.qml");
    QVERIFY(canvas->rootObject() != 0);
    canvas->show();
    canvas->setFocus();
    QApplication::setActiveWindow(canvas);
    QTest::qWaitForWindowShown(canvas);

    QDeclarativeTextInput *textInput = qobject_cast<QDeclarativeTextInput *>(canvas->rootObject());
    QVERIFY(textInput != 0);
    textInput->setFocus(Qt::OtherFocusReason);
    QRectF cursorRect = textInput->positionToRectangle(textInput->cursorPosition());
    QRectF microFocusFromScene = canvas->scene()->inputMethodQuery(Qt::ImMicroFocus).toRectF();
    QRectF microFocusFromApp= QApplication::focusWidget()->inputMethodQuery(Qt::ImMicroFocus).toRectF();

    QCOMPARE(microFocusFromScene.size(), cursorRect.size());
    QCOMPARE(microFocusFromApp.size(), cursorRect.size());
}

void tst_qdeclarativetextinput::deselect()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/positionAt.qml");
    QVERIFY(canvas->rootObject() != 0);
    canvas->show();
    canvas->setFocus();
    QApplication::setActiveWindow(canvas);
    QTest::qWaitForWindowShown(canvas);

    QDeclarativeTextInput *textInput = qobject_cast<QDeclarativeTextInput *>(canvas->rootObject());
    QVERIFY(textInput != 0);

    textInput->setText("Select");

    QSignalSpy selectionStartSpy(textInput, SIGNAL(selectionStartChanged()));
    QSignalSpy selectionEndSpy(textInput, SIGNAL(selectionEndChanged()));
    QSignalSpy selectedTextSpy(textInput, SIGNAL(selectedTextChanged()));

    textInput->select(5, 6);

    QCOMPARE(selectionStartSpy.count(), 1);
    QCOMPARE(selectionEndSpy.count(), 0);
    QCOMPARE(selectedTextSpy.count(), 1);
    QCOMPARE(textInput->selectionStart(), 5);
    QCOMPARE(textInput->selectionEnd(), 6);
    QCOMPARE(textInput->selectedText(), QLatin1String("t"));
    QCOMPARE(textInput->cursorPosition(), 6);

    textInput->deselect();

    QCOMPARE(selectionStartSpy.count(), 2);
    QCOMPARE(selectionEndSpy.count(), 1);
    QCOMPARE(selectedTextSpy.count(), 2);
    QCOMPARE(textInput->selectionStart(), textInput->cursorPosition());
    QCOMPARE(textInput->selectionEnd(), textInput->cursorPosition());
    QCOMPARE(textInput->selectedText(), QLatin1String(""));
    QCOMPARE(textInput->cursorPosition(), 6);

    textInput->select(5, 6);

    QCOMPARE(selectionStartSpy.count(), 3);
    QCOMPARE(selectionEndSpy.count(), 1);
    QCOMPARE(selectedTextSpy.count(), 3);
    QCOMPARE(textInput->selectionStart(), 5);
    QCOMPARE(textInput->selectionEnd(), 6);
    QCOMPARE(textInput->selectedText(), QLatin1String("t"));
    QCOMPARE(textInput->cursorPosition(), 6);

    QKeyEvent leftArrowPress(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier);
    QKeyEvent leftArrowRelese(QEvent::KeyRelease, Qt::Key_Left, Qt::NoModifier);
    QApplication::sendEvent(canvas, &leftArrowPress);
    QApplication::sendEvent(canvas, &leftArrowRelese);

    QCOMPARE(selectionStartSpy.count(), 4);
    QCOMPARE(selectionEndSpy.count(), 2);
    QCOMPARE(selectedTextSpy.count(), 4);
    QCOMPARE(textInput->selectionStart(), textInput->cursorPosition());
    QCOMPARE(textInput->selectionEnd(), textInput->cursorPosition());
    QCOMPARE(textInput->selectedText(), QLatin1String(""));
    QCOMPARE(textInput->cursorPosition(), 5);

    textInput->select(5, 6);

    QCOMPARE(selectionStartSpy.count(), 4);
    QCOMPARE(selectionEndSpy.count(), 3);
    QCOMPARE(selectedTextSpy.count(), 5);
    QCOMPARE(textInput->selectionStart(), 5);
    QCOMPARE(textInput->selectionEnd(), 6);
    QCOMPARE(textInput->selectedText(), QLatin1String("t"));
    QCOMPARE(textInput->cursorPosition(), 6);

    QList<QInputMethodEvent::Attribute> attributes;
    attributes << QInputMethodEvent::Attribute(QInputMethodEvent::Selection, 0, 0, QVariant());
    QInputMethodEvent event(QLatin1String(""), attributes);
    QApplication::sendEvent(canvas, &event);

    QCOMPARE(selectionStartSpy.count(), 5);
    QCOMPARE(selectionEndSpy.count(), 4);
    QCOMPARE(selectedTextSpy.count(), 6);
    QCOMPARE(textInput->selectionStart(), textInput->cursorPosition());
    QCOMPARE(textInput->selectionEnd(), textInput->cursorPosition());
    QCOMPARE(textInput->selectedText(), QLatin1String(""));
    QCOMPARE(textInput->cursorPosition(), 0);

    textInput->setCursorPosition(1);

    QCOMPARE(selectionStartSpy.count(), 6);
    QCOMPARE(selectionEndSpy.count(), 5);
    QCOMPARE(selectedTextSpy.count(), 6);

    QKeyEvent leftArrowShiftPress(QEvent::KeyPress, Qt::Key_Left, Qt::ShiftModifier);
    QKeyEvent leftArrowShiftRelese(QEvent::KeyRelease, Qt::Key_Left, Qt::ShiftModifier);
    QApplication::sendEvent(canvas, &leftArrowShiftPress);
    QApplication::sendEvent(canvas, &leftArrowShiftRelese);

    QCOMPARE(selectionStartSpy.count(), 7);
    QCOMPARE(selectionEndSpy.count(), 5);
    QCOMPARE(selectedTextSpy.count(), 7);
    QCOMPARE(textInput->selectionStart(), 0);
    QCOMPARE(textInput->selectionEnd(), 1);
    QCOMPARE(textInput->selectedText(), QLatin1String("S"));
    QCOMPARE(textInput->cursorPosition(), 0);

    QApplication::sendEvent(canvas, &event);

    QCOMPARE(selectionStartSpy.count(), 8);
    QCOMPARE(selectionEndSpy.count(), 6);
    QCOMPARE(selectedTextSpy.count(), 8);
    QCOMPARE(textInput->selectionStart(), textInput->cursorPosition());
    QCOMPARE(textInput->selectionEnd(), textInput->cursorPosition());
    QCOMPARE(textInput->selectedText(), QLatin1String(""));
    QCOMPARE(textInput->cursorPosition(), 0);
}

QTEST_MAIN(tst_qdeclarativetextinput)

#include "tst_qdeclarativetextinput.moc"

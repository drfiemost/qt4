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


#include "uiloader.h"

#include <QtTest/QtTest>
#include <QApplication>
#include <QDir>

#ifdef Q_OS_SYMBIAN
#define SRCDIR ""
#endif

class uiLoaderAutotest: public QObject
{

Q_OBJECT

public slots:
    void init();

private slots:
    void imageDiffTest();
    
private:
    QString currentDir;
        
};



void uiLoaderAutotest::init()
{
    currentDir = QDir::currentPath();
    QDir::setCurrent(QString(SRCDIR) + QString("/.."));
}

void uiLoaderAutotest::imageDiffTest()
{
    //QApplication app(argc, argv);

    QString pathToProgram   = currentDir + "/tst_screenshot/tst_screenshot";

#ifdef Q_WS_MAC
    pathToProgram += ".app/Contents/MacOS/tst_screenshot";
#endif

#ifdef Q_WS_WIN
    pathToProgram += ".exe";
#endif
    uiLoader wrapper(pathToProgram);
    QString errorMessage;
    switch(wrapper.runAutoTests(&errorMessage)) {
        case uiLoader::TestRunDone:
        break;
        case uiLoader::TestConfigError:
        QVERIFY2(false, qPrintable(errorMessage));
        break;
        case uiLoader::TestNoConfig:
        QSKIP(qPrintable(errorMessage), SkipAll);
        break;
    }
}

QTEST_MAIN(uiLoaderAutotest)
#include "tst_uiloader.moc"

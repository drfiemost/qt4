/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtTest module of the Qt Toolkit.
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

#ifndef QTESTBASICSTREAMER_H
#define QTESTBASICSTREAMER_H

#include <QtCore/qglobal.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QTestElement;
class QTestElementAttribute;
class QTestLogger;
struct QTestCharBuffer;

class QTestBasicStreamer
{
    public:
        QTestBasicStreamer();
        virtual ~QTestBasicStreamer();

        virtual void output(QTestElement *element) const;

        void outputString(const char *msg) const;

        void setLogger(const QTestLogger *tstLogger);
        const QTestLogger *logger() const;

    protected:
        virtual void formatStart(const QTestElement *element, QTestCharBuffer *formatted) const;
        virtual void formatEnd(const QTestElement *element, QTestCharBuffer *formatted) const;
        virtual void formatBeforeAttributes(const QTestElement *element, QTestCharBuffer *formatted) const;
        virtual void formatAfterAttributes(const QTestElement *element, QTestCharBuffer *formatted) const;
        virtual void formatAttributes(const QTestElement *element, const QTestElementAttribute *attribute, QTestCharBuffer *formatted) const;
        virtual void outputElements(QTestElement *element, bool isChildElement = false) const;
        virtual void outputElementAttributes(const QTestElement *element, QTestElementAttribute *attribute) const;

    private:
        const QTestLogger *testLogger;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif

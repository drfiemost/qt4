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

#ifndef QABSTRACTTESTLOGGER_P_H
#define QABSTRACTTESTLOGGER_P_H

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

#include <qglobal.h>

#include <stdlib.h>

QT_BEGIN_NAMESPACE

class QBenchmarkResult;

class QAbstractTestLogger
{
public:
    enum IncidentTypes {
        Pass,
        XFail,
        Fail,
        XPass
    };

    enum MessageTypes {
        Warn,
        QWarning,
        QDebug,
        QSystem,
        QFatal,
        Skip,
        Info
    };

    QAbstractTestLogger() = default;
    virtual ~QAbstractTestLogger() = default;

    virtual void startLogging(const char *filename);
    virtual void stopLogging();

    virtual void enterTestFunction(const char *function) = 0;
    virtual void leaveTestFunction() = 0;

    virtual void addIncident(IncidentTypes type, const char *description,
                             const char *file = nullptr, int line = 0) = 0;
    virtual void addBenchmarkResult(const QBenchmarkResult &result) = 0;

    virtual void addMessage(MessageTypes type, const char *message,
                            const char *file = nullptr, int line = 0) = 0;

    virtual void registerRandomSeed(unsigned int seed) = 0;

    static void outputString(const char *msg);
};

struct QTestCharBuffer
{
    enum { InitialSize = 512 };

    inline QTestCharBuffer()
            : _size(InitialSize), buf(staticBuf)
    {
        staticBuf[0] = '\0';
    }

    inline ~QTestCharBuffer()
    {
        if (buf != staticBuf)
            free(buf);
    }

    inline char *data()
    {
        return buf;
    }

    inline char **buffer()
    {
        return &buf;
    }

    inline const char* constData() const
    {
        return buf;
    }

    inline int size() const
    {
        return _size;
    }

    inline bool reset(int newSize)
    {
        char *newBuf = nullptr;
        if (buf == staticBuf) {
            // if we point to our internal buffer, we need to malloc first
            newBuf = reinterpret_cast<char *>(malloc(newSize));
        } else {
            // if we already malloc'ed, just realloc
            newBuf = reinterpret_cast<char *>(realloc(buf, newSize));
        }

        // if the allocation went wrong (newBuf == 0), we leave the object as is
        if (!newBuf)
            return false;

        _size = newSize;
        buf = newBuf;
        return true;
    }

private:
    int _size;
    char* buf;
    char staticBuf[InitialSize];
};

namespace QTest
{
    int qt_asprintf(QTestCharBuffer *buf, const char *format, ...);
}


QT_END_NAMESPACE

#endif

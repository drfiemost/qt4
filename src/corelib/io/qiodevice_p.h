/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QIODEVICE_P_H
#define QIODEVICE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of QIODevice. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "QtCore/qiodevice.h"
#include "QtCore/qbytearray.h"
#include "QtCore/qobjectdefs.h"
#include "QtCore/qstring.h"
#include "private/qringbuffer_p.h"
#ifndef QT_NO_QOBJECT
#include "private/qobject_p.h"
#endif

QT_BEGIN_NAMESPACE

#ifndef QIODEVICE_BUFFERSIZE
#define QIODEVICE_BUFFERSIZE Q_INT64_C(16384)
#endif

// This is QIODevice's read buffer, optimized for read(), isEmpty() and getChar()
class QIODevicePrivateLinearBuffer
{
public:
    QIODevicePrivateLinearBuffer(int) : len(0), first(nullptr), buf(nullptr), capacity(0) {
    }
    ~QIODevicePrivateLinearBuffer() {
        delete [] buf;
    }
    void clear() {
        first = buf;
        len = 0;
    }
    qint64 size() const {
        return len;
    }
    bool isEmpty() const {
        return len == 0;
    }
    void skip(qint64 n) {
        if (n >= len) {
            clear();
        } else {
            len -= n;
            first += n;
        }
    }
    int getChar() {
        if (len == 0)
            return -1;
        int ch = uchar(*first);
        len--;
        first++;
        return ch;
    }
    int read(char* target, qint64 size) {
        int r = std::min(size, len);

        if (r == 0) {
            return 0;
        }

        memcpy(target, first, r);
        len -= r;
        first += r;
        return r;
    }
    int peek(char* target, qint64 size) {
        int r = std::min(size, len);
        memcpy(target, first, r);
        return r;
    }
    char* reserve(int size) {
        makeSpace(size + len, freeSpaceAtEnd);
        char* writePtr = first + len;
        len += size;
        return writePtr;
    }
    void chop(qint64 size) {
        if (size >= len) {
            clear();
        } else {
            len -= size;
        }
    }
    QByteArray readAll() {
        char* f = first;
        int l = len;
        clear();
        return QByteArray(f, l);
    }
    int readLine(char* target, qint64 size) {
        int r = std::min(size, len);
        char* eol = static_cast<char*>(memchr(first, '\n', r));
        if (eol)
            r = 1+(eol-first);
        memcpy(target, first, r);
        len -= r;
        first += r;
        return int(r);
        }
    bool canReadLine() const {
        return memchr(first, '\n', len);
    }
    void ungetChar(char c) {
        if (first == buf) {
            // underflow, the existing valid data needs to move to the end of the (potentially bigger) buffer
            makeSpace(len+1, freeSpaceAtStart);
        }
        first--;
        len++;
        *first = c;
    }
    void ungetBlock(const char* block, int size) {
        if ((first - buf) < size) {
            // underflow, the existing valid data needs to move to the end of the (potentially bigger) buffer
            makeSpace(len + size, freeSpaceAtStart);
        }
        first -= size;
        len += size;
        memcpy(first, block, size);
    }

private:
    enum FreeSpacePos {freeSpaceAtStart, freeSpaceAtEnd};
    void makeSpace(size_t required, FreeSpacePos where) {
        size_t newCapacity = std::max(capacity, size_t(QIODEVICE_BUFFERSIZE));
        while (newCapacity < required)
            newCapacity *= 2;
        int moveOffset = (where == freeSpaceAtEnd) ? 0 : newCapacity - len;
        if (newCapacity > capacity) {
            // allocate more space
            char* newBuf = new char[newCapacity];
            if (first != nullptr && len != 0) {
                memmove(newBuf + moveOffset, first, len);
            }
            delete [] buf;
            buf = newBuf;
            capacity = newCapacity;
        } else {
            // shift any existing data to make space
            memmove(buf + moveOffset, first, len);
        }
        first = buf + moveOffset;
    }

private:
    // length of the unread data
    qint64 len;
    // start of the unread data
    char* first;
    // the allocated buffer
    char* buf;
    // allocated buffer size
    size_t capacity;
};

class Q_CORE_EXPORT QIODevicePrivate
#ifndef QT_NO_QOBJECT
    : public QObjectPrivate
#endif
{
    Q_DECLARE_PUBLIC(QIODevice)

public:
    QIODevicePrivate();
    virtual ~QIODevicePrivate();

    QIODevice::OpenMode openMode;
    QString errorString;

    QIODevicePrivateLinearBuffer buffer;
    qint64 pos;
    qint64 devicePos;
    // these three are for fast position updates during read, avoiding isSequential test
    qint64 seqDumpPos;
    qint64 *pPos;
    qint64 *pDevicePos;
    bool baseReadLineDataCalled;
    bool firstRead;

    virtual bool putCharHelper(char c);

    enum AccessMode {
        Unset,
        Sequential,
        RandomAccess
    };
    mutable AccessMode accessMode;
    inline bool isSequential() const
    {
        if (accessMode == Unset)
            accessMode = q_func()->isSequential() ? Sequential : RandomAccess;
        return accessMode == Sequential;
    }

    virtual qint64 peek(char *data, qint64 maxSize);
    virtual QByteArray peek(qint64 maxSize);

#ifdef QT_NO_QOBJECT
    QIODevice *q_ptr;
#endif
};

QT_END_NAMESPACE

#endif // QIODEVICE_P_H

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

#ifndef QFILE_H
#define QFILE_H

#include <QtCore/qiodevice.h>
#include <QtCore/qstring.h>
#include <stdio.h>

#ifdef open
#error qfile.h must be included before any header file that defines open
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QAbstractFileEngine;
class QFilePrivate;

class Q_CORE_EXPORT QFile : public QIODevice
{
#ifndef QT_NO_QOBJECT
    Q_OBJECT
#endif
    Q_DECLARE_PRIVATE(QFile)

public:

    enum FileError {
        NoError = 0,
        ReadError = 1,
        WriteError = 2,
        FatalError = 3,
        ResourceError = 4,
        OpenError = 5,
        AbortError = 6,
        TimeOutError = 7,
        UnspecifiedError = 8,
        RemoveError = 9,
        RenameError = 10,
        PositionError = 11,
        ResizeError = 12,
        PermissionsError = 13,
        CopyError = 14
    };

    enum Permission {
        ReadOwner = 0x4000, WriteOwner = 0x2000, ExeOwner = 0x1000,
        ReadUser  = 0x0400, WriteUser  = 0x0200, ExeUser  = 0x0100,
        ReadGroup = 0x0040, WriteGroup = 0x0020, ExeGroup = 0x0010,
        ReadOther = 0x0004, WriteOther = 0x0002, ExeOther = 0x0001
    };
    Q_DECLARE_FLAGS(Permissions, Permission)

    enum FileHandleFlag {
        AutoCloseHandle = 0x0001,
        DontCloseHandle = 0
    };
    Q_DECLARE_FLAGS(FileHandleFlags, FileHandleFlag)

    QFile();
    QFile(const QString &name);
#ifndef QT_NO_QOBJECT
    explicit QFile(QObject *parent);
    QFile(const QString &name, QObject *parent);
#endif
    ~QFile() override;

    FileError error() const;
    void unsetError();

    QString fileName() const;
    void setFileName(const QString &name);

    typedef QByteArray (*EncoderFn)(const QString &fileName);
    typedef QString (*DecoderFn)(const QByteArray &localfileName);
    static QByteArray encodeName(const QString &fileName);
    static QString decodeName(const QByteArray &localFileName);
    inline static QString decodeName(const char *localFileName)
        { return decodeName(QByteArray(localFileName)); }
    static void setEncodingFunction(EncoderFn);
    static void setDecodingFunction(DecoderFn);

    bool exists() const;
    static bool exists(const QString &fileName);

    QString readLink() const;
    static QString readLink(const QString &fileName);
    inline QString symLinkTarget() const { return readLink(); }
    inline static QString symLinkTarget(const QString &fileName) { return readLink(fileName); }

    bool remove();
    static bool remove(const QString &fileName);

    bool rename(const QString &newName);
    static bool rename(const QString &oldName, const QString &newName);

    bool link(const QString &newName);
    static bool link(const QString &oldname, const QString &newName);

    bool copy(const QString &newName);
    static bool copy(const QString &fileName, const QString &newName);

    bool isSequential() const override;

    bool open(OpenMode flags) override;
    bool open(FILE *f, OpenMode ioFlags, FileHandleFlags handleFlags=DontCloseHandle);
    bool open(int fd, OpenMode ioFlags, FileHandleFlags handleFlags=DontCloseHandle);
    void close() override;

    qint64 size() const override;
    qint64 pos() const override;
    bool seek(qint64 offset) override;
    bool atEnd() const override;
    bool flush();

    bool resize(qint64 sz);
    static bool resize(const QString &filename, qint64 sz);

    Permissions permissions() const;
    static Permissions permissions(const QString &filename);
    bool setPermissions(Permissions permissionSpec);
    static bool setPermissions(const QString &filename, Permissions permissionSpec);

    int handle() const;

    enum MemoryMapFlags {
        NoOptions = 0
    };

    uchar *map(qint64 offset, qint64 size, MemoryMapFlags flags = NoOptions);
    bool unmap(uchar *address);

    virtual QAbstractFileEngine *fileEngine() const;

protected:
#ifdef QT_NO_QOBJECT
    QFile(QFilePrivate &dd);
#else
    QFile(QFilePrivate &dd, QObject *parent = nullptr);
#endif

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;
    qint64 readLineData(char *data, qint64 maxlen) override;

private:
    Q_DISABLE_COPY(QFile)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QFile::Permissions)

QT_END_NAMESPACE

QT_END_HEADER

#endif // QFILE_H

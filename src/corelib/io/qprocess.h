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

#ifndef QPROCESS_H
#define QPROCESS_H

#include <QtCore/qiodevice.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qshareddata.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


#ifndef QT_NO_PROCESS

#if (!defined(Q_OS_WIN32)) || defined(qdoc)
typedef qint64 Q_PID;
#else
QT_END_NAMESPACE
typedef struct _PROCESS_INFORMATION *Q_PID;
QT_BEGIN_NAMESPACE
#endif

class QProcessPrivate;
class QProcessEnvironmentPrivate;

class Q_CORE_EXPORT QProcessEnvironment
{
public:
    QProcessEnvironment();
    QProcessEnvironment(const QProcessEnvironment &other);
    ~QProcessEnvironment();
    QProcessEnvironment &operator=(const QProcessEnvironment &other);

    inline void swap(QProcessEnvironment &other) { qSwap(d, other.d); }

    bool operator==(const QProcessEnvironment &other) const;
    inline bool operator!=(const QProcessEnvironment &other) const
    { return !(*this == other); }

    bool isEmpty() const;
    void clear();

    bool contains(const QString &name) const;
    void insert(const QString &name, const QString &value);
    void remove(const QString &name);
    QString value(const QString &name, const QString &defaultValue = QString()) const;

    QStringList toStringList() const;

    QStringList keys() const;

    void insert(const QProcessEnvironment &e);

    static QProcessEnvironment systemEnvironment();

private:
    friend class QProcessPrivate;
    friend class QProcessEnvironmentPrivate;
    QSharedDataPointer<QProcessEnvironmentPrivate> d;
};

class Q_CORE_EXPORT QProcess : public QIODevice
{
    Q_OBJECT
public:
    enum ProcessError {
        FailedToStart, //### file not found, resource error
        Crashed,
        Timedout,
        ReadError,
        WriteError,
        UnknownError
    };
    enum ProcessState {
        NotRunning,
        Starting,
        Running
    };
    enum ProcessChannel {
        StandardOutput,
        StandardError
    };
    enum ProcessChannelMode {
        SeparateChannels,
        MergedChannels,
        ForwardedChannels
    };
    enum ExitStatus {
        NormalExit,
        CrashExit
    };

    explicit QProcess(QObject *parent = nullptr);
    ~QProcess() override;

    void start(const QString &program, const QStringList &arguments, OpenMode mode = ReadWrite);
    void start(const QString &program, OpenMode mode = ReadWrite);

    ProcessChannelMode readChannelMode() const;
    void setReadChannelMode(ProcessChannelMode mode);
    ProcessChannelMode processChannelMode() const;
    void setProcessChannelMode(ProcessChannelMode mode);

    ProcessChannel readChannel() const;
    void setReadChannel(ProcessChannel channel);

    void closeReadChannel(ProcessChannel channel);
    void closeWriteChannel();

    void setStandardInputFile(const QString &fileName);
    void setStandardOutputFile(const QString &fileName, OpenMode mode = Truncate);
    void setStandardErrorFile(const QString &fileName, OpenMode mode = Truncate);
    void setStandardOutputProcess(QProcess *destination);

    QString workingDirectory() const;
    void setWorkingDirectory(const QString &dir);

    void setEnvironment(const QStringList &environment);
    QStringList environment() const;
    void setProcessEnvironment(const QProcessEnvironment &environment);
    QProcessEnvironment processEnvironment() const;

    QProcess::ProcessError error() const;
    QProcess::ProcessState state() const;

    // #### Qt 5: Q_PID is a pointer on Windows and a value on Unix
    Q_PID pid() const;

    bool waitForStarted(int msecs = 30000);
    bool waitForReadyRead(int msecs = 30000) override;
    bool waitForBytesWritten(int msecs = 30000) override;
    bool waitForFinished(int msecs = 30000);

    QByteArray readAllStandardOutput();
    QByteArray readAllStandardError();

    int exitCode() const;
    QProcess::ExitStatus exitStatus() const;

    // QIODevice
    qint64 bytesAvailable() const override;
    qint64 bytesToWrite() const override;
    bool isSequential() const override;
    bool canReadLine() const override;
    void close() override;
    bool atEnd() const override;

    static int execute(const QString &program, const QStringList &arguments);
    static int execute(const QString &program);

    static bool startDetached(const QString &program, const QStringList &arguments, const QString &workingDirectory,
                              qint64 *pid = nullptr);
    static bool startDetached(const QString &program, const QStringList &arguments);
    static bool startDetached(const QString &program);

    static QStringList systemEnvironment();

public Q_SLOTS:
    void terminate();
    void kill();

Q_SIGNALS:
    void started();
    void finished(int exitCode);
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void error(QProcess::ProcessError error);
    void stateChanged(QProcess::ProcessState state);

    void readyReadStandardOutput();
    void readyReadStandardError();

protected:
    void setProcessState(ProcessState state);

    virtual void setupChildProcess();

    // QIODevice
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

private:
    Q_DECLARE_PRIVATE(QProcess)
    Q_DISABLE_COPY(QProcess)

    Q_PRIVATE_SLOT(d_func(), bool _q_canReadStandardOutput())
    Q_PRIVATE_SLOT(d_func(), bool _q_canReadStandardError())
    Q_PRIVATE_SLOT(d_func(), bool _q_canWrite())
    Q_PRIVATE_SLOT(d_func(), bool _q_startupNotification())
    Q_PRIVATE_SLOT(d_func(), bool _q_processDied())
    Q_PRIVATE_SLOT(d_func(), void _q_notified())
    friend class QProcessManager;
};

#endif // QT_NO_PROCESS

QT_END_NAMESPACE

QT_END_HEADER

#endif // QPROCESS_H

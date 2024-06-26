/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtNetwork module of the Qt Toolkit.
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

#ifndef QNET_UNIX_P_H
#define QNET_UNIX_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of Qt code on Unix. This header file may change from version to
// version to version without notice, or even be removed.
//
// We mean it.
//

#include "private/qcore_unix_p.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// for inet_addr
#include <netdb.h>
#include <arpa/inet.h>
#include <resolv.h>

QT_BEGIN_NAMESPACE

// Almost always the same. If not, specify in qplatformdefs.h.
#if !defined(QT_SOCKOPTLEN_T)
# define QT_SOCKOPTLEN_T QT_SOCKLEN_T
#endif

// UnixWare 7 redefines socket -> _socket
static inline int qt_safe_socket(int domain, int type, int protocol, int flags = 0)
{
    Q_ASSERT((flags & ~O_NONBLOCK) == 0);

    int fd;
#if defined(SOCK_CLOEXEC) && defined(SOCK_NONBLOCK)
    int newtype = type | SOCK_CLOEXEC;
    if (flags & O_NONBLOCK)
        newtype |= SOCK_NONBLOCK;
    fd = ::socket(domain, newtype, protocol);
    if (fd != -1 || errno != EINVAL)
        return fd;
#endif

    fd = ::socket(domain, type, protocol);
    if (fd == -1)
        return -1;

    ::fcntl(fd, F_SETFD, FD_CLOEXEC);

    // set non-block too?
    if (flags & O_NONBLOCK)
        ::fcntl(fd, F_SETFL, ::fcntl(fd, F_GETFL) | O_NONBLOCK);

    return fd;
}

// Tru64 redefines accept -> _accept with _XOPEN_SOURCE_EXTENDED
static inline int qt_safe_accept(int s, struct sockaddr *addr, QT_SOCKLEN_T *addrlen, int flags = 0)
{
    Q_ASSERT((flags & ~O_NONBLOCK) == 0);

    int fd;
#if QT_UNIX_SUPPORTS_THREADSAFE_CLOEXEC && defined(SOCK_CLOEXEC) && defined(SOCK_NONBLOCK)
    // use accept4
    int sockflags = SOCK_CLOEXEC;
    if (flags & O_NONBLOCK)
        sockflags |= SOCK_NONBLOCK;
    fd = ::accept4(s, addr, static_cast<QT_SOCKLEN_T *>(addrlen), sockflags);
    if (fd != -1 || !(errno == ENOSYS || errno == EINVAL))
        return fd;
#endif

    fd = ::accept(s, addr, static_cast<QT_SOCKLEN_T *>(addrlen));
    if (fd == -1)
        return -1;

    ::fcntl(fd, F_SETFD, FD_CLOEXEC);

    // set non-block too?
    if (flags & O_NONBLOCK)
        ::fcntl(fd, F_SETFL, ::fcntl(fd, F_GETFL) | O_NONBLOCK);

    return fd;
}

// UnixWare 7 redefines listen -> _listen
static inline int qt_safe_listen(int s, int backlog)
{
    return ::listen(s, backlog);
}

static inline int qt_safe_connect(int sockfd, const struct sockaddr *addr, QT_SOCKLEN_T addrlen)
{
    int ret;
    // Solaris e.g. expects a non-const 2nd parameter
    EINTR_LOOP(ret, QT_SOCKET_CONNECT(sockfd, const_cast<struct sockaddr *>(addr), addrlen));
    return ret;
}
#undef QT_SOCKET_CONNECT
#define QT_SOCKET_CONNECT qt_safe_connect

#if defined(socket)
# undef socket
#endif
#if defined(accept)
# undef accept
#endif
#if defined(listen)
# undef listen
#endif

// VxWorks' headers specify 'int' instead of '...' for the 3rd ioctl() parameter.
template <typename T>
static inline int qt_safe_ioctl(int sockfd, int request, T arg)
{
    return ::ioctl(sockfd, request, arg);
}

// VxWorks' headers do not specify any const modifiers
static inline in_addr_t qt_safe_inet_addr(const char *cp)
{
    return ::inet_addr(cp);
}

// VxWorks' headers do not specify any const modifiers
static inline int qt_safe_sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *to, QT_SOCKLEN_T tolen)
{
#ifdef MSG_NOSIGNAL
    flags |= MSG_NOSIGNAL;
#else
    qt_ignore_sigpipe();
#endif

    int ret;
    EINTR_LOOP(ret, ::sendto(sockfd, buf, len, flags, to, tolen));
    return ret;
}

QT_END_NAMESPACE

#endif // QNET_UNIX_P_H

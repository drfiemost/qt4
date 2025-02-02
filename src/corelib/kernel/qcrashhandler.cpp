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

/*************************************************************************
 *
 * stacktrace.c 1.2 1998/12/21
 *
 * Copyright (c) 1998 by Bjorn Reese <breese@imada.ou.dk>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE AUTHORS AND
 * CONTRIBUTORS ACCEPT NO RESPONSIBILITY IN ANY CONCEIVABLE MANNER.
 *
 ************************************************************************/

#include "qplatformdefs.h"
#include "private/qcrashhandler_p.h"
#include "qbytearray.h" // for qvsnprintf()

#ifndef QT_NO_CRASHHANDLER

#include <csignal>
#include <cstdio>
#include <cstdlib>

QT_BEGIN_NAMESPACE

QtCrashHandler QSegfaultHandler::callback = nullptr;

#if defined(__GLIBC__) && (__GLIBC__ >= 2) && !defined(__UCLIBC__) && !defined(QT_LINUXBASE)
QT_BEGIN_INCLUDE_NAMESPACE
# include "qstring.h"
# include <execinfo.h>
QT_END_INCLUDE_NAMESPACE

static void print_backtrace(FILE *outb)
{
    void *stack[128];
    int stack_size = backtrace(stack, sizeof(stack) / sizeof(void *));
    char **stack_symbols = backtrace_symbols(stack, stack_size);
    fprintf(outb, "Stack [%d]:\n", stack_size);
    if(FILE *cppfilt = popen("c++filt", "rw")) {
        dup2(fileno(outb), fileno(cppfilt));
        for(int i = stack_size-1; i>=0; --i)
            fwrite(stack_symbols[i], 1, strlen(stack_symbols[i]), cppfilt);
        pclose(cppfilt);
    } else {
        for(int i = stack_size-1; i>=0; --i)
            fprintf(outb, "#%d  %p [%s]\n", i, stack[i], stack_symbols[i]);
    }
}
static void init_backtrace(char **, int)
{
}

#else /* Don't use the GLIBC callback */
/* Code sourced from: */
QT_BEGIN_INCLUDE_NAMESPACE
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

QT_END_INCLUDE_NAMESPACE


static char *globalProgName = NULL;
static bool backtrace_command(FILE *outb, const char *format, ...)
{

    bool ret = false;
    char buffer[50];

    /*
     * Please note that vsnprintf() is not ASync safe (ie. cannot safely
     * be used from a signal handler.) If this proves to be a problem
     * then the cmd string can be built by more basic functions such as
     * strcpy, strcat, and a home-made integer-to-ascii function.
     */
    va_list args;
    char cmd[512];
    va_start(args, format);
    qvsnprintf(cmd, 512, format, args);
    va_end(args);

    char *foo = cmd;
#if 0
    foo = "echo hi";
#endif
    if(FILE *inb = popen(foo, "r")) {
        while(!feof(inb)) {
            int len = fread(buffer, 1, sizeof(buffer), inb);
            if(!len)
                break;
            if(!ret) {
                fwrite("Output from ", 1, strlen("Output from "), outb);
                strtok(cmd, " ");
                fwrite(cmd, 1, strlen(cmd), outb);
                fwrite("\n", 1, 1, outb);
                ret = true;
            }
            fwrite(buffer, 1, len, outb);
        }
        fclose(inb);
    }
    return ret;
}

static void init_backtrace(char **argv, int argc)
{
    if(argc >= 1)
        globalProgName = argv[0];
}

static void print_backtrace(FILE *outb)
{
    /*
     * In general dbx seems to do a better job than gdb.
     *
     * Different dbx implementations require different flags/commands.
     */
#if defined(Q_OS_FREEBSD)
    /*
     * FreeBSD insists on sending a SIGSTOP to the process we
     * attach to, so we let the debugger send a SIGCONT to that
     * process after we have detached.
     */
    if(backtrace_command(outb, "gdb -q %s %d 2>/dev/null <<EOF\n"
                         "set prompt\n"
                         "where\n"
                         "detach\n"
                         "shell kill -CONT %d\n"
                         "quit\n"
                         "EOF\n",
                         globalProgName, (int)getpid(), (int)getpid()))
        return;
#else /* All other platforms */
    /*
     * TODO: SCO/UnixWare 7 must be something like (not tested)
     *  debug -i c <pid> <<EOF\nstack -f 4\nquit\nEOF\n
     */
# if !defined(__GNUC__)
    if(backtrace_command(outb, "dbx %s %d 2>/dev/null <<EOF\n"
                         "where\n"
                         "detach\n"
                         "EOF\n",
                         globalProgName, (int)getpid()))
        return;
# endif
    if(backtrace_command(outb, "gdb -q %s %d 2>/dev/null <<EOF\n"
                         "set prompt\n"
                         "where\n"
#if 0
                         "echo ---\\n\n"
                         "frame 4\n"
                         "set \\$x = 50\n"
                         "while (\\$x)\n"
                         "info locals\n"
                         "up\n"
                         "set \\$x--\n"
                         "end\n"
                         "echo ---\\n\n"
#endif
                         "detach\n"
                         "quit\n"
                         "EOF\n",
                         globalProgName, (int)getpid()))
        return;
#endif
    const char debug_err[] = "No debugger found\n";
    fwrite(debug_err, strlen(debug_err), 1, outb);
}
/* end of copied code */
#endif


void qt_signal_handler(int sig)
{
    signal(sig, SIG_DFL);
    if(QSegfaultHandler::callback) {
        (*QSegfaultHandler::callback)();
        _exit(1);
    }
    FILE *outb = stderr;
    if(char *crash_loc = ::getenv("QT_CRASH_OUTPUT")) {
        if(FILE *new_outb = fopen(crash_loc, "w")) {
            fprintf(stderr, "Crash (backtrace written to %s)!!!\n", crash_loc);
            outb = new_outb;
        }
    } else {
        fprintf(outb, "Crash!!!\n");
    }
    print_backtrace(outb);
    if(outb != stderr)
        fclose(outb);
    _exit(1);
}


void
QSegfaultHandler::initialize(char **argv, int argc)
{
    init_backtrace(argv, argc);

    struct sigaction SignalAction;
    SignalAction.sa_flags = 0;
    SignalAction.sa_handler = qt_signal_handler;
    sigemptyset(&SignalAction.sa_mask);
    sigaction(SIGSEGV, &SignalAction, nullptr);
    sigaction(SIGBUS, &SignalAction, nullptr);
}

QT_END_NAMESPACE

#endif // QT_NO_CRASHHANDLER

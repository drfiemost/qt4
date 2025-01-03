/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QRAWFONTPRIVATE_P_H
#define QRAWFONTPRIVATE_P_H

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

#include "qrawfont.h"

#include "qfontengine_p.h"
#include <QtCore/qthread.h>
#include <QtCore/qthreadstorage.h>

#if !defined(QT_NO_RAWFONT)

QT_BEGIN_NAMESPACE

namespace { class CustomFontFileLoader; }
class Q_AUTOTEST_EXPORT QRawFontPrivate
{
public:
    QRawFontPrivate()
        : fontEngine(nullptr)
        , hintingPreference(QFont::PreferDefaultHinting)
        , thread(nullptr)
#if defined(Q_WS_WIN)
        , fontHandle(NULL)
#endif
    {}

    QRawFontPrivate(const QRawFontPrivate &other)
        : fontEngine(other.fontEngine)
        , hintingPreference(other.hintingPreference)
        , thread(other.thread)
#if defined(Q_WS_WIN)
        , fontHandle(NULL)
#endif
    {
        if (fontEngine != nullptr)
            fontEngine->ref.ref();
    }

    ~QRawFontPrivate()
    {
        Q_ASSERT(ref.loadRelaxed() == 0);
        cleanUp();
    }

    inline bool isValid() const
    {
        Q_ASSERT(thread == 0 || thread == QThread::currentThread());
        return fontEngine != nullptr;
    }

    void cleanUp();
    void platformCleanUp();
    void platformLoadFromData(const QByteArray &fontData,
                              qreal pixelSize,
                              QFont::HintingPreference hintingPreference);

    static QRawFontPrivate *get(const QRawFont &font) { return font.d.data(); }

    QFontEngine *fontEngine;
    QFont::HintingPreference hintingPreference;
    QThread *thread;
    QAtomicInt ref;

#if defined(Q_WS_WIN)
    HANDLE fontHandle;
#endif
};

QT_END_NAMESPACE

#endif // QT_NO_RAWFONT

#endif // QRAWFONTPRIVATE_P_H

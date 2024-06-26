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

#ifndef QBENCHMARKVALGRIND_P_H
#define QBENCHMARKVALGRIND_P_H

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

#include "QtTest/private/qbenchmarkmeasurement_p.h"
#include "QtTest/private/qbenchmarkmetric_p.h"
#include <QtCore/qmap.h>
#include <QtCore/qstring.h>

class QStringList;

QT_BEGIN_NAMESPACE

class QBenchmarkValgrindUtils
{
public:
    static bool haveValgrind();
    static bool rerunThroughCallgrind(const QStringList &origAppArgs, int &exitCode);
    static bool runCallgrindSubProcess(const QStringList &origAppArgs, int &exitCode);
    static qint64 extractResult(const QString &fileName);
    static QString getNewestFileName();
    static qint64 extractLastResult();
    static void cleanup();
    static QString outFileBase(qint64 pid = -1);
};

class QBenchmarkCallgrindMeasurer : public QBenchmarkMeasurerBase
{
public:
    void start() override;
    qint64 checkpoint() override;
    qint64 stop() override;
    bool isMeasurementAccepted(qint64 measurement) override;
    int adjustIterationCount(int) override;
    int adjustMedianCount(int) override;
    bool needsWarmupIteration() override;
    QTest::QBenchmarkMetric metricType() override;
};

QT_END_NAMESPACE

#endif // QBENCHMARKVALGRIND_H

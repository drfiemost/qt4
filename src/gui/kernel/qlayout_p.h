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

#ifndef QLAYOUT_P_H
#define QLAYOUT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qlayout*.cpp, and qabstractlayout.cpp.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#include "private/qobject_p.h"
#include "qstyle.h"
#include "qsizepolicy.h"

QT_BEGIN_NAMESPACE

class QWidgetItem;
class QSpacerItem;

class Q_GUI_EXPORT QLayoutPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QLayout)

public:
    typedef QWidgetItem * (*QWidgetItemFactoryMethod)(const QLayout *layout, QWidget *widget);
    typedef QSpacerItem * (*QSpacerItemFactoryMethod)(const QLayout *layout, int w, int h, QSizePolicy::Policy hPolicy, QSizePolicy::Policy);

    QLayoutPrivate();

    void getMargin(int *result, int userMargin, QStyle::PixelMetric pm) const;
    void doResize(const QSize &);
    void reparentChildWidgets(QWidget *mw);
    bool checkWidget(QWidget *widget) const;
    bool checkLayout(QLayout *otherLayout) const;

    static QWidgetItem *createWidgetItem(const QLayout *layout, QWidget *widget);
    static QSpacerItem *createSpacerItem(const QLayout *layout, int w, int h, QSizePolicy::Policy hPolicy = QSizePolicy::Minimum, QSizePolicy::Policy vPolicy = QSizePolicy::Minimum);

    static QWidgetItemFactoryMethod widgetItemFactoryMethod;
    static QSpacerItemFactoryMethod spacerItemFactoryMethod;

    int insideSpacing;
    int userLeftMargin;
    int userTopMargin;
    int userRightMargin;
    int userBottomMargin;
    uint topLevel : 1;
    uint enabled : 1;
    uint activated : 1;
    uint autoNewChild : 1;
    QLayout::SizeConstraint constraint;
    QRect rect;
    QWidget *menubar;
};

QT_END_NAMESPACE

#endif // QLAYOUT_P_H

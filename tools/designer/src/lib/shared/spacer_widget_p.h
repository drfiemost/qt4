/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of Qt Designer.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//


#ifndef SPACER_WIDGET_H
#define SPACER_WIDGET_H

#include "shared_global_p.h"

#include <QtGui/QWidget>
#include <QtGui/QSizePolicy>

QT_BEGIN_NAMESPACE

class QDesignerFormWindowInterface;

class QDESIGNER_SHARED_EXPORT Spacer: public QWidget
{
    Q_OBJECT

    Q_ENUMS(SizeType)
    // Special hack: Make name appear as "spacer name"
    Q_PROPERTY(QString spacerName  READ objectName WRITE setObjectName)
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
    Q_PROPERTY(QSizePolicy::Policy sizeType READ sizeType WRITE setSizeType)
    Q_PROPERTY(QSize sizeHint READ sizeHintProperty WRITE setSizeHintProperty DESIGNABLE true STORED true)

public:
    Spacer(QWidget *parent = nullptr);

    QSize sizeHint() const override;

    QSize sizeHintProperty() const;
    void setSizeHintProperty(const QSize &s);

    QSizePolicy::Policy sizeType() const;
    void setSizeType(QSizePolicy::Policy t);

    Qt::Alignment alignment() const;
    Qt::Orientation orientation() const;

    void setOrientation(Qt::Orientation o);
    void setInteractiveMode(bool b) { m_interactive = b; };

    bool event(QEvent *e) override;

protected:
    void paintEvent(QPaintEvent *e) override;
    void resizeEvent(QResizeEvent* e) override;
    void updateMask();

private:
    bool isInLayout() const;
    void updateToolTip();

    const QSize m_SizeOffset;
    QDesignerFormWindowInterface *m_formWindow;
    Qt::Orientation m_orientation;
    bool m_interactive;
    // Cache information about 'being in layout' which is expensive to calculate.
    enum LayoutState { InLayout, OutsideLayout, UnknownLayoutState };
    mutable LayoutState m_layoutState;
    QSize m_sizeHint;
};

QT_END_NAMESPACE

#endif // SPACER_WIDGET_H

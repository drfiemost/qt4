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

#ifndef QSCROLLBAR_H
#define QSCROLLBAR_H

#include <QtGui/qwidget.h>
#include <QtGui/qabstractslider.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


#ifndef QT_NO_SCROLLBAR

class QScrollBarPrivate;
class QStyleOptionSlider;

class Q_GUI_EXPORT QScrollBar : public QAbstractSlider
{
    Q_OBJECT
public:
    explicit QScrollBar(QWidget *parent=nullptr);
    explicit QScrollBar(Qt::Orientation, QWidget *parent=nullptr);
    ~QScrollBar() override;

    QSize sizeHint() const override;
    bool event(QEvent *event) override;

protected:
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *) override;
#endif
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void hideEvent(QHideEvent*) override;
    void sliderChange(SliderChange change) override;
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *) override;
#endif
    void initStyleOption(QStyleOptionSlider *option) const;

private:
    friend Q_GUI_EXPORT QStyleOptionSlider qt_qscrollbarStyleOption(QScrollBar *scrollBar);

    Q_DISABLE_COPY(QScrollBar)
    Q_DECLARE_PRIVATE(QScrollBar)
};

#endif // QT_NO_SCROLLBAR

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSCROLLBAR_H

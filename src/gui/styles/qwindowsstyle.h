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

#ifndef QWINDOWSSTYLE_H
#define QWINDOWSSTYLE_H

#include <QtGui/qcommonstyle.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


#if !defined(QT_NO_STYLE_WINDOWS)

class QWindowsStylePrivate;

class Q_GUI_EXPORT QWindowsStyle : public QCommonStyle
{
    Q_OBJECT
public:
    QWindowsStyle();
    ~QWindowsStyle() override;

    void polish(QApplication*) override;
    void unpolish(QApplication*) override;

    void polish(QWidget*) override;
    void unpolish(QWidget*) override;

    void polish(QPalette &) override;

    void drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p,
                       const QWidget *w = nullptr) const override;
    void drawControl(ControlElement element, const QStyleOption *opt, QPainter *p,
                     const QWidget *w = nullptr) const override;
    QRect subElementRect(SubElement r, const QStyleOption *opt, const QWidget *widget = nullptr) const override;
    void drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p,
                            const QWidget *w = nullptr) const override;
    QSize sizeFromContents(ContentsType ct, const QStyleOption *opt,
                           const QSize &contentsSize, const QWidget *widget = nullptr) const override;

    int pixelMetric(PixelMetric pm, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;

    int styleHint(StyleHint hint, const QStyleOption *opt = nullptr, const QWidget *widget = nullptr,
                  QStyleHintReturn *returnData = nullptr) const override;

    QPixmap standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt,
                           const QWidget *widget = nullptr) const override;

protected Q_SLOTS:
    QIcon standardIconImplementation(StandardPixmap standardIcon, const QStyleOption *option,
                                     const QWidget *widget = nullptr) const;

protected:
    bool eventFilter(QObject *o, QEvent *e) override;
    void timerEvent(QTimerEvent *event) override;
    QWindowsStyle(QWindowsStylePrivate &dd);

private:
    Q_DISABLE_COPY(QWindowsStyle)
    Q_DECLARE_PRIVATE(QWindowsStyle)
    void *reserved;
};

#endif // QT_NO_STYLE_WINDOWS

QT_END_NAMESPACE

QT_END_HEADER

#endif // QWINDOWSSTYLE_H

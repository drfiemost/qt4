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

#ifndef QSYSTEMTRAYICON_H
#define QSYSTEMTRAYICON_H

#include <QtCore/qobject.h>

#ifndef QT_NO_SYSTEMTRAYICON

#include <QtGui/qicon.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QSystemTrayIconPrivate;

class QMenu;
class QEvent;
class QWheelEvent;
class QMouseEvent;
class QPoint;

class Q_GUI_EXPORT QSystemTrayIcon : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString toolTip READ toolTip WRITE setToolTip)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible DESIGNABLE false)

public:
    QSystemTrayIcon(QObject *parent = nullptr);
    QSystemTrayIcon(const QIcon &icon, QObject *parent = nullptr);
    ~QSystemTrayIcon() override;

    enum ActivationReason {
        Unknown,
        Context,
        DoubleClick,
        Trigger,
        MiddleClick
    };

#ifndef QT_NO_MENU
    void setContextMenu(QMenu *menu);
    QMenu *contextMenu() const;
#endif

    QIcon icon() const;
    void setIcon(const QIcon &icon);

    QString toolTip() const;
    void setToolTip(const QString &tip);

    static bool isSystemTrayAvailable();
    static bool supportsMessages();

    enum MessageIcon { NoIcon, Information, Warning, Critical };
    void showMessage(const QString &title, const QString &msg,
                     MessageIcon icon = Information, int msecs = 10000);

    QRect geometry() const;
    bool isVisible() const;

public Q_SLOTS:
    void setVisible(bool visible);
    inline void show() { setVisible(true); }
    inline void hide() { setVisible(false); }

Q_SIGNALS:
    void activated(QSystemTrayIcon::ActivationReason reason);
    void messageClicked();

protected:
    bool event(QEvent *event) override;

private:
    Q_DISABLE_COPY(QSystemTrayIcon)
    Q_DECLARE_PRIVATE(QSystemTrayIcon)

    friend class QSystemTrayIconSys;
    friend class QBalloonTip;
    friend void qtsystray_sendActivated(QSystemTrayIcon *, int);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_SYSTEMTRAYICON
#endif // QSYSTEMTRAYICON_H

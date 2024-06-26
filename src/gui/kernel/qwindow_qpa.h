/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QWINDOW_QPA_H
#define QWINDOW_QPA_H

#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QtGui/qwindowformat_qpa.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QWindowPrivate;
class QWidget;

class QResizeEvent;
class QShowEvent;
class QHideEvent;
class QKeyEvent;
class QInputMethodEvent;
class QMouseEvent;
#ifndef QT_NO_WHEELEVENT
class QWheelEvent;
#endif

class QPlatformWindow;
class QWindowContext;

class Q_GUI_EXPORT QWindow : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QWindow)

    Q_PROPERTY(QString windowTitle READ windowTitle WRITE setWindowTitle)

public:
    enum WindowType {
        Window,
        Dialog,
        Popup,
        Tool,
        SplashScreen,
        ToolTip,
        Sheet,
        Drawer
    };

    enum SurfaceType {
        RasterSurface,
        OpenGLSurface
    };

    QWindow(QWindow *parent = 0);
    virtual ~QWindow();

    // to be removed at some point in the future
    QWidget *widget() const;
    void setWidget(QWidget *widget);

    void setVisible(bool visible);
    void create();

    WId winId() const;
    void setParent(const QWindow *parent);

    void setWindowFormat(const QWindowFormat &format);
    QWindowFormat requestedWindowFormat() const;
    QWindowFormat actualWindowFormat() const;

    void setSurfaceType(SurfaceType type);
    SurfaceType surfaceType() const;

    void setWindowType(WindowType type);
    WindowType type() const;

    QString windowTitle() const;

    void setOpacity(qreal level);
    void requestActivateWindow();

    Qt::WindowStates windowState() const;
    void setWindowState(Qt::WindowStates state);

    QSize minimumSize() const;
    QSize maximumSize() const;

    void setMinimumSize(const QSize &size) const;
    void setMaximumSize(const QSize &size) const;

    void setGeometry(const QRect &rect);
    QRect geometry() const;

    void setWindowIcon(const QImage &icon) const;

    QWindowContext *glContext() const;

    void setRequestFormat(const QWindowFormat &format);
    QWindowFormat format() const;

    void destroy();

    QPlatformWindow *handle() const;

public Q_SLOTS:
    inline void show() { setVisible(true); }
    inline void hide() { setVisible(false); }

    void showMinimized();
    void showMaximized();
    void showFullScreen();
    void showNormal();

    bool close();
    void raise();
    void lower();

    void setWindowTitle(const QString &);

Q_SIGNALS:
    void backBufferReady();

protected:
    virtual void resizeEvent(QResizeEvent *);

    virtual void showEvent(QShowEvent *);
    virtual void hideEvent(QHideEvent *);

    virtual bool event(QEvent *);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void keyReleaseEvent(QKeyEvent *);
    virtual void inputMethodEvent(QInputMethodEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
#ifndef QT_NO_WHEELEVENT
    virtual void wheelEvent(QWheelEvent *);
#endif

private:
    Q_DISABLE_COPY(QWindow)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QWINDOW_QPA_H

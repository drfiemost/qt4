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

#ifndef QAPPLICATION_H
#define QAPPLICATION_H

#include <QtCore/qcoreapplication.h>
#include <QtGui/qwindowdefs.h>
#include <QtCore/qpoint.h>
#include <QtCore/qsize.h>
#include <QtGui/qcursor.h>
#ifdef QT_INCLUDE_COMPAT
# include <QtGui/qdesktopwidget.h>
#endif
#ifdef Q_WS_QWS
# include <QtGui/qrgb.h>
# include <QtGui/qtransportauth_qws.h>
#endif
#ifdef Q_WS_QPA
# include <QtGui/qguiapplication_qpa.h>
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QSessionManager;
class QDesktopWidget;
class QStyle;
class QEventLoop;
class QIcon;
#ifndef QT_NO_IM
class QInputContext;
#endif
template <typename T> class QList;
class QLocale;
#if defined(Q_WS_QWS)
class QDecoration;
#elif defined(Q_WS_QPA)
class QPlatformNativeInterface;
#endif

class QApplication;
class QApplicationPrivate;
#if defined(qApp)
#undef qApp
#endif
#define qApp (static_cast<QApplication *>(QCoreApplication::instance()))

#ifdef Q_WS_QPA
#define QApplicationBase QGuiApplication
#else
#define QApplicationBase QCoreApplication
#endif


class Q_GUI_EXPORT QApplication : public QApplicationBase
{
    Q_OBJECT
    Q_PROPERTY(Qt::LayoutDirection layoutDirection READ layoutDirection WRITE setLayoutDirection)
    Q_PROPERTY(QIcon windowIcon READ windowIcon WRITE setWindowIcon)
    Q_PROPERTY(int cursorFlashTime READ cursorFlashTime WRITE setCursorFlashTime)
    Q_PROPERTY(int doubleClickInterval  READ doubleClickInterval WRITE setDoubleClickInterval)
    Q_PROPERTY(int keyboardInputInterval READ keyboardInputInterval WRITE setKeyboardInputInterval)
#ifndef QT_NO_WHEELEVENT
    Q_PROPERTY(int wheelScrollLines  READ wheelScrollLines WRITE setWheelScrollLines)
#endif
    Q_PROPERTY(QSize globalStrut READ globalStrut WRITE setGlobalStrut)
    Q_PROPERTY(int startDragTime  READ startDragTime WRITE setStartDragTime)
    Q_PROPERTY(int startDragDistance  READ startDragDistance WRITE setStartDragDistance)
    Q_PROPERTY(bool quitOnLastWindowClosed  READ quitOnLastWindowClosed WRITE setQuitOnLastWindowClosed)
#ifndef QT_NO_STYLE_STYLESHEET
    Q_PROPERTY(QString styleSheet READ styleSheet WRITE setStyleSheet)
#endif
    Q_PROPERTY(bool autoSipEnabled READ autoSipEnabled WRITE setAutoSipEnabled)

public:
    enum Type { Tty, GuiClient, GuiServer };

#ifndef qdoc
    QApplication(int &argc, char **argv, int = ApplicationFlags);
    QApplication(int &argc, char **argv, bool GUIenabled, int = ApplicationFlags);
    QApplication(int &argc, char **argv, Type, int = ApplicationFlags);
#if defined(Q_WS_X11)
    QApplication(Display* dpy, Qt::HANDLE visual = 0, Qt::HANDLE cmap = 0, int = ApplicationFlags);
    QApplication(Display *dpy, int &argc, char **argv, Qt::HANDLE visual = 0, Qt::HANDLE cmap= 0, int = ApplicationFlags);
#endif
#endif
    ~QApplication() override;

    static Type type();

    static QStyle *style();
    static void setStyle(QStyle*);
    static QStyle *setStyle(const QString&);
    enum ColorSpec { NormalColor=0, CustomColor=1, ManyColor=2 };
    static int colorSpec();
    static void setColorSpec(int);
    static void setGraphicsSystem(const QString &);

#ifndef QT_NO_CURSOR
    static QCursor *overrideCursor();
    static void setOverrideCursor(const QCursor &);
    static void changeOverrideCursor(const QCursor &);
    static void restoreOverrideCursor();
#endif
    static QPalette palette();
    static QPalette palette(const QWidget *);
    static QPalette palette(const char *className);
    static void setPalette(const QPalette &, const char* className = nullptr);
    static QFont font();
    static QFont font(const QWidget*);
    static QFont font(const char *className);
    static void setFont(const QFont &, const char* className = nullptr);
    static QFontMetrics fontMetrics();

    static void setWindowIcon(const QIcon &icon);
    static QIcon windowIcon();

    static QWidgetList allWidgets();
    static QWidgetList topLevelWidgets();

    static QDesktopWidget *desktop();

    static QWidget *activePopupWidget();
    static QWidget *activeModalWidget();
#ifndef QT_NO_CLIPBOARD
    static QClipboard *clipboard();
#endif
    static QWidget *focusWidget();

    static QWidget *activeWindow();
    static void setActiveWindow(QWidget* act);

    static QWidget *widgetAt(const QPoint &p);
    static inline QWidget *widgetAt(int x, int y) { return widgetAt(QPoint(x, y)); }
    static QWidget *topLevelAt(const QPoint &p);
    static inline QWidget *topLevelAt(int x, int y)  { return topLevelAt(QPoint(x, y)); }

    static void syncX();
    static void beep();
    static void alert(QWidget *widget, int duration = 0);

    static Qt::KeyboardModifiers keyboardModifiers();
    static Qt::KeyboardModifiers queryKeyboardModifiers();
    static Qt::MouseButtons mouseButtons();

    static void setDesktopSettingsAware(bool);
    static bool desktopSettingsAware();

    static void setCursorFlashTime(int);
    static int cursorFlashTime();

    static void setDoubleClickInterval(int);
    static int doubleClickInterval();

    static void setKeyboardInputInterval(int);
    static int keyboardInputInterval();

#ifndef QT_NO_WHEELEVENT
    static void setWheelScrollLines(int);
    static int wheelScrollLines();
#endif
    static void setGlobalStrut(const QSize &);
    static QSize globalStrut();

    static void setStartDragTime(int ms);
    static int startDragTime();
    static void setStartDragDistance(int l);
    static int startDragDistance();

    static void setLayoutDirection(Qt::LayoutDirection direction);
    static Qt::LayoutDirection layoutDirection();

    static inline bool isRightToLeft() { return layoutDirection() == Qt::RightToLeft; }
    static inline bool isLeftToRight() { return layoutDirection() == Qt::LeftToRight; }

    static bool isEffectEnabled(Qt::UIEffect);
    static void setEffectEnabled(Qt::UIEffect, bool enable = true);

#if defined(Q_WS_MAC)
    virtual bool macEventFilter(EventHandlerCallRef, EventRef);
#endif
#if defined(Q_WS_X11)
    virtual bool x11EventFilter(XEvent *);
    virtual int x11ClientMessage(QWidget*, XEvent*, bool passive_only);
    int x11ProcessEvent(XEvent*);
#endif
#if defined(Q_WS_QWS)
    virtual bool qwsEventFilter(QWSEvent *);
    int qwsProcessEvent(QWSEvent*);
    void qwsSetCustomColors(QRgb *colortable, int start, int numColors);
#ifndef QT_NO_QWS_MANAGER
    static QDecoration &qwsDecoration();
    static void qwsSetDecoration(QDecoration *);
    static QDecoration *qwsSetDecoration(const QString &decoration);
#endif
#endif

#if defined(Q_WS_QPA)
    static QPlatformNativeInterface *platformNativeInterface();
#endif


#if defined(Q_WS_WIN)
    void winFocus(QWidget *, bool);
    static void winMouseButtonUp();
#endif
#ifndef QT_NO_SESSIONMANAGER
    // session management
    bool isSessionRestored() const;
    QString sessionId() const;
    QString sessionKey() const;
    virtual void commitData(QSessionManager& sm);
    virtual void saveState(QSessionManager& sm);
#endif

#ifndef QT_NO_IM
    void setInputContext(QInputContext *);
    QInputContext *inputContext() const;
#endif

    static QLocale keyboardInputLocale();
    static Qt::LayoutDirection keyboardInputDirection();

    static int exec();
    bool notify(QObject *, QEvent *) override;


    static void setQuitOnLastWindowClosed(bool quit);
    static bool quitOnLastWindowClosed();

#ifdef QT_KEYPAD_NAVIGATION
    static Q_DECL_DEPRECATED void setKeypadNavigationEnabled(bool);
    static bool keypadNavigationEnabled();
    static void setNavigationMode(Qt::NavigationMode mode);
    static Qt::NavigationMode navigationMode();
#endif

Q_SIGNALS:
    void lastWindowClosed();
    void focusChanged(QWidget *old, QWidget *now);
    void fontDatabaseChanged();
#ifndef QT_NO_SESSIONMANAGER
    void commitDataRequest(QSessionManager &sessionManager);
    void saveStateRequest(QSessionManager &sessionManager);
#endif

public:
    QString styleSheet() const;
public Q_SLOTS:
#ifndef QT_NO_STYLE_STYLESHEET
    void setStyleSheet(const QString& sheet);
#endif
    void setAutoSipEnabled(const bool enabled);
    bool autoSipEnabled() const;
    static void closeAllWindows();
    static void aboutQt();

protected:
#if defined(Q_WS_QWS)
    void setArgs(int, char **);
#endif
    bool event(QEvent *) override;
    bool compressEvent(QEvent *, QObject *receiver, QPostEventList *) override;

#if defined(Q_INTERNAL_QAPP_SRC) || defined(qdoc)
    QApplication(int &argc, char **argv);
    QApplication(int &argc, char **argv, bool GUIenabled);
    QApplication(int &argc, char **argv, Type);
#if defined(Q_WS_X11)
    QApplication(Display* dpy, Qt::HANDLE visual = 0, Qt::HANDLE cmap = 0);
    QApplication(Display *dpy, int &argc, char **argv, Qt::HANDLE visual = 0, Qt::HANDLE cmap= 0);
#endif
#endif

private:
    Q_DISABLE_COPY(QApplication)
    Q_DECLARE_PRIVATE(QApplication)

    friend class QGraphicsWidget;
    friend class QGraphicsItem;
    friend class QGraphicsScene;
    friend class QGraphicsScenePrivate;
    friend class QWidget;
    friend class QWidgetPrivate;
    friend class QETWidget;
    friend class QTranslator;
    friend class QWidgetAnimator;
#ifndef QT_NO_SHORTCUT
    friend class QShortcut;
    friend class QLineEdit;
    friend class QTextControl;
#endif
    friend class QAction;
    friend class QFontDatabasePrivate;

#if defined(Q_WS_QWS)
    friend class QInputContext;
    friend class QWSDirectPainterSurface;
    friend class QDirectPainter;
    friend class QDirectPainterPrivate;
#endif
#ifndef QT_NO_GESTURES
    friend class QGestureManager;
#endif

#if defined(Q_WS_MAC) || defined(Q_WS_X11)
    Q_PRIVATE_SLOT(d_func(), void _q_alertTimeOut())
#endif
#if defined(QT_RX71_MULTITOUCH)
    Q_PRIVATE_SLOT(d_func(), void _q_readRX71MultiTouchEvents())
#endif
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QAPPLICATION_H

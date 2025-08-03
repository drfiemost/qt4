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

#include "qapplication_p.h"
#include "qcolormap.h"
#include "qpixmapcache.h"

#if !defined(QT_NO_GLIB)
#include "qeventdispatcher_glib_qpa_p.h"
#endif
#include "qeventdispatcher_qpa_p.h"

#ifndef QT_NO_CURSOR
#include "private/qcursor_p.h"
#endif

#include "private/qwidget_p.h"
#include "private/qevent_p.h"

#include "qgenericpluginfactory_qpa.h"
#include "qplatformintegrationfactory_qpa_p.h"
#include <qdesktopwidget.h>

#include <qinputcontext.h>
#include <QPlatformCursor>
#include <qdebug.h>
#include <QWindowSystemInterface>
#include "qwindowsysteminterface_qpa_p.h"
#include <QPlatformIntegration>

#include "qdesktopwidget_qpa_p.h"

QT_BEGIN_NAMESPACE

static QString appName;
static QString appFont;

QString QApplicationPrivate::appName() const
{
    return QT_PREPEND_NAMESPACE(appName);
}

void QApplicationPrivate::createEventDispatcher()
{
    QGuiApplicationPrivate::createEventDispatcher();
}

static bool qt_try_modal(QWidget *widget, QEvent::Type type)
{
    QWidget * top = nullptr;

    if (QApplicationPrivate::tryModalHelper(widget, &top))
        return true;

    bool block_event  = false;
    bool paint_event = false;

    switch (type) {
#if 0
    case QEvent::Focus:
        if (!static_cast<QWSFocusEvent*>(event)->simpleData.get_focus)
            break;
        // drop through
#endif
    case QEvent::MouseButtonPress:                        // disallow mouse/key events
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::PlatformPanel:
        block_event         = true;
        break;
    default:
        break;
    }

    if ((block_event || paint_event) && top->parentWidget() == 0)
        top->raise();

    return !block_event;
}



void QApplicationPrivate::enterModal_sys(QWidget *)
{
#if 0
    if (!qt_modal_stack)
        qt_modal_stack = new QWidgetList;
    qt_modal_stack->insert(0, widget);
    app_do_modal = true;
#endif
}

void QApplicationPrivate::leaveModal_sys(QWidget *)
{
#if 0
    if (qt_modal_stack && qt_modal_stack->removeAll(widget)) {
        if (qt_modal_stack->isEmpty()) {
            delete qt_modal_stack;
            qt_modal_stack = nullptr;
        }
    }
    app_do_modal = qt_modal_stack != nullptr;
#endif
}

bool QApplicationPrivate::modalState()
{
    return false;
#if 0
    return app_do_modal;
#endif
}

void QApplicationPrivate::closePopup(QWidget *popup)
{
    Q_Q(QApplication);
    if (!popupWidgets)
        return;
    popupWidgets->removeAll(popup);

//###
//     if (popup == qt_popup_down) {
//         qt_button_down = 0;
//         qt_popup_down = 0;
//     }

    if (QApplicationPrivate::popupWidgets->count() == 0) {                // this was the last popup
        delete QApplicationPrivate::popupWidgets;
        QApplicationPrivate::popupWidgets = 0;

        //### replay mouse event?

        //### transfer/release mouse grab

        //### transfer/release keyboard grab

        //give back focus

        if (active_window) {
            if (QWidget *fw = active_window->focusWidget()) {
                if (fw != QApplication::focusWidget()) {
                    fw->setFocus(Qt::PopupFocusReason);
                } else {
                    QFocusEvent e(QEvent::FocusIn, Qt::PopupFocusReason);
                    q->sendEvent(fw, &e);
                }
            }
        }

    } else {
        // A popup was closed, so the previous popup gets the focus.

        QWidget* aw = QApplicationPrivate::popupWidgets->last();
        if (QWidget *fw = aw->focusWidget())
            fw->setFocus(Qt::PopupFocusReason);

        //### regrab the keyboard and mouse in case 'popup' lost the grab


    }

}

static int openPopupCount = 0;
void QApplicationPrivate::openPopup(QWidget *popup)
{
    openPopupCount++;
    if (!popupWidgets) {                        // create list
        popupWidgets = new QWidgetList;

        /* only grab if you are the first/parent popup */
        //####   ->grabMouse(popup,true);
        //####   ->grabKeyboard(popup,true);
        //### popupGrabOk = true;
    }
    popupWidgets->append(popup);                // add to end of list

    // popups are not focus-handled by the window system (the first
    // popup grabbed the keyboard), so we have to do that manually: A
    // new popup gets the focus
    if (popup->focusWidget()) {
        popup->focusWidget()->setFocus(Qt::PopupFocusReason);
    } else if (popupWidgets->count() == 1) { // this was the first popup
        if (QWidget *fw = QApplication::focusWidget()) {
            QFocusEvent e(QEvent::FocusOut, Qt::PopupFocusReason);
            QApplication::sendEvent(fw, &e);
        }
    }
}

void QApplicationPrivate::initializeMultitouch_sys()
{
}

void QApplicationPrivate::cleanupMultitouch_sys()
{
}

void QApplicationPrivate::initializeWidgetPaletteHash()
{
}

void QApplication::setCursorFlashTime(int msecs)
{
    QApplicationPrivate::cursor_flash_time = msecs;
}

int QApplication::cursorFlashTime()
{
    return QApplicationPrivate::cursor_flash_time;
}

void QApplication::setDoubleClickInterval(int ms)
{
    QApplicationPrivate::mouse_double_click_time = ms;
}

int QApplication::doubleClickInterval()
{
    return QApplicationPrivate::mouse_double_click_time;
}

void QApplication::setKeyboardInputInterval(int ms)
{
    QApplicationPrivate::keyboard_input_time = ms;
}

int QApplication::keyboardInputInterval()
{
    return QApplicationPrivate::keyboard_input_time;
}

#ifndef QT_NO_WHEELEVENT
void QApplication::setWheelScrollLines(int lines)
{
    QApplicationPrivate::wheel_scroll_lines = lines;
}

int QApplication::wheelScrollLines()
{
    return QApplicationPrivate::wheel_scroll_lines;
}
#endif

void QApplication::setEffectEnabled(Qt::UIEffect effect, bool enable)
{
    switch (effect) {
    case Qt::UI_AnimateMenu:
        QApplicationPrivate::animate_menu = enable;
        break;
    case Qt::UI_FadeMenu:
        if (enable)
            QApplicationPrivate::animate_menu = true;
        QApplicationPrivate::fade_menu = enable;
        break;
    case Qt::UI_AnimateCombo:
        QApplicationPrivate::animate_combo = enable;
        break;
    case Qt::UI_AnimateTooltip:
        QApplicationPrivate::animate_tooltip = enable;
        break;
    case Qt::UI_FadeTooltip:
        if (enable)
            QApplicationPrivate::animate_tooltip = true;
        QApplicationPrivate::fade_tooltip = enable;
        break;
    case Qt::UI_AnimateToolBox:
        QApplicationPrivate::animate_toolbox = enable;
        break;
    default:
        QApplicationPrivate::animate_ui = enable;
        break;
    }
}

bool QApplication::isEffectEnabled(Qt::UIEffect effect)
{
    if (QColormap::instance().depth() < 16 || !QApplicationPrivate::animate_ui)
        return false;

    switch(effect) {
    case Qt::UI_AnimateMenu:
        return QApplicationPrivate::animate_menu;
    case Qt::UI_FadeMenu:
        return QApplicationPrivate::fade_menu;
    case Qt::UI_AnimateCombo:
        return QApplicationPrivate::animate_combo;
    case Qt::UI_AnimateTooltip:
        return QApplicationPrivate::animate_tooltip;
    case Qt::UI_FadeTooltip:
        return QApplicationPrivate::fade_tooltip;
    case Qt::UI_AnimateToolBox:
        return QApplicationPrivate::animate_toolbox;
    default:
        return QApplicationPrivate::animate_ui;
    }
}

#ifndef QT_NO_CURSOR
void QApplication::setOverrideCursor(const QCursor &cursor)
{
    qApp->d_func()->cursor_list.prepend(cursor);
    qt_qpa_set_cursor(0, false);
}

void QApplication::restoreOverrideCursor()
{
    if (qApp->d_func()->cursor_list.isEmpty())
        return;
    qApp->d_func()->cursor_list.removeFirst();
    qt_qpa_set_cursor(0, false);
}

#endif// QT_NO_CURSOR

QWidget *QApplication::topLevelAt(const QPoint &pos)
{
    QPlatformIntegration *pi = QApplicationPrivate::platformIntegration();

    QList<QPlatformScreen *> screens = pi->screens();
    QList<QPlatformScreen *>::const_iterator screen = screens.constBegin();
    QList<QPlatformScreen *>::const_iterator end = screens.constEnd();

    // The first screen in a virtual environment should know about all top levels
    if (pi->isVirtualDesktop()) {
        QWidget *w = (*screen)->topLevelAt(pos);
        return w;
    }

    while (screen != end) {
        if ((*screen)->geometry().contains(pos))
            return (*screen)->topLevelAt(pos);
        ++screen;
    }
    return 0;
}

void QApplication::beep()
{
}

void QApplication::alert(QWidget *, int)
{
}

/*!
    \internal
*/
QPlatformNativeInterface *QApplication::platformNativeInterface()
{
    QPlatformIntegration *pi = QApplicationPrivate::platformIntegration();
    return pi ? pi->nativeInterface() : 0;
}

#ifndef QT_NO_QWS_INPUTMETHODS
class QDummyInputContext : public QInputContext
{
public:
    explicit QDummyInputContext(QObject* parent = 0) : QInputContext(parent) {}
    ~QDummyInputContext() {}
    QString identifierName() { return QString(); }
    QString language() { return QString(); }

    void reset() {}
    bool isComposing() const { return false; }

};
#endif // QT_NO_QWS_INPUTMETHODS

void qt_init(QApplicationPrivate *, int type)
{
    Q_UNUSED(type);

    qApp->setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

    QColormap::initialize();
#ifndef QT_NO_CURSOR
//    QCursorData::initialize();
#endif

    qApp->setObjectName(appName);

#ifndef QT_NO_QWS_INPUTMETHODS
        qApp->setInputContext(new QDummyInputContext(qApp));
#endif
}

void qt_cleanup()
{
    QPixmapCache::clear();
#ifndef QT_NO_CURSOR
    QCursorData::cleanup();
#endif
    QColormap::cleanup();
    delete QApplicationPrivate::inputContext;
    QApplicationPrivate::inputContext = 0;

    QApplicationPrivate::active_window = 0; //### this should not be necessary
}

QT_END_NAMESPACE

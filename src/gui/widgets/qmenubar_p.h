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

#ifndef QMENUBAR_P_H
#define QMENUBAR_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// platformMenuBarementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef QMAC_Q3MENUBAR_CPP_FILE
#include "QtGui/qstyleoption.h"
#include <private/qmenu_p.h> // Mac needs what in this file!

#ifdef Q_WS_X11
#include "qabstractplatformmenubar_p.h"
#endif

#ifndef QT_NO_MENUBAR
#endif

QT_BEGIN_NAMESPACE

#ifndef QT_NO_MENUBAR
class QToolBar;
class QMenuBarExtension;
class QMenuBarPrivate : public QWidgetPrivate
{
    Q_DECLARE_PUBLIC(QMenuBar)
public:
    QMenuBarPrivate() : itemsDirty(0), currentAction(nullptr), mouseDown(0),
                         closePopupMode(0), defaultPopDown(1), popupState(0), keyboardState(0), altPressed(0)
#ifndef Q_WS_X11
                         , nativeMenuBar(-1)
#endif
                         , doChildEffects(false)
#ifdef Q_WS_MAC
                         , mac_menubar(0)
#endif
#ifdef Q_WS_X11
                         , platformMenuBar(nullptr)
#endif

        { }
    ~QMenuBarPrivate() override
        {
#ifdef Q_WS_X11
            delete platformMenuBar;
#endif
#ifdef Q_WS_MAC
            delete mac_menubar;
#endif
        }

    void init();
    QAction *getNextAction(const int start, const int increment) const;

    //item calculations
    uint itemsDirty : 1;

    QVector<int> shortcutIndexMap;
    mutable QVector<QRect> actionRects;
    void calcActionRects(int max_width, int start) const;
    QRect actionRect(QAction *) const;
    void updateGeometries();

    //selection
    QPointer<QAction>currentAction;
    uint mouseDown : 1, closePopupMode : 1, defaultPopDown;
    QAction *actionAt(QPoint p) const;
    void setCurrentAction(QAction *, bool =false, bool =false);
    void popupAction(QAction *, bool);

    //active popup state
    uint popupState : 1;
    QPointer<QMenu> activeMenu;

    //keyboard mode for keyboard navigation
    void focusFirstAction();
    void setKeyboardMode(bool);
    uint keyboardState : 1, altPressed : 1;
    QPointer<QWidget> keyboardFocusWidget;

#ifndef Q_WS_X11
    int nativeMenuBar : 3;  // Only has values -1, 0, and 1
#endif
    //firing of events
    void activateAction(QAction *, QAction::ActionEvent);

    void _q_actionTriggered();
    void _q_actionHovered();
    void _q_internalShortcutActivated(int);
    void _q_updateLayout();

    //extra widgets in the menubar
    QPointer<QWidget> leftWidget, rightWidget;
    QMenuBarExtension *extension;
    bool isVisible(QAction *action);

    //menu fading/scrolling effects
    bool doChildEffects;

    QRect menuRect(bool) const;

    // reparenting
    void handleReparent();
    QWidget *oldParent;
    QWidget *oldWindow;

    QList<QAction*> hiddenActions;
    //default action
    QPointer<QAction> defaultAction;

    QBasicTimer autoReleaseTimer;
#ifdef Q_WS_X11
    QAbstractPlatformMenuBar *platformMenuBar;
#endif
#ifdef Q_WS_MAC
    //mac menubar binding
    struct QMacMenuBarPrivate {
        QList<QMacMenuAction*> actionItems;
        OSMenuRef menu, apple_menu;
        QMacMenuBarPrivate();
        ~QMacMenuBarPrivate();

        void addAction(QAction *, QAction* =0);
        void addAction(QMacMenuAction *, QMacMenuAction* =0);
        void syncAction(QMacMenuAction *);
        inline void syncAction(QAction *a) { syncAction(findAction(a)); }
        void removeAction(QMacMenuAction *);
        inline void removeAction(QAction *a) { removeAction(findAction(a)); }
        inline QMacMenuAction *findAction(QAction *a) {
            for(int i = 0; i < actionItems.size(); i++) {
                QMacMenuAction *act = actionItems[i];
                if(a == act->action)
                    return act;
            }
            return 0;
        }
    } *mac_menubar;
    static bool macUpdateMenuBarImmediatly();
    bool macWidgetHasNativeMenubar(QWidget *widget);
    void macCreateMenuBar(QWidget *);
    void macDestroyMenuBar();
    OSMenuRef macMenu();
#endif
#ifdef QT_SOFTKEYS_ENABLED
    QAction *menuBarAction;
#endif

#ifdef Q_WS_X11
    void updateCornerWidgetToolBar();
    QToolBar *cornerWidgetToolBar;
    QWidget *cornerWidgetContainer;
#endif
};
#endif

#endif // QT_NO_MENUBAR

QT_END_NAMESPACE

#endif // QMENUBAR_P_H

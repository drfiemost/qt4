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

#ifndef QTABBAR_H
#define QTABBAR_H

#include <QtGui/qwidget.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


#ifndef QT_NO_TABBAR

class QIcon;
class QTabBarPrivate;
class QStyleOptionTab;

class Q_GUI_EXPORT QTabBar: public QWidget
{
    Q_OBJECT

    Q_ENUMS(Shape)
    Q_PROPERTY(Shape shape READ shape WRITE setShape)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentChanged)
    Q_PROPERTY(int count READ count)
    Q_PROPERTY(bool drawBase READ drawBase WRITE setDrawBase)
    Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize)
    Q_PROPERTY(Qt::TextElideMode elideMode READ elideMode WRITE setElideMode)
    Q_PROPERTY(bool usesScrollButtons READ usesScrollButtons WRITE setUsesScrollButtons)
    Q_PROPERTY(bool tabsClosable READ tabsClosable WRITE setTabsClosable)
    Q_PROPERTY(SelectionBehavior selectionBehaviorOnRemove READ selectionBehaviorOnRemove WRITE setSelectionBehaviorOnRemove)
    Q_PROPERTY(bool expanding READ expanding WRITE setExpanding)
    Q_PROPERTY(bool movable READ isMovable WRITE setMovable)
    Q_PROPERTY(bool documentMode READ documentMode WRITE setDocumentMode)

public:
    explicit QTabBar(QWidget* parent=nullptr);
    ~QTabBar() override;

    enum Shape { RoundedNorth, RoundedSouth, RoundedWest, RoundedEast,
                 TriangularNorth, TriangularSouth, TriangularWest, TriangularEast
    };

    enum ButtonPosition {
        LeftSide,
        RightSide
    };

    enum SelectionBehavior {
        SelectLeftTab,
        SelectRightTab,
        SelectPreviousTab
    };

    Shape shape() const;
    void setShape(Shape shape);

    int addTab(const QString &text);
    int addTab(const QIcon &icon, const QString &text);

    int insertTab(int index, const QString &text);
    int insertTab(int index, const QIcon&icon, const QString &text);

    void removeTab(int index);
    void moveTab(int from, int to);

    bool isTabEnabled(int index) const;
    void setTabEnabled(int index, bool);

    QString tabText(int index) const;
    void setTabText(int index, const QString &text);

    QColor tabTextColor(int index) const;
    void setTabTextColor(int index, const QColor &color);

    QIcon tabIcon(int index) const;
    void setTabIcon(int index, const QIcon &icon);

    Qt::TextElideMode elideMode() const;
    void setElideMode(Qt::TextElideMode);

#ifndef QT_NO_TOOLTIP
    void setTabToolTip(int index, const QString &tip);
    QString tabToolTip(int index) const;
#endif

#ifndef QT_NO_WHATSTHIS
    void setTabWhatsThis(int index, const QString &text);
    QString tabWhatsThis(int index) const;
#endif

    void setTabData(int index, const QVariant &data);
    QVariant tabData(int index) const;

    QRect tabRect(int index) const;
    int tabAt(const QPoint &pos) const;

    int currentIndex() const;
    int count() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void setDrawBase(bool drawTheBase);
    bool drawBase() const;

    QSize iconSize() const;
    void setIconSize(const QSize &size);

    bool usesScrollButtons() const;
    void setUsesScrollButtons(bool useButtons);

    bool tabsClosable() const;
    void setTabsClosable(bool closable);

    void setTabButton(int index, ButtonPosition position, QWidget *widget);
    QWidget *tabButton(int index, ButtonPosition position) const;

    SelectionBehavior selectionBehaviorOnRemove() const;
    void setSelectionBehaviorOnRemove(SelectionBehavior behavior);

    bool expanding() const;
    void setExpanding(bool enabled);

    bool isMovable() const;
    void setMovable(bool movable);

    bool documentMode() const;
    void setDocumentMode(bool set);

public Q_SLOTS:
    void setCurrentIndex(int index);

Q_SIGNALS:
    void currentChanged(int index);
    void tabCloseRequested(int index);
    void tabMoved(int from, int to);

protected:
    virtual QSize tabSizeHint(int index) const;
    virtual void tabInserted(int index);
    virtual void tabRemoved(int index);
    virtual void tabLayoutChange();

    bool event(QEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    void showEvent(QShowEvent *) override;
    void hideEvent(QHideEvent *) override;
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent (QMouseEvent *) override;
    void mouseMoveEvent (QMouseEvent *) override;
    void mouseReleaseEvent (QMouseEvent *) override;
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *event) override;
#endif
    void keyPressEvent(QKeyEvent *) override;
    void changeEvent(QEvent *) override;
    void initStyleOption(QStyleOptionTab *option, int tabIndex) const;

    friend class QAccessibleTabBar;
private:
    Q_DISABLE_COPY(QTabBar)
    Q_DECLARE_PRIVATE(QTabBar)
    Q_PRIVATE_SLOT(d_func(), void _q_scrollTabs())
    Q_PRIVATE_SLOT(d_func(), void _q_closeTab())
};

#endif // QT_NO_TABBAR

QT_END_NAMESPACE

QT_END_HEADER

#endif // QTABBAR_H

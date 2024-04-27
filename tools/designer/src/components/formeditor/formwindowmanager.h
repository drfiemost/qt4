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

#ifndef FORMWINDOWMANAGER_H
#define FORMWINDOWMANAGER_H

#include "formeditor_global.h"

#include <QtDesigner/private/qdesigner_formwindowmanager_p.h>

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QPointer>
#include <QtCore/QMap>

QT_BEGIN_NAMESPACE

class QAction;
class QActionGroup;
class QUndoGroup;
class QDesignerFormEditorInterface;
class QDesignerWidgetBoxInterface;

namespace qdesigner_internal {

class FormWindow;
class PreviewManager;
class PreviewActionGroup;

class QT_FORMEDITOR_EXPORT FormWindowManager
    : public QDesignerFormWindowManager
{
    Q_OBJECT
public:
    explicit FormWindowManager(QDesignerFormEditorInterface *core, QObject *parent = nullptr);
    ~FormWindowManager() override;

    QDesignerFormEditorInterface *core() const override;

    inline QAction *actionCut() const override { return m_actionCut; }
    inline QAction *actionCopy() const override { return m_actionCopy; }
    inline QAction *actionPaste() const override { return m_actionPaste; }
    inline QAction *actionDelete() const override { return m_actionDelete; }
    inline QAction *actionSelectAll() const override { return m_actionSelectAll; }
    inline QAction *actionLower() const override { return m_actionLower; }
    inline QAction *actionRaise() const override { return m_actionRaise; }
    QAction *actionUndo() const override;
    QAction *actionRedo() const override;

    inline QAction *actionHorizontalLayout() const override { return m_actionHorizontalLayout; }
    inline QAction *actionVerticalLayout() const override { return m_actionVerticalLayout; }
    inline QAction *actionSplitHorizontal() const override { return m_actionSplitHorizontal; }
    inline QAction *actionSplitVertical() const override { return m_actionSplitVertical; }
    inline QAction *actionGridLayout() const override { return m_actionGridLayout; }
    inline QAction *actionBreakLayout() const override { return m_actionBreakLayout; }
    inline QAction *actionAdjustSize() const override { return m_actionAdjustSize; }

    inline QAction *actionDefaultPreview() const override { return m_actionDefaultPreview; }
    QActionGroup *actionGroupPreviewInStyle() const override;
    QAction *actionShowFormWindowSettingsDialog() const override;

    QDesignerFormWindowInterface *activeFormWindow() const override;

    int formWindowCount() const override;
    QDesignerFormWindowInterface *formWindow(int index) const override;

    QDesignerFormWindowInterface *createFormWindow(QWidget *parentWidget = nullptr, Qt::WindowFlags flags = nullptr) override;

    QPixmap createPreviewPixmap(QString *errorMessage) override;

    bool eventFilter(QObject *o, QEvent *e) override;

    void dragItems(const QList<QDesignerDnDItemInterface*> &item_list) override;

    QUndoGroup *undoGroup() const;

    PreviewManager *previewManager() const override { return m_previewManager; }

public slots:
    void addFormWindow(QDesignerFormWindowInterface *formWindow) override;
    void removeFormWindow(QDesignerFormWindowInterface *formWindow) override;
    void setActiveFormWindow(QDesignerFormWindowInterface *formWindow) override;
    void closeAllPreviews() override;
    void deviceProfilesChanged();

private slots:
    void slotActionCutActivated();
    void slotActionCopyActivated();
    void slotActionPasteActivated();
    void slotActionDeleteActivated();
    void slotActionSelectAllActivated();
    void slotActionLowerActivated();
    void slotActionRaiseActivated();
    void createLayout();
    void slotActionBreakLayoutActivated();
    void slotActionAdjustSizeActivated();
    void slotActionSimplifyLayoutActivated();
    void slotActionDefaultPreviewActivated();
    void slotActionGroupPreviewInStyle(const QString &style, int deviceProfileIndex);
    void slotActionShowFormWindowSettingsDialog();

    void slotUpdateActions();

private:
    void setupActions();
    FormWindow *findFormWindow(QWidget *w);
    QWidget *findManagedWidget(FormWindow *fw, QWidget *w);

    void setCurrentUndoStack(QUndoStack *stack);

private:
    enum CreateLayoutContext { LayoutContainer, LayoutSelection, MorphLayout };

    QDesignerFormEditorInterface *m_core;
    FormWindow *m_activeFormWindow;
    QList<FormWindow*> m_formWindows;

    PreviewManager *m_previewManager;

    /* Context of the layout actions and base for morphing layouts. Determined
     * in slotUpdateActions() and used later on in the action slots. */
    CreateLayoutContext m_createLayoutContext;
    QWidget *m_morphLayoutContainer;

    // edit actions
    QAction *m_actionCut;
    QAction *m_actionCopy;
    QAction *m_actionPaste;
    QAction *m_actionSelectAll;
    QAction *m_actionDelete;
    QAction *m_actionLower;
    QAction *m_actionRaise;
    // layout actions
    QAction *m_actionHorizontalLayout;
    QAction *m_actionVerticalLayout;
    QAction *m_actionSplitHorizontal;
    QAction *m_actionSplitVertical;
    QAction *m_actionGridLayout;
    QAction *m_actionBreakLayout;
    QAction *m_actionAdjustSize;
    // preview actions
    QAction *m_actionDefaultPreview;
    mutable PreviewActionGroup *m_actionGroupPreviewInStyle;
    QAction *m_actionShowFormWindowSettingsDialog;

    QAction *m_actionUndo;
    QAction *m_actionRedo;

    QMap<QWidget *,bool> getUnsortedLayoutsToBeBroken(bool firstOnly) const;
    bool hasLayoutsToBeBroken() const;
    QWidgetList layoutsToBeBroken(QWidget *w) const;
    QWidgetList layoutsToBeBroken() const;

    QUndoGroup *m_undoGroup;

};

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // FORMWINDOWMANAGER_H

/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#ifndef MESSAGEEDITOR_H
#define MESSAGEEDITOR_H

#include "messagemodel.h"

#include <QtCore/QLocale>
#include <QtCore/QTimer>

#include <QtGui/QFrame>
#include <QtGui/QScrollArea>

QT_BEGIN_NAMESPACE

class QBoxLayout;
class QMainWindow;
class QTextEdit;

class MessageEditor;
class FormatTextEdit;
class FormWidget;
class FormMultiWidget;

struct MessageEditorData {
    QWidget *container;
    FormWidget *transCommentText;
    QList<FormMultiWidget *> transTexts;
    QString invariantForm;
    QString firstForm;
    qreal fontSize;
    bool pluralEditMode;
};

class MessageEditor : public QScrollArea
{
    Q_OBJECT

public:
    MessageEditor(MultiDataModel *dataModel, QMainWindow *parent = nullptr);

    void showNothing();
    void showMessage(const MultiDataIndex &index);
    void setNumerusForms(int model, const QStringList &numerusForms);
    bool eventFilter(QObject *, QEvent *) override;
    void setTranslation(int model, const QString &translation, int numerus);
    int activeModel() const { return (m_editors.count() != 1) ? m_currentModel : 0; }
    void setEditorFocus(int model);
    void setUnfinishedEditorFocus();
    bool focusNextUnfinished();

signals:
    void translationChanged(const QStringList &translations);
    void translatorCommentChanged(const QString &comment);
    void activeModelChanged(int model);

    void undoAvailable(bool avail);
    void redoAvailable(bool avail);
    void cutAvailable(bool avail);
    void copyAvailable(bool avail);
    void pasteAvailable(bool avail);
    void beginFromSourceAvailable(bool enable);

public slots:
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void selectAll();
    void beginFromSource();
    void setEditorFocus();
    void setTranslation(int latestModel, const QString &translation);
    void setLengthVariants(bool on);

private slots:
    void editorCreated(QTextEdit *);
    void editorDestroyed();
    void selectionChanged(QTextEdit *);
    void resetHoverSelection();
    void emitTranslationChanged(QTextEdit *);
    void emitTranslatorCommentChanged(QTextEdit *);
    void updateCanPaste();
    void clipboardChanged();
    void messageModelAppended();
    void messageModelDeleted(int model);
    void allModelsDeleted();
    void setTargetLanguage(int model);
    void reallyFixTabOrder();

private:
    void setupEditorPage();
    void setEditingEnabled(int model, bool enabled);
    bool focusNextUnfinished(int start);
    void resetSelection();
    void grabFocus(QWidget *widget);
    void trackFocus(QWidget *widget);
    void activeModelAndNumerus(int *model, int *numerus) const;
    QTextEdit *activeTranslation() const;
    QTextEdit *activeOr1stTranslation() const;
    QTextEdit *activeTransComment() const;
    QTextEdit *activeEditor() const;
    QTextEdit *activeOr1stEditor() const;
    MessageEditorData *modelForWidget(const QObject *o);
    int activeTranslationNumerus() const;
    QStringList translations(int model) const;
    void updateBeginFromSource();
    void updateUndoRedo();
    void updateCanCutCopy();
    void addPluralForm(int model, const QString &label, bool writable);
    void fixTabOrder();
    QPalette paletteForModel(int model) const;

    MultiDataModel *m_dataModel;

    MultiDataIndex m_currentIndex;
    int m_currentModel;
    int m_currentNumerus;

    bool m_lengthVariants;

    bool m_undoAvail;
    bool m_redoAvail;
    bool m_cutAvail;
    bool m_copyAvail;

    bool m_clipboardEmpty;

    QTextEdit *m_selectionHolder;
    QWidget *m_focusWidget;
    QBoxLayout *m_layout;
    FormWidget *m_source;
    FormWidget *m_pluralSource;
    FormWidget *m_commentText;
    QList<MessageEditorData> m_editors;

    QTimer m_tabOrderTimer;
};

QT_END_NAMESPACE

#endif // MESSAGEEDITOR_H

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

#ifndef QCOMBOBOX_H
#define QCOMBOBOX_H

#include <QtGui/qwidget.h>
#include <QtGui/qabstractitemdelegate.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qvariant.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

#ifndef QT_NO_COMBOBOX

class QAbstractItemView;
class QLineEdit;
class QComboBoxPrivate;
class QCompleter;

class Q_GUI_EXPORT QComboBox : public QWidget
{
    Q_OBJECT

    Q_ENUMS(InsertPolicy)
    Q_ENUMS(SizeAdjustPolicy)
    Q_PROPERTY(bool editable READ isEditable WRITE setEditable)
    Q_PROPERTY(int count READ count)
    Q_PROPERTY(QString currentText READ currentText USER true)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(int maxVisibleItems READ maxVisibleItems WRITE setMaxVisibleItems)
    Q_PROPERTY(int maxCount READ maxCount WRITE setMaxCount)
    Q_PROPERTY(InsertPolicy insertPolicy READ insertPolicy WRITE setInsertPolicy)
    Q_PROPERTY(SizeAdjustPolicy sizeAdjustPolicy READ sizeAdjustPolicy WRITE setSizeAdjustPolicy)
    Q_PROPERTY(int minimumContentsLength READ minimumContentsLength WRITE setMinimumContentsLength)
    Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize)

#ifndef QT_NO_COMPLETER
    Q_PROPERTY(bool autoCompletion READ autoCompletion WRITE setAutoCompletion DESIGNABLE false)
    Q_PROPERTY(Qt::CaseSensitivity autoCompletionCaseSensitivity READ autoCompletionCaseSensitivity WRITE setAutoCompletionCaseSensitivity DESIGNABLE false)
#endif // QT_NO_COMPLETER

    Q_PROPERTY(bool duplicatesEnabled READ duplicatesEnabled WRITE setDuplicatesEnabled)
    Q_PROPERTY(bool frame READ hasFrame WRITE setFrame)
    Q_PROPERTY(int modelColumn READ modelColumn WRITE setModelColumn)

public:
    explicit QComboBox(QWidget *parent = nullptr);
    ~QComboBox() override;

    int maxVisibleItems() const;
    void setMaxVisibleItems(int maxItems);

    int count() const;
    void setMaxCount(int max);
    int maxCount() const;

#ifndef QT_NO_COMPLETER
    bool autoCompletion() const;
    void setAutoCompletion(bool enable);

    Qt::CaseSensitivity autoCompletionCaseSensitivity() const;
    void setAutoCompletionCaseSensitivity(Qt::CaseSensitivity sensitivity);
#endif

    bool duplicatesEnabled() const;
    void setDuplicatesEnabled(bool enable);

    void setFrame(bool);
    bool hasFrame() const;

    inline int findText(const QString &text,
                        Qt::MatchFlags flags = static_cast<Qt::MatchFlags>(Qt::MatchExactly|Qt::MatchCaseSensitive)) const
        { return findData(text, Qt::DisplayRole, flags); }
    int findData(const QVariant &data, int role = Qt::UserRole,
                 Qt::MatchFlags flags = static_cast<Qt::MatchFlags>(Qt::MatchExactly|Qt::MatchCaseSensitive)) const;

    enum InsertPolicy {
        NoInsert,
        InsertAtTop,
        InsertAtCurrent,
        InsertAtBottom,
        InsertAfterCurrent,
        InsertBeforeCurrent,
        InsertAlphabetically
    };

    InsertPolicy insertPolicy() const;
    void setInsertPolicy(InsertPolicy policy);

    enum SizeAdjustPolicy {
        AdjustToContents,
        AdjustToContentsOnFirstShow,
        AdjustToMinimumContentsLength, // ### Qt 5: remove
        AdjustToMinimumContentsLengthWithIcon
    };

    SizeAdjustPolicy sizeAdjustPolicy() const;
    void setSizeAdjustPolicy(SizeAdjustPolicy policy);
    int minimumContentsLength() const;
    void setMinimumContentsLength(int characters);
    QSize iconSize() const;
    void setIconSize(const QSize &size);

    bool isEditable() const;
    void setEditable(bool editable);
    void setLineEdit(QLineEdit *edit);
    QLineEdit *lineEdit() const;
#ifndef QT_NO_VALIDATOR
    void setValidator(const QValidator *v);
    const QValidator *validator() const;
#endif

#ifndef QT_NO_COMPLETER
    void setCompleter(QCompleter *c);
    QCompleter *completer() const;
#endif

    QAbstractItemDelegate *itemDelegate() const;
    void setItemDelegate(QAbstractItemDelegate *delegate);

    QAbstractItemModel *model() const;
    void setModel(QAbstractItemModel *model);

    QModelIndex rootModelIndex() const;
    void setRootModelIndex(const QModelIndex &index);

    int modelColumn() const;
    void setModelColumn(int visibleColumn);

    int currentIndex() const;

    QString currentText() const;

    QString itemText(int index) const;
    QIcon itemIcon(int index) const;
    QVariant itemData(int index, int role = Qt::UserRole) const;

    inline void addItem(const QString &text, const QVariant &userData = QVariant());
    inline void addItem(const QIcon &icon, const QString &text,
                        const QVariant &userData = QVariant());
    inline void addItems(const QStringList &texts)
        { insertItems(count(), texts); }

    inline void insertItem(int index, const QString &text, const QVariant &userData = QVariant());
    void insertItem(int index, const QIcon &icon, const QString &text,
                    const QVariant &userData = QVariant());
    void insertItems(int index, const QStringList &texts);
    void insertSeparator(int index);

    void removeItem(int index);

    void setItemText(int index, const QString &text);
    void setItemIcon(int index, const QIcon &icon);
    void setItemData(int index, const QVariant &value, int role = Qt::UserRole);

    QAbstractItemView *view() const;
    void setView(QAbstractItemView *itemView);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    virtual void showPopup();
    virtual void hidePopup();

    bool event(QEvent *event) override;

public Q_SLOTS:
    void clear();
    void clearEditText();
    void setEditText(const QString &text);
    void setCurrentIndex(int index);

Q_SIGNALS:
    void editTextChanged(const QString &);
    void activated(int index);
    void activated(const QString &);
    void highlighted(int index);
    void highlighted(const QString &);
    void currentIndexChanged(int index);
    void currentIndexChanged(const QString &);

protected:
    void focusInEvent(QFocusEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;
    void changeEvent(QEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void paintEvent(QPaintEvent *e) override;
    void showEvent(QShowEvent *e) override;
    void hideEvent(QHideEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *e) override;
#endif
    void contextMenuEvent(QContextMenuEvent *e) override;
    void inputMethodEvent(QInputMethodEvent *) override;
    QVariant inputMethodQuery(Qt::InputMethodQuery) const override;
    void initStyleOption(QStyleOptionComboBox *option) const;

protected:
    QComboBox(QComboBoxPrivate &, QWidget *);

private:
    Q_DECLARE_PRIVATE(QComboBox)
    Q_DISABLE_COPY(QComboBox)
    Q_PRIVATE_SLOT(d_func(), void _q_itemSelected(const QModelIndex &item))
    Q_PRIVATE_SLOT(d_func(), void _q_emitHighlighted(const QModelIndex &))
    Q_PRIVATE_SLOT(d_func(), void _q_emitCurrentIndexChanged(const QModelIndex &index))
    Q_PRIVATE_SLOT(d_func(), void _q_editingFinished())
    Q_PRIVATE_SLOT(d_func(), void _q_returnPressed())
    Q_PRIVATE_SLOT(d_func(), void _q_resetButton())
    Q_PRIVATE_SLOT(d_func(), void _q_dataChanged(const QModelIndex &, const QModelIndex &))
    Q_PRIVATE_SLOT(d_func(), void _q_updateIndexBeforeChange())
    Q_PRIVATE_SLOT(d_func(), void _q_rowsInserted(const QModelIndex & parent, int start, int end))
    Q_PRIVATE_SLOT(d_func(), void _q_rowsRemoved(const QModelIndex & parent, int start, int end))
    Q_PRIVATE_SLOT(d_func(), void _q_modelDestroyed())
    Q_PRIVATE_SLOT(d_func(), void _q_modelReset())
#ifdef QT_KEYPAD_NAVIGATION
    Q_PRIVATE_SLOT(d_func(), void _q_completerActivated())
#endif
};

inline void QComboBox::addItem(const QString &atext, const QVariant &auserData)
{ insertItem(count(), atext, auserData); }
inline void QComboBox::addItem(const QIcon &aicon, const QString &atext,
                               const QVariant &auserData)
{ insertItem(count(), aicon, atext, auserData); }

inline void QComboBox::insertItem(int aindex, const QString &atext,
                                  const QVariant &auserData)
{ insertItem(aindex, QIcon(), atext, auserData); }

#endif // QT_NO_COMBOBOX

QT_END_NAMESPACE

QT_END_HEADER

#endif // QCOMBOBOX_H

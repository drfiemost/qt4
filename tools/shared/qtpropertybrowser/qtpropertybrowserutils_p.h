/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the tools applications of the Qt Toolkit.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of Qt Designer.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef QTPROPERTYBROWSERUTILS_H
#define QTPROPERTYBROWSERUTILS_H

#include <QtCore/QMap>
#include <QtGui/QIcon>
#include <QtGui/QWidget>
#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE

class QMouseEvent;
class QCheckBox;
class QLineEdit;

class QtCursorDatabase
{
public:
    QtCursorDatabase();
    void clear();

    QStringList cursorShapeNames() const;
    QMap<int, QIcon> cursorShapeIcons() const;
    QString cursorToShapeName(const QCursor &cursor) const;
    QIcon cursorToShapeIcon(const QCursor &cursor) const;
    int cursorToValue(const QCursor &cursor) const;
#ifndef QT_NO_CURSOR
    QCursor valueToCursor(int value) const;
#endif
private:
    void appendCursor(Qt::CursorShape shape, const QString &name, const QIcon &icon);
    QStringList m_cursorNames;
    QMap<int, QIcon> m_cursorIcons;
    QMap<int, Qt::CursorShape> m_valueToCursorShape;
    QMap<Qt::CursorShape, int> m_cursorShapeToValue;
};

class QtPropertyBrowserUtils
{
public:
    static QPixmap brushValuePixmap(const QBrush &b);
    static QIcon brushValueIcon(const QBrush &b);
    static QString colorValueText(const QColor &c);
    static QPixmap fontValuePixmap(const QFont &f);
    static QIcon fontValueIcon(const QFont &f);
    static QString fontValueText(const QFont &f);
    static QString dateFormat();
    static QString timeFormat();
    static QString dateTimeFormat();
};

class QtBoolEdit : public QWidget {
    Q_OBJECT
public:
    QtBoolEdit(QWidget *parent = nullptr);

    bool textVisible() const { return m_textVisible; }
    void setTextVisible(bool textVisible);

    Qt::CheckState checkState() const;
    void setCheckState(Qt::CheckState state);

    bool isChecked() const;
    void setChecked(bool c);

    bool blockCheckBoxSignals(bool block);

Q_SIGNALS:
    void toggled(bool);

protected:
    void mousePressEvent(QMouseEvent * event) override;

private:
    QCheckBox *m_checkBox;
    bool m_textVisible;
};

class QtKeySequenceEdit : public QWidget
{
    Q_OBJECT
public:
    QtKeySequenceEdit(QWidget *parent = nullptr);

    QKeySequence keySequence() const;
    bool eventFilter(QObject *o, QEvent *e) override;
public Q_SLOTS:
    void setKeySequence(const QKeySequence &sequence);
Q_SIGNALS:
    void keySequenceChanged(const QKeySequence &sequence);
protected:
    void focusInEvent(QFocusEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;
    bool event(QEvent *e) override;
private slots:
    void slotClearShortcut();
private:
    void handleKeyEvent(QKeyEvent *e);
    int translateModifiers(Qt::KeyboardModifiers state, const QString &text) const;

    int m_num;
    QKeySequence m_keySequence;
    QLineEdit *m_lineEdit;
};

QT_END_NAMESPACE

#endif

/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#ifndef INDEXWINDOW_H
#define INDEXWINDOW_H

#include <QtCore/QUrl>
#include <QtGui/QWidget>
#include <QtGui/QLineEdit>

QT_BEGIN_NAMESPACE

class QHelpIndexWidget;
class QModelIndex;

class IndexWindow : public QWidget
{
    Q_OBJECT

public:
    IndexWindow(QWidget *parent = nullptr);
    ~IndexWindow() override;

    void setSearchLineEditText(const QString &text);
    QString searchLineEditText() const
    {
        return m_searchLineEdit->text();
    }

signals:
    void linkActivated(const QUrl &link);
    void linksActivated(const QMap<QString, QUrl> &links,
        const QString &keyword);
    void escapePressed();

private slots:
    void filterIndices(const QString &filter);
    void enableSearchLineEdit();
    void disableSearchLineEdit();

private:
    bool eventFilter(QObject *obj, QEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;
    void open(QHelpIndexWidget *indexWidget, const QModelIndex &index);

    QLineEdit *m_searchLineEdit;
    QHelpIndexWidget *m_indexWidget;
};

QT_END_NAMESPACE

#endif // INDEXWINDOW_H

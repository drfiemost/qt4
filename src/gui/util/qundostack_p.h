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

#ifndef QUNDOSTACK_P_H
#define QUNDOSTACK_P_H

#include <private/qobject_p.h>
#include <QtCore/qlist.h>
#include <QtCore/qstring.h>
#include <QtGui/qaction.h>

#include "qundostack.h"

QT_BEGIN_NAMESPACE
class QUndoCommand;
class QUndoGroup;

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qapplication_*.cpp, qwidget*.cpp and qfiledialog.cpp.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

class QUndoCommandPrivate
{
public:
    QUndoCommandPrivate() : id(-1) {}
    QList<QUndoCommand*> child_list;
    QString text;
    QString actionText;
    int id;
};

#ifndef QT_NO_UNDOSTACK

class QUndoStackPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QUndoStack)
public:
    QUndoStackPrivate() : index(0), clean_index(0), group(nullptr), undo_limit(0) {}

    QList<QUndoCommand*> command_list;
    QList<QUndoCommand*> macro_stack;
    int index;
    int clean_index;
    QUndoGroup *group;
    int undo_limit;

    void setIndex(int idx, bool clean);
    bool checkUndoLimit();
};

#ifndef QT_NO_ACTION
class QUndoAction : public QAction
{
    Q_OBJECT
public:
    QUndoAction(const QString &prefix, QObject *parent = nullptr);
    void setTextFormat(const QString &textFormat, const QString &defaultText);
public Q_SLOTS:
    void setPrefixedText(const QString &text);
private:
    QString m_prefix;
    QString m_defaultText;
};
#endif // QT_NO_ACTION


QT_END_NAMESPACE
#endif // QT_NO_UNDOSTACK
#endif // QUNDOSTACK_P_H

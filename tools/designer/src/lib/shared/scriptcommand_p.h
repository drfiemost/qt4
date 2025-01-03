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

#ifndef QDESIGNER_SCRIPTCOMMAND_H
#define QDESIGNER_SCRIPTCOMMAND_H

#include "qdesigner_formwindowcommand_p.h"

#include <QtCore/QPair>
#include <QtCore/QList>
#include <QtCore/QPointer>

QT_BEGIN_NAMESPACE

class QDesignerFormWindowInterface;

namespace qdesigner_internal {

class QDESIGNER_SHARED_EXPORT ScriptCommand: public QDesignerFormWindowCommand
{
    ScriptCommand(const ScriptCommand &) = delete;
    ScriptCommand& operator=(const ScriptCommand &) = delete;

public:
    explicit ScriptCommand(QDesignerFormWindowInterface *formWindow);

    typedef QList<QObject *> ObjectList;
    bool init(const ObjectList &list, const QString &script);

    void redo() override;
    void undo() override;

private:
    typedef QPair<QPointer<QObject>, QString> ObjectScriptPair;
    typedef QList<ObjectScriptPair> ObjectScriptList;
    ObjectScriptList m_oldValues;
    QString m_script;
};

} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // QDESIGNER_SCRIPTCOMMAND_H

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


#include "qdesigner_formwindowcommand_p.h"
#include "qdesigner_objectinspector_p.h"
#include "layout_p.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerObjectInspectorInterface>
#include <QtDesigner/QDesignerActionEditorInterface>
#include <QtDesigner/QDesignerMetaDataBaseInterface>
#include <QtDesigner/QDesignerPropertySheetExtension>
#include <QtDesigner/QDesignerPropertyEditorInterface>
#include <QtDesigner/QExtensionManager>

#include <QtCore/QVariant>
#include <QtGui/QWidget>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

// ---- QDesignerFormWindowCommand ----
QDesignerFormWindowCommand::QDesignerFormWindowCommand(const QString &description,
                                                       QDesignerFormWindowInterface *formWindow,
                                                       QUndoCommand *parent)
    : QUndoCommand(description, parent),
      m_formWindow(formWindow)
{
}

QDesignerFormWindowInterface *QDesignerFormWindowCommand::formWindow() const
{
    return m_formWindow;
}

QDesignerFormEditorInterface *QDesignerFormWindowCommand::core() const
{
    if (QDesignerFormWindowInterface *fw = formWindow())
        return fw->core();

    return nullptr;
}

void QDesignerFormWindowCommand::undo()
{
    cheapUpdate();
}

void QDesignerFormWindowCommand::redo()
{
    cheapUpdate();
}

void QDesignerFormWindowCommand::cheapUpdate()
{
    if (core()->objectInspector())
        core()->objectInspector()->setFormWindow(formWindow());

    if (core()->actionEditor())
        core()->actionEditor()->setFormWindow(formWindow());
}

QDesignerPropertySheetExtension* QDesignerFormWindowCommand::propertySheet(QObject *object) const
{
    return  qt_extension<QDesignerPropertySheetExtension*>(formWindow()->core()->extensionManager(), object);
}

void QDesignerFormWindowCommand::updateBuddies(QDesignerFormWindowInterface *form,
                                               const QString &old_name,
                                               const QString &new_name)
{
    QExtensionManager* extensionManager = form->core()->extensionManager();

    typedef QList<QLabel*> LabelList;

    const LabelList label_list = form->findChildren<QLabel*>();
    if (label_list.empty())
        return;

    const QString buddyProperty = QLatin1String("buddy");
    const QByteArray oldNameU8 = old_name.toUtf8();
    const QByteArray newNameU8 = new_name.toUtf8();

    const LabelList::const_iterator cend = label_list.constEnd();
    for (LabelList::const_iterator it = label_list.constBegin(); it != cend; ++it ) {
        if (QDesignerPropertySheetExtension* sheet = qt_extension<QDesignerPropertySheetExtension*>(extensionManager, *it)) {
            const int idx = sheet->indexOf(buddyProperty);
            if (idx != -1) {
                const QByteArray oldBuddy = sheet->property(idx).toByteArray();
                if (oldBuddy == oldNameU8)
                    sheet->setProperty(idx, newNameU8);
            }
        }
    }
}

void QDesignerFormWindowCommand::selectUnmanagedObject(QObject *unmanagedObject)
{
    // Keep selection in sync
    if (QDesignerObjectInspector *oi = qobject_cast<QDesignerObjectInspector *>(core()->objectInspector())) {
        oi->clearSelection();
        oi->selectObject(unmanagedObject);
    }
    core()->propertyEditor()->setObject(unmanagedObject);
}

} // namespace qdesigner_internal

QT_END_NAMESPACE

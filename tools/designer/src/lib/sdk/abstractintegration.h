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

#ifndef ABSTRACTINTEGRATION_H
#define ABSTRACTINTEGRATION_H

#include <QtDesigner/sdk_global.h>

#include <QtCore/QObject>
#include <QtCore/QString>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QDesignerFormEditorInterface;

class QDESIGNER_SDK_EXPORT QDesignerIntegrationInterface: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString headerSuffix READ headerSuffix WRITE setHeaderSuffix)
    Q_PROPERTY(bool headerLowercase READ isHeaderLowercase WRITE setHeaderLowercase)

public:
    QDesignerIntegrationInterface(QDesignerFormEditorInterface *core, QObject *parent = nullptr);

    inline QDesignerFormEditorInterface *core() const;

    virtual QWidget *containerWindow(QWidget *widget) const = 0;

    QString headerSuffix() const;
    void setHeaderSuffix(const QString &headerSuffix);

    bool isHeaderLowercase() const;
    void setHeaderLowercase(bool headerLowerCase);

private:
    QDesignerFormEditorInterface *m_core;
};

inline QDesignerFormEditorInterface *QDesignerIntegrationInterface::core() const
{ return m_core; }

QT_END_NAMESPACE

QT_END_HEADER

#endif // ABSTRACTINTEGRATION_H

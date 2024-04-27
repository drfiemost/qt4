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

/*
  cppcodemarker.h
*/

#ifndef CPPCODEMARKER_H
#define CPPCODEMARKER_H

#include "codemarker.h"

QT_BEGIN_NAMESPACE

class CppCodeMarker : public CodeMarker
{
 public:
    CppCodeMarker();
    ~CppCodeMarker() override;

    bool recognizeCode(const QString& code) override;
    bool recognizeExtension(const QString& ext) override;
    bool recognizeLanguage(const QString& lang) override;
    Atom::Type atomType() const override;
    QString plainName(const Node *node) override;
    QString plainFullName(const Node *node, const Node *relative) override;
    QString markedUpCode(const QString& code, 
                                 const Node *relative, 
                                 const Location &location) override;
    QString markedUpSynopsis(const Node *node, 
                                     const Node *relative,
                                     SynopsisStyle style) override;
#ifdef QDOC_QML
    QString markedUpQmlItem(const Node *node, bool summary) override;
#endif
    QString markedUpName(const Node *node) override;
    QString markedUpFullName(const Node *node, const Node *relative) override;
    QString markedUpEnumValue(const QString &enumValue, const Node *relative) override;
    QString markedUpIncludes(const QStringList& includes) override;
    QString functionBeginRegExp(const QString& funcName) override;
    QString functionEndRegExp(const QString& funcName) override;
    QList<Section> sections(const InnerNode *innerNode, 
                                    SynopsisStyle style, 
                                    Status status) override;
    QList<Section> qmlSections(const QmlClassNode* qmlClassNode,
                                       SynopsisStyle style,
                                       const Tree* tree) override;
    const Node* resolveTarget(const QString& target, 
                                      const Tree* tree, 
                                      const Node* relative,
                                      const Node* self = nullptr) override;

private:
    QString addMarkUp(const QString& protectedCode, 
                      const Node *relative, 
                      const Location &location);
};

QT_END_NAMESPACE

#endif

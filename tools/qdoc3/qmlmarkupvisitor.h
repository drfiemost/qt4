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

#ifndef QMLVISITOR_H
#define QMLVISITOR_H

#include <QString>
#include "declarativeparser/qdeclarativejsastvisitor_p.h"
#include "node.h"
#include "tree.h"

QT_BEGIN_NAMESPACE

class QmlMarkupVisitor : public QDeclarativeJS::AST::Visitor
{
public:
    enum ExtraType{
        Comment,
        Pragma
    };

    QmlMarkupVisitor(const QString &code,
                     const QList<QDeclarativeJS::AST::SourceLocation> &pragmas,
                     QDeclarativeJS::Engine *engine);
    ~QmlMarkupVisitor() override;

    QString markedUpCode();

    bool visit(QDeclarativeJS::AST::UiImport *) override;
    void endVisit(QDeclarativeJS::AST::UiImport *) override;

    bool visit(QDeclarativeJS::AST::UiPublicMember *) override;
    bool visit(QDeclarativeJS::AST::UiObjectDefinition *) override;

    bool visit(QDeclarativeJS::AST::UiObjectInitializer *) override;
    void endVisit(QDeclarativeJS::AST::UiObjectInitializer *) override;

    bool visit(QDeclarativeJS::AST::UiObjectBinding *) override;
    bool visit(QDeclarativeJS::AST::UiScriptBinding *) override;
    bool visit(QDeclarativeJS::AST::UiArrayBinding *) override;
    bool visit(QDeclarativeJS::AST::UiArrayMemberList *) override;
    bool visit(QDeclarativeJS::AST::UiQualifiedId *) override;

    bool visit(QDeclarativeJS::AST::UiSignature *) override;
    void endVisit(QDeclarativeJS::AST::UiSignature *) override;

    bool visit(QDeclarativeJS::AST::UiFormal *) override;
    bool visit(QDeclarativeJS::AST::ThisExpression *) override;
    bool visit(QDeclarativeJS::AST::IdentifierExpression *) override;
    bool visit(QDeclarativeJS::AST::NullExpression *) override;
    bool visit(QDeclarativeJS::AST::TrueLiteral *) override;
    bool visit(QDeclarativeJS::AST::FalseLiteral *) override;
    bool visit(QDeclarativeJS::AST::NumericLiteral *) override;
    bool visit(QDeclarativeJS::AST::StringLiteral *) override;
    bool visit(QDeclarativeJS::AST::RegExpLiteral *) override;
    bool visit(QDeclarativeJS::AST::ArrayLiteral *) override;

    bool visit(QDeclarativeJS::AST::ObjectLiteral *) override;
    void endVisit(QDeclarativeJS::AST::ObjectLiteral *) override;

    bool visit(QDeclarativeJS::AST::ElementList *) override;
    bool visit(QDeclarativeJS::AST::Elision *) override;
    bool visit(QDeclarativeJS::AST::PropertyNameAndValueList *) override;
    bool visit(QDeclarativeJS::AST::ArrayMemberExpression *) override;
    bool visit(QDeclarativeJS::AST::FieldMemberExpression *) override;
    bool visit(QDeclarativeJS::AST::NewMemberExpression *) override;
    bool visit(QDeclarativeJS::AST::NewExpression *) override;
    bool visit(QDeclarativeJS::AST::ArgumentList *) override;
    bool visit(QDeclarativeJS::AST::PostIncrementExpression *) override;
    bool visit(QDeclarativeJS::AST::PostDecrementExpression *) override;
    bool visit(QDeclarativeJS::AST::DeleteExpression *) override;
    bool visit(QDeclarativeJS::AST::VoidExpression *) override;
    bool visit(QDeclarativeJS::AST::TypeOfExpression *) override;
    bool visit(QDeclarativeJS::AST::PreIncrementExpression *) override;
    bool visit(QDeclarativeJS::AST::PreDecrementExpression *) override;
    bool visit(QDeclarativeJS::AST::UnaryPlusExpression *) override;
    bool visit(QDeclarativeJS::AST::UnaryMinusExpression *) override;
    bool visit(QDeclarativeJS::AST::TildeExpression *) override;
    bool visit(QDeclarativeJS::AST::NotExpression *) override;
    bool visit(QDeclarativeJS::AST::BinaryExpression *) override;
    bool visit(QDeclarativeJS::AST::ConditionalExpression *) override;
    bool visit(QDeclarativeJS::AST::Expression *) override;

    bool visit(QDeclarativeJS::AST::Block *) override;
    void endVisit(QDeclarativeJS::AST::Block *) override;

    bool visit(QDeclarativeJS::AST::VariableStatement *) override;
    bool visit(QDeclarativeJS::AST::VariableDeclarationList *) override;
    bool visit(QDeclarativeJS::AST::VariableDeclaration *) override;
    bool visit(QDeclarativeJS::AST::EmptyStatement *) override;
    bool visit(QDeclarativeJS::AST::ExpressionStatement *) override;
    bool visit(QDeclarativeJS::AST::IfStatement *) override;
    bool visit(QDeclarativeJS::AST::DoWhileStatement *) override;
    bool visit(QDeclarativeJS::AST::WhileStatement *) override;
    bool visit(QDeclarativeJS::AST::ForStatement *) override;
    bool visit(QDeclarativeJS::AST::LocalForStatement *) override;
    bool visit(QDeclarativeJS::AST::ForEachStatement *) override;
    bool visit(QDeclarativeJS::AST::LocalForEachStatement *) override;
    bool visit(QDeclarativeJS::AST::ContinueStatement *) override;
    bool visit(QDeclarativeJS::AST::BreakStatement *) override;
    bool visit(QDeclarativeJS::AST::ReturnStatement *) override;
    bool visit(QDeclarativeJS::AST::WithStatement *) override;

    bool visit(QDeclarativeJS::AST::CaseBlock *) override;
    void endVisit(QDeclarativeJS::AST::CaseBlock *) override;

    bool visit(QDeclarativeJS::AST::SwitchStatement *) override;
    bool visit(QDeclarativeJS::AST::CaseClause *) override;
    bool visit(QDeclarativeJS::AST::DefaultClause *) override;
    bool visit(QDeclarativeJS::AST::LabelledStatement *) override;
    bool visit(QDeclarativeJS::AST::ThrowStatement *) override;
    bool visit(QDeclarativeJS::AST::TryStatement *) override;
    bool visit(QDeclarativeJS::AST::Catch *) override;
    bool visit(QDeclarativeJS::AST::Finally *) override;
    bool visit(QDeclarativeJS::AST::FunctionDeclaration *) override;
    bool visit(QDeclarativeJS::AST::FunctionExpression *) override;
    bool visit(QDeclarativeJS::AST::FormalParameterList *) override;
    bool visit(QDeclarativeJS::AST::DebuggerStatement *) override;

protected:
    QString protect(const QString &string);

private:
    typedef QHash<QString, QString> StringHash;
    void addExtra(quint32 start, quint32 finish);
    void addMarkedUpToken(QDeclarativeJS::AST::SourceLocation &location,
                          const QString &text,
                          const StringHash &attributes = StringHash());
    void addVerbatim(QDeclarativeJS::AST::SourceLocation first,
                     QDeclarativeJS::AST::SourceLocation last = QDeclarativeJS::AST::SourceLocation());
    QString sourceText(QDeclarativeJS::AST::SourceLocation &location);

    QDeclarativeJS::Engine *engine;
    QList<ExtraType> extraTypes;
    QList<QDeclarativeJS::AST::SourceLocation> extraLocations;
    QString source;
    QString output;
    quint32 cursor;
    int extraIndex;
};

QT_END_NAMESPACE

#endif

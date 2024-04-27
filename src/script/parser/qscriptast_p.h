/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL-ONLY$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** us via http://www.qt.io/contact-us/.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSCRIPTAST_P_H
#define QSCRIPTAST_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/QString>

#include "qscriptastvisitor_p.h"

QT_BEGIN_NAMESPACE

class QScriptNameIdImpl;

namespace QSOperator // ### rename
{

enum Op {
    Add,
    And,
    InplaceAnd,
    Assign,
    BitAnd,
    BitOr,
    BitXor,
    InplaceSub,
    Div,
    InplaceDiv,
    Equal,
    Ge,
    Gt,
    In,
    InplaceAdd,
    InstanceOf,
    Le,
    LShift,
    InplaceLeftShift,
    Lt,
    Mod,
    InplaceMod,
    Mul,
    InplaceMul,
    NotEqual,
    Or,
    InplaceOr,
    RShift,
    InplaceRightShift,
    StrictEqual,
    StrictNotEqual,
    Sub,
    URShift,
    InplaceURightShift,
    InplaceXor
};

} // namespace QSOperator

namespace QScript { namespace AST {

class Node
{
public:
    enum Kind {
        Kind_Node,
        Kind_ExpressionNode,
        Kind_Statement,
        Kind_ThisExpression,
        Kind_IdentifierExpression,
        Kind_NullExpression,
        Kind_TrueLiteral,
        Kind_FalseLiteral,
        Kind_NumericLiteral,
        Kind_StringLiteral,
        Kind_RegExpLiteral,
        Kind_ArrayLiteral,
        Kind_ObjectLiteral,
        Kind_ElementList,
        Kind_Elision,
        Kind_PropertyNameAndValueList,
        Kind_PropertyName,
        Kind_IdentifierPropertyName,
        Kind_StringLiteralPropertyName,
        Kind_NumericLiteralPropertyName,
        Kind_ArrayMemberExpression,
        Kind_FieldMemberExpression,
        Kind_NewMemberExpression,
        Kind_NewExpression,
        Kind_CallExpression,
        Kind_ArgumentList,
        Kind_PostIncrementExpression,
        Kind_PostDecrementExpression,
        Kind_DeleteExpression,
        Kind_VoidExpression,
        Kind_TypeOfExpression,
        Kind_PreIncrementExpression,
        Kind_PreDecrementExpression,
        Kind_UnaryPlusExpression,
        Kind_UnaryMinusExpression,
        Kind_TildeExpression,
        Kind_NotExpression,
        Kind_BinaryExpression,
        Kind_ConditionalExpression,
        Kind_Expression,
        Kind_Block,
        Kind_StatementList,
        Kind_VariableStatement,
        Kind_VariableDeclarationList,
        Kind_VariableDeclaration,
        Kind_EmptyStatement,
        Kind_ExpressionStatement,
        Kind_IfStatement,
        Kind_DoWhileStatement,
        Kind_WhileStatement,
        Kind_ForStatement,
        Kind_LocalForStatement,
        Kind_ForEachStatement,
        Kind_LocalForEachStatement,
        Kind_ContinueStatement,
        Kind_BreakStatement,
        Kind_ReturnStatement,
        Kind_WithStatement,
        Kind_SwitchStatement,
        Kind_CaseBlock,
        Kind_CaseClauses,
        Kind_CaseClause,
        Kind_DefaultClause,
        Kind_LabelledStatement,
        Kind_ThrowStatement,
        Kind_TryStatement,
        Kind_Catch,
        Kind_Finally,
        Kind_FunctionDeclaration,
        Kind_FunctionExpression,
        Kind_FormalParameterList,
        Kind_FunctionBody,
        Kind_Program,
        Kind_SourceElements,
        Kind_SourceElement,
        Kind_FunctionSourceElement,
        Kind_StatementSourceElement,
        Kind_DebuggerStatement
    };

    inline Node():
        startLine(0), startColumn(0),
        endLine(0), endColumn(0), kind(Kind_Node) {}

    virtual ~Node() {}

    virtual ExpressionNode *expressionCast();
    virtual BinaryExpression *binaryExpressionCast();
    virtual Statement *statementCast();

    inline void accept(Visitor *visitor)
    {
        if (visitor->preVisit(this)) {
            accept0(visitor);
            visitor->postVisit(this);
        }
    }

    static void acceptChild(Node *node, Visitor *visitor)
    {
        if (node)
            node->accept(visitor);
    }

    virtual void accept0(Visitor *visitor) = 0;

    int startLine;
    int startColumn;
    int endLine;
    int endColumn;
    Kind kind;
};

class ExpressionNode: public Node
{
public:
    ExpressionNode() { kind = Kind_ExpressionNode; }
    ~ExpressionNode() override {}

    ExpressionNode *expressionCast() override;
};

class Statement: public Node
{
public:
    Statement() { kind = Kind_Statement; }
    ~Statement() override {}

    Statement *statementCast() override;
};

class ThisExpression: public ExpressionNode
{
public:
    ThisExpression() { kind = Kind_ThisExpression; }
    ~ThisExpression() override {}

    void accept0(Visitor *visitor) override;
};

class IdentifierExpression: public ExpressionNode
{
public:
    IdentifierExpression(QScriptNameIdImpl *n):
        name (n) { kind = Kind_IdentifierExpression; }

    ~IdentifierExpression() override {}

    void accept0(Visitor *visitor) override;

// attributes
    QScriptNameIdImpl *name;
};

class NullExpression: public ExpressionNode
{
public:
    NullExpression() { kind = Kind_NullExpression; }
    ~NullExpression() override {}

    void accept0(Visitor *visitor) override;
};

class TrueLiteral: public ExpressionNode
{
public:
    TrueLiteral() { kind = Kind_TrueLiteral; }
    ~TrueLiteral() override {}

    void accept0(Visitor *visitor) override;
};

class FalseLiteral: public ExpressionNode
{
public:
    FalseLiteral() { kind = Kind_FalseLiteral; }
    ~FalseLiteral() override {}

    void accept0(Visitor *visitor) override;
};

class NumericLiteral: public ExpressionNode
{
public:
    NumericLiteral(double v):
        value (v) { kind = Kind_NumericLiteral; }
    ~NumericLiteral() override {}

    void accept0(Visitor *visitor) override;

// attributes:
    double value;
};

class StringLiteral: public ExpressionNode
{
public:
    StringLiteral(QScriptNameIdImpl *v):
        value (v) { kind = Kind_StringLiteral; }

    ~StringLiteral() override {}

    void accept0(Visitor *visitor) override;

// attributes:
    QScriptNameIdImpl *value;
};

class RegExpLiteral: public ExpressionNode
{
public:
    RegExpLiteral(QScriptNameIdImpl *p, int f):
        pattern (p), flags (f) { kind = Kind_RegExpLiteral; }

    ~RegExpLiteral() override {}

    void accept0(Visitor *visitor) override;

// attributes:
    QScriptNameIdImpl *pattern;
    int flags;
};

class ArrayLiteral: public ExpressionNode
{
public:
    ArrayLiteral(Elision *e):
        elements (nullptr), elision (e)
        { kind = Kind_ArrayLiteral; }

    ArrayLiteral(ElementList *elts):
        elements (elts), elision (nullptr)
        { kind = Kind_ArrayLiteral; }

    ArrayLiteral(ElementList *elts, Elision *e):
        elements (elts), elision (e)
        { kind = Kind_ArrayLiteral; }

    ~ArrayLiteral() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ElementList *elements;
    Elision *elision;
};

class ObjectLiteral: public ExpressionNode
{
public:
    ObjectLiteral():
        properties (nullptr) { kind = Kind_ObjectLiteral; }

    ObjectLiteral(PropertyNameAndValueList *plist):
        properties (plist) { kind = Kind_ObjectLiteral; }

    ~ObjectLiteral() override {}

    void accept0(Visitor *visitor) override;

// attributes
    PropertyNameAndValueList *properties;
};

class ElementList: public Node
{
public:
    ElementList(Elision *e, ExpressionNode *expr):
        elision (e), expression (expr), next (this)
        { kind = Kind_ElementList; }

    ElementList(ElementList *previous, Elision *e, ExpressionNode *expr):
        elision (e), expression (expr)
    {
        kind = Kind_ElementList;
        next = previous->next;
        previous->next = this;
    }

    ~ElementList() override {}

    inline ElementList *finish ()
    {
        ElementList *front = next;
        next = nullptr;
        return front;
    }

    void accept0(Visitor *visitor) override;

// attributes
    Elision *elision;
    ExpressionNode *expression;
    ElementList *next;
};

class Elision: public Node
{
public:
    Elision():
        next (this) { kind = Kind_Elision; }

    Elision(Elision *previous)
    {
        kind = Kind_Elision;
        next = previous->next;
        previous->next = this;
    }

    ~Elision() override {}

    void accept0(Visitor *visitor) override;

    inline Elision *finish ()
    {
        Elision *front = next;
        next = nullptr;
        return front;
    }

// attributes
    Elision *next;
};

class PropertyNameAndValueList: public Node
{
public:
    PropertyNameAndValueList(PropertyName *n, ExpressionNode *v):
        name (n), value (v), next (this)
        { kind = Kind_PropertyNameAndValueList; }

    PropertyNameAndValueList(PropertyNameAndValueList *previous, PropertyName *n, ExpressionNode *v):
        name (n), value (v)
    {
        kind = Kind_PropertyNameAndValueList;
        next = previous->next;
        previous->next = this;
    }

    ~PropertyNameAndValueList() override {}

    void accept0(Visitor *visitor) override;

    inline PropertyNameAndValueList *finish ()
    {
        PropertyNameAndValueList *front = next;
        next = nullptr;
        return front;
    }

// attributes
    PropertyName *name;
    ExpressionNode *value;
    PropertyNameAndValueList *next;
};

class PropertyName: public Node
{
public:
    PropertyName() { kind = Kind_PropertyName; }
    ~PropertyName() override {}
};

class IdentifierPropertyName: public PropertyName
{
public:
    IdentifierPropertyName(QScriptNameIdImpl *n):
        id (n) { kind = Kind_IdentifierPropertyName; }

    ~IdentifierPropertyName() override {}

    void accept0(Visitor *visitor) override;

// attributes
    QScriptNameIdImpl *id;
};

class StringLiteralPropertyName: public PropertyName
{
public:
    StringLiteralPropertyName(QScriptNameIdImpl *n):
        id (n) { kind = Kind_StringLiteralPropertyName; }
    ~StringLiteralPropertyName() override {}

    void accept0(Visitor *visitor) override;

// attributes
    QScriptNameIdImpl *id;
};

class NumericLiteralPropertyName: public PropertyName
{
public:
    NumericLiteralPropertyName(double n):
        id (n) { kind = Kind_NumericLiteralPropertyName; }
    ~NumericLiteralPropertyName() override {}

    void accept0(Visitor *visitor) override;

// attributes
    double id;
};

class ArrayMemberExpression: public ExpressionNode
{
public:
    ArrayMemberExpression(ExpressionNode *b, ExpressionNode *e):
        base (b), expression (e)
        { kind = Kind_ArrayMemberExpression; }

    ~ArrayMemberExpression() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *base;
    ExpressionNode *expression;
};

class FieldMemberExpression: public ExpressionNode
{
public:
    FieldMemberExpression(ExpressionNode *b, QScriptNameIdImpl *n):
        base (b), name (n)
        { kind = Kind_FieldMemberExpression; }

    ~FieldMemberExpression() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *base;
    QScriptNameIdImpl *name;
};

class NewMemberExpression: public ExpressionNode
{
public:
    NewMemberExpression(ExpressionNode *b, ArgumentList *a):
        base (b), arguments (a)
        { kind = Kind_NewMemberExpression; }

    ~NewMemberExpression() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *base;
    ArgumentList *arguments;
};

class NewExpression: public ExpressionNode
{
public:
    NewExpression(ExpressionNode *e):
        expression (e) { kind = Kind_NewExpression; }

    ~NewExpression() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *expression;
};

class CallExpression: public ExpressionNode
{
public:
    CallExpression(ExpressionNode *b, ArgumentList *a):
        base (b), arguments (a)
        { kind = Kind_CallExpression; }

    ~CallExpression() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *base;
    ArgumentList *arguments;
};

class ArgumentList: public Node
{
public:
    ArgumentList(ExpressionNode *e):
        expression (e), next (this)
        { kind = Kind_ArgumentList; }

    ArgumentList(ArgumentList *previous, ExpressionNode *e):
        expression (e)
    {
        kind = Kind_ArgumentList;
        next = previous->next;
        previous->next = this;
    }

    ~ArgumentList() override {}

    void accept0(Visitor *visitor) override;

    inline ArgumentList *finish ()
    {
        ArgumentList *front = next;
        next = nullptr;
        return front;
    }

// attributes
    ExpressionNode *expression;
    ArgumentList *next;
};

class PostIncrementExpression: public ExpressionNode
{
public:
    PostIncrementExpression(ExpressionNode *b):
        base (b) { kind = Kind_PostIncrementExpression; }

    ~PostIncrementExpression() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *base;
};

class PostDecrementExpression: public ExpressionNode
{
public:
    PostDecrementExpression(ExpressionNode *b):
        base (b) { kind = Kind_PostDecrementExpression; }

    ~PostDecrementExpression() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *base;
};

class DeleteExpression: public ExpressionNode
{
public:
    DeleteExpression(ExpressionNode *e):
        expression (e) { kind = Kind_DeleteExpression; }
    ~DeleteExpression() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *expression;
};

class VoidExpression: public ExpressionNode
{
public:
    VoidExpression(ExpressionNode *e):
        expression (e) { kind = Kind_VoidExpression; }

    ~VoidExpression() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *expression;
};

class TypeOfExpression: public ExpressionNode
{
public:
    TypeOfExpression(ExpressionNode *e):
        expression (e) { kind = Kind_TypeOfExpression; }

    ~TypeOfExpression() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *expression;
};

class PreIncrementExpression: public ExpressionNode
{
public:
    PreIncrementExpression(ExpressionNode *e):
        expression (e) { kind = Kind_PreIncrementExpression; }

    ~PreIncrementExpression() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *expression;
};

class PreDecrementExpression: public ExpressionNode
{
public:
    PreDecrementExpression(ExpressionNode *e):
        expression (e) { kind = Kind_PreDecrementExpression; }

    ~PreDecrementExpression() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *expression;
};

class UnaryPlusExpression: public ExpressionNode
{
public:
    UnaryPlusExpression(ExpressionNode *e):
        expression (e) { kind = Kind_UnaryPlusExpression; }

    ~UnaryPlusExpression() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *expression;
};

class UnaryMinusExpression: public ExpressionNode
{
public:
    UnaryMinusExpression(ExpressionNode *e):
        expression (e) { kind = Kind_UnaryMinusExpression; }

    ~UnaryMinusExpression() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *expression;
};

class TildeExpression: public ExpressionNode
{
public:
    TildeExpression(ExpressionNode *e):
        expression (e) { kind = Kind_TildeExpression; }

    ~TildeExpression() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *expression;
};

class NotExpression: public ExpressionNode
{
public:
    NotExpression(ExpressionNode *e):
        expression (e) { kind = Kind_NotExpression; }

    ~NotExpression() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *expression;
};

class BinaryExpression: public ExpressionNode
{
public:
    BinaryExpression(ExpressionNode *l, int o, ExpressionNode *r):
        left (l), op (o), right (r)
        { kind = Kind_BinaryExpression; }

    ~BinaryExpression() override {}

    BinaryExpression *binaryExpressionCast() override;

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *left;
    int op;
    ExpressionNode *right;
};

class ConditionalExpression: public ExpressionNode
{
public:
    ConditionalExpression(ExpressionNode *e, ExpressionNode *t, ExpressionNode *f):
        expression (e), ok (t), ko (f)
        { kind = Kind_ConditionalExpression; }

    ~ConditionalExpression() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *expression;
    ExpressionNode *ok;
    ExpressionNode *ko;
};

class Expression: public ExpressionNode // ### rename
{
public:
    Expression(ExpressionNode *l, ExpressionNode *r):
        left (l), right (r) { kind = Kind_Expression; }

    ~Expression() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *left;
    ExpressionNode *right;
};

class Block: public Statement
{
public:
    Block(StatementList *slist):
        statements (slist) { kind = Kind_Block; }

    ~Block() override {}

    void accept0(Visitor *visitor) override;

// attributes
    StatementList *statements;
};

class StatementList: public Node
{
public:
    StatementList(Statement *stmt):
        statement (stmt), next (this)
        { kind = Kind_StatementList; }

    StatementList(StatementList *previous, Statement *stmt):
        statement (stmt)
    {
        kind = Kind_StatementList;
        next = previous->next;
        previous->next = this;
    }

    ~StatementList() override {}

    void accept0(Visitor *visitor) override;

    inline StatementList *finish ()
    {
        StatementList *front = next;
        next = nullptr;
        return front;
    }

// attributes
    Statement *statement;
    StatementList *next;
};

class VariableStatement: public Statement
{
public:
    VariableStatement(VariableDeclarationList *vlist):
        declarations (vlist)
        { kind = Kind_VariableStatement; }

    ~VariableStatement() override {}

    void accept0(Visitor *visitor) override;

// attributes
    VariableDeclarationList *declarations;
};

class VariableDeclaration: public Node
{
public:
    VariableDeclaration(QScriptNameIdImpl *n, ExpressionNode *e):
        name (n), expression (e), readOnly(false)
        { kind = Kind_VariableDeclaration; }

    ~VariableDeclaration() override {}

    void accept0(Visitor *visitor) override;

// attributes
    QScriptNameIdImpl *name;
    ExpressionNode *expression;
    bool readOnly;
};

class VariableDeclarationList: public Node
{
public:
    VariableDeclarationList(VariableDeclaration *decl):
        declaration (decl), next (this)
        { kind = Kind_VariableDeclarationList; }

    VariableDeclarationList(VariableDeclarationList *previous, VariableDeclaration *decl):
        declaration (decl)
    {
        kind = Kind_VariableDeclarationList;
        next = previous->next;
        previous->next = this;
    }

    ~VariableDeclarationList() override {}

    void accept0(Visitor *visitor) override;

    inline VariableDeclarationList *finish (bool readOnly)
    {
        VariableDeclarationList *front = next;
        next = nullptr;
        if (readOnly) {
            VariableDeclarationList *vdl;
            for (vdl = front; vdl != nullptr; vdl = vdl->next)
                vdl->declaration->readOnly = true;
        }
        return front;
    }

// attributes
    VariableDeclaration *declaration;
    VariableDeclarationList *next;
};

class EmptyStatement: public Statement
{
public:
    EmptyStatement() { kind = Kind_EmptyStatement; }
    ~EmptyStatement() override {}

    void accept0(Visitor *visitor) override;
};

class ExpressionStatement: public Statement
{
public:
    ExpressionStatement(ExpressionNode *e):
        expression (e) { kind = Kind_ExpressionStatement; }

    ~ExpressionStatement() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *expression;
};

class IfStatement: public Statement
{
public:
    IfStatement(ExpressionNode *e, Statement *t, Statement *f = nullptr):
        expression (e), ok (t), ko (f)
        { kind = Kind_IfStatement; }

    ~IfStatement() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *expression;
    Statement *ok;
    Statement *ko;
};

class DoWhileStatement: public Statement
{
public:
    DoWhileStatement(Statement *stmt, ExpressionNode *e):
        statement (stmt), expression (e)
        { kind = Kind_DoWhileStatement; }

    ~DoWhileStatement() override {}

    void accept0(Visitor *visitor) override;

// attributes
    Statement *statement;
    ExpressionNode *expression;
};

class WhileStatement: public Statement
{
public:
    WhileStatement(ExpressionNode *e, Statement *stmt):
        expression (e), statement (stmt)
        { kind = Kind_WhileStatement; }

    ~WhileStatement() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *expression;
    Statement *statement;
};

class ForStatement: public Statement
{
public:
    ForStatement(ExpressionNode *i, ExpressionNode *c, ExpressionNode *e, Statement *stmt):
        initialiser (i), condition (c), expression (e), statement (stmt)
        { kind = Kind_ForStatement; }

    ~ForStatement() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *initialiser;
    ExpressionNode *condition;
    ExpressionNode *expression;
    Statement *statement;
};

class LocalForStatement: public Statement
{
public:
    LocalForStatement(VariableDeclarationList *vlist, ExpressionNode *c, ExpressionNode *e, Statement *stmt):
        declarations (vlist), condition (c), expression (e), statement (stmt)
        { kind = Kind_LocalForStatement; }

    ~LocalForStatement() override {}

    void accept0(Visitor *visitor) override;

// attributes
    VariableDeclarationList *declarations;
    ExpressionNode *condition;
    ExpressionNode *expression;
    Statement *statement;
};

class ForEachStatement: public Statement
{
public:
    ForEachStatement(ExpressionNode *i, ExpressionNode *e, Statement *stmt):
        initialiser (i), expression (e), statement (stmt)
        { kind = Kind_ForEachStatement; }

    ~ForEachStatement() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *initialiser;
    ExpressionNode *expression;
    Statement *statement;
};

class LocalForEachStatement: public Statement
{
public:
    LocalForEachStatement(VariableDeclaration *v, ExpressionNode *e, Statement *stmt):
        declaration (v), expression (e), statement (stmt)
        { kind = Kind_LocalForEachStatement; }

    ~LocalForEachStatement() override {}

    void accept0(Visitor *visitor) override;

// attributes
    VariableDeclaration *declaration;
    ExpressionNode *expression;
    Statement *statement;
};

class ContinueStatement: public Statement
{
public:
    ContinueStatement(QScriptNameIdImpl *l = nullptr):
        label (l) { kind = Kind_ContinueStatement; }

    ~ContinueStatement() override {}

    void accept0(Visitor *visitor) override;

// attributes
    QScriptNameIdImpl *label;
};

class BreakStatement: public Statement
{
public:
    BreakStatement(QScriptNameIdImpl *l = nullptr):
        label (l) { kind = Kind_BreakStatement; }

    ~BreakStatement() override {}

    void accept0(Visitor *visitor) override;

// attributes
    QScriptNameIdImpl *label;
};

class ReturnStatement: public Statement
{
public:
    ReturnStatement(ExpressionNode *e):
        expression (e) { kind = Kind_ReturnStatement; }

    ~ReturnStatement() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *expression;
};

class WithStatement: public Statement
{
public:
    WithStatement(ExpressionNode *e, Statement *stmt):
        expression (e), statement (stmt)
        { kind = Kind_WithStatement; }

    ~WithStatement() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *expression;
    Statement *statement;
};

class SwitchStatement: public Statement
{
public:
    SwitchStatement(ExpressionNode *e, CaseBlock *b):
        expression (e), block (b)
        { kind = Kind_SwitchStatement; }

    ~SwitchStatement() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *expression;
    CaseBlock *block;
};

class CaseBlock: public Node
{
public:
    CaseBlock(CaseClauses *c, DefaultClause *d = nullptr, CaseClauses *r = nullptr):
        clauses (c), defaultClause (d), moreClauses (r)
        { kind = Kind_CaseBlock; }

    ~CaseBlock() override {}

    void accept0(Visitor *visitor) override;

// attributes
    CaseClauses *clauses;
    DefaultClause *defaultClause;
    CaseClauses *moreClauses;
};

class CaseClauses: public Node
{
public:
    CaseClauses(CaseClause *c):
        clause (c), next (this)
        { kind = Kind_CaseClauses; }

    CaseClauses(CaseClauses *previous, CaseClause *c):
        clause (c)
    {
        kind = Kind_CaseClauses;
        next = previous->next;
        previous->next = this;
    }

    ~CaseClauses() override {}

    void accept0(Visitor *visitor) override;

    inline CaseClauses *finish ()
    {
        CaseClauses *front = next;
        next = nullptr;
        return front;
    }

//attributes
    CaseClause *clause;
    CaseClauses *next;
};

class CaseClause: public Node
{
public:
    CaseClause(ExpressionNode *e, StatementList *slist):
        expression (e), statements (slist)
        { kind = Kind_CaseClause; }

    ~CaseClause() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *expression;
    StatementList *statements;
};

class DefaultClause: public Node
{
public:
    DefaultClause(StatementList *slist):
        statements (slist)
        { kind = Kind_DefaultClause; }

    ~DefaultClause() override {}

    void accept0(Visitor *visitor) override;

// attributes
    StatementList *statements;
};

class LabelledStatement: public Statement
{
public:
    LabelledStatement(QScriptNameIdImpl *l, Statement *stmt):
        label (l), statement (stmt)
        { kind = Kind_LabelledStatement; }

    ~LabelledStatement() override {}

    void accept0(Visitor *visitor) override;

// attributes
    QScriptNameIdImpl *label;
    Statement *statement;
};

class ThrowStatement: public Statement
{
public:
    ThrowStatement(ExpressionNode *e):
        expression (e) { kind = Kind_ThrowStatement; }

    ~ThrowStatement() override {}

    void accept0(Visitor *visitor) override;

// attributes
    ExpressionNode *expression;
};

class TryStatement: public Statement
{
public:
    TryStatement(Statement *stmt, Catch *c, Finally *f):
        statement (stmt), catchExpression (c), finallyExpression (f)
        { kind = Kind_TryStatement; }

    TryStatement(Statement *stmt, Finally *f):
        statement (stmt), catchExpression (nullptr), finallyExpression (f)
        { kind = Kind_TryStatement; }

    TryStatement(Statement *stmt, Catch *c):
        statement (stmt), catchExpression (c), finallyExpression (nullptr)
        { kind = Kind_TryStatement; }

    ~TryStatement() override {}

    void accept0(Visitor *visitor) override;

// attributes
    Statement *statement;
    Catch *catchExpression;
    Finally *finallyExpression;
};

class Catch: public Node
{
public:
    Catch(QScriptNameIdImpl *n, Statement *stmt):
        name (n), statement (stmt)
        { kind = Kind_Catch; }

    ~Catch() override {}

    void accept0(Visitor *visitor) override;

// attributes
    QScriptNameIdImpl *name;
    Statement *statement;
};

class Finally: public Node
{
public:
    Finally(Statement *stmt):
        statement (stmt)
        { kind = Kind_Finally; }

    ~Finally() override {}

    void accept0(Visitor *visitor) override;

// attributes
    Statement *statement;
};

class FunctionExpression: public ExpressionNode
{
public:
    FunctionExpression(QScriptNameIdImpl *n, FormalParameterList *f, FunctionBody *b):
        name (n), formals (f), body (b)
        { kind = Kind_FunctionExpression; }

    ~FunctionExpression() override {}

    void accept0(Visitor *visitor) override;

// attributes
    QScriptNameIdImpl *name;
    FormalParameterList *formals;
    FunctionBody *body;
};

class FunctionDeclaration: public FunctionExpression
{
public:
    FunctionDeclaration(QScriptNameIdImpl *n, FormalParameterList *f, FunctionBody *b):
        FunctionExpression(n, f, b)
        { kind = Kind_FunctionDeclaration; }

    ~FunctionDeclaration() override {}

    void accept0(Visitor *visitor) override;
};

class FormalParameterList: public Node
{
public:
    FormalParameterList(QScriptNameIdImpl *n):
        name (n), next (this)
        { kind = Kind_FormalParameterList; }

    FormalParameterList(FormalParameterList *previous, QScriptNameIdImpl *n):
        name (n)
    {
        kind = Kind_FormalParameterList;
        next = previous->next;
        previous->next = this;
    }

    ~FormalParameterList() override {}

    void accept0(Visitor *visitor) override;

    inline FormalParameterList *finish ()
    {
        FormalParameterList *front = next;
        next = nullptr;
        return front;
    }

// attributes
    QScriptNameIdImpl *name;
    FormalParameterList *next;
};

class FunctionBody: public Node
{
public:
    FunctionBody(SourceElements *elts):
        elements (elts)
        { kind = Kind_FunctionBody; }

    ~FunctionBody() override {}

    void accept0(Visitor *visitor) override;

// attributes
    SourceElements *elements;
};

class Program: public Node
{
public:
    Program(SourceElements *elts):
        elements (elts)
        { kind = Kind_Program; }

    ~Program() override {}

    void accept0(Visitor *visitor) override;

// attributes
    SourceElements *elements;
};

class SourceElements: public Node
{
public:
    SourceElements(SourceElement *elt):
        element (elt), next (this)
        { kind = Kind_SourceElements; }

    SourceElements(SourceElements *previous, SourceElement *elt):
        element (elt)
    {
        kind = Kind_SourceElements;
        next = previous->next;
        previous->next = this;
    }

    ~SourceElements() override {}

    void accept0(Visitor *visitor) override;

    inline SourceElements *finish ()
    {
        SourceElements *front = next;
        next = nullptr;
        return front;
    }

// attributes
    SourceElement *element;
    SourceElements *next;
};

class SourceElement: public Node
{
public:
    inline SourceElement()
        { kind = Kind_SourceElement; }

    ~SourceElement() override {}
};

class FunctionSourceElement: public SourceElement
{
public:
    FunctionSourceElement(FunctionDeclaration *f):
        declaration (f)
        { kind = Kind_FunctionSourceElement; }

    ~FunctionSourceElement() override {}

    void accept0(Visitor *visitor) override;

// attributes
    FunctionDeclaration *declaration;
};

class StatementSourceElement: public SourceElement
{
public:
    StatementSourceElement(Statement *stmt):
        statement (stmt)
        { kind = Kind_StatementSourceElement; }

    ~StatementSourceElement() override {}

    void accept0(Visitor *visitor) override;

// attributes
    Statement *statement;
};

class DebuggerStatement: public Statement
{
public:
    DebuggerStatement()
        { kind = Kind_DebuggerStatement; }

    ~DebuggerStatement() override {}

    void accept0(Visitor *visitor) override;
};

} } // namespace AST

QT_END_NAMESPACE

#endif

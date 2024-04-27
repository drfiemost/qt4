/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#ifndef QDECLARATIVEREWRITE_P_H
#define QDECLARATIVEREWRITE_P_H

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

#include "rewriter/textwriter_p.h"
#include "parser/qdeclarativejslexer_p.h"
#include "parser/qdeclarativejsparser_p.h"
#include "parser/qdeclarativejsnodepool_p.h"

QT_BEGIN_NAMESPACE

namespace QDeclarativeRewrite {
using namespace QDeclarativeJS;

class SharedBindingTester : protected AST::Visitor
{
    bool _sharable;
public:
    bool isSharable(const QString &code);
    bool isSharable(AST::Node *Node);
    
    bool visit(AST::FunctionDeclaration *) override { _sharable = false; return false; }
    bool visit(AST::FunctionExpression *) override { _sharable = false; return false; }
    bool visit(AST::CallExpression *) override { _sharable = false; return false; }
};

class RewriteBinding: protected AST::Visitor
{
    unsigned _position;
    TextWriter *_writer;
    QByteArray _name;

public:
    QString operator()(const QString &code, bool *ok = nullptr, bool *sharable = nullptr);
    QString operator()(QDeclarativeJS::AST::Node *node, const QString &code, bool *sharable = nullptr);

    //name of the function:  used for the debugger
    void setName(const QByteArray &name) { _name = name; }

protected:
    using AST::Visitor::visit;

    void accept(AST::Node *node);
    QString rewrite(QString code, unsigned position, AST::Statement *node);

    bool visit(AST::Block *ast) override;
    bool visit(AST::ExpressionStatement *ast) override;

    bool visit(AST::DoWhileStatement *ast) override;
    void endVisit(AST::DoWhileStatement *ast) override;

    bool visit(AST::WhileStatement *ast) override;
    void endVisit(AST::WhileStatement *ast) override;

    bool visit(AST::ForStatement *ast) override;
    void endVisit(AST::ForStatement *ast) override;

    bool visit(AST::LocalForStatement *ast) override;
    void endVisit(AST::LocalForStatement *ast) override;

    bool visit(AST::ForEachStatement *ast) override;
    void endVisit(AST::ForEachStatement *ast) override;

    bool visit(AST::LocalForEachStatement *ast) override;
    void endVisit(AST::LocalForEachStatement *ast) override;

private:
    int _inLoop;
};

} // namespace QDeclarativeRewrite

QT_END_NAMESPACE

#endif // QDECLARATIVEREWRITE_P_H


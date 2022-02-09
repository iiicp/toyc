/**********************************
* File:     StmtSema.cpp.c
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2022/1/31
***********************************/

#include "Sema.h"

namespace CCC
{
  void Sema::VisitorExprStmtNode(ExprStmtNode *node) {
    if (node->Lhs) {
      node->Lhs->Accept(this);
    }
  }
  void Sema::VisitorIfStmtNode(IfStmtNode *node) {
    node->Cond->Accept(this);
    node->Then->Accept(this);
    if (node->Else)
      node->Else->Accept(this);
  }
  void Sema::VisitorWhileStmtNode(WhileStmtNode *node) {
    node->Cond->Accept(this);
    node->Then->Accept(this);
  }
  void Sema::VisitorDoWhileStmtNode(DoWhileStmtNode *node) {
    node->Stmt->Accept(this);
    node->Cond->Accept(this);
  }
  void Sema::VisitorForStmtNode(ForStmtNode *node) {
    if (node->Init)
      node->Init->Accept(this);
    if (node->Cond)
      node->Cond->Accept(this);
    if (node->Inc)
      node->Inc->Accept(this);
    node->Stmt->Accept(this);
  }
  void Sema::VisitorBlockStmtNode(BlockStmtNode *node) {

    if (node->needEnterScope)
      SymTable.EnterScope();

    for (auto &decl : node->Decls) {
        decl->Accept(this);
    }

    for (auto &stmt : node->Stmts) {
        stmt->Accept(this);
    }

    SymTable.ExitScope();
  }

  void Sema::VisitorReturnStmtNode(ReturnStmtNode *node) {
    if (node->Lhs) {
      node->Lhs->Accept(this);
    }
  }
}
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
    LoopStmts.push(node);
    Breakables.push(node);

    node->Cond->Accept(this);
    node->Then->Accept(this);

    LoopStmts.pop();
    Breakables.pop();
  }
  void Sema::VisitorDoWhileStmtNode(DoWhileStmtNode *node) {

    LoopStmts.push(node);
    Breakables.push(node);

    node->Stmt->Accept(this);
    node->Cond->Accept(this);

    LoopStmts.pop();
    Breakables.pop();
  }

  void Sema::VisitorForStmtNode(ForStmtNode *node) {
    LoopStmts.push(node);
    Breakables.push(node);

    if (node->InitExpr)
      node->InitExpr->Accept(this);
    else if (node->InitDecl) {
      SymTable.EnterScope();
      node->InitDecl->Accept(this);
    }
    if (node->Cond)
      node->Cond->Accept(this);
    if (node->Inc)
      node->Inc->Accept(this);

    /// 如果语句是block语句，就不要重复进入作用域
    if (node->Stmt->Cls == StmtNodeClass::BlockStmtNode && node->InitDecl) {
      auto blockStmt = std::dynamic_pointer_cast<BlockStmtNode>(node->Stmt);
      blockStmt->needEnterScope = false;
    }
    node->Stmt->Accept(this);

    if (node->InitDecl) {
      SymTable.ExitScope();
    }

    LoopStmts.pop();
    Breakables.pop();
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

    if (node->needEnterScope)
      SymTable.ExitScope();
  }

  void Sema::VisitorReturnStmtNode(ReturnStmtNode *node) {
    if (node->Lhs) {
      node->Lhs->Accept(this);
    }
  }

  void Sema::VisitorBreakStmtNode(BreakStmtNode *node) {
    if (Breakables.empty()) {
      SemaDiag(node->Tok->Location, "break shall in loop or switch stmt");
    }
  }

  void Sema::VisitorContinueStmtNode(ContinueStmtNode *node) {
    if (LoopStmts.empty()) {
      SemaDiag(node->Tok->Location, "continue shall in loop stmt");
    }
  }
}
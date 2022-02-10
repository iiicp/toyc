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
    CurrentFunc->LoopStmts.push(node);
    CurrentFunc->Breakables.push(node);

    node->Cond->Accept(this);
    node->Then->Accept(this);

    CurrentFunc->LoopStmts.pop();
    CurrentFunc->Breakables.pop();
  }
  void Sema::VisitorDoWhileStmtNode(DoWhileStmtNode *node) {

    CurrentFunc->LoopStmts.push(node);
    CurrentFunc->Breakables.push(node);

    node->Stmt->Accept(this);
    node->Cond->Accept(this);

    CurrentFunc->LoopStmts.pop();
    CurrentFunc->Breakables.pop();
  }

  void Sema::VisitorForStmtNode(ForStmtNode *node) {
    CurrentFunc->LoopStmts.push(node);
    CurrentFunc->Breakables.push(node);

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

    CurrentFunc->LoopStmts.pop();
    CurrentFunc->Breakables.pop();
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
    if (CurrentFunc->Breakables.empty()) {
      SemaDiag(node->Tok->Location, "break shall in loop or switch stmt");
    }
  }

  void Sema::VisitorContinueStmtNode(ContinueStmtNode *node) {
    if (CurrentFunc->LoopStmts.empty()) {
      SemaDiag(node->Tok->Location, "continue shall in loop stmt");
    }
  }

  void Sema::VisitorGotoStmtNode(GotoStmtNode *node) {
    if (CurrentFunc->LabelsMap.find(node->LabelName) == CurrentFunc->LabelsMap.end()) {
      CurrentFunc->LabelsMap[node->LabelName] = std::make_shared<Label>(node->LabelName);
    }
  }

  void Sema::VisitorLabelStmtNode(LabelStmtNode *node) {
    if (CurrentFunc->LabelsMap.find(node->LabelName) != CurrentFunc->LabelsMap.end()) {
      auto lab = CurrentFunc->LabelsMap[node->LabelName];
      if (lab->Defined) {
        SemaDiag(node->Tok->Location, "redefinition of label");
      }
      lab->Defined = true;
    }else {
      auto lab = std::make_shared<Label>(node->LabelName);
      lab->Defined = true;
      CurrentFunc->LabelsMap[node->LabelName] = lab;
    }
    node->Stmt->Accept(this);
  }

  void Sema::VisitorCaseStmtNode(CaseStmtNode *node) {
    if (CurrentFunc->SwitchStmts.empty()) {
      SemaDiag(node->Tok->Location, "'case' statement not in switch statement");
    }
    node->Expr->Accept(this);
    if (!node->Expr->Ty->IsIntegerTy()) {
      SemaDiag(node->Tok->Location, "case expr value should be integer");
    }
    node->Stmt->Accept(this);

    auto &switchNode = CurrentFunc->SwitchStmts.top();
    switchNode->CaseStmtList.push_back(node);
  }

  void Sema::VisitorDefaultStmtNode(DefaultStmtNode *node) {
    if (CurrentFunc->SwitchStmts.empty()) {
      SemaDiag(node->Tok->Location, "'default' statement not in switch statement");
    }

    auto &switchNode = CurrentFunc->SwitchStmts.top();
    if (switchNode->DefaultStmt) {
      SemaDiag(node->Tok->Location, "multiple default labels in one switch");
    }
    switchNode->DefaultStmt = node;

    node->Stmt->Accept(this);
  }

  void Sema::VisitorSwitchStmtNode(SwitchStmtNode *node) {
    CurrentFunc->SwitchStmts.push(node);
    CurrentFunc->Breakables.push(node);

    node->Expr->Accept(this);
    if (!node->Expr->Ty->IsIntegerTy()) {
      SemaDiag(node->Tok->Location, "switch expr value should be integer");
    }
    node->Stmt->Accept(this);

    CurrentFunc->SwitchStmts.pop();
    CurrentFunc->Breakables.pop();
  }
}
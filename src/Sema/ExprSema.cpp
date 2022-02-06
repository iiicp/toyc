/**********************************
* File:     ExprSema.cpp.c
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
  void Sema::VisitorAssignExprNode(AssignExpr *node) {
    node->Lhs->Accept(this);
    node->Rhs->Accept(this);
    node->Ty = node->Lhs->Ty;
  }

  void Sema::VisitorSizeOfExprNode(SizeOfExpr *node) {
    node->Lhs->Accept(this);
    node->Ty = node->Lhs->Ty;
  }

  void Sema::VisitorBinaryExprNode(BinaryExpr *node) {
    switch (node->BinOp) {
    case BinaryOperator::Add:
      CheckAddOperator(node);
      break;
    case BinaryOperator::Sub:
      CheckSubOperator(node);
      break;
    case BinaryOperator::Mul:
    case BinaryOperator::Div: {
      node->Lhs->Accept(this);
      node->Rhs->Accept(this);
      node->Ty = node->Lhs->Ty;
      if (!node->Lhs->Ty->IsArithTy() && !node->Rhs->Ty->IsArithTy()) {
        SemaDiag(node->Tok->Location, "Invalid operands");
      }
      break;
    }
    case BinaryOperator::Equal:
    case BinaryOperator::PipeEqual:
    case BinaryOperator::Greater:
    case BinaryOperator::GreaterEqual:
    case BinaryOperator::Lesser:
    case BinaryOperator::LesserEqual: {
      node->Lhs->Accept(this);
      node->Rhs->Accept(this);
      node->Ty = Type::LongTy;
      break;
    }
    default:
      break;
    }
  }

  void Sema::VisitorUnaryExprNode(UnaryExpr *node) {
    node->Lhs->Accept(this);
    switch (node->Uop)
    {
    case UnaryOperator::Plus:
    case UnaryOperator::Minus: {
      node->Ty = node->Lhs->Ty;
      break;
    }
    case UnaryOperator::Amp: {
      node->Ty = std::make_shared<PointerType>(node->Lhs->Ty);
      break;
    }
    case UnaryOperator::Star: {
      if (node->Lhs->Ty->IsPointerTy()) {
        node->Ty = std::dynamic_pointer_cast<PointerType>(node->Lhs->Ty)->BaseTy;
      }else if (node->Lhs->Ty->IsArrayTy()) {
        node->Ty = std::dynamic_pointer_cast<ArrayType>(node->Lhs->Ty)->ElementType;
      }
      else {
        SemaDiag(node->Tok->Location, "invalid dereference operation");
        assert(0);
      }
      break;
    }
    default:
      break;
    }
  }

  void Sema::VisitorNumExprNode(NumExpr *node)
  {
    node->Ty = Type::LongTy;
  }

  void Sema::VisitorVarExprNode(VarExpr *node)
  {
    std::shared_ptr<Symbol> sym = SymTable.FindVar(node->VarName);
    if (!sym) {
      SemaDiag(node->Tok->Location, "Undeclared variable");
    }
    node->Sym = sym;
    node->Ty = sym->Ty;
  }

  void Sema::VisitorFuncCallExprNode(FuncCallExpr *node) {
    std::shared_ptr<Symbol> sym = SymTable.FindVar(node->FuncName);
    if (sym == nullptr) {
      SemaDiag(node->Tok->Location, "Undeclared function");
    }
    for (auto &arg : node->Args) {
      arg->Accept(this);
    }
    node->Ty = Type::LongTy;
  }

  void Sema::VisitorStmtExprNode(StmtExpr *node) {
    SymTable.EnterScope();

    for (auto &decl : node->Decls) {
      decl->Accept(this);
    }

    for (auto &stmt : node->Stmts) {
      stmt->Accept(this);
    }

    SymTable.ExitScope();
  }

  void Sema::VisitorMemberExprNode(MemberExpr *node)
  {
    node->Lhs->Accept(this);

    if (!node->Lhs->Ty->IsRecordTy())
      SemaDiag(node->Tok->Location, "Invalid left operands");

    auto rty = std::dynamic_pointer_cast<RecordType>(node->Lhs->Ty);
    std::shared_ptr<Field> fld = rty->FindField(node->RhsName);
    if (fld == nullptr)
      SemaDiag(node->Tok->Location, "haven't the filed");

    node->Ty = fld->Ty;
    node->Fld = fld;
  }

  void Sema::CheckAddOperator(BinaryExpr *expr)
  {
    expr->Lhs->Accept(this);
    expr->Rhs->Accept(this);
    expr->Ty = expr->Lhs->Ty;

    auto lhsTy = AdjustExprTy(expr->Lhs.get());
    auto rhsTy = AdjustExprTy(expr->Rhs.get());

    if (lhsTy->IsArithTy() && rhsTy->IsArithTy()) {
      return;
    }

    if (rhsTy->IsObjPtr() && lhsTy->IsIntegerTy()) {
      SwapKids(expr);
      lhsTy = AdjustExprTy(expr->Lhs.get());
      rhsTy = AdjustExprTy(expr->Rhs.get());
    }

    if (lhsTy->IsObjPtr() && rhsTy->IsIntegerTy()) {

      auto binNode = std::make_shared<BinaryExpr>(BinaryOperator::Mul, expr->Tok);
      auto num = std::make_shared<NumExpr>(expr->Tok);
      num->Value = std::dynamic_pointer_cast<PointerType>(lhsTy)->BaseTy->Size;;
      binNode->Lhs = num;
      binNode->Rhs = expr->Rhs;

      expr->Rhs = binNode;
      expr->Ty = expr->Lhs->Ty;
      return;
    }

    SemaDiag(expr->Tok->Location, "Invalid operands");
  }

  void Sema::CheckSubOperator(BinaryExpr* expr)
  {
    expr->Lhs->Accept(this);
    expr->Rhs->Accept(this);
    expr->Ty = expr->Lhs->Ty;

    auto lhsTy = AdjustExprTy(expr->Lhs.get());
    auto rhsTy = AdjustExprTy(expr->Rhs.get());

    if (lhsTy->IsArithTy() && rhsTy->IsArithTy()) {
      return;
    }

    if (lhsTy->IsObjPtr() && rhsTy->IsIntegerTy()) {

      auto binNode = std::make_shared<BinaryExpr>(BinaryOperator::Mul, expr->Tok);
      auto num = std::make_shared<NumExpr>(expr->Tok);
      num->Value = std::dynamic_pointer_cast<PointerType>(lhsTy)->BaseTy->Size;;
      binNode->Lhs = num;
      binNode->Rhs = expr->Rhs;

      expr->Rhs = binNode;
      expr->Ty = expr->Lhs->Ty;
      return;
    }

    if (lhsTy->IsObjPtr() && rhsTy->IsObjPtr()) {
      expr->Ty = Type::LongTy;

      auto num = std::make_shared<NumExpr>(expr->Tok);
      num->Value = std::dynamic_pointer_cast<PointerType>(lhsTy)->BaseTy->Size;;

      auto divLeft = std::make_shared<BinaryExpr>(BinaryOperator::Div, expr->Tok);
      divLeft->Lhs = expr->Lhs;
      divLeft->Rhs = num;

      auto divRight = std::make_shared<BinaryExpr>(BinaryOperator::Div, expr->Tok);
      divRight->Lhs = expr->Rhs;
      divRight->Rhs = num;

      expr->Lhs = divLeft;
      expr->Rhs = divRight;
      return;
    }

    SemaDiag(expr->Tok->Location, "Invalid operands");
  }

  std::shared_ptr<Type> Sema::AdjustExprTy(ExprNode *expr)
  {
    if (expr->Ty->IsFuncTy()) {
       return std::make_shared<PointerType>(expr->Ty);
    }else if (expr->Ty->IsArrayTy()) {
       return std::make_shared<PointerType>(std::dynamic_pointer_cast<ArrayType>(expr->Ty)->ElementType);
    }
    return expr->Ty;
  }

  void Sema::SwapKids(BinaryExpr *expr)
  {
    auto t = expr->Lhs;
    expr->Lhs = expr->Rhs;
    expr->Rhs = t;
  }
}
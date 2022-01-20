/***********************************
* File:     Type.cpp.cc
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2022/1/16
***********************************/

#include "Type.h"
#include "Diag.h"

namespace C100
{
  std::shared_ptr<BuildInType> Type::IntType = std::make_shared<BuildInType>(BuildInType::Kind::Int, 8, 8);

  bool Type::IsIntegerType() const{
    if (TC == TypeClass::BuildInType) {
      auto bi = dynamic_cast<const BuildInType *>(this);
      return bi->GetKind() == BuildInType::Kind::Int;
    }
    return false;
  }

  bool Type::IsPointerType() const{
    return TC == TypeClass::PointerType;
  }

  bool Type::IsFunctionType() const{
    return TC == TypeClass::FunctionType;
  }

  void TypeVisitor::VisitorProgramNode(ProgramNode *node) {
    for (auto &f : node->Funcs)
      f->Accept(this);
  }
  void TypeVisitor::VisitorFunctionNode(FunctionNode *node) {
    for (auto &s : node->Stmts)
      s->Accept(this);
  }
  void TypeVisitor::VisitorFuncCallNode(FuncCallNode *node) {
    for (auto &arg : node->Args)
      arg->Accept(this);
    node->Ty = Type::IntType;
  }
  void TypeVisitor::VisitorExprStmtNode(ExprStmtNode *node) {
    if (node->Lhs) {
      node->Lhs->Accept(this);
      node->Ty = node->Lhs->Ty;
    }
  }
  void TypeVisitor::VisitorIfStmtNode(IfStmtNode *node) {
    node->Cond->Accept(this);
    node->Then->Accept(this);
    if (node->Else)
      node->Else->Accept(this);
  }
  void TypeVisitor::VisitorWhileStmtNode(WhileStmtNode *node) {
    node->Cond->Accept(this);
    node->Then->Accept(this);
  }
  void TypeVisitor::VisitorDoWhileStmtNode(DoWhileStmtNode *node) {
    node->Cond->Accept(this);
    node->Stmt->Accept(this);
  }
  void TypeVisitor::VisitorForStmtNode(ForStmtNode *node) {
    node->Init->Accept(this);
    node->Cond->Accept(this);
    node->Inc->Accept(this);
    node->Stmt->Accept(this);
  }
  void TypeVisitor::VisitorBlockStmtNode(BlockStmtNode *node)  {
    for (auto &s : node->Stmts)
      s->Accept(this);
  }
  void TypeVisitor::VisitorReturnStmtNode(ReturnStmtNode *node)  {
    node->Lhs->Accept(this);
    node->Ty = node->Lhs->Ty;
  }

  void TypeVisitor::VisitorStmtExprNode(StmtExprNode *node) {
    for (auto &s : node->Stmts) {
      s->Accept(this);
    }
  }
  void TypeVisitor::VisitorDeclarationStmtNode(DeclarationStmtNode *node) {
    for (auto &n : node->AssignNodes) {
        n->Accept(this);
    }
  }
  void TypeVisitor::VisitorAssignExprNode(AssignExprNode *node) {
    node->Lhs->Accept(this);
    node->Rhs->Accept(this);
    node->Ty = node->Lhs->Ty;
  }
  void TypeVisitor::VisitorBinaryNode(BinaryNode *node) {
    node->Lhs->Accept(this);
    node->Rhs->Accept(this);
    switch (node->BinOp) {
    case BinaryOperator::Add:
    case BinaryOperator::Sub:
    case BinaryOperator::Mul:
    case BinaryOperator::Div:
      node->Ty = node->Lhs->Ty;
      break;
    case BinaryOperator::Equal:
    case BinaryOperator::PipeEqual:
    case BinaryOperator::Greater:
    case BinaryOperator::GreaterEqual:
    case BinaryOperator::Lesser:
    case BinaryOperator::LesserEqual:
      node->Ty = Type::IntType;
      break;
    default:
      assert(0);
      break;
    }
  }

  void TypeVisitor::VisitorUnaryNode(UnaryNode *node) {
    node->Lhs->Accept(this);
    switch (node->Uop) {
    case UnaryOperator::Plus:
    case UnaryOperator::Minus: {
      node->Ty = node->Lhs->Ty;
      break;
    }
    case UnaryOperator::Star: {
      if (node->Lhs->Ty->IsPointerType()) {
        node->Ty = std::dynamic_pointer_cast<PointerType>(node->Lhs->Ty)->Base;
      } else {
        DiagLoc(node->Tok->Location, "invalid dereference operation");
        assert(0);
      }
      break;
    }
    case UnaryOperator::Amp: {
      node->Ty = std::make_shared<PointerType>(node->Lhs->Ty);
      break;
    }
    }
  }

  void TypeVisitor::VisitorConstantNode(ConstantNode *node) {
    node->Ty = Type::IntType;
  }
  void TypeVisitor::VisitorVarExprNode(VarExprNode *node) {
    node->Ty = node->VarObj->Ty;
  }

  TypeVisitor *TypeVisitor::Visitor() {
    static TypeVisitor visitor;
    return &visitor;
  }
}
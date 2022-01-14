/**********************************
 * File:     PrintVisitor.cpp.cc
 *
 * Author:   caipeng
 *
 * Email:    iiicp@outlook.com
 *
 * Date:     2022/1/3
 ***********************************/

#include "PrintVisitor.h"
#include <stdio.h>
#include <cassert>

using namespace C100;

void PrintVisitor::VisitorProgramNode(ProgramNode *node)
{
  for (auto &f : node->Funcs)
    f->Accept(this);
}

void PrintVisitor::VisitorFunctionNode(FunctionNode *node)
{
  Content += std::string(node->FuncName);
  Content += "(";
  for (int i = 0; i < node->Params.size(); ++i) {
    Content += std::string(node->Params[i]->Name);
    if (i != node->Params.size() - 1) {
      Content += ",";
    }
  }
  Content += ")";
  Content += "{";
  for (auto &s : node->Stmts)
    s->Accept(this);
  Content += "}";
}

void PrintVisitor::VisitorBinaryNode(BinaryNode *node)
{
  node->Lhs->Accept(this);
  switch (node->BinOp) {
  case BinaryOperator::Add:
    Content += "+";
    break;
  case BinaryOperator::Sub:
    Content += "-";
    break;
  case BinaryOperator::Mul:
    Content += "*";
    break;
  case BinaryOperator::Div:
    Content += "/";
    break;
  case BinaryOperator::Equal:
    Content += "==";
    break;
  case BinaryOperator::PipeEqual:
    Content += "!=";
    break;
  case BinaryOperator::Greater:
    Content += ">";
    break;
  case BinaryOperator::GreaterEqual:
    Content += ">=";
    break;
  case BinaryOperator::Lesser:
    Content += "<";
    break;
  case BinaryOperator::LesserEqual:
    Content += "<=";
    break;
  default:
    assert(0);
    break;
  }
  node->Rhs->Accept(this);
}

void PrintVisitor::VisitorConstantNode(ConstantNode *node)
{
  //printf(" %d ", node->Value);
  Content += std::to_string(node->Value);
}

void PrintVisitor::VisitorExprStmtNode(ExprStmtNode *node)
{
  if (node->Lhs)
    node->Lhs->Accept(this);
  Content += ";";
}

void PrintVisitor::VisitorVarExprNode(VarExprNode *node) {
  Content += std::string(node->VarObj->Name);
}

void PrintVisitor::VisitorAssignExprNode(AssignExprNode *node) {
  node->Lhs->Accept(this);
  Content += "=";
  node->Rhs->Accept(this);
}

void PrintVisitor::VisitorIfStmtNode(IfStmtNode *node) {
  Content += "if";
  Content += "(";
  node->Cond->Accept(this);
  Content += ")";
  node->Then->Accept(this);
  if (node->Else) {
    Content += "else ";
    node->Else->Accept(this);
  }
}

void PrintVisitor::VisitorBlockStmtNode(BlockStmtNode *node) {
  Content += "{";
  for (auto &s : node->Stmts)
    s->Accept(this);
  Content += "}";
}

void PrintVisitor::VisitorWhileStmtNode(WhileStmtNode *node) {
  Content += "while";
  Content += "(";
  node->Cond->Accept(this);
  Content += ")";
  node->Then->Accept(this);
}

void PrintVisitor::VisitorDoWhileStmtNode(DoWhileStmtNode *node) {
  Content += "do ";
  node->Stmt->Accept(this);
  Content += "while";
  Content += "(";
  node->Cond->Accept(this);
  Content += ")";
  Content += ";";
}

void PrintVisitor::VisitorForStmtNode(ForStmtNode *node) {
  Content += "for";
  Content += "(";
  if (node->Init)
    node->Init->Accept(this);
  Content += ";";
  if (node->Cond)
    node->Cond->Accept(this);
  Content += ";";
  if (node->Inc)
    node->Inc->Accept(this);
  Content += ")";
  node->Stmt->Accept(this);
}

void PrintVisitor::VisitorFuncCallNode(FuncCallNode *node) {
  Content += node->FuncName;
  Content += "(";
  for (int i = 0; i < node->Args.size(); ++i) {
    node->Args[i]->Accept(this);
    if (i != node->Args.size()-1) {
      Content += ",";
    }
  }
  Content += ")";
}

void PrintVisitor::VisitorReturnStmtNode(ReturnStmtNode *node) {
  Content += "return ";
  node->Lhs->Accept(this);
  Content += ";";
}

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

using namespace C100;

void PrintVisitor::VisitorProgramNode(ProgramNode *node)
{
  for (auto &S : node->Stmts)
    S->Accept(this);
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
  node->Lhs->Accept(this);
  Content += ";";
}

void PrintVisitor::VisitorVarExprNode(VarExprNode *node) {
  Content += std::string(node->VarObj->Name);
}

void PrintVisitor::VisitorAssignExprNode(AssignExprNode *node) {
  node->Lhs->Accept(this);
  Content += '=';
  node->Rhs->Accept(this);
}

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
  node->Lhs->Accept(this);
  //Content += "\n";
}

void PrintVisitor::VisitorBinaryNode(BinaryNode *node)
{
  node->Rhs->Accept(this);
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
}

void PrintVisitor::VisitorConstantNode(ConstantNode *node)
{
  //printf(" %d ", node->Value);
  Content += std::to_string(node->Value);
}

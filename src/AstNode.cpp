/**********************************
 * File:     AstNode.cpp.cc
 *
 * Author:   caipeng
 *
 * Email:    iiicp@outlook.com
 *
 * Date:     2022/1/3
 ***********************************/

#include "AstNode.h"

using namespace C100;

void ProgramNode::Accept(AstVisitor *visitor)
{
  visitor->VisitorProgramNode(this);
}

void BinaryNode::Accept(AstVisitor *visitor)
{
  visitor->VisitorBinaryNode(this);
}


void ConstantNode::Accept(AstVisitor *visitor)
{
  visitor->VisitorConstantNode(this);
}
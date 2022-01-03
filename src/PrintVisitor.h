/**********************************
 * File:     PrintVisitor.h
 *
 * Author:   caipeng
 *
 * Email:    iiicp@outlook.com
 *
 * Date:     2022/1/3
 ***********************************/

#ifndef C100_PRINTVISITOR_H
#define C100_PRINTVISITOR_H

#include "AstNode.h"

namespace C100
{
class PrintVisitor : public AstVisitor
{
public:
  void VisitorProgramNode(ProgramNode *node) override;

private:
  void VisitorBinaryNode(BinaryNode *node) override;
  void VisitorConstantNode(ConstantNode *node) override;
};
}

#endif // C100_PRINTVISITOR_H

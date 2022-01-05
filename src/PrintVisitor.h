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
#include <string>

namespace C100
{
class PrintVisitor : public AstVisitor
{
public:
    std::string Content;
public:
  void VisitorProgramNode(ProgramNode *node) override;

private:
  void VisitorExprStmtNode(ExprStmtNode *node) override;
  void VisitorAssignExprNode(AssignExprNode *node) override;
  void VisitorBinaryNode(BinaryNode *node) override;
  void VisitorConstantNode(ConstantNode *node) override;
  void VisitorVarExprNode(VarExprNode *node) override;
};
}

#endif // C100_PRINTVISITOR_H

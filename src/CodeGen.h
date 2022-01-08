/**********************************
 * File:     CodeGen.h
 *
 * Author:   caipeng
 *
 * Email:    iiicp@outlook.com
 *
 * Date:     2022/1/3
 ***********************************/

#ifndef C100_CODEGEN_H
#define C100_CODEGEN_H

#include "AstNode.h"

namespace C100 {
  class CodeGen : public AstVisitor
  {
  private:
    int StackLevel{0};
    int Sequence{0};
  public:
    CodeGen() {}
    void VisitorProgramNode(ProgramNode *node) override;

  private:
    void VisitorExprStmtNode(ExprStmtNode *node)override;
    void VisitorAssignExprNode(AssignExprNode *node)override;
    void VisitorIfStmtNode(IfStmtNode *node) override;
    void VisitorBlockStmtNode(BlockStmtNode *node) override;
    void VisitorBinaryNode(BinaryNode *node) override;
    void VisitorConstantNode(ConstantNode *node) override;
    void VisitorVarExprNode(VarExprNode *node)override;

    void Push();
    void Pop(const char *reg);
  };
}

#endif // C100_CODEGEN_H

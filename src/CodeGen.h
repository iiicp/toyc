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
#include <string>

namespace C100 {
  class CodeGen : public AstVisitor
  {
  private:
    int StackLevel{0};
    int Sequence{0};
    const char *Reg8[6] = {"%dil", "%sil", "%dl", "%cl", "%r8b", "%r9b"};
    const char *Reg16[6] = {"%di", "%si", "%dx", "%cx", "%r8w", "%r9w"};
    const char *Reg32[6] = {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"};
    const char *Reg64[6] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    std::string CurrentFuncName;
  public:
    CodeGen() {}
    void VisitorProgramNode(ProgramNode *node) override;

  private:
    void VisitorFunctionNode(FunctionNode *node) override;
    void VisitorFuncCallNode(FuncCallNode *node) override;
    void VisitorExprStmtNode(ExprStmtNode *node)override;
    void VisitorAssignExprNode(AssignExprNode *node)override;
    void VisitorSizeofExprNode(SizeofExprNode *node) override;
    void VisitorIfStmtNode(IfStmtNode *node) override;
    void VisitorWhileStmtNode(WhileStmtNode *node) override;
    void VisitorDoWhileStmtNode(DoWhileStmtNode *node) override;
    void VisitorForStmtNode(ForStmtNode *node) override;
    void VisitorBlockStmtNode(BlockStmtNode *node) override;
    void VisitorReturnStmtNode(ReturnStmtNode *node) override;
    void VisitorStmtExprNode(StmtExprNode *node) override;
    void VisitorDeclarationStmtNode(DeclarationStmtNode *node) override;
    void VisitorBinaryNode(BinaryNode *node) override;
    void VisitorUnaryNode(UnaryNode *node) override;
    void VisitorNumNode(NumNode *node) override;
    void VisitorVarExprNode(VarExprNode *node)override;
    void VisitorMemberAccessNode(MemberAccessNode *node) override;

    void Push();
    void Pop(const char *reg);

    void GenAddr(AstNode *node);
    void Load(std::shared_ptr<Type> ty);
    void Store(std::shared_ptr<Type> ty);


  };
}

#endif // C100_CODEGEN_H

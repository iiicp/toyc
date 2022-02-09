/**********************************
 * File:     Gen.h
 *
 * Author:   caipeng
 *
 * Email:    iiicp@outlook.com
 *
 * Date:     2022/1/3
 ***********************************/

#ifndef CCC_CODEGEN_H
#define CCC_CODEGEN_H

#include "AstVisitor.h"
#include <string>
#include "Type.h"
#include "SymbolTable.h"
#include <stack>

namespace CCC {
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
    std::stack<int> LabelStack;
  public:
    CodeGen() {}

    void VisitorTranslationUnitNode(TranslationUnit *node) override;

  private:
    void VisitorFunctionNode(Function *node) override;
    void VisitorDeclarationNode(Declaration *node) override;

    void VisitorExprStmtNode(ExprStmtNode *node) override;
    void VisitorIfStmtNode(IfStmtNode *node) override;
    void VisitorWhileStmtNode(WhileStmtNode *node) override;
    void VisitorDoWhileStmtNode(DoWhileStmtNode *node) override;
    void VisitorForStmtNode(ForStmtNode *node) override;
    void VisitorBlockStmtNode(BlockStmtNode *node) override;
    void VisitorReturnStmtNode(ReturnStmtNode *node) override;
    void VisitorBreakStmtNode(BreakStmtNode *node) override;
    void VisitorContinueStmtNode(ContinueStmtNode *node) override;

    void VisitorAssignExprNode(AssignExpr *node) override;
    void VisitorSizeOfExprNode(SizeOfExpr *node) override;
    void VisitorBinaryExprNode(BinaryExpr *node) override;
    void VisitorUnaryExprNode(UnaryExpr *node) override;
    void VisitorNumExprNode(NumExpr *node) override;
    void VisitorVarExprNode(VarExpr *node) override;
    void VisitorFuncCallExprNode(FuncCallExpr *node) override;
    void VisitorStmtExprNode(StmtExpr *node) override;
    void VisitorMemberExprNode(MemberExpr *node) override;


    void Push();
    void Pop(const char *reg);

    void GenAddr(AstNode *node);
    void Load(std::shared_ptr<Type> ty);
    void Store(std::shared_ptr<Type> ty);


  };
}

#endif // CCC_CODEGEN_H

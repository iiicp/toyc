/**********************************
 * File:     AstVisitor.h
 *
 * Author:   caipeng
 *
 * Email:    iiicp@outlook.com
 *
 * Date:     2022/1/30
 ***********************************/

#ifndef CCC_SRC_ASTVISITOR_H
#define CCC_SRC_ASTVISITOR_H

#include "AstNode.h"

namespace CCC
{
  /// declaration
  class TranslationUnit;
  class Function;
  class Declaration;
  class DeclSpecifier;
  class Declarator;
  class NameDeclarator;
  class FunctionDeclarator;
  class ArrayDeclarator;
  class PointerDeclarator;
  class ParamTypeList;
  class ParamDeclaration;
  class InitDeclarator;
  class Initializer;
  class StructSpecifier;
  class StructDeclaration;
  class StructDeclarator;
  class EnumSpecifier;
  class EnumDeclarator;
  class DeclSpecToken;

  /// stmt
  class StmtNode;
  class ExprStmtNode;
  class IfStmtNode;
  class WhileStmtNode;
  class DoWhileStmtNode;
  class ForStmtNode;
  class BlockStmtNode;
  class ReturnStmtNode;
  class BreakStmtNode;
  class ContinueStmtNode;
  class GotoStmtNode;
  class LabelStmtNode;
  class CaseStmtNode;
  class DefaultStmtNode;
  class SwitchStmtNode;

  /// expr
  class ExprNode;
  class AssignExpr;
  class SizeOfExpr;
  class BinaryExpr;
  class UnaryExpr;
  class NumExpr;
  class VarExpr;
  class FuncCallExpr;
  class StmtExpr;
  class MemberExpr;

  class AstVisitor {
  public:
    virtual ~AstVisitor() {}
    virtual void VisitorTranslationUnitNode(TranslationUnit *node) {};
    virtual void VisitorFunctionNode(Function *node) {};
    virtual void VisitorDeclarationNode(Declaration *node) {};
    virtual void VisitorDeclSpecifierNode(DeclSpecifier *node) {};
    virtual void VisitorPointerDeclaratorNode(PointerDeclarator *node) {};
    virtual void VisitorNameDeclaratorNode(NameDeclarator *node)  {};
    virtual void VisitorFuncDeclaratorNode(FunctionDeclarator *node) {};
    virtual void VisitorArrayDeclaratorNode(ArrayDeclarator *node)  {};
    virtual void VisitorInitDeclaratorNode(InitDeclarator *node)  {};
    virtual void VisitorInitializerNode(Initializer *node)  {};
    virtual void VisitorParamTypeListNode(ParamTypeList *node)  {};
    virtual void VisitorParamDeclarationNode(ParamDeclaration *node)  {};
    virtual void VisitorStructSpecifierNode(StructSpecifier *node)  {};
    virtual void VisitorStructDeclarationNode(StructDeclaration *node)  {};
    virtual void VisitorStructDeclaratorNode(StructDeclarator *node)  {};
    virtual void VisitorEnumSpecifierNode(EnumSpecifier *node)  {};
    virtual void VisitorEnumDeclaratorNode(EnumDeclarator *node)  {};
    virtual void VisitorDeclSpecTokenNode(DeclSpecToken *node)  {};

    virtual void VisitorExprStmtNode(ExprStmtNode *node) = 0;
    virtual void VisitorIfStmtNode(IfStmtNode *node) = 0;
    virtual void VisitorWhileStmtNode(WhileStmtNode *node) = 0;
    virtual void VisitorDoWhileStmtNode(DoWhileStmtNode *node) = 0;
    virtual void VisitorForStmtNode(ForStmtNode *node) = 0;
    virtual void VisitorBlockStmtNode(BlockStmtNode *node) = 0;
    virtual void VisitorReturnStmtNode(ReturnStmtNode *node) = 0;
    virtual void VisitorBreakStmtNode(BreakStmtNode *node) = 0;
    virtual void VisitorContinueStmtNode(ContinueStmtNode *node) = 0;
    virtual void VisitorGotoStmtNode(GotoStmtNode *node) = 0;
    virtual void VisitorLabelStmtNode(LabelStmtNode *node) = 0;
    virtual void VisitorCaseStmtNode(CaseStmtNode *node) = 0;
    virtual void VisitorDefaultStmtNode(DefaultStmtNode *node) = 0;
    virtual void VisitorSwitchStmtNode(SwitchStmtNode *node) = 0;

    virtual void VisitorAssignExprNode(AssignExpr *node) = 0;
    virtual void VisitorSizeOfExprNode(SizeOfExpr *node) = 0;
    virtual void VisitorBinaryExprNode(BinaryExpr *node) = 0;
    virtual void VisitorUnaryExprNode(UnaryExpr *node) = 0;
    virtual void VisitorNumExprNode(NumExpr *node) = 0;
    virtual void VisitorVarExprNode(VarExpr *node) = 0;
    virtual void VisitorFuncCallExprNode(FuncCallExpr *node) = 0;
    virtual void VisitorStmtExprNode(StmtExpr *node) = 0;
    virtual void VisitorMemberExprNode(MemberExpr *node) = 0;
  };
}


#endif // CCC_SRC_ASTVISITOR_H

/**********************************
 * File:     AstNode.h
 *
 * Author:   caipeng
 *
 * Email:    iiicp@outlook.com
 *
 * Date:     2022/1/3
 ***********************************/

#ifndef C100_ASTNODE_H
#define C100_ASTNODE_H

#include <memory>
#include <list>
#include <string_view>
#include <vector>

namespace C100
{
  class AstVisitor;
  class Type;
  class Token;
  class AstNode
  {
  public:
    std::shared_ptr<Type> Ty;
    std::shared_ptr<Token> Tok;
    virtual ~AstNode() {}
    AstNode(std::shared_ptr<Token> tok) : Tok(tok) {}
    virtual void Accept(AstVisitor *visitor) = 0;
  };

  class Var
  {
  public:
    std::string_view Name;
    std::shared_ptr<Type> Ty;
    int Offset;
  };

  class ProgramNode : public AstNode
  {
  public:
    std::list<std::shared_ptr<AstNode>> Funcs;
    ProgramNode(std::shared_ptr<Token> tok) : AstNode(tok) {}
    void Accept(AstVisitor *visitor) override;
  };

  class FunctionNode : public AstNode
  {
  public:
    std::string_view FuncName;
    std::list<std::shared_ptr<Var>> Params{};
    std::list<std::shared_ptr<Var>> Locals{};
    std::list<std::shared_ptr<AstNode>> Stmts{};
    FunctionNode(std::shared_ptr<Token> tok) : AstNode(tok) {}
    void Accept(AstVisitor *visitor) override;
  };

  class ExprStmtNode : public AstNode
  {
  public:
    std::shared_ptr<AstNode> Lhs{nullptr};
    ExprStmtNode(std::shared_ptr<Token> tok) : AstNode(tok) {}
    void Accept(AstVisitor *visitor) override;
  };

  class IfStmtNode : public AstNode
  {
  public:
    std::shared_ptr<AstNode> Cond{nullptr};
    std::shared_ptr<AstNode> Then{nullptr};
    std::shared_ptr<AstNode> Else{nullptr};
    IfStmtNode(std::shared_ptr<Token> tok) : AstNode(tok) {}
    void Accept(AstVisitor *visitor) override;
  };

  class WhileStmtNode : public AstNode
  {
  public:
    std::shared_ptr<AstNode> Cond{nullptr};
    std::shared_ptr<AstNode> Then{nullptr};
    WhileStmtNode(std::shared_ptr<Token> tok) : AstNode(tok) {}
    void Accept(AstVisitor *visitor) override;
  };

  class DoWhileStmtNode : public AstNode
  {
  public:
    std::shared_ptr<AstNode> Stmt{nullptr};
    std::shared_ptr<AstNode> Cond{nullptr};
    DoWhileStmtNode(std::shared_ptr<Token> tok) : AstNode(tok) {}
    void Accept(AstVisitor *visitor) override;
  };

  class ForStmtNode : public AstNode
  {
  public:
    std::shared_ptr<AstNode> Init{nullptr};
    std::shared_ptr<AstNode> Cond{nullptr};
    std::shared_ptr<AstNode> Inc{nullptr};
    std::shared_ptr<AstNode> Stmt{nullptr};
    ForStmtNode(std::shared_ptr<Token> tok) : AstNode(tok) {}
    void Accept(AstVisitor *visitor) override;
  };

  class BlockStmtNode : public AstNode
  {
  public:
    std::list<std::shared_ptr<AstNode>> Stmts;
    BlockStmtNode(std::shared_ptr<Token> tok) : AstNode(tok) {}
    void Accept(AstVisitor *visitor) override;
  };

  class ReturnStmtNode : public AstNode
  {
  public:
    std::shared_ptr<AstNode> Lhs{nullptr};
    ReturnStmtNode(std::shared_ptr<Token> tok) : AstNode(tok) {}
    void Accept(AstVisitor *visitor) override;
  };

  class AssignExprNode : public AstNode
  {
  public:
    std::shared_ptr<AstNode> Lhs{nullptr};
    std::shared_ptr<AstNode> Rhs{nullptr};
    AssignExprNode(std::shared_ptr<Token> tok) : AstNode(tok) {}
    void Accept(AstVisitor *visitor) override;
  };

  class SizeofExprNode : public AstNode
  {
  public:
    std::shared_ptr<AstNode> Lhs{nullptr};
    SizeofExprNode(std::shared_ptr<Token> tok) : AstNode(tok) {}
    void Accept(AstVisitor *visitor) override;
  };

  class DeclarationStmtNode : public AstNode
  {
  public:
    std::list<std::shared_ptr<AssignExprNode>> AssignNodes;
    DeclarationStmtNode(std::shared_ptr<Token> tok) : AstNode(tok) {}
    void Accept(AstVisitor *visitor) override;
  };

  enum class BinaryOperator
  {
    Add,
    PtrAdd,
    Sub,
    PtrSub,
    PtrDiff,
    Mul,
    Div,
    Equal,
    PipeEqual,
    Greater,
    GreaterEqual,
    Lesser,
    LesserEqual
  };

  class BinaryNode : public AstNode
  {
  public:
    BinaryOperator BinOp;
    std::shared_ptr<AstNode> Lhs{nullptr};
    std::shared_ptr<AstNode> Rhs{nullptr};
    BinaryNode(BinaryOperator op, std::shared_ptr<Token> tok) : AstNode(tok), BinOp(op) {}
    void Accept(AstVisitor *visitor) override;
  };

  enum class UnaryOperator
  {
    Plus,
    Minus,
    Star,
    Amp
  };
  class UnaryNode : public AstNode
  {
  public:
    UnaryOperator Uop;
    std::shared_ptr<AstNode> Lhs{nullptr};
    UnaryNode(UnaryOperator op, std::shared_ptr<Token> tok) : AstNode(tok), Uop(op) {}
    void Accept(AstVisitor *visitor) override;
  };

  class NumNode : public AstNode
  {
  public:
    int Value;
    NumNode(std::shared_ptr<Token> tok) : AstNode(tok) {}
    void Accept(AstVisitor *visitor) override;
  };

  class VarExprNode : public AstNode
  {
  public:
    std::shared_ptr<Var> VarObj;
    VarExprNode(std::shared_ptr<Token> tok) : AstNode(tok) {}
    void Accept(AstVisitor *visitor) override;
  };

  class FuncCallNode : public AstNode
  {
  public:
    std::string_view FuncName;
    std::vector<std::shared_ptr<AstNode>> Args;
    FuncCallNode(std::shared_ptr<Token> tok) : AstNode(tok) {}
    void Accept(AstVisitor *visitor) override;
  };

  class StmtExprNode : public AstNode
  {
  public:
    std::list<std::shared_ptr<AstNode>> Stmts;
    StmtExprNode(std::shared_ptr<Token> tok) : AstNode(tok) {}
    void Accept(AstVisitor *visitor) override;
  };

  class AstVisitor
  {
  public:
    virtual ~AstVisitor(){}
    virtual void VisitorProgramNode(ProgramNode *node) = 0;
    virtual void VisitorFunctionNode(FunctionNode *node) = 0;
    virtual void VisitorFuncCallNode(FuncCallNode *node) = 0;
    virtual void VisitorExprStmtNode(ExprStmtNode *node) = 0;
    virtual void VisitorIfStmtNode(IfStmtNode *node) = 0;
    virtual void VisitorWhileStmtNode(WhileStmtNode *node) = 0;
    virtual void VisitorDoWhileStmtNode(DoWhileStmtNode *node) = 0;
    virtual void VisitorForStmtNode(ForStmtNode *node) = 0;
    virtual void VisitorBlockStmtNode(BlockStmtNode *node)  = 0;
    virtual void VisitorReturnStmtNode(ReturnStmtNode *node)  = 0;
    virtual void VisitorDeclarationStmtNode(DeclarationStmtNode *node) = 0;
    virtual void VisitorStmtExprNode(StmtExprNode *node) = 0;
    virtual void VisitorAssignExprNode(AssignExprNode *node) = 0;
    virtual void VisitorSizeofExprNode(SizeofExprNode *node) = 0;
    virtual void VisitorBinaryNode(BinaryNode *node) = 0;
    virtual void VisitorUnaryNode(UnaryNode *node) = 0;
    virtual void VisitorNumNode(NumNode *node) = 0;
    virtual void VisitorVarExprNode(VarExprNode *node) = 0;
  };
}

#endif // C100_ASTNODE_H

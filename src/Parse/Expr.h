/**********************************
 * File:     Expr.h
 *
 * Author:   caipeng
 *
 * Email:    iiicp@outlook.com
 *
 * Date:     2022/1/30
 ***********************************/

#ifndef CCC_SRC_EXPR_H
#define CCC_SRC_EXPR_H

#include "AstVisitor.h"
#include "SymbolTable.h"

namespace CCC {

  class Type;
  class ExprNode : public AstNode
  {
  public:
    std::shared_ptr<Type> Ty;
    ExprNode(std::shared_ptr<Token> tok) : AstNode(tok) {}
    virtual void Accept(AstVisitor *visitor) = 0;
  };

  class AssignExpr : public ExprNode
  {
  public:
    std::shared_ptr<ExprNode> Lhs{nullptr};
    std::shared_ptr<ExprNode> Rhs{nullptr};
    AssignExpr(std::shared_ptr<Token> tok) : ExprNode(tok) {}
    void Accept(AstVisitor *visitor) override;
  };

  class SizeOfExpr : public ExprNode
  {
  public:
    std::shared_ptr<ExprNode> Lhs{nullptr};
    SizeOfExpr(std::shared_ptr<Token> tok) : ExprNode(tok) {}
    void Accept(AstVisitor *visitor) override;
  };

  enum class BinaryOperator
  {
    Add, Sub, Mul, Div,
    Equal, PipeEqual, Greater, GreaterEqual,
    Lesser, LesserEqual
  };

  class BinaryExpr : public ExprNode
  {
  public:
    BinaryOperator BinOp;
    std::shared_ptr<ExprNode> Lhs{nullptr};
    std::shared_ptr<ExprNode> Rhs{nullptr};
    BinaryExpr(BinaryOperator op, std::shared_ptr<Token> tok) : ExprNode(tok), BinOp(op) {}
    void Accept(AstVisitor *visitor) override;
  };

  enum class UnaryOperator
  {
    Plus, Minus, Star, Amp
  };
  class UnaryExpr : public ExprNode
  {
  public:
    UnaryOperator Uop;
    std::shared_ptr<ExprNode> Lhs{nullptr};
    UnaryExpr(UnaryOperator op, std::shared_ptr<Token> tok) : ExprNode(tok), Uop(op) {}
    void Accept(AstVisitor *visitor) override;
  };

  class NumExpr : public ExprNode
  {
  public:
    long Value;
    NumExpr(std::shared_ptr<Token> tok) : ExprNode(tok) {}
    void Accept(AstVisitor *visitor) override;
  };

  class VarExpr : public ExprNode
  {
  public:
    std::string_view VarName;
    VarExpr(std::shared_ptr<Token> tok) : ExprNode(tok) {}
    void Accept(AstVisitor *visitor) override;

  public:
    /// sema
    std::shared_ptr<Symbol> Sym;
  };

  class FuncCallExpr : public ExprNode
  {
  public:
    std::string_view FuncName;
    std::vector<std::shared_ptr<ExprNode>> Args;
    FuncCallExpr(std::shared_ptr<Token> tok) : ExprNode(tok) {}
    void Accept(AstVisitor *visitor) override;
  };

  class StmtExpr : public ExprNode
  {
  public:
    std::list<std::shared_ptr<Declaration>> Decls;
    std::list<std::shared_ptr<StmtNode>> Stmts;
    StmtExpr(std::shared_ptr<Token> tok) : ExprNode(tok) {}
    void Accept(AstVisitor *visitor) override;
  };

  class MemberExpr : public ExprNode
  {
  public:
    std::shared_ptr<ExprNode> Lhs;
    std::string_view RhsName;
    MemberExpr(std::shared_ptr<Token> tok) : ExprNode(tok) {}
    void Accept(AstVisitor *visitor) override;

  public:
    /// sema
    std::shared_ptr<Field> Fld{nullptr};
  };
}

#endif // CCC_SRC_EXPR_H

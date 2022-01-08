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

namespace C100
{
  class AstVisitor;
  class AstNode
  {
  public:
    virtual ~AstNode() {}
    virtual void Accept(AstVisitor *visitor) {};
  };

  class Var
  {
  public:
    std::string_view Name;
    int Offset;
  };

  class ProgramNode : public AstNode
  {
  public:
    std::list<std::shared_ptr<AstNode>> Stmts;
    std::list<std::shared_ptr<Var>> LocalVars;
    void Accept(AstVisitor *visitor) override;
  };

  class ExprStmtNode : public AstNode
  {
  public:
    std::shared_ptr<AstNode> Lhs;
    void Accept(AstVisitor *visitor) override;
  };

  class IfStmtNode : public AstNode
  {
  public:
    std::shared_ptr<AstNode> Cond{nullptr};
    std::shared_ptr<AstNode> Then{nullptr};
    std::shared_ptr<AstNode> Else{nullptr};

    void Accept(AstVisitor *visitor) override;
  };

  class BlockStmtNode : public AstNode
  {
  public:
    std::list<std::shared_ptr<AstNode>> Stmts;
    void Accept(AstVisitor *visitor) override;
  };

  class AssignExprNode : public AstNode
  {
  public:
    std::shared_ptr<AstNode> Lhs;
    std::shared_ptr<AstNode> Rhs;
    void Accept(AstVisitor *visitor) override;
  };

  enum class BinaryOperator
  {
    Add,
    Sub,
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
    std::shared_ptr<AstNode> Lhs;
    std::shared_ptr<AstNode> Rhs;
    void Accept(AstVisitor *visitor) override;
  };

  class ConstantNode : public AstNode
  {
  public:
    int Value;
    void Accept(AstVisitor *visitor) override;
  };

  class VarExprNode : public AstNode
  {
  public:
    std::shared_ptr<Var> VarObj;
    void Accept(AstVisitor *visitor) override;
  };

  class AstVisitor
  {
  public:
    virtual ~AstVisitor(){}
    virtual void VisitorProgramNode(ProgramNode *node){};
    virtual void VisitorExprStmtNode(ExprStmtNode *node){};
    virtual void VisitorIfStmtNode(IfStmtNode *node){};
    virtual void VisitorBlockStmtNode(BlockStmtNode *node) {}
    virtual void VisitorAssignExprNode(AssignExprNode *node){};
    virtual void VisitorBinaryNode(BinaryNode *node){};
    virtual void VisitorConstantNode(ConstantNode *node){};
    virtual void VisitorVarExprNode(VarExprNode *node){};
  };
}

#endif // C100_ASTNODE_H

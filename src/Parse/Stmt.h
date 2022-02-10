/**********************************
 * File:     Stmt.h
 *
 * Author:   caipeng
 *
 * Email:    iiicp@outlook.com
 *
 * Date:     2022/1/30
 ***********************************/

#ifndef CCC_SRC_STMT_H
#define CCC_SRC_STMT_H

#include "AstVisitor.h"

namespace CCC
{
  enum class StmtNodeClass {
    ExprStmtNode,
    IfStmtNode,
    WhileStmtNode,
    DoWhileStmtNode,
    ForStmtNode,
    BlockStmtNode,
    ReturnStmtNode,
    BreakStmtNode,
    ContinueStmtNode,
    GotoStmtNode,
    LabelStmtNode,
    CaseStmtNode,
    DefaultStmtNode,
    SwitchStmtNode
  };

  class StmtNode : public AstNode
  {
  public:
    StmtNodeClass Cls;
    StmtNode(std::shared_ptr<Token> tok) : AstNode(tok) {}
    virtual void Accept(AstVisitor *visitor) = 0;
  };

  class ExprStmtNode : public StmtNode
  {
  public:
    std::shared_ptr<ExprNode> Lhs{nullptr};
    ExprStmtNode(std::shared_ptr<Token> tok) : StmtNode(tok) {
      Cls = StmtNodeClass::ExprStmtNode;
    }
    void Accept(AstVisitor *visitor) override;
  };

  class IfStmtNode : public StmtNode
  {
  public:
    std::shared_ptr<ExprNode> Cond{nullptr};
    std::shared_ptr<StmtNode> Then{nullptr};
    std::shared_ptr<StmtNode> Else{nullptr};
    IfStmtNode(std::shared_ptr<Token> tok) : StmtNode(tok) {
      Cls = StmtNodeClass::IfStmtNode;
    }
    void Accept(AstVisitor *visitor) override;
  };

  class WhileStmtNode : public StmtNode
  {
  public:
    std::shared_ptr<ExprNode> Cond{nullptr};
    std::shared_ptr<StmtNode> Then{nullptr};
    WhileStmtNode(std::shared_ptr<Token> tok) : StmtNode(tok) {
      Cls = StmtNodeClass::WhileStmtNode;
    }
    void Accept(AstVisitor *visitor) override;
  };

  class DoWhileStmtNode : public StmtNode
  {
  public:
    std::shared_ptr<StmtNode> Stmt{nullptr};
    std::shared_ptr<ExprNode> Cond{nullptr};
    DoWhileStmtNode(std::shared_ptr<Token> tok) : StmtNode(tok) {
      Cls = StmtNodeClass::DoWhileStmtNode;
    }
    void Accept(AstVisitor *visitor) override;
  };

  class ForStmtNode : public StmtNode
  {
  public:
    std::shared_ptr<ExprNode> InitExpr{nullptr};
    std::shared_ptr<Declaration> InitDecl{nullptr};
    std::shared_ptr<ExprNode> Cond{nullptr};
    std::shared_ptr<ExprNode> Inc{nullptr};
    std::shared_ptr<StmtNode> Stmt{nullptr};
    ForStmtNode(std::shared_ptr<Token> tok) : StmtNode(tok) {
      Cls = StmtNodeClass::ForStmtNode;
    }
    void Accept(AstVisitor *visitor) override;
  };

  class BlockStmtNode : public StmtNode
  {
  public:
    std::list<std::shared_ptr<Declaration>> Decls;
    std::list<std::shared_ptr<StmtNode>> Stmts;
    BlockStmtNode(std::shared_ptr<Token> tok) : StmtNode(tok) {
      Cls = StmtNodeClass::BlockStmtNode;
    }
    void Accept(AstVisitor *visitor) override;

  public:
    /// sema
    bool needEnterScope{true};
  };

  class ReturnStmtNode : public StmtNode
  {
  public:
    std::shared_ptr<ExprNode> Lhs{nullptr};
    ReturnStmtNode(std::shared_ptr<Token> tok) : StmtNode(tok) {
      Cls = StmtNodeClass::ReturnStmtNode;
    }
    void Accept(AstVisitor *visitor) override;
  };

  class BreakStmtNode : public StmtNode
  {
  public:
    BreakStmtNode(std::shared_ptr<Token> tok) : StmtNode(tok) {
        Cls = StmtNodeClass::BreakStmtNode;
    }
    void Accept(AstVisitor *visitor) override;
  };

  class ContinueStmtNode : public StmtNode
  {
  public:
    ContinueStmtNode(std::shared_ptr<Token> tok) : StmtNode(tok) {
      Cls = StmtNodeClass::ContinueStmtNode;
    }
    void Accept(AstVisitor *visitor) override;
  };

  class GotoStmtNode : public StmtNode
  {
  public:
    std::string_view LabelName;
    GotoStmtNode(std::shared_ptr<Token> tok) : StmtNode(tok) {
        Cls = StmtNodeClass::GotoStmtNode;
    }
    void Accept(AstVisitor *visitor) override;
  };

  class LabelStmtNode : public StmtNode
  {
  public:
    std::string_view LabelName;
    std::shared_ptr<StmtNode> Stmt;
    LabelStmtNode(std::shared_ptr<Token> tok) : StmtNode(tok) {
        Cls = StmtNodeClass::LabelStmtNode;
    }
    void Accept(AstVisitor *visitor) override;
  };

  class CaseStmtNode : public StmtNode
  {
  public:
    std::shared_ptr<ExprNode> Expr;
    std::shared_ptr<StmtNode> Stmt;
    CaseStmtNode(std::shared_ptr<Token> tok) : StmtNode(tok) {
      Cls = StmtNodeClass::CaseStmtNode;
    }
    void Accept(AstVisitor *visitor) override;
  public:
    /// codegen
    int Label{0};
  };

  class DefaultStmtNode : public StmtNode
  {
  public:
    std::shared_ptr<StmtNode> Stmt;
    DefaultStmtNode(std::shared_ptr<Token> tok) : StmtNode(tok) {
      Cls = StmtNodeClass::DefaultStmtNode;
    }
    void Accept(AstVisitor *visitor) override;
  public:
    /// codegen
    int Label{0};
  };

  class SwitchStmtNode : public StmtNode
  {
  public:
    std::shared_ptr<ExprNode> Expr;
    std::shared_ptr<StmtNode> Stmt;
    SwitchStmtNode(std::shared_ptr<Token> tok) : StmtNode(tok) {
      Cls = StmtNodeClass::SwitchStmtNode;
    }
    void Accept(AstVisitor *visitor) override;

  public:
    /// sema
    std::list<CaseStmtNode *> CaseStmtList;
    DefaultStmtNode *DefaultStmt{nullptr};
  };
}

#endif // CCC_SRC_STMT_H

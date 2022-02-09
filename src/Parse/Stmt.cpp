/**********************************
 * File:     Stmt.cpp.cc
 *
 * Author:   caipeng
 *
 * Email:    iiicp@outlook.com
 *
 * Date:     2022/1/30
 ***********************************/

#include "Stmt.h"
#include "Parser.h"

namespace CCC
{
  std::shared_ptr<StmtNode> Parser::ParseStmt()
  {
    if (Lex.Match(TokenKind::If)) {
      return ParseIfStmt();
    }
    else if (Lex.Match(TokenKind::While)) {
      return ParseWhileStmt();
    }
    else if (Lex.Match(TokenKind::Do)) {
      return ParseDoWhileStmt();
    }
    else if (Lex.Match(TokenKind::For)) {
      return ParseForStmt();
    }
    else if (Lex.Match(TokenKind::LBrace)) {
      return ParseBlockStmt();
    }
    else if (Lex.Match(TokenKind::Return)) {
      return ParseReturnStmt();
    }
    else if (Lex.Match(TokenKind::Break)) {
      return ParseBreakStmt();
    }
    else if (Lex.Match(TokenKind::Continue)) {
      return ParseContinueStmt();
    }
    else {
      return ParseExprStmt();
    }
  }

  std::shared_ptr<IfStmtNode> Parser::ParseIfStmt()
  {
    auto node = std::make_shared<IfStmtNode>(Lex.CurrentToken);
    Lex.SkipToken(TokenKind::If);
    Lex.ExpectToken(TokenKind::LParen);
    node->Cond = ParseExpr();
    Lex.ExpectToken(TokenKind::RParen);
    node->Then = ParseStmt();
    if (Lex.Match(TokenKind::Else)) {
      Lex.SkipToken(TokenKind::Else);
      node->Else = ParseStmt();
    }
    return node;
  }

  std::shared_ptr<WhileStmtNode> Parser::ParseWhileStmt()
  {
    auto node = std::make_shared<WhileStmtNode>(Lex.CurrentToken);
    Lex.SkipToken(TokenKind::While);
    Lex.ExpectToken(TokenKind::LParen);
    node->Cond = ParseExpr();
    Lex.ExpectToken(TokenKind::RParen);
    node->Then = ParseStmt();
    return node;
  }

  std::shared_ptr<DoWhileStmtNode> Parser::ParseDoWhileStmt()
  {
    auto node = std::make_shared<DoWhileStmtNode>(Lex.CurrentToken);
    Lex.SkipToken(TokenKind::Do);
    node->Stmt = ParseStmt();
    Lex.ExpectToken(TokenKind::While);
    Lex.ExpectToken(TokenKind::LParen);
    node->Cond = ParseExpr();
    Lex.ExpectToken(TokenKind::RParen);
    Lex.ExpectToken(TokenKind::Semicolon);
    return node;
  }

  std::shared_ptr<ForStmtNode> Parser::ParseForStmt()
  {
    auto node = std::make_shared<ForStmtNode>(Lex.CurrentToken);
    Lex.SkipToken(TokenKind::For);
    Lex.ExpectToken(TokenKind::LParen);
    if (!Lex.Match(TokenKind::Semicolon)) {
      if (FirstDeclarationSet.find(Lex.CurrentToken->Kind) != FirstDeclarationSet.end()) {
        node->InitDecl = ParseDeclaration();
      }else {
        node->InitExpr = ParseExpr();
        Lex.ExpectToken(TokenKind::Semicolon);
      }
    }else {
      Lex.ExpectToken(TokenKind::Semicolon);
    }
    if (!Lex.Match(TokenKind::Semicolon))
      node->Cond = ParseExpr();
    Lex.ExpectToken(TokenKind::Semicolon);
    if (!Lex.Match(TokenKind::RParen))
      node->Inc = ParseExpr();
    Lex.ExpectToken(TokenKind::RParen);
    node->Stmt = ParseStmt();
    return node;
  }

  std::shared_ptr<BlockStmtNode> Parser::ParseBlockStmt()
  {
    auto node = std::make_shared<BlockStmtNode>(Lex.CurrentToken);
    Lex.SkipToken(TokenKind::LBrace);
    while (!Lex.Match(TokenKind::RBrace)) {
      if (FirstDeclarationSet.find(Lex.CurrentToken->Kind) != FirstDeclarationSet.end()) {
        auto decl = ParseDeclaration();
        decl->IsLocalDecl = true;
        node->Decls.push_back(decl);
      }else {
        node->Stmts.push_back(ParseStmt());
      }
    }
    Lex.ExpectToken(TokenKind::RBrace);

    return node;
  }

  std::shared_ptr<ReturnStmtNode> Parser::ParseReturnStmt()
  {
    auto node = std::make_shared<ReturnStmtNode>(Lex.CurrentToken);
    Lex.SkipToken(TokenKind::Return);
    node->Lhs = ParseExpr();
    Lex.ExpectToken(TokenKind::Semicolon);
    return node;
  }

  std::shared_ptr<ExprStmtNode> Parser::ParseExprStmt()
  {
    auto node = std::make_shared<ExprStmtNode>(Lex.CurrentToken);
    if (!Lex.Match(TokenKind::Semicolon)) {
      node->Lhs = ParseExpr();
    }
    Lex.ExpectToken(TokenKind::Semicolon);
    return node;
  }

  std::shared_ptr<BreakStmtNode> Parser::ParseBreakStmt() {
    auto node = std::make_shared<BreakStmtNode>(Lex.CurrentToken);
    Lex.ExpectToken(TokenKind::Break);
    Lex.ExpectToken(TokenKind::Semicolon);
    return node;
  }

  std::shared_ptr<ContinueStmtNode> Parser::ParseContinueStmt() {
    auto node = std::make_shared<ContinueStmtNode>(Lex.CurrentToken);
    Lex.ExpectToken(TokenKind::Continue);
    Lex.ExpectToken(TokenKind::Semicolon);
    return node;
  }

  void IfStmtNode::Accept(AstVisitor *visitor) {
    visitor->VisitorIfStmtNode(this);
  }

  void DoWhileStmtNode::Accept(AstVisitor *visitor) {
    visitor->VisitorDoWhileStmtNode(this);
  }

  void WhileStmtNode::Accept(AstVisitor *visitor) {
    visitor->VisitorWhileStmtNode(this);
  }

  void ForStmtNode::Accept(AstVisitor *visitor) {
    visitor->VisitorForStmtNode(this);
  }

  void ReturnStmtNode::Accept(AstVisitor *visitor) {
    visitor->VisitorReturnStmtNode(this);
  }

  void BlockStmtNode::Accept(AstVisitor *visitor) {
    visitor->VisitorBlockStmtNode(this);
  }

  void ExprStmtNode::Accept(AstVisitor *visitor) {
    visitor->VisitorExprStmtNode(this);
  }

  void BreakStmtNode::Accept(AstVisitor *visitor) {
    visitor->VisitorBreakStmtNode(this);
  }

  void ContinueStmtNode::Accept(AstVisitor *visitor) {
    visitor->VisitorContinueStmtNode(this);
  }
}
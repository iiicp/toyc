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
    else if (Lex.Match(TokenKind::Goto)) {
      return ParseGotoStmt();
    }
    else if (Lex.Match(TokenKind::Case)) {
      return ParseCaseStmt();
    }
    else if (Lex.Match(TokenKind::Default)) {
      return ParseDefaultStmt();
    }
    else if (Lex.Match(TokenKind::Switch)) {
      return ParseSwitchStmt();
    }
    else if (Lex.Match(TokenKind::Id)) {
      return ParseLabelStmt();
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

  std::shared_ptr<GotoStmtNode> Parser::ParseGotoStmt() {
    auto node = std::make_shared<GotoStmtNode>(Lex.CurrentToken);
    Lex.SkipToken(TokenKind::Goto);
    if (!Lex.Match(TokenKind::Id)) {
      ParseDiag(node->Tok->Location, "expected a identifier");
    }
    node->LabelName = Lex.CurrentToken->Content;
    Lex.SkipToken(TokenKind::Id);
    Lex.ExpectToken(TokenKind::Semicolon);
    return node;
  }

  std::shared_ptr<StmtNode>  Parser::ParseLabelStmt() {
    Lex.BeginPeekToken();
    Lex.GetNextToken();
    if (Lex.Match(TokenKind::Colon)) {
      Lex.EndPeekToken();
      auto node = std::make_shared<LabelStmtNode>(Lex.CurrentToken);
      node->LabelName = Lex.CurrentToken->Content;
      Lex.SkipToken(TokenKind::Id);
      Lex.SkipToken(TokenKind::Colon);
      node->Stmt = ParseStmt();
      return node;
    }
    Lex.EndPeekToken();
    return ParseExprStmt();
  }

  std::shared_ptr<CaseStmtNode> Parser::ParseCaseStmt() {
    auto node = std::make_shared<CaseStmtNode>(Lex.CurrentToken);
    Lex.SkipToken(TokenKind::Case);
    node->Expr = ParseExpr();
    Lex.ExpectToken(TokenKind::Colon);
    node->Stmt = ParseStmt();
    return node;
  }

  std::shared_ptr<DefaultStmtNode>  Parser::ParseDefaultStmt() {
    auto node = std::make_shared<DefaultStmtNode>(Lex.CurrentToken);
    Lex.SkipToken(TokenKind::Default);
    Lex.ExpectToken(TokenKind::Colon);
    node->Stmt = ParseStmt();
    return node;
  }

  std::shared_ptr<SwitchStmtNode>   Parser::ParseSwitchStmt() {
    auto node = std::make_shared<SwitchStmtNode>(Lex.CurrentToken);
    Lex.SkipToken(TokenKind::Switch);
    Lex.ExpectToken(TokenKind::LParen);
    node->Expr = ParseExpr();
    Lex.ExpectToken(TokenKind::RParen);
    node->Stmt = ParseStmt();
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

  void GotoStmtNode::Accept(AstVisitor *visitor) {
    visitor->VisitorGotoStmtNode(this);
  }

  void LabelStmtNode::Accept(AstVisitor *visitor) {
    visitor->VisitorLabelStmtNode(this);
  }

  void CaseStmtNode::Accept(AstVisitor *visitor) {
    visitor->VisitorCaseStmtNode(this);
  }

  void DefaultStmtNode::Accept(AstVisitor *visitor) {
    visitor->VisitorDefaultStmtNode(this);
  }

  void SwitchStmtNode::Accept(AstVisitor *visitor) {
    visitor->VisitorSwitchStmtNode(this);
  }
}
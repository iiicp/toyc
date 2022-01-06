/**********************************
 * File:     Parser.cpp.cc
 *
 * Author:   caipeng
 *
 * Email:    iiicp@outlook.com
 *
 * Date:     2022/1/3
 ***********************************/

#include "Parser.h"
#include "Diag.h"
#include <string>

using namespace C100;

std::shared_ptr<ProgramNode> Parser::Parse()
{
  auto node = std::make_shared<ProgramNode>();
  Locals = &node->LocalVars;
  while (Lex.CurrentToken->Kind != TokenKind::Eof) {
    node->Stmts.push_back(ParseStmt());
  }

  return node;
}

std::shared_ptr<AstNode> Parser::ParseExpr()
{
  return ParseAssignExpr();
}

std::shared_ptr<AstNode> Parser::ParseAddExpr()
{
  std::shared_ptr<AstNode> left = ParseMultiExpr();
  while (Lex.CurrentToken->Kind == TokenKind::Add
         || Lex.CurrentToken->Kind == TokenKind::Sub) {
    BinaryOperator anOperator = BinaryOperator::Add;
    if (Lex.CurrentToken->Kind == TokenKind::Sub)
      anOperator = BinaryOperator::Sub;
    Lex.GetNextToken();
    auto node = std::make_shared<BinaryNode>();
    node->BinOp = anOperator;
    node->Lhs = left;
    node->Rhs = ParseMultiExpr();
    left = node;
  }
  return left;
}

std::shared_ptr<AstNode> Parser::ParseMultiExpr()
{
  std::shared_ptr<AstNode> left = ParsePrimaryExpr();
  while (Lex.CurrentToken->Kind == TokenKind::Mul
         || Lex.CurrentToken->Kind == TokenKind::Div) {
    BinaryOperator anOperator = BinaryOperator::Mul;
    if (Lex.CurrentToken->Kind == TokenKind::Div)
      anOperator = BinaryOperator::Div;
    Lex.GetNextToken();
    auto node = std::make_shared<BinaryNode>();
    node->BinOp = anOperator;
    node->Lhs = left;
    node->Rhs = ParsePrimaryExpr();
    left = node;
  }
  return left;
}

std::shared_ptr<AstNode> Parser::ParsePrimaryExpr()
{
    if (Lex.CurrentToken->Kind == TokenKind::LParent) {
        Lex.GetNextToken();
        auto node = ParseExpr();
        Lex.ExpectToken(TokenKind::RParent);
        return node;
    }else if (Lex.CurrentToken->Kind == TokenKind::Num) {
        auto node = std::make_shared<ConstantNode>();
        node->Value = Lex.CurrentToken->Value;
        Lex.GetNextToken();
        return node;
    }else if (Lex.CurrentToken->Kind == TokenKind::Identifier) {
        auto node = std::make_shared<VarExprNode>();

        std::shared_ptr<Var> obj = FindLocalVar(Lex.CurrentToken->Content);
        if (!obj) {
          obj = MakeLocalVar(Lex.CurrentToken->Content);
        }
        node->VarObj = obj;
        Lex.GetNextToken();
        return node;
    }else {
      DiagE(Lex.SourceCode, Lex.CurrentToken->Location,
            "not support");
    }
  return nullptr;
}

std::shared_ptr<AstNode> Parser::ParseStmt()
{
  auto node = std::make_shared<ExprStmtNode>();
  node->Lhs = ParseExpr();
  Lex.ExpectToken(TokenKind::Semicolon);
  return node;
}

std::shared_ptr<AstNode> Parser::ParseAssignExpr()
{
  auto left = ParseAddExpr();

  if (Lex.CurrentToken->Kind == TokenKind::Assign) {
    Lex.GetNextToken();
    auto node = std::make_shared<AssignExprNode>();
    node->Lhs = left;
    node->Rhs = ParseAssignExpr();
    return node;
  }
  return left;
}

std::shared_ptr<Var> Parser::FindLocalVar(std::string_view name) {
  if (LocalsMap.find(name) != LocalsMap.end()) {
    return LocalsMap[name];
  }
  return nullptr;
}

std::shared_ptr<Var> Parser::MakeLocalVar(std::string_view name) {
  auto obj = std::make_shared<Var>();
  obj->Name = name;
  obj->Offset = 0;
  Locals->push_front(obj);
  LocalsMap[name] = obj;
  return obj;
}


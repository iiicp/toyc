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
  while (Lex.CurrentToken->Kind != TokenKind::Eof) {
    node->Funcs.push_back(ParseFunction());
  }

  return node;
}

std::shared_ptr<AstNode> Parser::ParseFunction()
{
  auto node = std::make_shared<FunctionNode>();
  Locals = &node->Locals;
  LocalsMap.clear();

  auto baseType = ParseDeclarationSpec();
  std::shared_ptr<Token> tok;
  std::shared_ptr<Type> ty = ParseDeclarator(baseType, tok);

  node->FuncName = tok->Content;

  auto funcTy = std::dynamic_pointer_cast<FunctionType>(ty);
  ///node->Params.reserve(funcTy->Params.size());
  for (int i = funcTy->Params.size()-1; i >= 0; i--) {
     node->Params.push_front(MakeLocalVar(funcTy->Params[i]->Tok->Content, funcTy->Params[i]->Ty));
  }
  node->Ty = funcTy;
  Lex.ExpectToken(TokenKind::LBrace);
  while (Lex.CurrentToken->Kind != TokenKind::RBrace) {
    node->Stmts.push_back(ParseStmt());
  }
  Lex.ExpectToken(TokenKind::RBrace);

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

        Lex.BeginPeekToken();
        Lex.GetNextToken();
        if (Lex.CurrentToken->Kind == TokenKind::LParent) {
          Lex.EndPeekToken();
          return ParseFuncCallNode();
        }
        Lex.EndPeekToken();
        auto node = std::make_shared<VarExprNode>();
        std::shared_ptr<Var> obj = FindLocalVar(Lex.CurrentToken->Content);
        if (!obj) {
          DiagE(Lex.SourceCode, Lex.CurrentToken->Location, "undefined variable");
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
  if (Lex.CurrentToken->Kind == TokenKind::If) {
    auto node = std::make_shared<IfStmtNode>();
    Lex.GetNextToken();
    Lex.ExpectToken(TokenKind::LParent);
    node->Cond = ParseExpr();
    Lex.ExpectToken(TokenKind::RParent);
    node->Then = ParseStmt();
    if (Lex.CurrentToken->Kind == TokenKind::Else) {
      Lex.GetNextToken();
      node->Else = ParseStmt();
    }
    return node;
  }
  else if (Lex.CurrentToken->Kind == TokenKind::While) {
    auto node = std::make_shared<WhileStmtNode>();
    Lex.GetNextToken();
    Lex.ExpectToken(TokenKind::LParent);
    node->Cond = ParseExpr();
    Lex.ExpectToken(TokenKind::RParent);
    node->Then = ParseStmt();
    return node;
  }
  else if (Lex.CurrentToken->Kind == TokenKind::Do) {
    auto node = std::make_shared<DoWhileStmtNode>();
    Lex.GetNextToken();
    node->Stmt = ParseStmt();
    Lex.ExpectToken(TokenKind::While);
    Lex.ExpectToken(TokenKind::LParent);
    node->Cond = ParseExpr();
    Lex.ExpectToken(TokenKind::RParent);
    Lex.ExpectToken(TokenKind::Semicolon);
    return node;
  }
  else if (Lex.CurrentToken->Kind == TokenKind::For) {
    auto node = std::make_shared<ForStmtNode>();
    Lex.GetNextToken();
    Lex.ExpectToken(TokenKind::LParent);
    if (Lex.CurrentToken->Kind != TokenKind::Semicolon)
      node->Init = ParseExpr();
    Lex.ExpectToken(TokenKind::Semicolon);
    if (Lex.CurrentToken->Kind != TokenKind::Semicolon)
      node->Cond = ParseExpr();
    Lex.ExpectToken(TokenKind::Semicolon);
    if (Lex.CurrentToken->Kind != TokenKind::RParent)
      node->Inc = ParseExpr();
    Lex.ExpectToken(TokenKind::RParent);
    node->Stmt = ParseStmt();
    return node;
  }
  else if (Lex.CurrentToken->Kind == TokenKind::LBrace) {
    auto node = std::make_shared<BlockStmtNode>();
    Lex.GetNextToken();
    while (Lex.CurrentToken->Kind != TokenKind::RBrace) {
      node->Stmts.push_back(ParseStmt());
    }
    Lex.ExpectToken(TokenKind::RBrace);
    return node;
  }
  else if (Lex.CurrentToken->Kind == TokenKind::Return) {
    auto node = std::make_shared<ReturnStmtNode>();
    Lex.GetNextToken();
    node->Lhs = ParseExpr();
    Lex.ExpectToken(TokenKind::Semicolon);
    return node;
  }
  else if (IsTypeName()) {
    auto baseType = ParseDeclarationSpec();
    auto node = std::make_shared<BlockStmtNode>();
    int i = 0;
    while (Lex.CurrentToken->Kind != TokenKind::Semicolon) {
      if (i > 0) {
        Lex.ExpectToken(TokenKind::Comma);
      }
      std::shared_ptr<Token> tok;
      auto ty = ParseDeclarator(baseType, tok);
      auto varObj = MakeLocalVar(tok->Content, ty);
      i++;
      if (Lex.CurrentToken->Kind != TokenKind::Assign) {
        continue;
      }
      Lex.GetNextToken();
      auto assign = std::make_shared<AssignExprNode>();
      assign->Lhs = MakeVarNode(varObj);
      assign->Rhs = ParseExpr();

      auto exprStmt = std::make_shared<ExprStmtNode>();
      exprStmt->Lhs = assign;
      node->Stmts.push_back(exprStmt);
    }
    Lex.ExpectToken(TokenKind::Semicolon);
    return node;
  }
  else {
    auto node = std::make_shared<ExprStmtNode>();
    if (Lex.CurrentToken->Kind != TokenKind::Semicolon) {
      node->Lhs = ParseExpr();
    }
    Lex.ExpectToken(TokenKind::Semicolon);
    return node;
  }
}


std::shared_ptr<AstNode> Parser::ParseEqualExpr() {
  auto left = ParseRelationalExpr();
  while (Lex.CurrentToken->Kind == TokenKind::Equal ||
  Lex.CurrentToken->Kind == TokenKind::PipeEqual ) {
    BinaryOperator op = BinaryOperator::Equal;
    if (Lex.CurrentToken->Kind == TokenKind::PipeEqual)
      op = BinaryOperator::PipeEqual;
    Lex.GetNextToken();
    auto node = std::make_shared<BinaryNode>();
    node->BinOp = op;
    node->Lhs = left;
    node->Rhs = ParseRelationalExpr();
    left = node;
  }
  return left;
}

std::shared_ptr<AstNode> Parser::ParseRelationalExpr() {
  auto left = ParseAddExpr();
  while (Lex.CurrentToken->Kind == TokenKind::Greater ||
      Lex.CurrentToken->Kind == TokenKind::GreaterEqual ||
      Lex.CurrentToken->Kind == TokenKind::Lesser ||
      Lex.CurrentToken->Kind == TokenKind::LesserEqual) {
    BinaryOperator op = BinaryOperator::Greater;
    if (Lex.CurrentToken->Kind == TokenKind::GreaterEqual)
      op = BinaryOperator::GreaterEqual;
    else if (Lex.CurrentToken->Kind == TokenKind::Lesser)
      op = BinaryOperator::Lesser;
    else if (Lex.CurrentToken->Kind == TokenKind::LesserEqual)
      op = BinaryOperator::LesserEqual;

    Lex.GetNextToken();
    auto node = std::make_shared<BinaryNode>();
    node->BinOp = op;
    node->Lhs = left;
    node->Rhs = ParseAddExpr();
    left = node;
  }
  return left;
}

std::shared_ptr<AstNode> Parser::ParseAssignExpr()
{
  auto left = ParseEqualExpr();

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

std::shared_ptr<Var> Parser::MakeLocalVar(std::string_view name, std::shared_ptr<Type> ty) {
  auto obj = std::make_shared<Var>();
  obj->Name = name;
  obj->Ty = ty;
  obj->Offset = 0;
  Locals->push_front(obj);
  LocalsMap[name] = obj;
  return obj;
}

std::shared_ptr<AstNode> Parser::ParseFuncCallNode() {

  auto node = std::make_shared<FuncCallNode>();
  node->FuncName = Lex.CurrentToken->Content;
  Lex.ExpectToken(TokenKind::Identifier);
  Lex.ExpectToken(TokenKind::LParent);
  if (Lex.CurrentToken->Kind != TokenKind::RParent) {
    node->Args.push_back(ParseAssignExpr());
    while (Lex.CurrentToken->Kind == TokenKind::Comma) {
      Lex.GetNextToken();
      node->Args.push_back(ParseAssignExpr());
    }
  }
  Lex.ExpectToken(TokenKind::RParent);
  return node;
}

std::shared_ptr<Type> Parser::ParseDeclarationSpec() {
  if (Lex.CurrentToken->Kind == TokenKind::Int) {
    Lex.GetNextToken();
    return Type::IntType;
  }
  DiagE(Lex.SourceCode, Lex.CurrentToken->Location, "type not support yet!");
  return nullptr;
}

std::shared_ptr<Type> Parser::ParseDeclarator(std::shared_ptr<Type> baseType, std::shared_ptr<Token> &tok)
{
  auto ty = baseType;
  while (Lex.CurrentToken->Kind == TokenKind::Mul) {
    ty = std::make_shared<PointerType>(ty);
    Lex.GetNextToken();
  }

  if (Lex.CurrentToken->Kind != TokenKind::Identifier) {
    DiagE(Lex.SourceCode, Lex.CurrentToken->Location, "expected a variable name");
  }
  tok = Lex.CurrentToken;
  Lex.GetNextToken();

  ty = ParseTypeSuffix(ty);
  return ty;
}

std::shared_ptr<Type> Parser::ParseTypeSuffix(std::shared_ptr<Type> baseType)
{
  if (Lex.CurrentToken->Kind == TokenKind::LParent) {
    auto funcTy = std::make_shared<FunctionType>(baseType);
    Lex.GetNextToken();
    std::vector<std::shared_ptr<Param>> params;
    if (Lex.CurrentToken->Kind != TokenKind::RParent) {
      std::shared_ptr<Token> tok;
      auto ty = ParseDeclarator(ParseDeclarationSpec(), tok);
      auto pa = std::make_shared<Param>();
      pa->Ty = ty;
      pa->Tok = tok;
      params.push_back(pa);
      while (Lex.CurrentToken->Kind != TokenKind::RParent) {
        Lex.ExpectToken(TokenKind::Comma);
        auto ty = ParseDeclarator(ParseDeclarationSpec(), tok);
        auto pa = std::make_shared<Param>();
        pa->Ty = ty;
        pa->Tok = tok;
        params.push_back(pa);
      }
    }
    funcTy->Params = params;
    Lex.ExpectToken(TokenKind::RParent);
    return funcTy;
  }
  else {
    return baseType;
  }
}

bool Parser::IsTypeName() {
  return Lex.CurrentToken->Kind == TokenKind::Int;
}

std::shared_ptr<VarExprNode> Parser::MakeVarNode(std::shared_ptr<Var> var) {
  auto node = std::make_shared<VarExprNode>();
  node->VarObj = var;
  node->Ty = var->Ty;
  return node;
}



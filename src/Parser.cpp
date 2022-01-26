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
  auto node = std::make_shared<ProgramNode>(Lex.CurrentToken);
  while (Lex.CurrentToken->Kind != TokenKind::Eof) {
    node->Funcs.push_back(ParseFunction());
  }

  return node;
}

std::shared_ptr<AstNode> Parser::ParseFunction()
{

  auto baseType = ParseDeclarationSpec();
  std::shared_ptr<Token> tok;
  std::shared_ptr<Type> ty = ParseDeclarator(baseType, tok);

  auto node = std::make_shared<FunctionNode>(tok);
  Locals = &node->Locals;
  LocalsMap.clear();
  node->FuncName = tok->Content;

  auto funcTy = std::dynamic_pointer_cast<FunctionType>(ty);
  for (auto &param : funcTy->Params) {
     node->Params.push_back(MakeLocalVar(param->Tok->Content, param->Ty));
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
  while (Lex.CurrentToken->Kind == TokenKind::Plus
         || Lex.CurrentToken->Kind == TokenKind::Minus) {

    auto tok = Lex.CurrentToken;
    Lex.GetNextToken();
    auto right = ParseMultiExpr();

    left->Accept(TypeVisitor::Visitor());
    right->Accept(TypeVisitor::Visitor());

    BinaryOperator bop;

    if (tok->Kind == TokenKind::Plus) {
      if ((left->Ty->IsPointerType() || left->Ty->IsArrayType()) && right->Ty->IsIntegerType()) {
        bop = BinaryOperator::PtrAdd;
      } else if (left->Ty->IsIntegerType() && (right->Ty->IsPointerType() || right->Ty->IsArrayType())) {
        auto tmp = left;
        left = right;
        right = tmp;
        bop = BinaryOperator::PtrAdd;
      } else if (left->Ty->IsIntegerType() && right->Ty->IsIntegerType()) {
        bop = BinaryOperator::Add;
      } else {
        DiagLoc(tok->Location, "invalid add operation");
      }
    }else {
      if ((left->Ty->IsPointerType() || left->Ty->IsArrayType()) && right->Ty->IsIntegerType()) {
        bop = BinaryOperator::PtrSub;
      } else if (left->Ty->IsIntegerType() && right->Ty->IsIntegerType()) {
        bop = BinaryOperator::Sub;
      } else if ((left->Ty->IsPointerType() || left->Ty->IsArrayType()) && (right->Ty->IsPointerType() || right->Ty->IsArrayType())) {
        bop = BinaryOperator::PtrDiff;
      }
      else {
        DiagLoc(tok->Location, "invalid sub operation");
      }
    }

    auto node = std::make_shared<BinaryNode>(bop, tok);
    node->Lhs = left;
    node->Rhs = right;
    left = node;
  }
  return left;
}

std::shared_ptr<AstNode> Parser::ParseMultiExpr()
{
  std::shared_ptr<AstNode> left = ParseUnaryExpr();
  while (Lex.CurrentToken->Kind == TokenKind::Star
         || Lex.CurrentToken->Kind == TokenKind::Slash) {
    BinaryOperator anOperator = BinaryOperator::Mul;
    if (Lex.CurrentToken->Kind == TokenKind::Slash)
      anOperator = BinaryOperator::Div;
    auto node = std::make_shared<BinaryNode>(anOperator, Lex.CurrentToken);
    Lex.GetNextToken();
    node->Lhs = left;
    node->Rhs = ParseUnaryExpr();
    left = node;
  }
  return left;
}

std::shared_ptr<AstNode> Parser::ParsePostFixExpr() {
  auto left = ParsePrimaryExpr();
  while (true) {
//    if (Lex.CurrentToken->Kind == TokenKind::LParen) {
//      return ParseFuncCallNode(left->Tok);
//    }
//    else
      if (Lex.CurrentToken->Kind == TokenKind::LBracket) {
      /// x[y] = > *(x+y)
      Lex.SkipToken(TokenKind::LBracket);

      left->Accept(TypeVisitor::Visitor());
      auto right = ParseExpr();
      right->Accept(TypeVisitor::Visitor());
      if (left->Ty->IsIntegerType() && right->Ty->IsArrayType()) {
        auto tmp = left;
        left = right;
        right = tmp;
      }

      auto addNode = std::make_shared<BinaryNode>(BinaryOperator::PtrAdd, left->Tok);
      addNode->Lhs = left;
      addNode->Rhs = right;
      auto starNode = std::make_shared<UnaryNode>(UnaryOperator::Star, left->Tok);
      starNode->Lhs = addNode;
      Lex.ExpectToken(TokenKind::RBracket);
      left = starNode;
      continue;
    }
    else if (Lex.CurrentToken->Kind == TokenKind::Period) {
      Lex.SkipToken(TokenKind::Period);
      auto node = MakeMemberAccessNode(left);
      left = node;
      continue;
    }else {
      break;
    }
  }
  return left;
}

std::shared_ptr<AstNode> Parser::ParseUnaryExpr() {
    switch (Lex.CurrentToken->Kind) {
    case TokenKind::Plus: {
      auto node = std::make_shared<UnaryNode>(UnaryOperator::Plus, Lex.CurrentToken);
      Lex.GetNextToken();
      node->Lhs = ParseUnaryExpr();
      return node;
    }
    case TokenKind::Minus: {
      auto node = std::make_shared<UnaryNode>(UnaryOperator::Minus, Lex.CurrentToken);
      Lex.GetNextToken();
      node->Lhs = ParseUnaryExpr();
      return node;
    }
    case TokenKind::Star: {
      auto node = std::make_shared<UnaryNode>(UnaryOperator::Star, Lex.CurrentToken);
      Lex.GetNextToken();
      node->Lhs = ParseUnaryExpr();
      return node;
    }
    case TokenKind::Amp: {
      auto node = std::make_shared<UnaryNode>(UnaryOperator::Amp, Lex.CurrentToken);
      Lex.GetNextToken();
      node->Lhs = ParseUnaryExpr();
      return node;
      }
    default:
      return ParsePostFixExpr();
    }
}

std::shared_ptr<AstNode> Parser::ParsePrimaryExpr()
{
    if (Lex.CurrentToken->Kind == TokenKind::LParen) {
        Lex.BeginPeekToken();
        Lex.GetNextToken();
        if (Lex.CurrentToken->Kind == TokenKind::LBrace) {
          Lex.EndPeekToken();
          auto tok = Lex.CurrentToken;
          Lex.SkipToken(TokenKind::LParen);
          Lex.SkipToken(TokenKind::LBrace);
          auto node = std::make_shared<StmtExprNode>(tok);
          while (Lex.CurrentToken->Kind != TokenKind::RBrace) {
            node->Stmts.push_back(ParseStmt());
          }
          Lex.SkipToken(TokenKind::RBrace);
          Lex.ExpectToken(TokenKind::RParen);
          return node;
        }
        Lex.EndPeekToken();

        Lex.GetNextToken();
        auto node = ParseExpr();
        Lex.ExpectToken(TokenKind::RParen);
        return node;
    }else if (Lex.CurrentToken->Kind == TokenKind::Num) {
        auto node = std::make_shared<NumNode>(Lex.CurrentToken);
        node->Value = Lex.CurrentToken->Value;
        Lex.GetNextToken();
        return node;
    }else if (Lex.CurrentToken->Kind == TokenKind::Identifier) {

        Lex.BeginPeekToken();
        Lex.GetNextToken();
        if (Lex.CurrentToken->Kind == TokenKind::LParen) {
          Lex.EndPeekToken();
          auto tok = Lex.CurrentToken;
          Lex.SkipToken(TokenKind::Identifier);
          return ParseFuncCallNode(tok);
        }
        Lex.EndPeekToken();
        auto node = std::make_shared<VarExprNode>(Lex.CurrentToken);
        std::shared_ptr<Var> obj = FindLocalVar(Lex.CurrentToken->Content);
        if (!obj) {
          DiagLoc(Lex.CurrentToken->Location, "undefined variable");
        }
        node->VarObj = obj;
        Lex.GetNextToken();
        return node;
    }else if (Lex.CurrentToken->Kind == TokenKind::SizeOf) {
        auto node = std::make_shared<SizeofExprNode>(Lex.CurrentToken);
        Lex.SkipToken(TokenKind::SizeOf);
        node->Lhs = ParseUnaryExpr();
        return node;
    }
    else {
      DiagLoc(Lex.CurrentToken->Location,
            "not support");
    }
  return nullptr;
}

std::shared_ptr<AstNode> Parser::ParseStmt()
{
  if (Lex.CurrentToken->Kind == TokenKind::If) {
    auto node = std::make_shared<IfStmtNode>(Lex.CurrentToken);
    Lex.SkipToken(TokenKind::If);
    Lex.ExpectToken(TokenKind::LParen);
    node->Cond = ParseExpr();
    Lex.ExpectToken(TokenKind::RParen);
    node->Then = ParseStmt();
    if (Lex.CurrentToken->Kind == TokenKind::Else) {
      Lex.SkipToken(TokenKind::Else);
      node->Else = ParseStmt();
    }
    return node;
  }
  else if (Lex.CurrentToken->Kind == TokenKind::While) {
    auto node = std::make_shared<WhileStmtNode>(Lex.CurrentToken);
    Lex.SkipToken(TokenKind::While);
    Lex.ExpectToken(TokenKind::LParen);
    node->Cond = ParseExpr();
    Lex.ExpectToken(TokenKind::RParen);
    node->Then = ParseStmt();
    return node;
  }
  else if (Lex.CurrentToken->Kind == TokenKind::Do) {
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
  else if (Lex.CurrentToken->Kind == TokenKind::For) {
    auto node = std::make_shared<ForStmtNode>(Lex.CurrentToken);
    Lex.SkipToken(TokenKind::For);
    Lex.ExpectToken(TokenKind::LParen);
    if (Lex.CurrentToken->Kind != TokenKind::Semicolon)
      node->Init = ParseExpr();
    Lex.ExpectToken(TokenKind::Semicolon);
    if (Lex.CurrentToken->Kind != TokenKind::Semicolon)
      node->Cond = ParseExpr();
    Lex.ExpectToken(TokenKind::Semicolon);
    if (Lex.CurrentToken->Kind != TokenKind::RParen)
      node->Inc = ParseExpr();
    Lex.ExpectToken(TokenKind::RParen);
    node->Stmt = ParseStmt();
    return node;
  }
  else if (Lex.CurrentToken->Kind == TokenKind::LBrace) {
    auto node = std::make_shared<BlockStmtNode>(Lex.CurrentToken);
    Lex.SkipToken(TokenKind::LBrace);
    while (Lex.CurrentToken->Kind != TokenKind::RBrace) {
      node->Stmts.push_back(ParseStmt());
    }
    Lex.ExpectToken(TokenKind::RBrace);
    return node;
  }
  else if (Lex.CurrentToken->Kind == TokenKind::Return) {
    auto node = std::make_shared<ReturnStmtNode>(Lex.CurrentToken);
    Lex.SkipToken(TokenKind::Return);
    node->Lhs = ParseExpr();
    Lex.ExpectToken(TokenKind::Semicolon);
    return node;
  }
  else if (IsTypeName()) {
    auto baseType = ParseDeclarationSpec();
    auto node = std::make_shared<DeclarationStmtNode>(Lex.CurrentToken);
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
      auto assign = std::make_shared<AssignExprNode>(Lex.CurrentToken);
      Lex.SkipToken(TokenKind::Assign);
      assign->Lhs = MakeVarNode(varObj, Lex.CurrentToken);
      assign->Rhs = ParseExpr();

      node->AssignNodes.push_back(assign);
    }
    Lex.ExpectToken(TokenKind::Semicolon);
    return node;
  }
  else {
    auto node = std::make_shared<ExprStmtNode>(Lex.CurrentToken);
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
    auto node = std::make_shared<BinaryNode>(op, Lex.CurrentToken);
    Lex.GetNextToken();
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

    auto node = std::make_shared<BinaryNode>(op, Lex.CurrentToken);
    Lex.GetNextToken();
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
    auto node = std::make_shared<AssignExprNode>(Lex.CurrentToken);
    Lex.SkipToken(TokenKind::Assign);
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
  Locals->push_back(obj);
  LocalsMap[name] = obj;
  return obj;
}

std::shared_ptr<AstNode> Parser::ParseFuncCallNode(std::shared_ptr<Token> nameTok) {

  auto node = std::make_shared<FuncCallNode>(nameTok);
  node->FuncName = nameTok->Content;
  Lex.ExpectToken(TokenKind::LParen);
  if (Lex.CurrentToken->Kind != TokenKind::RParen) {
    node->Args.push_back(ParseAssignExpr());
    while (Lex.CurrentToken->Kind == TokenKind::Comma) {
      Lex.GetNextToken();
      node->Args.push_back(ParseAssignExpr());
    }
  }
  Lex.ExpectToken(TokenKind::RParen);
  return node;
}

std::shared_ptr<Type> Parser::ParseDeclarationSpec() {
  if (Lex.CurrentToken->Kind == TokenKind::Char) {
    Lex.SkipToken(TokenKind::Char);
    return Type::CharType;
  }
  if (Lex.CurrentToken->Kind == TokenKind::Short) {
    Lex.SkipToken(TokenKind::Short);
    return Type::ShortType;
  }
  if (Lex.CurrentToken->Kind == TokenKind::Int) {
    Lex.SkipToken(TokenKind::Int);
    return Type::IntType;
  }
  if (Lex.CurrentToken->Kind == TokenKind::Long) {
    Lex.SkipToken(TokenKind::Long);
    return Type::LongType;
  }
  if (Lex.CurrentToken->Kind == TokenKind::Struct) {
    Lex.SkipToken(TokenKind::Struct);
    return ParseStructDeclaration();
  }
  if (Lex.CurrentToken->Kind == TokenKind::Union) {
    Lex.SkipToken(TokenKind::Union);
    return ParseUnionDeclaration();
  }
  DiagLoc(Lex.CurrentToken->Location, "type not support yet!");
  return nullptr;
}

std::shared_ptr<Type> Parser::ParseDeclarator(std::shared_ptr<Type> baseType, std::shared_ptr<Token> &tok)
{
  auto ty = baseType;
  while (Lex.CurrentToken->Kind == TokenKind::Star) {
    ty = std::make_shared<PointerType>(ty);
    Lex.GetNextToken();
  }

  if (Lex.CurrentToken->Kind != TokenKind::Identifier) {
    DiagLoc(Lex.CurrentToken->Location, "expected a variable name");
  }
  tok = Lex.CurrentToken;
  Lex.GetNextToken();

  ty = ParseTypeSuffix(ty);
  return ty;
}

std::shared_ptr<Type> Parser::ParseTypeSuffix(std::shared_ptr<Type> baseType)
{
  if (Lex.CurrentToken->Kind == TokenKind::LParen) {
    auto funcTy = std::make_shared<FunctionType>(baseType);
    Lex.GetNextToken();
    std::list<std::shared_ptr<Param>> params;
    if (Lex.CurrentToken->Kind != TokenKind::RParen) {
      std::shared_ptr<Token> tok;
      auto ty = ParseDeclarator(ParseDeclarationSpec(), tok);
      auto pa = std::make_shared<Param>();
      pa->Ty = ty;
      pa->Tok = tok;
      params.push_back(pa);
      while (Lex.CurrentToken->Kind != TokenKind::RParen) {
        Lex.ExpectToken(TokenKind::Comma);
        auto ty = ParseDeclarator(ParseDeclarationSpec(), tok);
        auto pa = std::make_shared<Param>();
        pa->Ty = ty;
        pa->Tok = tok;
        params.push_back(pa);
      }
    }
    funcTy->Params = params;
    Lex.ExpectToken(TokenKind::RParen);
    return funcTy;
  }
  else if (Lex.CurrentToken->Kind == TokenKind::LBracket) {
    Lex.SkipToken(TokenKind::LBracket);
    int num = Lex.CurrentToken->Value;
    Lex.ExpectToken(TokenKind::Num);
    Lex.ExpectToken(TokenKind::RBracket);
    auto ty = ParseTypeSuffix(baseType);
    return std::make_shared<ArrayType>(ty, num);
  }
  else {
    return baseType;
  }
}

std::shared_ptr<RecordType> Parser::ParseStructOrUnionDeclaration(bool isStruct) {

  RecordType::TagKind kind = RecordType::TagKind::Struct;
  if (!isStruct) {
    kind = RecordType::TagKind::Union;
  }

  auto recordTy = std::make_shared<RecordType>(kind);
  Lex.ExpectToken(TokenKind::LBrace);
  while (Lex.CurrentToken->Kind != TokenKind::RBrace) {
    auto baseTy = ParseDeclarationSpec();
    std::shared_ptr<Token> tok;
    auto ty = ParseDeclarator(baseTy, tok);
    recordTy->Flds.push_back(std::make_shared<Filed>(ty, tok, 0));

    while (Lex.CurrentToken->Kind == TokenKind::Comma) {
      Lex.SkipToken(TokenKind::Comma);
      ty = ParseDeclarator(baseTy, tok);
      recordTy->Flds.push_back(std::make_shared<Filed>(ty, tok, 0));
    }
    Lex.ExpectToken(TokenKind::Semicolon);
  }
  Lex.SkipToken(TokenKind::RBrace);
  return recordTy;
}

std::shared_ptr<Type> Parser::ParseStructDeclaration() {
  auto ty = ParseStructOrUnionDeclaration(true);

  int offset = 0;
  for (auto &fd : ty->Flds) {
    offset = AlignTo(offset, fd->Ty->Align);
    fd->Offset = offset;
    offset += fd->Ty->Size;

    if (ty->Align < fd->Ty->Align)
      ty->Align = fd->Ty->Align;
  }
  ty->Size = AlignTo(offset, ty->Align);

  return ty;
}

std::shared_ptr<Type> Parser::ParseUnionDeclaration() {
  auto ty = ParseStructOrUnionDeclaration(false);

  for (auto &fd : ty->Flds) {
    if (ty->Size < fd->Ty->Size)
      ty->Size = fd->Ty->Size;
    if (ty->Align < fd->Ty->Align)
      ty->Align = fd->Ty->Align;
  }
  return ty;
}

bool Parser::IsTypeName() {
  return Lex.CurrentToken->Kind == TokenKind::Char
      || Lex.CurrentToken->Kind == TokenKind::Short
      || Lex.CurrentToken->Kind == TokenKind::Int
      || Lex.CurrentToken->Kind == TokenKind::Long
      || Lex.CurrentToken->Kind == TokenKind::Struct
      || Lex.CurrentToken->Kind == TokenKind::Union;
}

std::shared_ptr<VarExprNode> Parser::MakeVarNode(std::shared_ptr<Var> var, std::shared_ptr<Token> tok) {
  auto node = std::make_shared<VarExprNode>(tok);
  node->VarObj = var;
  node->Ty = var->Ty;
  return node;
}

std::shared_ptr<AstNode> Parser::MakeMemberAccessNode(std::shared_ptr<AstNode> left) {

  /// 判断左边的类型是否是struct
  left->Accept(TypeVisitor::Visitor());
  if (!left->Ty->IsStructType() && !left->Ty->IsUnionType()) {
    DiagLoc(left->Tok->Location, "not a struct or a union");
  }

  /// 查找是否存在此成员
  auto structTy = std::dynamic_pointer_cast<RecordType>(left->Ty);
  std::shared_ptr<Filed> fd = nullptr;
  for (auto &elem : structTy->Flds) {
    if (Lex.CurrentToken->Content == elem->Tok->Content) {
      fd = elem;
      break;
    }
  }
  if (fd == nullptr) {
    DiagLoc(left->Tok->Location, "no member named '%s' in '%s'",
            std::string(Lex.CurrentToken->Content).data(), std::string(left->Tok->Content).data());
  }
  auto node = std::make_shared<MemberAccessNode>(Lex.CurrentToken);
  node->Lhs = left;
  node->Fld = fd;
  Lex.ExpectToken(TokenKind::Identifier);

  return node;
}





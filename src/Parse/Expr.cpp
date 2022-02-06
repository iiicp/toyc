/**********************************
 * File:     Expr.cpp.cc
 *
 * Author:   caipeng
 *
 * Email:    iiicp@outlook.com
 *
 * Date:     2022/1/30
 ***********************************/

#include "Expr.h"
#include "Parser.h"
#include "Type.h"

namespace CCC
{

  /**
   expression:
      assignment-expression
      expression , assignment-expression
   */
  std::shared_ptr<ExprNode> Parser::ParseExpr()
  {
    return ParseAssignExpr();
  }

  /**
    constant-expression:
        conditional-expression
   */
  std::shared_ptr<ExprNode> Parser::ParseConstantExpression()
  {
    return ParseAssignExpr();
  }

  /**
    conditional-expression:
       logical-OR-expression
       logical-OR-expression ? expression : conditional-expression
  */
  std::shared_ptr<ExprNode> Parser::ParseConditionalExpression()
  {
    return nullptr;
  }

  /**
   assignment-expression:
      conditional-expression
      unary-expression assignment-operator assignment-expression

    assignment-operator: one of
                =  *=  /=  %=  +=  -=  <<=  >>=  &=  ^=  |=
   */
  std::shared_ptr<ExprNode> Parser::ParseAssignExpr()
  {
    auto left = ParseEqualExpr();

    if (Lex.CurIs(TokenKind::Assign)) {
      auto node = std::make_shared<AssignExpr>(Lex.CurrentToken);
      Lex.SkipToken(TokenKind::Assign);
      node->Lhs = left;
      node->Rhs = ParseAssignExpr();
      return node;
    }
    return left;
  }

  /**
   equality-expression:
      relational-expression
      equality-expression == relational-expression
      equality-expression != relational-expression
   */
  std::shared_ptr<ExprNode> Parser::ParseEqualExpr() {
    auto left = ParseRelationalExpr();
    while (Lex.CurIs(TokenKind::Equal) ||
        Lex.CurIs(TokenKind::PipeEqual) ) {
      BinaryOperator op = BinaryOperator::Equal;
      if (Lex.CurIs(TokenKind::PipeEqual))
        op = BinaryOperator::PipeEqual;
      auto node = std::make_shared<BinaryExpr>(op, Lex.CurrentToken);
      Lex.GetNextToken();
      node->Lhs = left;
      node->Rhs = ParseRelationalExpr();
      left = node;
    }
    return left;
  }

  /**
   relational-expression:
      shift-expression
      relational-expression < shift-expression
      relational-expression > shift-expression
      relational-expression <= shift-expression
      relational-expression >= shift-expression
   */
  std::shared_ptr<ExprNode> Parser::ParseRelationalExpr() {
    auto left = ParseAddExpr();
    while (Lex.CurIs(TokenKind::Greater) ||
        Lex.CurIs(TokenKind::GreaterEqual) ||
        Lex.CurIs(TokenKind::Lesser) ||
        Lex.CurIs(TokenKind::LesserEqual)) {

      BinaryOperator op = BinaryOperator::Greater;
      if (Lex.CurIs(TokenKind::GreaterEqual))
        op = BinaryOperator::GreaterEqual;
      else if (Lex.CurIs(TokenKind::Lesser))
        op = BinaryOperator::Lesser;
      else if (Lex.CurIs(TokenKind::LesserEqual))
        op = BinaryOperator::LesserEqual;

      auto node = std::make_shared<BinaryExpr>(op, Lex.CurrentToken);
      Lex.GetNextToken();
      node->Lhs = left;
      node->Rhs = ParseAddExpr();
      left = node;
    }
    return left;
  }

  /**
   additive-expression:
      multiplicative-expression
      additive-expression + multiplicative-expression
      additive-expression - multiplicative-expression
   */
  std::shared_ptr<ExprNode> Parser::ParseAddExpr()
  {
    std::shared_ptr<ExprNode> left = ParseMultiExpr();
    while (Lex.CurIs(TokenKind::Plus)
        || Lex.CurIs(TokenKind::Minus)) {

      BinaryOperator bop = BinaryOperator::Add;
      if (Lex.CurIs(TokenKind::Minus))
        bop = BinaryOperator::Sub;

      auto node = std::make_shared<BinaryExpr>(bop, Lex.CurrentToken);
      Lex.GetNextToken();
      node->Lhs = left;
      node->Rhs = ParseMultiExpr();
      left = node;
    }
    return left;
  }

  /**
   multiplicative-expression:
      cast-expression
      multiplicative-expression * cast-expression
      multiplicative-expression / cast-expression
      multiplicative-expression % cast-expression
   */
  std::shared_ptr<ExprNode> Parser::ParseMultiExpr()
  {
    std::shared_ptr<ExprNode> left = ParseUnaryExpr();
    while (Lex.CurIs(TokenKind::Star)
        || Lex.CurIs(TokenKind::Slash)) {
      BinaryOperator anOperator = BinaryOperator::Mul;
      if (Lex.CurIs(TokenKind::Slash))
        anOperator = BinaryOperator::Div;
      auto node = std::make_shared<BinaryExpr>(anOperator, Lex.CurrentToken);
      Lex.GetNextToken();
      node->Lhs = left;
      node->Rhs = ParseUnaryExpr();
      left = node;
    }
    return left;
  }

  /**
   unary-expression:
      postfix-expression
      ++ unary-expression
      -- unary-expression
      unary-operator cast-expression
      sizeof unary-expression
      sizeof ( type-name )

   unary-operator:
      one of & * + - ~ !
   */
  std::shared_ptr<ExprNode> Parser::ParseUnaryExpr() {
    switch (Lex.CurrentToken->Kind) {
    case TokenKind::Plus: {
      auto node = std::make_shared<UnaryExpr>(UnaryOperator::Plus, Lex.CurrentToken);
      Lex.GetNextToken();
      node->Lhs = ParseUnaryExpr();
      return node;
    }
    case TokenKind::Minus: {
      auto node = std::make_shared<UnaryExpr>(UnaryOperator::Minus, Lex.CurrentToken);
      Lex.GetNextToken();
      node->Lhs = ParseUnaryExpr();
      return node;
    }
    case TokenKind::Star: {
      auto node = std::make_shared<UnaryExpr>(UnaryOperator::Star, Lex.CurrentToken);
      Lex.GetNextToken();
      node->Lhs = ParseUnaryExpr();
      return node;
    }
    case TokenKind::Amp: {
      auto node = std::make_shared<UnaryExpr>(UnaryOperator::Amp, Lex.CurrentToken);
      Lex.GetNextToken();
      node->Lhs = ParseUnaryExpr();
      return node;
    }
    case TokenKind::SizeOf: {
      return ParseSizeofExpr();
    }
    default:
      return ParsePostFixExpr();
    }
  }

  /**
   postfix-expression:
      primary-expression
      postfix-expression [ expression ]
      postfix-expression ( argument-expression-list? )
      postfix-expression . identifier
      postfix-expression -> identifier
      postfix-expression ++
      postfix-expression --
      ( type-name ) { initializer-list }
      ( type-name ) { initializer-list , }
   */

  std::shared_ptr<ExprNode> Parser::ParsePostFixExpr() {
    auto expr = ParsePrimaryExpr();
    while (true) {
      switch (Lex.CurrentToken->Kind) {
        case TokenKind::LBracket: {
          /// a[3] => *(a + 3)
          auto starNode = std::make_shared<UnaryExpr>(UnaryOperator::Star, expr->Tok);
          auto addNode = std::make_shared<BinaryExpr>(BinaryOperator::Add, expr->Tok);
          addNode->Lhs = expr;
          Lex.SkipToken(TokenKind::LBracket);
          addNode->Rhs = ParseExpr();
          Lex.SkipToken(TokenKind::RBracket);
          starNode->Lhs = addNode;

          expr= starNode;
          break;
        }
        case TokenKind::LParen: {
          expr = ParseFuncCallNode(expr);
          break;
        }
        case TokenKind::Period: {
          expr = ParseMemberNode(expr);
          break;
        }
        case TokenKind::Arrow: {
          /// a->b => (*a).b;
          auto starNode = std::make_shared<UnaryExpr>(UnaryOperator::Star, expr->Tok);
          starNode->Lhs = expr;
          expr = ParseMemberNode(starNode);
          break;
        }
        default:
          return expr;
        }
    }
  }

  std::shared_ptr<ExprNode> Parser::ParseFuncCallNode(std::shared_ptr<ExprNode> expr) {

    Lex.SkipToken(TokenKind::LParen);
    auto node = std::make_shared<FuncCallExpr>(expr->Tok);
    node->FuncName = expr->Tok->Content;
    if (Lex.CurNotIs(TokenKind::RParen)) {
      node->Args.push_back(ParseAssignExpr());
      while (Lex.CurIs(TokenKind::Comma)) {
        Lex.SkipToken(TokenKind::Comma);
        node->Args.push_back(ParseAssignExpr());
      }
    }
    Lex.ExpectToken(TokenKind::RParen);
    return node;
  }

  std::shared_ptr<ExprNode> Parser::ParseMemberNode(std::shared_ptr<ExprNode> expr)
  {
    auto node = std::make_shared<MemberExpr>(Lex.CurrentToken);
    Lex.GetNextToken();
    node->Lhs = expr;
    if (Lex.CurNotIs(TokenKind::Id)) {
      ParseDiag(Lex.CurrentToken->Location, "expected a identifier as Struct or Union member");
    }
    node->RhsName = Lex.CurrentToken->Content;
    Lex.SkipToken(TokenKind::Id);
    return node;
  }
  /**
      primary-expression:
          identifier
          constant
          string-literal
          ( expression )
          ({})
   */
  std::shared_ptr<ExprNode> Parser::ParsePrimaryExpr()
  {
    if (Lex.CurIs(TokenKind::LParen)) {
      Lex.BeginPeekToken();
      Lex.GetNextToken();
      if (Lex.CurIs(TokenKind::LBrace)) {
        Lex.EndPeekToken();
        return ParseStmtExpr();
      }
      Lex.EndPeekToken();

      Lex.GetNextToken();
      auto node = ParseExpr();
      Lex.ExpectToken(TokenKind::RParen);
      return node;
    }else if (Lex.CurIs(TokenKind::Num)) {
      return ParseNumExpr();
    }else if (Lex.CurIs(TokenKind::Id)) {
      return ParseVarExpr();
    }else {
      ParseDiag(Lex.CurrentToken->Location, "error expr or not supported");
    }
    return nullptr;
  }

  std::shared_ptr<ExprNode> Parser::ParseStmtExpr()
  {
    auto node = std::make_shared<StmtExpr>(Lex.CurrentToken);

    Lex.SkipToken(TokenKind::LParen);
    Lex.SkipToken(TokenKind::LBrace);


    while (Lex.CurNotIs(TokenKind::RBrace)) {
      if (FirstDeclarationSet.find(Lex.CurrentToken->Kind) != FirstDeclarationSet.end()) {
        node->Decls.push_back(ParseDeclaration());
      }else {
        node->Stmts.push_back(ParseStmt());
      }
    }

    Lex.SkipToken(TokenKind::RBrace);
    Lex.ExpectToken(TokenKind::RParen);

    return node;
  }

  std::shared_ptr<ExprNode> Parser::ParseNumExpr()
  {
    auto node = std::make_shared<NumExpr>(Lex.CurrentToken);
    node->Value = Lex.CurrentToken->Value;
    node->Ty = Type::LongTy;
    Lex.GetNextToken();
    return node;
  }

  std::shared_ptr<ExprNode> Parser::ParseVarExpr()
  {
    auto node = std::make_shared<VarExpr>(Lex.CurrentToken);
    node->VarName = Lex.CurrentToken->Content;
    Lex.GetNextToken();
    return node;
  }

  std::shared_ptr<ExprNode> Parser::ParseSizeofExpr()
  {
    auto node = std::make_shared<SizeOfExpr>(Lex.CurrentToken);
    Lex.SkipToken(TokenKind::SizeOf);
    node->Lhs = ParseUnaryExpr();
    return node;
  }

  void AssignExpr::Accept(AstVisitor *visitor) {
    visitor->VisitorAssignExprNode(this);
  }

  void SizeOfExpr::Accept(AstVisitor *visitor) {
    visitor->VisitorSizeOfExprNode(this);
  }

  void BinaryExpr::Accept(AstVisitor *visitor) {
    visitor->VisitorBinaryExprNode(this);
  }

  void UnaryExpr::Accept(AstVisitor *visitor) {
    visitor->VisitorUnaryExprNode(this);
  }

  void NumExpr::Accept(AstVisitor *visitor) {
    visitor->VisitorNumExprNode(this);
  }

  void VarExpr::Accept(AstVisitor *visitor) {
    visitor->VisitorVarExprNode(this);
  }

  void FuncCallExpr::Accept(AstVisitor *visitor) {
    visitor->VisitorFuncCallExprNode(this);
  }

  void StmtExpr::Accept(AstVisitor *visitor) {
    visitor->VisitorStmtExprNode(this);
  }

  void MemberExpr::Accept(AstVisitor *visitor) {
    visitor->VisitorMemberExprNode(this);
  }
}

/**********************************
 * File:     Parser.h
 *
 * Author:   caipeng
 *
 * Email:    iiicp@outlook.com
 *
 * Date:     2022/1/3
 ***********************************/

#ifndef C100_PARSER_H
#define C100_PARSER_H
#include "Lexer.h"
#include "AstNode.h"
#include <unordered_map>
#include "Type.h"

namespace C100
{
  class Parser
  {
  private:
    Lexer &Lex;
    std::list<std::shared_ptr<Var>> *Locals{nullptr};
    std::unordered_map<std::string_view, std::shared_ptr<Var>> LocalsMap;
  public:
    Parser(Lexer &lex):Lex(lex) {}
    std::shared_ptr<ProgramNode> Parse();

  private:
    std::shared_ptr<AstNode> ParseFunction();
    std::shared_ptr<AstNode> ParseStmt();
    std::shared_ptr<AstNode> ParseExpr();
    std::shared_ptr<AstNode> ParseAssignExpr();
    std::shared_ptr<AstNode> ParseEqualExpr();
    std::shared_ptr<AstNode> ParseRelationalExpr();
    std::shared_ptr<AstNode> ParseAddExpr();
    std::shared_ptr<AstNode> ParseMultiExpr();
    std::shared_ptr<AstNode> ParseUnaryExpr();
    std::shared_ptr<AstNode> ParsePostFixExpr();
    std::shared_ptr<AstNode> ParsePrimaryExpr();

    bool IsTypeName();
    std::shared_ptr<Type> ParseDeclarationSpec();
    std::shared_ptr<Type> ParseDeclarator(std::shared_ptr<Type> baseType, std::shared_ptr<Token> &tok);
    std::shared_ptr<Type> ParseTypeSuffix(std::shared_ptr<Type> baseType);

    std::shared_ptr<RecordType> ParseStructOrUnionDeclaration(bool isStruct);
    std::shared_ptr<Type> ParseStructDeclaration();
    std::shared_ptr<Type> ParseUnionDeclaration();
    std::shared_ptr<AstNode> MakeMemberAccessNode(std::shared_ptr<AstNode> left);

    std::shared_ptr<AstNode> ParseFuncCallNode(std::shared_ptr<Token> nameTok);
    std::shared_ptr<Var> FindLocalVar(std::string_view name);
    std::shared_ptr<Var> MakeLocalVar(std::string_view name, std::shared_ptr<Type> ty);
    std::shared_ptr<VarExprNode> MakeVarNode(std::shared_ptr<Var> var, std::shared_ptr<Token> tok);
  };
}

#endif // C100_PARSER_H

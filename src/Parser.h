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
    std::shared_ptr<AstNode> ParseStmt();
    std::shared_ptr<AstNode> ParseExpr();
    std::shared_ptr<AstNode> ParseAssignExpr();
    std::shared_ptr<AstNode> ParseAddExpr();
    std::shared_ptr<AstNode> ParseMultiExpr();
    std::shared_ptr<AstNode> ParsePrimaryExpr();

    std::shared_ptr<Var> FindLocalVar(std::string_view name);
    std::shared_ptr<Var> MakeLocalVar(std::string_view name);
  };
}

#endif // C100_PARSER_H

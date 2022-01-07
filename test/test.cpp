#include "catch.hpp"
#include "Lexer.h"
#include "Parser.h"
#include "PrintVisitor.h"

using namespace C100;
TEST_CASE("C100_lexer1", "[lexer]") {
    const char *code = "5 + a10000bbb + (1-  3)*4/2;";
    Lexer lex(code);
    lex.GetNextToken();
    REQUIRE("5" == lex.CurrentToken->Content);
    lex.GetNextToken();
    REQUIRE("+" == lex.CurrentToken->Content);
    lex.GetNextToken();
    REQUIRE("a10000bbb" == lex.CurrentToken->Content);
    lex.GetNextToken();
    REQUIRE("+" == lex.CurrentToken->Content);
    lex.GetNextToken();
    REQUIRE("(" == lex.CurrentToken->Content);
    lex.GetNextToken();
    REQUIRE("1" == lex.CurrentToken->Content);
    lex.GetNextToken();
    REQUIRE("-" == lex.CurrentToken->Content);
    lex.GetNextToken();
    REQUIRE("3" == lex.CurrentToken->Content);
    lex.GetNextToken();
    REQUIRE(")" == lex.CurrentToken->Content);
    lex.GetNextToken();
    REQUIRE("*" == lex.CurrentToken->Content);
    lex.GetNextToken();
    REQUIRE("4" == lex.CurrentToken->Content);
    lex.GetNextToken();
    REQUIRE("/" == lex.CurrentToken->Content);
    lex.GetNextToken();
    REQUIRE("2" == lex.CurrentToken->Content);
    lex.GetNextToken();
    REQUIRE(";" == lex.CurrentToken->Content);
    lex.GetNextToken();
    REQUIRE(TokenKind::Eof == lex.CurrentToken->Kind);
}

TEST_CASE("C100_lexer2", "[lexer]") {
  const char *code = "a==3;a!=3;a>3;a>=3;a<3;a<=3;";
  Lexer lex(code);

  lex.GetNextToken();
  REQUIRE("a" == lex.CurrentToken->Content);
  lex.GetNextToken();
  REQUIRE("==" == lex.CurrentToken->Content);
  lex.GetNextToken();
  REQUIRE("3" == lex.CurrentToken->Content);
  lex.GetNextToken();
  REQUIRE(";" == lex.CurrentToken->Content);

  lex.GetNextToken();
  REQUIRE("a" == lex.CurrentToken->Content);
  lex.GetNextToken();
  REQUIRE("!=" == lex.CurrentToken->Content);
  lex.GetNextToken();
  REQUIRE("3" == lex.CurrentToken->Content);
  lex.GetNextToken();
  REQUIRE(";" == lex.CurrentToken->Content);

  lex.GetNextToken();
  REQUIRE("a" == lex.CurrentToken->Content);
  lex.GetNextToken();
  REQUIRE(">" == lex.CurrentToken->Content);
  lex.GetNextToken();
  REQUIRE("3" == lex.CurrentToken->Content);
  lex.GetNextToken();
  REQUIRE(";" == lex.CurrentToken->Content);

  lex.GetNextToken();
  REQUIRE("a" == lex.CurrentToken->Content);
  lex.GetNextToken();
  REQUIRE(">=" == lex.CurrentToken->Content);
  lex.GetNextToken();
  REQUIRE("3" == lex.CurrentToken->Content);
  lex.GetNextToken();
  REQUIRE(";" == lex.CurrentToken->Content);

  lex.GetNextToken();
  REQUIRE("a" == lex.CurrentToken->Content);
  lex.GetNextToken();
  REQUIRE("<" == lex.CurrentToken->Content);
  lex.GetNextToken();
  REQUIRE("3" == lex.CurrentToken->Content);
  lex.GetNextToken();
  REQUIRE(";" == lex.CurrentToken->Content);

  lex.GetNextToken();
  REQUIRE("a" == lex.CurrentToken->Content);
  lex.GetNextToken();
  REQUIRE("<=" == lex.CurrentToken->Content);
  lex.GetNextToken();
  REQUIRE("3" == lex.CurrentToken->Content);
  lex.GetNextToken();
  REQUIRE(";" == lex.CurrentToken->Content);

  lex.GetNextToken();
  REQUIRE(TokenKind::Eof == lex.CurrentToken->Kind);
}

TEST_CASE("C100_parser1", "[parser]")
{
    const char *code = "5 + (1-  3)*4/2;a+2;";
    Lexer lexer(code);
    lexer.GetNextToken();
    Parser parser(lexer);
    auto root = parser.Parse();

    PrintVisitor visitor;
    root->Accept(&visitor);

    REQUIRE("5+1-3*4/2;a+2;" == visitor.Content);
}

TEST_CASE("C100_parser2", "[parser]")
{
  const char *code = "a = 3;a; a + 5 + (1-  3)*4/2;";
  Lexer lexer(code);
  lexer.GetNextToken();
  Parser parser(lexer);
  auto root = parser.Parse();

  PrintVisitor visitor;
  root->Accept(&visitor);

  REQUIRE("a=3;a;a+5+1-3*4/2;" == visitor.Content);
}

TEST_CASE("C100_parser3", "[parser]")
{
  const char *code = "a==3;a!=3;a>3;a>=3;a<3;a<=3;";
  Lexer lexer(code);
  lexer.GetNextToken();
  Parser parser(lexer);
  auto root = parser.Parse();

  PrintVisitor visitor;
  root->Accept(&visitor);

  REQUIRE("a==3;a!=3;a>3;a>=3;a<3;a<=3;" == visitor.Content);
}
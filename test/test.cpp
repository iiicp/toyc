#include "catch.hpp"
#include "Lexer.h"
#include "Parser.h"
#include "PrintVisitor.h"

using namespace C100;
TEST_CASE("C100_lexer", "5 + (1-  3)*4/2") {
    const char *code = "5 + (1-  3)*4/2";
    Lexer lex(code);
    lex.GetNextToken();
    REQUIRE("5" == lex.CurrentToken->Content);
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
    REQUIRE(TokenKind::Eof == lex.CurrentToken->Kind);
}

TEST_CASE("C100_parser", "parse 5 + (1-  3)*4/2")
{
    const char *code = "5 + (1-  3)*4/2";
    Lexer lexer(code);
    lexer.GetNextToken();
    Parser parser(lexer);
    auto root = parser.Parse();

    PrintVisitor visitor;
    root->Accept(&visitor);

    REQUIRE("2431-*/5+" == visitor.Content);
}
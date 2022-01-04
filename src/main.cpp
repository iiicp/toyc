#include <stdio.h>
#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"

using namespace C100;


int main(int argc, char *argv[]) {

  if (argc != 2) {
    printf("please input: ./c100 code\n");
    return 0;
  }

  const char *source = argv[1];
  Lexer lex(source);
  lex.GetNextToken();

  Parser parser(lex);
  CodeGen codeGen;

  auto root = parser.Parse();
  root->Accept(&codeGen);
  return 0;
}
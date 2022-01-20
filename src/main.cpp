#include <stdio.h>
#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"
#include "Type.h"

using namespace C100;


int main(int argc, char *argv[]) {

  if (argc != 2) {
    printf("please input: ./c100 filePath\n");
    return 0;
  }

  Lexer lex(argv[1]);
  lex.GetNextToken();

  Parser parser(lex);
  CodeGen codeGen;

  auto root = parser.Parse();
  root->Accept(TypeVisitor::Visitor());
  root->Accept(&codeGen);
  return 0;
}
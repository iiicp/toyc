#include "Lexer.h"
#include "Parser.h"
#include "Sema.h"
#include "CodeGen.h"

using namespace CCC;

int main(int argc, char *argv[]) {

  if (argc != 2) {
    printf("please input: ./ccc codePath\n");
    return 0;
  }

  Lexer lex(argv[1]);
  Parser parser(lex);
  auto translationUnit =  parser.ParseTranslationUnit();
  Sema sema;
  translationUnit->Accept(&sema);
  CodeGen codeGen;
  translationUnit->Accept(&codeGen);
  return 0;
}
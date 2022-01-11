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

  FILE *fp = fopen(argv[1], "r");
  if (fp == nullptr) {
    printf("file open failed: %s\n", argv[1]);
    return 0;
  }

  char buf[1024 * 10] = {0};
  size_t len = fread(buf, 1, sizeof(buf), fp);
  buf[len] = '\0';

  const char *source = buf;
  Lexer lex(source);
  lex.GetNextToken();

  Parser parser(lex);
  CodeGen codeGen;

  auto root = parser.Parse();
  root->Accept(&codeGen);
  return 0;
}
/**********************************
 * File:     Lexer.cpp.cc
 *
 * Author:   caipeng
 *
 * Email:    iiicp@outlook.com
 *
 * Date:     2022/1/3
 ***********************************/

#include "Lexer.h"
#include <cstring>

using namespace C100;

void Lexer::GetNextChar() {
  if (Cursor == SourceCode.size()) {
    CurChar = '\0';
    Cursor++;
  }else {
    CurChar = SourceCode[Cursor++];
  }
}

void Lexer::GetNextToken() {
  /// 1. skip write space
  while (isspace(CurChar))
    GetNextChar();

  TokenKind kind;
  int value = 0;
  int startPos = Cursor - 1;
  if (CurChar == '\0')
  {
    kind = TokenKind::Eof;
  }
  else if (CurChar == '+'){
    kind = TokenKind::Add;
    GetNextChar();
  }
  else if (CurChar == '-') {
    kind = TokenKind::Sub;
    GetNextChar();
  }
  else if (CurChar == '*') {
    kind = TokenKind::Mul;
    GetNextChar();
  }
  else if (CurChar == '/') {
    kind = TokenKind::Div;
    GetNextChar();
  }
  else if (CurChar == '(') {
    kind = TokenKind::LParent;
    GetNextChar();
  }
  else if (CurChar == ')') {
    kind = TokenKind::RParent;
    GetNextChar();
  }
  else if (CurChar == ';') {
    kind = TokenKind::Semicolon;
    GetNextChar();
  }
  else if (CurChar == '=') {
    kind = TokenKind::Assign;
    GetNextChar();
  }
  else if (isdigit(CurChar)) {
    kind = TokenKind::Num;
    value = 0;
    do {
      value = value * 10 + CurChar - '0';
      GetNextChar();
    } while (isdigit(CurChar));
  }else {
    if (IsLetter()) {
      while (IsLetterOrDigit()) {
        GetNextChar();
      }
      kind = TokenKind::Identifier;
    }else {
      printf("not support %c\n", CurChar);
      assert(0);
    }
  }
  CurrentToken = std::make_shared<Token>();
  CurrentToken->Kind = kind;
  CurrentToken->Value = value;
  CurrentToken->Content = SourceCode.substr(startPos, Cursor-1-startPos);
}

bool Lexer::IsLetter()
{
  return (CurChar >= 'a' && CurChar <= 'z') ||  (CurChar >= 'A' && CurChar <= 'Z') || (CurChar == '_');
}

bool Lexer::IsDigit()
{
  return CurChar >= '0' && CurChar <= '9';
}

bool Lexer::IsLetterOrDigit()
{
  return IsLetter() || IsDigit();
}

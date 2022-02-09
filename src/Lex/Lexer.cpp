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
#include "Basic/Diag.h"
#include <cassert>
#include <cstring>
#include <string>
#include "ReserveWord.cpp"

using namespace CCC;

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
  SkipWhiteSpace();

  SourceLocation Location;
  Location.Line = Line;
  Location.Col = Cursor-1-LineHead;
  Location.LineHead = LineHead;
  Location.FilePath = CurrentFilePath;
  Location.Code = SourceCode;

  TokenKind kind;
  int value = 0;
  int startPos = Cursor - 1;
  if (CurChar == '\0')
  {
    kind = TokenKind::Eof;
  }
  else if (CurChar == '+'){
    kind = TokenKind::Plus;
    GetNextChar();
  }
  else if (CurChar == '-') {
    if (PeekChar(1) == '>') {
      GetNextChar();
      kind = TokenKind::Arrow;
    }else {
      kind = TokenKind::Minus;
    }
    GetNextChar();
  }
  else if (CurChar == '*') {
    kind = TokenKind::Star;
    GetNextChar();
  }
  else if (CurChar == '/') {
    kind = TokenKind::Slash;
    GetNextChar();
  }
  else if (CurChar == '&') {
    kind = TokenKind::Amp;
    GetNextChar();
  }
  else if (CurChar == '(') {
    kind = TokenKind::LParen;
    GetNextChar();
  }
  else if (CurChar == ')') {
    kind = TokenKind::RParen;
    GetNextChar();
  }
  else if (CurChar == '[') {
    kind = TokenKind::LBracket;
    GetNextChar();
  }
  else if (CurChar == ']') {
    kind = TokenKind::RBracket;
    GetNextChar();
  }
  else if (CurChar == '{') {
    kind = TokenKind::LBrace;
    GetNextChar();
  }
  else if (CurChar == '}') {
    kind = TokenKind::RBrace;
    GetNextChar();
  }
  else if (CurChar == ';') {
    kind = TokenKind::Semicolon;
    GetNextChar();
  }
  else if (CurChar == ',') {
    kind = TokenKind::Comma;
    GetNextChar();
  }
  else if (CurChar == '.') {
    if (PeekChar(1) == '.' && PeekChar(2) == '.') {
      GetNextChar();
      GetNextChar();
      kind = TokenKind::Ellipsis;
    }else {
      kind = TokenKind::Period;
    }
    GetNextChar();
  }
  else if (CurChar == '=') {
    if (PeekChar(1) == '=') {
      GetNextChar();
      kind = TokenKind::Equal;
    }else {
      kind = TokenKind::Assign;
    }
    GetNextChar();
  }
  else if (CurChar == '!') {
    if (PeekChar(1) == '=') {
      GetNextChar();
      kind = TokenKind::PipeEqual;
    }else {
      LexDiag(Location, "current '%c' is illegal", CurChar);
    }
    GetNextChar();
  }
  else if (CurChar == '>') {
    if (PeekChar(1) == '=') {
      GetNextChar();
      kind = TokenKind::GreaterEqual;
    }else {
      kind = TokenKind::Greater;
    }
    GetNextChar();
  }
  else if (CurChar == '<') {
    if (PeekChar(1) == '=') {
      GetNextChar();
      kind = TokenKind::LesserEqual;
    }else {
      kind = TokenKind::Lesser;
    }
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
      kind = TokenKind::Id;
      std::string_view content = SourceCode.substr(startPos, Cursor-1-startPos);
      if (NameToTokenKindMap.find(content) != NameToTokenKindMap.end()) {
        kind = NameToTokenKindMap[content];
      }
    }else {
      LexDiag(Location, "current '%c' is illegal", CurChar);
    }
  }
  CurrentToken = std::make_shared<Token>();
  CurrentToken->Kind = kind;
  CurrentToken->Value = value;
  CurrentToken->Location = Location;
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

void Lexer::ExpectToken(TokenKind kind)
{
  if (CurrentToken->Kind == kind) {
    GetNextToken();
  }else {
    LexDiag(CurrentToken->Location, "'%s' expected", GetTokenSimpleSpelling(kind));
  }
}

void Lexer::SkipToken(TokenKind kind)
{
  if (CurrentToken->Kind == kind) {
    GetNextToken();
  }else {
    LexDiag(CurrentToken->Location, "'%s' skipped", GetTokenSimpleSpelling(kind));
  }
}

bool Lexer::Match(TokenKind kind) {
  return CurrentToken->Kind == kind;
}

const char  *Lexer::GetTokenSimpleSpelling(TokenKind kind){
  if (kind == TokenKind::Eof)
    return "eof";
  if (TokenKindToNameMap.find(kind) != TokenKindToNameMap.end()) {
    return TokenKindToNameMap[kind].data();
  }
  return nullptr;
}

char Lexer::PeekChar(int distance)
{
    assert(distance>=0);
    if (Cursor - 1 + distance < SourceCode.size()) {
      return SourceCode[Cursor-1+distance];
    }else {
      return '\0';
    }
}


void Lexer::BeginPeekToken() {
  PeekPt.CurChar = CurChar;
  PeekPt.Cursor = Cursor;
  PeekPt.Line = Line;
  PeekPt.LineHead = LineHead;
  PeekPt.CurrentToken = CurrentToken;
}

void Lexer::EndPeekToken() {
  CurChar = PeekPt.CurChar;
  Cursor = PeekPt.Cursor;
  Line = PeekPt.Line;
  LineHead = PeekPt.LineHead;
  CurrentToken = PeekPt.CurrentToken;
}

void Lexer::SkipWhiteSpace() {
  while (isspace(CurChar)
  || (CurChar == '/' && PeekChar(1) == '/')
  || (CurChar == '/' && PeekChar(1) == '*')) {
    if (CurChar == '/') {
      SkipComment();
      continue;
    }else if (CurChar == '\n') {
      Line++;
      LineHead = Cursor;
    }
    GetNextChar();
  }
}

void Lexer::SkipComment() {
  if (CurChar == '/' && PeekChar(1) == '/') {
    while (CurChar != '\n')
      GetNextChar();
  }else {
    auto pos = SourceCode.find("*/", Cursor+1);
    if (pos == std::string_view::npos) {
      LexDiag(GetLocation(), "unclosed \"*/\"");
      assert(0);
    }else {
      CurChar = PeekChar((pos + 2) - (Cursor - 1));
      Cursor = pos + 3;
    }
  }
}

SourceLocation Lexer::GetLocation() {
  SourceLocation Location;
  Location.Line = Line;
  Location.Col = Cursor-1-LineHead;
  Location.LineHead = LineHead;
  Location.FilePath = CurrentFilePath;
  Location.Code = SourceCode;
  return Location;
}

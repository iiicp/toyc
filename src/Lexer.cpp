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
#include "Diag.h"
#include <string>
#include <cassert>

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
    kind = TokenKind::Minus;
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
    kind = TokenKind::Period;
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
      DiagLoc(Location, "current '%c' is illegal", CurChar);
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
      kind = TokenKind::Identifier;
      std::string_view content = SourceCode.substr(startPos, Cursor-1-startPos);
      if (content == "if") {
        kind = TokenKind::If;
      }else if (content == "else") {
        kind = TokenKind::Else;
      }else if (content == "while") {
        kind = TokenKind::While;
      }else if (content == "do") {
        kind = TokenKind::Do;
      }else if (content == "for") {
        kind = TokenKind::For;
      }else if (content == "return") {
        kind = TokenKind::Return;
      }else if (content == "char") {
        kind = TokenKind::Char;
      }else if (content == "short") {
        kind = TokenKind::Short;
      }else if (content == "int") {
        kind = TokenKind::Int;
      }else if (content == "long") {
        kind = TokenKind::Long;
      }else if (content == "sizeof") {
        kind = TokenKind::SizeOf;
      }else if (content == "struct") {
        kind = TokenKind::Struct;
      }else if (content == "union") {
        kind = TokenKind::Union;
      }
    }else {
      DiagLoc(Location, "current '%c' is illegal", CurChar);
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
    DiagLoc(CurrentToken->Location, "'%s' expected", GetTokenSimpleSpelling(kind));
  }
}

void Lexer::SkipToken(TokenKind kind)
{
  if (CurrentToken->Kind == kind) {
    GetNextToken();
  }else {
    DiagLoc(CurrentToken->Location, "'%s' skipped", GetTokenSimpleSpelling(kind));
  }
}

const char  *Lexer::GetTokenSimpleSpelling(TokenKind kind){
  switch (kind) {
  case TokenKind::Plus: return "+";
  case TokenKind::Minus: return "-";
  case TokenKind::Star: return "*";
  case TokenKind::Slash: return "/";
  case TokenKind::LParen: return "(";
  case TokenKind::RParen: return ")";
  case TokenKind::LBracket: return "[";
  case TokenKind::RBracket: return "]";
  case TokenKind::LBrace: return "{";
  case TokenKind::RBrace: return "}";
  case TokenKind::Semicolon: return ";";
  case TokenKind::Assign: return "=";
  case TokenKind::Comma: return ",";
  case TokenKind::Amp: return "&";
  case TokenKind::Equal:return "==";
  case TokenKind::PipeEqual:return "!=";
  case TokenKind::Greater:return ">";
  case TokenKind::GreaterEqual:return ">=";
  case TokenKind::Lesser:return "<";
  case TokenKind::LesserEqual:return "<=";
  case TokenKind::If:return "if";
  case TokenKind::Else:return "else";
  case TokenKind::While:return "while";
  case TokenKind::Do: return "do";
  case TokenKind::For:return "for";
  case TokenKind::Return:return "return";
  case TokenKind::Int:return "int";
  case TokenKind::SizeOf:return "sizeof";
  case TokenKind::Eof: return "eof";
  default:
    break;
  }
  return 0;
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
      DiagLoc(GetLocation(), "unclosed \"*/\"");
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

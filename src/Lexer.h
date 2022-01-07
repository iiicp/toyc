/**********************************
 * File:     Lexer.h
 *
 * Author:   caipeng
 *
 * Email:    iiicp@outlook.com
 *
 * Date:     2022/1/3
 ***********************************/

#ifndef C100_LEXER_H
#define C100_LEXER_H

#include <string_view>

namespace C100
{
  enum class TokenKind
  {
      Add,
      Sub,
      Mul,
      Div,
      Num,
      LParent,
      RParent,
      Identifier,
      Semicolon,
      Assign,
      Equal,
      PipeEqual,
      Greater,
      GreaterEqual,
      Lesser,
      LesserEqual,
      Eof
  };

  struct SourceLocation
  {
    int Line;
    int Col;
  };

  class Token
  {
  public:
    TokenKind Kind;
    int Value;
    SourceLocation Location;
    std::string_view Content;
  };

  class Lexer
  {
  private:
    char CurChar{' '};
    int Cursor{0};
    int Line{0};
    int LineHead{0};
  public:
    std::shared_ptr<Token> CurrentToken;
    std::string_view SourceCode;
  public:
    Lexer(const char *code){
      SourceCode = code;
    }
    void GetNextToken();
    void GetNextChar();
    void ExpectToken(TokenKind kind);
  private:
    bool IsLetter();
    bool IsDigit();
    bool IsLetterOrDigit();

    char PeekChar(int distance);

    const char *GetTokenSimpleSpelling(TokenKind kind);
  };
}

#endif // C100_LEXER_H

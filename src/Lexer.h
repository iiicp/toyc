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
#include <memory>
#include <cstdio>
#include <cassert>

namespace C100
{
  enum class TokenKind
  {
      Plus,
      Minus,
      Star,
      Slash,
      Num,
      LParent,
      RParent,
      LBrace,
      RBrace,
      Identifier,
      Semicolon,
      Assign,
      Comma,
      Amp,
      Equal,
      PipeEqual,
      Greater,
      GreaterEqual,
      Lesser,
      LesserEqual,
      If,
      Else,
      While,
      Do,
      For,
      Return,
      Int,
      Eof
  };

  struct SourceLocation
  {
    const char *FilePath;
    std::string_view Code;
    int LineHead;
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

  class PeekPoint
  {
  public:
    char CurChar;
    int Cursor;
    int Line;
    int LineHead;
    std::shared_ptr<Token> CurrentToken;
  };

  class Lexer
  {
  private:
    char CurChar{' '};
    int Cursor{0};
    int Line{0};
    int LineHead{0};
    PeekPoint PeekPt;
    std::string_view SourceCode;
    char *CodeBuf{nullptr};
    const char *CurrentFilePath{nullptr};
  public:
    std::shared_ptr<Token> CurrentToken;
  public:
    Lexer(const char *filePath){
      CurrentFilePath = filePath;
      FILE *fp = fopen(filePath, "r");
      if (fp) {
        fseek(fp, 0, SEEK_END);
        long fileSize = ftell(fp);
        CodeBuf = (char *)malloc(fileSize + 1);
        CodeBuf[fileSize] = '\0';
        fseek(fp, 0, SEEK_SET);
        fread(CodeBuf, fileSize, 1, fp);
        fclose(fp);
      }else {
        printf("%s open failed\n", filePath);
        assert(0);
      }
      SourceCode = CodeBuf;
    }
    void GetNextToken();
    void GetNextChar();
    void ExpectToken(TokenKind kind);

    void BeginPeekToken();
    void EndPeekToken();

  private:
    bool IsLetter();
    bool IsDigit();
    bool IsLetterOrDigit();

    char PeekChar(int distance);

    void SkipWhiteSpace();
    void SkipComment();

    SourceLocation GetLocation();

    const char *GetTokenSimpleSpelling(TokenKind kind);
  };
}

#endif // C100_LEXER_H

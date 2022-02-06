/**********************************
 * File:     Parser.cpp.cc
 *
 * Author:   caipeng
 *
 * Email:    iiicp@outlook.com
 *
 * Date:     2022/1/3
 ***********************************/

#include "Parser.h"
#include <string>

namespace CCC {
  Parser::Parser(Lexer &lex) : Lex(lex) {
    Lex.GetNextToken();
    FirstDeclarationSet = {
        TokenKind::Void, TokenKind::Bool, TokenKind::Char, TokenKind::Short, TokenKind::Int,
        TokenKind::Long, TokenKind::Float, TokenKind::Double, TokenKind::Signed, TokenKind::UnSigned,
        TokenKind::Struct, TokenKind::Union, TokenKind::Enum,
        TokenKind::Auto, TokenKind::Static, TokenKind::Register, TokenKind::Extern, TokenKind::Typedef,
        TokenKind::Const, TokenKind::Volatile,TokenKind::Restrict
    };
  }

  bool Parser::IsFunctionDeclaration(std::shared_ptr<Declarator> dec, std::string_view &funcName)
  {
    while (dec) {
      /// void (*a(int a))(int a);
      /// func->point->func->name
      if (dec->Cls == DeclClass::FunctionDeclarator &&
      dec->Dec && dec->Dec->Cls == DeclClass::NameDeclarator) {
        funcName = dec->Dec->Id;
        return true;
      }
      dec = dec->Dec;
    }
    return false;
  }
}



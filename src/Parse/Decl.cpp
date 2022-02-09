/**********************************
 * File:     Decl.cpp.cc
 *
 * Author:   caipeng
 *
 * Email:    iiicp@outlook.com
 *
 * Date:     2022/1/30
 ***********************************/

#include "Decl.h"
#include "Parser.h"

namespace CCC
{
  /**
     translation-unit:
        external-declaration
        translation-unit external-declaration

      external-declaration:
        function-definition
        declaration
   */
  std::shared_ptr<TranslationUnit> Parser::ParseTranslationUnit()
  {
    auto node = std::make_shared<TranslationUnit>(Lex.CurrentToken);
    while (!Lex.Match(TokenKind::Eof)) {
      node->ExtDecl.push_back(ParseExternalDeclaration());
    }
    return node;
  }

 /**
   declaration:
        declaration-specifiers init-declarator-list? ;

   function-definition:
        declaration-specifiers declarator declaration-list? compound-statement
 */
  std::shared_ptr<DeclBase> Parser::ParseExternalDeclaration()
  {
    auto decl = ParseCommonHeader();

    auto OnlyOneDec = [&decl]()->bool {
      return decl->DecList.size() == 1 && decl->DecList.front()->Init == nullptr;
    };

    if (OnlyOneDec())
    {
      std::string_view funcName;
      bool isFunc = IsFunctionDeclaration(decl->DecList.front()->Dec, funcName);
      if(isFunc) {

        /// function declaration
        /// void (*a())(int);
        if (Lex.Match(TokenKind::Semicolon)) {
          decl->IsLocalDecl = false;
          Lex.SkipToken(TokenKind::Semicolon);
          return decl;
        }

        /// function definition
        /// void (*a())(int) {}
        auto funcNode = std::make_shared<Function>(Lex.CurrentToken);
        funcNode->FuncName = funcName;
        funcNode->Spec = decl->Spec;
        funcNode->Dec = decl->DecList.front()->Dec;
        funcNode->BlockStmt = ParseBlockStmt();
        return funcNode;
      }
    }

    /// global declaration
    Lex.ExpectToken(TokenKind::Semicolon);
    decl->IsLocalDecl = false;
    return decl;
  }

    /**
     /// common header ?
     declaration-specifiers init-declarator-list?

     init-declarator-list:
        init-declarator
        init-declarator-list , init-declarator
    */
  std::shared_ptr<Declaration> Parser::ParseCommonHeader() {
    auto node = std::make_shared<Declaration>(Lex.CurrentToken);
    node->Spec = ParseDeclSpecifier();
    if (!Lex.Match(TokenKind::Semicolon)) {
      node->DecList.push_back(ParseInitDeclarator());
      while (Lex.CurrentToken->Kind == TokenKind::Comma) {
        Lex.SkipToken(TokenKind::Comma);
        node->DecList.push_back(ParseInitDeclarator());
      }
    }
    return node;
  }

  /**
      declaration:
        declaration-specifiers init-declarator-list? ;
   */
  std::shared_ptr<Declaration> Parser::ParseDeclaration()
  {
    auto node = ParseCommonHeader();
    Lex.ExpectToken(TokenKind::Semicolon);
    return node;
  }

  /**
     storage-class-specifier:
         typedef extern static auto register

     type-specifier:
         void char short int long
         float double signed unsigned _Bool
         _Complex
         struct-or-union-specifier
         enum-specifier
         typedef-name

     type-qualifier:
         const restrict volatile

     declaration-specifiers:
        storage-class-specifier declaration-specifiers?
        type-specifier declaration-specifiers?
        type-qualifier declaration-specifiers?
        function-specifier declaration-specifiers?
   */

  std::shared_ptr<DeclSpecifier> Parser::ParseDeclSpecifier()
  {
    auto node = std::make_shared<DeclSpecifier>(Lex.CurrentToken);
    while (true) {
      switch (Lex.CurrentToken->Kind) {
      case TokenKind::Auto:
      case TokenKind::Static:
      case TokenKind::Extern:
      case TokenKind::Typedef:
      case TokenKind::Register: {
        auto astTok = std::make_shared<DeclSpecToken>(Lex.CurrentToken);
        node->StorageClsList.push_back(astTok);
        Lex.GetNextToken();
        break;
      }
      case TokenKind::Const:
      case TokenKind::Volatile:
      case TokenKind::Restrict: {
        auto astTok = std::make_shared<DeclSpecToken>(Lex.CurrentToken);
        node->TyQualifyList.push_back(astTok);
        Lex.GetNextToken();
        break;
      }
      case TokenKind::Void:
      case TokenKind::Bool:
      case TokenKind::Char:
      case TokenKind::Short:
      case TokenKind::Int:
      case TokenKind::Long:
      case TokenKind::Float:
      case TokenKind::Double:
      case TokenKind::Signed:
      case TokenKind::UnSigned:{
        auto astTok = std::make_shared<DeclSpecToken>(Lex.CurrentToken);
        node->TySpecList.push_back(astTok);
        Lex.GetNextToken();
        break;
      }
      case TokenKind::Struct:
      case TokenKind::Union: {
        node->TySpecList.push_back(ParseStructOrUnionSpecifier());
        break;
      }
      case TokenKind::Enum: {
        node->TySpecList.push_back(ParseEnumSpecifier());
        break;
      }
      default:
        return node;
      }
    }
  }

  /**
     init-declarator:
        declarator
        declarator = initializer
   */
  std::shared_ptr<InitDeclarator> Parser::ParseInitDeclarator() {
    auto node = std::make_shared<InitDeclarator>(Lex.CurrentToken);
    node->Dec = ParseDeclarator();
    node->Id = node->Dec->Id;
    if (Lex.Match(TokenKind::Assign)) {
      Lex.SkipToken(TokenKind::Assign);
      node->Init = ParseInitializer();
    }
    return node;
  }

  /**
      declarator:
        pointer? direct-declarator


      type-qualifier-list:
          type-qualifier
          type-qualifier-list type-qualifier
   */

  std::shared_ptr<Declarator> Parser::ParseDeclarator()
  {
    std::shared_ptr<Declarator> dec = ParsePointerDeclarator();
    if (!dec) {
      return ParseDirectDeclarator();
    }

    auto p = dec;
    while (p && p->Dec) {
      p = p->Dec;
    }
    p->Dec = ParseDirectDeclarator();
    p->Id = p->Dec->Id;

    return dec;
  }

  /**
        pointer:
          * type-qualifier-list?
          * type-qualifier-list? pointer
   */
  std::shared_ptr<PointerDeclarator> Parser::ParsePointerDeclarator()
  {
    if (Lex.CurrentToken->Kind == TokenKind::Star) {
      auto node = std::make_shared<PointerDeclarator>(Lex.CurrentToken);
      Lex.SkipToken(TokenKind::Star);
      node->Dec = ParsePointerDeclarator();
      return node;
    }
    return nullptr;
  }

  /**
   direct-declarator:
        identifier
        ( declarator )
        direct-declarator [ type-qualifier-list? assignment-expression? ]
        direct-declarator [ static type-qualifier-list? assignment-expression ]
        direct-declarator [ type-qualifier-list static assignment-expression ]
        direct-declarator [ type-qualifier-list? * ]
        direct-declarator ( parameter-type-list )
        direct-declarator ( identifier-list? )

   */
  std::shared_ptr<Declarator> Parser::ParseDirectDeclarator()
  {
    std::shared_ptr<Declarator> dec;

    if (Lex.Match(TokenKind::LParen)) {
      Lex.SkipToken(TokenKind::LParen);
      dec = ParseDeclarator();
      Lex.ExpectToken(TokenKind::RParen);
    }else {
      if (!Lex.Match(TokenKind::Id)) {
        ParseDiag(Lex.CurrentToken->Location, "expected a identifier");
      }
      dec = std::make_shared<NameDeclarator>(Lex.CurrentToken);
      dec->Id = Lex.CurrentToken->Content;
      Lex.SkipToken(TokenKind::Id);
    }

    while (true)
    {
      if (Lex.Match(TokenKind::LBracket))
      {
        auto arrayDec = std::make_shared<ArrayDeclarator>(Lex.CurrentToken);
        arrayDec->Dec = dec;
        arrayDec->Id = dec->Id;
        Lex.SkipToken(TokenKind::LBracket);
        if (Lex.CurrentToken->Kind != TokenKind::Num) {
          ParseDiag(Lex.CurrentToken->Location, "expected a num");
        }
//        arrayDec->Expr = ParseConstantExpression();
        arrayDec->ArrayLen = Lex.CurrentToken->Value;
        Lex.SkipToken(TokenKind::Num);
        Lex.ExpectToken(TokenKind::RBracket);
        dec = arrayDec;
      }
      else if (Lex.Match(TokenKind::LParen))
      {
        auto funcDec = std::make_shared<FunctionDeclarator>(Lex.CurrentToken);
        funcDec->Dec = dec;
        funcDec->Id = dec->Id;
        Lex.SkipToken(TokenKind::LParen);
        funcDec->ParamTyLists = ParseParamTypeList();
        Lex.ExpectToken(TokenKind::RParen);
        dec = funcDec;
      }
      else {
        break;
      }
    }
    return dec;
  }

  /**
    parameter-type-list:
        parameter-list
        parameter-list , ...

    parameter-list:
        parameter-declaration
        parameter-list , parameter-declaration
   */
  std::shared_ptr<ParamTypeList> Parser::ParseParamTypeList()
  {
    auto node = std::make_shared<ParamTypeList>(Lex.CurrentToken);
    if (!Lex.Match(TokenKind::RParen)) {
      node->ParamDecl.push_back(ParseParamDeclaration());
    }
    while (!Lex.Match(TokenKind::RParen)) {
      Lex.ExpectToken(TokenKind::Comma);
      if (Lex.CurrentToken->Kind == TokenKind::Ellipsis) {
        node->HaveEllipsis = true;
        break;
      }
      node->ParamDecl.push_back(ParseParamDeclaration());
    }
    return node;
  }

  /**
    parameter-declaration:
        declaration-specifiers declarator
        declaration-specifiers abstract-declarator?
   */
  std::shared_ptr<ParamDeclaration> Parser::ParseParamDeclaration() {
    auto node = std::make_shared<ParamDeclaration>(Lex.CurrentToken);
    node->Spec = ParseDeclSpecifier();
    node->Dec = ParseDeclarator();
    return node;
  }

  /**
    initializer:
        assignment-expression
        { initializer-list }
        { initializer-list , }

    initializer-list:
        designation? initializer
        initializer-list , designation? initializer

    designation:
        designator-list =

    designator-list:
        designator
        designator-list designator

    designator:
        [ constant-expression ]
        . identifier
   */
  std::shared_ptr<Initializer> Parser::ParseInitializer() {
    auto node = std::make_shared<Initializer>(Lex.CurrentToken);
    /// 开头视野
    if (Lex.Match(TokenKind::LBrace)) {
      node->LBrace = true;
      Lex.SkipToken(TokenKind::LBrace);
      node->Initials = ParseInitializer();
      auto tail = node->Initials;
      /// 内部视野
      while (Lex.Match(TokenKind::Comma)) {
        Lex.SkipToken(TokenKind::Comma);
        if (Lex.Match(TokenKind::RBrace)) {
          break;
        }
        tail->Next = ParseInitializer();
        tail = tail->Next;
      }
      Lex.ExpectToken(TokenKind::RBrace);
    }else {
      node->LBrace = false;
      node->Expr = ParseAssignExpr();
    }
    return node;
  }


  /**
    struct-or-union-specifier:
        struct-or-union identifier? { struct-declaration-list }
        struct-or-union identifier

    struct-or-union:
        struct union

    struct-declaration-list:
        struct-declaration
        struct-declaration-list struct-declaration
   */
  std::shared_ptr<StructSpecifier> Parser::ParseStructOrUnionSpecifier()
  {
    DeclClass cls = DeclClass::StructSpecifier;
    if (Lex.Match(TokenKind::Union)) {
      cls = DeclClass::UnionSpecifier;
    }
    auto node = std::make_shared<StructSpecifier>(Lex.CurrentToken);
    node->Cls = cls;
    Lex.GetNextToken();

    if (Lex.Match(TokenKind::Id)) {
      node->Id = Lex.CurrentToken->Content;
      Lex.SkipToken(TokenKind::Id);
    }

    if (!Lex.Match(TokenKind::LBrace)) {
      return node;
    }

    node->HasLBrace = true;
    Lex.ExpectToken(TokenKind::LBrace);
    while (!Lex.Match(TokenKind::RBrace)) {
      node->DeclList.push_back(ParseStructDeclaration());
    }
    Lex.ExpectToken(TokenKind::RBrace);
    return node;
  }

  /**
   struct-declaration:
        specifier-qualifier-list struct-declarator-list ;

   specifier-qualifier-list:
        type-specifier specifier-qualifier-list?
        type-qualifier specifier-qualifier-list?

   struct-declarator-list:
        struct-declarator
        struct-declarator-list , struct-declarator
   */
  std::shared_ptr<StructDeclaration> Parser::ParseStructDeclaration()
  {
    auto node = std::make_shared<StructDeclaration>(Lex.CurrentToken);
    node->Spec = ParseDeclSpecifier();
    node->DecList.push_back(ParseStructDeclarator());
    while (Lex.Match(TokenKind::Comma)) {
      Lex.SkipToken(TokenKind::Comma);
      node->DecList.push_back(ParseStructDeclarator());
    }
    Lex.ExpectToken(TokenKind::Semicolon);
    return node;
  }

  /**
   struct-declarator:
        declarator
        declarator? : constant-expression
   */
  std::shared_ptr<StructDeclarator> Parser::ParseStructDeclarator() {
    auto node = std::make_shared<StructDeclarator>(Lex.CurrentToken);
    node->Dec = ParseDeclarator();
    return node;
  }

  /**
   enum-specifier:
       enum identifier? { enumerator-list }
       enum identifier? { enumerator-list , }
       enum identifier

   enumerator-list:
       enumerator
       enumerator-list , enumerator
   */
  std::shared_ptr<EnumSpecifier> Parser::ParseEnumSpecifier()
  {
    auto node = std::make_shared<EnumSpecifier>(Lex.CurrentToken);
    Lex.SkipToken(TokenKind::Enum);

    if (Lex.Match(TokenKind::Id)) {
      Lex.SkipToken(TokenKind::Id);
      node->Id = Lex.CurrentToken->Content;
    }
    if (!Lex.Match(TokenKind::LBrace)) {
      return node;
    }
    Lex.ExpectToken(TokenKind::LBrace);
    node->DecList.push_back(ParseEnumDeclarator());
    while (Lex.Match(TokenKind::Comma)) {
      Lex.SkipToken(TokenKind::Comma);
      if (Lex.Match(TokenKind::RBrace)) {
        break;
      }
      node->DecList.push_back(ParseEnumDeclarator());
    }
    Lex.ExpectToken(TokenKind::RBrace);
    return node;
  }

  std::shared_ptr<EnumDeclarator> Parser::ParseEnumDeclarator() {
    auto node = std::make_shared<EnumDeclarator>(Lex.CurrentToken);
    if (!Lex.Match(TokenKind::Id)) {
      ParseDiag(Lex.CurrentToken->Location, "expected a identifier");
    }
    node->Id = Lex.CurrentToken->Content;
    return node;
  }

  void DeclSpecToken::Accept(AstVisitor *visitor) {
    visitor->VisitorDeclSpecTokenNode(this);
  }

  void InitDeclarator::Accept(AstVisitor *visitor) {
    visitor->VisitorInitDeclaratorNode(this);
  }

  void DeclSpecifier::Accept(AstVisitor *visitor) {
    visitor->VisitorDeclSpecifierNode(this);
  }

  void Initializer::Accept(AstVisitor *visitor) {
    visitor->VisitorInitializerNode(this);
  }

  void ParamTypeList::Accept(AstVisitor *visitor) {
    visitor->VisitorParamTypeListNode(this);
  }

  void ParamDeclaration::Accept(AstVisitor *visitor) {
    visitor->VisitorParamDeclarationNode(this);
  }

  void NameDeclarator::Accept(AstVisitor *visitor) {
    visitor->VisitorNameDeclaratorNode(this);
  }

  void PointerDeclarator::Accept(AstVisitor *visitor) {
    visitor->VisitorPointerDeclaratorNode(this);
  }

  void ArrayDeclarator::Accept(AstVisitor *visitor) {
    visitor->VisitorArrayDeclaratorNode(this);
  }

  void FunctionDeclarator::Accept(AstVisitor *visitor) {
    visitor->VisitorFuncDeclaratorNode(this);
  }

  void StructSpecifier::Accept(AstVisitor *visitor) {
    visitor->VisitorStructSpecifierNode(this);
  }

  void StructDeclaration::Accept(AstVisitor *visitor) {
    visitor->VisitorStructDeclarationNode(this);
  }

  void StructDeclarator::Accept(AstVisitor *visitor) {
    visitor->VisitorStructDeclaratorNode(this);
  }

  void EnumSpecifier::Accept(AstVisitor *visitor) {
    visitor->VisitorEnumSpecifierNode(this);
  }

  void EnumDeclarator::Accept(AstVisitor *visitor) {
    visitor->VisitorEnumDeclaratorNode(this);
  }

  void Declaration::Accept(AstVisitor *visitor) {
    visitor->VisitorDeclarationNode(this);
  }

  void Function::Accept(AstVisitor *visitor) {
    visitor->VisitorFunctionNode(this);
  }

  void TranslationUnit::Accept(AstVisitor *visitor) {
    visitor->VisitorTranslationUnitNode(this);
  }
}

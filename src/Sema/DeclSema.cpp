/**********************************
* File:     DeclSema.cpp.cc
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2022/1/31
***********************************/

#include "Sema.h"
#include <string>

namespace CCC {

   /**
     translation-unit:
        external-declaration
        translation-unit external-declaration

      external-declaration:
        function-definition
        declaration
   */
  void Sema::VisitorTranslationUnitNode(TranslationUnit *node)
  {
    SymTable.EnterScope();
    for (auto &extDec : node->ExtDecl) {
      extDec->Accept(this);
    }
    SymTable.ExitScope();
  }

  /**
   function-definition:
        declaration-specifiers declarator declaration-list? compound-statement
  */
  void Sema::VisitorFunctionNode(Function *node)
  {
    node->Spec->Accept(this);

    node->Dec->BaseTy = node->Spec->Ty;
    node->Dec->Accept(this);
    node->Ty = node->Dec->Ty;

    std::shared_ptr<Symbol> fSym = SymTable.FindVar(node->FuncName);
    if (fSym == nullptr) {
        node->FSym = SymTable.AddFunction(node->FuncName, node->Ty, node->Tok->Location);
    }
    else if (!fSym->Ty->IsFuncTy()) {
        SemaDiag(node->Tok->Location, "Redeclaration as a function");
    }
    else {
      if (node->FSym->Defined) {
        SemaDiag(node->Tok->Location, "Function redefinition");
      }
    }
    node->FSym->Defined = true;

    CurrentFunc = node;

    SymTable.EnterScope();

    auto funTy = std::dynamic_pointer_cast<FunctionType>(node->Ty);
    for (auto &param : funTy->Params) {
      auto sym = SymTable.AddVariable(param->Tok->Content, param->Ty, param->Tok->Location);
      node->FSym->Params.push_back(sym);
      node->FSym->Locals.push_back(sym);
    }

//    SymTable.ExitScope();
    node->BlockStmt->IsFuncFirstBlockStmt = true;
    node->BlockStmt->Accept(this);

    CurrentFunc = nullptr;
  }

  void Sema::VisitorDeclarationNode(Declaration *node) {
    if (node->IsLocalDecl) {
      CheckLocalDeclaration(node);
    }else {
      CheckGlobalDeclaration(node);
    }
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
  void Sema::VisitorDeclSpecifierNode(DeclSpecifier *node)
  {
    CheckStorageSpecifier(node);
    CheckTypeSpecifier(node);
  }

  void Sema::VisitorNameDeclaratorNode(NameDeclarator *node) {
    auto ty = node->BaseTy;
    if (node->Dec != nullptr) {
      node->Dec->BaseTy = ty;
      node->Dec->Accept(this);
      node->Ty = node->Dec->Ty;
      node->Id = node->Dec->Id;
    }else {
      node->Ty = ty;
    }
  }

  void Sema::VisitorFuncDeclaratorNode(FunctionDeclarator *node) {
    auto ty = std::make_shared<FunctionType>(node->BaseTy);
    node->ParamTyLists->FunTy = ty;
    node->ParamTyLists->Accept(this);

    if (node->Dec != nullptr) {
      node->Dec->BaseTy = ty;
      node->Dec->Accept(this);
      node->Ty = node->Dec->Ty;
      node->Id = node->Dec->Id;
    }else {
      node->Ty = ty;
    }
  }

  void Sema::VisitorArrayDeclaratorNode(ArrayDeclarator *node)
  {
    auto ty = std::make_shared<ArrayType>(node->BaseTy, node->ArrayLen);
    if (node->Dec) {
      node->Dec->BaseTy = ty;
      node->Dec->Accept(this);
      node->Ty = node->Dec->Ty;
      node->Id = node->Dec->Id;
    }else {
      node->Ty = ty;
    }
  }

  void Sema::VisitorPointerDeclaratorNode(PointerDeclarator *node) {
    auto ty = std::make_shared<PointerType>(node->BaseTy);
    if (node->Dec) {
      node->Dec->BaseTy = ty;
      node->Dec->Accept(this);
      node->Ty = node->Dec->Ty;
      node->Id = node->Dec->Id;
    }else {
      node->Ty = ty;
    }
  }

  void Sema::VisitorInitDeclaratorNode(InitDeclarator *node) {
    node->Dec->BaseTy = node->BaseTy;
    node->Dec->Accept(this);
    node->Ty = node->Dec->Ty;
    node->Id = node->Dec->Id;

    if (node->Init) {
      node->Init->Ty = node->Ty;
      node->Init->Accept(this);
    }
  }

  void Sema::VisitorInitializerNode(Initializer *node) {
    CheckInitializerInternal(node);
  }

  /**
  parameter-type-list:
      parameter-list
      parameter-list , ...

  parameter-list:
      parameter-declaration
      parameter-list , parameter-declaration
  */
  void Sema::VisitorParamTypeListNode(ParamTypeList *node) {
    for (auto &p : node->ParamDecl) {
      p->FunTy = node->FunTy;
      p->Accept(this);
    }
  }

  /**
  parameter-declaration:
      declaration-specifiers declarator
      declaration-specifiers abstract-declarator?
  */
  void Sema::VisitorParamDeclarationNode(ParamDeclaration *node) {
    node->Spec->Accept(this);
    node->Dec->BaseTy = node->Spec->Ty;
    node->Dec->Accept(this);
    node->Ty = node->Dec->Ty;

    auto param = std::make_shared<Param>();
    param->Ty = node->Ty;
    param->Ty = param->Ty->AdjustParameter(param->Ty);
    param->Tok = node->Dec->Tok;
    node->FunTy->Params.push_back(param);
  }

  void Sema::VisitorStructSpecifierNode(StructSpecifier *node) {

    std::shared_ptr<Symbol> tag;
    std::shared_ptr<RecordType> ty;
    TypeClass cls = node->Cls == DeclClass::StructSpecifier ? TypeClass::Struct : TypeClass::Union;
    if (!node->Id.empty() && !node->HasLBrace) {
      /// struct-or-union id
      tag = SymTable.FindTag(node->Id);
      if (tag == nullptr) {
        ty = std::make_shared<RecordType>(cls);
        tag = SymTable.AddTag(node->Id, ty, node->Tok->Location);
      }else {
        ty = std::dynamic_pointer_cast<RecordType>(tag->Ty);
        if (ty->TyCls != cls) {
          SemaDiag(node->Tok->Location, "Inconsistent tag declaration.");
        }
      }
    }else if (node->Id.empty() && node->HasLBrace) {
      /// struct-or-union {struct-declaration-list}
      ty = std::make_shared<RecordType>(cls);
      ty->Complete = true;
    }else if (!node->Id.empty() && node->HasLBrace) {
      /// struct-or-union id {struct-declaration-list}
      tag = SymTable.FindTag(node->Id);
      if (tag == nullptr) {
        ty = std::make_shared<RecordType>(cls);
        ty->Complete = true;
        tag = SymTable.AddTag(node->Id, ty, node->Tok->Location);
      }else {
        if (tag->Level < SymTable.Level) {
          ty = std::make_shared<RecordType>(cls);
          ty->Complete = true;
          tag = SymTable.AddTag(node->Id, ty, node->Tok->Location);
        }else {
          ty = std::dynamic_pointer_cast<RecordType>(tag->Ty);
          if (ty->TyCls != cls) {
            SemaDiag(node->Tok->Location, "use of '%s' with tag type that does not match previous declaration",
                     tag->Name.data());
          }
          if (ty->Complete) {
            SemaDiag(node->Tok->Location, "redefinition");
          }
          ty->Complete = true;
        }
      }
    }
    for (auto &decl : node->DeclList) {
      decl->RecordTy = ty;
      decl->Accept(this);
    }

    ty->FinishLayout(0);

    node->Ty = ty;
  }

  void Sema::VisitorStructDeclarationNode(StructDeclaration *node)
  {
    node->Spec->Accept(this);

    if (node->DecList.size() == 0)
    {
      if(node->Spec->Ty->IsRecordTy()) {
        /**
          struct A{
            struct{
              int b;
                  ^  here
            };
          };
        */
        if (node->Spec->TagName.empty()) {
          node->RecordTy->AddField(std::make_shared<Field>(node->Spec->Ty, 0));
          return;
        }
      }
        /**
          struct A{
            struct C{
              int b;
                  ^  unable to use, ignore, warning
            };
          };

          struct A {int;}
                      ^  unable to use, ignore, warning
        */
      return;
    }

    /**
    * struct D {
    *  struct Data {int a; int b;} s1, s2;
    *                              ^    ^
    * }
    */
    for (auto &dec : node->DecList) {
      dec->RecordTy = node->RecordTy;
      dec->BaseTy = node->Spec->Ty;
      dec->Accept(this);
      dec->Ty = dec->Dec->Ty;
    }
  }

  /**
  struct-declarator:
      declarator
      declarator? : constant-expression
  */
  void Sema::VisitorStructDeclaratorNode(StructDeclarator *node)
  {
    if (node->Dec != nullptr)
    {
      node->Dec->BaseTy = node->BaseTy;
      node->Dec->Accept(this);
      node->Ty = node->Dec->Ty;
      node->Id = node->Dec->Id;
    }else {
      node->Ty = node->BaseTy;
    }

    if (!node->Id.empty() &&  node->RecordTy->FindField(node->Id) != nullptr)
    {
      SemaDiag(node->Tok->Location, "member redefinition");
    }
    node->RecordTy->AddField(std::make_shared<Field>(node->Id, node->Ty, 0));
  }

  void Sema::VisitorEnumSpecifierNode(EnumSpecifier *node) {

  }

  void Sema::VisitorEnumDeclaratorNode(EnumDeclarator *node) {

  }

  void Sema::VisitorDeclSpecTokenNode(DeclSpecToken *node) {
    /// nop
  }

  void Sema::CheckLocalDeclaration(Declaration *decl)
  {
    decl->Spec->Accept(this);

    for (auto &initDec : decl->DecList)
    {
      initDec->BaseTy = decl->Spec->Ty;
      initDec->Accept(this);
      initDec->Ty = initDec->Dec->Ty;

      std::shared_ptr<Symbol> varSym = SymTable.FindVar(initDec->Id);
      if (varSym == nullptr || varSym->Level < SymTable.Level)
      {
        auto sym = SymTable.AddVariable(initDec->Id, initDec->Ty, initDec->Tok->Location);
        if (initDec->Init) {
          sym->Init = initDec->Init->IData;
        }
        initDec->Sym = sym;
        CurrentFunc->FSym->Locals.push_back(sym);
      }else
      {
        if(initDec->Ty->TyCls == varSym->Ty->TyCls){
          SemaDiag(initDec->Tok->Location,"redefinition of \'%s\' ",initDec->Id.data());
        }else{
          SemaDiag(initDec->Tok->Location,"redefinition of \'%s\' as different kind of symbol", initDec->Id.data());
        }
      }
    }
  }

  void Sema::CheckGlobalDeclaration(Declaration *decl)
  {
    decl->Spec->Accept(this);

    for (auto &initDec : decl->DecList) {
      initDec->BaseTy = decl->Spec->Ty;
      initDec->Accept(this);

      if (initDec->Ty->IsFuncTy())
      {
        if (initDec->Init)
          SemaDiag(initDec->Tok->Location, "please don't initialize function");

        std::shared_ptr<Symbol> sym = SymTable.FindVar(initDec->Id);
        if (sym == nullptr)
          sym = SymTable.AddFunction(initDec->Id, initDec->Ty, initDec->Tok->Location);
      }
      else
      {
        /**
            Check for global variables
        */
        std::shared_ptr<Symbol> sym = SymTable.FindVar(initDec->Id);
        if (sym == nullptr)
          sym = SymTable.AddVariable(initDec->Id, initDec->Ty, initDec->Tok->Location);

        if (initDec->Init)
        {
          if (sym->Defined){
            SemaDiag(initDec->Tok->Location, "redefinition of %s", initDec->Id.data());
          }
          else{
            sym->Init = initDec->Init->IData;
            sym->Defined = true;
          }
        }
      }
    }
  }

  void Sema::CheckInitializerInternal(Initializer* node)
  {
    if (node->Ty->IsScalarType())
    {
      if(node->LBrace){
        SemaDiag(node->Tok->Location,"braces around scalar initializer");
      }
      node->Expr->Accept(this);

      auto iData = std::make_shared<InitData>();
      iData->Expr = node->Expr;
      iData->Next = nullptr;
      node->IData = iData;
      return;
    }

    SemaDiag(node->Tok->Location, "not support yet");
  }
}

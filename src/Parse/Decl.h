/**********************************
 * File:     Decl.h
 *
 * Author:   caipeng
 *
 * Email:    iiicp@outlook.com
 *
 * Date:     2022/1/30
 ***********************************/

#ifndef CCC_SRC_DECL_H
#define CCC_SRC_DECL_H

#include "AstVisitor.h"
#include "Symbol.h"

namespace CCC {

  enum class DeclClass {
    TranslationUnit,
    Function,
    Declaration,
    DeclSpecifier,
    StructSpecifier,
    UnionSpecifier,
    StructDeclaration,
    StructDeclarator,
    EnumSpecifier,
    EnumDeclarator,
    DeclSpecToken,
    PointerDeclarator,
    NameDeclarator,
    FunctionDeclarator,
    ArrayDeclarator,
    InitDeclarator,
    Initializer,
    ParamTypeList,
    ParamDeclaration
  };

  class DeclBase : public AstNode
  {
  public:
    virtual ~DeclBase() {}
    DeclClass Cls;
    std::shared_ptr<Type> Ty;
    DeclBase(std::shared_ptr<Token> tok) : AstNode(tok) {}
    virtual void Accept(AstVisitor *visitor) = 0;
  };

  class DeclSpecToken : public DeclBase
  {
  public:
    DeclSpecToken(std::shared_ptr<Token> tok) : DeclBase(tok) {
      Cls = DeclClass::DeclSpecToken;
    }
    void Accept(AstVisitor *visitor) override;
  };

  class DeclSpecifier : public DeclBase
  {
  public:
    std::list<std::shared_ptr<DeclSpecToken>> StorageClsList;
    std::list<std::shared_ptr<DeclSpecToken>> TyQualifyList;
    std::list<std::shared_ptr<DeclBase>> TySpecList;
    DeclSpecifier(std::shared_ptr<Token> tok) : DeclBase(tok) {
      Cls = DeclClass::DeclSpecifier;
    }
    void Accept(AstVisitor *visitor) override;

  public:
    /// sema
    StorageClass SClass;
    TypeQualifier TyQualify;
    std::string_view TagName;
  };

  class Declarator : public DeclBase
  {
  public:
    std::string_view Id{};
    std::shared_ptr<Declarator> Dec{nullptr};
    Declarator(std::shared_ptr<Token> tok) : DeclBase(tok) {}
    virtual void Accept(AstVisitor *visitor) = 0;

  public:
    std::shared_ptr<Type> BaseTy;
  };

  class PointerDeclarator : public Declarator
  {
  public:
    PointerDeclarator(std::shared_ptr<Token> tok) : Declarator(tok) {
      Cls = DeclClass::PointerDeclarator;
    }
    void Accept(AstVisitor *visitor) override;
  };

  class NameDeclarator : public Declarator
  {
  public:
    NameDeclarator(std::shared_ptr<Token> tok) : Declarator(tok) {
      Cls = DeclClass::NameDeclarator;
    }
    void Accept(AstVisitor *visitor) override;
  };

  class FunctionDeclarator : public Declarator
  {
  public:
    std::shared_ptr<ParamTypeList> ParamTyLists;
    FunctionDeclarator(std::shared_ptr<Token> tok) : Declarator(tok) {
      Cls = DeclClass::FunctionDeclarator;
    }
    void Accept(AstVisitor *visitor) override;
  };

  class ArrayDeclarator : public Declarator
  {
  public:
//    std::shared_ptr<ExprNode> Expr;
    int ArrayLen{0};
    ArrayDeclarator(std::shared_ptr<Token> tok) : Declarator(tok) {
      Cls = DeclClass::ArrayDeclarator;
    }
    void Accept(AstVisitor *visitor) override;
  };

  class Initializer : public DeclBase
  {
  public:
    bool LBrace{false};
    std::shared_ptr<Initializer> Initials{nullptr};
    std::shared_ptr<ExprNode> Expr{nullptr};
    std::shared_ptr<Initializer> Next{nullptr};
    Initializer(std::shared_ptr<Token> tok) : DeclBase(tok) {
      Cls = DeclClass::Initializer;
    }
    void Accept(AstVisitor *visitor) override;
  public:
    ///sema
    std::shared_ptr<InitData> IData;
  };

  class InitDeclarator : public DeclBase
  {
  public:
    std::shared_ptr<Declarator> Dec;
    std::shared_ptr<Initializer> Init{nullptr};
    InitDeclarator(std::shared_ptr<Token> tok) : DeclBase(tok) {
      Cls = DeclClass::InitDeclarator;
    }
    void Accept(AstVisitor *visitor) override;

  public:
    /// sema
    std::shared_ptr<Type> BaseTy;
    std::string_view Id;
    std::shared_ptr<Symbol> Sym;
  };


  /**
    struct-declarator:
        declarator
        declarator? : constant-expression
   */
  class StructDeclarator : public DeclBase
  {
  public:
    std::shared_ptr<Declarator> Dec;
    StructDeclarator(std::shared_ptr<Token> tok) : DeclBase(tok) {
      Cls = DeclClass::StructDeclarator;
    }
    void Accept(AstVisitor *visitor) override;

  public:
    /// sema
    std::shared_ptr<RecordType> RecordTy;
    std::shared_ptr<Type> BaseTy;
    std::string_view Id;
  };

  class StructDeclaration : public DeclBase
  {
  public:
    std::shared_ptr<DeclSpecifier> Spec;
    std::list<std::shared_ptr<StructDeclarator>> DecList;
    StructDeclaration(std::shared_ptr<Token> tok) : DeclBase(tok) {
      Cls = DeclClass::StructDeclaration;
    }
    void Accept(AstVisitor *visitor) override;

  public:
    /// sema
    std::shared_ptr<RecordType> RecordTy;
  };

  class StructSpecifier : public DeclBase
  {
  public:
    std::string_view Id;
    bool HasLBrace{false};
    std::list<std::shared_ptr<StructDeclaration>> DeclList;
    StructSpecifier(std::shared_ptr<Token> tok) : DeclBase(tok) {
    }
    void Accept(AstVisitor *visitor) override;
  };

  class EnumDeclarator : public DeclBase
  {
  public:
    std::string_view Id;
    std::shared_ptr<ExprNode> Expr;
    EnumDeclarator(std::shared_ptr<Token> tok) : DeclBase(tok) {
      Cls = DeclClass::EnumDeclarator;
    }
    void Accept(AstVisitor *visitor) override;
  };

  class EnumSpecifier : public DeclBase
  {
  public:
    std::string_view Id;
    std::list<std::shared_ptr<EnumDeclarator>> DecList;
    EnumSpecifier(std::shared_ptr<Token> tok) : DeclBase(tok) {
      Cls = DeclClass::EnumSpecifier;
    }
    void Accept(AstVisitor *visitor) override;
  };

  class Declaration : public DeclBase
  {
  public:
    bool IsLocalDecl{true};
    std::shared_ptr<DeclSpecifier> Spec;
    std::list<std::shared_ptr<InitDeclarator>> DecList;
    Declaration(std::shared_ptr<Token> tok) : DeclBase(tok) {
      Cls = DeclClass::Declaration;
    }
    void Accept(AstVisitor *visitor) override;
  };

  class ParamDeclaration : public DeclBase
  {
  public:
    std::shared_ptr<DeclSpecifier> Spec;
    std::shared_ptr<Declarator> Dec;
    ParamDeclaration(std::shared_ptr<Token> tok) : DeclBase(tok) {
      Cls = DeclClass::ParamDeclaration;
    }
    void Accept(AstVisitor *visitor) override;

  public:
    /// sema
    std::shared_ptr<FunctionType> FunTy;
  };

  class ParamTypeList : public DeclBase
  {
  public:
    bool HaveEllipsis{false};
    std::list<std::shared_ptr<ParamDeclaration>> ParamDecl;
    ParamTypeList(std::shared_ptr<Token> tok) : DeclBase(tok) {
      Cls = DeclClass::ParamTypeList;
    }
    void Accept(AstVisitor *visitor) override;

  public:
    /// sema
    std::shared_ptr<FunctionType> FunTy;
  };

  class Symbol;
  class Function : public DeclBase
  {
  public:
    /// parse
    std::string_view FuncName;
    std::shared_ptr<DeclSpecifier> Spec;
    std::shared_ptr<Declarator> Dec;
    std::shared_ptr<BlockStmtNode> BlockStmt;

    Function(std::shared_ptr<Token> tok) : DeclBase(tok) {
      Cls = DeclClass::Function;
    }
    void Accept(AstVisitor *visitor) override;

  public:
    /// sema
    std::shared_ptr<Symbol> FSym;
  };

  class TranslationUnit : public DeclBase
  {
  public:
    std::list<std::shared_ptr<DeclBase>> ExtDecl;
    TranslationUnit(std::shared_ptr<Token> tok) : DeclBase(tok) {
      Cls = DeclClass::TranslationUnit;
    }
    void Accept(AstVisitor *visitor) override;
  };
}

#endif // CCC_SRC_DECL_H

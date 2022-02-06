/**********************************
 * File:     SemanticsVisitor.cpp.cc
 *
 * Author:   caipeng
 *
 * Email:    iiicp@outlook.com
 *
 * Date:     2022/1/28
 ***********************************/

#include "Sema.h"
#include <cassert>

namespace CCC
{
  void Sema::CheckStorageSpecifier(DeclSpecifier *declSpecifier)
  {
    /// storage class specifier
    if (declSpecifier->StorageClsList.size() == 0) {
      declSpecifier->SClass = StorageClass::UnSpecified;
    }else if (declSpecifier->StorageClsList.size() == 1) {
      auto tok = std::dynamic_pointer_cast<DeclSpecToken>(declSpecifier->StorageClsList.front())->Tok;
      switch (tok->Kind) {
      case TokenKind::Auto:
        declSpecifier->SClass = StorageClass::Auto;
        break;
      case TokenKind::Register:
        declSpecifier->SClass = StorageClass::Register;
        break;
      case TokenKind::Static:
        declSpecifier->SClass = StorageClass::Static;
        break;
      case TokenKind::Extern:
        declSpecifier->SClass = StorageClass::Extern;
        break;
      case TokenKind::Typedef:
        declSpecifier->SClass = StorageClass::Typedef;
        break;
      default:
        break;
      }
    }else {
      SemaDiag(declSpecifier->Tok->Location, "At most one storage class");
    }
  }

  void Sema::CheckTypeSpecifier(DeclSpecifier *declSpecifier)
  {
    std::shared_ptr<Type> ty;

    int tyCnt = 0, signCnt = 0, sizeCnt = 0;
    TokenKind size = TokenKind::Eof, sign = TokenKind::Eof;

    for (auto &p : declSpecifier->TySpecList)
    {
      switch (p->Cls)
      {
        case DeclClass::StructSpecifier:
        case DeclClass::UnionSpecifier:
        {
          tyCnt++;
          p->Accept(this);
          ty = p->Ty;
          declSpecifier->TagName = std::dynamic_pointer_cast<StructSpecifier>(p)->Id;
          break;
        }
        case DeclClass::EnumSpecifier:
        {
          tyCnt++;
          p->Accept(this);
          ty = p->Ty;
          declSpecifier->TagName = std::dynamic_pointer_cast<EnumSpecifier>(p)->Id;
          break;
        }
        default:
        {
          auto tok = std::dynamic_pointer_cast<DeclSpecToken>(p)->Tok;
          switch (tok->Kind) {
          case TokenKind::Float:
          case TokenKind::Double: {
            assert(0);
            break;
          }
          case TokenKind::Void: {
            tyCnt++;
            ty = Type::VoidTy;
            break;
          }
          case TokenKind::Bool: {
            tyCnt++;
            ty = Type::BoolTy;
            break;
          }
          case TokenKind::Char: {
            tyCnt++;
            ty = Type::CharTy;
            break;
          }
          case TokenKind::Int: {
            tyCnt++;
            ty = Type::IntTy;
            break;
          }
          case TokenKind::Signed:
          case TokenKind::UnSigned: {
            signCnt++;
            sign = tok->Kind;
            break;
          }
          case TokenKind::Short: {
            sizeCnt++;
            size = tok->Kind;
            break;
          }
          case TokenKind::Long: {
            if (size == TokenKind::Long && sizeCnt == 1) {
              size = TokenKind::LongLong;
            } else {
              sizeCnt++;
              size = TokenKind::Long;
            }
            break;
          }
          default:
            assert(0);
          }
        }
      }
    }

    ty = tyCnt == 0 ? Type::IntTy : ty;

    if (signCnt > 1 || sizeCnt > 1 || tyCnt > 1) {
      goto err;
    }

    if (ty == Type::CharTy) {
      sign = TokenKind::Eof;
    }

    if (ty == Type::IntTy)
    {
      switch (size)
      {
      case TokenKind::Short: {
        ty = Type::ShortTy;
        break;
      }
      case TokenKind::Long: {
        ty = Type::LongTy;
        break;
      }
      case TokenKind::LongLong: {
        ty = Type::LongTy;
        break;
      }
      default:
        assert(size == TokenKind::Eof);
        break;
      }
    }
    else if (sign != TokenKind::Eof || size != TokenKind::Eof) {
      goto err;
    }
    declSpecifier->Ty = ty;
    return;

  err:
    SemaDiag(declSpecifier->Tok->Location, "Illegal type specifier.");
    declSpecifier->Ty = Type::IntTy;
    return;
  }
}

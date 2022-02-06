/***********************************
* File:     Type.h
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2022/1/16
***********************************/

#ifndef CCC_SRC_TYPE_H
#define CCC_SRC_TYPE_H
#include "Lexer.h"
#include <memory>
#include <vector>
#include <list>
#include "AstNode.h"

namespace CCC
{
  enum class TypeClass
  {
    Void, Bool, Char, UChar, Short, UShort,
    Int, UInt, Long, ULong, Float, Double, LongDouble,
    Function, Pointer, Array,
    Struct, Union, Enum
  };

  class BuildInType;
  class Type : public std::enable_shared_from_this<Type>
  {
  public:
    TypeClass TyCls;
    int Size;
    int Align;
    bool Complete{false};
  public:
    static std::shared_ptr<BuildInType>  VoidTy;
    static std::shared_ptr<BuildInType>  BoolTy;
    static std::shared_ptr<BuildInType>  CharTy;
    static std::shared_ptr<BuildInType>  UCharTy;
    static std::shared_ptr<BuildInType>  ShortTy;
    static std::shared_ptr<BuildInType>  UShortTy;
    static std::shared_ptr<BuildInType>  IntTy;
    static std::shared_ptr<BuildInType>  UIntTy;
    static std::shared_ptr<BuildInType>  LongTy;
    static std::shared_ptr<BuildInType>  ULongTy;

  public:
    virtual ~Type() {}
    Type(TypeClass cls, int size, int align) : TyCls(cls), Size(size), Align(align){}

    bool IsIntegerTy() const;

    bool IsUnsignedTy() const;
    bool IsVoidTy() const;
    bool IsRealTy() const;
    bool IsArithTy() const;
    bool IsRecordTy() const;
    bool IsFuncTy() const;
    bool IsArrayTy() const;
    bool IsPointerTy() const;
    bool IsScalarType() const;

    bool IsObjPtr() const;
    bool IsVoidPtr() const;
    bool IsInCompletePtr() const;
    std::shared_ptr<Type> AdjustParameter(std::shared_ptr<Type> ty);
  };

  class BuildInType : public Type
  {
  public:
    BuildInType(TypeClass cls, int size, int align) : Type(cls, size, align) {
      Complete = cls == TypeClass::Void ? false : true;
    }
  };

  struct Param
  {
    std::shared_ptr<Type> Ty;
    std::shared_ptr<Token> Tok;
  };
  class FunctionType : public Type
  {
  private:
    std::shared_ptr<Type> ReturnType;
  public:
    std::list<std::shared_ptr<Param>> Params;
    FunctionType(std::shared_ptr<Type> returnTy)
        : Type(TypeClass::Function, 1, 1), ReturnType(returnTy) {
      Complete = false;
    }
  };

  class PointerType : public Type
  {
  public:
    std::shared_ptr<Type> BaseTy;
    PointerType(std::shared_ptr<Type> baseTy)
        : Type(TypeClass::Pointer, 8, 8), BaseTy(baseTy) {
      Complete = BaseTy->Size == 0 ? false : true;
    }
  };

  class ArrayType : public Type
  {
  public:
    std::shared_ptr<Type> ElementType;
    int ArrayLen;
    ArrayType(std::shared_ptr<Type> elementType, int len)
    : Type(TypeClass::Array, len * elementType->Size, elementType->Align),
    ElementType(elementType), ArrayLen(len){
      if (elementType->TyCls == TypeClass::Void || len == 0) {
        Complete = false;
      }else {
        Complete = true;
      }
    }
  };

  struct Field {
  public:
    std::string_view Id;
    std::shared_ptr<Type> Ty;
    int Offset;
    Field( std::string_view id, std::shared_ptr<Type> ty, int offset)
    :Id(id), Ty(ty), Offset(offset) {}

    Field(std::shared_ptr<Type> ty, int offset)
        :Ty(ty), Offset(offset) {}
  };

  class RecordType : public Type
  {
  public:
    std::list<std::shared_ptr<Field>> FldList{};
    RecordType(TypeClass cls) : Type(cls, 0, 1) {
      Complete = false;
    }
    void AddField(std::shared_ptr<Field> fld);
    std::shared_ptr<Field> FindField(std::string_view id);
    void FinishLayout(int offset);
  };

  class EnumType : public Type
  {
  public:
    std::shared_ptr<Type> BaseTy;
    EnumType() : Type(TypeClass::Enum, 4, 4), BaseTy(Type::IntTy) {}
  };

  int AlignTo(int size, int align);
}

#endif //CCC_SRC_TYPE_H

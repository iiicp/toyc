/***********************************
* File:     Type.cpp.cc
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2022/1/16
***********************************/

#include "Type.h"
#include "Basic/Diag.h"

namespace CCC
{
  std::shared_ptr<BuildInType> Type::VoidTy = std::make_shared<BuildInType>(TypeClass::Void, 0, 0);
  std::shared_ptr<BuildInType> Type::BoolTy = std::make_shared<BuildInType>(TypeClass::Bool, 1, 1);
  std::shared_ptr<BuildInType> Type::CharTy = std::make_shared<BuildInType>(TypeClass::Char, 1, 1);
  std::shared_ptr<BuildInType> Type::UCharTy = std::make_shared<BuildInType>(TypeClass::UChar, 1, 1);
  std::shared_ptr<BuildInType> Type::ShortTy = std::make_shared<BuildInType>(TypeClass::Short, 2, 2);
  std::shared_ptr<BuildInType> Type::UShortTy = std::make_shared<BuildInType>(TypeClass::UShort, 2, 2);
  std::shared_ptr<BuildInType> Type::IntTy = std::make_shared<BuildInType>(TypeClass::Int, 4, 4);
  std::shared_ptr<BuildInType> Type::UIntTy = std::make_shared<BuildInType>(TypeClass::UInt, 4, 4);
  std::shared_ptr<BuildInType> Type::LongTy = std::make_shared<BuildInType>(TypeClass::Long, 8, 8);
  std::shared_ptr<BuildInType> Type::ULongTy = std::make_shared<BuildInType>(TypeClass::ULong, 8, 8);

  bool Type::IsIntegerTy() const {
    return TyCls == TypeClass::Bool
    || TyCls == TypeClass::Char || TyCls == TypeClass::UChar
    || TyCls == TypeClass::Short || TyCls == TypeClass::UShort
    || TyCls == TypeClass::Int || TyCls == TypeClass::UInt
    || TyCls == TypeClass::Long || TyCls == TypeClass::ULong
    || TyCls == TypeClass::Enum;
  }

  bool Type::IsUnsignedTy() const {
    return TyCls == TypeClass::UChar || TyCls == TypeClass::UShort
    || TyCls == TypeClass::UInt || TyCls == TypeClass::ULong;
  }

  bool Type::IsVoidTy() const {
    return TyCls == TypeClass::Void;
  }

  bool Type::IsRealTy() const {
    return TyCls == TypeClass::Float || TyCls == TypeClass::Double
    || TyCls == TypeClass::LongDouble;
  }

  bool Type::IsArithTy() const {
    return IsIntegerTy() || IsRealTy();
  }

  bool Type::IsRecordTy() const{
    return TyCls == TypeClass::Struct || TyCls == TypeClass::Union;
  }

  bool Type::IsFuncTy() const{
    return TyCls == TypeClass::Function;
  }

  bool Type::IsArrayTy() const {
    return TyCls == TypeClass::Array;
  }

  bool Type::IsPointerTy() const {
    return TyCls == TypeClass::Pointer;
  }

  bool Type::IsScalarType() const {
    return IsIntegerTy() || IsRealTy() || IsPointerTy();
  }

  bool Type::IsObjPtr() const {
    return IsPointerTy() && !dynamic_cast<const PointerType *>(this)->BaseTy->IsFuncTy();
  }

  bool Type::IsVoidPtr() const {
    return IsPointerTy() && dynamic_cast<const PointerType *>(this)->BaseTy->IsVoidTy();
  }

  bool Type::IsInCompletePtr() const {
    return IsPointerTy() && dynamic_cast<const PointerType *>(this)->BaseTy->Size == 0;
  }

  std::shared_ptr<Type> Type::AdjustParameter(std::shared_ptr<Type> ty) {
    if (ty->TyCls == TypeClass::Array) {
      auto arrTy = std::dynamic_pointer_cast<ArrayType>(ty);
      return std::make_shared<PointerType>(arrTy->ElementType);
    }else if (ty->TyCls == TypeClass::Function) {
      auto funcTy = std::dynamic_pointer_cast<FunctionType>(ty);
      return std::make_shared<FunctionType>(funcTy);
    }
    return ty;
  }

  void RecordType::AddField(std::shared_ptr<Field> fld) {
    FldList.push_back(fld);
  }

  std::shared_ptr<Field> RecordType::FindField(std::string_view id) {
    assert(!id.empty());
    for (auto &fld : FldList) {

      if (fld->Id == id)
        return fld;

      if (fld->Id.empty() && fld->Ty->IsRecordTy()) {
        auto rty = std::dynamic_pointer_cast<RecordType>(fld->Ty);
        std::shared_ptr<Field> res = rty->FindField(id);
        if (res != nullptr)
          return res;
      }
    }
    return nullptr;
  }

  void RecordType::FinishLayout(int offset) {
    if (TyCls == TypeClass::Struct) {
      int initOffset = offset;
      for (auto &fld : FldList) {
        offset = AlignTo(offset, fld->Ty->Align);
        fld->Offset = offset;

        if (fld->Id.empty() && fld->Ty->IsRecordTy()) {
          auto rty = std::dynamic_pointer_cast<RecordType>(fld->Ty);
          rty->FinishLayout(offset);
        }
        offset += fld->Ty->Size;

        if (Align < fld->Ty->Align)
          Align = fld->Ty->Align;
      }
      Size = AlignTo(offset - initOffset, Align);
    }else {
      for (auto &fld : FldList) {
        fld->Offset = offset;
        if (Align < fld->Ty->Align)
          Align = fld->Ty->Align;
        if (Size < fld->Ty->Size)
          Size = fld->Ty->Size;
      }
      Size = AlignTo(Size, Align);
    }
  }

  int AlignTo(int size, int align) {
    return (size + align - 1) / align * align;
  }
}
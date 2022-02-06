/**********************************
* File:     Symbol.h
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2022/1/31
***********************************/

#ifndef CCC_SRC_SEMANTICS_SYMBOL_H
#define CCC_SRC_SEMANTICS_SYMBOL_H

#include <string_view>
#include "Type.h"

namespace CCC
{
  enum class SymbolKind {
    Tag,
    Variable,
    Constant,
    Function
  };

  /**
      For example:
          struct st{
              struct{
                  int a,b;
              };
              int c;
          } st = {{2},3};
      We use a initData list to represent the above initialization:
          (offset:0, expr:2)  --> (offset:8,expr:3)
  */
  class ExprNode;
  struct InitData
  {
    int Offset;
    std::shared_ptr<ExprNode> Expr{nullptr};
    std::shared_ptr<InitData> Next{nullptr};
  };

  class Symbol {
  public:
    SymbolKind Cls;
    std::string_view Name{};
    std::shared_ptr<Type> Ty;
    int Level{0};
    bool Defined{false};
    SourceLocation Loc;

    /// variable symbol
    std::shared_ptr<InitData> Init{nullptr};
    int Offset;

    /// function symbol
    std::list<std::shared_ptr<Symbol>> Params{};
    std::list<std::shared_ptr<Symbol>> Locals{};
    Symbol(SymbolKind cls) : Cls(cls), Ty(nullptr) {}
  };
}

#endif //CCC_SRC_SEMANTICS_SYMBOL_H

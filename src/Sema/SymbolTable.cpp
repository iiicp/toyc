/***********************************
* File:     Scope.cpp.cc
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2022/1/8
***********************************/

#include "SymbolTable.h"

namespace CCC
{
  void SymbolTable::EnterScope() {
    Level++;
    SymTab.push_back(std::make_shared<Env>());
  }

  void SymbolTable::ExitScope() {
    Level--;
    SymTab.pop_back();
  }

  std::shared_ptr<Symbol> SymbolTable::FindVar(std::string_view id)
  {
    for (auto it = SymTab.rbegin(); it != SymTab.rend(); ++it) {
      std::shared_ptr<Env> env = *it;
      if (env->VarEnv.find(id) != env->VarEnv.end()) {
        return env->VarEnv[id];
      }
    }
    return nullptr;
  }

  std::shared_ptr<Symbol> SymbolTable::FindTag(std::string_view id) {
    for (auto it = SymTab.rbegin(); it != SymTab.rend(); ++it) {
      std::shared_ptr<Env> env = *it;
      if (env->TagEnv.find(id) != env->TagEnv.end()) {
        return env->TagEnv[id];
      }
    }
    return nullptr;
  }

  std::shared_ptr<Symbol> SymbolTable::AddTag(std::string_view id, std::shared_ptr<Type> ty, SourceLocation loc) {
    auto sym = std::make_shared<Symbol>(SymbolKind::Tag);
    sym->Name = id;
    sym->Ty = ty;
    sym->Loc = loc;
    auto &env = SymTab.back();
    sym->Level = Level;
    return env->TagEnv[id] = sym;
  }

  std::shared_ptr<Symbol> SymbolTable::AddVariable(std::string_view id, std::shared_ptr<Type> ty, SourceLocation loc) {
    auto varSym = std::make_shared<Symbol>(SymbolKind::Variable);
    varSym->Name = id;
    varSym->Ty = ty;
    varSym->Loc = loc;

    if (Level == 0) {
      GlobalSymList.push_back(varSym);
    }
    varSym->Level = Level;
    auto &env = SymTab.back();
    env->VarEnv[id] = varSym;
    return varSym;
  }

  std::shared_ptr<Symbol> SymbolTable::AddFunction(std::string_view id, std::shared_ptr<Type> ty, SourceLocation loc) {
    auto funSym = std::make_shared<Symbol>(SymbolKind::Function);
    funSym->Name = id;
    funSym->Ty = ty;
    funSym->Loc = loc;

    FuncSymList.push_back(funSym);

    funSym->Level = 0;
    auto &env = SymTab.front();
    env->VarEnv[id] = funSym;
    return funSym;
  }
}
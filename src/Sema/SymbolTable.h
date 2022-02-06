/***********************************
* File:     Scope.h
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2022/1/26
***********************************/

#ifndef CCC_SCOPE_H
#define CCC_SCOPE_H

#include "AstNode.h"
#include <stack>
#include <string_view>
#include <unordered_map>
#include "Symbol.h"

namespace CCC
{
    class SymbolTable
    {
    private:
      class Env
      {
      public:
        std::unordered_map<std::string_view, std::shared_ptr<Symbol>> VarEnv;
        std::unordered_map<std::string_view, std::shared_ptr<Symbol>> TagEnv;
      };
      std::list<std::shared_ptr<Env>> SymTab;
    public:
      int Level{-1};

      std::list<std::shared_ptr<Symbol>> FuncSymList;
      std::list<std::shared_ptr<Symbol>> GlobalSymList;

      void EnterScope();
      void ExitScope();

      std::shared_ptr<Symbol> FindVar(std::string_view id);
      std::shared_ptr<Symbol> FindTag(std::string_view id);
      std::shared_ptr<Symbol> AddTag(std::string_view id, std::shared_ptr<Type> ty, SourceLocation loc);
      std::shared_ptr<Symbol> AddVariable(std::string_view id, std::shared_ptr<Type> ty, SourceLocation loc);
      std::shared_ptr<Symbol> AddFunction(std::string_view id, std::shared_ptr<Type> ty, SourceLocation loc);
    };
}



#endif//OMEGA_SCOPE_H

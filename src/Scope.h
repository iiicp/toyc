/***********************************
* File:     Scope.h
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2022/1/26
***********************************/

#ifndef C100_SCOPE_H
#define C100_SCOPE_H

#include <stack>
#include <unordered_map>
#include <string_view>
#include "AstNode.h"

namespace C100
{
      class Scope
      {
      private:
          class ScopeItem
          {
          public:
              std::unordered_map<std::string_view, std::shared_ptr<Var>> VarScope;
              std::unordered_map<std::string_view, std::shared_ptr<Type>> TagScope;
          };
      private:
          std::list<std::shared_ptr<ScopeItem>> Sp{};
      public:
          void EnterScope();
          void LeaveScope();

          void PushVar(std::string_view name, std::shared_ptr<Var> var);
          std::shared_ptr<Var> FindVar(std::string_view name);
          std::shared_ptr<Var> FindVarInCurrentScope(std::string_view name);

          void PushTag(std::string_view name, std::shared_ptr<Type> tag);
          std::shared_ptr<Type> FindTag(std::string_view name);
          std::shared_ptr<Type> FindTagInCurrentScope(std::string_view name);

          bool CheckScopeDepthZero();
      };
}



#endif//OMEGA_SCOPE_H

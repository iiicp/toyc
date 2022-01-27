/***********************************
* File:     Scope.cpp.cc
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2022/1/8
***********************************/

#include "Scope.h"

using namespace C100;

void Scope::EnterScope()
{
    Sp.push_front(std::make_shared<ScopeItem>());
}

void Scope::LeaveScope()
{
    Sp.pop_front();
}

std::shared_ptr<Var> Scope::FindVar(std::string_view name)
{
    for (auto &m : Sp) {
        if (m->VarScope.find(name) != m->VarScope.end()) {
            return m->VarScope[name];
        }
    }
    return nullptr;
}

std::shared_ptr<Var> Scope::FindVarInCurrentScope(std::string_view name)
{
    auto m = Sp.front();
    if (m->VarScope.find(name) != m->VarScope.end())
        return m->VarScope[name];
    return nullptr;
}

void Scope::PushVar(std::string_view name, std::shared_ptr<Var> var)
{
    auto &m = Sp.front();
    m->VarScope[name] = var;
}

bool Scope::CheckScopeDepthZero() {
    return Sp.size() == 0;
}

void Scope::PushTag(std::string_view name, std::shared_ptr<Type> tag) {
    auto &m = Sp.front();
    m->TagScope[name] = tag;
}

std::shared_ptr<Type> Scope::FindTag(std::string_view name) {
    for (auto &m : Sp)
        if (m->TagScope.find(name) != m->TagScope.end())
            return m->TagScope[name];
    return nullptr;
}

std::shared_ptr<Type> Scope::FindTagInCurrentScope(std::string_view name) {
    auto &m = Sp.front();
    if (m->TagScope.find(name) != m->TagScope.end())
        return m->TagScope[name];
    return nullptr;
}

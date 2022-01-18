/**********************************
 * File:     CodeGen.cpp.cc
 *
 * Author:   caipeng
 *
 * Email:    iiicp@outlook.com
 *
 * Date:     2022/1/3
 ***********************************/

#include "CodeGen.h"
#include <cassert>

using namespace C100;

void CodeGen::VisitorProgramNode(ProgramNode *node)
{
  for (auto &f : node->Funcs)
    f->Accept(this);
}

void CodeGen::VisitorFunctionNode(FunctionNode *node)
{
  printf("\t.text\n");
  CurrentFuncName = node->FuncName;
#ifdef __linux__
  printf("\t.globl %s\n", name.data());
  printf("%s:\n",name.data());
#else
  /// macos
  printf("\t.globl _%s\n",CurrentFuncName.data());
  printf("_%s:\n",CurrentFuncName.data());
#endif

  int stackSize = 0;
  for (auto &V : node->Locals)
  {
    stackSize += 8;
    V->Offset = stackSize * -1;
  }

  stackSize = AlignTo(stackSize, 16);

  printf("\tpush %%rbp\n");
  printf("\tmov %%rsp, %%rbp\n");
  printf("\tsub $%d, %%rsp\n", stackSize);

  int i = 0;
  for (auto &var : node->Params) {
    printf("\tmov %s, %d(%%rbp)\n", Reg64[i++], var->Offset);
  }

  for (auto &S : node->Stmts) {
    S->Accept(this);
    assert(StackLevel == 0);
  }
  printf(".LReturn_%s:\n", CurrentFuncName.data());
  printf("\tmov %%rbp, %%rsp\n");
  printf("\tpop %%rbp\n");
  printf("\tret\n");
}

void CodeGen::VisitorBinaryNode(BinaryNode *node)
{
  node->Rhs->Accept(this);
  Push();
  node->Lhs->Accept(this);
  Pop("%rdi");

  switch (node->BinOp) {
  case BinaryOperator::Add:
    printf("\tadd %%rdi, %%rax\n");
    break;
  case BinaryOperator::Sub:
    printf("\tsub %%rdi, %%rax\n");
    break;
  case BinaryOperator::Mul:
    printf("\timul %%rdi, %%rax\n");
    break;
  case BinaryOperator::Div:
    printf("\tcqo\n");
    printf("\tidiv %%rdi\n");
    break;
  case BinaryOperator::Equal:
  {
    printf("\tcmp %%rdi, %%rax\n");
    printf("\tsete %%al\n");
    printf("\tmovzb %%al, %%rax\n");
  }
    break;
  case BinaryOperator::PipeEqual:
  {
    printf("\tcmp %%rdi, %%rax\n");
    printf("\tsetne %%al\n");
    printf("\tmovzb %%al, %%rax\n");
  }
    break;
  case BinaryOperator::Greater:
  {
    printf("\tcmp %%rdi, %%rax\n");
    printf("\tsetg %%al\n");
    printf("\tmovzb %%al, %%rax\n");
  }
    break;
  case BinaryOperator::GreaterEqual:
  {
    printf("\tcmp %%rdi, %%rax\n");
    printf("\tsetge %%al\n");
    printf("\tmovzb %%al, %%rax\n");
  }
    break;
  case BinaryOperator::Lesser:
  {
    printf("\tcmp %%rdi, %%rax\n");
    printf("\tsetl %%al\n");
    printf("\tmovzb %%al, %%rax\n");
  }
    break;
  case BinaryOperator::LesserEqual:
  {
    printf("\tcmp %%rdi, %%rax\n");
    printf("\tsetle %%al\n");
    printf("\tmovzb %%al, %%rax\n");
  }
    break;
  default:
    assert(0);
    break;
  }
}

void CodeGen::VisitorConstantNode(ConstantNode *node)
{
  printf("\tmov $%d, %%rax\n", node->Value);
}

void CodeGen::Push()
{
  printf("\tpush %%rax\n");
  StackLevel++;
}

void CodeGen::Pop(const char *reg)
{
  printf("\tpop %s\n", reg);
  StackLevel--;
}

void CodeGen::VisitorExprStmtNode(ExprStmtNode *node) {
  if (node->Lhs)
    node->Lhs->Accept(this);
}

void CodeGen::VisitorAssignExprNode(AssignExprNode *node) {

  auto varNode = std::dynamic_pointer_cast<VarExprNode>(node->Lhs);
  assert(varNode != nullptr);
  printf("\tlea %d(%%rbp), %%rax\n", varNode->VarObj->Offset);
  Push();
  node->Rhs->Accept(this);
  Pop("%rdi");
  printf("\tmov %%rax, (%%rdi)\n");
}

void CodeGen::VisitorVarExprNode(VarExprNode *node) {
  printf("\tlea %d(%%rbp), %%rax\n", node->VarObj->Offset);
  printf("\tmov (%%rax), %%rax\n");
}

void CodeGen::VisitorIfStmtNode(IfStmtNode *node) {
    /// 1. 判断条件
    int n = Sequence++;
    node->Cond->Accept(this);
    printf("\tcmp $0, %%rax\n");
    if (node->Else) {
      printf("\tje .L.else_%d\n",n);
    }else {
      printf("\tje .L.end_%d\n", n);
    }

    node->Then->Accept(this);
    printf("\tjmp .L.end_%d\n", n);

    if (node->Else) {
      printf(".L.else_%d:\n", n);
      node->Else->Accept(this);
      printf("\tjmp .L.end_%d\n", n);
    }

    printf(".L.end_%d:\n", n);
}

void CodeGen::VisitorBlockStmtNode(BlockStmtNode *node)
{
  for (auto &s : node->Stmts)
    s->Accept(this);
}

void CodeGen::VisitorWhileStmtNode(WhileStmtNode *node)
{
  int n = Sequence++;
  printf(".L.begin_%d:\n", n);
  node->Cond->Accept(this);
  printf("\tcmp $0, %%rax\n");
  printf("\tje .L.end_%d\n", n);
  node->Then->Accept(this);
  printf("\tjmp .L.begin_%d\n", n);
  printf(".L.end_%d:\n", n);
}

void CodeGen::VisitorDoWhileStmtNode(DoWhileStmtNode *node)
{
  int n = Sequence++;
  printf(".L.begin_%d:\n", n);
  node->Stmt->Accept(this);
  node->Cond->Accept(this);
  printf("\tcmp $0, %%rax\n");
  printf("\tje .L.end_%d\n", n);
  printf("\tjmp .L.begin_%d\n", n);
  printf(".L.end_%d:\n", n);
}

void CodeGen::VisitorForStmtNode(ForStmtNode *node)
{
  int n = Sequence++;
  if (node->Init)
    node->Init->Accept(this);
  printf(".L.begin_%d:\n", n);
  if (node->Cond) {
    node->Cond->Accept(this);
    printf("\tcmp $0, %%rax\n");
    printf("\tje .L.end_%d\n", n);
  }
  node->Stmt->Accept(this);
  if (node->Inc) {
    node->Inc->Accept(this);
  }
  printf("\tjmp .L.begin_%d\n", n);
  printf(".L.end_%d:\n", n);
}

int CodeGen::AlignTo(int size, int align) {
  return (size + align - 1) / align * align;
}

void CodeGen::VisitorFuncCallNode(FuncCallNode *node) {
  for(auto &arg : node->Args) {
    arg->Accept(this);
    Push();
  }
  for(int i = node->Args.size()-1; i >= 0; i--) {
    Pop(Reg64[i]);
  }

  std::string FuncName(node->FuncName);

#ifdef __linux__
  printf("\tcall %s\n",FuncName.data());
#else
  printf("\tcall _%s\n",FuncName.data());
#endif
}

void CodeGen::VisitorReturnStmtNode(ReturnStmtNode *node) {
  node->Lhs->Accept(this);
  printf("\tjmp .LReturn_%s\n",CurrentFuncName.data());
}

void CodeGen::VisitorDeclarationStmtNode(DeclarationStmtNode *node) {
  for (auto &n : node->AssignNodes)
    n->Accept(this);
}


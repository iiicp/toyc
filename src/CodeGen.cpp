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
    printf("\t.text\n");
#ifdef __linux__
    printf("\t.globl prog\n");
    printf("prog:\n");
#else
    /// macos
    printf("\t.globl _prog\n");
    printf("_prog:\n");
#endif

    printf("\tpush %%rbp\n");
    printf("\tmov %%rsp, %%rbp\n");
    printf("\tsub $32, %%rsp\n");

    node->Lhs->Accept(this);
    assert(StackLevel == 0);

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


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

    int stackSize = 0;
    for (auto &V : node->LocalVars)
    {
      stackSize += 8;
      V->Offset = stackSize * -1;
    }

    printf("\tpush %%rbp\n");
    printf("\tmov %%rsp, %%rbp\n");
    printf("\tsub $%d, %%rsp\n", stackSize);

    for (auto &S : node->Stmts) {
      S->Accept(this);
      assert(StackLevel == 0);
    }

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
    printf("\tje .L.end_%d\n", n);

    if (node->Else) {
      printf(".L.else_%d:\n", n);
      node->Else->Accept(this);
    }

    printf(".L.end_%d:\n", n);
}

void CodeGen::VisitorBlockStmtNode(BlockStmtNode *node)
{
  for (auto &s : node->Stmts)
    s->Accept(this);
}


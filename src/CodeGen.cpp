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
#include "Diag.h"
#include "Type.h"

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
  printf("\t.globl %s\n", CurrentFuncName.data());
  printf("%s:\n",CurrentFuncName.data());
#else
  /// macos
  printf("\t.globl _%s\n",CurrentFuncName.data());
  printf("_%s:\n",CurrentFuncName.data());
#endif

  int offset = 0;
  for (auto &var : node->Locals)
  {
    offset += var->Ty->Size;
    offset = AlignTo(offset, var->Ty->Align);
    var->Offset = -offset;
  }

  offset = AlignTo(offset, 16);

  printf("\tpush %%rbp\n");
  printf("\tmov %%rsp, %%rbp\n");
  printf("\tsub $%d, %%rsp\n", offset);

  int i = 0;
  for (auto &var : node->Params) {
    if (var->Ty->Size == 1) {
      printf("\tmov %s, %d(%%rbp)\n", Reg8[i++], var->Offset);
    }else if (var->Ty->Size == 2) {
      printf("\tmov %s, %d(%%rbp)\n", Reg16[i++], var->Offset);
    }else if (var->Ty->Size == 4) {
      printf("\tmov %s, %d(%%rbp)\n", Reg32[i++], var->Offset);
    }else if (var->Ty->Size == 8) {
      printf("\tmov %s, %d(%%rbp)\n", Reg64[i++], var->Offset);
    }else {
      assert(0);
    }
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
  case BinaryOperator::PtrAdd: {
    if (node->Lhs->Ty->IsPointerType()) {
      auto pty = std::dynamic_pointer_cast<PointerType>(node->Lhs->Ty)->Base;
      printf("\timul $%d, %%rdi\n", pty->Size);
    }else {
      auto pty = std::dynamic_pointer_cast<ArrayType>(node->Lhs->Ty)->ElementType;
      printf("\timul $%d, %%rdi\n", pty->Size);
    }
     printf("\tadd %%rdi, %%rax\n");
     break;
  }
  case BinaryOperator::PtrSub: {
    if (node->Lhs->Ty->IsPointerType()) {
      auto pty = std::dynamic_pointer_cast<PointerType>(node->Lhs->Ty)->Base;
      printf("\timul $%d, %%rdi\n", pty->Size);
    }else {
      auto pty = std::dynamic_pointer_cast<ArrayType>(node->Lhs->Ty)->ElementType;
      printf("\timul $%d, %%rdi\n", pty->Size);
    }
    printf("\tsub %%rdi, %%rax\n");
    break;
  }
  case BinaryOperator::PtrDiff: {
    printf("\tsub %%rdi, %%rax\n");
    if (node->Lhs->Ty->IsPointerType()) {
      auto pty = std::dynamic_pointer_cast<PointerType>(node->Lhs->Ty)->Base;
      printf("\tmov $%d, %%rdi\n", pty->Size);
    }else {
      auto pty = std::dynamic_pointer_cast<ArrayType>(node->Lhs->Ty)->ElementType;
      printf("\tmov $%d, %%rdi\n", pty->Size);
    }
    printf("\tcqo\n");
    printf("\tidiv %%rdi\n");
    break;
  }
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

void CodeGen::VisitorNumNode(NumNode *node)
{
  printf("\tmovabs $%ld, %%rax\n", node->Value);
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
  GenAddr(node->Lhs.get());
  Push();
  node->Rhs->Accept(this);
  Store(node->Ty);
}

void CodeGen::VisitorVarExprNode(VarExprNode *node) {
  GenAddr(node);
  Load(node->Ty);
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

void CodeGen::VisitorStmtExprNode(StmtExprNode *node) {
  for (auto &s : node->Stmts)
    s->Accept(this);
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
  printf("\tmov $0, %%rax\n");
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

void CodeGen::VisitorUnaryNode(UnaryNode *node) {
  switch (node->Uop) {
  case UnaryOperator::Plus: {
    node->Lhs->Accept(this);
    break;
  }
  case UnaryOperator::Minus: {
    node->Lhs->Accept(this);
    printf("\tneg %%rax\n");
    break;
  }
  case UnaryOperator::Star: {
    GenAddr(node);
    Load(node->Ty);
    break;
  }
  case UnaryOperator::Amp: {
    GenAddr(node->Lhs.get());
    break;
  }
  default:
    break;
  }
}

void CodeGen::GenAddr(AstNode *node) {
  if (auto var = dynamic_cast<VarExprNode *>(node)) {
    printf("\tlea %d(%%rbp), %%rax\n", var->VarObj->Offset);
  }else if (auto unaryNode = dynamic_cast<UnaryNode *>(node)) {
    if (unaryNode->Uop == UnaryOperator::Star) {
      unaryNode->Lhs->Accept(this);
    }else {
      DiagLoc(node->Tok->Location,"unaryNode must be dereference operation");
    }
  }else {
    DiagLoc(node->Tok->Location, "not a lvalue");
  }
}

void CodeGen::VisitorSizeofExprNode(SizeofExprNode *node) {
  printf("\tmov $%d, %%rax\n", node->Ty->Size);
}

void CodeGen::Load(std::shared_ptr<Type> ty) {
  if (ty->IsArrayType()) {
    return;
  }
  if (ty->Size == 1) {
    printf("\tmovsb (%%rax), %%rax\n");
  }else if (ty->Size == 2) {
    printf("\tmovsw (%%rax), %%rax\n");
  }else if (ty->Size == 4) {
    printf("\tmovsl (%%rax), %%rax\n");
  }else if (ty->Size == 8) {
    printf("\tmov (%%rax), %%rax\n");
  }else {
    assert(0);
  }
}

void CodeGen::Store(std::shared_ptr<Type> ty) {
  Pop("%rdi");
  if (ty->Size == 1) {
    printf("\tmov %%al, (%%rdi)\n");
  }else if (ty->Size == 2) {
    printf("\tmov %%ax, (%%rdi)\n");
  }else if (ty->Size == 4) {
    printf("\tmov %%eax, (%%rdi)\n");
  }else if (ty->Size == 8) {
    printf("\tmov %%rax, (%%rdi)\n");
  }else {
    assert(0);
  }
}


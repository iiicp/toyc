/**********************************
 * File:     Gen.cpp.cc
 *
 * Author:   caipeng
 *
 * Email:    iiicp@outlook.com
 *
 * Date:     2022/1/3
 ***********************************/

#include "CodeGen.h"
#include "Expr.h"
#include "Decl.h"
#include "Stmt.h"
#include "Basic/Diag.h"
#include <cassert>
#include "SymbolTable.h"

using namespace CCC;

void CodeGen::VisitorTranslationUnitNode(TranslationUnit *node)
{
  for (auto &f : node->ExtDecl)
    f->Accept(this);
}

void CodeGen::VisitorDeclarationNode(Declaration *node)
{
  if (node->IsLocalDecl) {
    for (auto &initDec : node->DecList) {
      if (initDec->Sym->Init != nullptr) {
        printf("\tlea %d(%%rbp), %%rax\n", initDec->Sym->Offset);
        Push();
        initDec->Sym->Init->Expr->Accept(this);
        Store(initDec->Sym->Ty);
      }
    }
  }
}

void CodeGen::VisitorFunctionNode(Function *node)
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
  for (auto &sym : node->FSym->Locals) {
    offset += sym->Ty->Size;
    offset = AlignTo(offset, sym->Ty->Align);
    sym->Offset = -offset;
  }

  offset = AlignTo(offset, 16);

  printf("\tpush %%rbp\n");
  printf("\tmov %%rsp, %%rbp\n");
  printf("\tsub $%d, %%rsp\n", offset);

  int i = 0;
  for (auto &sym : node->FSym->Params) {
    if (sym->Ty->Size == 1) {
      printf("\tmov %s, %d(%%rbp)\n", Reg8[i++], sym->Offset);
    }else if (sym->Ty->Size == 2) {
      printf("\tmov %s, %d(%%rbp)\n", Reg16[i++], sym->Offset);
    }else if (sym->Ty->Size == 4) {
      printf("\tmov %s, %d(%%rbp)\n", Reg32[i++], sym->Offset);
    }else if (sym->Ty->Size == 8) {
      printf("\tmov %s, %d(%%rbp)\n", Reg64[i++], sym->Offset);
    }else {
      assert(0);
    }
  }

  node->BlockStmt->Accept(this);

  printf(".LReturn_%s:\n", CurrentFuncName.data());
  printf("\tmov %%rbp, %%rsp\n");
  printf("\tpop %%rbp\n");
  printf("\tret\n");
}

void CodeGen::VisitorBinaryExprNode(BinaryExpr *node)
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
  case BinaryOperator::Div: {
    printf("\tcqo\n");
    printf("\tidiv %%rdi\n");
    break;
  }
  case BinaryOperator::Equal:{
    printf("\tcmp %%rdi, %%rax\n");
    printf("\tsete %%al\n");
    printf("\tmovzb %%al, %%rax\n");
    break;
  }
  case BinaryOperator::PipeEqual: {
    printf("\tcmp %%rdi, %%rax\n");
    printf("\tsetne %%al\n");
    printf("\tmovzb %%al, %%rax\n");
    break;
  }
  case BinaryOperator::Greater: {
    printf("\tcmp %%rdi, %%rax\n");
    printf("\tsetg %%al\n");
    printf("\tmovzb %%al, %%rax\n");
    break;
  }
  case BinaryOperator::GreaterEqual: {
    printf("\tcmp %%rdi, %%rax\n");
    printf("\tsetge %%al\n");
    printf("\tmovzb %%al, %%rax\n");
    break;
  }
  case BinaryOperator::Lesser: {
    printf("\tcmp %%rdi, %%rax\n");
    printf("\tsetl %%al\n");
    printf("\tmovzb %%al, %%rax\n");
    break;
  }
  case BinaryOperator::LesserEqual: {
    printf("\tcmp %%rdi, %%rax\n");
    printf("\tsetle %%al\n");
    printf("\tmovzb %%al, %%rax\n");
    break;
  }
  default:
    assert(0);
    break;
  }
}

void CodeGen::VisitorNumExprNode(NumExpr *node)
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

void CodeGen::VisitorAssignExprNode(AssignExpr *node) {
  GenAddr(node->Lhs.get());
  Push();
  node->Rhs->Accept(this);
  Store(node->Ty);
}

void CodeGen::VisitorVarExprNode(VarExpr *node) {
  GenAddr(node);
  Load(node->Ty);
}

void CodeGen::VisitorMemberExprNode(MemberExpr *node) {
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
  for (auto &d : node->Decls)
    d->Accept(this);
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

void CodeGen::VisitorStmtExprNode(StmtExpr *node) {
  for (auto &decl : node->Decls) {
    decl->Accept(this);
  }
  for (auto &s : node->Stmts)
    s->Accept(this);
}

void CodeGen::VisitorFuncCallExprNode(FuncCallExpr *node) {
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


void CodeGen::VisitorUnaryExprNode(UnaryExpr *node) {
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

  if (auto varNode = dynamic_cast<VarExpr *>(node)) {
    printf("\tlea %d(%%rbp), %%rax\n", varNode->Sym->Offset);
  }
  else if (auto unaryNode = dynamic_cast<UnaryExpr *>(node)) {
    if (unaryNode->Uop == UnaryOperator::Star) {
      unaryNode->Lhs->Accept(this);
    }else {
      GenDiag(node->Tok->Location,"unaryNode must be dereference operation");
    }
  }else if (auto memberAccessNode = dynamic_cast<MemberExpr *>(node)) {
    GenAddr(memberAccessNode->Lhs.get());
    printf("\tadd $%d, %%rax\n", memberAccessNode->Fld->Offset);
  }
    ///GenDiag(sym->Loc, "not a lvalue");
}

void CodeGen::VisitorSizeOfExprNode(SizeOfExpr *node) {
  printf("\tmov $%d, %%rax\n", node->Ty->Size);
}

void CodeGen::Load(std::shared_ptr<Type> ty) {
  if (ty->IsArrayTy() || ty->IsRecordTy()) {
    return;
  }
  if (ty->Size == 1) {
    printf("\tmovsbq (%%rax), %%rax\n");
  }else if (ty->Size == 2) {
    printf("\tmovswq (%%rax), %%rax\n");
  }else if (ty->Size == 4) {
    printf("\tmovslq (%%rax), %%rax\n");
  }else {
    printf("\tmov (%%rax), %%rax\n");
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
  }else if (ty->Size == 8){
    printf("\tmov %%rax, (%%rdi)\n");
  }else {
    printf("size = %d\n", ty->Size);
    assert(0);
  }
}


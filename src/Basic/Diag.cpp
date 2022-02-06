/***********************************
* File:     Diag.cpp.cc
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2022/1/6
***********************************/

#include "Diag.h"
#include <stdarg.h>
#include <iostream>
#include <string>

namespace CCC {

  std::string PhaseToString(DiagPhase diagPhase) {
    switch (diagPhase) {
      case DiagPhase::LexPhase:
        return "LexPhase";
      case DiagPhase::ParsePhase:
        return "ParsePhase";
      case DiagPhase::SemaPhase:
        return "SemanticsPhase";
      case DiagPhase::CodeGenPhase:
        return "CodeGenPhase";
    }
  }

  void DiagLoc(DiagPhase diagPhase, SourceLocation loc, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int len = fprintf(stderr, "%s:%s:%d:%d ", loc.FilePath, PhaseToString(diagPhase).data(), loc.Line+1, loc.Col);
    int i = loc.LineHead;
    while (loc.Code[i]!='\n'){
      fprintf(stderr, "%c", loc.Code[i]);
      i++;
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "%*s", len + loc.Col,"");
    fprintf(stderr, "^");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(0);
  }
}

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

namespace C100 {
  void DiagLoc(SourceLocation loc, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int len = fprintf(stderr, "%s:%d:%d ", loc.FilePath, loc.Line+1, loc.Col);
    int i = loc.LineHead;
    while (loc.Code[i]!='\n'){
      fprintf(stderr, "%c", loc.Code[i]);
      i++;
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "%*s", len+loc.Col,"");
    fprintf(stderr, "^");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(0);
  }
}

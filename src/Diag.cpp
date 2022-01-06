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
  void DiagE(std::string_view code, SourceLocation loc, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    std::cerr << code << std::endl;
    fprintf(stderr, "%*s^ ", loc.Col, "");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(0);
  }
}

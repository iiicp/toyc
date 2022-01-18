/***********************************
* File:     Diag.h
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2022/1/6
***********************************/

#ifndef C100_SRC_DIAG_H
#define C100_SRC_DIAG_H

#include <string_view>
#include "Lexer.h"

namespace C100 {
  void DiagLoc(SourceLocation loc, const char*fmt, ...);
}

#endif //C100_SRC_DIAG_H

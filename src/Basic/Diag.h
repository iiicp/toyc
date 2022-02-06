/***********************************
* File:     Diag.h
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2022/1/6
***********************************/

#ifndef CCC_SRC_DIAG_H
#define CCC_SRC_DIAG_H

#include "Lexer.h"
#include <string_view>

namespace CCC {

  #define LexDiag(loc, fmt, ...)       DiagLoc(DiagPhase::LexPhase, loc, fmt, ##__VA_ARGS__);
  #define ParseDiag(loc, fmt, ...)     DiagLoc(DiagPhase::ParsePhase, loc, fmt, ##__VA_ARGS__);
  #define SemaDiag(loc, fmt, ...)      DiagLoc(DiagPhase::SemaPhase, loc, fmt, ##__VA_ARGS__);
  #define GenDiag(loc, fmt, ...)       DiagLoc(DiagPhase::CodeGenPhase, loc, fmt, ##__VA_ARGS__);

  enum class DiagPhase {
    LexPhase,
    ParsePhase,
    SemaPhase,
    CodeGenPhase
  };

  void DiagLoc(DiagPhase diagPhase, SourceLocation loc, const char*fmt, ...);


}

#endif //CCC_SRC_DIAG_H

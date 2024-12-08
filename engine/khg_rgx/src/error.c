#include "khg_rgx/config.h"

const char *wrx_error(int code) {
	if(code >= 0) {
    return "No error";
  }
	switch(code) {
    case WRX_MEMORY:
      return "Out of memory";
    case WRX_VALUE: 
      return "Value expected";
    case WRX_BRACKET: 
      return "')' expected";
    case WRX_INVALID: 
      return "Invalid expression";
    case WRX_ANGLEB: 
      return "']' expected";
    case WRX_SET: 
      return "Error in [...] set";
    case WRX_RNG_ORDER: 
      return "v < u in the range [u-v]";
    case WRX_RNG_BADCHAR: 
      return "Non-alphnumeric character in [u-v]";
    case WRX_RNG_MISMATCH: 
      return "Mismatch in range [u-v]";
    case WRX_ESCAPE: 
      return "Invalid escape sequence";
    case WRX_BAD_DOLLAR: 
      return "'$' not at end of pattern";
    case WRX_CURLYB: 
      return "'}' expected";
    case WRX_BAD_CURLYB: 
      return "m > n in expression {m,n}";
    case WRX_BAD_NFA: 
      return "NFA invalid";
    case WRX_SMALL_NSM: 
      return "nsm parameter to wrx_exec() is too small";
    case WRX_INV_BREF:
      return "Invalid backreference";
    case WRX_MANY_STATES: 
      return "Too many states in expression";
    case WRX_STACK: 
      return "Can't grow stack any further";
    case WRX_OPCODE: 
      return "Unknown opcode";
	}
	return "Unknown error";
}

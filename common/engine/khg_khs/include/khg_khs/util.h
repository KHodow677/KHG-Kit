#pragma once

#define KHS_UTIL_LENOF(x) (sizeof(x)/sizeof(x[0]))
#define KHS_UTIL_UNPACK_S(strv) (strv).str, (strv).len


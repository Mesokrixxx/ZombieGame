#pragma once

#include "macros.h"

#define _ASSERTX(_c, expr, fmt, ...)										\
	do {																	\
		if (!(_c)) {														\
			fprintf(stderr,													\
				"%s:%d (%s) - ASSERTION FAILED - [%s]\n>>> "				\
				fmt "\n",													\
				__FILE__, __LINE__, __func__, expr, ##__VA_ARGS__);			\
			exit(1);														\
		}																	\
	} while (0)

#define _ASSERT8(_c, fmt, ...) _ASSERTX(_c, #_c, fmt, __VA_ARGS__)
#define _ASSERT7(_c, fmt, ...) _ASSERTX(_c, #_c, fmt, __VA_ARGS__)
#define _ASSERT6(_c, fmt, ...) _ASSERTX(_c, #_c, fmt, __VA_ARGS__)
#define _ASSERT5(_c, fmt, ...) _ASSERTX(_c, #_c, fmt, __VA_ARGS__)
#define _ASSERT4(_c, fmt, ...) _ASSERTX(_c, #_c, fmt, __VA_ARGS__)
#define _ASSERT3(_c, fmt, ...) _ASSERTX(_c, #_c, fmt, __VA_ARGS__)
#define _ASSERT2(_c, fmt) _ASSERTX(_c, #_c, fmt)
#define _ASSERT1(_c) _ASSERTX(_c, #_c, "Check your code buddy")
#define ASSERT(...) MKMACRO(_ASSERT, __VA_ARGS__)

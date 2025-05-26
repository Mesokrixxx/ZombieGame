#pragma once

#include "math.h"
#include "macros.h"

typedef v4	col;

#define _COLOF4(r, g, b, a) ((col) {{ r / 255.0, g / 255.0, b / 255.0, a }})
#define _COLOF3(r, g, b) ((col) {{ r / 255.0, g / 255.0, b / 255.0, 1 }})
#define _COLOF1(rgb) ({ f32 _rgb = (rgb / 255.0); ((col) {{ _rgb, _rgb, _rgb, 1 }}); })
#define _COLOF0() ((col) {{ 0, 0, 0, 1 }})
#define col_of(...) MKMACRO(_COLOF, __VA_ARGS__)

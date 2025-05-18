#pragma once

#include "../types.h"
#include "../macros.h"

typedef struct vec2_s {
	union {
		struct { f32 x, y; };
		f32 _data[2];
	};
}	vec2_t;

typedef struct ivec2_s {
	union {
		struct { i32 x, y; };
		i32 _data[2];
	};
}	ivec2_t;	

#define _VEC2OF2(x, y) (vec2_t){ x, y }
#define _VEC2OF1(xy) ({ f32 _xy = (xy); (vec2_t){ _xy, _xy }; })
#define _VEC2OF0() (vec2_t){ 0, 0 }
#define vec2_of(...) MKMACRO(_VEC2OF, __VA_ARGS__)
#define vec2_add(v1, v2) ({ vec2_t _v1 = (v1), _v2 = (v2); (vec2_t){ _v1.x + _v2.x, _v1.y + _v2.y }; })

#define _IVEC2OF2(x, y) (ivec2_t){ x, y }
#define _IVEC2OF1(xy) ({ i32 _xy = (xy); (ivec2_t){ _xy, _xy }; })
#define _IVEC2OF0() (ivec2_t){ 0, 0 }
#define ivec2_of(...) MKMACRO(_IVEC2OF, __VA_ARGS__)
#define ivec2_add(v1, v2) ({ ivec2_t _v1 = (v1), _v2 = (v2); (ivec2_t){ _v1.x + _v2.x, _v1.y + _v2.y }; })

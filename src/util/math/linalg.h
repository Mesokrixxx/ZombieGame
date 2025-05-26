#pragma once

#include <cglm/types-struct.h>
#include <cglm/struct/vec2.h>
#include <cglm/struct/ivec2.h>
#include <cglm/struct/vec3.h>
#include <cglm/struct/ivec3.h>
#include <cglm/struct/vec4.h>
#include <cglm/struct/ivec4.h>
#include <cglm/struct/mat2.h>
#include <cglm/struct/mat3.h>
#include <cglm/struct/mat4.h>
#include <cglm/struct/cam.h>
#include <cglm/cglm.h>

#include "../macros.h"
#include "../types.h"

#define _V2OF2(x, y) ((v2) {{ x, y }})
#define _V2OF1(xy) ({ const f32 _xy = (xy); ((v2) {{ _xy, _xy }}); })
#define _V2OF0() ((v2) {{ 0, 0 }})
#define v2_of(...) MKMACRO(_V2OF, __VA_ARGS__)
#define v2_from_v(v) ({ __typeof__(v) _v = (v); ((v2) {{ _v.x, _v.y }}); })
#define _v2_prt(v) v.x, v.y
#define v2_prt(v) _v2_prt((v))

#define _V2IOF2(x, y) ((v2i) {{ x, y }})
#define _V2IOF1(xy) ({ const i32 _xy = (xy); ((v2i) {{ _xy, _xy }}); })
#define _V2IOF0() ((v2i) {{ 0, 0 }})
#define v2i_of(...) MKMACRO(_V2IOF, __VA_ARGS__)
#define v2i_from_v(v) ({ __typeof__(v) _v = (v); ((v2i) {{ _v.x, _v.y }}); })
#define v2i_prt(v) _v2_prt((v))

#define _V3OF3(x, y, z) ((v3) {{ x, y, z }})
#define _V3OF2(v2, z) ({ __typeof__(v2) _v2 = (v2); ((v3) {{ _v2.x, _v2.y, z }}); })
#define _V3OF1(xyz) ({ const f32 _xyz = (xyz); ((v3) {{ _xyz, _xyz, _xyz }}); })
#define _V3OF0() ((v3) {{ 0, 0, 0 }})
#define v3_of(...) MKMACRO(_V3OF, __VA_ARGS__)
#define v3_from_v(v) ({ __typeof__(v) _v = (v); ((v3) {{ _v.x, _v.y, _v.z }}); })
#define _v3_prt(v) v.x, v.y, v.z
#define v3_prt(v) _v3_prt((v))

#define _V3IOF3(x, y, z) ((v3i) {{ x, y, z }})
#define _V3IOF2(v2, z) ({ __typeof__(v2) _v2 = (v2); ((v3i) {{ _v2.x, _v2.y, z }}); })
#define _V3IOF1(xyz) ({ const i32 _xyz = (xyz); ((v3i) {{ _xyz, _xyz, _xyz }}); })
#define _V3IOF0() ((v3i) {{ 0, 0, 0 }})
#define v3i_of(...) MKMACRO(_V3IOF, __VA_ARGS__)
#define v3i_from_v(v) ({ __typeof__(v) _v = (v); ((v3i) {{ _v.x, _v.y, _v.z }}); })
#define v3i_prt(v) _v3_prt((v))

#define _V4OF4(x, y, z, w) ((v4) {{ x, y, z, w }})
#define _V4OF3(v2, z, w) ({ __typeof__(v2) _v2 = (v2); ((v4) {{ _v2.x, _v2.y, z, w }}); })
#define _V4OF2(v3, w) ({ __typeof__(v3) _v3 = (v3); ((v4) {{ _v3.x, _v3.y, _v3.z, w }}); })
#define _V4OF1(xyzw) ({ const f32 _xyzw = (xyzw); ((v4) {{ _xyzw, _xyzw, _xyzw, _xyzw }}); })
#define _V4OF0() ((v4) {{ 0, 0, 0, 0 }})
#define v4_of(...) MKMACRO(_V4OF, __VA_ARGS__)
#define v4_from_v(v) ({ __typeof__(v) _v = (v); ((v4) {{ _v.x, _v.y, _v.z, _v.z }}); })
#define _v4_prt(v) v.x, v.y, v.z, v.w
#define v4_prt(v) _v4_prt((v))

#define _V4IOF4(x, y, z, w) ((v4i) {{ x, y, z, w }})
#define _V4IOF3(v2, z, w) ({ __typeof__(v2) _v2 = (v2); ((v4i) {{ _v2.x, _v2.y, z, w }}); })
#define _V4IOF2(v3, w) ({ __typeof__(v3) _v3 = (v3); ((v4i) {{ _v3.x, _v3.y, _v3.z, w }}); })
#define _V4IOF1(xyzw) ({ const i32 _xyzw = (xyzw); ((v4i) {{ _xyzw, _xyzw, _xyzw, _xyzw }}); })
#define _V4IOF0() ((v4i) {{ 0, 0, 0, 0 }})
#define v4i_of(...) MKMACRO(_V4IOF, __VA_ARGS__)
#define v4i_from_v(v) ({ __typeof__(v) _v = (v); ((v4i) {{ _v.x, _v.y, _v.z, _v.z }}); })
#define v4i_prt(v) _v4_prt((v))

#include "font.h"
#include "util/math.h"
#include "util/assert.h"

#include <ctype.h>

static v2i	find_char(char c)
{
	c = toupper(c);

	static const char	*chars[] = {
		"ABCDEFGHIJKLMNOP",
		"QRSTUVWXYZ",
		",?;./:!-+012345",
		"6789*\\_&~'\"#([])",
		"|^@={}%<>$"
	};

	for (i32 y = 0; y < 5; y++) {
		const char *p = chars[y];
		for (i32 x = 0; p[x]; x++) {
			if (p[x] == c) {
				return (v2i_of(x, y));
			}
		}
	}
	return (v2i_of(1, 2)); // ? by default
}

void	font_char(sprite_batch_t *batch, char c, v2 pos, col color, f32 z, i32 flags)
{
	sprite_batch_push(batch, 
		&(sprite_t){
			.color = color,
			.z = z,
			.flags = flags,
			.pos = pos,
			.index = find_char(c),
		});
}

void	font_str(sprite_batch_t *batch, char *c, v2 pos, col color, f32 z, i32 flags)
{
	v2	npos;

	npos = pos;
	for (i32 i = 0; c[i]; i++) {
		if (c[i] == '\n')
			npos = v2_of(pos.x, npos.y += 8);
		else if (c[i] == ' ')
			npos.x += 8;
		else {
			font_char(batch, c[i], npos, color, z, flags);
			npos.x += 8;
		}
	}
}

void	font_draw(sprite_batch_t *batch, m4 *model, m4 *view, m4 *proj)
{
	sprite_batch_draw(batch, model, view, proj);
}


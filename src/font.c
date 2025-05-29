#include "font.h"
#include "util/math.h"
#include "util/assert.h"
#include "palette.h"

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

void	font_str(sprite_batch_t *batch, char *c, v2 pos, col ocolor, f32 z, i32 flags)
{
	col	color;
	v2	npos;

	color = ocolor;
	npos = pos;
	for (i32 i = 0; c[i]; i++) {
		if (c[i] == '\n')
			npos = v2_of(pos.x, npos.y += 8);
		else if (c[i] == ' ')
			npos.x += 8;
		else if (c[i] == '$')
		{
			i++;
			if (!c[i] || c[i] == '$')
				goto draw_char; // If we got "$\0" or "$$" draw a '$'
			else if (isdigit(c[i]))
			{
				bool	two_digit_index = isdigit(c[i + 1]);

				i32	index = two_digit_index ? (c[i] * 10 + c[i + 1] - 2 * '0') : (c[i] - '0');
				if (index > PALETTE_MAXCOLOR_COUNT) index = PALETTE_MAXCOLOR_COUNT;
				color = PALETTE[index];
				i += two_digit_index ? 2 : 1;
			}
		}
		else
			goto draw_char;

draw_char:
		font_char(batch, c[i], npos, color, z, flags);
		npos.x += 8;
	}
}

void	font_draw(sprite_batch_t *batch, m4 *model, m4 *view, m4 *proj)
{
	sprite_batch_draw(batch, model, view, proj);
}


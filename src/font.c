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

void	font_char(t_sprite_batch *batch, char c, v2 pos, col color, f32 z, i32 flags)
{
	sprite_batch_push(batch, 
		&(t_sprite){
			.color = color,
			.z = z,
			.flags = flags,
			.pos = pos,
			.index = find_char(c),
		});
}

void	font_str(t_sprite_batch *batch, char *str, v2 pos, col ocolor, f32 z, i32 flags)
{
	col	color;
	v2	npos;

	color = ocolor;
	npos = pos;
	for (i32 i = 0; str[i]; i++) {
		if (str[i] == '\n')
			npos = v2_of(pos.x, npos.y += 8);
		if (str[i] >= 0 && str[i] <= 31)
			continue ;
		else if (str[i] == ' ')
		{
			npos.x += 8;
			continue ;
		}
		else if (str[i] == '$')
		{
			i++;
			if (!str[i] || str[i] == '$')
				goto draw_char; // If we got "$\0" or "$$" draw a '$'
			else if (isdigit(str[i]))
			{
				bool	two_digit_index = isdigit(str[i + 1]);

				i32	index = two_digit_index ? (str[i] * 10 + str[i + 1] - 2 * '0') : (str[i] - '0');
				if (index > PALETTE_MAXCOLOR_COUNT) index = PALETTE_MAXCOLOR_COUNT;
				color = PALETTE[index];
				i += two_digit_index ? 2 : 1;
			}
		}
		else
			goto draw_char;

draw_char:
		font_char(batch, str[i], npos, color, z, flags);
		npos.x += 8;
	}
}

void	font_draw(t_sprite_batch *batch, m4 *model, m4 *view, m4 *proj)
{
	sprite_batch_draw(batch, model, view, proj);
}


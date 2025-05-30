#pragma once

#include "util/sprite.h"
#include "util/color.h"

void	font_char(t_sprite_batch *batch, char c, v2 pos, col color, f32 z, i32 flags);
void	font_str(t_sprite_batch *batch, char *str, v2 pos, col ocolor, f32 z, i32 flags);
void	font_draw(t_sprite_batch *batch, m4 *model, m4 *view, m4 *proj);

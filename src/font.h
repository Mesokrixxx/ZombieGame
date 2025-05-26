#pragma once

#include "util/sprite.h"
#include "util/color.h"

void	font_char(sprite_batch_t *batch, char c, v2 pos, col color, f32 z, i32 flags);
void	font_str(sprite_batch_t *batch, char *c, v2 pos, col color, f32 z, i32 flags);
void	font_draw(sprite_batch_t *batch, m4 *model, m4 *view, m4 *proj);

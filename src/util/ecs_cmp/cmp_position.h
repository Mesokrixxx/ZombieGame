#pragma once

#include "../math.h"

typedef struct s_ECSCMP_Position {
	v3	pos;
}	t_ECSCMP_Position;

#ifdef UTIL_IMPL

#include "../sparseset.h"

#include <stdio.h>

static void cmppos_update(t_ECSCMP_Position *cmp, i32 id) {
	printf("UPDATING POS OF %d: (%f %f %f)\n", id, v3_prt(cmp->pos));
}

static void cmppos_tick(t_ECSCMP_Position *cmp, i32 id) {
	printf("TICKING POS OF %d: (%f %f %f)\n", id, v3_prt(cmp->pos));
}

void	cmp_position_init(t_ecscmp *cmp, i32 isize) {
	sparseset_init(&cmp->data, sizeof(t_ECSCMP_Position), isize);
	cmp->funcs = (t_ecssys){
		.init = NULL,
		.update = (f_ECSSYSFunc) cmppos_update,
		.tick = (f_ECSSYSFunc) cmppos_tick,
		.render = NULL,
		.destroy = NULL,
	};
}

#endif
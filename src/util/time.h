#pragma once

#include <SDL2/SDL_timer.h>

#include "types.h"

#define TIME_NS_TO_S(ns) ((ns) / 1000000000.0)
#define TIME_NS_TO_MS(ns) ((ns) / 1000000.0)
#define TIME_S_TO_NS(s) ((s) * 1000000000.0)
#define TIME_S_TO_MS(s) ((s) * 1000.0)
#define TIME_MS_TO_S(ms) ((ms) / 1000.0)
#define TIME_MS_TO_NS(ms) ((ms) * 1000000.0)

u64		time_ns();
void	time_cap(f32 Fdt, f32 frame_start);

static inline f64	time_s() {
	return (TIME_NS_TO_S(time_ns()));
}

static inline i32	time_fps(f32 dt) {
	return (1 / dt);
}

static inline i32	time_fps_ns(u64	dt) {
	return (time_fps(TIME_NS_TO_S(dt)));
}

#ifdef UTIL_IMPL

static u64	_time_start, _time_freq;

u64	time_ns()
{
	u64	now = SDL_GetPerformanceCounter();

	if (!_time_start)
		_time_start = now;

	if (!_time_freq)
		_time_freq = SDL_GetPerformanceFrequency();

	u64 diff = now - _time_start;
	return (TIME_S_TO_NS(diff / (f64)_time_freq));
}

void	time_cap(f32 Fdt, f32 frame_start)
{
	if (Fdt <= 0)
		return ;

	f32	elapsed_time = time_s() - frame_start;

	if (Fdt > elapsed_time)
		SDL_Delay(TIME_S_TO_MS(Fdt - elapsed_time));
}

#endif

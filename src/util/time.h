#pragma once

#include <SDL2/SDL_timer.h>

#include "types.h"

#define TIME_NS_TO_S(ns) ((ns) / 1000000000.0)
#define TIME_S_TO_NS(s) ((s) * 1000000000.0)

u64	time_ns();

static inline f64	time_s() {
	return (TIME_NS_TO_S(time_ns()));
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

#endif

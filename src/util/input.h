#pragma once

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_scancode.h>

#include "time.h"
#include "sparseset.h"
#include "math/vectors.h"

enum {
	INPUT_INVALID = 0,
	INPUT_PRESSED = (1 << 0),
	INPUT_DOWN = (1 << 1),
	INPUT_RELEASED = (1 << 2),
	INPUT_REPEAT = (1 << 3),
	INPUT_PRESENT = (1 << 7),
};

#define _INPUT_MOUSE_BUTTON_OFFSET SDL_NUM_SCANCODES
#define _INPUT_NUM_BUTTONS _INPUT_MOUSE
enum {
	INPUT_MOUSE_LEFT = _INPUT_MOUSE_BUTTON_OFFSET,
	INPUT_MOUSE_MIDDLE,
	INPUT_MOUSE_RIGHT,
	INPUT_MOUSE_4,
	INPUT_MOUSE_5,
	_INPUT_MOUSE,
};

typedef struct input_info_s {
	i8	state;
	u64	last; // Last interaction with button
}	input_info_t;

typedef struct input_s {
	u64 now;

	struct {
		ivec2_t	pos;
		ivec2_t	motion;
		vec2_t	scroll;
	}	mouse;

	sparseset_t	buttons;
	sparseset_t clear;
}	input_t;

void	input_init(input_t *i);
void	input_update(input_t *i, u64 now);
void	input_process(input_t *i, SDL_Event *ev);
void	input_destroy(input_t *i);

static inline input_info_t *input_get(input_t *i, SDL_Scancode c) {
	return (sparseset_get(&i->buttons, c));
}

static inline i8	input_state(input_t *i, SDL_Scancode c) {
	return (input_get(i, c)->state);
}

// get last interaction with button c
static inline u64	input_since(input_t *i, SDL_Scancode c) {
	return (input_get(i, c)->last);
}

static inline f64	input_since_second(input_t *i, SDL_Scancode c) {
	return (TIME_NS_TO_S(input_since(i, c)));
}

#ifdef UTIL_IMPL

void	input_init(input_t *i)
{
	*i = (input_t){0};

	sparseset_init(&i->buttons, sizeof(input_info_t), _INPUT_NUM_BUTTONS);
	sparseset_init(&i->clear, sizeof(SDL_Scancode), 8); // Up to 8 keys pressed by frame by default

	input_info_t null_info = {0};
	for (SDL_Scancode code = 0; code < _INPUT_NUM_BUTTONS; code++) {
		sparseset_add(&i->buttons, &null_info, code);
	}
}

void	input_update(input_t *i, u64 now)
{
	i->now = now;
	
	sparseset_each(&i->clear, 
		SDL_Scancode *code = sparseset_get(&i->clear, id);
		input_info_t *iinf = input_get(i, *code);
		ASSERT(iinf, "Trying to get a non set button (code: %d)", *code);
		iinf->state &= ~(INPUT_PRESSED | INPUT_RELEASED););
	sparseset_reset(&i->clear);

	i->mouse.motion = ivec2_of();
	i->mouse.scroll = vec2_of();
}

void	input_process(input_t *i, SDL_Event *ev)
{
	switch (ev->type) {
		case (SDL_MOUSEWHEEL):
			i->mouse.scroll = 
				vec2_add(i->mouse.scroll,
					vec2_of(ev->wheel.preciseX, ev->wheel.preciseY));
			break;
		case (SDL_MOUSEMOTION):
			i->mouse.motion = 
				ivec2_add(i->mouse.motion,
					ivec2_of(ev->motion.xrel, ev->motion.yrel));
			i->mouse.pos = ivec2_of(ev->motion.x, ev->motion.y);
			break;
		case (SDL_MOUSEBUTTONDOWN):
		case (SDL_MOUSEBUTTONUP):
		case (SDL_KEYDOWN):
		case (SDL_KEYUP):
			SDL_Scancode code;
			bool down, repeat = false;
			if (ev->type == SDL_MOUSEBUTTONDOWN ||
				ev->type == SDL_MOUSEBUTTONUP) {
				down = ev->button.type == SDL_MOUSEBUTTONDOWN;
				code = _INPUT_MOUSE_BUTTON_OFFSET + ev->button.button - 1;
			}
			else if (ev->type == SDL_KEYDOWN ||
				ev->type == SDL_KEYUP) {
				down = ev->key.type == SDL_KEYDOWN;
				repeat = ev->key.repeat;
				code = ev->key.keysym.scancode;
			}

			u64 last = input_since(i, code);
			i8 state = input_state(i, code);
			input_info_t *iinf = input_get(i, code);

			if (last == i->now && repeat) {
				// reject repeat on same update as press
				break;
			}

			if (!(state & INPUT_DOWN) || !repeat) {
				iinf->last = i->now;
			}

			i8 new_state = INPUT_PRESENT;

			if (repeat) {
				new_state |= INPUT_DOWN | INPUT_REPEAT;
			}

			// TODO: hack
			// fix bug where press and release happen in same update
			if (state & INPUT_PRESSED) {
				new_state |= INPUT_PRESSED;
			}

			if (state & INPUT_RELEASED) {
				new_state |= INPUT_RELEASED;
			}

			if (down) {
				if (!(state & INPUT_DOWN)) {
					new_state |= INPUT_PRESSED;
					sparseset_add(&i->clear, &code);
				}

				new_state |= INPUT_DOWN;
			} else {
				if (state & INPUT_DOWN) {
					new_state |= INPUT_RELEASED;
					sparseset_add(&i->clear, &code);
				}
			}

			if ((state & INPUT_PRESSED) || (state & INPUT_RELEASED)) {
				iinf->last = i->now;
			}

			iinf->state = new_state;
			break;
	}
}

void	input_destroy(input_t *i)
{
	sparseset_destroy(&i->buttons);
	sparseset_destroy(&i->clear);
}

#endif

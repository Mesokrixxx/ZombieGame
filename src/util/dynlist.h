#pragma once

#include "types.h"
#include "macros.h"

#define DYNLIST_DEFAULT_CAP 24

typedef struct dynlist_s {
	void	*data;
	usize	data_size;
	i32		count;
	i32		cap;
}	dynlist_t;

void	_dynlist_init(dynlist_t *l, usize data_size, i32 cap);
void	dynlist_destroy(dynlist_t *l);
void	dynlist_add(dynlist_t *l, void *data);

static inline void	dynlist_clear(dynlist_t *l) {
	l->count = 0;	
};

static inline i32	dynlist_size(dynlist_t *l) {
	return (l->count);
}

static inline i32	dynlist_cap(dynlist_t *l) {
	return (l->cap);
}

// return size of total stored data
static inline usize	dynlist_datasize(dynlist_t *l) {
	return (l->count * l->data_size);
}

// Return a ptr to the dynlist data
static inline void	*dynlist_data(dynlist_t *l) {
	return (l->data);
}

#define dynlist_each(dl_ptr, ...)						\
	dynlist_t	*l = (dl_ptr);							\
	for (i32 idx = 0; idx < l->count; idx++) {			\
		void	*vdata = l->data + idx * l->data_size;	\
		__VA_ARGS__										\
	};

#define _DLINIT3(dl_ptr, data_size, cap) _dynlist_init(dl_ptr, data_size, cap)
#define _DLINIT2(dl_ptr, data_size) _dynlist_init(dl_ptr, data_size, DYNLIST_DEFAULT_CAP)
#define dynlist_init(...) MKMACRO(_DLINIT, __VA_ARGS__)

#ifdef UTIL_IMPL

#include "debug.h"
#include <unistd.h>

void	_dynlist_init(dynlist_t *l, usize data_size, i32 cap)
{
	*l = (dynlist_t){0};

	l->cap = cap;
	l->data_size = data_size;
	l->data = _malloc(l->cap * l->data_size);
}

void	dynlist_destroy(dynlist_t *l)
{
	_free(l->data);

	*l = (dynlist_t){0};
}

void	dynlist_add(dynlist_t *l, void *data)
{
	if (l->count >= l->cap) {
		l->data = _realloc(l->data, l->cap * l->data_size, l->cap * 2 * l->data_size);
		l->cap *= 2;
	}
	memcpy(l->data + l->count * l->data_size, data, l->data_size);
	l->count++;
}

#endif

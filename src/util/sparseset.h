#pragma once

#include "types.h"
#include "macros.h"

typedef struct sparseset_s {
	i32		*sparse;
	i32		*dense;
	void	*data;
	i32		count;
	i32		capacity;
	i32		sparse_size;
	usize	data_size;
}	sparseset_t;

void	sparseset_init(sparseset_t *ss, usize data_size, i32 capacity);
void	_sparseset_add(sparseset_t *ss, void *data, i32 id);
void	*_sparseset_get(sparseset_t *ss, i32 id);
void	sparseset_remove(sparseset_t *ss, i32 id);
void	sparseset_destroy(sparseset_t *ss);

// Return the current number of elements inside ss
static inline i32	sparseset_size(sparseset_t *ss) {
	return (ss->count);
}

static inline i32	sparseset_cap(sparseset_t *ss) {
	return (ss->capacity);
}

// Return the id of the index, does assert index out of range
static inline i32	sparseset_id(sparseset_t *ss, i32 idx) {
	ASSERT(idx >= 0 && idx < ss->count,
		"Index out of range. Index: %d, limit: %d", idx, ss->count);
	
	return (ss->dense[idx]);
}

// Return the index of the id, does assert id out of range
static inline i32	sparseset_index(sparseset_t *ss, i32 id) {
	ASSERT(id >= 0 && id < ss->sparse_size,
		"ID out of range. Index: %d, limit: %d", id, ss->sparse_size);

	return (ss->sparse[id]);
}

static inline bool	sparseset_contains(sparseset_t *ss, i32 id) {
	return (id < ss->sparse_size && id >= 0 && ss->sparse[id] != -1);
}

static inline void	sparseset_reset(sparseset_t *ss) {
	memset(ss->sparse, -1, ss->count * sizeof(i32));
	ss->count = 0;
}

#define _SSADD3(ss_ptr, data, id) _sparseset_add(ss_ptr, data, id)
#define _SSADD2(ss_ptr, data) _sparseset_add(ss_ptr, data, (ss_ptr)->count)
#define sparseset_add(...) MKMACRO(_SSADD, __VA_ARGS__)

#define _SSGET3(ss_ptr, id, t) (t)_sparseset_get(ss_ptr, id)
#define _SSGET2(ss_ptr, id) _sparseset_get(ss_ptr, id)
#define sparseset_get(...) MKMACRO(_SSGET, __VA_ARGS__)

#define sparseset_each(ss_ptr, ...)				\
	do {										\
		i32 size = sparseset_size(ss_ptr);		\
		for (i32 idx = 0; idx < size; idx++) {	\
			i32 id = (ss_ptr)->sparse[idx]; 	\
			__VA_ARGS__							\
		}										\
	} while (0)

#ifdef UTIL_IMPL

#include "debug.h"

void	sparseset_init(sparseset_t *ss, usize data_size, i32 capacity)
{
	if (capacity <= 0)
		capacity = 4;

	ss->capacity = capacity;
	ss->sparse_size = capacity;
	ss->data_size = data_size;

	ss->count = 0;
	ss->dense = _malloc(ss->capacity * sizeof(i32));
	ss->data = _malloc(ss->capacity * ss->data_size);
	ss->sparse = _malloc(ss->sparse_size * sizeof(i32));
	memset(ss->sparse, -1, ss->sparse_size * sizeof(i32));
}

void	_sparseset_add(sparseset_t *ss, void *data, i32 id)
{
	ASSERT(!sparseset_contains(ss, id), "Sparseset already contains %d", id);

	if (id >= ss->sparse_size)
	{
		i32 new_sparse_size = ss->sparse_size * 2;

		if (id >= new_sparse_size)
			new_sparse_size = id + (i32)(id / 2);
		ss->sparse = _realloc(ss->sparse, ss->sparse_size * sizeof(i32),
			new_sparse_size * sizeof(i32));
		memset((char*)ss->sparse + ss->sparse_size * sizeof(i32),
			-1, (new_sparse_size - ss->sparse_size) * sizeof(i32));
		ss->sparse_size = new_sparse_size;
	}

	if (ss->count >= ss->capacity)
	{
		i32 new_capacity = ss->capacity * 2;

		ss->dense = _realloc(ss->dense, ss->capacity * sizeof(i32), new_capacity * sizeof(i32));
		ss->data = _realloc(ss->data, ss->capacity * ss->data_size, new_capacity * ss->data_size);
		ss->capacity = new_capacity;
	}

	ss->sparse[id] = ss->count;
	ss->dense[ss->count] = id;
	memcpy((char*)ss->data + ss->count * ss->data_size, data, ss->data_size);
	ss->count++;
}

void	*_sparseset_get(sparseset_t *ss, i32 id)
{
	if (sparseset_contains(ss, id))
		return ((char*)ss->data + ss->sparse[id] * ss->data_size);
	return (NULL);
}

void	sparseset_remove(sparseset_t *ss, i32 id)
{
	ASSERT(sparseset_contains(ss, id), "Sparseset doesn't contain id: %d", id);

	i32 idx = ss->sparse[id];
	i32 max_index = ss->count - 1;

	if (idx != max_index)
	{
		i32 last_id = ss->dense[max_index];

		ss->dense[idx] = last_id;

		void *dest = (char*)ss->data + idx * ss->data_size;
		void *src = (char*)ss->data + max_index * ss->data_size;

		memcpy(dest, src, ss->data_size);
	}

	ss->sparse[id] = -1;
	ss->count--;
}

void	sparseset_destroy(sparseset_t *ss)
{
	_free(ss->dense);
	_free(ss->sparse);
	_free(ss->data);

	*ss = (sparseset_t){0};
}

#endif

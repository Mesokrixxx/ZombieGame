#pragma once

#include "types.h"
#include "macros.h"

typedef struct s_sparseset {
	i32		*sparse;
	i32		*dense;
	void	*data;
	i32		count;
	i32		capacity;
	i32		sparse_size;
	usize	data_size;
	bool	auto_resize;
	bool	initialized;
}	t_sparseset;

void	_sparseset_init(t_sparseset *ss, usize data_size, i32 capacity, bool auto_resize);
void	_sparseset_init_manual(t_sparseset *ss, usize data_size, i32 cap, i32 *sparse, i32 *dense, void *data, bool auto_resize);
void	_sparseset_add(t_sparseset *ss, void *data, i32 id);
void	*sparseset_get(t_sparseset *ss, i32 id);
void	*sparseset_get_idx(t_sparseset *ss, i32 idx);
void	sparseset_remove(t_sparseset *ss, i32 id);
void	sparseset_destroy(t_sparseset *ss);

// Return the current number of elements inside ss
static inline i32	sparseset_size(t_sparseset *ss) {
	return (ss->count);
}

static inline i32	sparseset_cap(t_sparseset *ss) {
	return (ss->capacity);
}

// Return the id of the index, does assert index out of range
static inline i32	sparseset_id(t_sparseset *ss, i32 idx) {
	ASSERT(idx >= 0 && idx < ss->count,
		"Index out of range. Index: %d, limit: %d", idx, ss->count);
	
	return (ss->dense[idx]);
}

// Return the index of the id, does assert id out of range
static inline i32	sparseset_index(t_sparseset *ss, i32 id) {
	ASSERT(id >= 0 && id < ss->sparse_size,
		"ID out of range. Index: %d, limit: %d", id, ss->sparse_size);

	return (ss->sparse[id]);
}

static inline bool	sparseset_contains(t_sparseset *ss, i32 id) {
	return (id < ss->sparse_size && id >= 0 && ss->sparse[id] != -1);
}

static inline void	sparseset_reset(t_sparseset *ss) {
	memset(ss->sparse, -1, ss->count * sizeof(i32));
	ss->count = 0;
}

static inline bool	sparseset_valid(t_sparseset *ss) {
	return (ss->initialized);
}

#define _SSADD3(ss_ptr, data, id) _sparseset_add(ss_ptr, data, id)
#define _SSADD2(ss_ptr, data) _sparseset_add(ss_ptr, data, (ss_ptr)->count)
#define sparseset_add(...) MKMACRO(_SSADD, __VA_ARGS__)

#define sparseset_each(ss_ptr, ...)				\
	do {										\
		i32 size = sparseset_size(ss_ptr);		\
		for (i32 idx = 0; idx < size; idx++) {	\
			i32 id = (ss_ptr)->sparse[idx]; 	\
			__VA_ARGS__							\
		}										\
	} while (0)

#define _SSINIT4(ss_ptr, dsize, cap, autoresize) _sparseset_init(ss_ptr, dsize, cap, autoresize);
#define _SSINIT3(ss_ptr, dsize, cap) _sparseset_init(ss_ptr, dsize, cap, true);
#define sparseset_init(...) MKMACRO(_SSINIT, __VA_ARGS__)

#define _SSINITM7(ss_ptr, dsize, cap, sparse_ptr, dense_ptr, data_ptr, autoresize) \
	_sparseset_init_manual(ss_ptr, dsize, cap, sparse_ptr, dense_ptr, data_ptr, autoresize);
#define _SSINITM6(ss_ptr, dsize, cap, sparse_ptr, dense_ptr, data_ptr) \
	_sparseset_init_manual(ss_ptr, dsize, cap, sparse_ptr, dense_ptr, data_ptr, true);
#define sparseset_init_manual(...) MKMACRO(_SSINITM, __VA_ARGS__)

#ifdef UTIL_IMPL

#include "debug.h"

void	_sparseset_init_manual(t_sparseset *ss, usize data_size, i32 cap, i32 *sparse, i32 *dense, void *data, bool auto_resize)
{
	*ss = (t_sparseset){0};

	ss->capacity = cap;
	ss->sparse_size = cap;
	ss->data_size = cap;
	ss->auto_resize = auto_resize;

	ss->dense = dense;
	ss->sparse = sparse;
	ss->data = data;
	memset(ss->sparse, -1, ss->sparse_size * sizeof(i32));

	ss->initialized = true;
}

void	_sparseset_init(t_sparseset *ss, usize data_size, i32 capacity, bool auto_resize)
{
	*ss = (t_sparseset){0};

	if (capacity <= 0)
		capacity = 4;

	ss->capacity = capacity;
	ss->sparse_size = capacity;
	ss->data_size = data_size;
	ss->auto_resize = auto_resize;

	ss->dense = _malloc(ss->capacity * sizeof(i32));
	ss->data = _malloc(ss->capacity * ss->data_size);
	ss->sparse = _malloc(ss->sparse_size * sizeof(i32));
	memset(ss->sparse, -1, ss->sparse_size * sizeof(i32));

	ss->initialized = true;
}

void	_sparseset_add(t_sparseset *ss, void *data, i32 id)
{
	ASSERT(!sparseset_contains(ss, id), "Sparseset already contains %d", id);

	if (id >= ss->sparse_size)
	{
		ASSERT(ss->auto_resize, "Reach max cap for sparseset, enable auto resize or change max cap");

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
		ASSERT(ss->auto_resize, "Reach max cap for sparseset, enable auto resize or change max cap");

		i32 new_capacity = ss->capacity * 2;

		ss->dense = _realloc(ss->dense, ss->capacity * sizeof(i32), new_capacity * sizeof(i32));
		ss->data = _realloc(ss->data, ss->capacity * ss->data_size, new_capacity * ss->data_size);
		ss->capacity = new_capacity;
	}

	ss->sparse[id] = ss->count;
	ss->dense[ss->count] = id;
	if (data)
		memcpy((char*)ss->data + ss->count * ss->data_size, data, ss->data_size);
	else
		memset((char*)ss->data + ss->count * ss->data_size, 0, ss->data_size);
	ss->count++;
}

void	*sparseset_get(t_sparseset *ss, i32 id)
{
	if (sparseset_contains(ss, id))
		return ((char*)ss->data + ss->sparse[id] * ss->data_size);
	return (NULL);
}

void	*sparseset_get_idx(t_sparseset *ss, i32 idx)
{
	return (sparseset_get(ss, sparseset_id(ss, idx)));
}

void	sparseset_remove(t_sparseset *ss, i32 id)
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

void	sparseset_destroy(t_sparseset *ss)
{
	_free(ss->dense);
	_free(ss->sparse);
	_free(ss->data);

	*ss = (t_sparseset){0};
}

#endif

#pragma once

#include "types.h"

void	*_malloc(usize size);
void	_free(void *ptr);
// Use _malloc if given ptr is NULL and n_size > 0
void	*_realloc(void *ptr, usize o_size, usize n_size);

void	debug_print_alloc_stats();

#ifdef UTIL_IMPL

# include <stdlib.h>
# include <stdio.h>

i32		_alloc_count = 0;
i32		_free_count = 0;
i32		_realloc_count = 0;
usize	_total_alloc_size = 0;

void	*_malloc(usize size)
{
	void *ptr;

	ptr = malloc(size);
	if (ptr) {
		_alloc_count++;
		_total_alloc_size += size;
	}
	return (ptr);
}

void	_free(void *ptr)
{
	if (!ptr)
		return ;

	free(ptr);
	_free_count++;
}

void	*_realloc(void *ptr, usize o_size, usize n_size)
{
	void *new_ptr;
	
	if (!ptr && n_size > 0)
		return (_malloc(n_size));
	
	new_ptr = realloc(ptr, n_size);
	if (new_ptr) {
		_realloc_count++;
		usize diff = n_size - o_size;
		if (diff > 0)
			_total_alloc_size += diff;
	}
	return (new_ptr);
}

void	debug_print_alloc_stats()
{
	printf("----- ALLOCATION STATS -----\n"
		"Allocated %d times.\n"
		"Freed %d times.\n"
		"Reallocated %d times.\n"
		"Allocated in total:\n"
		"> %zu bytes\n"
		"> %zu KO\n"
		"> %zu MO\n"
		"----------------------------\n",
		_alloc_count, _free_count, _realloc_count,
		_total_alloc_size, _total_alloc_size / 1024,
		_total_alloc_size / 1024 / 1024);
}

#endif

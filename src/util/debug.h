#pragma once

#include "types.h"

void	*_malloc(usize size);
void	_free(void *ptr);
void	*_realloc(void *ptr, usize o_size, usize n_size);

void	debug_print_alloc_stats();

#ifdef UTIL_IMPL

# include <stdlib.h>
# include <stdio.h>

i32		alloc_count = 0;
i32		free_count = 0;
i32		realloc_count = 0;
usize	total_alloc_size = 0;

void	*_malloc(usize size)
{
	void *ptr;

	ptr = malloc(size);
	if (ptr) {
		alloc_count++;
		total_alloc_size += size;
	}
	return (ptr);
}

void	_free(void *ptr)
{
	if (!ptr)
		return ;

	free(ptr);
	free_count++;
}

void	*_realloc(void *ptr, usize o_size, usize n_size)
{
	void *new_ptr;
	
	if (!ptr)
		return NULL;
	
	new_ptr = realloc(ptr, n_size);
	if (new_ptr) {
		realloc_count++;
		usize diff = n_size - o_size;
		if (diff > 0)
			total_alloc_size += diff;
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
		alloc_count, free_count, realloc_count,
		total_alloc_size, total_alloc_size / 1024,
		total_alloc_size / 1024 / 1024);
}

#endif

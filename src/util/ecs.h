#pragma once 

#include "types.h"
#include "sparseset.h"

enum {
	ECS_CMP_POSITION = 0,
	_ECS_CMP_COUNT,
};

enum {
	ECS_EVT_INIT = 0,
	ECS_EVT_UPDATE,
	ECS_EVT_TICK,
	ECS_EVT_RENDER,
	ECS_EVT_DESTROY,
	_ECS_EVT_COUNT,
};

typedef void (*f_ECSSYSFunc)(void *, i32);

typedef union u_ecssys {
	struct {
		f_ECSSYSFunc	init;
		f_ECSSYSFunc	update;
		f_ECSSYSFunc	tick;
		f_ECSSYSFunc	render;
		f_ECSSYSFunc	destroy;
	};
	f_ECSSYSFunc		_fs[_ECS_EVT_COUNT];
}	t_ecssys;

typedef struct s_ecscmp {
	t_sparseset	data;
	t_ecssys	funcs;
}	t_ecscmp;

typedef struct s_ecs {
	t_sparseset	entities; // Store in entity id all linked systems
	t_ecscmp	composants[_ECS_CMP_COUNT];
}	t_ecs;

// isize is the default entity cap the ecs can hold
void	ecs_init(t_ecs *ecs, i32 ecs_isize);
void	ecs_event(t_ecs *ecs, u64 event);
void	ecs_event_id(t_ecs *ecs, u64 event, i32 id);
void	ecs_destroy(t_ecs *ecs);

// Return id of created entity
i32		ecs_new(t_ecs *ecs);
void	_ecs_add(t_ecs *ecs, i32 id, u64 cmp, void *data);
void	ecs_remove(t_ecs *ecs, i32 id);
void	*ecs_get(t_ecs *ecs, i32 id, u64 system);

#define _ECSADD4(ecs_ptr, id, cmp, data) _ecs_add(ecs_ptr, id, cmp, data);
#define _ECSADD3(ecs_ptr, id, cmp) _ecs_add(ecs_ptr, id, cmp, NULL);
#define ecs_add(...) MKMACRO(_ECSADD, __VA_ARGS__)

static inline i32	ecs_size(t_ecs *ecs) {
	return (sparseset_size(&ecs->entities));
}

#ifdef UTIL_IMPL

#include "ecs_cmp/cmp_position.h"

#define _ecs_cmpss_init(cmp_name, cmp_ptr, size) \
	cmp_name##_init(cmp_ptr, size)

void	ecs_init(t_ecs *ecs, i32 ecs_isize)
{
	*ecs = (t_ecs){0};

	sparseset_init(&ecs->entities, sizeof(char), ecs_isize);

	_ecs_cmpss_init(cmp_position, &ecs->composants[0], ecs_isize);
}

void	ecs_event(t_ecs *ecs, u64 event)
{
	for (i32 i = 0; i < _ECS_CMP_COUNT; i++) {
		f_ECSSYSFunc f = ecs->composants[i].funcs._fs[event];
		if (!f)
			continue ;

		i32	count = sparseset_size(&ecs->composants[i].data);
		for (i32 i = 0; i < count; i++) {
			void	*data = sparseset_get_idx(&ecs->composants[i].data, i);
			
			f(data, i);
		}
	}
}

void	ecs_event_id(t_ecs *ecs, u64 event, i32 id)
{
	for (i32 i = 0; i < _ECS_CMP_COUNT; i++) {
		f_ECSSYSFunc f = ecs->composants[i].funcs._fs[event];
		if (!f)
			continue ;

		void	*data = sparseset_get(&ecs->composants[i].data, i);
		
		f(data, i);
	}
}

void	ecs_destroy(t_ecs *ecs)
{
	sparseset_destroy(&ecs->entities);
	for (i32 i = 0; i < _ECS_CMP_COUNT; i++)
		sparseset_destroy(&ecs->composants[i].data);

	*ecs = (t_ecs){0};
}

i32	ecs_new(t_ecs *ecs)
{
	i32	id = ecs_size(ecs);

	sparseset_add(&ecs->entities, NULL, id);
	return (id);
}

void	_ecs_add(t_ecs *ecs, i32 id, u64 cmp, void *data)
{
	sparseset_add(&ecs->composants[cmp].data, data, id);
}

void	ecs_remove(t_ecs *ecs, i32 id)
{
	ecs_event_id(ecs, ECS_EVT_DESTROY, id);

	sparseset_remove(&ecs->entities, id);
	for (i32 i = 0; i < _ECS_CMP_COUNT; i++) {
		sparseset_remove(&ecs->composants[i].data, id);
	}
}

void	*ecs_get(t_ecs *ecs, i32 id, u64 system)
{
	return (sparseset_get(&ecs->composants[system].data, id));
}

#endif

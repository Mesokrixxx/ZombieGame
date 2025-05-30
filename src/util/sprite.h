#pragma once

#include "math.h"
#include "dynlist.h"
#include "color.h"

#define SPRITE_MAX_INSTANCES 65536 // Per frame
#define _SPRITE_SHADER_VERTSOURCE_PATH "src/util/res/shaders/sprites.vert"
#define _SPRITE_SHADER_FRAGSOURCE_PATH "src/util/res/shaders/sprites.frag"

enum {
	SPRITE_NO_FLAGS = 0 << 0,
};

typedef struct s_sprite_atlas {
	u32	id;
	u32	bind_point;

	v2i	size_sprite; // Size in sprites
	v2i	size_px; // size in pixels
	v2i sprite_size_px; // Intern sprite size (pixels)
	v2	sprite_size_tx; // Intern sprite size (texels)
	v2	tx_per_px; // texel/pixel
}	t_sprite_atlas;

typedef struct s_sprite {
	v2	pos;
	v2i	index;
	col	color;
	f32	z;
	i32	flags;
}	t_sprite;

typedef struct s_sprite_batch {
	t_sprite_atlas	*sa;
	t_dynlist		sprites;
}	t_sprite_batch;

void	sprite_init();
void	sprite_end();
void	sprite_atlas_init(t_sprite_atlas *sa, const char *path, v2i sprite_size_px);
void	sprite_atlas_destroy(t_sprite_atlas *sa);
void	sprite_batch_init(t_sprite_batch *batch, t_sprite_atlas *sa);
// model is optional
void	sprite_batch_draw(t_sprite_batch *batch, m4 *model, m4 *view, m4 *proj);
void	sprite_batch_destroy(t_sprite_batch *batch);
void	sprite_batch_push(t_sprite_batch *batch, t_sprite *sprite);

static inline void	sprite_batch_clear(t_sprite_batch *batch) {
	dynlist_clear(&batch->sprites);
}

#ifdef UTIL_IMPL

#include "gfx.h"

typedef struct _sprite_instance_s {
	v2	offset;
	v2	scale;
	v2	uv_min, uv_max;
	v4	color;
	f32	z;
	i32	flags;
}	_sprite_instance_t;

typedef struct _sprite_vertex_s {
	v2	position;
	v2	texcoord;
}	_sprite_vertex_t;

typedef struct {
	mat4	model;
	mat4	view;
	mat4	proj;
}	_sprite_vs_param;

static struct {
	bool	valid;
	u32		indicebuf, vertexbuf, instancebuf;
	struct {
		u32	id;
		u32	buf_id;
		u32	buf_bind_point;
	}		s_shader;
	u32		s_pipeline;
}	_sprite;

void	sprite_init()
{
	if (_sprite.valid)
		return ;

	_sprite.valid = true;

	u16	indices[] = {2, 1, 0, 3, 2, 0};
	_sprite_vertex_t vertices[] = {
		// pos       		  // uv
        { v2_of(0.0f, 1.0f),  v2_of(0.0f, 1.0f), },
        { v2_of(1.0f, 1.0f),  v2_of(1.0f, 1.0f), },
        { v2_of(1.0f, 0.0f),  v2_of(1.0f, 0.0f), },
        { v2_of(0.0f, 0.0f),  v2_of(0.0f, 0.0f), },
	};

	gfx_create(
		&(t_gfx){
			.s_shader = {
				.id = &_sprite.s_shader.id,
				.vertsource_path = _SPRITE_SHADER_VERTSOURCE_PATH,
				.fragsource_path = _SPRITE_SHADER_FRAGSOURCE_PATH,
				.s_uniform_buffer[0] = {
					.id = &_sprite.s_shader.buf_id,
					.bind_point = &_sprite.s_shader.buf_bind_point,
					.size = sizeof(_sprite_vs_param),
					.usage = GFX_BUFUSAGE_DYNAMIC_DRAW,
					.uniname = "vs_params",
				},
			},
			.s_buffers[0] = {
				.id = &_sprite.indicebuf,
				.data = indices,
				.size = sizeof(indices),
				.type = GFX_BUFTYPE_INDEX,
				.usage = GFX_BUFUSAGE_STATIC_DRAW
			},
			.s_buffers[1] = {
				.id = &_sprite.vertexbuf,
				.data = vertices,
				.size = sizeof(vertices),
				.type = GFX_BUFTYPE_VERTEX,
				.usage = GFX_BUFUSAGE_STATIC_DRAW
			},
			.s_buffers[2] = {
				.id = &_sprite.instancebuf,
				.data = NULL,
				.size = sizeof(_sprite_instance_t) * SPRITE_MAX_INSTANCES,
				.type = GFX_BUFTYPE_VERTEX,
				.usage = GFX_BUFUSAGE_STREAM_DRAW,
			},
			.s_pipeline = {
				.id = &_sprite.s_pipeline,
				.s_buffers = {
					.s_vertex_buffers[0] = {
						.id = &_sprite.vertexbuf,
						.stride = sizeof(_sprite_vertex_t),
					},
					.s_vertex_buffers[1] = {
						.id = &_sprite.instancebuf,
						.stride = sizeof(_sprite_instance_t),
						.step_type = GFX_PIPSTEP_TYPE_PERINSTANCE
					}
				},
				.s_attributes = {
					.s_attr[0] = {
						.buffer = 0,
						.format = GFX_ATTBFORMAT_FLOAT2,
						.offset = offsetof(_sprite_vertex_t, position),
					},
					.s_attr[1] = {
						.buffer = 0,
						.format = GFX_ATTBFORMAT_FLOAT2,
						.offset = offsetof(_sprite_vertex_t, texcoord),
					},
					.s_attr[2] = {
						.buffer = 1,
						.format = GFX_ATTBFORMAT_FLOAT2,
						.offset = offsetof(_sprite_instance_t, offset),
					},
					.s_attr[3] = {
						.buffer = 1,
						.format = GFX_ATTBFORMAT_FLOAT2,
						.offset = offsetof(_sprite_instance_t, scale),
					},
					.s_attr[4] = {
						.buffer = 1,
						.format = GFX_ATTBFORMAT_FLOAT2,
						.offset = offsetof(_sprite_instance_t, uv_min),
					},
					.s_attr[5] = {
						.buffer = 1,
						.format = GFX_ATTBFORMAT_FLOAT2,
						.offset = offsetof(_sprite_instance_t, uv_max),
					},
					.s_attr[6] = {
						.buffer = 1,
						.format = GFX_ATTBFORMAT_FLOAT4,
						.offset = offsetof(_sprite_instance_t, color),
					},
					.s_attr[7] = {
						.buffer = 1,
						.format = GFX_ATTBFORMAT_FLOAT,
						.offset = offsetof(_sprite_instance_t, z),
					},
					.s_attr[8] = {
						.buffer = 1,
						.format = GFX_ATTBFORMAT_INT,
						.offset = offsetof(_sprite_instance_t, flags),
					},
				},
			},
		});
}

void	sprite_end()
{
	_sprite.valid = false;

	gfx_delete(
		&(t_gfx){
			.s_shader = {
				.id = &_sprite.s_shader.id,
				.s_uniform_buffer[0] = { .id = &_sprite.s_shader.buf_id },
			},
			.s_buffers[0] = { .id = &_sprite.indicebuf, .type = GFX_BUFTYPE_INDEX },
			.s_buffers[1] = { .id = &_sprite.vertexbuf, .type = GFX_BUFTYPE_VERTEX },
			.s_buffers[2] = { .id = &_sprite.instancebuf, GFX_BUFTYPE_VERTEX },
			.s_pipeline = { .id = &_sprite.s_pipeline },
		});
}

void	sprite_atlas_init(t_sprite_atlas *sa, const char *path, v2i sprite_size_px)
{
	char	*data;
	v2i		size;

	gfx_create(
		&(t_gfx){
			.s_texture = {
				.id = &sa->id,
				.bind_point = &sa->bind_point,
				.path = path,
				.size = &size,
				.internalformat = GFX_TEXINTERNFORMAT_RGBA8,
				.texformat = GFX_TEXFORMAT_RGBA,
				.s_sampler = {
					.wrap_u = GFX_SMPWRAP_CLAMP_TO_EDGE,	
					.wrap_v = GFX_SMPWRAP_CLAMP_TO_EDGE,
					.min_filter = GFX_SMPFILTER_NEAREST,	
					.max_filter = GFX_SMPFILTER_NEAREST,	
				},
			},
		});

	ASSERT(size.x % sprite_size_px.x == 0,
		"Wrong dimensions for sprite atlas %s", path);
	ASSERT(size.y % sprite_size_px.y == 0,
		"Wrong dimensions for sprite atlas %s", path);

	sa->size_px = size;
	sa->sprite_size_px = sprite_size_px;
	sa->size_sprite = 
		v2i_div(sa->size_px, sa->sprite_size_px);
	sa->tx_per_px = v2_div(v2_of(1), v2_from_v(sa->size_px));
	sa->sprite_size_tx = v2_mul(v2_from_v(sa->sprite_size_px), sa->tx_per_px);

	gfx_apply(
		&(t_gfx){
			.s_shader = {
				.id = &_sprite.s_shader.id,
			},
			.s_texture = {
				.s_uniform[0] = {
					.bind_point = sa->bind_point,
					.id = "tex",
				}
			}
		});
}

void	sprite_atlas_destroy(t_sprite_atlas *sa)
{
	gfx_delete(
		&(t_gfx){
			.s_texture = {
				.id = &sa->id,
			}
		});
}

void	sprite_batch_init(t_sprite_batch *batch, t_sprite_atlas *sa)
{
	*batch = (t_sprite_batch){0};

	dynlist_init(&batch->sprites, sizeof(_sprite_instance_t));
	batch->sa = sa;
}

void	sprite_batch_draw(t_sprite_batch *batch, m4 *model, m4 *view, m4 *proj)
{
	_sprite_vs_param	param;
	m4	identity = m4_identity();
	memcpy(param.model, model ? model->raw : identity.raw, sizeof(*model));
	memcpy(param.view, view, sizeof(*view));
	memcpy(param.proj, proj, sizeof(*proj));

	gfx_bind(
		&(t_gfx){
			.s_shader = { .id = &_sprite.s_shader.id },
			.s_pipeline = { .id = &_sprite.s_pipeline },
			.s_texture = { .id = &batch->sa->id },
			.s_buffers[0] = { .id = &_sprite.indicebuf, .type = GFX_BUFTYPE_INDEX },
		});

	gfx_append(
		&(t_gfx){
			.s_buffers[0] = {
				.id = &_sprite.instancebuf,
				.type = GFX_BUFTYPE_VERTEX,
				.data = dynlist_data(&batch->sprites),
				.size = dynlist_datasize(&batch->sprites),
			},
			.s_buffers[1] = {
				.id = &_sprite.s_shader.buf_id,
				.type = GFX_BUFTYPE_UNIFORM,
				.data = &param,
				.size = sizeof(param),
			}
		});

	gfx_render(
		&(t_gfx){
			.s_render = {
				.primitives = GFX_PIPPRIMITIVE_TYPES_TRIANGLES,
				.count = dynlist_size(&batch->sprites),
				.indice_count = 6,
				.indice_type = GFX_RENDERINDEX_TYPE_U16,
				.offset = 0,
			}
		});

	dynlist_clear(&batch->sprites);
}

void	sprite_batch_destroy(t_sprite_batch *batch)
{
	dynlist_destroy(&batch->sprites);

	*batch = (t_sprite_batch){0};
}

void	sprite_batch_push(t_sprite_batch *batch, t_sprite *sprite)
{
	v2	uv_min = 
			v2_mul(
				v2_from_v(
					v2i_mul(sprite->index, batch->sa->sprite_size_px)),
				batch->sa->tx_per_px);
	v2	uv_max =
			v2_add(uv_min, batch->sa->sprite_size_tx);

	dynlist_add(&batch->sprites,
		&(_sprite_instance_t){
			.offset = sprite->pos,
			.scale = v2_from_v(batch->sa->sprite_size_px),
			.uv_min = uv_min,
			.uv_max = uv_max,
			.color = sprite->color,
			.z = sprite->z,
			.flags = sprite->flags,
		});
}

#endif

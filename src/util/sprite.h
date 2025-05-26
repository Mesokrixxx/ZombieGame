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

typedef struct sprite_atlas_s {
	u32	id;
	u32	bind_point;

	v2i	size_sprite; // Size in sprites
	v2i	size_px; // size in pixels
	v2i sprite_size_px; // Intern sprite size (pixels)
	v2	sprite_size_tx; // Intern sprite size (texels)
	v2	tx_per_px; // texel/pixel
}	sprite_atlas_t;

typedef struct sprite_s {
	v2	pos;
	v2i	index;
	col	color;
	f32	z;
	i32	flags;
}	sprite_t;

typedef struct sprite_batch_s {
	sprite_atlas_t	*sa;
	dynlist_t		sprites;
}	sprite_batch_t;

void	sprite_init();
void	sprite_end();
void	sprite_atlas_init(sprite_atlas_t *sa, const char *path, v2i sprite_size_px);
void	sprite_atlas_destroy(sprite_atlas_t *sa);
void	sprite_batch_init(sprite_batch_t *batch, sprite_atlas_t *sa);
// model is optional
void	sprite_batch_draw(sprite_batch_t *batch, m4 *model, m4 *view, m4 *proj);
void	sprite_batch_destroy(sprite_batch_t *batch);
void	sprite_batch_push(sprite_batch_t *batch, sprite_t *sprite);

static inline void	sprite_batch_clear(sprite_batch_t *batch) {
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
	}		shader;
	u32		pipeline;
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
		&(gfx_t){
			.shader = {
				.id = &_sprite.shader.id,
				.vertsource_path = _SPRITE_SHADER_VERTSOURCE_PATH,
				.fragsource_path = _SPRITE_SHADER_FRAGSOURCE_PATH,
				.uniform_buffer[0] = {
					.id = &_sprite.shader.buf_id,
					.bind_point = &_sprite.shader.buf_bind_point,
					.size = sizeof(_sprite_vs_param),
					.usage = GFX_BUFUSAGE_DYNAMIC_DRAW,
					.uniname = "vs_params",
				},
			},
			.buffers[0] = {
				.id = &_sprite.indicebuf,
				.data = indices,
				.size = sizeof(indices),
				.type = GFX_BUFTYPE_INDEX,
				.usage = GFX_BUFUSAGE_STATIC_DRAW
			},
			.buffers[1] = {
				.id = &_sprite.vertexbuf,
				.data = vertices,
				.size = sizeof(vertices),
				.type = GFX_BUFTYPE_VERTEX,
				.usage = GFX_BUFUSAGE_STATIC_DRAW
			},
			.buffers[2] = {
				.id = &_sprite.instancebuf,
				.data = NULL,
				.size = sizeof(_sprite_instance_t) * SPRITE_MAX_INSTANCES,
				.type = GFX_BUFTYPE_VERTEX,
				.usage = GFX_BUFUSAGE_STREAM_DRAW,
			},
			.pipeline = {
				.id = &_sprite.pipeline,
				.buffers = {
					.vertex_buffers[0] = {
						.id = &_sprite.vertexbuf,
						.stride = sizeof(_sprite_vertex_t),
					},
					.vertex_buffers[1] = {
						.id = &_sprite.instancebuf,
						.stride = sizeof(_sprite_instance_t),
						.step_type = GFX_PIPSTEP_TYPE_PERINSTANCE
					}
				},
				.attributes = {
					.attr[0] = {
						.buffer = 0,
						.format = GFX_ATTBFORMAT_FLOAT2,
						.offset = offsetof(_sprite_vertex_t, position),
					},
					.attr[1] = {
						.buffer = 0,
						.format = GFX_ATTBFORMAT_FLOAT2,
						.offset = offsetof(_sprite_vertex_t, texcoord),
					},
					.attr[2] = {
						.buffer = 1,
						.format = GFX_ATTBFORMAT_FLOAT2,
						.offset = offsetof(_sprite_instance_t, offset),
					},
					.attr[3] = {
						.buffer = 1,
						.format = GFX_ATTBFORMAT_FLOAT2,
						.offset = offsetof(_sprite_instance_t, scale),
					},
					.attr[4] = {
						.buffer = 1,
						.format = GFX_ATTBFORMAT_FLOAT2,
						.offset = offsetof(_sprite_instance_t, uv_min),
					},
					.attr[5] = {
						.buffer = 1,
						.format = GFX_ATTBFORMAT_FLOAT2,
						.offset = offsetof(_sprite_instance_t, uv_max),
					},
					.attr[6] = {
						.buffer = 1,
						.format = GFX_ATTBFORMAT_FLOAT4,
						.offset = offsetof(_sprite_instance_t, color),
					},
					.attr[7] = {
						.buffer = 1,
						.format = GFX_ATTBFORMAT_FLOAT,
						.offset = offsetof(_sprite_instance_t, z),
					},
					.attr[8] = {
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
		&(gfx_t){
			.shader = {
				.id = &_sprite.shader.id,
				.uniform_buffer[0] = { .id = &_sprite.shader.buf_id },
			},
			.buffers[0] = { .id = &_sprite.indicebuf, .type = GFX_BUFTYPE_INDEX },
			.buffers[1] = { .id = &_sprite.vertexbuf, .type = GFX_BUFTYPE_VERTEX },
			.buffers[2] = { .id = &_sprite.instancebuf, GFX_BUFTYPE_VERTEX },
			.pipeline = { .id = &_sprite.pipeline },
		});
}

void	sprite_atlas_init(sprite_atlas_t *sa, const char *path, v2i sprite_size_px)
{
	char	*data;
	v2i		size;

	gfx_create(
		&(gfx_t){
			.texture = {
				.id = &sa->id,
				.bind_point = &sa->bind_point,
				.path = path,
				.size = &size,
				.internalformat = GFX_TEXINTERNFORMAT_RGBA8,
				.texformat = GFX_TEXFORMAT_RGBA,
				.sampler = {
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
		&(gfx_t){
			.shader = {
				.id = &_sprite.shader.id,
			},
			.texture = {
				.uniform[0] = {
					.bind_point = sa->bind_point,
					.id = "tex",
				}
			}
		});
}

void	sprite_atlas_destroy(sprite_atlas_t *sa)
{
	gfx_delete(
		&(gfx_t){
			.texture = {
				.id = &sa->id,
			}
		});
}

void	sprite_batch_init(sprite_batch_t *batch, sprite_atlas_t *sa)
{
	*batch = (sprite_batch_t){0};

	dynlist_init(&batch->sprites, sizeof(_sprite_instance_t));
	batch->sa = sa;
}

void	sprite_batch_draw(sprite_batch_t *batch, m4 *model, m4 *view, m4 *proj)
{
	_sprite_vs_param	param;
	m4	identity = m4_identity();
	memcpy(param.model, model ? model->raw : identity.raw, sizeof(*model));
	memcpy(param.view, view, sizeof(*view));
	memcpy(param.proj, proj, sizeof(*proj));

	gfx_bind(
		&(gfx_t){
			.shader = { .id = &_sprite.shader.id },
			.pipeline = { .id = &_sprite.pipeline },
			.texture = { .id = &batch->sa->id },
			.buffers[0] = { .id = &_sprite.indicebuf, .type = GFX_BUFTYPE_INDEX },
		});

	gfx_append(
		&(gfx_t){
			.buffers[0] = {
				.id = &_sprite.instancebuf,
				.type = GFX_BUFTYPE_VERTEX,
				.data = dynlist_data(&batch->sprites),
				.size = dynlist_datasize(&batch->sprites),
			},
			.buffers[1] = {
				.id = &_sprite.shader.buf_id,
				.type = GFX_BUFTYPE_UNIFORM,
				.data = &param,
				.size = sizeof(param),
			}
		});

	gfx_render(
		&(gfx_t){
			.render = {
				.primitives = GFX_PIPPRIMITIVE_TYPES_TRIANGLES,
				.count = dynlist_size(&batch->sprites),
				.indice_count = 6,
				.indice_type = GFX_RENDERINDEX_TYPE_U16,
				.offset = 0,
			}
		});

	dynlist_clear(&batch->sprites);
}

void	sprite_batch_destroy(sprite_batch_t *batch)
{
	dynlist_destroy(&batch->sprites);

	*batch = (sprite_batch_t){0};
}

void	sprite_batch_push(sprite_batch_t *batch, sprite_t *sprite)
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

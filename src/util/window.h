#pragma once

#include "types.h"
#include "math.h"
#include "color.h"

#define _WINDOW_SDR_FRAGSOURCE_PATH "src/util/res/shaders/window.frag"
#define _WINDOW_SDR_VERTSOURCE_PATH "src/util/res/shaders/window.vert"

#define WINDOW_BUF_COLOR GL_COLOR_BUFFER_BIT
#define WINDOW_BUF_DEPTH GL_DEPTH_BUFFER_BIT

typedef struct window_s {
	v2i	window_size, resolution;
	u32	buffers;

	u32	fbo, rbo;
	u32	tex, tex_bp;
	u32	shdr, ufo, ufo_bp;
	u32	vbuf, ibuf;
	u32	pipeline;
}	window_t;

void	window_init(window_t *w, v2i window_size, v2i resolution, u32 buffers);
// Must be called before window_commit
// Bg col is only valid if WINDOW_BUF_COLOR is in w buffers
void	window_update(window_t *w, col bg_col);
// Must be called after window_update
void	window_commit(window_t *w, col window_bg_col);
void	window_destroy(window_t *w);

#ifdef UTIL_IMPL

#include "gfx.h"
#include "assert.h"

typedef struct {
	v2	pos;
	v2	uv;
}	_window_vertex;

typedef struct {
	mat4 model;
	mat4 view;
	mat4 proj;
} _window_vs_params;

static void	_window_set_viewport(u32 viewport, v2i size)
{
	static u32 last_viewport = 0;

	if (last_viewport != viewport) {
		gfx_bind(
			&(gfx_t){
				.buffers[0] = { 
					.id = &viewport, 
					.type = GFX_BUFTYPE_FRAME 
				},
			});
		glViewport(0, 0, size.x, size.y);
		last_viewport = viewport;
	}
}

void	window_init(window_t *w, v2i window_size, v2i resolution, u32 buffers)
{
	*w = (window_t){
		.window_size = window_size,
		.resolution = resolution,
		.buffers = buffers,
	};

	_window_vertex vertices[] = {
		// pos			//uv
		{ v2_of(0, 1),	v2_of(0, 1) },
		{ v2_of(1, 1),	v2_of(1, 1) },
		{ v2_of(1, 0),	v2_of(1, 0) },
		{ v2_of(0, 0),	v2_of(0, 0) },
	};
	u16	indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	gfx_create(
		&(gfx_t){
			.texture = {
				.id = &w->tex,
				.bind_point = &w->tex_bp,
				.size = &w->resolution,
				.internalformat = GFX_TEXINTERNFORMAT_RGBA8,
				.texformat = GFX_TEXFORMAT_RGBA,
				.sampler = {
					.min_filter = GFX_SMPFILTER_NEAREST,
					.max_filter = GFX_SMPFILTER_NEAREST,
					.wrap_u = GFX_SMPWRAP_CLAMP_TO_EDGE,
					.wrap_v = GFX_SMPWRAP_CLAMP_TO_EDGE,
				}
			},
			.buffers[0] = {
				.id = &w->fbo,
				.type = GFX_BUFTYPE_FRAME,
				.frame = {
					.textureid = &w->tex,
					.attachment = GFX_FRAMEATTAHC_COLOR0,
				}
			},
			.buffers[1] = {
				.id = &w->vbuf,
				.type = GFX_BUFTYPE_VERTEX,
				.usage = GFX_BUFUSAGE_STATIC_DRAW,
				.data = vertices,
				.size = sizeof(vertices),
			},
			.buffers[2] = {
				.id = &w->ibuf,
				.type = GFX_BUFTYPE_INDEX,
				.usage = GFX_BUFUSAGE_STATIC_DRAW,
				.data = indices,
				.size = sizeof(indices),
			},
			.shader = {
				.id = &w->shdr,
				.vertsource_path = _WINDOW_SDR_VERTSOURCE_PATH,
				.fragsource_path = _WINDOW_SDR_FRAGSOURCE_PATH,
				.uniform_buffer[0] = {
					.id = &w->ufo,
					.bind_point = &w->ufo_bp,
					.uniname = "vs_params",
					.size = sizeof(_window_vs_params),
					.usage = GFX_BUFUSAGE_DYNAMIC_DRAW,
				}
			},
			.pipeline = {
				.id = &w->pipeline,
				.buffers = {
					.vertex_buffers[0] = {
						.id = &w->vbuf,
						.stride = sizeof(_window_vertex),
					}
				},
				.attributes = {
					.attr[0] = {
						.buffer = 0,
						.format = GFX_ATTBFORMAT_FLOAT2,
						.offset = offsetof(_window_vertex, pos),
					},
					.attr[1] = {
						.buffer = 0,
						.format = GFX_ATTBFORMAT_FLOAT2,
						.offset = offsetof(_window_vertex, uv),
					}
				}
			},
		});
	gfx_apply(
		&(gfx_t){
			.shader = {
				.id = &w->shdr,
			},
			.texture = {
				.uniform[0] = {
					.bind_point = w->tex_bp,
					.id = "tex",
				}	
			}
		});
	if (w->buffers & WINDOW_BUF_DEPTH)
		gfx_create(
			&(gfx_t){
				.buffers[0] = {
					.id = &w->rbo,
					.type = GFX_BUFTYPE_RENDER,
					.render = {
						.attachment = GFX_RENDERATTACH_DEPTH_STENCIL,
						.size = resolution,
						.format = GFX_RENDERFMT_DEPTH24_STENCIL8,
						.framebuffer_type = GFX_BUFTYPE_FRAME,
						.framebufferid = &w->fbo,
					}
				}
			});
	ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GFX_BUFTYPE_FRAME, 0);
}

void	window_update(window_t *w, col bg_col)
{
	_window_set_viewport(w->fbo, w->resolution);

	glClearColor(bg_col.r, bg_col.g, bg_col.b, bg_col.a);
	glClear(w->buffers);
}

void	window_commit(window_t *w, col bg_window_col)
{
	_window_set_viewport(0, w->window_size);
	
	glClearColor(bg_window_col.r, bg_window_col.g, bg_window_col.b, bg_window_col.a);
	glClear(WINDOW_BUF_COLOR);

	gfx_bind(
		&(gfx_t){
			.shader = { .id = &w->shdr, },
			.pipeline = { .id = &w->pipeline },
			.texture = { .id = &w->tex },
			.buffers[0] = { .id = &w->ibuf, .type = GFX_BUFTYPE_INDEX },
		});
		
	const m4 
		model = m4_identity(),
		view = m4_identity(),
		ortho = m4_ortho(0, 1, 0, 1, -1, 1);
	_window_vs_params params;
	memcpy(params.model, &model, sizeof(model));
	memcpy(params.view, &view, sizeof(view));
	memcpy(params.proj, &ortho, sizeof(ortho));

	gfx_append(
		&(gfx_t){
			.buffers[0] = {
				.id = &w->ufo,
				.type = GFX_BUFTYPE_UNIFORM,
				.data = &params,
				.size = sizeof(params),
			}
		});

	gfx_render(
		&(gfx_t){
			.render = {
				.offset = 0,
				.count = 1,
				.indice_count = 6,
				.indice_type = GFX_RENDERINDEX_TYPE_U16,
				.primitives = GFX_PIPPRIMITIVE_TYPES_TRIANGLES,
			}
		});
}

void	window_destroy(window_t *w)
{
	gfx_delete(
		&(gfx_t){
			.texture = { .id = &w->tex },
			.buffers[0] = { .id = &w->fbo, .type = GFX_BUFTYPE_FRAME },
			.buffers[1] = { .id = &w->rbo, .type = GFX_BUFTYPE_RENDER },
			.buffers[2] = { .id = &w->vbuf, .type = GFX_BUFTYPE_VERTEX },
			.buffers[3] = { .id = &w->ibuf, .type = GFX_BUFTYPE_INDEX },
			.buffers[4] = { .id = &w->ufo, .type = GFX_BUFTYPE_UNIFORM },
			.shader = { .id = &w->shdr },
			.pipeline = { .id = &w->pipeline },
		});
}

#endif

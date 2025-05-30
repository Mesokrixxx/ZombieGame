#pragma once

#include "types.h"
#include "math.h"
#include "color.h"

#define _WINDOW_SDR_FRAGSOURCE_PATH "src/util/res/shaders/window.frag"
#define _WINDOW_SDR_VERTSOURCE_PATH "src/util/res/shaders/window.vert"

#define WINDOW_BUF_COLOR GL_COLOR_BUFFER_BIT
#define WINDOW_BUF_DEPTH GL_DEPTH_BUFFER_BIT

typedef struct s_window {
	v2i	window_size, resolution;
	u32	s_buffers;

	u32	fbo, rbo;
	u32	tex, tex_bp;
	u32	shdr, ufo, ufo_bp;
	u32	vbuf, ibuf;
	u32	s_pipeline;
}	t_window;

void	window_init(t_window *w, v2i window_size, v2i resolution, u32 s_buffers);
// Must be called before window_commit
// Bg col is only valid if WINDOW_BUF_COLOR is in w s_buffers
void	window_update(t_window *w, col bg_col);
// Must be called after window_update
void	window_commit(t_window *w, col window_bg_col);
void	window_destroy(t_window *w);

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
			&(t_gfx){
				.s_buffers[0] = { 
					.id = &viewport, 
					.type = GFX_BUFTYPE_FRAME 
				},
			});
		glViewport(0, 0, size.x, size.y);
		last_viewport = viewport;
	}
}

void	window_init(t_window *w, v2i window_size, v2i resolution, u32 s_buffers)
{
	*w = (t_window){
		.window_size = window_size,
		.resolution = resolution,
		.s_buffers = s_buffers,
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
		&(t_gfx){
			.s_texture = {
				.id = &w->tex,
				.bind_point = &w->tex_bp,
				.size = &w->resolution,
				.internalformat = GFX_TEXINTERNFORMAT_RGBA8,
				.texformat = GFX_TEXFORMAT_RGBA,
				.s_sampler = {
					.min_filter = GFX_SMPFILTER_NEAREST,
					.max_filter = GFX_SMPFILTER_NEAREST,
					.wrap_u = GFX_SMPWRAP_CLAMP_TO_EDGE,
					.wrap_v = GFX_SMPWRAP_CLAMP_TO_EDGE,
				}
			},
			.s_buffers[0] = {
				.id = &w->fbo,
				.type = GFX_BUFTYPE_FRAME,
				.s_frame = {
					.textureid = &w->tex,
					.attachment = GFX_FRAMEATTAHC_COLOR0,
				}
			},
			.s_buffers[1] = {
				.id = &w->vbuf,
				.type = GFX_BUFTYPE_VERTEX,
				.usage = GFX_BUFUSAGE_STATIC_DRAW,
				.data = vertices,
				.size = sizeof(vertices),
			},
			.s_buffers[2] = {
				.id = &w->ibuf,
				.type = GFX_BUFTYPE_INDEX,
				.usage = GFX_BUFUSAGE_STATIC_DRAW,
				.data = indices,
				.size = sizeof(indices),
			},
			.s_shader = {
				.id = &w->shdr,
				.vertsource_path = _WINDOW_SDR_VERTSOURCE_PATH,
				.fragsource_path = _WINDOW_SDR_FRAGSOURCE_PATH,
				.s_uniform_buffer[0] = {
					.id = &w->ufo,
					.bind_point = &w->ufo_bp,
					.uniname = "vs_params",
					.size = sizeof(_window_vs_params),
					.usage = GFX_BUFUSAGE_DYNAMIC_DRAW,
				}
			},
			.s_pipeline = {
				.id = &w->s_pipeline,
				.s_buffers = {
					.s_vertex_buffers[0] = {
						.id = &w->vbuf,
						.stride = sizeof(_window_vertex),
					}
				},
				.s_attributes = {
					.s_attr[0] = {
						.buffer = 0,
						.format = GFX_ATTBFORMAT_FLOAT2,
						.offset = offsetof(_window_vertex, pos),
					},
					.s_attr[1] = {
						.buffer = 0,
						.format = GFX_ATTBFORMAT_FLOAT2,
						.offset = offsetof(_window_vertex, uv),
					}
				}
			},
		});
	gfx_apply(
		&(t_gfx){
			.s_shader = {
				.id = &w->shdr,
			},
			.s_texture = {
				.s_uniform[0] = {
					.bind_point = w->tex_bp,
					.id = "tex",
				}	
			}
		});
	if (w->s_buffers & WINDOW_BUF_DEPTH)
		gfx_create(
			&(t_gfx){
				.s_buffers[0] = {
					.id = &w->rbo,
					.type = GFX_BUFTYPE_RENDER,
					.s_render = {
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

void	window_update(t_window *w, col bg_col)
{
	_window_set_viewport(w->fbo, w->resolution);

	glClearColor(bg_col.r, bg_col.g, bg_col.b, bg_col.a);
	glClear(w->s_buffers);
}

void	window_commit(t_window *w, col bg_window_col)
{
	_window_set_viewport(0, w->window_size);
	
	glClearColor(bg_window_col.r, bg_window_col.g, bg_window_col.b, bg_window_col.a);
	glClear(WINDOW_BUF_COLOR);

	gfx_bind(
		&(t_gfx){
			.s_shader = { .id = &w->shdr, },
			.s_pipeline = { .id = &w->s_pipeline },
			.s_texture = { .id = &w->tex },
			.s_buffers[0] = { .id = &w->ibuf, .type = GFX_BUFTYPE_INDEX },
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
		&(t_gfx){
			.s_buffers[0] = {
				.id = &w->ufo,
				.type = GFX_BUFTYPE_UNIFORM,
				.data = &params,
				.size = sizeof(params),
			}
		});

	gfx_render(
		&(t_gfx){
			.s_render = {
				.offset = 0,
				.count = 1,
				.indice_count = 6,
				.indice_type = GFX_RENDERINDEX_TYPE_U16,
				.primitives = GFX_PIPPRIMITIVE_TYPES_TRIANGLES,
			}
		});
}

void	window_destroy(t_window *w)
{
	gfx_delete(
		&(t_gfx){
			.s_texture = { .id = &w->tex },
			.s_buffers[0] = { .id = &w->fbo, .type = GFX_BUFTYPE_FRAME },
			.s_buffers[1] = { .id = &w->rbo, .type = GFX_BUFTYPE_RENDER },
			.s_buffers[2] = { .id = &w->vbuf, .type = GFX_BUFTYPE_VERTEX },
			.s_buffers[3] = { .id = &w->ibuf, .type = GFX_BUFTYPE_INDEX },
			.s_buffers[4] = { .id = &w->ufo, .type = GFX_BUFTYPE_UNIFORM },
			.s_shader = { .id = &w->shdr },
			.s_pipeline = { .id = &w->s_pipeline },
		});
}

#endif

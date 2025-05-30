#pragma once

/* Low level action with gfx */

#include <GL/glew.h>

#include "types.h"
#include "math.h"

#ifndef STB_IMAGE_IMPLEMENTATION
	#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb/stb_image.h>

#define GFX_TEXT_MAXCOUNT 32
#define GFX_ATTR_MAXCOUNT 12
#define GFX_BUFF_MAXCOUNT 8
#define GFX_UNIF_MAXCOUNT 8
#define GFX_UNIFBUF_MAXCOUNT 8
#define GFX_BUFFTYPE_MAXCOUNT 8
#define GFX_TEXUNI_MAXCOUNT 8

// Texture internal format
#define GFX_TEXINTERNFORMAT_RGBA8 GL_RGBA8
// Texture Format
#define GFX_TEXFORMAT_RGB GL_RGB
#define GFX_TEXFORMAT_RGBA GL_RGBA
// Filters
#define GFX_SMPFILTER_LINEAR GL_LINEAR
#define GFX_SMPFILTER_NEAREST GL_NEAREST
#define GFX_SMPFILTER_LINEAR_MIPMAP_LINEAR GL_LINEAR_MIPMAP_LINEAR
#define GFX_SMPFILTER_NEAREST_MIPMAP_LINEAR GL_NEAREST_MIPMAP_LINEAR
#define GFX_SMPFILTER_LINEAR_MIPMAP_NEAREST GL_LINEAR_MIPMAP_NEAREST
#define GFX_SMPFILTER_NEAREST_MIPMAP_NEAREST GL_NEAREST_MIPMAP_NEAREST
// Wraps
#define GFX_SMPWRAP_REPEAT GL_REPEAT
#define GFX_SMPWRAP_CLAMP_TO_EDGE GL_CLAMP_TO_EDGE
// Buffer Usage
#define GFX_BUFUSAGE_STREAM_DRAW GL_STREAM_DRAW
#define GFX_BUFUSAGE_DYNAMIC_DRAW GL_DYNAMIC_DRAW
#define GFX_BUFUSAGE_STATIC_DRAW GL_STATIC_DRAW
// Buffer type
#define GFX_BUFTYPE_INDEX GL_ELEMENT_ARRAY_BUFFER
#define GFX_BUFTYPE_VERTEX GL_ARRAY_BUFFER
#define GFX_BUFTYPE_UNIFORM GL_UNIFORM_BUFFER
#define GFX_BUFTYPE_FRAME GL_FRAMEBUFFER
#define GFX_BUFTYPE_RENDER GL_RENDERBUFFER
// Primitive types
#define GFX_PIPPRIMITIVE_TYPES_TRIANGLES GL_TRIANGLES
// Index type
#define GFX_RENDERINDEX_TYPE_U16 GL_UNSIGNED_SHORT
#define GFX_RENDERINDEX_TYPE_U32 GL_UNSIGNED_INT
// Frame attachment
#define GFX_FRAMEATTAHC_COLOR0 GL_COLOR_ATTACHMENT0
// Render buffer format
#define GFX_RENDERFMT_DEPTH24_STENCIL8 GL_DEPTH24_STENCIL8
// Render buffer 
#define GFX_RENDERATTACH_DEPTH_STENCIL GL_DEPTH_STENCIL_ATTACHMENT
// Attributes format
enum {
	GFX_ATTBFORMAT_FLOAT = 1,
	GFX_ATTBFORMAT_FLOAT2,
	GFX_ATTBFORMAT_FLOAT3,
	GFX_ATTBFORMAT_FLOAT4,
	GFX_ATTBFORMAT_INT,
	GFX_ATTBFORMAT_INT2,
	GFX_ATTBFORMAT_INT3,
	GFX_ATTBFORMAT_INT4,
};
// Step type
enum {
	GFX_PIPSTEP_TYPE_PERVERTEX = 0,
	GFX_PIPSTEP_TYPE_PERINSTANCE
};

typedef struct s_gfx {
	struct {
		u32			*id;
		u32			*bind_point;
		const char	*path;
		v2i			*size;
		u32			texformat;
		u32			internalformat;
		stbi_uc		*data;
		struct {
			i32	wrap_u, wrap_v;
			i32	min_filter, max_filter;
		}	s_sampler;
		struct {
			const char *id;
			u32			bind_point;
		}	s_uniform[GFX_TEXUNI_MAXCOUNT];
	}	s_texture;
	
	struct {
		u32			*id;
		const char	*vertsource_path;
		const char	*fragsource_path;
		struct {
			u32			*id;
			u32			*bind_point;
			const char	*uniname;
			void		*data;
			u32			usage;
			usize		size;
		}	s_uniform_buffer[GFX_UNIFBUF_MAXCOUNT];
	}	s_shader;

	struct {
		u32		*id;
		u32		type;
		usize	size;
		void	*data;
		u32		usage;
		usize	append_offset;
		struct {
			u32	attachment;
			u32	*textureid;
			i32	level;
		}	s_frame;
		struct {
			u32	format;
			v2i	size;
			u32	*framebufferid;
			u32	framebuffer_type;
			u32	attachment;
		}	s_render;
	}	s_buffers[GFX_BUFF_MAXCOUNT];

	struct {
		u32		*id;
		struct {
			struct {
				u32		*id;
				usize	stride;
				u32		step_type;
			}	s_vertex_buffers[GFX_BUFF_MAXCOUNT];
		}	s_buffers;
		struct {
			struct {
				u32		buffer;
				u32		format;
				usize	offset;
				bool	normalize;
			}	s_attr[GFX_ATTR_MAXCOUNT];
		}	s_attributes;
	}	s_pipeline;

	struct {
		u32		primitives;
		usize	offset;
		i32		count;
		u32		indice_type;
		u32		indice_count;
	}	s_render;

	struct {
		const char *id;
		u32			data_type;
		void		*data;
	}	s_uniform[GFX_UNIF_MAXCOUNT];
}	t_gfx;

void	gfx_create(t_gfx *gfx);
void	gfx_delete(t_gfx *gfx);
void	gfx_bind(t_gfx *gfx);
void	gfx_append(t_gfx *gfx);
void	gfx_render(t_gfx *gfx);
void	gfx_apply(t_gfx *gfx);

#ifndef APIENTRY
#define APIENTRY
#endif
void APIENTRY gfx_glcallback(GLenum source,
                        GLenum type,
                        GLuint id,
                        GLenum severity,
                        GLsizei length,
                        const GLchar* message,
                        const void* userParam);

#ifdef UTIL_IMPL

#include "assert.h"
#include "file.h"
#include "debug.h"
#include "sparseset.h"

static void	_gfx_image_load(t_gfx *gfx)
{
	i32	req_comp;

	switch (gfx->s_texture.texformat) {
		case (GFX_TEXFORMAT_RGB):
			req_comp = 3;
			break ;
		case (GFX_TEXFORMAT_RGBA):
			req_comp = 4;
			break ;
		default:
			ASSERT(false);
	};

	gfx->s_texture.data = 
		stbi_load(
			gfx->s_texture.path, 
			&gfx->s_texture.size->x, &gfx->s_texture.size->y, 
			NULL, req_comp);

	ASSERT(gfx->s_texture.data,
		"failed to load '%s'", gfx->s_texture.path);
}

static void	_gfx_image_release(t_gfx *gfx)
{
	if (!gfx->s_texture.data)
		return ;

	stbi_image_free(gfx->s_texture.data);
}

static void	_gfx_texture_create(t_gfx *gfx)
{
	static i32	textures_count;

	ASSERT(textures_count < GFX_TEXT_MAXCOUNT,
		"Too many textures");

	if (gfx->s_texture.path)
		_gfx_image_load(gfx);

	glGenTextures(1, gfx->s_texture.id);
	glActiveTexture(GL_TEXTURE0 + textures_count);
	glBindTexture(GL_TEXTURE_2D, *gfx->s_texture.id);

	// Sampling parameters
	bool	mipmap_use = false;
	// No need to check for max_filter (mipmap apply only when s_texture is far away)
	switch (gfx->s_texture.s_sampler.min_filter) { 
		case (GFX_SMPFILTER_LINEAR_MIPMAP_LINEAR):
		case (GFX_SMPFILTER_LINEAR_MIPMAP_NEAREST):
		case (GFX_SMPFILTER_NEAREST_MIPMAP_NEAREST):
		case (GFX_SMPFILTER_NEAREST_MIPMAP_LINEAR):
			mipmap_use = true;
			break;
	}
	if (gfx->s_texture.s_sampler.wrap_u)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gfx->s_texture.s_sampler.wrap_u);
	if (gfx->s_texture.s_sampler.wrap_v)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gfx->s_texture.s_sampler.wrap_v);
	if (gfx->s_texture.s_sampler.min_filter)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gfx->s_texture.s_sampler.min_filter);	
	if (gfx->s_texture.s_sampler.max_filter)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gfx->s_texture.s_sampler.max_filter);

	glTexImage2D(
		GL_TEXTURE_2D,
		0, 
		gfx->s_texture.internalformat, 
		gfx->s_texture.size->x, gfx->s_texture.size->y, 
		0, 
		gfx->s_texture.texformat, 
		GL_UNSIGNED_BYTE, gfx->s_texture.data);
	if (mipmap_use) 
		glGenerateMipmap(GL_TEXTURE_2D);

	_gfx_image_release(gfx);
	*gfx->s_texture.bind_point = textures_count;
	textures_count++;
}

static void	_gfx_texture_destroy(t_gfx *gfx)
{
	glDeleteTextures(1, gfx->s_texture.id);
}

static void	_gfx_texture_bind(t_gfx *gfx)
{
	static u32	last_texture;	

	if (last_texture != *gfx->s_texture.id) {
		last_texture = *gfx->s_texture.id;
		glBindTexture(GL_TEXTURE_2D, last_texture);
	}
}

static void	_gfx_buffer_create(t_gfx *gfx)
{
	for (i32 i = 0; gfx->s_buffers[i].id; i++)
	{
		u32	buf;

		switch (gfx->s_buffers[i].type) {
			case (GFX_BUFTYPE_FRAME):
				glGenFramebuffers(1, &buf);
				glBindFramebuffer(gfx->s_buffers[i].type, buf);
				glFramebufferTexture2D(
					gfx->s_buffers[i].type,
					gfx->s_buffers[i].s_frame.attachment,
					GL_TEXTURE_2D,
					*gfx->s_buffers[i].s_frame.textureid,
					gfx->s_buffers[i].s_frame.level);
				break ;
			case (GFX_BUFTYPE_RENDER):
				glGenRenderbuffers(1, &buf);
				glBindRenderbuffer(gfx->s_buffers[i].type, buf);
				glRenderbufferStorage(
					gfx->s_buffers[i].type,
					gfx->s_buffers[i].s_render.format,
					gfx->s_buffers[i].s_render.size.x,
					gfx->s_buffers[i].s_render.size.y);
				glFramebufferRenderbuffer(
					gfx->s_buffers[i].s_render.framebuffer_type,
					gfx->s_buffers[i].s_render.attachment,
					gfx->s_buffers[i].type,
					buf);
				break ;
			default:
				glGenBuffers(1, &buf);
				glBindBuffer(gfx->s_buffers[i].type, buf);
				glBufferData(
					gfx->s_buffers[i].type, 
					gfx->s_buffers[i].size,
					gfx->s_buffers[i].data, 
					gfx->s_buffers[i].usage);
				break ;
		};

		*gfx->s_buffers[i].id = buf;
	}
}

static void	_gfx_buffer_destroy(t_gfx *gfx)
{
	for (i32 i = 0; gfx->s_buffers[i].id; i++)
		switch (gfx->s_buffers[i].type) {
			case (GFX_BUFTYPE_VERTEX):
			case (GFX_BUFTYPE_INDEX):
			case (GFX_BUFTYPE_UNIFORM):
				glDeleteBuffers(1, gfx->s_buffers[i].id);
				break ;
			case (GFX_BUFTYPE_FRAME):
				glDeleteFramebuffers(1, gfx->s_buffers[i].id);
				break ;
			case (GFX_BUFTYPE_RENDER):
				glDeleteRenderbuffers(1, gfx->s_buffers[i].id);
				break ;
			default:
				ASSERT(false);
		};
}

static u32	_gfx_bufferbind_gettypeid(u32 type)
{
	switch (type) {
		case (GFX_BUFTYPE_VERTEX):
			return 0;
		case (GFX_BUFTYPE_UNIFORM):
			return 1;
		case (GFX_BUFTYPE_INDEX):
			return 2;
		case (GFX_BUFTYPE_FRAME):
			return 3;
		case (GFX_BUFTYPE_RENDER):
			return 4;
		default:
			ASSERT(false, "Buf type undefined");
	}
}

static void	_gfx_buffer_bind(t_gfx *gfx)
{
	static t_sparseset	buff_ss;
	static i32			sparse[GFX_BUFF_MAXCOUNT];
	static i32			dense[GFX_BUFF_MAXCOUNT];
	static u32			data[GFX_BUFF_MAXCOUNT];

	if (!sparseset_valid(&buff_ss))
		sparseset_init_manual(
			&buff_ss,
			sizeof(u32), GFX_BUFF_MAXCOUNT,
			sparse, dense, (void*)data, false);
	for (i32 i = 0; gfx->s_buffers[i].id; i++) {
		u32	type = gfx->s_buffers[i].type;
		u32	id = *gfx->s_buffers[i].id;
		u32	typeid = _gfx_bufferbind_gettypeid(type);
		u32	*stored_id = sparseset_get(&buff_ss, typeid);
		if (!stored_id)
			sparseset_add(&buff_ss, &id, typeid);
		else if (*stored_id != id)
			*stored_id = id;
		else
			continue ;
			
		switch (type) {
			case (GFX_BUFTYPE_FRAME):
				glBindFramebuffer(type, id);
				break ;
			case (GFX_BUFTYPE_RENDER):
				glBindRenderbuffer(type, id);
				break ;
			default:
				glBindBuffer(type, id);
				break ;
		}
	}
}

static void	_gfx_buffer_append(t_gfx *gfx) {
	for (i32 i = 0; gfx->s_buffers[i].id; i++) {
		_gfx_buffer_bind(
			&(t_gfx){
				.s_buffers[0] = {
					.id = gfx->s_buffers[i].id,
					.type = gfx->s_buffers[i].type,
				}
			});
		glBufferSubData(
			gfx->s_buffers[i].type, 
			gfx->s_buffers[i].append_offset,
			gfx->s_buffers[i].size,
			gfx->s_buffers[i].data);
	}
}

static void _gfx_shader_compile_cherror(u32 *s_shader, GLenum type)
{
	i32 	success;
	char	infolog[512];

	switch (type) {
		case(GL_VERTEX_SHADER):
		case(GL_FRAGMENT_SHADER):
			glGetShaderiv(*s_shader, GL_COMPILE_STATUS, &success);
            if (!success)
                glGetShaderInfoLog(*s_shader, 1024, NULL, infolog);
			break ;
		case(GL_SHADER):
			glGetProgramiv(*s_shader, GL_LINK_STATUS, &success);
            if (!success)
                glGetProgramInfoLog(*s_shader, 1024, NULL, infolog);
			break ;
		default:
			ASSERT(false);
	}
}

static void	_gfx_shader_compile_sh(u32 *s_shader, GLenum type, const char *source)
{
	u32	shader_temp;

	switch (type) {
		case(GL_VERTEX_SHADER):
		case(GL_FRAGMENT_SHADER):
			shader_temp = glCreateShader(type);
			glShaderSource(shader_temp, 1, &source, NULL);
			glCompileShader(shader_temp);
			*s_shader = shader_temp;
			break ;
		default:
			ASSERT(false);
	}
}

static void	_gfx_shader_compile(t_gfx *gfx)
{
	static u32	uni_buf_count = 0;

	char	*source_vert = file_get(gfx->s_shader.vertsource_path);
	char	*source_frag = file_get(gfx->s_shader.fragsource_path);

	u32	shader_frag, shader_vert, shader_prog;

	_gfx_shader_compile_sh(&shader_vert, GL_VERTEX_SHADER, source_vert);
	_gfx_shader_compile_cherror(&shader_vert, GL_VERTEX_SHADER);
	_gfx_shader_compile_sh(&shader_frag, GL_FRAGMENT_SHADER, source_frag);
	_gfx_shader_compile_cherror(&shader_frag, GL_FRAGMENT_SHADER);
	
	shader_prog = glCreateProgram();
	glAttachShader(shader_prog, shader_vert);
	glAttachShader(shader_prog, shader_frag);
	glLinkProgram(shader_prog);
	_gfx_shader_compile_cherror(&shader_prog, GL_SHADER);

	glDeleteShader(shader_vert);
	glDeleteShader(shader_frag);
	_free(source_vert);
	_free(source_frag);

	glUseProgram(shader_prog);
	*gfx->s_shader.id = shader_prog;

	for (u32 i = 0; gfx->s_shader.s_uniform_buffer[i].id; i++) {
		ASSERT(uni_buf_count < GFX_UNIFBUF_MAXCOUNT,
			"Max s_uniform buffer reach");

		_gfx_buffer_create(
			&(t_gfx){
				.s_buffers[0] = {
					.id = gfx->s_shader.s_uniform_buffer[i].id,
					.type = GFX_BUFTYPE_UNIFORM,
					.size = gfx->s_shader.s_uniform_buffer[i].size,
					.data = gfx->s_shader.s_uniform_buffer[i].data,
					.usage = gfx->s_shader.s_uniform_buffer[i].usage,
				},
			});
		u32	uni_index = 
			glGetUniformBlockIndex(
				shader_prog, 
				gfx->s_shader.s_uniform_buffer[i].uniname);
		glUniformBlockBinding(shader_prog, uni_index, uni_buf_count);
		glBindBufferBase(
			GFX_BUFTYPE_UNIFORM, uni_buf_count,
			*gfx->s_shader.s_uniform_buffer[i].id);
		*gfx->s_shader.s_uniform_buffer[i].bind_point = uni_buf_count;
		uni_buf_count++;
	}
}

static void	_gfx_shader_delete(t_gfx *gfx)
{
	glDeleteProgram(*gfx->s_shader.id);
	for (u32 i = 0; gfx->s_shader.s_uniform_buffer[i].id; i++) {
		glDeleteBuffers(1, gfx->s_shader.s_uniform_buffer[i].id);
	}
}

static void	_gfx_shader_use(t_gfx *gfx)
{
	static u32	last_shader;

	if (last_shader != *gfx->s_shader.id)
	{
		glUseProgram(*gfx->s_shader.id);
		last_shader = *gfx->s_shader.id;
	}
	if (gfx->s_shader.s_uniform_buffer[0].id)
		_gfx_buffer_bind(
			&(t_gfx){
				.s_buffers[0] = {
					.id = gfx->s_shader.s_uniform_buffer[0].id,
					.type = GFX_BUFTYPE_UNIFORM,
				}
			});
}

static u32	_gfx_pipeline_get_attrcount(u32 format) {
	switch (format) {
		case (GFX_ATTBFORMAT_FLOAT):
		case (GFX_ATTBFORMAT_INT):
			return (1);
		case (GFX_ATTBFORMAT_FLOAT2):
		case (GFX_ATTBFORMAT_INT2):
			return (2);
		case (GFX_ATTBFORMAT_FLOAT3):
		case (GFX_ATTBFORMAT_INT3):
			return (3);
		case (GFX_ATTBFORMAT_FLOAT4):
		case (GFX_ATTBFORMAT_INT4):
			return (4);
		default: 
			ASSERT(false, "Unknown type");
	};
}

static u32	_gfx_pipeline_get_attrformat(u32 format) {
	switch (format) {
		case (GFX_ATTBFORMAT_FLOAT):
		case (GFX_ATTBFORMAT_FLOAT2):
		case (GFX_ATTBFORMAT_FLOAT3):
		case (GFX_ATTBFORMAT_FLOAT4):
			return (GL_FLOAT);
		case (GFX_ATTBFORMAT_INT):
		case (GFX_ATTBFORMAT_INT2):
		case (GFX_ATTBFORMAT_INT3):
		case (GFX_ATTBFORMAT_INT4):
			return (GL_INT);
		default: 
			ASSERT(false, "Unknown type");
	};
}

static void	_gfx_pipeline_create(t_gfx *gfx)
{
	u32	vao;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	for (u32 i = 0; gfx->s_pipeline.s_attributes.s_attr[i].format; i++)
	{
		u32	buffer_index = gfx->s_pipeline.s_attributes.s_attr[i].buffer;
		gfx_bind(
			&(t_gfx){
				.s_buffers[0] = {
					.id = gfx->s_pipeline.s_buffers.s_vertex_buffers[buffer_index].id,
					.type = GFX_BUFTYPE_VERTEX,
				},
			});

		glEnableVertexAttribArray(i);
		u32	format = _gfx_pipeline_get_attrformat(gfx->s_pipeline.s_attributes.s_attr[i].format);
		switch (format) {
			case (GL_FLOAT):
				glVertexAttribPointer(
					i, _gfx_pipeline_get_attrcount(gfx->s_pipeline.s_attributes.s_attr[i].format), 
					format, 
					gfx->s_pipeline.s_attributes.s_attr[i].normalize, 
					gfx->s_pipeline.s_buffers.s_vertex_buffers[buffer_index].stride,
					(void*)(gfx->s_pipeline.s_attributes.s_attr[i].offset));
				break ;
			case (GL_INT):
				glVertexAttribIPointer(
					i, _gfx_pipeline_get_attrcount(gfx->s_pipeline.s_attributes.s_attr[i].format), 
					format,
					gfx->s_pipeline.s_buffers.s_vertex_buffers[buffer_index].stride,
					(void*)(gfx->s_pipeline.s_attributes.s_attr[i].offset));
				break ;
		}
		glVertexAttribDivisor(i, gfx->s_pipeline.s_buffers.s_vertex_buffers[buffer_index].step_type);
	}

	*gfx->s_pipeline.id = vao;
}

static void	_gfx_pipeline_destroy(t_gfx *gfx)
{
	glDeleteVertexArrays(1, gfx->s_pipeline.id);
}

static void	_gfx_pipeline_bind(t_gfx *gfx)
{
	static u32 last_pipeline;

	u32 new_pip = *gfx->s_pipeline.id;
	if (last_pipeline != new_pip) {
		glBindVertexArray(new_pip);
		last_pipeline = new_pip;
	}
}

void	gfx_create(t_gfx *gfx)
{
	i32	gl_error;

	if (gfx->s_texture.id) {
		_gfx_texture_create(gfx);
	}

	if (gfx->s_shader.id) {
		ASSERT(
			gfx->s_shader.fragsource_path && gfx->s_shader.vertsource_path,
			"Both required path need to be valid to compile a s_shader");
		_gfx_shader_compile(gfx);
	}

	if (gfx->s_buffers[0].id) {
		_gfx_buffer_create(gfx);
	}

	if (gfx->s_pipeline.id) {
		_gfx_pipeline_create(gfx);
	}
}

void	gfx_delete(t_gfx *gfx)
{
	if (gfx->s_texture.id)
		_gfx_texture_destroy(gfx);

	if (gfx->s_shader.id)
		_gfx_shader_delete(gfx);

	if (gfx->s_buffers[0].id)
		_gfx_buffer_destroy(gfx);

	if (gfx->s_pipeline.id)
		_gfx_pipeline_destroy(gfx);
}

void	gfx_bind(t_gfx *gfx)
{
	if (gfx->s_shader.id)
		_gfx_shader_use(gfx);

	if (gfx->s_pipeline.id)
		_gfx_pipeline_bind(gfx);

	if (gfx->s_texture.id)
		_gfx_texture_bind(gfx);

	if (gfx->s_buffers[0].id)
		_gfx_buffer_bind(gfx);
}

void	gfx_append(t_gfx *gfx)
{
	if (gfx->s_buffers[0].id) {
		_gfx_buffer_append(gfx);
	}
}

void	gfx_render(t_gfx *gfx)
{
	if (!gfx->s_render.count || gfx->s_render.count < 2)
		glDrawElements(
			gfx->s_render.primitives,
			gfx->s_render.indice_count,
			gfx->s_render.indice_type,
			(void*)(gfx->s_render.offset));
	else
		glDrawElementsInstanced(
			gfx->s_render.primitives,
			gfx->s_render.indice_count,
			gfx->s_render.indice_type,
			(void*)(gfx->s_render.offset),
			gfx->s_render.count);
}

static void	_gfx_apply_texuni(t_gfx *gfx)
{
	for (u32 i = 0; gfx->s_texture.s_uniform[i].id; i++) {
		glUniform1i(
			glGetUniformLocation(
				*gfx->s_shader.id, 
				gfx->s_texture.s_uniform[i].id), 
				gfx->s_texture.s_uniform[i].bind_point);
	}
}

static void _gfx_apply_uniform(t_gfx *gfx)
{
	for (u32 i = 0; gfx->s_uniform[i].id; i++) {

	}
}

void	gfx_apply(t_gfx *gfx)
{
	if (gfx->s_shader.id)
		_gfx_shader_use(gfx);

	if (gfx->s_texture.s_uniform[0].id)
		_gfx_apply_texuni(gfx);

	if (gfx->s_uniform[0].id)
		_gfx_apply_uniform(gfx);
}

void APIENTRY gfx_glcallback(GLenum source,
                                     GLenum type,
                                     GLuint id,
                                     GLenum severity,
                                     GLsizei length,
                                     const GLchar* message,
                                     const void* userParam) {
    fprintf(stderr, "OPENGL NOTIF \n>>> %s\n", message);
}

#endif

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

typedef struct gfx_s {
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
		}	sampler;
		struct {
			const char *id;
			u32			bind_point;
		}	uniform[GFX_TEXUNI_MAXCOUNT];
	}	texture;
	
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
		}	uniform_buffer[GFX_UNIFBUF_MAXCOUNT];
	}	shader;

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
		}	frame;
		struct {
			u32	format;
			v2i	size;
			u32	*framebufferid;
			u32	framebuffer_type;
			u32	attachment;
		}	render;
	}	buffers[GFX_BUFF_MAXCOUNT];

	struct {
		u32		*id;
		struct {
			struct {
				u32		*id;
				usize	stride;
				u32		step_type;
			}	vertex_buffers[GFX_BUFF_MAXCOUNT];
		}	buffers;
		struct {
			struct {
				u32		buffer;
				u32		format;
				usize	offset;
				bool	normalize;
			}	attr[GFX_ATTR_MAXCOUNT];
		}	attributes;
	}	pipeline;

	struct {
		u32		primitives;
		usize	offset;
		i32		count;
		u32		indice_type;
		u32		indice_count;
	}	render;

	struct {
		const char *id;
		u32			data_type;
		void		*data;
	}	uniform[GFX_UNIF_MAXCOUNT];
}	gfx_t;

void	gfx_create(gfx_t *gfx);
void	gfx_delete(gfx_t *gfx);
void	gfx_bind(gfx_t *gfx);
void	gfx_append(gfx_t *gfx);
void	gfx_render(gfx_t *gfx);
void	gfx_apply(gfx_t *gfx);

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

static void	_gfx_image_load(gfx_t *gfx)
{
	i32	req_comp;

	switch (gfx->texture.texformat) {
		case (GFX_TEXFORMAT_RGB):
			req_comp = 3;
			break ;
		case (GFX_TEXFORMAT_RGBA):
			req_comp = 4;
			break ;
		default:
			ASSERT(false);
	};

	gfx->texture.data = 
		stbi_load(
			gfx->texture.path, 
			&gfx->texture.size->x, &gfx->texture.size->y, 
			NULL, req_comp);

	ASSERT(gfx->texture.data,
		"failed to load '%s'", gfx->texture.path);
}

static void	_gfx_image_release(gfx_t *gfx)
{
	if (!gfx->texture.data)
		return ;

	stbi_image_free(gfx->texture.data);
}

static void	_gfx_texture_create(gfx_t *gfx)
{
	static i32	textures_count;

	ASSERT(textures_count < GFX_TEXT_MAXCOUNT,
		"Too many textures");

	if (gfx->texture.path)
		_gfx_image_load(gfx);

	glGenTextures(1, gfx->texture.id);
	glActiveTexture(GL_TEXTURE0 + textures_count);
	glBindTexture(GL_TEXTURE_2D, *gfx->texture.id);

	// Sampling parameters
	bool	mipmap_use = false;
	// No need to check for max_filter (mipmap apply only when texture is far away)
	switch (gfx->texture.sampler.min_filter) { 
		case (GFX_SMPFILTER_LINEAR_MIPMAP_LINEAR):
		case (GFX_SMPFILTER_LINEAR_MIPMAP_NEAREST):
		case (GFX_SMPFILTER_NEAREST_MIPMAP_NEAREST):
		case (GFX_SMPFILTER_NEAREST_MIPMAP_LINEAR):
			mipmap_use = true;
			break;
	}
	if (gfx->texture.sampler.wrap_u)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gfx->texture.sampler.wrap_u);
	if (gfx->texture.sampler.wrap_v)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gfx->texture.sampler.wrap_v);
	if (gfx->texture.sampler.min_filter)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gfx->texture.sampler.min_filter);	
	if (gfx->texture.sampler.max_filter)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gfx->texture.sampler.max_filter);

	glTexImage2D(
		GL_TEXTURE_2D,
		0, 
		gfx->texture.internalformat, 
		gfx->texture.size->x, gfx->texture.size->y, 
		0, 
		gfx->texture.texformat, 
		GL_UNSIGNED_BYTE, gfx->texture.data);
	if (mipmap_use) 
		glGenerateMipmap(GL_TEXTURE_2D);

	_gfx_image_release(gfx);
	*gfx->texture.bind_point = textures_count;
	textures_count++;
}

static void	_gfx_texture_destroy(gfx_t *gfx)
{
	glDeleteTextures(1, gfx->texture.id);
}

static void	_gfx_texture_bind(gfx_t *gfx)
{
	static u32	last_texture;	

	if (last_texture != *gfx->texture.id) {
		last_texture = *gfx->texture.id;
		glBindTexture(GL_TEXTURE_2D, last_texture);
	}
}

static void	_gfx_buffer_create(gfx_t *gfx)
{
	for (i32 i = 0; gfx->buffers[i].id; i++)
	{
		u32	buf;

		switch (gfx->buffers[i].type) {
			case (GFX_BUFTYPE_FRAME):
				glGenFramebuffers(1, &buf);
				glBindFramebuffer(gfx->buffers[i].type, buf);
				glFramebufferTexture2D(
					gfx->buffers[i].type,
					gfx->buffers[i].frame.attachment,
					GL_TEXTURE_2D,
					*gfx->buffers[i].frame.textureid,
					gfx->buffers[i].frame.level);
				break ;
			case (GFX_BUFTYPE_RENDER):
				glGenRenderbuffers(1, &buf);
				glBindRenderbuffer(gfx->buffers[i].type, buf);
				glRenderbufferStorage(
					gfx->buffers[i].type,
					gfx->buffers[i].render.format,
					gfx->buffers[i].render.size.x,
					gfx->buffers[i].render.size.y);
				glFramebufferRenderbuffer(
					gfx->buffers[i].render.framebuffer_type,
					gfx->buffers[i].render.attachment,
					gfx->buffers[i].type,
					buf);
				break ;
			default:
				glGenBuffers(1, &buf);
				glBindBuffer(gfx->buffers[i].type, buf);
				glBufferData(
					gfx->buffers[i].type, 
					gfx->buffers[i].size,
					gfx->buffers[i].data, 
					gfx->buffers[i].usage);
				break ;
		};

		*gfx->buffers[i].id = buf;
	}
}

static void	_gfx_buffer_destroy(gfx_t *gfx)
{
	for (i32 i = 0; gfx->buffers[i].id; i++)
		switch (gfx->buffers[i].type) {
			case (GFX_BUFTYPE_VERTEX):
			case (GFX_BUFTYPE_INDEX):
			case (GFX_BUFTYPE_UNIFORM):
				glDeleteBuffers(1, gfx->buffers[i].id);
				break ;
			case (GFX_BUFTYPE_FRAME):
				glDeleteFramebuffers(1, gfx->buffers[i].id);
				break ;
			case (GFX_BUFTYPE_RENDER):
				glDeleteRenderbuffers(1, gfx->buffers[i].id);
				break ;
			default:
				ASSERT(false);
		};
}

static void	_gfx_buffer_bind(gfx_t *gfx)
{
	static struct {
		u32	type;
		u32	last_id;
	}	buffers[GFX_BUFFTYPE_MAXCOUNT];

	i32	i = 0;
	while (i < GFX_BUFFTYPE_MAXCOUNT) {
		if (!buffers[i].type || buffers[i].type == gfx->buffers[0].type) {
			buffers[i].type = gfx->buffers[0].type;
			if (buffers[i].last_id != *gfx->buffers[0].id) {
				buffers[i].last_id = *gfx->buffers[0].id;
				switch (buffers[i].type) {
					case (GFX_BUFTYPE_FRAME):
						glBindFramebuffer(buffers[i].type, buffers[i].last_id);
						break ;
					default:
						glBindBuffer(buffers[i].type, buffers[i].last_id);
						break ;
				}
			}
			return ;
		}
		i++;
	}
	ASSERT(false, "Exceeded buff type");
}

static void	_gfx_buffer_append(gfx_t *gfx) {
	for (i32 i = 0; gfx->buffers[i].id; i++) {
		_gfx_buffer_bind(
			&(gfx_t){
				.buffers[0] = {
					.id = gfx->buffers[i].id,
					.type = gfx->buffers[i].type,
				}
			});
		glBufferSubData(
			gfx->buffers[i].type, 
			gfx->buffers[i].append_offset,
			gfx->buffers[i].size,
			gfx->buffers[i].data);
	}
}

static void _gfx_shader_compile_cherror(u32 *shader, GLenum type)
{
	i32 	success;
	char	infolog[512];

	switch (type) {
		case(GL_VERTEX_SHADER):
		case(GL_FRAGMENT_SHADER):
			glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
            if (!success)
                glGetShaderInfoLog(*shader, 1024, NULL, infolog);
			break ;
		case(GL_SHADER):
			glGetProgramiv(*shader, GL_LINK_STATUS, &success);
            if (!success)
                glGetProgramInfoLog(*shader, 1024, NULL, infolog);
			break ;
		default:
			ASSERT(false);
	}
}

static void	_gfx_shader_compile_sh(u32 *shader, GLenum type, const char *source)
{
	u32	shader_temp;

	switch (type) {
		case(GL_VERTEX_SHADER):
		case(GL_FRAGMENT_SHADER):
			shader_temp = glCreateShader(type);
			glShaderSource(shader_temp, 1, &source, NULL);
			glCompileShader(shader_temp);
			*shader = shader_temp;
			break ;
		default:
			ASSERT(false);
	}
}

static void	_gfx_shader_compile(gfx_t *gfx)
{
	static u32	uni_buf_count = 0;

	char	*source_vert = file_get(gfx->shader.vertsource_path);
	char	*source_frag = file_get(gfx->shader.fragsource_path);

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
	*gfx->shader.id = shader_prog;

	for (u32 i = 0; gfx->shader.uniform_buffer[i].id; i++) {
		ASSERT(uni_buf_count < GFX_UNIFBUF_MAXCOUNT,
			"Max uniform buffer reach");

		_gfx_buffer_create(
			&(gfx_t){
				.buffers[0] = {
					.id = gfx->shader.uniform_buffer[i].id,
					.type = GFX_BUFTYPE_UNIFORM,
					.size = gfx->shader.uniform_buffer[i].size,
					.data = gfx->shader.uniform_buffer[i].data,
					.usage = gfx->shader.uniform_buffer[i].usage,
				},
			});
		u32	uni_index = 
			glGetUniformBlockIndex(
				shader_prog, 
				gfx->shader.uniform_buffer[i].uniname);
		glUniformBlockBinding(shader_prog, uni_index, uni_buf_count);
		glBindBufferBase(
			GFX_BUFTYPE_UNIFORM, uni_buf_count,
			*gfx->shader.uniform_buffer[i].id);
		*gfx->shader.uniform_buffer[i].bind_point = uni_buf_count;
		uni_buf_count++;
	}
}

static void	_gfx_shader_delete(gfx_t *gfx)
{
	glDeleteProgram(*gfx->shader.id);
	for (u32 i = 0; gfx->shader.uniform_buffer[i].id; i++) {
		glDeleteBuffers(1, gfx->shader.uniform_buffer[i].id);
	}
}

static void	_gfx_shader_use(gfx_t *gfx)
{
	static u32	last_shader;

	if (last_shader != *gfx->shader.id)
	{
		glUseProgram(*gfx->shader.id);
		last_shader = *gfx->shader.id;
	}
	if (gfx->shader.uniform_buffer[0].id)
		_gfx_buffer_bind(
			&(gfx_t){
				.buffers[0] = {
					.id = gfx->shader.uniform_buffer[0].id,
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

static void	_gfx_pipeline_create(gfx_t *gfx)
{
	u32	vao;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	for (u32 i = 0; gfx->pipeline.attributes.attr[i].format; i++)
	{
		u32	buffer_index = gfx->pipeline.attributes.attr[i].buffer;
		gfx_bind(
			&(gfx_t){
				.buffers[0] = {
					.id = gfx->pipeline.buffers.vertex_buffers[buffer_index].id,
					.type = GFX_BUFTYPE_VERTEX,
				},
			});

		glEnableVertexAttribArray(i);
		u32	format = _gfx_pipeline_get_attrformat(gfx->pipeline.attributes.attr[i].format);
		switch (format) {
			case (GL_FLOAT):
				glVertexAttribPointer(
					i, _gfx_pipeline_get_attrcount(gfx->pipeline.attributes.attr[i].format), 
					format, 
					gfx->pipeline.attributes.attr[i].normalize, 
					gfx->pipeline.buffers.vertex_buffers[buffer_index].stride,
					(void*)(gfx->pipeline.attributes.attr[i].offset));
				break ;
			case (GL_INT):
				glVertexAttribIPointer(
					i, _gfx_pipeline_get_attrcount(gfx->pipeline.attributes.attr[i].format), 
					format,
					gfx->pipeline.buffers.vertex_buffers[buffer_index].stride,
					(void*)(gfx->pipeline.attributes.attr[i].offset));
				break ;
		}
		glVertexAttribDivisor(i, gfx->pipeline.buffers.vertex_buffers[buffer_index].step_type);
	}

	*gfx->pipeline.id = vao;
}

static void	_gfx_pipeline_destroy(gfx_t *gfx)
{
	glDeleteVertexArrays(1, gfx->pipeline.id);
}

static void	_gfx_pipeline_bind(gfx_t *gfx)
{
	static u32 last_pipeline;

	u32 new_pip = *gfx->pipeline.id;
	if (last_pipeline != new_pip) {
		glBindVertexArray(new_pip);
		last_pipeline = new_pip;
	}
}

void	gfx_create(gfx_t *gfx)
{
	i32	gl_error;

	if (gfx->texture.id) {
		_gfx_texture_create(gfx);
	}

	if (gfx->shader.id) {
		ASSERT(
			gfx->shader.fragsource_path && gfx->shader.vertsource_path,
			"Both required path need to be valid to compile a shader");
		_gfx_shader_compile(gfx);
	}

	if (gfx->buffers[0].id) {
		_gfx_buffer_create(gfx);
	}

	if (gfx->pipeline.id) {
		_gfx_pipeline_create(gfx);
	}
}

void	gfx_delete(gfx_t *gfx)
{
	if (gfx->texture.id)
		_gfx_texture_destroy(gfx);

	if (gfx->shader.id)
		_gfx_shader_delete(gfx);

	if (gfx->buffers[0].id)
		_gfx_buffer_destroy(gfx);

	if (gfx->pipeline.id)
		_gfx_pipeline_destroy(gfx);
}

void	gfx_bind(gfx_t *gfx)
{
	if (gfx->shader.id)
		_gfx_shader_use(gfx);

	if (gfx->pipeline.id)
		_gfx_pipeline_bind(gfx);

	if (gfx->texture.id)
		_gfx_texture_bind(gfx);

	if (gfx->buffers[0].id)
		_gfx_buffer_bind(gfx);
}

void	gfx_append(gfx_t *gfx)
{
	if (gfx->buffers[0].id) {
		_gfx_buffer_append(gfx);
	}
}

void	gfx_render(gfx_t *gfx)
{
	if (!gfx->render.count || gfx->render.count < 2)
		glDrawElements(
			gfx->render.primitives,
			gfx->render.indice_count,
			gfx->render.indice_type,
			(void*)(gfx->render.offset));
	else
		glDrawElementsInstanced(
			gfx->render.primitives,
			gfx->render.indice_count,
			gfx->render.indice_type,
			(void*)(gfx->render.offset),
			gfx->render.count);
}

static void	_gfx_apply_texuni(gfx_t *gfx)
{
	for (u32 i = 0; gfx->texture.uniform[i].id; i++) {
		glUniform1i(
			glGetUniformLocation(
				*gfx->shader.id, 
				gfx->texture.uniform[i].id), 
				gfx->texture.uniform[i].bind_point);
	}
}

static void _gfx_apply_uniform(gfx_t *gfx)
{
	for (u32 i = 0; gfx->uniform[i].id; i++) {

	}
}

void	gfx_apply(gfx_t *gfx)
{
	if (gfx->shader.id)
		_gfx_shader_use(gfx);

	if (gfx->texture.uniform[0].id)
		_gfx_apply_texuni(gfx);

	if (gfx->uniform[0].id)
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

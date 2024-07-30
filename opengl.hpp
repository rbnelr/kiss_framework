#pragma once
#include "common.hpp"

#include "glad/glad.h"
#include "tracy/TracyOpenGL.hpp"

#include "camera.hpp"

#include "kisslib/stb_image_write.hpp"

namespace ogl {
//
//// MISC stuff
//

#if OGL_USE_REVERSE_DEPTH
//// Use reverse depth to fix depth precision issues if possible
// requires float depth buffer and glClipControl
// https://nlguillemot.wordpress.com/2016/12/07/reversed-z-in-opengl/
inline bool reverse_depth = false;
#endif

inline float max_aniso = 1.0f;

inline void glSetEnable (GLenum cap, bool enabled) {
	if (enabled)
		glEnable(cap);
	else
		glDisable(cap);
}

#if RENDERER_DEBUG_LABELS

// glObjectLabel and glPush/PopDebugGroup core in 4.3
inline void set_object_label (GLenum type, GLuint handle, std::string_view label) {
	if (glObjectLabel) glObjectLabel(type, handle, (GLsizei)label.size(), label.data());
}

struct _ScopedGpuTrace {
	inline _ScopedGpuTrace (std::string_view name) {
		if (glPushDebugGroup) glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, (GLsizei)name.size(), name.data());
	}
	inline ~_ScopedGpuTrace () {
		if (glPopDebugGroup) glPopDebugGroup();
	}
};

#define OGL_DBG_LABEL(type, handle, label) ogl::set_object_label(type, handle, label)
#define OGL_TRACE(name) \
		ogl::_ScopedGpuTrace __scoped_##__COUNTER__ (name); \
		TracyGpuZone(name)

#else
#define OGL_DBG_LABEL(type, handle, label)
#define OGL_TRACE(name) TracyGpuZone(name)
#endif

typedef struct {
	uint32_t  count;
	uint32_t  instanceCount;
	uint32_t  first;
	uint32_t  baseInstance;
} glDrawArraysIndirectCommand;
typedef struct {
	uint32_t  count;
	uint32_t  instanceCount;
	uint32_t  firstIndex;
	int32_t   baseVertex;
	uint32_t  baseInstance;
} glDrawElementsIndirectCommand;

template <typename IDX_T> inline constexpr GLenum get_gl_idx_type ();
template<> inline constexpr GLenum get_gl_idx_type<uint16_t> () { return GL_UNSIGNED_SHORT; }
template<> inline constexpr GLenum get_gl_idx_type<uint32_t> () { return GL_UNSIGNED_INT; }

void APIENTRY debug_callback (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char* message, void const* userParam);

inline const char* enum2str (GLenum enum_val) {
	// Add to here as needed
	switch (enum_val) {
		case GL_BOOL        : return "GL_BOOL";
		case GL_FLOAT       : return "GL_FLOAT";
		case GL_FLOAT_VEC2  : return "GL_FLOAT_VEC2";
		case GL_FLOAT_VEC3  : return "GL_FLOAT_VEC3";
		case GL_FLOAT_VEC4  : return "GL_FLOAT_VEC4";
		case GL_INT         : return "GL_INT";
		case GL_UNSIGNED_INT: return "GL_UNSIGNED_INT";
		case GL_INT_VEC2    : return "GL_INT_VEC2";
		case GL_INT_VEC3    : return "GL_INT_VEC3";
		case GL_INT_VEC4    : return "GL_INT_VEC4";
		case GL_FLOAT_MAT2  : return "GL_FLOAT_MAT2";
		case GL_FLOAT_MAT3  : return "GL_FLOAT_MAT3";
		case GL_FLOAT_MAT4  : return "GL_FLOAT_MAT4";

		default: return "<unknown>";
	}
}

//
//// Shader manager
//
namespace shader {
	struct Uniform {
		std::string	name;
		GLenum		type;
		GLint		location;
	};

	//typedef ordered_map<std::string, ShaderUniform> uniform_set;
	typedef std::vector<Uniform> uniform_set;

	inline void _check_uniform (Uniform& u, GLenum passed) {
		if (u.type == passed) return;
		if (passed == GL_INT && (u.type == GL_SAMPLER_1D || u.type == GL_SAMPLER_2D || u.type == GL_SAMPLER_3D)) return;
		fprintf(stderr, "ogl::set_uniform type mismatch: uniform: %s[%x], passed: %s[%x]\n", enum2str(u.type), u.type, enum2str(passed), passed);
		assert(false);
	}

	inline void _set_uniform (Uniform& u, float    const* values, int arr_count) { _check_uniform(u, GL_FLOAT        ); glUniform1fv(u.location, arr_count, values); }
	inline void _set_uniform (Uniform& u, float2   const* values, int arr_count) { _check_uniform(u, GL_FLOAT_VEC2   ); glUniform2fv(u.location, arr_count, &values->x); }
	inline void _set_uniform (Uniform& u, float3   const* values, int arr_count) { _check_uniform(u, GL_FLOAT_VEC3   ); glUniform3fv(u.location, arr_count, &values->x); }
	inline void _set_uniform (Uniform& u, float4   const* values, int arr_count) { _check_uniform(u, GL_FLOAT_VEC4   ); glUniform4fv(u.location, arr_count, &values->x); }
	inline void _set_uniform (Uniform& u, int      const* values, int arr_count) { _check_uniform(u, GL_INT          ); glUniform1iv(u.location, arr_count, values); }
	inline void _set_uniform (Uniform& u, uint32_t const* values, int arr_count) { _check_uniform(u, GL_UNSIGNED_INT ); glUniform1uiv(u.location, arr_count, values); }
	inline void _set_uniform (Uniform& u, int2     const* values, int arr_count) { _check_uniform(u, GL_INT_VEC2     ); glUniform2iv(u.location, arr_count, &values->x); }
	inline void _set_uniform (Uniform& u, int3     const* values, int arr_count) { _check_uniform(u, GL_INT_VEC3     ); glUniform3iv(u.location, arr_count, &values->x); }
	inline void _set_uniform (Uniform& u, int4     const* values, int arr_count) { _check_uniform(u, GL_INT_VEC4     ); glUniform4iv(u.location, arr_count, &values->x); }
	inline void _set_uniform (Uniform& u, float2x2 const* values, int arr_count) { _check_uniform(u, GL_FLOAT_MAT2   ); glUniformMatrix2fv(u.location, arr_count, GL_FALSE, &values->arr[0].x); }
	inline void _set_uniform (Uniform& u, float3x3 const* values, int arr_count) { _check_uniform(u, GL_FLOAT_MAT3   ); glUniformMatrix3fv(u.location, arr_count, GL_FALSE, &values->arr[0].x); }
	inline void _set_uniform (Uniform& u, float4x4 const* values, int arr_count) { _check_uniform(u, GL_FLOAT_MAT4   ); glUniformMatrix4fv(u.location, arr_count, GL_FALSE, &values->arr[0].x); }

	inline void _set_uniform (Uniform& u, bool     const& val) { _check_uniform(u, GL_BOOL         ); glUniform1i(u.location, val ? GL_TRUE : GL_FALSE); }
	inline void _set_uniform (Uniform& u, float    const& val) { _check_uniform(u, GL_FLOAT        ); glUniform1f(u.location, val); }
	inline void _set_uniform (Uniform& u, float2   const& val) { _check_uniform(u, GL_FLOAT_VEC2   ); glUniform2fv(u.location, 1, &val.x); }
	inline void _set_uniform (Uniform& u, float3   const& val) { _check_uniform(u, GL_FLOAT_VEC3   ); glUniform3fv(u.location, 1, &val.x); }
	inline void _set_uniform (Uniform& u, float4   const& val) { _check_uniform(u, GL_FLOAT_VEC4   ); glUniform4fv(u.location, 1, &val.x); }
	inline void _set_uniform (Uniform& u, int      const& val) { _check_uniform(u, GL_INT          ); glUniform1i(u.location, val); }
	inline void _set_uniform (Uniform& u, uint32_t const& val) { _check_uniform(u, GL_UNSIGNED_INT ); glUniform1ui(u.location, val); }
	inline void _set_uniform (Uniform& u, int2     const& val) { _check_uniform(u, GL_INT_VEC2     ); glUniform2iv(u.location, 1, &val.x); }
	inline void _set_uniform (Uniform& u, int3     const& val) { _check_uniform(u, GL_INT_VEC3     ); glUniform3iv(u.location, 1, &val.x); }
	inline void _set_uniform (Uniform& u, int4     const& val) { _check_uniform(u, GL_INT_VEC4     ); glUniform4iv(u.location, 1, &val.x); }
	inline void _set_uniform (Uniform& u, float2x2 const& val) { _check_uniform(u, GL_FLOAT_MAT2   ); glUniformMatrix2fv(u.location, 1, GL_FALSE, &val.arr[0].x); }
	inline void _set_uniform (Uniform& u, float3x3 const& val) { _check_uniform(u, GL_FLOAT_MAT3   ); glUniformMatrix3fv(u.location, 1, GL_FALSE, &val.arr[0].x); }
	inline void _set_uniform (Uniform& u, float4x4 const& val) { _check_uniform(u, GL_FLOAT_MAT4   ); glUniformMatrix4fv(u.location, 1, GL_FALSE, &val.arr[0].x); }

	inline bool _findUniform (Uniform const& l, std::string_view const& r) { return l.name == r; }
	
	enum Stage {
		VERTEX_SHADER,
		FRAGMENT_SHADER,
		COMPUTE_SHADER,
		GEOMETRY_SHADER,
	};

	inline constexpr GLenum SHADER_STAGE_GLENUM[] = {
		GL_VERTEX_SHADER,
		GL_FRAGMENT_SHADER,
		GL_COMPUTE_SHADER,
		GL_GEOMETRY_SHADER,
	};
	inline constexpr const char* SHADER_STAGE_NAME[] = {
		"vertex",
		"fragment",
		"compute",
		"geometry",
	};
	inline constexpr const char* SHADER_STAGE_MACRO[] = {
		"_VERTEX",
		"_FRAGMENT",
		"_COMPUTE",
		"_GEOMETRY",
	};

	struct MacroDefinition {
		std::string name;
		std::string value;
	};

	struct Shader;
	bool compile_shader (Shader& shad, const char* name, const char* dbgname,
			std::vector<Stage> const& stages, std::vector<MacroDefinition> const& macros, bool wireframe);

	struct Shader {
		GLuint                          prog = 0;
	
		std::string                     name;
		std::string                     dbgname;

		std::vector<Stage>        stages;
		std::vector<MacroDefinition>    macros;
	
		uniform_set                     uniforms;
		std::vector<std::string>        src_files;
	
		~Shader () {
			if (prog) glDeleteProgram(prog);
			prog = 0;
		}
	
		inline bool recompile (bool wireframe) {
			Shader tmp;

			bool success = compile_shader(tmp, name.c_str(), dbgname.c_str(), stages, macros, wireframe);
		
			if (success) {
				// success, delete old shader
				if (prog) glDeleteProgram(prog);
				// replace with new shader
				prog = tmp.prog;
				tmp.prog = 0;

				uniforms = std::move(tmp.uniforms);
				src_files = std::move(tmp.src_files);
				return true;
			} else {
				// fail, keep old shader
				if (tmp.prog) glDeleteProgram(tmp.prog);
				return false;
			}

			return success;
		}
	
		inline GLint get_uniform_location (std::string_view const& name) {
			int i = indexof(uniforms, name, _findUniform);
			if (i < 0) return i;
			return uniforms[i].location;
		}
	
		template <typename T>
		inline void set_uniform (std::string_view const& name, T const& val) {
			int i = indexof(uniforms, name, _findUniform);
			if (i >= 0)
				_set_uniform(uniforms[i], val);
		}

		// WARNING: Opengl for whatever reason decided that you need to specify "uniform_name[0]" for uniform arrays
		// Don't let yourself be confused by this again!
		template <typename T>
		inline void set_uniform_array (std::string_view const& name, T const* values, int arr_count) {
			int i = indexof(uniforms, name, _findUniform);
			if (i >= 0)
				_set_uniform(uniforms[i], values, arr_count);
		}
	};

	struct Shaders {
		// Shaders are owned by global Shaders objects
		// if users no longer use shader it still sticks around until the end of the program for simplicity
		std::vector<std::unique_ptr<Shader>> shaders;
	
		bool wireframe = false;
		
		inline void shutdown () {
			ZoneScoped;
			shaders.clear();
		}
	
		inline bool update_recompilation (ChangedFiles& changed_files) {
			bool success = true;
		
			if (changed_files.any()) {
				for (auto& s : shaders) {
					std::string const* changed_file;
					if (changed_files.contains_any(s->src_files, FILE_ADDED|FILE_MODIFIED|FILE_RENAMED_NEW_NAME, &changed_file)) {
						// any source file was changed
						fprintf(stdout, "[Shaders] Recompile shader %-35s due to shader source change (%s)\n", s->name.c_str(), changed_file->c_str());
						if (!s->recompile(wireframe))
							success = false; // any failed shader signals reload fail
					}
				}
			}
		
			return success;
		}
		inline bool set_wireframe (bool wireframe) {
			bool success = true;
		
			if (this->wireframe != wireframe) {
				this->wireframe = wireframe;
			
				for (auto& s : shaders) {
					fprintf(stdout, "[Shaders] Recompile shader %-35s due to wireframe toggle\n", s->name.c_str());
					s->recompile(wireframe);
				}
			}
		
			return success;
		}
	
		inline Shader* compile (char const* name,
				std::vector<MacroDefinition>&& macros = {},
				std::initializer_list<Stage> stages = { VERTEX_SHADER, FRAGMENT_SHADER },
				char const* dbgname=nullptr) {
			ZoneScoped;
		
			auto s = std::make_unique<Shader>();
			s->name = name;
			s->dbgname = dbgname ? dbgname : name;
			s->stages = stages;
			s->macros = std::move(macros);

			bool success = compile_shader(*s, s->name.c_str(), s->dbgname.c_str(), s->stages, s->macros, wireframe);
		
			auto* ptr = s.get();
			// remember shader regardless of compilation success to allow for shaders with errors
			// to be fixed using update_recompilation()
			shaders.push_back(std::move(s));
		
			return ptr;
		}
	};
}
using shader::Shader;
using shader::Stage;

inline shader::Shaders g_shaders;

//
//// simple gl resource wrappers to avoid writing destructors manually all the time
//
class Vbo {
	GLuint vbo = 0;
public:
	MOVE_ONLY_CLASS_MEMBER(Vbo, vbo);

	Vbo () {} // not allocated
	Vbo (std::string_view label) { // allocate
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		OGL_DBG_LABEL(GL_BUFFER, vbo, label);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	~Vbo () {
		if (vbo)
			glDeleteBuffers(1, &vbo);
	}

	operator GLuint () const { return vbo; }
};
class Ebo {
	GLuint ebo = 0;
public:
	MOVE_ONLY_CLASS_MEMBER(Ebo, ebo);

	Ebo () {} // not allocated
	Ebo (std::string_view label) { // allocate
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		OGL_DBG_LABEL(GL_BUFFER, ebo, label);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	~Ebo () {
		if (ebo) glDeleteBuffers(1, &ebo);
	}

	operator GLuint () const { return ebo; }
};
class Ubo {
	GLuint ubo = 0;
public:
	MOVE_ONLY_CLASS_MEMBER(Ubo, ubo);

	Ubo () {} // not allocated
	Ubo (std::string_view label) { // allocate
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		OGL_DBG_LABEL(GL_BUFFER, ubo, label);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	~Ubo () {
		if (ubo) glDeleteBuffers(1, &ubo);
	}

	operator GLuint () const { return ubo; }
};
class Vao {
	GLuint vao = 0;
public:
	MOVE_ONLY_CLASS_MEMBER(Vao, vao);

	Vao () {} // not allocated
	Vao (std::string_view label) { // allocate
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		OGL_DBG_LABEL(GL_VERTEX_ARRAY, vao, label);
		glBindVertexArray(0);
	}
	~Vao () {
		if (vao) glDeleteVertexArrays(1, &vao);
	}

	operator GLuint () const { return vao; }
};
class Ssbo {
	GLuint ssbo = 0;
public:
	MOVE_ONLY_CLASS_MEMBER(Ssbo, ssbo);

	Ssbo () {} // not allocated
	Ssbo (std::string_view label) { // allocate
		glGenBuffers(1, &ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		OGL_DBG_LABEL(GL_BUFFER, ssbo, label);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
	~Ssbo () {
		if (ssbo) glDeleteBuffers(1, &ssbo);
	}

	operator GLuint () const { return ssbo; }
};
class Sampler {
	GLuint sampler = 0;
public:
	MOVE_ONLY_CLASS_MEMBER(Sampler, sampler);

	Sampler () {} // not allocated
	Sampler (std::string_view label) { // allocate
		glGenSamplers(1, &sampler);
		glBindSampler(0, sampler);
		OGL_DBG_LABEL(GL_SAMPLER, sampler, label);
		glBindSampler(0, 0);
	}
	~Sampler () {
		if (sampler) glDeleteSamplers(1, &sampler);
	}

	operator GLuint () const { return sampler; }
};
class Texture1D {
	GLuint tex = 0;
public:
	MOVE_ONLY_CLASS_MEMBER(Texture1D, tex);

	Texture1D () {} // not allocated
	Texture1D (std::string_view label) { // allocate
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_1D, tex);
		OGL_DBG_LABEL(GL_TEXTURE, tex, label);
		glBindTexture(GL_TEXTURE_1D, 0);
	}
	~Texture1D () {
		if (tex) glDeleteTextures(1, &tex);
	}

	operator GLuint () const { return tex; }
};
class Texture1DArray {
	GLuint tex = 0;
public:
	MOVE_ONLY_CLASS_MEMBER(Texture1DArray, tex);

	Texture1DArray () {} // not allocated
	Texture1DArray (std::string_view label) { // allocate
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_1D_ARRAY, tex);
		OGL_DBG_LABEL(GL_TEXTURE, tex, label);
		glBindTexture(GL_TEXTURE_1D_ARRAY, 0);
	}
	~Texture1DArray () {
		if (tex) glDeleteTextures(1, &tex);
	}

	operator GLuint () const { return tex; }
};
class Texture2D {
	GLuint tex = 0;
public:
	MOVE_ONLY_CLASS_MEMBER(Texture2D, tex);

	Texture2D () {} // not allocated
	Texture2D (std::string_view label) { // allocate
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		OGL_DBG_LABEL(GL_TEXTURE, tex, label);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	~Texture2D () {
		if (tex) glDeleteTextures(1, &tex);
	}

	operator GLuint () const { return tex; }
};
class Texture3D {
	GLuint tex = 0;
public:
	MOVE_ONLY_CLASS_MEMBER(Texture3D, tex);

	Texture3D () {} // not allocated
	Texture3D (std::string_view label) { // allocate
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_3D, tex);
		OGL_DBG_LABEL(GL_TEXTURE, tex, label);
		glBindTexture(GL_TEXTURE_3D, 0);
	}
	~Texture3D () {
		if (tex) glDeleteTextures(1, &tex);
	}

	operator GLuint () const { return tex; }
};
class Texture2DArray {
	GLuint tex = 0;
public:
	MOVE_ONLY_CLASS_MEMBER(Texture2DArray, tex);

	Texture2DArray () {} // not allocated
	Texture2DArray (std::string_view label) { // allocate
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D_ARRAY, tex);
		OGL_DBG_LABEL(GL_TEXTURE, tex, label);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}
	~Texture2DArray () {
		if (tex) glDeleteTextures(1, &tex);
	}

	operator GLuint () const { return tex; }
};
class TextureCubemap {
	GLuint tex = 0;
public:
	MOVE_ONLY_CLASS_MEMBER(TextureCubemap, tex);

	TextureCubemap () {} // not allocated
	TextureCubemap (std::string_view label) { // allocate
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
		OGL_DBG_LABEL(GL_TEXTURE, tex, label);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
	~TextureCubemap () {
		if (tex) glDeleteTextures(1, &tex);
	}

	operator GLuint () const { return tex; }
};
class Fbo {
	GLuint fbo = 0;
public:
	MOVE_ONLY_CLASS_MEMBER(Fbo, fbo);

	Fbo () {} // not allocated
	Fbo (std::string_view label) { // allocate
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		OGL_DBG_LABEL(GL_FRAMEBUFFER, fbo, label);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	~Fbo () {
		if (fbo) glDeleteFramebuffers(1, &fbo);
	}

	operator GLuint () const { return fbo; }
};

inline Fbo make_and_bind_temp_fbo (GLuint textarget, GLuint tex, int mip) {
	auto fbo = Fbo("tmp_fbo");
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textarget, tex, mip);
	GLuint bufs[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(ARRLEN(bufs), bufs);
		
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		//fprintf(stderr, "glCheckFramebufferStatus: %x\n", status);
		assert(false);
	}
	return fbo;
}
inline Fbo make_and_bind_temp_fbo_layered (GLuint tex, int mip) {
	auto fbo = Fbo("tmp_fbo");
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// attaches entire eg. cubemap as "layered image" which using a geometry shader, can be draw to in a single drawcall
	// NOTE: glFramebufferTextureLayer seems to attach a particular layer of the cubemap as single image, just like glFramebufferTexture2D(.., GL_TEXTURE_CUBE_MAP_POSITIVE_X, ...)
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex, mip);
	GLuint bufs[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(ARRLEN(bufs), bufs);
		
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		//fprintf(stderr, "glCheckFramebufferStatus: %x\n", status);
		assert(false);
	}
	return fbo;
}

/*
// WARNING: This is ungodly slow for whatever reason, like 1.5ms to copy some cubemap mipmaps
// while generating them using 512 samples per texel with math and texture access takes ~2ms
inline void copy_texels (GLenum textarget,
		GLuint src, GLint src_mip, int2 srcXY0, int2 srcXY1,
		GLuint dst, GLint dst_mip, int2 dstXY0, int2 dstXY1,
		GLbitfield mask=GL_COLOR_BUFFER_BIT, GLenum filter=GL_NEAREST) {
	auto src_fbo = make_and_bind_temp_fbo(textarget, src, src_mip);
	auto dst_fbo = make_and_bind_temp_fbo(textarget, dst, dst_mip);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, src_fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst_fbo);

	glBlitFramebuffer(
		srcXY0.x,srcXY0.y, srcXY1.x,srcXY1.y,
		dstXY0.x,dstXY0.y, dstXY1.x,dstXY1.y,
		mask, filter);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}*/

inline void dispatch_compute (int3 count, int3 workgroup_size) {
	int3 dispatch_size = (count + workgroup_size-1) / workgroup_size; // round up
	glDispatchCompute((GLuint)dispatch_size.x, (GLuint)dispatch_size.y, (GLuint)dispatch_size.z);
}

//
//// VBO helper functions and classes
//

// upload data to buffer once via pointer
inline void upload_buffer (GLenum target, GLuint buf, size_t size, void const* data, GLenum usage = GL_STATIC_DRAW) {
	glBindBuffer(target, buf);

	glBufferData(target, size, data, usage);

	glBindBuffer(target, 0);
}
// upload data to buffer via pointer in a streaming way (buffer orphaning)
inline void stream_buffer (GLenum target, GLuint buf, size_t size, void const* data, GLenum usage = GL_STREAM_DRAW) {
	glBindBuffer(target, buf);

	glBufferData(target, size, nullptr, usage);
	if (size > 0)
		glBufferData(target, size, data, usage);

	glBindBuffer(target, 0);
}

// Non-indexed (VAO + VBO) with uploading functions (vao configured externally)
struct VertexBuffer {
	Vao vao;
	Vbo vbo;
	VertexBuffer (std::string_view label): vao{label + ".vao"}, vbo{label + ".vbo"} {}
	
	template <typename VT> void upload (VT const* vertices, size_t vertex_count) {
		upload_buffer(GL_ARRAY_BUFFER, vbo, (GLsizeiptr)vertex_count * sizeof(VT), vertices, GL_STATIC_DRAW);
	}
	template <typename VT> void stream (VT const* vertices, size_t vertex_count) {
		stream_buffer(GL_ARRAY_BUFFER, vbo, (GLsizeiptr)vertex_count * sizeof(VT), vertices, GL_STREAM_DRAW);
	}

	template <typename VT> void upload (std::vector<VT> const& vertices) {
		upload(vertices.data(), vertices.size());
	}
	template <typename VT> void stream (std::vector<VT> const& vertices) {
		stream(vertices.data(), vertices.size());
	}
};
// Indexed (VAO + VBO + EBO) with uploading functions (vao configured externally)
struct VertexBufferI {
	Vao vao;
	Vbo vbo;
	Ebo ebo;
	VertexBufferI (std::string_view label): vao{label + ".vao"}, vbo{label + ".vbo"}, ebo{label + ".ebo"} {}
	
	template <typename VT, typename IT> void upload (VT const* vertices, size_t vertex_count, IT const* indices, size_t index_count) {
		upload_buffer(GL_ARRAY_BUFFER        , vbo, (GLsizeiptr)vertex_count * sizeof(VT), vertices, GL_STATIC_DRAW);
		upload_buffer(GL_ELEMENT_ARRAY_BUFFER, ebo, (GLsizeiptr)index_count  * sizeof(IT), indices , GL_STATIC_DRAW);
	}
	template <typename VT, typename IT> void stream (VT const* vertices, size_t vertex_count, IT const* indices, size_t index_count) {
		stream_buffer(GL_ARRAY_BUFFER        , vbo, (GLsizeiptr)vertex_count * sizeof(VT), vertices, GL_STREAM_DRAW);
		stream_buffer(GL_ELEMENT_ARRAY_BUFFER, ebo, (GLsizeiptr)index_count  * sizeof(IT), indices , GL_STREAM_DRAW);
	}

	template <typename VT, typename IT> void upload (std::vector<VT> const& vertices, std::vector<IT> const& indices) {
		upload(vertices.data(), vertices.size(), indices.data(), indices.size());
	}
	template <typename VT, typename IT> void stream (std::vector<VT> const& vertices, std::vector<IT> const& indices) {
		stream(vertices.data(), vertices.size(), indices.data(), indices.size());
	}
};

// Non-indexed and instanced (VAO + mesh VBO + instances VBO) with uploading functions (vao configured externally)
struct VertexBufferInstanced {
	Vao vao;
	Vbo vbo;
	Vbo instances;
	VertexBufferInstanced (std::string_view label): vao{label + ".vao"}, vbo{label + ".vbo"}, instances{label + ".instances.vbo"} {}
	
	template <typename VT> void upload_mesh (VT const* vertices, size_t vertex_count) {
		upload_buffer(GL_ARRAY_BUFFER, vbo, (GLsizeiptr)vertex_count * sizeof(VT), vertices, GL_STATIC_DRAW);
	}
	template <typename IT> void stream_instances (IT const* instance_data, size_t vertex_count) {
		stream_buffer(GL_ARRAY_BUFFER, instances, (GLsizeiptr)vertex_count * sizeof(IT), instance_data, GL_STREAM_DRAW);
	}

	template <typename VT> void upload_mesh (std::vector<VT> const& vertices) {
		upload_mesh(vertices.data(), vertices.size());
	}
	template <typename IT> void stream_instances (std::vector<IT> const& instance_data) {
		stream_instances(instance_data.data(), instance_data.size());
	}
};
// Indexed and instanced (VAO + mesh VBO + mesh EBO + instances VBO) with uploading functions (vao configured externally)
struct VertexBufferInstancedI {
	Vao vao;
	Vbo vbo;
	Ebo ebo;
	Vbo instances;
	VertexBufferInstancedI (std::string_view label): vao{label + ".vao"}, vbo{label + ".vbo"}, ebo{label + ".ebo"}, instances{label + ".instances.vbo"} {}

	template <typename VT, typename IT> void upload_mesh (VT const* vertices, size_t vertex_count, IT const* indices, size_t index_count) {
		upload_buffer(GL_ARRAY_BUFFER        , vbo, (GLsizeiptr)vertex_count * sizeof(VT), vertices, GL_STATIC_DRAW);
		upload_buffer(GL_ELEMENT_ARRAY_BUFFER, ebo, (GLsizeiptr)index_count  * sizeof(IT),  indices, GL_STATIC_DRAW);
	}
	template <typename IT> void stream_instances (IT const* instance_data, size_t vertex_count) {
		stream_buffer(GL_ARRAY_BUFFER, instances, (GLsizeiptr)vertex_count * sizeof(IT), instance_data, GL_STREAM_DRAW);
	}
	
	template <typename VT, typename IT> void upload_mesh (std::vector<VT> const& vertices, std::vector<IT> const& indices) {
		upload_mesh(vertices.data(), vertices.size(), indices.data(), indices.size());
	}
	template <typename IT> void stream_instances (std::vector<IT> const& instance_data) {
		stream_instances(instance_data.data(), instance_data.size());
	}
};

//// VAO Attribute creation helper using Generic Render VERTEX_CONFIG
struct GLAttrib {
	GLenum scalar_type; // what (scalar) type is the data in
	bool int_in_shader; // should it be read as int in vertex shader? (can convert int data to float)
	bool normalized;    // should int be normalized to 0.0-1.0 range? (when int data is read as float)
};
static constexpr inline GLAttrib ATTRIB_GL_TYPES[] = {
	{ GL_FLOAT,         false, false },
	{ GL_INT,           true, false },
	{ GL_UNSIGNED_BYTE, true, false },
};

template <size_t N>
inline int setup_vao_attribs (render::VertexAttributes<N> const& attribs, int idx, int instance_divisor=0, size_t base_offs=0) {
	using namespace render;

	for (auto const& a : attribs.attribs) {
		auto& gltype = ATTRIB_GL_TYPES[a.type];

		glEnableVertexAttribArray(idx);
		
		if (instance_divisor > 0) {
			glVertexAttribDivisor(idx, instance_divisor);
		}

		assert(a.stride > 0);
		assert(a.components > 0 && a.components <= 4);

		if (gltype.int_in_shader) {
			glVertexAttribIPointer(idx, a.components, gltype.scalar_type,
				a.stride, (void*)((size_t)a.offset + base_offs));
		}
		else {
			glVertexAttribPointer(idx, a.components, gltype.scalar_type, gltype.normalized,
				a.stride, (void*)((size_t)a.offset + base_offs));
		}

		idx++;
	}

	return idx;
}

// Create VAO from Vertex::_attribs and existing VBO and optional existing EBO (Generic Render VERTEX_CONFIG)
// vbo_base_offs is offset from start of VBO memory (usually 0), useful for packing mixed vertex info into same VBO
template <size_t N>
inline void setup_vao (render::VertexAttributes<N> const& attribs, GLuint vao, GLuint vbo, GLuint ebo=0, size_t vbo_base_offs=0) {
	glBindVertexArray(vao);
	if (ebo) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	setup_vao_attribs(attribs, 0, 0, vbo_base_offs);
	glBindBuffer(GL_ARRAY_BUFFER, 0); // glVertexAttribPointer remembers VAO

	glBindVertexArray(0);
	if (ebo) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // VAO remembers EBO (note that we need to unbind VAO first)
}

// Create Non-indexed VBO (with VAO) for this Vertex config
template <typename VERTEX>
inline VertexBuffer vertex_buffer (std::string_view label) {
	VertexBuffer buf = {label};
	setup_vao(VERTEX::attribs(), buf.vao, buf.vbo);
	return buf;
}
// Create Indexed VBO (with EBO and VAO) for this Vertex config
template <typename VERTEX>
inline VertexBufferI vertex_bufferI (std::string_view label) {
	VertexBufferI buf = {label};
	setup_vao(VERTEX::attribs(), buf.vao, buf.vbo, buf.ebo);
	return buf;
}

// Create Non-indexed, Instanced VBO (with VAO) for this Vertex config
template <typename MESH_VERTEX, typename INSTANCE_VERTEX>
inline VertexBufferInstanced vertex_buffer_instanced (std::string_view label) {
	VertexBufferInstanced buf = {label};
	
	glBindVertexArray(buf.vao);

	glBindBuffer(GL_ARRAY_BUFFER, buf.vbo);
	int idx = setup_vao_attribs(MESH_VERTEX::attribs(), 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, buf.instances);
	    idx = setup_vao_attribs(INSTANCE_VERTEX::attribs(), idx, 1);
	glBindBuffer(GL_ARRAY_BUFFER, 0); // glVertexAttribPointer remembers VAO

	glBindVertexArray(0);

	return buf;
}
// Create Indexed, Instanced VBO (with EBO and VAO) for this Vertex config
template <typename MESH_VERTEX, typename INSTANCE_VERTEX>
inline VertexBufferInstancedI vertex_buffer_instancedI (std::string_view label) {
	VertexBufferInstancedI buf = {label};
	
	glBindVertexArray(buf.vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.ebo);

	glBindBuffer(GL_ARRAY_BUFFER, buf.vbo);
	int idx = setup_vao_attribs(MESH_VERTEX::attribs(), 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, buf.instances);
	    idx = setup_vao_attribs(INSTANCE_VERTEX::attribs(), idx, 1);
	glBindBuffer(GL_ARRAY_BUFFER, 0); // glVertexAttribPointer remembers VAO

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // VAO remembers EBO (note that we need to unbind VAO first)

	return buf;
}

inline void _debug_vao (std::string msg="") {

	msg.append( " ... querying VAO state:\n" );
	int vab, eabb, eabbs=0, mva, isOn = 1, vaabb;
	
	glGetIntegerv( GL_VERTEX_ARRAY_BINDING, &vab );
	glGetIntegerv( GL_ELEMENT_ARRAY_BUFFER_BINDING, &eabb );
	if (eabb)
		glGetBufferParameteriv( GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &eabbs );

	msg.append( "  VAO: " + std::to_string( vab ) + "\n" );
	msg.append( "  IBO: " + std::to_string( eabb ) + ", size=" + std::to_string( eabbs )  + "\n" );

	glGetIntegerv( GL_MAX_VERTEX_ATTRIBS, &mva );
	for (int i=0; i<mva; ++i) {
		glGetVertexAttribiv( i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &isOn );
		if (isOn) {
			glGetVertexAttribiv( i, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &vaabb );
			msg.append( "  attrib #" + std::to_string( i ) + ": VBO=" + std::to_string( vaabb ) + "\n" );
		}
	}

	puts(msg.c_str());
}

//
//// Workaround to fix opengl global state
//
namespace state {
	struct TextureBind {
		struct _Texture {
			GLenum type;
			GLuint tex;

			_Texture (GLenum type, GLuint tex): type{type}, tex{tex} {}

			_Texture (): type{0}, tex{0} {}
			_Texture (Texture1D      const& tex): type{ GL_TEXTURE_1D       }, tex{tex} {}
			_Texture (Texture1DArray const& tex): type{ GL_TEXTURE_1D_ARRAY }, tex{tex} {}
			_Texture (Texture2D      const& tex): type{ GL_TEXTURE_2D       }, tex{tex} {}
			_Texture (Texture3D      const& tex): type{ GL_TEXTURE_3D       }, tex{tex} {}
			_Texture (Texture2DArray const& tex): type{ GL_TEXTURE_2D_ARRAY }, tex{tex} {}
			_Texture (TextureCubemap const& tex): type{ GL_TEXTURE_CUBE_MAP }, tex{tex} {}
		};
		std::string_view	uniform_name;
		_Texture			tex;
		GLuint				sampler;

		TextureBind (): uniform_name{}, tex{} {} // null -> empty texture unit

		// allow no null sampler
		TextureBind (std::string_view uniform_name, _Texture const& tex)
			: uniform_name{uniform_name}, tex{tex}, sampler{0} {}
		TextureBind (std::string_view uniform_name, _Texture const& tex, Sampler const& sampl)
			: uniform_name{uniform_name}, tex{tex}, sampler{sampl} {}
	};

	struct TextureBinds {
		std::vector<TextureBind> vec;
		
		TextureBinds& operator+= (TextureBind const& r) {
			vec.push_back(r);
			return *this;
		}
		TextureBinds& operator+= (TextureBinds const& r) {
			vec.insert(vec.end(), r.vec.begin(), r.vec.end());
			return *this;
		}

		friend TextureBinds operator+ (TextureBinds const& l, TextureBinds const& r) {
			TextureBinds b = l;
			b += r;
			return b;
		}
		friend TextureBinds operator+ (TextureBinds const& l, TextureBind const& r) {
			TextureBinds b = l;
			b += r;
			return b;
		}
	};

	enum DepthFunc {
		DEPTH_INFRONT, // normal: draw infront (or equal depth) of other things
		DEPTH_BEHIND, // inverted: draw behind other things
	};
	inline GLenum map_depth_func (DepthFunc func) {
		switch (func) {
			case DEPTH_INFRONT:	return GL_LEQUAL;
			case DEPTH_BEHIND:	return GL_GEQUAL;
			default: return 0;
		}
	}
	#if OGL_USE_REVERSE_DEPTH
	inline GLenum map_depth_func_reverse (DepthFunc func) {
		switch (func) {
			case DEPTH_INFRONT:	return GL_GEQUAL;
			case DEPTH_BEHIND:	return GL_LEQUAL;
			default: return 0;
		}
	}
	#endif

	enum CullFace {
		CULL_BACK,
		CULL_FRONT,
	};
	enum PrimitiveMode {
		PRIM_TRIANGELS=0,
		PRIM_LINES,
	};
	enum PolyMode {
		POLY_FILL=0,
		POLY_LINE,
	};

	struct BlendFunc {
		GLenum equation = GL_FUNC_ADD;
		GLenum sfactor = GL_SRC_ALPHA;
		GLenum dfactor = GL_ONE_MINUS_SRC_ALPHA;
	};

	struct PipelineState {
		bool depth_test = true;
		bool depth_write = true;
		bool depth_clamp = false;
		DepthFunc depth_func = DEPTH_INFRONT;

		bool scissor_test = false;

		bool cull_face = true;
		CullFace front_face = CULL_BACK;

		bool blend_enable = false;
		BlendFunc blend_func = BlendFunc();

		PolyMode poly_mode = POLY_FILL;
	};
	struct StateManager {
		PipelineState state;

		// overrides
		bool wireframe = false;
		bool wireframe_no_cull = false;
		bool wireframe_no_blend = false;
	
		Vao dummy_vao = {"dummy_vao"};

		PipelineState _override (PipelineState const& s) {
			PipelineState o = s;

			if (wireframe) {
				o.poly_mode = POLY_LINE;
				if (wireframe_no_cull)  o.cull_face = false;
				if (wireframe_no_blend) o.blend_enable = false;
			}

			return o;
		}

		void set_default () {
			auto s = _override( PipelineState() );

			glSetEnable(GL_DEPTH_TEST, s.depth_test);
			// 
		
	#if OGL_USE_REVERSE_DEPTH
			if (reverse_depth) {
				glDepthFunc(map_depth_func_reverse(s.depth_func));
				glClearDepth(0.0f);
				glDepthRange(0.0f, 1.0f); // do not flip like this, since precision will be bad, need to flip in projection matrix

				glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
			}
			else {
				glDepthFunc(map_depth_func(s.depth_func));
				glClearDepth(1.0f);
				glDepthRange(0.0f, 1.0f);

				glClipControl(GL_LOWER_LEFT, GL_NEGATIVE_ONE_TO_ONE);
			}
	#else
			glDepthFunc(map_depth_func(s.depth_func));
	#endif
			glDepthMask(s.depth_write ? GL_TRUE : GL_FALSE);
			glSetEnable(GL_DEPTH_CLAMP, s.depth_clamp);

			glSetEnable(GL_SCISSOR_TEST, s.scissor_test);

			// culling
			glSetEnable(GL_CULL_FACE, s.cull_face);
			glCullFace(s.front_face == CULL_FRONT ? GL_FRONT : GL_BACK);
			glFrontFace(GL_CCW);
			// blending
			glSetEnable(GL_BLEND, s.blend_enable);
			glBlendEquation(s.blend_func.equation);
			glBlendFunc(s.blend_func.sfactor, s.blend_func.dfactor);

			glPolygonMode(GL_FRONT_AND_BACK, s.poly_mode == POLY_FILL ? GL_FILL : GL_LINE);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			state = s;
		}

		// set opengl drawing state to a set of values, where only changes are applied
		// no overrides to not break fullscreen quads etc.
		void set_no_override (PipelineState const& s) {
		#if OGL_STATE_ASSERT
			{
				assert(state.depth_test == !!glIsEnabled(GL_DEPTH_TEST));
				GLint depth_func;		glGetIntegerv(GL_DEPTH_FUNC, &depth_func);
	#if OGL_USE_REVERSE_DEPTH
				assert((reverse_depth ? map_depth_func_reverse(state.depth_func) : map_depth_func(state.depth_func)) == depth_func);
	#else
				assert(map_depth_func(state.depth_func) == depth_func);
	#endif
				GLint depth_write;		glGetIntegerv(GL_DEPTH_WRITEMASK, &depth_write);
				assert(state.depth_write == !!depth_write);

				GLint depth_clamp;		glGetIntegerv(GL_DEPTH_CLAMP, &depth_clamp);
				assert(state.depth_clamp == !!depth_clamp);
		
				assert(state.scissor_test == !!glIsEnabled(GL_SCISSOR_TEST));
		
				bool cull_face = !!glIsEnabled(GL_CULL_FACE);
				assert(state.cull_face == cull_face);
				GLint front_face;		glGetIntegerv(GL_CULL_FACE_MODE, &front_face);
				assert((state.front_face == CULL_FRONT ? GL_FRONT : GL_BACK) == front_face);
		
				assert(state.blend_enable == !!glIsEnabled(GL_BLEND));
				GLint blend_eq;		glGetIntegerv(GL_BLEND_EQUATION, &blend_eq);
				assert(state.blend_func.equation == blend_eq);
				GLint blend_rgbs, blend_rgbd, blend_as, blend_ad;
				glGetIntegerv(GL_BLEND_SRC_RGB, &blend_rgbs);
				glGetIntegerv(GL_BLEND_DST_RGB, &blend_rgbd);
				glGetIntegerv(GL_BLEND_SRC_ALPHA, &blend_as);
				glGetIntegerv(GL_BLEND_DST_ALPHA, &blend_ad);
				assert(state.blend_func.sfactor == blend_rgbs && state.blend_func.sfactor == blend_as);
				assert(state.blend_func.dfactor == blend_rgbd && state.blend_func.dfactor == blend_ad);
		
				// WARNING: glGetIntegerv(GL_POLYGON_MODE) returns two values  [0]: front facing [1]: back facing
				GLint poly_mode[2] = {};		glGetIntegerv(GL_POLYGON_MODE, poly_mode);
				assert((state.poly_mode == POLY_FILL ? GL_FILL : GL_LINE) == poly_mode[0]); //  && poly_mode[0] == poly_mode[1] works on AMD, does not on NV, does NV even return 2 values like AMD? why is opengl so broken?
			}
		#endif

			if (state.depth_test != s.depth_test)
				glSetEnable(GL_DEPTH_TEST, s.depth_test);
			if (state.depth_func != s.depth_func) {
	#if OGL_USE_REVERSE_DEPTH
				if (reverse_depth) glDepthFunc(map_depth_func_reverse(state.depth_func));
				else               glDepthFunc(map_depth_func(state.depth_func));
	#else
				glDepthFunc(map_depth_func(state.depth_func));
	#endif
			}
			if (state.depth_write != s.depth_write)
				glDepthMask(s.depth_write ? GL_TRUE : GL_FALSE);

			if (state.depth_clamp != s.depth_clamp)
				glSetEnable(GL_DEPTH_CLAMP, s.depth_clamp);

			if (state.scissor_test != s.scissor_test)
				glSetEnable(GL_SCISSOR_TEST, s.scissor_test);

			if (state.cull_face != s.cull_face)
				glSetEnable(GL_CULL_FACE, s.cull_face);
			if (state.front_face != s.front_face)
				glCullFace(s.front_face == CULL_FRONT ? GL_FRONT : GL_BACK);

			// blending
			if (state.blend_enable != s.blend_enable)
				glSetEnable(GL_BLEND, s.blend_enable);
			if (state.blend_func.equation != s.blend_func.equation)
				glBlendEquation(s.blend_func.equation);
			if (state.blend_func.sfactor != s.blend_func.sfactor || state.blend_func.dfactor != s.blend_func.dfactor)
				glBlendFunc(s.blend_func.sfactor, s.blend_func.dfactor);

			if (state.poly_mode != s.poly_mode)
				glPolygonMode(GL_FRONT_AND_BACK, s.poly_mode == POLY_FILL ? GL_FILL : GL_LINE);

			state = s;
		}

		// set opengl drawing state to a set of values, where only changes are applied
		// override for wireframe etc. are applied to these
		void set (PipelineState const& s) {
			auto o = _override(s);
			set_no_override(o);
		}

	
		// assume every temporarily bound texture is unbound again
		// (by other texture uploading code etc.)

		std::vector<GLenum> bound_texture_types;

		// bind a set of textures into texture units
		// and assign them to shader uniform samplers
		void bind_textures (Shader* shad, TextureBind const* textures, size_t count) {
			bound_texture_types.resize(std::max(bound_texture_types.size(), count));

			for (int i=0; i<(int)count; ++i) {
				auto& to_bind = textures[i];
				auto& bound_type = bound_texture_types[i];
			
				bool unbind_old = to_bind.tex.type != bound_type && bound_type != 0;
				bool bind_new   = to_bind.tex.type != 0;

				if (unbind_old || bind_new)
					glActiveTexture((GLenum)(GL_TEXTURE0 + i));

				// can have multiple textures of differing types bound in the same texture unit
				// this is super confusing in the graphics debugger, unbind if previous texture is a different type
				// (otherwise we overwrite it anyway, this just saves one gl call)
				if (unbind_old) {
					glBindTexture(bound_type, 0); // unbind previous
					glBindSampler((GLuint)i, 0);
				}

				// overwrite previous texture binding
				// could try to optimize this by detecting when rebinding same texture,
				// but at that point should just avoid calling this and do it manually keep it bound instead
				if (bind_new) {
					glBindSampler((GLuint)i, to_bind.sampler);

					glBindTexture(to_bind.tex.type, to_bind.tex.tex); // bind new

					auto loc = shad->get_uniform_location(to_bind.uniform_name);
					if (loc >= 0)
						glUniform1i(loc, (GLint)i);
				}

				bound_type = to_bind.tex.type;
			}

			// unbind rest of texture units
			for (int i=(int)count; i<(int)bound_texture_types.size(); ++i) {
				auto& bound_type = bound_texture_types[i];

				if (bound_type != 0) {
					glActiveTexture((GLenum)(GL_TEXTURE0 + i));
					// unbind previous
					glBindSampler((GLuint)i, 0);
					glBindTexture(bound_type, 0);
					bound_type = 0;
				}
			}
		}
		void bind_textures (Shader* shad, std::initializer_list<TextureBind> textures) {
			bind_textures(shad, textures.begin(), textures.size());
		}
		void bind_textures (Shader* shad, TextureBinds const& textures) {
			bind_textures(shad, textures.vec.data(), textures.vec.size());
		}
	};
}
using namespace state;

//
//// Texture handling
//
enum TexFilterMode {
	FILTER_MIPMAPPED = 0,
	FILTER_NEAREST,
	FILTER_BILINEAR,
};

inline Sampler make_sampler (std::string_view label, TexFilterMode filter, GLenum wrap_mode, lrgba border_col, bool aniso=false) {
	auto sampler = Sampler(label);
	
	GLenum min=0, mag=0;
	switch (filter) {
		case FILTER_MIPMAPPED: {
			min = GL_LINEAR_MIPMAP_LINEAR;
			mag = GL_LINEAR;
		} break;
		case FILTER_NEAREST: {
			min = GL_NEAREST;
			mag = GL_NEAREST;
		} break;
		case FILTER_BILINEAR: {
			min = GL_LINEAR;
			mag = GL_LINEAR;
		} break;
	}
	glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, min);
	glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, mag);

	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, wrap_mode);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, wrap_mode);
	//glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, wrap_mode);

	if (wrap_mode == GL_CLAMP_TO_BORDER) {
		glSamplerParameterfv(sampler, GL_TEXTURE_BORDER_COLOR, &border_col.x);
	}

	glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY, aniso ? max_aniso : 1.0f);

	return sampler;
}
inline Sampler make_sampler (std::string_view label, TexFilterMode filter, GLenum wrap_mode=GL_REPEAT, bool aniso=false) {
	return make_sampler(label, filter, wrap_mode, 0, aniso);
}

inline int calc_mipmaps (int w, int h) {
	return floori(log2f((float)max(w, h))) + 1;
	//for (int count=1;; count++) {
	//	if (w == 1 && h == 1)
	//		return count;
	//	w = max(w / 2, 1);
	//	h = max(h / 2, 1);
	//}
}

template <typename T>
inline void _upload_texture2D (GLenum targ, Image<T> const& img);

template<> inline void _upload_texture2D<srgb8> (GLenum targ, Image<srgb8> const& img) {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, img.size.x, img.size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, img.pixels);
}
template<> inline void _upload_texture2D<srgba8> (GLenum targ, Image<srgba8> const& img) {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, img.size.x, img.size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.pixels);
}
template<> inline void _upload_texture2D<float> (GLenum targ, Image<float> const& img) {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, img.size.x, img.size.y, 0, GL_RED, GL_FLOAT, img.pixels);
}
template<> inline void _upload_texture2D<uint16_t> (GLenum targ, Image<uint16_t> const& img) {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, img.size.x, img.size.y, 0, GL_RED, GL_UNSIGNED_SHORT, img.pixels);
}

template <typename T>
inline void upload_image2D (GLuint tex, Image<T> const& img, bool mips=true) {
	glBindTexture(GL_TEXTURE_2D, tex);
	_upload_texture2D(GL_TEXTURE_2D, img);

	if (mips) {
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

template <typename T>
inline bool upload_texture2D (GLuint tex, const char* filepath, bool mips=true) {
	Image<T> img;
	if (!Image<T>::load_from_file(filepath, &img)) {
		fprintf(stderr, "Error! Could not load texture \"%s\"\n", filepath);
		return false;
	}

	upload_image2D(tex, img, mips);
	return true;
}
template <typename T>
inline bool upload_texture2D (Texture2D& tex, const char* filepath, bool mips=true) {
	return upload_texture2D<T>((GLuint)tex, filepath, mips);
}

template <typename T>
inline Texture2D texture2D (std::string_view label, const char* filepath, bool mips=true) {
	Texture2D tex(label);
	if (!upload_texture2D<T>((GLuint)tex, filepath, mips)) {
		fprintf(stderr, "Texture \"%s\" not found!\n", filepath);
		exit(1);
	}
	return tex;
}

#if 0
// Needs to be here so that setup_shader_ubo can be used by the Shader manager
struct CommonUniforms {
	static constexpr int UBO_BINDING = 0;

	Ubo ubo = {"common_ubo"};

	struct Common {
		View3D view;
	};

	void set (View3D const& view) {
		Common common = {};
		common.view = view;
		stream_buffer(GL_UNIFORM_BUFFER, ubo, sizeof(common), &common, GL_STREAM_DRAW);

		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBindBufferBase(GL_UNIFORM_BUFFER, UBO_BINDING, ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	// only needed in gl <4.2 since  layout(binding = 0) uniform Common  syntex is not available
	static void setup_shader_ubo (GLuint prog) {
		GLuint block_idx = glGetUniformBlockIndex(prog, "Common");
		if (block_idx != GL_INVALID_INDEX)
			glUniformBlockBinding(prog, block_idx, UBO_BINDING);
	}
};
#endif

class Render_Texture {
	GLuint tex = 0;
public:
	MOVE_ONLY_CLASS_MEMBER(Render_Texture, tex);
	
	Render_Texture () {} // not allocated
	Render_Texture (std::string_view label, int2 size, GLenum format, int levels=1, int msaa=0) {
		glGenTextures(1, &tex);

		if (msaa > 1) {
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);
			OGL_DBG_LABEL(GL_TEXTURE, tex, label);

			glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, msaa, format, size.x, size.y, GL_TRUE);
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAX_LEVEL, 0);
			glTexParameterf(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Avoid supposed fbo incomplete error possible from this
			glTexParameterf(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, tex);
			OGL_DBG_LABEL(GL_TEXTURE, tex, label);

			glTexStorage2D(GL_TEXTURE_2D, levels, format, size.x, size.y);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, levels-1);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
	~Render_Texture () {
		if (tex) glDeleteTextures(1, &tex);
	}

	operator GLuint () const { return tex; }
};

struct Renderbuffer {
	MOVE_ONLY_CLASS(Renderbuffer); // No move implemented for now
public:

	static void swap (Renderbuffer& l, Renderbuffer& r) {
		std::swap(l.fbo, r.fbo);
		std::swap(l.col, r.col);
	}

	Fbo fbo = {};
	Render_Texture col = {};

	Renderbuffer () {} // not allocated
	Renderbuffer (std::string_view label, int2 size, GLenum color_format, bool mips=false, int msaa=1) { // allocate
		GLint levels = mips ? calc_mipmaps(size.x, size.y) : 1;

		std::string lbl = (std::string)label;

		col = Render_Texture(lbl+".col", size, color_format, levels, msaa);

		GLenum msaa_targ = msaa > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		
		{
			fbo = Fbo(lbl+".fbo");
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, msaa_targ, col, 0);

			GLuint bufs[] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(ARRLEN(bufs), bufs);
		
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (status != GL_FRAMEBUFFER_COMPLETE) {
				fprintf(stderr, "glCheckFramebufferStatus: %x\n", status);
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(msaa_targ, 0);
	}
};

struct RenderbufferWithDepth {
	MOVE_ONLY_CLASS(RenderbufferWithDepth); // No move implemented for now
public:

	static void swap (RenderbufferWithDepth& l, RenderbufferWithDepth& r) {
		std::swap(l.fbo, r.fbo);
		std::swap(l.col, r.col);
		std::swap(l.depth, r.depth);
	}

	Fbo fbo = {};
	Render_Texture col = {};
	Render_Texture depth = {};

	RenderbufferWithDepth () {} // not allocated
	RenderbufferWithDepth (std::string_view label, int2 size, GLenum color_format, GLenum depth_format=0, bool color_mips=false, int msaa=1) { // allocate
		GLint levels = color_mips ? calc_mipmaps(size.x, size.y) : 1;

		std::string lbl = (std::string)label;

		col = Render_Texture(lbl+".col", size, color_format, levels, msaa);

		if (depth_format)
			depth = Render_Texture(lbl+".depth", size, depth_format, levels, msaa);

		GLenum msaa_targ = msaa > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		
		{
			fbo = Fbo(lbl+".fbo");
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, msaa_targ, col, 0);
			if (depth)
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, msaa_targ, depth, 0);
		
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (status != GL_FRAMEBUFFER_COMPLETE) {
				fprintf(stderr, "glCheckFramebufferStatus: %x\n", status);
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(msaa_targ, 0);
	}
};

// framebuffer for rendering at different resolution and to make sure we get float buffers
struct FramebufferTexture {
	RenderbufferWithDepth fbo_MSAA = {}; // msaa>1 ?   MSAA textures : normal textures
	RenderbufferWithDepth fbo      = {}; // msaa>1 ? normal textures : 0
	
	render::RenderScale_with_MSAA renderscale;

	Sampler fbo_sampler         = make_sampler("fbo_sampler", FILTER_MIPMAPPED, GL_CLAMP_TO_EDGE);
	Sampler fbo_sampler_nearest = make_sampler("fbo_sampler_nearest", FILTER_NEAREST, GL_CLAMP_TO_EDGE);
	
	static constexpr GLenum color_format = GL_R11F_G11F_B10F;// GL_RGBA16F GL_RGBA32F GL_SRGB8_ALPHA8
	static constexpr GLenum color_format_resolve = GL_SRGB8_ALPHA8;
	static constexpr GLenum depth_format = GL_DEPTH_COMPONENT32F;
	static constexpr bool color_mips = false;

	void update (int2 window_size) {
		if (renderscale.update(window_size)) {
			glActiveTexture(GL_TEXTURE0); // try clobber consistent texture at least
			
			fbo_MSAA = {};
			fbo = {};

			// create new

			if (renderscale.MSAA > 1) {
				fbo_MSAA  = RenderbufferWithDepth("fbo.MSAA", renderscale.size, color_format, depth_format, false, renderscale.MSAA);
				fbo       = RenderbufferWithDepth("fbo",      renderscale.size, color_format, 0, color_mips, 1);
			}
			else {
				fbo       = RenderbufferWithDepth("fbo",      renderscale.size, color_format, depth_format, color_mips, 1);
				// leave fbo_resolve as 0
			}
		}
	}
	
	void bind () {
		glBindFramebuffer(GL_FRAMEBUFFER, (fbo_MSAA.fbo ? fbo_MSAA : fbo).fbo);
	}
	void resolve_and_blit_to_screen (int2 window_size) {

		if (fbo_MSAA.fbo) {
			OGL_TRACE("FBO.MSAA_resolve");
			
			// blit MSAA fbo into normal fbo (resolve MSAA)
			glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_MSAA.fbo);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo     .fbo);

			glBlitFramebuffer(0,0, renderscale.size.x,renderscale.size.y,
							  0,0, renderscale.size.x,renderscale.size.y,
							  GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}
		{
			OGL_TRACE("FBO.rescale");

			// blit normal fbo into window fbo (rescale)
			glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo.fbo);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

			glBlitFramebuffer(0,0, renderscale.size.x,renderscale.size.y,
								0,0, window_size.x,window_size.y,
								GL_COLOR_BUFFER_BIT, renderscale.nearest ? GL_NEAREST : GL_LINEAR);
		}

		// now draw to window fbo
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	// resolve MSAA fbo into fbo_resolve
	// run postprocess shader to draw to window fbo yourself (use fbo.col texture)
	void resolve (int2 window_size) {
		if (fbo_MSAA.fbo) {
			OGL_TRACE("FBO.MSAA_resolve");
			
			// blit MSAA fbo into normal fbo (resolve MSAA)
			glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_MSAA.fbo);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo     .fbo);

			glBlitFramebuffer(0,0, renderscale.size.x,renderscale.size.y,
							  0,0, renderscale.size.x,renderscale.size.y,
							  GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}
		
		// now draw to window fbo
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	Sampler& get_sampler () {
		return renderscale.nearest ? fbo_sampler_nearest : fbo_sampler;
	}
};

inline void draw_fullscreen_triangle (StateManager& state) {
	PipelineState s;
	s.depth_test   = false;
	s.depth_write  = false;
	s.blend_enable = false;
	state.set_no_override(s);
		
	glBindVertexArray(state.dummy_vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

// take screenshot of current bound framebuffer
void take_screenshot (int2 size);

} // namespace ogl

#pragma once
#include "common.hpp"
#include "camera.hpp"
#include "glad/glad.h"
#include "TracyOpenGL.hpp"

namespace ogl {

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


void APIENTRY debug_callback (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char* message, void const* userParam);

struct ShaderUniform {
	std::string	name;
	GLenum		type;
	GLint		location;
};

//typedef ordered_map<std::string, ShaderUniform> uniform_set;
typedef std::vector<ShaderUniform> uniform_set;

namespace uniforms {
	inline void _check_uniform (ShaderUniform& u, GLenum passed) {
		if (u.type == passed) return;
		if (passed == GL_INT && (u.type == GL_SAMPLER_1D || u.type == GL_SAMPLER_2D || u.type == GL_SAMPLER_3D)) return;
		fprintf(stderr, "%x\n", u.type);
		assert(false);
	}

	inline void _set_uniform (ShaderUniform& u, float    const* values, int arr_count) { _check_uniform(u, GL_FLOAT        ); glUniform1fv(u.location, arr_count, values); }
	inline void _set_uniform (ShaderUniform& u, float2   const* values, int arr_count) { _check_uniform(u, GL_FLOAT_VEC2   ); glUniform2fv(u.location, arr_count, &values->x); }
	inline void _set_uniform (ShaderUniform& u, float3   const* values, int arr_count) { _check_uniform(u, GL_FLOAT_VEC3   ); glUniform3fv(u.location, arr_count, &values->x); }
	inline void _set_uniform (ShaderUniform& u, float4   const* values, int arr_count) { _check_uniform(u, GL_FLOAT_VEC4   ); glUniform4fv(u.location, arr_count, &values->x); }
	inline void _set_uniform (ShaderUniform& u, int      const* values, int arr_count) { _check_uniform(u, GL_INT          ); glUniform1iv(u.location, arr_count, values); }
	inline void _set_uniform (ShaderUniform& u, uint32_t const* values, int arr_count) { _check_uniform(u, GL_UNSIGNED_INT ); glUniform1uiv(u.location, arr_count, values); }
	inline void _set_uniform (ShaderUniform& u, int2     const* values, int arr_count) { _check_uniform(u, GL_INT_VEC2     ); glUniform2iv(u.location, arr_count, &values->x); }
	inline void _set_uniform (ShaderUniform& u, int3     const* values, int arr_count) { _check_uniform(u, GL_INT_VEC3     ); glUniform3iv(u.location, arr_count, &values->x); }
	inline void _set_uniform (ShaderUniform& u, int4     const* values, int arr_count) { _check_uniform(u, GL_INT_VEC4     ); glUniform4iv(u.location, arr_count, &values->x); }
	inline void _set_uniform (ShaderUniform& u, float2x2 const* values, int arr_count) { _check_uniform(u, GL_FLOAT_MAT2   ); glUniformMatrix2fv(u.location, arr_count, GL_FALSE, &values->arr[0].x); }
	inline void _set_uniform (ShaderUniform& u, float3x3 const* values, int arr_count) { _check_uniform(u, GL_FLOAT_MAT3   ); glUniformMatrix3fv(u.location, arr_count, GL_FALSE, &values->arr[0].x); }
	inline void _set_uniform (ShaderUniform& u, float4x4 const* values, int arr_count) { _check_uniform(u, GL_FLOAT_MAT4   ); glUniformMatrix4fv(u.location, arr_count, GL_FALSE, &values->arr[0].x); }

	inline void _set_uniform (ShaderUniform& u, bool     const& val) { _check_uniform(u, GL_BOOL         ); glUniform1i(u.location, val ? GL_TRUE : GL_FALSE); }
	inline void _set_uniform (ShaderUniform& u, float    const& val) { _check_uniform(u, GL_FLOAT        ); glUniform1f(u.location, val); }
	inline void _set_uniform (ShaderUniform& u, float2   const& val) { _check_uniform(u, GL_FLOAT_VEC2   ); glUniform2fv(u.location, 1, &val.x); }
	inline void _set_uniform (ShaderUniform& u, float3   const& val) { _check_uniform(u, GL_FLOAT_VEC3   ); glUniform3fv(u.location, 1, &val.x); }
	inline void _set_uniform (ShaderUniform& u, float4   const& val) { _check_uniform(u, GL_FLOAT_VEC4   ); glUniform4fv(u.location, 1, &val.x); }
	inline void _set_uniform (ShaderUniform& u, int      const& val) { _check_uniform(u, GL_INT          ); glUniform1i(u.location, val); }
	inline void _set_uniform (ShaderUniform& u, uint32_t const& val) { _check_uniform(u, GL_UNSIGNED_INT ); glUniform1ui(u.location, val); }
	inline void _set_uniform (ShaderUniform& u, int2     const& val) { _check_uniform(u, GL_INT_VEC2     ); glUniform2iv(u.location, 1, &val.x); }
	inline void _set_uniform (ShaderUniform& u, int3     const& val) { _check_uniform(u, GL_INT_VEC3     ); glUniform3iv(u.location, 1, &val.x); }
	inline void _set_uniform (ShaderUniform& u, int4     const& val) { _check_uniform(u, GL_INT_VEC4     ); glUniform4iv(u.location, 1, &val.x); }
	inline void _set_uniform (ShaderUniform& u, float2x2 const& val) { _check_uniform(u, GL_FLOAT_MAT2   ); glUniformMatrix2fv(u.location, 1, GL_FALSE, &val.arr[0].x); }
	inline void _set_uniform (ShaderUniform& u, float3x3 const& val) { _check_uniform(u, GL_FLOAT_MAT3   ); glUniformMatrix3fv(u.location, 1, GL_FALSE, &val.arr[0].x); }
	inline void _set_uniform (ShaderUniform& u, float4x4 const& val) { _check_uniform(u, GL_FLOAT_MAT4   ); glUniformMatrix4fv(u.location, 1, GL_FALSE, &val.arr[0].x); }

	inline bool _findUniform (ShaderUniform const& l, std::string_view const& r) { return l.name == r; }
}

enum ShaderStage {
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

bool compile_shader (const char* name, const char* dbgname,
		std::vector<ShaderStage> const& stages, std::vector<MacroDefinition> const& macros,
		GLuint* out_prog, uniform_set* out_uniforms, std::vector<std::string>* src_files);

struct Shader {
	GLuint                          prog = 0;
	
	std::string                     name;
	std::string                     dbgname;

	std::vector<ShaderStage>        stages;
	std::vector<MacroDefinition>    macros;
	
	uniform_set                     uniforms;
	std::vector<std::string>        src_files;
	
	~Shader () {
		if (prog) glDeleteProgram(prog);
		prog = 0;
	}
	
	inline bool recompile () {
		GLuint new_prog = 0;
		uniform_set new_uniforms;
		std::vector<std::string> new_src_files;

		bool success = compile_shader(name.c_str(), dbgname.c_str(), stages, macros,
			&new_prog, &new_uniforms, &new_src_files);
		
		if (success) {
			// success, delete old shader
			if (prog) glDeleteProgram(prog);
			// replace with new shader
			prog = new_prog;
			uniforms = new_uniforms;
			src_files = new_src_files;
			return true;
		} else {
			// fail, keep old shader
			if (new_prog) glDeleteProgram(new_prog);
			return false;
		}

		return success;
	}
	
	inline GLint get_uniform_location (std::string_view const& name) {
		int i = indexof(uniforms, name, uniforms::_findUniform);
		if (i < 0) return i;
		return uniforms[i].location;
	}
	
	template <typename T>
	inline void set_uniform (std::string_view const& name, T const& val) {
		int i = indexof(uniforms, name, uniforms::_findUniform);
		if (i >= 0)
			uniforms::_set_uniform(uniforms[i], val);
	}
	template <typename T>
	inline void set_uniform_array (std::string_view const& name, T const* values, int arr_count) {
		int i = indexof(uniforms, name, uniforms::_findUniform);
		if (i >= 0)
			uniforms::_set_uniform(uniforms[i], values, arr_count);
	}
};

struct Shaders {
	// Shaders are owned by global Shaders objects
	// if users no longer use shader it still sticks around until the end of the program for simplicity
	std::vector<std::unique_ptr<Shader>> shaders;
	
	inline void shutdown () {
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
					if (!s->recompile())
						success = false; // any failed shader signals reload fail
				}
			}
		}
		
		/*
		if (this->wireframe != wireframe) {
			this->wireframe = wireframe;
        
			for (auto& s : shaders) {
				s->recompile("wireframe toggle", wireframe);
			}
		}*/
		
		return success;
	}
	
	inline Shader* compile (char const* name,
			std::vector<MacroDefinition>&& macros = {},
			std::initializer_list<ShaderStage> stages = { VERTEX_SHADER, FRAGMENT_SHADER },
			char const* dbgname=nullptr) {
		ZoneScoped;
		
		auto s = std::make_unique<Shader>();
		s->name = name;
		s->dbgname = dbgname ? dbgname : name;
		s->stages = stages;
		s->macros = std::move(macros);
		
		if (!compile_shader(s->name.c_str(), s->dbgname.c_str(), s->stages, s->macros,
				&s->prog, &s->uniforms, &s->src_files)) {
			// will crash if attempt to be used, but 0 program in gl might also crash
			// alternative: return dummy shader?
			return nullptr;
		}
		
		auto* ptr = s.get();
		shaders.push_back(std::move(s));
		return ptr;
	}
};

inline Shaders g_shaders;

// simple gl resource wrappers to avoid writing destructors manually all the time
class Vbo {
	GLuint vbo = 0;
public:
	MOVE_ONLY_CLASS_MEMBER(Vbo, vbo);

	Vbo () {} // not allocated
	Vbo (std::string_view label) { // allocate
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		OGL_DBG_LABEL(GL_BUFFER, vbo, label);
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
	}
	~Texture1D () {
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
	}
	~Texture2DArray () {
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
	}
	~Fbo () {
		if (fbo) glDeleteFramebuffers(1, &fbo);
	}

	operator GLuint () const { return fbo; }
};

inline void upload_buffer (GLenum target, GLuint buf, size_t size, void const* data, GLenum usage = GL_STATIC_DRAW) {
	glBindBuffer(target, buf);

	glBufferData(target, size, data, usage);

	glBindBuffer(target, 0);
}
inline void stream_buffer (GLenum target, GLuint buf, size_t size, void const* data, GLenum usage = GL_STREAM_DRAW) {
	glBindBuffer(target, buf);

	glBufferData(target, size, nullptr, usage);
	if (size > 0)
		glBufferData(target, size, data, usage);

	glBindBuffer(target, 0);
}

struct VertexBuffer {
	Vao vao;
	Vbo vbo;
	VertexBuffer (std::string_view label): vao{label + ".vao"}, vbo{label + ".vbo"} {}

	template <typename VT> void upload (std::vector<VT> const& vertices) {
		upload_buffer(GL_ARRAY_BUFFER, vbo, (GLsizeiptr)vertices.size() * sizeof(VT), vertices.data(), GL_STATIC_DRAW);
	}
	template <typename VT> void stream (std::vector<VT> const& vertices) {
		stream_buffer(GL_ARRAY_BUFFER, vbo, (GLsizeiptr)vertices.size() * sizeof(VT), vertices.data(), GL_STREAM_DRAW);
	}
};
struct VertexBufferI {
	Vao vao;
	Vbo vbo;
	Ebo ebo;
	VertexBufferI (std::string_view label): vao{label + ".vao"}, vbo{label + ".vbo"}, ebo{label + ".ebo"} {}

	template <typename VT, typename IT> void upload (std::vector<VT> const& vertices, std::vector<IT> const& indices) {
		upload_buffer(GL_ARRAY_BUFFER        , vbo, (GLsizeiptr)vertices.size() * sizeof(VT), vertices.data(), GL_STATIC_DRAW);
		upload_buffer(GL_ELEMENT_ARRAY_BUFFER, ebo, (GLsizeiptr) indices.size() * sizeof(IT),  indices.data(), GL_STATIC_DRAW);
	}
	template <typename VT, typename IT> void stream (std::vector<VT> const& vertices, std::vector<IT> const& indices) {
		stream_buffer(GL_ARRAY_BUFFER        , vbo, (GLsizeiptr)vertices.size() * sizeof(VT), vertices.data(), GL_STREAM_DRAW);
		stream_buffer(GL_ELEMENT_ARRAY_BUFFER, ebo, (GLsizeiptr) indices.size() * sizeof(IT),  indices.data(), GL_STREAM_DRAW);
	}
};

struct VertexBufferInstanced {
	Vao vao;
	Vbo vbo;
	Vbo instances;
	VertexBufferInstanced (std::string_view label): vao{label + ".vao"}, vbo{label + ".vbo"}, instances{label + ".instances.vbo"} {}

	template <typename VT> void upload_mesh (std::vector<VT> const& vertices) {
		upload_buffer(GL_ARRAY_BUFFER, vbo, (GLsizeiptr)vertices.size() * sizeof(VT), vertices.data(), GL_STATIC_DRAW);
	}
	template <typename IT> void stream_instances (std::vector<IT> const& instance_data) {
		stream_buffer(GL_ARRAY_BUFFER, instances, (GLsizeiptr)instance_data.size() * sizeof(IT), instance_data.data(), GL_STREAM_DRAW);
	}
};
struct VertexBufferInstancedI {
	Vao vao;
	Vbo vbo;
	Ebo ebo;
	Vbo instances;
	VertexBufferInstancedI (std::string_view label): vao{label + ".vao"}, vbo{label + ".vbo"}, ebo{label + ".ebo"}, instances{label + ".instances.vbo"} {}

	template <typename VT, typename IT> void upload_mesh (std::vector<VT> const& vertices, std::vector<IT> const& indices) {
		upload_buffer(GL_ARRAY_BUFFER        , vbo, (GLsizeiptr)vertices.size() * sizeof(VT), vertices.data(), GL_STATIC_DRAW);
		upload_buffer(GL_ELEMENT_ARRAY_BUFFER, ebo, (GLsizeiptr) indices.size() * sizeof(IT),  indices.data(), GL_STATIC_DRAW);
	}
	template <typename IT> void stream_instances (std::vector<IT> const& instance_data) {
		stream_buffer(GL_ARRAY_BUFFER, instances, (GLsizeiptr)instance_data.size() * sizeof(IT), instance_data.data(), GL_STREAM_DRAW);
	}
};

#define ATTRIB(IDX, TYPE, COMPONENTS, STRUCT, NAME) do { \
	auto _idx = IDX; \
	glEnableVertexAttribArray(_idx); \
	glVertexAttribPointer(_idx, COMPONENTS, TYPE, false, sizeof(STRUCT), (void*)offsetof(STRUCT, NAME)); \
} while (false)
#define ATTRIBI(IDX, TYPE, COMPONENTS, STRUCT, NAME) do { \
	auto _idx = IDX; \
	glEnableVertexAttribArray(_idx); \
	glVertexAttribPointerI(_idx, COMPONENTS, TYPE, false, sizeof(STRUCT), (void*)offsetof(STRUCT, NAME)); \
} while (false)

#define ATTRIB(IDX, TYPE, COMPONENTS, STRUCT, NAME) do { \
	auto _idx = IDX; \
	glEnableVertexAttribArray(_idx); \
	glVertexAttribPointer(_idx, COMPONENTS, TYPE, false, sizeof(STRUCT), (void*)offsetof(STRUCT, NAME)); \
} while (false)
#define ATTRIBI(IDX, TYPE, COMPONENTS, STRUCT, NAME) do { \
	auto _idx = IDX; \
	glEnableVertexAttribArray(_idx); \
	glVertexAttribPointerI(_idx, COMPONENTS, TYPE, false, sizeof(STRUCT), (void*)offsetof(STRUCT, NAME)); \
} while (false)

typedef void (*vertex_attrib_func)(int);
/* Use like:
	struct Vertex {
		float3 pos;
		float2 uv;
		float4 col;

		static void attrib (int idx) {
			ATTRIB(GL_FLOAT, 3, Vertex, pos);
			ATTRIB(GL_FLOAT, 2, Vertex, uv);
			ATTRIB(GL_FLOAT, 4, Vertex, col);
		}
	};
*/

inline void setup_vao (vertex_attrib_func vertex_attrib, GLuint vao, GLuint vbo, GLuint ebo=0) {
	glBindVertexArray(vao);
	if (ebo) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	vertex_attrib(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0); // glVertexAttribPointer remembers VAO

	glBindVertexArray(0);
	if (ebo) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // VAO remembers EBO (note that we need to unbind VAO first)
}

template <typename VERTEX>
inline VertexBuffer vertex_buffer (std::string_view label) {
	VertexBuffer buf = {label};
	setup_vao(VERTEX::attrib, buf.vao, buf.vbo, 0);
	return buf;
}
template <typename VERTEX>
inline VertexBufferI vertex_bufferI (std::string_view label) {
	VertexBufferI buf = {label};
	setup_vao(VERTEX::attrib, buf.vao, buf.vbo, buf.ebo);
	return buf;
}

/* 3 --- 2
   |   / |
   | /   |
   0 --- 1 */
static constexpr uint16_t QUAD_INDICES[6] = { 1,2,0, 0,2,3 };

//// Opengl global state management

enum DepthFunc {
	DEPTH_INFRONT, // normal: draw infront (or equal depth) of other things
	DEPTH_BEHIND, // inverted: draw behind other things
};
inline GLenum map_depth_func (DepthFunc func) {
	switch (func) {
		case DEPTH_INFRONT:  return GL_LEQUAL;
		case DEPTH_BEHIND:   return GL_GEQUAL;
		default: return 0;
	}
}

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
		state = PipelineState();
		state = _override(state);

		glSetEnable(GL_DEPTH_TEST, state.depth_test);
		// use_reverse_depth
		glDepthFunc(map_depth_func(state.depth_func));
		glClearDepth(0.0f);
		glDepthRange(0.0f, 1.0f);
		glDepthMask(state.depth_write ? GL_TRUE : GL_FALSE);

		glSetEnable(GL_SCISSOR_TEST, state.scissor_test);

		// culling
		glSetEnable(GL_CULL_FACE, state.cull_face);
		glCullFace(state.front_face == CULL_FRONT ? GL_FRONT : GL_BACK);
		glFrontFace(GL_CCW);
		// blending
		glSetEnable(GL_BLEND, state.blend_enable);
		glBlendEquation(state.blend_func.equation);
		glBlendFunc(state.blend_func.sfactor, state.blend_func.dfactor);

		glPolygonMode(GL_FRONT_AND_BACK, state.poly_mode == POLY_FILL ? GL_FILL : GL_LINE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// set opengl drawing state to a set of values, where only changes are applied
	// no overrides to not break fullscreen quads etc.
	void set_no_override (PipelineState const& s) {
	#if OGL_STATE_ASSERT
		{
			assert(state.depth_test == !!glIsEnabled(GL_DEPTH_TEST));
			GLint depth_func;		glGetIntegerv(GL_DEPTH_FUNC, &depth_func);
			assert(map_depth_func(state.depth_func) == depth_func);
			GLint depth_write;		glGetIntegerv(GL_DEPTH_WRITEMASK, &depth_write);
			assert(state.depth_write == !!depth_write);

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

			GLint poly_mode;		glGetIntegerv(GL_POLYGON_MODE, &poly_mode);
			assert((state.poly_mode == POLY_FILL ? GL_FILL : GL_LINE) == poly_mode);
		}
	#endif

		if (state.depth_test != s.depth_test)
			glSetEnable(GL_DEPTH_TEST, s.depth_test);
		if (state.depth_func != s.depth_func)
			glDepthFunc(map_depth_func(s.depth_func));
		if (state.depth_write != s.depth_write)
			glDepthMask(s.depth_write ? GL_TRUE : GL_FALSE);

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
};

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

struct LineRenderer {

	Shader* shad = g_shaders.compile("line_render");

	struct Vertex {
		float3 pos;
		float4 col;

		static void attrib (int idx) {
			ATTRIB(idx++, GL_FLOAT,3, Vertex, pos);
			ATTRIB(idx++, GL_FLOAT,4, Vertex, col);
		}
	};
	VertexBuffer vbo = vertex_buffer<Vertex>("LineDrawer");

	std::vector<Vertex> vertices;

	void begin () {
		vertices.clear();
	}

	struct DrawCall {
		float line_width;
		bool line_antialias;

		int first_vertex;
		int vertex_count = 0;
	};

	DrawCall begin_draw (float line_width = 1.0f, bool line_antialias = true) {
		return { line_width, line_antialias, (int)vertices.size() };
	}

	int draw_line (float3 const& a, float3 const& b, float4 const& col) {
		auto* v = push_back(vertices, 2);
		
		v[0].pos = a;
		v[0].col = col;
		v[1].pos = b;
		v[1].col = col;

		return 2;
	}

	void upload_vertices () {
		vbo.stream(vertices);
	}

	void render (StateManager& state, DrawCall const* draws, int draw_count) {
		OGL_TRACE("LineDrawer");
		ZoneScoped;

		glUseProgram(shad->prog);

		PipelineState s;
		s.depth_test = false;
		s.blend_enable = true;
		state.set(s);

		glBindVertexArray(vbo.vao);

		for (int i=0; i<draw_count; ++i) {
			if (draws[i].vertex_count == 0) continue;
			
			glSetEnable(GL_LINE_SMOOTH, draws[i].line_antialias);
			glLineWidth(max(draws[i].line_width, 0.01f));

			glDrawArrays(GL_LINES, draws[i].first_vertex, (GLsizei)draws[i].vertex_count);
		}
	}
	void render (StateManager& state, DrawCall const& draw) {
		render(state, &draw, 1);
	}

	void render (StateManager& state, float line_width = 1.0f, bool line_antialias = true) {
		DrawCall draw = { line_width, line_antialias, 0, (int)vertices.size() };
		render(state, &draw, 1);
	}
};

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
	printf( msg.c_str() );
}

struct ShapeRenderer {
	Shader* shad;

	ShapeRenderer (const char* shad_name) {
		shad = g_shaders.compile(shad_name);

		upload_buffer(GL_ARRAY_BUFFER        , vbo.vbo, (GLsizeiptr)sizeof(VERTICES),     VERTICES,     GL_STATIC_DRAW);
		upload_buffer(GL_ELEMENT_ARRAY_BUFFER, vbo.ebo, (GLsizeiptr)sizeof(QUAD_INDICES), QUAD_INDICES, GL_STATIC_DRAW);
	}

	struct Vertex {
		float2 mesh_pos;
	};
	struct Instance {
		float3 inst_pos;
		float  inst_size; // in pixels
		float4 inst_col;
	};

	static inline constexpr Vertex VERTICES[4] = {
		{{ -0.5f, -0.5f }},
		{{ +0.5f, -0.5f }},
		{{ +0.5f, +0.5f }},
		{{ -0.5f, +0.5f }},
	};

	VertexBufferInstancedI make_instanced_vbo () {
		VertexBufferInstancedI buf = {"ShapeRenderer"};

		glBindVertexArray(buf.vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.ebo);
		
		glBindBuffer(GL_ARRAY_BUFFER, buf.vbo);
		{
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, mesh_pos));
		}

		glBindBuffer(GL_ARRAY_BUFFER, buf.instances);
		{
			glEnableVertexAttribArray(1);
			glVertexAttribDivisor(1, 1);
			glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Instance), (void*)offsetof(Instance, inst_pos));

			glEnableVertexAttribArray(2);
			glVertexAttribDivisor(2, 1);
			glVertexAttribPointer(2, 1, GL_FLOAT, false, sizeof(Instance), (void*)offsetof(Instance, inst_size));

			glEnableVertexAttribArray(3);
			glVertexAttribDivisor(3, 1);
			glVertexAttribPointer(3, 4, GL_FLOAT, false, sizeof(Instance), (void*)offsetof(Instance, inst_col));
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
		return buf;
	}

	VertexBufferInstancedI vbo = make_instanced_vbo();

	std::vector<Instance> instances;

	void begin () {
		instances.clear();
	}

	int draw (float3 const& a, float size, float4 const& col) {
		auto* i = push_back(instances, 1);

		i->inst_pos  = a;
		i->inst_size = size;
		i->inst_col  = col;

		return 1;
	}

	void upload_vertices () {
		vbo.stream_instances(instances);
	}

	void render (StateManager& state) {
		if (instances.empty()) return;

		OGL_TRACE("ShapeRenderer");
		ZoneScoped;

		glUseProgram(shad->prog);

		PipelineState s;
		s.depth_test = false;
		s.blend_enable = true;
		state.set(s);

		glBindVertexArray(vbo.vao);
		glDrawElementsInstanced(GL_TRIANGLES, ARRLEN(QUAD_INDICES), GL_UNSIGNED_SHORT, (void*)0, (GLsizei)instances.size());
	}
};

} // namespace ogl
using ogl::g_shaders;

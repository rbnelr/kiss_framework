#include "common.hpp" // for PCH
#include "opengl.hpp"
#include "kisslib/strparse.hpp"

#include "kisslib/stb_image_write.hpp"

namespace ogl {

void APIENTRY debug_callback (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char* message, void const* userParam) {
	//Engine* eng = (Engine*)userParam;
	
	//if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB) return;
	if (source == GL_DEBUG_SOURCE_APPLICATION_ARB) {
		//printf("%.*s\n", length, message); // message is not null terminated, pass explicit length
		return; // OGL_TRACE is only for organizing drawcalls in nsight, not to spam the console
	}
	
	// hiding irrelevant infos/warnings
	switch (id) {
		case 131185: // Buffer detailed info
		//case 1282: // using shader that was not compiled successfully
		//case 2: // API_ID_RECOMPILE_FRAGMENT_SHADER performance warning has been generated. Fragment shader recompiled due to state change.
		//case 131218: // Program/shader state performance warning: Fragment shader in program 3 is being recompiled based on GL state.
		
		////case 131154: // Pixel transfer sync with rendering warning
		//
		//case 1282: // Wierd error on notebook when trying to do texture streaming
		//case 131222: // warning with unused shadow samplers ? (Program undefined behavior warning: Sampler object 0 is bound to non-depth texture 0, yet it is used with a program that uses a shadow sampler . This is undefined behavior.), This might just be unused shadow samplers, which should not be a problem
		//case 131218: // performance warning, because of shader recompiling based on some 'key'

		// Pixel-path detailed info: The current pixel-path operation converts data from 2-bit integer to 1-bit integer, and may exhibit data loss
		// when intentionally uploading uint16_t to GL_R8UI
		case 131153:
			return;
		case 1281:
		case 5145:
			return; // shader compile error?
		case 1282:
			return; // using shader with compile error, don't care, i'm going to fix it
		default:
			break;
	}

	const char* src_str = "<unknown>";
	switch (source) {
		case GL_DEBUG_SOURCE_API_ARB:				src_str = "API";				break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:		src_str = "WINDOW_SYSTEM";		break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:	src_str = "SHADER_COMPILER";	break;
		case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:		src_str = "THIRD_PARTY";		break;
		case GL_DEBUG_SOURCE_APPLICATION_ARB:		src_str = "APPLICATION";		break;
		case GL_DEBUG_SOURCE_OTHER_ARB:				src_str = "OTHER";				break;
	}

	const char* type_str = "<unknown>";
	switch (source) {
		case GL_DEBUG_TYPE_ERROR_ARB:				type_str = "ERROR";					break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:	type_str = "DEPRECATED_BEHAVIOR";	break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:	type_str = "UNDEFINED_BEHAVIOR";	break;
		case GL_DEBUG_TYPE_PORTABILITY_ARB:			type_str = "PORTABILITY";			break;
		case GL_DEBUG_TYPE_PERFORMANCE_ARB:			type_str = "PERFORMANCE";			break;
		case GL_DEBUG_TYPE_OTHER_ARB:				type_str = "OTHER";					break;
	}

	const char* severity_str = "<unknown>";
	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH_ARB:			severity_str = "HIGH";		break;
		case GL_DEBUG_SEVERITY_MEDIUM_ARB:			severity_str = "MEDIUM";	break;
		case GL_DEBUG_SEVERITY_LOW_ARB:				severity_str = "LOW";		break;
	}

	fprintf(stderr, "[OpenGL] debug message: severity:%s  src:%s  type:%s  id:%d\n%.*s\n", severity_str, src_str, type_str, id, length, message); // message is not null terminated, pass explicit length
	fflush(stderr);

#if RENDERER_DEBUG_OUTPUT_BREAKPOINT
	if (severity == GL_DEBUG_SEVERITY_HIGH_ARB)
		__debugbreak();
#endif
}

//// Shader manager
/* simple shader
GLuint Shader::compile_shader (const char* name, const char* vertex, const char* fragment) {
	GLuint prog = glCreateProgram();

	GLuint vert = glCreateShader(GL_VERTEX_SHADER);
	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);

	glAttachShader(prog, vert);
	glAttachShader(prog, frag);

	const char* vertex_ptr = vertex;
	const char* fragment_ptr = fragment;

	glShaderSource(vert, 1, &vertex_ptr, NULL);
	glShaderSource(frag, 1, &fragment_ptr, NULL);

	glCompileShader(vert);
	glCompileShader(frag);

	bool error = false;
	error = check_shader(vert, name) || error;
	error = check_shader(frag, name) || error;

	if (!error) {
		glLinkProgram(prog);

		error = check_program(prog, name);
	}

	glDetachShader(prog, vert);
	glDeleteShader(vert);

	glDetachShader(prog, frag);
	glDeleteShader(frag);

	if (error) {
		glDeleteProgram(prog);
		prog = 0;
	}
	return prog;
}
*/

// Quick hack to find out if we are on nvidia
bool gl_is_nvidia () {
	static int vendor_NV = -1;
	if (vendor_NV == -1) {
		// Find vendor once
		std::string str = (const char*)glGetString(GL_VENDOR);
		vendor_NV = kiss::contains(kiss::to_upper(str), "NVIDIA") ? 1 : 0;
	}
	return vendor_NV == 1;
}

namespace shader {
	void line_marker (std::string* result, int line, char const* filename, int fileidx) {
		if (gl_is_nvidia())
			prints(result, "#line %d \"%s\"\n", line, filename); // only works on nvidia
		else
			prints(result, "\n#line %d %d\n", line, fileidx);
	}

	bool preprocess_include_file (char const* shader_name, char const* filename, std::string* result, std::vector<std::string>* src_files) {
		using namespace parse;

		int fileidx = indexof(*src_files, std::string_view(filename));
		if (fileidx < 0) {
			fileidx = (int)src_files->size();
			src_files->push_back(filename);
		}

		if (!result->empty()) { // not allowed to come before #version macro
			line_marker(result, 1, filename, fileidx);
		}

		std::string source;
		if (!kiss::load_text_file(filename, &source)) {
			fprintf(stderr, "[Shaders] \"%s\": could not find file \"%s\"!\n", shader_name, filename);
			return false;
		}

		bool success = true;

		auto path = kiss::get_path(filename);
		
		int line_no = 1;

		char const* c = source.c_str();
		while (*c != '\0') { // for all lines
			char const* line = c;

			whitespace(c);

			if (*c == '#') {
				c++; // skip '#'
				whitespace(c);

				std::string_view ident;
				if (identifier(c, &ident) && ident == "include") {

					whitespace(c);

					std::string_view inc_filename;
					if (!quoted_string(c, &inc_filename)) {
						fprintf(stderr, "[Shaders] \"%s\": expected filename in include at line %d!\n", shader_name, line_no);
						success = false;
					} else {

						std::string inc_filepath = path + inc_filename;
					
						if (!preprocess_include_file(shader_name, inc_filepath.c_str(), result, src_files)) // include file text instead of ' #include "filename" '
							success = false;

						line_marker(result, line_no, filename, fileidx);

						line = c; // set line to after ' #include "filename" '
					}
				}
			}

			// skip to after end of line line
			while (!newline(c) && *c != '\0')
				c++;
			
			line_no++;

			result->append(std::string_view(line, c - line));
		}

		return success;
	}

	// Need ability to define per-shader macros, but simply prepending the macros to the source code does not work
	// because first line in glsl shader needs to be "#versiom ...."
	// So workaround this fact by scanning for the right place to insert
	std::string preprocessor_insert_macro_defs (std::string const& source, char const* filename, std::string const& macros) {
		// assume first line is always #version, so simply scan for first newline
		char const* c = source.c_str();
		while (!parse::newline(c) && *c != '\0')
			c++;

		std::string result;
		result.reserve(source.capacity());

		result += std::string_view(source.c_str(), c - source.c_str()); // #version line
		result += macros;
		//prints(&result, "#line 1 \"%s\"\n", filename); // reset source line number
		prints(&result, "#line 1 %d\n", 0); // reset source line number
		result += std::string_view(c, source.size() - (c - source.c_str()));
		return result;
	}

	std::string shader_macros (std::vector<MacroDefinition> const& macros, Stage stage, bool wireframe) {
		std::string macro_text;
		macro_text.reserve(512);

		macro_text += "// Per-shader macro definitions\n";
		macro_text += prints("#define %s\n", SHADER_STAGE_MACRO[stage]);
		if (wireframe)
			macro_text += prints("#define _WIREFRAME\n");
		for (auto& m : macros)
			macro_text += prints("#define %s %s\n", m.name.c_str(), m.value.c_str());
		macro_text += "\n";
		
		return macro_text;
	}

	////
	bool get_shader_compile_log (GLuint shad, std::string* log) {
		GLsizei log_len;
		{
			GLint temp = 0;
			glGetShaderiv(shad, GL_INFO_LOG_LENGTH, &temp);
			log_len = (GLsizei)temp;
		}

		if (log_len <= 1) {
			return false; // no log available
		} else {
			// GL_INFO_LOG_LENGTH includes the null terminator, but it is not allowed to write the null terminator in std::string, so we have to allocate one additional char and then resize it away at the end

			log->resize(log_len);

			GLsizei written_len = 0;
			glGetShaderInfoLog(shad, log_len, &written_len, &(*log)[0]);
			assert(written_len == (log_len -1));

			log->resize(written_len);

			return true;
		}
	}
	bool get_program_link_log (GLuint prog, std::string* log) {
		GLsizei log_len;
		{
			GLint temp = 0;
			glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &temp);
			log_len = (GLsizei)temp;
		}

		if (log_len <= 1) {
			return false; // no log available
		} else {
			// GL_INFO_LOG_LENGTH includes the null terminator, but it is not allowed to write the null terminator in std::string, so we have to allocate one additional char and then resize it away at the end

			log->resize(log_len);

			GLsizei written_len = 0;
			glGetProgramInfoLog(prog, log_len, &written_len, &(*log)[0]);
			assert(written_len == (log_len -1));

			log->resize(written_len);

			return true;
		}
	}
	bool check_shader (GLuint shad, const char* name) {
		GLint status;
		glGetShaderiv(shad, GL_COMPILE_STATUS, &status);

		std::string log_str;
		bool log_avail = get_shader_compile_log(shad, &log_str);
		//if (log_avail) log_str = map_shader_log(log_str, stage_source.c_str());

		bool stage_error = status == GL_FALSE;
		if (stage_error) {
			// compilation failed
			printf("[Shaders] OpenGL error in shader compilation \"%s\"!\n>>>\n%s\n<<<\n", name, log_avail ? log_str.c_str() : "<no log available>");
			return true;
		} else {
			// compilation success
			if (log_avail) {
				printf("[Shaders] OpenGL shader compilation log \"%s\":\n>>>\n%s\n<<<\n", name, log_str.c_str());
			}
			return false;
		}
	}
	bool check_program (GLuint prog, const char* name) {

		GLint status;
		glGetProgramiv(prog, GL_LINK_STATUS, &status);

		std::string log_str;
		bool log_avail = get_program_link_log(prog, &log_str);

		bool error = status == GL_FALSE;
		if (error) {
			// linking failed
			printf("[Shaders] OpenGL error in shader linkage \"%s\"!\n>>>\n%s\n<<<\n", name, log_avail ? log_str.c_str() : "<no log available>");
			return true;
		} else {
			// linking success
			if (log_avail) {
				printf("[Shaders] OpenGL shader linkage log \"%s\":\n>>>\n%s\n<<<\n", name, log_str.c_str());
			}
			return false;
		}
	}

	uniform_set get_uniforms (GLuint prog) {
		uniform_set uniforms;

		GLint uniform_count = 0;
		glGetProgramiv(prog, GL_ACTIVE_UNIFORMS, &uniform_count);

		if (uniform_count != 0) {
			GLint max_name_len = 0;
			glGetProgramiv(prog, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);

			auto uniform_name = std::make_unique<char[]>(max_name_len);

			for (GLint i=0; i<uniform_count; ++i) {
				GLsizei length = 0;
				GLsizei count = 0;
				GLenum 	type = GL_NONE;
				glGetActiveUniform(prog, i, max_name_len, &length, &count, &type, uniform_name.get());

				Uniform u;
				u.location = glGetUniformLocation(prog, uniform_name.get());
				u.name = std::string(uniform_name.get(), length);
				u.type = type;

				uniforms.emplace_back(std::move(u));
			}
		}

		return uniforms;
	}

	////
	bool compile_shader (Shader& shad, const char* name, const char* dbgname,
			std::vector<Stage> const& stages, std::vector<MacroDefinition> const& macros, bool wireframe) {
		shad.src_files.clear();
		shad.uniforms.clear();

		std::string source;
		source.reserve(4096);

		std::string filename = prints("shaders/%s.glsl", name);

		// Load shader base source file
		if (!preprocess_include_file(name, filename.c_str(), &source, &shad.src_files)) {
			fprintf(stderr, "[Shaders] \"%s\": shader compilation error!\n", name);
			return false;
		}

		// Compile shader stages

		GLuint prog = glCreateProgram();
		OGL_DBG_LABEL(GL_PROGRAM, prog, dbgname);

		std::vector<GLuint> compiled_stages;

		bool error = false;
	
		for (auto stage : stages) {

			GLuint shad = glCreateShader(SHADER_STAGE_GLENUM[stage]);
			glAttachShader(prog, shad);
	
			std::string stage_source = preprocessor_insert_macro_defs(source, filename.c_str(), shader_macros(macros, stage, wireframe));

			{
				const char* ptr = stage_source.c_str();
				glShaderSource(shad, 1, &ptr, NULL);
			}

			glCompileShader(shad);

			error = check_shader(shad, name) || error;

			compiled_stages.push_back(shad);
		}

		if (!error) { // skip linking if stage has error
			glLinkProgram(prog);
		
			error = check_program(prog, name);
		
			shad.uniforms = get_uniforms(prog);
			//CommonUniforms::setup_shader_ubo(prog);
		}

		for (auto stage : compiled_stages) {
			glDetachShader(prog, stage);
			glDeleteShader(stage);
		}

		if (error) {
			fprintf(stderr, "[Shaders] \"%s\": shader compilation error!\n", name);

			glDeleteProgram(prog);
			shad.prog = 0;
		}
		shad.prog = prog;
		return !error;
	}
}

void take_screenshot (int2 size) {
	Image<srgb8> img (size);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0,0, size.x, size.y, GL_RGB, GL_UNSIGNED_BYTE, img.pixels);

	time_t t = time(0); // get time now
	struct tm* now = localtime(&t);

	char timestr [80];
	strftime(timestr, 80, "%g%m%d-%H%M%S", now); // yy-mm-dd_hh-mm-ss

	static int counter = 0; // counter to avoid overwriting files in edge cases
	auto filename = prints("../screenshots/screen_%s_%d.jpg", timestr, counter++);
	counter %= 100;

	stbi_flip_vertically_on_write(true);
	stbi_write_jpg(filename.c_str(), size.x, size.y, 3, img.pixels, 95);
}

} // namespace ogl

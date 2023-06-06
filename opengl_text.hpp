#pragma once
#include "common.hpp"
#include "opengl.hpp"

#include "kisslib/stb_rect_pack.hpp"
#include "kisslib/stb_truetype.hpp"

namespace ogl {

struct TextRenderer {

// Config data
	
	struct GlyphSet {
		static constexpr int ASCII_FIRST = ' ';
		static constexpr int ASCII_LAST  = '~';
		static constexpr int ASCII_COUNT = ASCII_LAST+1 - ASCII_FIRST;

		std::vector<int> codepoints;
		std::unordered_map<int, int> codepoint2glyph;

		int get_glyph (int codepoint) { // codepoint to chardata index
			// get ascii glyphs through special case instead of codepoint2glyph to speed up ascii text
			int idx = codepoint - ASCII_FIRST;
			if (idx >= 0 && idx < ASCII_COUNT)
				return idx + 1; // +1 for missing-glyph at index 0

			auto it = codepoint2glyph.find(codepoint);
			if (it != codepoint2glyph.end())
				return it->second; // codepoint was registered in unicode_glpyhs

			return 0; // missing codepoint
		}

		// always contains ASCII glyphs
		// unicode_glyphs should be a utf8 strings that contains all the unicode glpyhs desired
		// (duplicate glyphs in this strings are deduplicated)
		GlyphSet (const char* unicode_glpyhs=nullptr) {
			
			codepoints.resize(1 + ASCII_COUNT);

			codepoints[0] = 0; // 0 codepoint will return missing-glyph glyph from font

			for (int i=0; i<ASCII_COUNT; ++i)
				codepoints[i+1] = ASCII_FIRST + i;

			if (unicode_glpyhs) {
				const char* cur = unicode_glpyhs;
				while (*cur != '\0') {
					int codepoint = kiss::utf8_decode_single(cur);
					auto res = codepoint2glyph.emplace(codepoint, (int)codepoints.size());
					if (res.second) // codepoint was unique
						codepoints.push_back(codepoint);
				}
			}
		}
	};
	GlyphSet glyph_set;

	std::string font_path;
	float       font_res;

	bool enable_sdf;
	
	int2 max_atlas_size = int2(1024, 1024*8);
	int2 atlas_size;

	bool   sdf_outline = true;
	float4 sdf_outline_col = float4(0,0,0,1);
	float  sdf_outline_w = 3; // in texels relative to font_res
	float  sdf_grow = 0;

	int   sdf_onedge = 64; // uint8_t values in SDF on the edge of glyphs
	float sdf_scale = 10.0f; // how much uint8_t values in SDF change per SDF texel
	float sdf_padding = 0.1f; // padding_in_pixels = font_size * sdf_padding
	
	int pixel_padding = 4; // for non-sdf
	//int mipmaps = 4;

	TextRenderer (char const* font_path = "fonts/DroidSerif-WmoY.ttf", float font_res = 64, bool sdf = true,
			const char* unicode_glyphs=nullptr): glyph_set{unicode_glyphs} { 

		this->font_path = font_path;
		this->font_res = font_res;
		this->enable_sdf = sdf;

		generate();
	}

// Generated data for drawing
	std::vector<stbtt_packedchar> chardata;

	float line_ascent, line_descent;
	float line_advance;
	float right_padding;

// Opengl objects
	Shader* shad;
	Texture2D atlas_tex = {"TextRenderer::atlas_tex"};

	struct GlyphQuad {
		float2 pos;

		VERTEX_CONFIG(
			ATTRIB(FLT2, GlyphQuad, pos),
		)
	};
	static constexpr GlyphQuad GLYPH_QUAD[4] = {
		{{ 0,1 }},
		{{ 1,1 }},
		{{ 1,0 }},
		{{ 0,0 }},
	};

	struct GlyphInstance {
		float4 px_rect; // (x0,y0, x1,y1)  in screen pixels, top-down y axis
		float4 uv_rect; // (x0,y0, x1,y1)
		float4 text_col;
		
		VERTEX_CONFIG_INSTANCED(
			ATTRIB(FLT4, GlyphInstance, px_rect),
			ATTRIB(FLT4, GlyphInstance, uv_rect),
			ATTRIB(FLT4, GlyphInstance, text_col),
		)
	};

	static VertexBufferInstancedI create_vbo () {
		VertexBufferInstancedI buf = {"text"};
		
		glBindVertexArray(buf.vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.ebo);

		glBindBuffer(GL_ARRAY_BUFFER, buf.vbo);
		setup_vao_attribs(GlyphQuad::attribs(), 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, buf.instances);
		setup_vao_attribs(GlyphQuad::attribs(), 1, offsetof(GlyphInstance, px_rect));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		upload_buffer(GL_ARRAY_BUFFER        , buf.vbo, sizeof(GLYPH_QUAD), GLYPH_QUAD);
		upload_buffer(GL_ELEMENT_ARRAY_BUFFER, buf.ebo, sizeof(render::shapes::QUAD_INDICES), render::shapes::QUAD_INDICES);

		return buf;
	}
	VertexBufferInstancedI vbo = create_vbo();

	std::vector<GlyphInstance> glyph_instances;

	void imgui () {
		if (!imgui_Header("Text Renderer")) return;

		ImGui::InputText("font_path", &font_path);
		bool changed = ImGui::Button("Load");

		changed = ImGui::SliderFloat("font_res", &font_res, 2, 80) || changed;

		changed = ImGui::Checkbox("enable_sdf", &enable_sdf) || changed;

		changed = ImGui::Checkbox("sdf_outline", &sdf_outline) || changed;
		ImGui::ColorEdit4("sdf_outline_col", &sdf_outline_col.x);
		ImGui::DragFloat("sdf_outline_w", &sdf_outline_w, 0.02f);
		ImGui::DragFloat("sdf_grow", &sdf_grow, 0.02f);

		if (changed)
			generate();

		ImGui::PopID();
	}

	bool generate ();
	void get_line_metrics (stbtt_fontinfo const& info, float scale);
	bool generate_bitmap (Image<uint8_t>& atlas, stbtt_pack_range* ranges, int num_ranges, const unsigned char* ttf_file);
	bool generate_sdf    (Image<uint8_t>& atlas, stbtt_pack_range* ranges, int num_ranges, const unsigned char* ttf_file);
	void upload_texture  (Image<uint8_t>& atlas);

	void begin () {
		glyph_instances.clear();
	}

	// core text drawing function
	// draw <text> glyphs with <color>
	// start at <pos> and modifiy <pos> when drawing, such that multple draw_text can be chained (init <pos> to 0)
	// '\n' or '\r\n' character sequences in the text will reset <pos.x> to 0 and increment <pos.y> to the next line
	// <x_max> keeps the maximum x value of the glyph bounds
	// to allow determining the bounding box of the entire text after the fact
	int generate_glyphs (std::string_view text, float font_size, float4 const& color, float2* pos, float* boundsx);

	void offset_glyphs (int idx, int len, float2 const& offset);

	struct PreparedText {
		// range of generated glyphs in glyphs array
		int idx, len;
		float2 bounds;
	};
	PreparedText prepare_text (std::string_view text, float font_size, float4 const& color) {
		PreparedText t;
		float scale = font_size / font_res;

		float2 pos = 0; // start at 0,0
		pos.y += line_ascent * scale; // account for baseline position so that top of glyph is at y=0

		t.bounds.x = 0; // keep track of max glpyh x1

		t.idx = (int)glyph_instances.size(); // store index of sequence of glpyhs to be generated
		t.len = generate_glyphs(text, font_size, color, &pos, &t.bounds.x);

		pos.y += line_descent * scale; // acount for line decent so that pos.y is now bottom of last line of glyphs
		
		t.bounds.y = pos.y; // size is max t.size.x and 
		return t;
	}

	void align_text (PreparedText const& prep_text, float2 const& base_pos, float2 const& align=0, float2 const& padding=0) {
		float2 padded_bounds = prep_text.bounds + padding * 2.0f;
		float2 offset = base_pos + padding - padded_bounds * align;

		offset_glyphs(prep_text.idx, prep_text.len, offset);
	}

	int draw_text (std::string_view text, float font_size, float4 const& color,
			float2 const& base_pos, float2 const& align=0, float2 const& align_padding=0) {
		ZoneScoped;
		auto ptext = prepare_text(text, font_size, color);
		align_text(ptext, base_pos, align, align_padding);
		return ptext.len;
	}

	void render (StateManager& state);
};

} // namespace ogl

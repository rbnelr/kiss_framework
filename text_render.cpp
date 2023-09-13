#include "common.hpp" // for PCH
#include "text_render.hpp"

Image<uint8_t> TextRenderer::generate_tex () {
	ZoneScoped;

	stbtt_pack_range range = {};
	{
		chardata.resize(glyph_set.codepoints.size());

		range.font_size                        = font_res;
		range.first_unicode_codepoint_in_range = 0;
		range.num_chars                        = (int)glyph_set.codepoints.size();
		range.array_of_unicode_codepoints      = glyph_set.codepoints.data();
		range.chardata_for_range               = chardata.data();
	}

	// use cpu atlas size of max_atlas_size
	// but keep track of actual used size and resize texture later to be as small (height) as possible
	Image<uint8_t> atlas(max_atlas_size);

	{
		uint64_t ttfsz;
		auto ttfdata = load_binary_file(font_path.c_str(), &ttfsz);
		if (!ttfdata.get()) {
			fprintf(stderr, "font file \"%s\" not found!\n", font_path.c_str());

			// try to at least render something if font is not found
			atlas.set(0,0, 255);
			atlas_size = 1;

		} else {

			if (!enable_sdf) {
				if (!generate_bitmap(atlas, &range, 1, ttfdata.get())) {
					fprintf(stderr, "error during init_bitmap()!\n");
				}
			} else {
				if (!generate_sdf(atlas, &range, 1, ttfdata.get())) {
					fprintf(stderr, "error during init_sdf()!\n");
				}
			}
		}
	}

	//
	return atlas;
}

void TextRenderer::get_line_metrics (stbtt_fontinfo const& info, float scale) {
	int ascent, descent, line_gap;
	stbtt_GetFontVMetrics(&info, &ascent, &descent, &line_gap);
	line_advance = scale * (float)(ascent - descent + line_gap);
	line_ascent  = scale * (float)ascent;
	line_descent = scale * (float)-descent;
}

bool TextRenderer::generate_bitmap (Image<uint8_t>& atlas, stbtt_pack_range* ranges, int num_ranges, const unsigned char* ttf_file) {
	ZoneScoped;

	{
		stbtt_pack_context spc;
		if (!stbtt_PackBegin(&spc, atlas.pixels, atlas.size.x,atlas.size.y, 0, pixel_padding, nullptr))
			return false;
		//stbtt_PackSetOversampling(&spc, 2,2);

		if (!stbtt_PackFontRanges(&spc, ttf_file, 0, ranges, num_ranges))
			return false;

		stbtt_PackEnd(&spc);
	}

	{ // find how many rows of the atlas were actually filled (stb_rect_pack fills them from the bottom)
		int max_y1 = 0;
		for (auto& c : chardata)
			max_y1 = max(c.y1, max_y1);

		assert(max_y1 <= max_atlas_size.y);

		atlas_size.x = max_atlas_size.x;
		atlas_size.y = min(max_y1, max_atlas_size.y);
	}

	{
		right_padding = 0; // bitmap packed glyphs do not render with padding (padding is during packing)

		stbtt_fontinfo info;
		if (!stbtt_InitFont(&info, ttf_file, stbtt_GetFontOffsetForIndex(ttf_file, 0)))
			return false;

		float scale = stbtt_ScaleForPixelHeight(&info, font_res);

		get_line_metrics(info, scale);
	}
	return true;
}

struct SDF_builder {
	int padding;
	float scale;

	stbtt_pack_range* ranges;
	int num_ranges;

	int sdf_onedge = 64; // uint8_t values in SDF on the edge of glyphs
	float sdf_scale = 10.0f; // how much uint8_t values in SDF change per SDF texel

	stbtt_fontinfo info = {};
	
	stbtt_packedchar* chardata;

	std::unique_ptr<stbrp_rect []> rects;
	std::unique_ptr<unsigned char* []> glyph_sdfs;

	int gather_sdf_rects () {
		ZoneScoped;

		int num_rects = 0;
		for (int ri=0; ri<num_ranges; ++ri)
			num_rects += ranges[ri].num_chars;

		rects = std::unique_ptr<stbrp_rect[]>(new stbrp_rect[num_rects]);
		glyph_sdfs = std::unique_ptr<unsigned char*[]>(new unsigned char*[num_rects]);

		bool missing_glyph_added = false;

		int rect_i = 0;
		for (int ri=0; ri<num_ranges; ++ri) {
			auto& range = ranges[ri];

			for (int ci=0; ci<range.num_chars; ++ci) {
				auto& rect = rects[rect_i];
				auto& glyph_sdf = glyph_sdfs[rect_i];
				stbtt_packedchar *bc = &range.chardata_for_range[ci];

				int codepoint = range.array_of_unicode_codepoints == NULL ? range.first_unicode_codepoint_in_range + ci : range.array_of_unicode_codepoints[ci];
				int glyph = stbtt_FindGlyphIndex(&info, codepoint);

				int w=0,h=0, xoff=0, yoff=0;
				glyph_sdf = nullptr;

				if (glyph != 0 || !missing_glyph_added) {
					glyph_sdf = stbtt_GetGlyphSDF(&info, scale, glyph, padding,
						sdf_onedge, sdf_scale, &w,&h, &xoff,&yoff);

					if (glyph == 0)
						missing_glyph_added = true;
				}

				rect.w = w;
				rect.h = h;

				int advance, lsb;
				stbtt_GetGlyphHMetrics(&info, glyph, &advance, &lsb);

				bc->xadvance = scale * advance;
				bc->xoff     = (float) xoff;
				bc->yoff     = (float) yoff;
				bc->xoff2    = (float)(xoff + w);
				bc->yoff2    = (float)(yoff + h);

				rect_i++;
			}
		}

		return rect_i;
	}
	int2 finish_rects (Image<uint8_t>& atlas) {
		ZoneScoped;

		int max_y1 = 0;

		int missing_glyph_i = -1;

		int rect_i = 0;
		for (int ri=0; ri<num_ranges; ++ri) {
			auto& range = ranges[ri];

			for (int ci=0; ci<range.num_chars; ++ci) {
				auto& rect = rects[rect_i];
				auto& glyph_sdf = glyph_sdfs[rect_i];
				stbtt_packedchar *bc = &range.chardata_for_range[ci];

				int codepoint = range.array_of_unicode_codepoints == NULL ? range.first_unicode_codepoint_in_range + ci : range.array_of_unicode_codepoints[ci];
				int glyph = stbtt_FindGlyphIndex(&info, codepoint);

				if (rect.was_packed && glyph_sdf) {
					{ // copy glyph pixels into atlas at the location of the packed rect
						uint8_t* dst = atlas.pixels + rect.y*atlas.size.x + rect.x;
						uint8_t* src = glyph_sdf;

						for (int y=0; y<rect.h; ++y) {
							memcpy(dst, src, rect.w * sizeof(uint8_t));

							dst += atlas.size.x;
							src += rect.w;
						}
					}

					bc->x0 = (unsigned short) rect.x;
					bc->y0 = (unsigned short) rect.y;
					bc->x1 = (unsigned short)(rect.x + rect.w);
					bc->y1 = (unsigned short)(rect.y + rect.h);

					if (glyph == 0)
						missing_glyph_i = rect_i;
				} else if (missing_glyph_i >= 0) {
					// glyph is empty because it was missing from the font (or the special missing-glyph glyph)
					*bc = chardata[missing_glyph_i]; // turn every missing glyph the missing-glyph glyph
				} else {
					// glyph is empty and no missing_glyph encountered yet -> actually empty glyph in font, ie. space char
					bc->x0 = 0;
					bc->y0 = 0;
					bc->x1 = 0;
					bc->y1 = 0;
				}

				max_y1 = max(max_y1, bc->y1);

				free(glyph_sdf);
				rect_i++;
			}
		}

		// find how many rows of the atlas were actually filled (stb_rect_pack fills them from the bottom)
		assert(max_y1 <= atlas.size.y);

		int2 atlas_size;
		atlas_size.x = atlas.size.x;
		atlas_size.y = min(max_y1, atlas.size.y);

		return atlas_size;
	}
};
bool TextRenderer::generate_sdf (Image<uint8_t>& atlas, stbtt_pack_range* ranges, int num_ranges, const unsigned char* ttf_file) {
	ZoneScoped;

	SDF_builder sdf;
	sdf.ranges = ranges;
	sdf.num_ranges = num_ranges;

	sdf.padding = (int)((float)font_res * sdf_padding);

	sdf.chardata = chardata.data();

	if (!stbtt_InitFont(&sdf.info, ttf_file, stbtt_GetFontOffsetForIndex(ttf_file, 0)))
		return false;
	sdf.scale = stbtt_ScaleForPixelHeight(&sdf.info, font_res);

	right_padding = (float)sdf.padding; // padding is rendered in SDF glyphs

	get_line_metrics(sdf.info, sdf.scale);

	int num_rects = sdf.gather_sdf_rects();

	bool all_packed;
	{
		int num_nodes = atlas.size.x;
		auto nodes = std::unique_ptr<stbrp_node[]>(new stbrp_node[num_nodes]);

		stbrp_context rpc;
		stbrp_init_target(&rpc, atlas.size.x, atlas.size.y, nodes.get(), num_nodes);

		all_packed = stbrp_pack_rects(&rpc, sdf.rects.get(), num_rects);
	}

	atlas.clear(0);
	atlas_size = sdf.finish_rects(atlas);

	return all_packed;
}

int TextRenderer::generate_glyphs (std::string_view text, float font_size, float4 const& color, float2* pos, float* boundsx) {
	float ipw = 1.0f / (float)atlas_size.x;
	float iph = 1.0f / (float)atlas_size.y;
	float scale = font_size / font_res;
	
	char const* cur = text.data();
	char const* end = text.data() + text.size();

	int glyph_idx = (int)glyph_instances.size();

	while (cur < end) {
		if (cur[0] == '\r' || cur[0] == '\n') {
			// skip either one '\n' or "\r\n"
			if (*cur == '\r') cur++;
			if (*cur == '\n') cur++;

			pos->x = 0;
			pos->y += line_advance * scale;
			continue;
		}

		int codepoint = utf8_decode_single(cur);

		int glyph = glyph_set.get_glyph(codepoint);

		auto& b = chardata[glyph];

		int w = b.x1 - b.x0;
		int h = b.y1 - b.y0;

		float next_x = pos->x + b.xadvance * scale;
		// x += scale*stbtt_GetCodepointKernAdvance(&font, text[ch],text[ch+1]);

		// check c because stbtt_PackFontRanges() still generates a 1x1 quad for ' ' (because padding=1 I assume)
		if (codepoint != ' ' && w>0 && h>0) {
			auto* glyph = push_back(glyph_instances, 1);

			glyph->px_rect.x = pos->x + b.xoff  * scale;
			glyph->px_rect.y = pos->y + b.yoff  * scale;
			glyph->px_rect.z = pos->x + b.xoff2 * scale;
			glyph->px_rect.w = pos->y + b.yoff2 * scale;
			glyph->uv_rect.x = b.x0 * ipw;
			glyph->uv_rect.y = b.y0 * iph;
			glyph->uv_rect.z = b.x1 * ipw;
			glyph->uv_rect.w = b.y1 * iph;

			glyph->text_col = color;

			*boundsx = max(*boundsx, pos->x + (b.xoff2 * 1.25f - right_padding) * scale);
		} else {
			// space character (or missing glyph for other reasons?)
			*boundsx = max(*boundsx, next_x); // use next basepoint for bounding box
		}

		pos->x = next_x;
	}

	return (int)glyph_instances.size() - glyph_idx;
}

void TextRenderer::offset_glyphs (int idx, int len, float2 const& offset) {
	for (int i=idx; i<idx+len; ++i) {
		auto& glyph = glyph_instances[i];
		glyph.px_rect.x += offset.x;
		glyph.px_rect.y += offset.y;
		glyph.px_rect.z += offset.x;
		glyph.px_rect.w += offset.y;
	}
}


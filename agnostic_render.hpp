#pragma once
#include "camera.hpp"

namespace render {
	
struct Attribute {
	enum Type {
		FLT, INT, UBYTE,
		UBYTE_UNORM,
	};

	Type type; // for example type,components like Attribute::FLT,3
	int components;
	int stride; // usually sizeof(Vertex_struct)
	int offset; // usually offsetof(Vertex_struct, member)
};

template <size_t N>
struct VertexAttributes {
	std::array<Attribute, N> attribs;
};

template <size_t N>
inline constexpr VertexAttributes<N> _make_attribs (std::array<render::Attribute, N> attribs) { return {attribs}; }

/* Use like:
	struct Vertex {
		float3 pos;
		float2 uv;
		float4 col;

		VERTEX_CONFIG(
			ATTRIB(FLT3, Vertex, pos);
			ATTRIB(FLT2, Vertex, uv);
			ATTRIB(FLT4, Vertex, col);
		)
	};
*/
#define VERTEX_CONFIG(...)           static inline constexpr auto attribs () { return _make_attribs(std::array{__VA_ARGS__}); }
#define ATTRIB(type, comps, VertexType, member) render::Attribute{render::Attribute::type, comps, (int)sizeof(VertexType), (int)offsetof(VertexType, member)}

namespace shapes {
	// Generic Vertex with all info for common shapes, use copy out only the info you need
	struct GenericVertex {
		float3 pos;
		float3 norm;
		float2 uv;
		// TODO: tangent ?

		VERTEX_CONFIG(
			ATTRIB(FLT,3, GenericVertex, pos),
			ATTRIB(FLT,3, GenericVertex, norm),
			ATTRIB(FLT,2, GenericVertex, uv),
		)
	};

	// Quad [0,1] pointing at Z
	inline constexpr GenericVertex QUAD_VERTICES[] = {
		{ {0,0,0}, {0,0,1}, {0,0} },
		{ {0,1,0}, {0,0,1}, {0,1} },
		{ {1,1,0}, {0,0,1}, {1,1} },
		{ {1,0,0}, {0,0,1}, {1,0} },
	};

	// D---C
	// | / |
	// A---B
	inline constexpr uint16_t QUAD_INDICES[] = {
		1,2,0, 0,2,3
	};
	// D---C
	// | \ |
	// A---B
	inline constexpr uint16_t QUAD_INDICES_ROT[] = {
		0,1,3, 1,2,3
	};

	#define REND_QUAD_INDICES(a,b,c,d)     (b), (c), (a), (a), (c), (d)
	#define REND_QUAD_INDICES_ROT(a,b,c,d) (a), (b), (d), (b), (c), (d)

	template <typename T>
	inline void push_quad_indices (std::vector<T>& vec, T a, T b, T c, T d) {
		T* indx = push_back(vec, 6);
		indx[0] = b;
		indx[1] = c;
		indx[2] = a;
		indx[3] = a;
		indx[4] = c;
		indx[5] = d;
	}
	
	inline constexpr uint16_t WIRE_QUAD_INDICES[] = {
		0,1,  1,2,  2,3,  3,0,
	};

	// Quad [0,1] pointing at Z
	inline constexpr float3 CUBE_VERTICES[] = {
		{0,0,0},
		{1,0,0},
		{1,1,0},
		{0,1,0},
		{0,0,1},
		{1,0,1},
		{1,1,1},
		{0,1,1},
	};
	inline constexpr float3 CUBE_CENTERED_VERTICES[] = {
		{-0.5f,-0.5f,-0.5f},
		{+0.5f,-0.5f,-0.5f},
		{+0.5f,+0.5f,-0.5f},
		{-0.5f,+0.5f,-0.5f},
		{-0.5f,-0.5f,+0.5f},
		{+0.5f,-0.5f,+0.5f},
		{+0.5f,+0.5f,+0.5f},
		{-0.5f,+0.5f,+0.5f},
	};
	
	inline constexpr uint16_t CUBE_INDICES[] = {
		REND_QUAD_INDICES(0,1,5,4),
		REND_QUAD_INDICES(1,2,6,5),
		REND_QUAD_INDICES(2,3,7,6),
		REND_QUAD_INDICES(3,0,4,7),
		REND_QUAD_INDICES(4,5,6,7),
		REND_QUAD_INDICES(3,2,1,0),
	};

	inline constexpr uint16_t WIRE_CUBE_INDICES[] {
		// bottom lines
		0,1,  1,2,  2,3,  3,0,
		// vertical lines
		0,4,  1,5,  2,6,  3,7,
		// top lines
		4,5,  5,6,  6,7,  7,4,
	};

	
	void wire_sphere (std::vector<float3>* vertices, std::vector<uint16_t>* indices, float r, int segments);
}

}
#include "text_render.hpp" // ugh, but I hate having to break down files even more just to fix circular include
namespace render {

struct DebugDraw {
	struct LineVertex {
		float3 pos;
		float4 col;

		VERTEX_CONFIG(
			ATTRIB(FLT,3, LineVertex, pos),
			ATTRIB(FLT,4, LineVertex, col),
		)
	};
	struct TriVertex {
		float3 pos;
		float3 norm;
		float4 col;

		VERTEX_CONFIG(
			ATTRIB(FLT,3, TriVertex, pos),
			ATTRIB(FLT,3, TriVertex, norm),
			ATTRIB(FLT,4, TriVertex, col),
		)
	};

	static constexpr lrgba COLS[] = {
		{1,0,0,1},
		{0,1,0,1},
		{0,0,1,1},
		{1,1,0,1},
		{1,0,1,1},
		{0,1,1,1},

		{1.0f, 0.5f, 0.5f, 1},
		{0.5f, 1.0f, 0.5f, 1},
		{0.5f, 0.5f, 1.0f, 1},
		{1.0f, 1.0f, 0.5f, 1},
		{1.0f, 0.5f, 1.0f, 1},
		{0.5f, 1.0f, 1.0f, 1},
	};

	std::vector<LineVertex> lines;
	std::vector<TriVertex> tris;

	void clear () {
		// only dealloc if no lines were drawn
		if (lines.size() == 0)
			lines.shrink_to_fit();
		if (tris.size() == 0)
			tris.shrink_to_fit();

		lines.clear();
		tris.clear();
	}

	void point (float3 const& pos, float3 const& size, lrgba const& col);
	void line (float3 const& a, float3 const& b, lrgba const& col);
	// arrowhead facing camera
	void arrow (View3D const& view, float3 const& pos, float3 const& dir, float head_sz, lrgba const& col);
	// flat, arrowhead lying down
	void arrow (float3 const& pos, float3 const& dir, float head_sz, lrgba const& col);

	void wire_quad (float3 const& pos, float2 size, lrgba const& col);
	void wire_cube (float3 const& pos, float3 const& size, lrgba const& col);
	void wire_circle (float3 const& pos, float r, lrgba const& col, int angres=32);
	void wire_sphere (float3 const& pos, float r, lrgba const& col, int angres=32, int wires=16);
	void wire_sphere (float3x4 const& mat, float r, lrgba const& col, int angres=32, int wires=16);
	void wire_cone (float3 const& pos, float ang, float length, float3x3 const& rot, lrgba const& col, int circres=16, int wires=8);
	//void wire_frustrum (Camera_View const& view, lrgba const& col);

	void quad (float3 const& pos, float2 size, lrgba const& col);
	void cylinder (float3 const& base, float radius, float height, lrgba const& col, int sides=32);

	void axis_gizmo (View3D const& view, int2 const& viewport_size);

	TextRenderer text;
};

// use in backend Renderbuffer
struct RenderScale_with_MSAA {
	SERIALIZE(RenderScale_with_MSAA, renderscale, MSAA, nearest)

	int2 size = -1;
	float renderscale = 1.0f;

	int MSAA = 1;

	bool nearest = false;

	bool changed = false;
	
	void imgui () {
		if (ImGui::TreeNode("RenderScale")) {

			ImGui::Text("res: %4d x %4d px (%5.2f Mpx)", size.x, size.y, (float)(size.x * size.y) / (1000*1000));
			changed = ImGui::SliderFloat("renderscale", &renderscale, 0.02f, 2.0f);

			//changed = ImGui::Checkbox("depth_float32", &depth_float32) || changed;
			changed = ImGui::Checkbox("nearest", &nearest) || changed;

			changed = ImGui::SliderInt("MSAA", &MSAA, 1, 16) || changed;

			ImGui::TreePop();
		}
	}

	bool update (int2 output_size) {
		auto old_size = size;
		size = max(1, roundi((float2)output_size * renderscale));

		bool upd = old_size != size || changed;
		changed = false;
		return upd;
	}
};
// use in backend Renderbuffer
struct RenderScale {
	SERIALIZE(RenderScale, renderscale, nearest)

	int2 size = -1;
	float renderscale = 1.0f;

	bool nearest = false;

	bool changed = false;
	
	void imgui () {
		if (ImGui::TreeNode("RenderScale")) {

			ImGui::Text("res: %4d x %4d px (%5.2f Mpx)", size.x, size.y, (float)(size.x * size.y) / (1000*1000));
			changed = ImGui::SliderFloat("renderscale", &renderscale, 0.02f, 2.0f);

			//changed = ImGui::Checkbox("depth_float32", &depth_float32) || changed;
			changed = ImGui::Checkbox("nearest", &nearest) || changed;

			ImGui::TreePop();
		}
	}

	bool update (int2 output_size) {
		auto old_size = size;
		size = max(1, roundi((float2)output_size * renderscale));

		bool upd = old_size != size || changed;
		changed = false;
		return upd;
	}
};

} // namespace render

inline render::DebugDraw g_dbgdraw; // really need something like this to be global

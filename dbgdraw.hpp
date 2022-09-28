#pragma once
#include "common.hpp"
#include "camera.hpp"

struct DebugDraw {
	static constexpr lrgba COLS[] = {
		{1,0,0,1},
		{0,1,0,1},
		{0,0,1,1},
		{1,1,0,1},
		{1,0,1,1},
		{0,1,1,1},
	};

	struct LineVertex {
		float3 pos;
		float4 col;

		static void attrib (int idx);
	};
	struct TriVertex {
		float3 pos;
		float3 normal;
		float4 col;

		static void attrib (int idx);
	};

	std::vector<LineVertex> lines;
	std::vector<TriVertex> tris;
	
	static constexpr float2 _wire_quad_vertices[] {
		float2(0,0),
		float2(1,0),
		float2(1,1),
		float2(0,1),
	};
	static constexpr uint16_t _wire_quad_indices[] {
		0,1,  1,2,  2,3,  3,0,
	};

	static constexpr float3 _wire_cube_vertices[] {
		float3(0,0,0),
		float3(1,0,0),
		float3(1,1,0),
		float3(0,1,0),
		float3(0,0,1),
		float3(1,0,1),
		float3(1,1,1),
		float3(0,1,1),
	};
	static constexpr uint16_t _wire_cube_indices[] {
		// bottom lines
		0,1,  1,2,  2,3,  3,0,
		// vertical lines
		0,4,  1,5,  2,6,  3,7,
		// top lines
		4,5,  5,6,  6,7,  7,4,
	};
	
	static void gen_simple_wire_sphere (std::vector<float3>* vertices, std::vector<uint16_t>* indices, float r, int segments) {
		int count = 3 * segments;

		auto* vert = push_back(*vertices, 3 * segments);
		auto* ind = push_back(*indices, 3 * segments*2);
		
		float ang_step = deg(360) / (float)segments;

		for (int i=0; i<segments; ++i) {
			float s = sin((float)i * ang_step);
			float c = cos((float)i * ang_step);

			vert[i           ] = float3(c,s,0) * r;
			vert[i+segments  ] = float3(c,0,s) * r;
			vert[i+segments*2] = float3(0,c,s) * r;
		}

		uint16_t base = 0;
		for (int i=0; i<segments; ++i) {
			*ind++ = base + (uint16_t)( i );
			*ind++ = base + (uint16_t)((i+1) == segments ? 0 : i+1);
		}
		base += segments;
		for (int i=0; i<segments; ++i) {
			*ind++ = base + (uint16_t)( i );
			*ind++ = base + (uint16_t)((i+1) == segments ? 0 : i+1);
		}
		base += segments;
		for (int i=0; i<segments; ++i) {
			*ind++ = base + (uint16_t)( i );
			*ind++ = base + (uint16_t)((i+1) == segments ? 0 : i+1);
		}
	}
	
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
	void vector (float3 const& pos, float3 const& dir, lrgba const& col);

	void wire_quad (float3 const& pos, float2 size, lrgba const& col);
	void wire_cube (float3 const& pos, float3 const& size, lrgba const& col);
	void wire_sphere (float3 const& pos, float r, lrgba const& col, int angres=32, int wires=16);
	void wire_cone (float3 const& pos, float ang, float length, float3x3 const& rot, lrgba const& col, int circres=16, int wires=8);
	//void wire_frustrum (Camera_View const& view, lrgba const& col);
	
	void quad (float3 const& pos, float2 size, lrgba const& col);
	void cylinder (float3 const& base, float radius, float height, lrgba const& col, int sides=32);
	
	void axis_gizmo (View3D const& view, int2 const& viewport_size);
};

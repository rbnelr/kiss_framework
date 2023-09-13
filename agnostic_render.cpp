#include "common.hpp"
#include "agnostic_render.hpp"

namespace render {

void DebugDraw::line (float3 const& a, float3 const& b, lrgba const& col) {
	auto* out = push_back(lines, 2);

	*out++ = { a, col };
	*out++ = { b, col };
}	
void DebugDraw::arrow (View3D const& view, float3 const& pos, float3 const& dir, float head_sz, lrgba const& col) {
	float3 forw_dir = normalizesafe(dir);

	float3 cam_dir = view.cam_pos - pos;
	float3 left_vec = normalizesafe(cross(forw_dir, cam_dir));
	
	float3 tip = pos + dir;
	auto* out = push_back(lines, 6);


	*out++ = { pos, col };
	*out++ = { tip, col };
	
	*out++ = { tip, col };
	*out++ = { tip + (+left_vec - forw_dir) * head_sz, col };

	*out++ = { tip, col };
	*out++ = { tip + (-left_vec - forw_dir) * head_sz, col };
}

void DebugDraw::point (float3 const& pos, float3 const& size, lrgba const& col) {
	auto* out = push_back(lines, 8);

	*out++ = { pos + size * float3(-1,-1,-1), col };
	*out++ = { pos + size * float3(+1,+1,+1), col };

	*out++ = { pos + size * float3(+1,-1,-1), col };
	*out++ = { pos + size * float3(-1,+1,+1), col };

	*out++ = { pos + size * float3(-1,+1,-1), col };
	*out++ = { pos + size * float3(+1,-1,+1), col };

	*out++ = { pos + size * float3(+1,+1,-1), col };
	*out++ = { pos + size * float3(-1,-1,+1), col };
}

void DebugDraw::wire_quad (float3 const& pos, float2 size, lrgba const& col) {
	auto* out = push_back(lines, ARRLEN(shapes::WIRE_QUAD_INDICES));

	for (auto& idx : shapes::WIRE_QUAD_INDICES) {
		auto& v = shapes::QUAD_VERTICES[idx];

		out->pos.x = v.pos.x * size.x + pos.x;
		out->pos.y = v.pos.y * size.y + pos.y;
		out->pos.z = pos.z;

		out->col = col;

		out++;
	}
}
void DebugDraw::wire_cube (float3 const& pos, float3 const& size, lrgba const& col) {
	auto* out = push_back(lines, ARRLEN(shapes::WIRE_CUBE_INDICES));

	for (auto& idx : shapes::WIRE_CUBE_INDICES) {
		auto& v = shapes::WIRE_CUBE_VERTICES[idx];

		out->pos.x = v.x * size.x + pos.x;
		out->pos.y = v.y * size.y + pos.y;
		out->pos.z = v.z * size.z + pos.z;

		out->col = col;

		out++;
	}
}

void DebugDraw::wire_circle (float3 const& pos, float r, lrgba const& col, int angres) {
	auto* out = push_back(lines, angres * 2);

	float ang_step = deg(360) / (float)angres;
	float s0=0, c0=1; // optimize not calling sin&cos 2x per loop
	for (int i=0; i<angres; ++i) {
		float t = (float)(i+1) * ang_step;

		float s1 = sin(t);
		float c1 = cos(t);

		out->pos = float3(c0, s0, 0.0f) * r + pos;
		out->col = col;
		out++;

		out->pos = float3(c1, s1, 0.0f) * r + pos;
		out->col = col;
		out++;

		s0 = s1;
		c0 = c1;
	}
}

void DebugDraw::wire_sphere (float3 const& pos, float r, lrgba const& col, int angres, int wires) {
	int wiresz = wires/2 -1; // one less wire, so we get even vertical wires and odd number of horiz wires, so that there is a 'middle' horiz wire
	int wiresxy = wires;

	int count = (wiresz + wiresxy) * angres * 2; // every wire is <angres> lines, <wires> * 2 because horiz and vert wires

	auto* out = push_back(lines, count);

	float ang_step = deg(360) / (float)angres;

	for (int i=0; i<count; ++i)
		out[i].col = col;

	auto set = [&] (float3 p) {
		out->pos = p * r + pos;
		out++;
	};

	for (int j=0; j<wiresz; ++j) {
		float a = (((float)j + 1) / (float)(wires/2) - 0.5f) * deg(180); // j +1 / (wires/2) gives us better placement of wires

		float sa = sin(a);
		float ca = cos(a);

		float sb0=0, cb0=1; // optimize not calling sin&cos 2x per loop
		for (int i=0; i<angres; ++i) {
			float b1 = (float)(i+1) * ang_step;

			float sb1 = sin(b1);
			float cb1 = cos(b1);

			set(float3(cb0 * ca, sb0 * ca, sa));
			set(float3(cb1 * ca, sb1 * ca, sa));

			sb0 = sb1;
			cb0 = cb1;
		}
	}

	for (int j=0; j<wiresxy; ++j) {
		float a = (float)j / (float)wiresxy * deg(360);
	
		float sa = sin(a);
		float ca = cos(a);

		float sb0=0, cb0=1; // optimize not calling sin&cos 2x per loop
		for (int i=0; i<angres; ++i) {
			float b1 = (float)(i+1) * ang_step;

			float sb1 = sin(b1);
			float cb1 = cos(b1);

			set(float3(cb0 * ca, cb0 * sa, sb0));
			set(float3(cb1 * ca, cb1 * sa, sb1));

			sb0 = sb1;
			cb0 = cb1;
		}
	}
}

void DebugDraw::wire_cone (float3 const& pos, float ang, float length, float3x3 const& rot, lrgba const& col, int circres, int wires) {
	int count = (circres + wires) * 2;

	auto* out = push_back(lines, count);

	float r = tan(ang * 0.5f);

	for (int i=0; i<count; ++i)
		out[i].col = col;

	auto set = [&] (float3 p) {
		out->pos = pos + (rot * p) * length;
		out++;
	};

	// circle of cone base
	float s0 = 0;
	float c0 = 1;
	for (int i=0; i<circres; ++i) {
		float ang = (float)(i + 1) * deg(360) / (float)circres;

		float s1 = sin(ang);
		float c1 = cos(ang);

		set(float3(c0*r, s0*r, 1));
		set(float3(c1*r, s1*r, 1));

		s0 = s1;
		c0 = c1;
	}

	// lines from tip to base
	for (int i=0; i<wires; ++i) {
		float ang = (float)(i + 1) * deg(360) / (float)wires;

		set(float3(0, 0, 0));
		set(float3(cos(ang)*r, sin(ang)*r, 1));
	}
}

void DebugDraw::quad (float3 const& pos, float2 size, lrgba const& col) {
	auto* out = push_back(tris, 6);
	
	*out++ = { pos + float3(size.x,      0, 0), float3(0,0,1), col };
	*out++ = { pos + float3(size.x, size.y, 0), float3(0,0,1), col };
	*out++ = { pos + float3(     0,      0, 0), float3(0,0,1), col };
	*out++ = { pos + float3(     0,      0, 0), float3(0,0,1), col };
	*out++ = { pos + float3(size.x, size.y, 0), float3(0,0,1), col };
	*out++ = { pos + float3(     0, size.y, 0), float3(0,0,1), col };
}

void DebugDraw::cylinder (float3 const& base, float radius, float height, lrgba const& col, int sides) {
	auto* out = push_back(tris, sides * 4 * 3); // tri for bottom + top cap + 2 tris for side

	float ang_step = 2*PI / (float)sides;

	float sin0=0, cos0=1; // optimize not calling sin 2x per loop

	auto push = [&] (float3 pos, float3 norm) {
		out->pos = pos * float3(radius, radius, height) + base;
		out->norm = norm;
		out->col = col;
		out++;
	};

	for (int i=0; i<sides; ++i) {
		float ang1 = (float)(i+1) * ang_step;

		float sin1 = sin(ang1);
		float cos1 = cos(ang1);

		push(float3(   0,    0, 0), float3(0, 0, -1));
		push(float3(cos1, sin1, 0), float3(0, 0, -1));
		push(float3(cos0, sin0, 0), float3(0, 0, -1));

		push(float3(cos1, sin1, 0), float3(cos1, sin1, 0));
		push(float3(cos1, sin1, 1), float3(cos1, sin1, 0));
		push(float3(cos0, sin0, 0), float3(cos0, sin0, 0));
		push(float3(cos0, sin0, 0), float3(cos0, sin0, 0));
		push(float3(cos1, sin1, 1), float3(cos1, sin1, 0));
		push(float3(cos0, sin0, 1), float3(cos0, sin0, 0));

		push(float3(   0,    0, 1), float3(0, 0, +1));
		push(float3(cos0, sin0, 1), float3(0, 0, +1));
		push(float3(cos1, sin1, 1), float3(0, 0, +1));

		sin0 = sin1;
		cos0 = cos1;
	}
}

namespace shapes {
	
	void wire_sphere (std::vector<float3>* vertices, std::vector<uint16_t>* indices, float r, int segments) {
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
}

void DebugDraw::axis_gizmo (View3D const& view, int2 const& viewport_size) {
	//float2 pos_px = float2(viewport_size.x - 100.5f, 100.5f);
	//float2 pos_clip = pos_px / (float2)viewport_size * 2.0f - 1.0f;
	float2 pos_clip = float2(+0.9f, -0.9f);

	float3 pos_cam = (float3)(view.clip2cam * float4(pos_clip, 0,1));
	float3 pos_world = (float3)(view.cam2world * float4(pos_cam,1));

	float size_clip = 0.05f;
	float3 pos_cam2 = (float3)(view.clip2cam * float4(pos_clip.x, pos_clip.y + size_clip, 0,1)).y; // size in cam = size in world
	float size_world = pos_cam2.y - pos_cam.y;

	arrow(view, pos_world, float3(size_world,0,0), size_world*0.15f, lrgba(1,0,0,1));
	arrow(view, pos_world, float3(0,size_world,0), size_world*0.15f, lrgba(0,1,0,1));
	arrow(view, pos_world, float3(0,0,size_world), size_world*0.15f, lrgba(0,0,1,1));
}

} // namespace render

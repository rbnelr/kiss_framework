#pragma once
#include "kissmath.hpp"

struct Line {
	float3 a, b;
};
struct Ray {
	float3 pos;
	float3 dir; // normalized
};
struct Plane {
	float3 pos;
	float3 normal;
};
struct CylinderZ {
	float3 pos;
	float  height;
	float  radius;
};

struct CollisionHit {
	float dist; // how far obj A moved relative to obj B to hit it
	float3 pos; // pos of obj A relative to obj B at collision time
	float3 normal; // normal of the collision pointing from obj B to obj A
};
struct RayEntryExit {
	float t0;
	float t1;
};

template <typename T>
struct AABB {
	T lo=+INF, hi=-INF;

	T center () {
		return (lo + hi) * 0.5f;
	}
	T size () {
		return hi - lo;
	}

	static AABB add (AABB const& l, T const& r) {
		AABB res;
		res.lo = ::min(l.lo, r);
		res.hi = ::max(l.hi, r);
		//assert(all(res.lo <= res.hi));
		return res;
	}
	static AABB add (AABB const& l, AABB const& r) {
		AABB res;
		res.lo = ::min(l.lo, r.lo);
		res.hi = ::max(l.hi, r.hi);
		//assert(all(res.lo <= res.hi));
		return res;
	}

	void add (T const& r) {
		*this = add(*this, r);
	}
	void add (AABB const& r) {
		*this = add(*this, r);
	}

	bool overlaps (AABB const& r) const {
		return all(hi >= r.lo && lo <= r.hi);
	}
};

using AABB3 = AABB<float3>;
using AABB2 = AABB<float2>;

struct RectInt {
	int2 lo=INT_MAX, hi=INT_MIN;
	
	static const RectInt EMPTY;
	static const RectInt INF;

	bool empty () const {
		return lo.x > hi.x || lo.y > hi.y;
	}
	
	static RectInt add (RectInt const& l, RectInt const& r) {
		RectInt res;
		res.lo = ::min(l.lo, r.lo);
		res.hi = ::max(l.hi, r.hi);
		assert(all(res.lo <= res.hi));
		return res;
	}
	void add (RectInt const& r) {
		*this = add(*this, r);
	}
};
inline constexpr RectInt RectInt::EMPTY = {};
inline constexpr RectInt RectInt::INF = {INT_MIN, INT_MAX};

// intersection test between circle and 1x1 square going from 0,0 to 1,1
inline bool circle_square_intersect (float2 const& circ_origin, float circ_radius) {

	float2 nearest_pos_on_square = clamp(circ_origin, 0,1);

	return length_sqr(nearest_pos_on_square -circ_origin) < circ_radius*circ_radius;
}
// intersection test between cylinder and 1x1x1 cube going from 0,0,0 to 1,1,1
// cylinder origin is at the center of the circle at the base of the cylinder (-z circle)
inline bool cylinder_cube_intersect (float3 const& cyl_origin, float cyl_radius, float cyl_height) {

	if (cyl_origin.z >= 1) return false; // cylinder above cube
	if (cyl_origin.z <= -cyl_height) return false; // cylinder below cube

	return circle_square_intersect((float2)cyl_origin, cyl_radius);
}

// nearest distance from point to square (square covers [square_pos, square_pos +square_size] on each axis)
inline float point_square_dist (float2 const& square_pos, float2 const& square_size, float2 const& point) {

	float2 pos_rel = point -square_pos;

	float2 nearest_pos_on_square = clamp(pos_rel, 0,square_size);

	return length(nearest_pos_on_square -pos_rel);
}

inline float point_box_dist_sqr (float3 const& box_pos, float3 const& box_size, float3 const& point) {

	float3 pos_rel = point - box_pos;

	float3 nearest_pos_on_square = clamp(pos_rel, 0, box_size);

	return length_sqr(nearest_pos_on_square - pos_rel);
}

// nearest distance from point to box (box covers [box_pos, box_pos + box_size] on each axis)
inline float point_box_dist (float3 const& box_pos, float3 const& box_size, float3 const& point) {
	return sqrt(point_box_dist_sqr(box_pos, box_size, point));
}

// distance of point to infinite line
inline float point_line_dist (float2 const& line_pos, float2 const& line_dir, float2 const& point) {

	float2 pos_rel = point - line_pos;

	float len_sqr = length_sqr(line_dir);
	float t = len_sqr == 0.0f ? 0 : dot(line_dir, pos_rel) / len_sqr;

	float2 projected = t * line_dir;
	float2 offset = pos_rel - projected;

	return length(offset);
}

// distance of point to line segment (line_dir allowed to be 0)
inline float point_line_segment_dist (float2 const& line_pos, float2 const& line_dir, float2 const& point, float2* out_point) {

	float2 pos_rel = point - line_pos;

	float len_sqr = length_sqr(line_dir);
	float t = len_sqr == 0.0f ? 0 : dot(line_dir, pos_rel) / len_sqr;

	t = clamp(t, 0.0f, 1.0f);

	float2 projected = t * line_dir;
	float2 offset = pos_rel - projected;

	if (out_point) *out_point = line_pos + line_dir * t;
	return length(offset);
}

// some black magic math I found online at some point
// returns closest point on infinite line to a ray passing it
// useful for axis translation gizmos
inline bool ray_line_closest_intersect (float3 const& ray_pos, float3 const& ray_dir, float3 const& line_pos, float3 const& line_dir,
	float3* intersect) {

	float3 cross_dir = cross(ray_dir, line_dir);
	float divisor = length_sqr(cross_dir);

	if (divisor == 0.0f)
		return false; // target ray and axis line are exactly parallel

	float inv = 1.0f / divisor;
	float3 rel = line_pos - ray_pos;

	float3x3 m;
	m.arr[0] = rel;
	m.arr[1] = line_dir;
	m.arr[2] = cross_dir;
	float t_ray = determinant(m);
	t_ray *= inv;

	if (t_ray < 0.0f)
		return false; // target ray points away from axis line (intersection would be backwards on the ray (behind the camera))

	m.arr[1] = ray_dir;
	float t_line = determinant(m);
	t_line *= inv;

	*intersect = line_pos + line_dir * t_line;
	return true;
}


inline bool intersect_cylinder_ray (CylinderZ const& cyl, Ray const& ray, RayEntryExit* hit) {
	float tz0 = -INF;
	float tz1 = +INF;
	if (ray.dir.z != 0) {
		float t0 = (cyl.pos.z              - ray.pos.z) * (1.0f / ray.dir.z);
		float t1 = (cyl.pos.z + cyl.height - ray.pos.z) * (1.0f / ray.dir.z);
		tz0 = min(t0, t1);
		tz1 = max(t0, t1);
	}
	else {
		// ray parallel
		if (ray.pos.z < cyl.pos.z || ray.pos.z > cyl.pos.z + cyl.height)
			return false; // miss
	}

	float2 rel = (float2)ray.pos - (float2)cyl.pos;

	float c = 1.0f / (ray.dir.x*ray.dir.x + ray.dir.y*ray.dir.y);

	float p = c * (ray.dir.x*rel.x + ray.dir.y*rel.y);
	float q = c * (rel.x*rel.x + rel.y*rel.y - cyl.radius*cyl.radius);

	float rt = p*p-q;
	if (rt < 0.0f)
		return false; // miss

	rt = sqrt(rt);
	float t0 = -p - rt;
	float t1 = -p + rt;

	t0 = max(tz0, t0);
	t1 = min(tz1, t1);

	t0 = max(t0, 0.0f);

	if (t0 > t1)
		return false;
	
	hit->t0 = t0;
	hit->t1 = t1;
	return true;
}

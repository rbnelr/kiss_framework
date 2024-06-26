#pragma once
#include "kissmath.hpp"

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

	bool overlap (AABB const& r) const {
		return all(hi >= r.lo && lo <= r.hi);
	}
};

using AABB3 = AABB<float3>;
using AABB2 = AABB<float2>;

struct View_Frustrum {
	// The view frustrum planes in world space
	// in the order: near, left, right, bottom, up, far
	// far was put last because it is not really needed in view frustrum culling since it is undesirable for the far plane to intersect any geometry anyway, so it usually gets get to far enough to never cull anything
	Plane		planes[6];

	// Frustrum corners in world space
	// in the order: LBN, RBN, RTN, LTN, LBF, RBF, RTF, LTF
	// (L=left R=right B=bottom T=top N=near F=far)
	float3		corners[8];
};


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


// aabb gets culled when is lies completely on +normal dir side of palne
// returns true when culled
inline bool plane_cull_aabb (Plane const& plane, AABB3 const& aabb) {
	// test if any of the 9 points lie inside the plane => not culled
	float3 lo = aabb.lo - plane.pos;
	float3 hi = aabb.hi - plane.pos;

	if (dot(plane.normal, float3(lo.x, lo.y, lo.z)) <= 0) return false;
	if (dot(plane.normal, float3(hi.x, lo.y, lo.z)) <= 0) return false;
	if (dot(plane.normal, float3(lo.x, hi.y, lo.z)) <= 0) return false;
	if (dot(plane.normal, float3(hi.x, hi.y, lo.z)) <= 0) return false;
	if (dot(plane.normal, float3(lo.x, lo.y, hi.z)) <= 0) return false;
	if (dot(plane.normal, float3(hi.x, lo.y, hi.z)) <= 0) return false;
	if (dot(plane.normal, float3(lo.x, hi.y, hi.z)) <= 0) return false;
	if (dot(plane.normal, float3(hi.x, hi.y, hi.z)) <= 0) return false;

	return true;
}

// cull (return true) if AABB is completely outside of one of the view frustrums planes
// this cull 99% of the AABB that are invisible, but returns a false negative sometimes
inline bool frustrum_cull_aabb (View_Frustrum const& frust, float lx, float ly, float lz, float hx, float hy, float hz) {
	//// Optimized version

	// cull if outside of one plane
	for (int i=0; i<5; ++i) {
		auto& plane = frust.planes[i];

		// parts of the dot products
		float dlx = plane.normal.x * (lx - plane.pos.x);
		float dly = plane.normal.y * (ly - plane.pos.y);
		float dlz = plane.normal.z * (lz - plane.pos.z);
		float dhx = plane.normal.x * (hx - plane.pos.x);
		float dhy = plane.normal.y * (hy - plane.pos.y);
		float dhz = plane.normal.z * (hz - plane.pos.z);

	#if 0
		//if ((dlx + dly + dlz) <= 0) continue;
		//if ((dhx + dly + dlz) <= 0) continue;
		//if ((dlx + dhy + dlz) <= 0) continue;
		//if ((dhx + dhy + dlz) <= 0) continue;
		//if ((dlx + dly + dhz) <= 0) continue;
		//if ((dhx + dly + dhz) <= 0) continue;
		//if ((dlx + dhy + dhz) <= 0) continue;
		//if ((dhx + dhy + dhz) <= 0) continue;

		return true; // aabb completely outside plane
	#else
		float a = (dlx + dly + dlz);
		float b = (dhx + dly + dlz);
		float c = (dlx + dhy + dlz);
		float d = (dhx + dhy + dlz);
		float e = (dlx + dly + dhz);
		float f = (dhx + dly + dhz);
		float g = (dlx + dhy + dhz);
		float h = (dhx + dhy + dhz);

	#define MIN(a,b) kissmath::min(a,b)
		float least = MIN( MIN(MIN(a,b), MIN(c,d)), MIN(MIN(e,f), MIN(g,h)) );
	#undef MIN

		if (least > 0)
			return true;// true: aabb completely outside plane
	#endif
	}
	return false;
}

inline bool frustrum_cull_aabb (View_Frustrum const& frust, AABB3 const& aabb) {
	return frustrum_cull_aabb(frust, aabb.lo.x, aabb.lo.y, aabb.lo.z, aabb.hi.x, aabb.hi.y, aabb.hi.z);
}

#if 0
// raycast against yz aligned plane from (pos_x, 0, -height) to (pos_x, 1, 1)
inline void _minkowski_cylinder_cube__raycast_x_plane (
		float3 ray_pos, float3 ray_dir,
		float plane_x, float normal_x, float height,
		CollisionHit* hit) {
	float delta_x = plane_x - ray_pos.x;
	
	if (ray_dir.x * delta_x < 0 || ray_dir.x * normal_x >= 0) return; // ray parallel to plane or ray points away from plane or going through backside of plane

	float2 delta_yz = delta_x * (float2(ray_dir.y,ray_dir.z) / ray_dir.x);

	float2 hit_pos_yz = float2(ray_pos.y,ray_pos.z) + delta_yz;

	if (!all(hit_pos_yz > float2(0,-height) && hit_pos_yz < 1)) return;

	float hit_dist = length(float3(delta_x, delta_yz[0], delta_yz[1]));

	if (hit_dist < hit->dist) {
		hit->dist = hit_dist;
		hit->pos = float3(plane_x, hit_pos_yz[0], hit_pos_yz[1]);
		hit->normal = float3(normal_x,0,0);
	}
}
// raycast against xz aligned plane from (0, pos_y, -height) to (1, pos_y, 1)
inline void _minkowski_cylinder_cube__raycast_y_plane (
		float3 ray_pos, float3 ray_dir,
		float plane_y, float normal_y, float height,
		CollisionHit* hit) {
	float delta_y = plane_y - ray_pos.y;
	
	if (ray_dir.y * delta_y < 0 || ray_dir.y * normal_y >= 0) return; // ray parallel to plane or ray points away from plane or going through backside of plane

	float2 delta_xz = delta_y * (float2(ray_dir.x,ray_dir.z) / ray_dir.y);

	float2 hit_pos_xz = float2(ray_pos.x,ray_pos.z) + delta_xz;

	if (!all(hit_pos_xz > float2(0,-height) && hit_pos_xz < 1)) return;

	float hit_dist = length(float3(delta_xz[0], delta_y, delta_xz[1]));

	if (hit_dist < hit->dist) {
		hit->dist = hit_dist;
		hit->pos = float3(hit_pos_xz[0], plane_y, hit_pos_xz[1]);
		hit->normal = float3(0,normal_y,0);
	}
}

// raycast against cylinder side ie. cylinder with axis z and radius cyl_r from z -height to 1
//  NOTE: cylinder caps are not handled here but instead count for _minkowski_cylinder_cube__raycast_cap_plane
inline void _minkowski_cylinder_cube__raycast_cylinder_side (
		float3 ray_pos, float3 ray_dir,
		float2 cyl_pos2d, float height, float cyl_r,
		CollisionHit* hit) {
	// do 2d circle raycase using on xy plane
	float ray_dir2d_len = length((float2)ray_dir);
	if (ray_dir2d_len == 0) return; // ray parallel to cylinder
	float2 unit_ray_dir2d = (float2)ray_dir / ray_dir2d_len;

	float2 circ_rel_p = cyl_pos2d - (float2)ray_pos;

	// dist along ray to closest approach with cylinder circle 
	float closest_p_dist = dot(unit_ray_dir2d, circ_rel_p);
	float2 closest_p = unit_ray_dir2d * closest_p_dist;

	// vector from cyl circle to closest point
	float2 circ_to_closest = closest_p - circ_rel_p;

	float r_sqr = cyl_r * cyl_r;
	float dist_sqr = length_sqr(circ_to_closest);

	if (dist_sqr >= r_sqr) return; // ray does not cross cylinder

	// calc ray circle entry and exit distances (in the 2d version of the ray)
	float chord_half_length = sqrt( r_sqr - dist_sqr );
	float closest_hit_dist2d = closest_p_dist - chord_half_length;
	//float furthest_hit_dist2d = closest_p_dist + chord_half_length;
	
	float hit_dist2d;
	if (closest_hit_dist2d >= 0)		hit_dist2d = closest_hit_dist2d;
	//else if (furthest_hit_dist2d >= 0)	hit_dist2d = furthest_hit_dist2d; // hit cylinder from the inside
	else								return; // circle hit is on backwards direction of ray, ie. no hit

	// get xy delta to hit
	float2 delta_xy = hit_dist2d * unit_ray_dir2d;

	// calc hit z delta
	float delta_z = length(delta_xy) * (ray_dir.z / ray_dir2d_len);

	float3 delta = float3(delta_xy, delta_z); // relative to ray
	float3 hit_pos = ray_pos + delta; // relative to ray

	if (!(hit_pos.z > -height && hit_pos.z < 1)) return; // ray above or below cylinder (cap is not handled here, but instead in _minkowski_cylinder_cube__raycast_cap_plane)

	float dist = length(delta);

	if (dist < hit->dist) {
		hit->dist = dist;
		hit->pos = hit_pos;
		hit->normal = float3(normalize(delta_z - circ_rel_p), 0);
	}
}

// raycast against xy aligned square with rounded corners ie. (0,0) to (1,1) square + radius
inline void _minkowski_cylinder_cube__raycast_cap_plane (
		float3 ray_pos, float3 ray_dir,
		float plane_z, float normal_z, float radius,
		CollisionHit* hit) {
	// normal axis aligned plane raycast
	float delta_z = plane_z -ray_pos.z;

	if (ray_dir.z * delta_z < 0 || ray_dir.z * normal_z >= 0) return; // if ray parallel to plane or ray points away from plane or going through backside of plane

	float2 delta_xy = delta_z * (((float2)ray_dir) / ray_dir.z);

	float2 plane_hit_xy = (float2)ray_pos + delta_xy;

	// check if cylinder base/top circle cap intersects with block top/bottom square
	float2 closest_p = clamp(plane_hit_xy, 0,1);

	float dist_sqr = length_sqr(closest_p -plane_hit_xy);
	if (dist_sqr >= radius*radius) return; // hit outside of square + radius

	float hit_dist = length(float3(delta_xy, delta_z));

	if (hit_dist < hit->dist) {
		hit->dist = hit_dist;
		hit->pos = float3(plane_hit_xy, plane_z);
		hit->normal = float3(0,0, normal_z);
	}
}

// Calculate first collision CollisionHit between axis aligned unit cube and a axis aligned cylinder (cylinder axis is z)
//  offset = cylinder.pos - cube.pos  (cylinder.pos is it's center)
//  dir    = cylinder.vel - cube.vel  (does not need to be normalized)
//  cyl_r  = cylinder.radius
//  cyl_h  = cylinder.height
// coll gets written to if calculated dist < coll->dist (init coll->dist to INF intially)
inline void cylinder_cube_cast (float3 const& offset, float3 const& dir, float cyl_r, float cyl_h, CollisionHit* hit) {
	// this geometry we are raycasting onto represents the minowski sum of the cube and the cylinder

	_minkowski_cylinder_cube__raycast_cap_plane(offset, dir, 1,		   +1, cyl_r, hit); // block top
	_minkowski_cylinder_cube__raycast_cap_plane(offset, dir, -cyl_h,   -1, cyl_r, hit); // block bottom

	_minkowski_cylinder_cube__raycast_x_plane(  offset, dir,   -cyl_r, -1, cyl_h, hit); // block -X
	_minkowski_cylinder_cube__raycast_x_plane(  offset, dir, 1 +cyl_r, +1, cyl_h, hit); // block +X
	_minkowski_cylinder_cube__raycast_y_plane(  offset, dir,   -cyl_r, -1, cyl_h, hit); // block -Y
	_minkowski_cylinder_cube__raycast_y_plane(  offset, dir, 1 +cyl_r, +1, cyl_h, hit); // block +Y

	_minkowski_cylinder_cube__raycast_cylinder_side( offset, dir, float2( 0, 0), cyl_h, cyl_r, hit); // block rouned edge
	_minkowski_cylinder_cube__raycast_cylinder_side( offset, dir, float2( 0,+1), cyl_h, cyl_r, hit); // block rouned edge
	_minkowski_cylinder_cube__raycast_cylinder_side( offset, dir, float2(+1, 0), cyl_h, cyl_r, hit); // block rouned edge
	_minkowski_cylinder_cube__raycast_cylinder_side( offset, dir, float2(+1,+1), cyl_h, cyl_r, hit); // block rouned edge
}
#endif

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

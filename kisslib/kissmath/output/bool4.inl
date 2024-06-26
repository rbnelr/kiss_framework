// file was generated by kissmath.py at <TODO: add github link>

////// Inline definitions

#include "uint8v4.hpp"
#include "bool3.hpp"
#include "float4.hpp"
#include "int64v4.hpp"
#include "bool2.hpp"
#include "int4.hpp"
#include "bool4.hpp"

namespace kissmath {
	//// forward declarations
	// typedef these because the _t suffix is kinda unwieldy when using these types often
	
	typedef uint8_t uint8;
	typedef int64_t int64;
	
	// Component indexing operator
	inline constexpr bool& bool4::operator[] (int i) {
		return arr[i];
	}
	
	// Component indexing operator
	inline constexpr bool const& bool4::operator[] (int i) const {
		return arr[i];
	}
	
	
	// sets all components to one value
	// implicit constructor -> float3(x,y,z) * 5 will be turned into float3(x,y,z) * float3(5) by to compiler to be able to execute operator*(float3, float3), which is desirable
	// and short initialization like float3 a = 0; works
	inline constexpr bool4::bool4 (bool all): x{all}, y{all}, z{all}, w{all} {
		
	}
	
	// supply all components
	inline constexpr bool4::bool4 (bool x, bool y, bool z, bool w): x{x}, y{y}, z{z}, w{w} {
		
	}
	
	// extend vector
	inline constexpr bool4::bool4 (bool2 xy, bool z, bool w): x{xy.x}, y{xy.y}, z{z}, w{w} {
		
	}
	
	// extend vector
	inline constexpr bool4::bool4 (bool3 xyz, bool w): x{xyz.x}, y{xyz.y}, z{xyz.z}, w{w} {
		
	}
	
	//// Truncating cast operators
	
	
	// truncating cast operator
	inline constexpr bool4::operator bool2 () const {
		return bool2(x, y);
	}
	
	// truncating cast operator
	inline constexpr bool4::operator bool3 () const {
		return bool3(x, y, z);
	}
	
	//// Type cast operators
	
	
	// type cast operator
	inline constexpr bool4::operator float4 () const {
		return float4((float)x, (float)y, (float)z, (float)w);
	}
	
	// type cast operator
	inline constexpr bool4::operator int4 () const {
		return int4((int)x, (int)y, (int)z, (int)w);
	}
	
	// type cast operator
	inline constexpr bool4::operator int64v4 () const {
		return int64v4((int64)x, (int64)y, (int64)z, (int64)w);
	}
	
	// type cast operator
	inline constexpr bool4::operator uint8v4 () const {
		return uint8v4((uint8)x, (uint8)y, (uint8)z, (uint8)w);
	}
	
	//// reducing ops
	
	
	// all components are true
	inline constexpr bool all (bool4 v) {
		return v.x && v.y && v.z && v.w;
	}
	
	// any component is true
	inline constexpr bool any (bool4 v) {
		return v.x || v.y || v.z || v.w;
	}
	
	//// boolean ops
	
	
	inline constexpr bool4 operator! (bool4 v) {
		return bool4(!v.x, !v.y, !v.z, !v.w);
	}
	
	inline constexpr bool4 operator&& (bool4 l, bool4 r) {
		return bool4(l.x && r.x, l.y && r.y, l.z && r.z, l.w && r.w);
	}
	
	inline constexpr bool4 operator|| (bool4 l, bool4 r) {
		return bool4(l.x || r.x, l.y || r.y, l.z || r.z, l.w || r.w);
	}
	
	//// comparison ops
	
	
	// componentwise equality comparison, returns a bool vector
	inline constexpr bool4 equal (bool4 l, bool4 r) {
		return bool4(l.x == r.x, l.y == r.y, l.z == r.z, l.w == r.w);
	}
	
	// componentwise inequality comparison, returns a bool vector
	inline constexpr bool4 nequal (bool4 l, bool4 r) {
		return bool4(l.x != r.x, l.y != r.y, l.z != r.z, l.w != r.w);
	}
	
	// full equality comparison, returns true only if all components are equal
	inline constexpr bool operator== (bool4 l, bool4 r) {
		return (l.x == r.x) && (l.y == r.y) && (l.z == r.z) && (l.w == r.w);
	}
	
	// full inequality comparison, returns true if any components are inequal
	inline constexpr bool operator!= (bool4 l, bool4 r) {
		return (l.x != r.x) || (l.y != r.y) || (l.z != r.z) || (l.w != r.w);
	}
	
	// componentwise ternary (c ? l : r)
	inline constexpr bool4 select (bool4 c, bool4 l, bool4 r) {
		return bool4(c.x ? l.x : r.x, c.y ? l.y : r.y, c.z ? l.z : r.z, c.w ? l.w : r.w);
	}
}


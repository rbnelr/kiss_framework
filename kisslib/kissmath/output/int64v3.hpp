// file was generated by kissmath.py at <TODO: add github link>
#pragma once

////// Forward declarations

#include "int64.hpp"

namespace kissmath {
	//// forward declarations
	
	struct int64v2;
	struct bool3;
	struct int64v4;
	struct int3;
	struct uint8v3;
	struct float3;
	
	struct int64v3 {
		union { // Union with named members and array members to allow vector[] operator, not 100% sure that this is not undefined behavoir, but I think all compilers definitely don't screw up this use case
			struct {
				int64	x, y, z;
			};
			int64		arr[3];
		};
		
		// Component indexing operator
		inline constexpr int64& operator[] (int i);
		
		// Component indexing operator
		inline constexpr int64 const& operator[] (int i) const;
		
		
		// uninitialized constructor
		inline int64v3 () = default;
		
		// sets all components to one value
		// implicit constructor -> float3(x,y,z) * 5 will be turned into float3(x,y,z) * float3(5) by to compiler to be able to execute operator*(float3, float3), which is desirable
		// and short initialization like float3 a = 0; works
		inline constexpr int64v3 (int64 all);
		
		// supply all components
		inline constexpr int64v3 (int64 x, int64 y, int64 z);
		
		// extend vector
		inline constexpr int64v3 (int64v2 xy, int64 z);
		
		// truncate vector
		inline constexpr int64v3 (int64v4 v);
		
		
		//// Truncating cast operators
		
		// truncating cast operator
		inline constexpr explicit operator int64v2 () const;
		
		
		//// Type cast operators
		
		// type cast operator
		inline constexpr explicit operator bool3 () const;
		
		// type cast operator
		inline constexpr explicit operator float3 () const;
		
		// type cast operator
		inline constexpr explicit operator int3 () const;
		
		// type cast operator
		inline constexpr explicit operator uint8v3 () const;
		
		
		// componentwise arithmetic operator
		inline int64v3 operator+= (int64v3 r);
		
		// componentwise arithmetic operator
		inline int64v3 operator-= (int64v3 r);
		
		// componentwise arithmetic operator
		inline int64v3 operator*= (int64v3 r);
		
		// componentwise arithmetic operator
		inline int64v3 operator/= (int64v3 r);
		
	};
	
	//// arthmethic ops
	
	inline constexpr int64v3 operator+ (int64v3 v);
	
	inline constexpr int64v3 operator- (int64v3 v);
	
	inline constexpr int64v3 operator+ (int64v3 l, int64v3 r);
	
	inline constexpr int64v3 operator- (int64v3 l, int64v3 r);
	
	inline constexpr int64v3 operator* (int64v3 l, int64v3 r);
	
	inline constexpr int64v3 operator/ (int64v3 l, int64v3 r);
	
	
	//// bitwise ops
	
	inline constexpr int64v3 operator~ (int64v3 v);
	
	inline constexpr int64v3 operator& (int64v3 l, int64v3 r);
	
	inline constexpr int64v3 operator| (int64v3 l, int64v3 r);
	
	inline constexpr int64v3 operator^ (int64v3 l, int64v3 r);
	
	inline constexpr int64v3 operator<< (int64v3 l, int64v3 r);
	
	inline constexpr int64v3 operator>> (int64v3 l, int64v3 r);
	
	
	//// comparison ops
	
	// componentwise comparison returns a bool vector
	inline constexpr bool3 operator< (int64v3 l, int64v3 r);
	
	// componentwise comparison returns a bool vector
	inline constexpr bool3 operator<= (int64v3 l, int64v3 r);
	
	// componentwise comparison returns a bool vector
	inline constexpr bool3 operator> (int64v3 l, int64v3 r);
	
	// componentwise comparison returns a bool vector
	inline constexpr bool3 operator>= (int64v3 l, int64v3 r);
	
	// componentwise equality comparison, returns a bool vector
	inline constexpr bool3 equal (int64v3 l, int64v3 r);
	
	// componentwise inequality comparison, returns a bool vector
	inline constexpr bool3 nequal (int64v3 l, int64v3 r);
	
	// full equality comparison, returns true only if all components are equal
	inline constexpr bool operator== (int64v3 l, int64v3 r);
	
	// full inequality comparison, returns true if any components are inequal
	inline constexpr bool operator!= (int64v3 l, int64v3 r);
	
	// componentwise ternary (c ? l : r)
	inline constexpr int64v3 select (bool3 c, int64v3 l, int64v3 r);
	
	
	//// misc ops
	// componentwise absolute
	inline int64v3 abs (int64v3 v);
	
	// componentwise minimum
	inline constexpr int64v3 min (int64v3 l, int64v3 r);
	
	// componentwise maximum
	inline constexpr int64v3 max (int64v3 l, int64v3 r);
	
	// componentwise clamp into range [a,b]
	inline constexpr int64v3 clamp (int64v3 x, int64v3 a, int64v3 b);
	
	// componentwise clamp into range [0,1] also known as saturate in hlsl
	inline constexpr int64v3 clamp (int64v3 x);
	
	// get minimum component of vector, optionally get component index via min_index
	inline int64 min_component (int64v3 v, int* min_index=nullptr);
	
	// get maximum component of vector, optionally get component index via max_index
	inline int64 max_component (int64v3 v, int* max_index=nullptr);
	
	
	// componentwise wrap
	inline int64v3 wrap (int64v3 v, int64v3 range);
	
	// componentwise wrap
	inline int64v3 wrap (int64v3 v, int64v3 a, int64v3 b);
	
	
	
	//// Vector math
	
	// squared magnitude of vector, cheaper than length() because it avoids the sqrt(), some algorithms only need the squared magnitude
	inline constexpr int64 length_sqr (int64v3 v);
	
	// dot product
	inline constexpr int64 dot (int64v3 l, int64v3 r);
	
	// 3d cross product
	inline constexpr int64v3 cross (int64v3 l, int64v3 r);
	
}


#include "int64v3.inl"
